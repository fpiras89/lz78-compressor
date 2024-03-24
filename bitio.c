/*
 * bitio.c
 * Francesco Piras - Dario Varano
 * June 2015
 */

#include "bitio.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <endian.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 16

typedef struct bit_file {
	int fd;						// the file descriptor
	bool reading;				// flag indicating reading mode (writing if false)
	int next;					// next buffer bit
	int end;					// end buffer bit
	int size;					// number of buffer's bits
	uint64_t buf[BUF_SIZE];		// the buffer
} BIT_FILE;

/**
 * @brief It poors the bit file buffer by writing the remaining data
 *
 * @param bf the pointer to the data structure where the data will be written
 * @return int a flag indicating if the flush operation has been completed successfully (0) or if an error occurs (-1)
 */
int bit_flush (BIT_FILE* bf);

BIT_FILE* bit_open(char* name, char* mode)
{
	BIT_FILE* bf;
	int size;

	// check that parameters are correct
	if (name == NULL)
		return NULL;

	if (mode == NULL)
		return NULL;

	// check that the mode specified as parameter is acceptable (only "r" and "w" permitted)
	if ((strcmp(mode, "r") != 0) && (strcmp(mode, "w") != 0))
		return NULL;
	
	// allocation of the bit file data structure and set all bytes to 0
	bf = calloc(1, sizeof(BIT_FILE));

	// check that the allocation succeed
	if (bf == NULL)
		return NULL;

	// opening the file in the specified mode and check that the operation succeed
	bf->fd = open(name, (mode[0] == 'r') ? (O_RDONLY) : (O_WRONLY | O_CREAT | O_TRUNC), S_IRWXU);
	if (bf->fd < 0)
		return NULL;
	
	// check that the allocation succeed
	if (bf == NULL)
		return NULL;

	// setting the reading flag
	bf->reading = (mode[0] == 'r')?(true):(false);

	// setting the size (in bits) of the buffer
	size = sizeof(bf->buf) * 8;

	// if reading, the parameter "end" is set to 0, because the buffer is empty right now
	if (bf->reading == true) {
		bf->next = 0;
		bf->end = 0;
		bf->size = size;
	}
	// if writing, the parameter "end" is set to the size of the buffer
	else {
		bf->next = 0;
		bf->end = size;
		bf->size = size;
	}

	return bf;
}

int bit_close (BIT_FILE* bf)
{
	int result;

	if (bf == NULL)
		return -1;

	// if writing, we need to poor the buffer by performing a bit_flush, in order to write the entire buffer into the file
	if (bf->reading == false) {
		result = bit_flush(bf);
		if (result < 0)
			goto error;
	}

	// effectively closing the bit file and check that the operation succeed
	result = close(bf->fd);
	if (result < 0)
		goto error;

	// deallocation of the dynamic memory
	free(bf);
	
	return 0;
	
error:
	free(bf);
	return -1;
}

int bit_read(BIT_FILE* bf, uint64_t* data, int len)
{
	int remaining_row_bits1, remaining_row_bits2, result, offset, available_bits;
	uint64_t* buffer_row;
	uint64_t tmp1, tmp2;
	
	// flag used to handle the case of multiple read
	bool repeat_flag;

	repeat_flag = false;
	
	// Checking parameters
	if (bf == NULL || len < 1 || len > (8* sizeof(data)))
			return -1;

	// check that the we are in the case of a reading operation
	if (bf->reading == false)
		return -1;

	while (len > 0) {

		// computing the number of bits still in the buffer
		available_bits = (bf->end - bf->next);

		// if there are no available bits to be read
		if (available_bits == 0 || bf->next == 0) {

			// try to fill the buffer
			// reading operation and check that succeed
			result = (int)read(bf->fd, bf->buf, sizeof(bf->buf));
			if (result < 0)
				return -1;

			// updating values of the data structure
 			bf->next = 0;
			bf->end = (result * 8);
		}

		// buffer_row is the pointer to the row of the buffer where the bit from which to start reading is placed
		buffer_row = bf->buf + (bf->next / 64);
		
		// is the offset of the bit inside the buffer row
		offset = bf->next % 64;
		
		// the remaining bits of the buffer row to read
		remaining_row_bits1 = 64 - offset;

		// check that we read at most len bits
		if (len < remaining_row_bits1)
			remaining_row_bits1 = len;

		// convert the buffer row from little endian to host format
		tmp1 = le64toh(*buffer_row);
		
		// shift right the buffer row of offset bits
		tmp1 >>= offset;

		// apply a mask to read the remaining bits of the data
		tmp1 &= (((uint64_t)1 << len)-1);

		// Update the data structure
		bf->next = (bf->next + remaining_row_bits1) % bf->end;
		len -= remaining_row_bits1;

		// handle the case that the datum is placed on multiple rows of the buffer
		if (len > 0) {
			repeat_flag = true;
			tmp2 = tmp1;
			remaining_row_bits2 = remaining_row_bits1;
		}

	}

	// if we are in the case of a repetition in the reading operation
	// we need to shift the new data and concatenate the old ones
	if (repeat_flag == true){
		tmp1 = tmp1 << remaining_row_bits2;
		tmp1 = (tmp1|tmp2);
	}

	// convert back from host to little endian format
	tmp1 = htole64(tmp1);

	// assign the read values to data
	*data = tmp1;

	// check if the read data is EOS
	if (*data == EOS)
		return 2;

	return 0;
}

int bit_write(BIT_FILE* bf, uint64_t* data, int len)
{
	//static int count = 0;
	//bit_print_data(data, "0x%03x", &count);

	int space, offset, result, remaining_row_bits;

	uint64_t* buffer_row;
	uint64_t tmp;

	// checking parameters
	if (bf == NULL || len < 1 || len > (8* sizeof(*data)))
		return -1;

	// check that we are in the case of a writing operation
	if (bf->reading == true)
		return -1;

	while (len > 0) {
		
		// computing the available space in the buffer
		space = bf->end - bf->next;
		if (space < 0) {
			printf("Error here!\n");
			return -1;
		}

		// if buffer is full, flush data to file and reinit BIT_IO struct
		if (space == 0) {
			result = bit_flush(bf);
			if (result < 0)
				return -1;
		}
		
		// which buffer row
		buffer_row = bf->buf + (bf->next/64);
		
		// offset inside the row
		offset = bf->next % 64;
		
		// how many bits
		remaining_row_bits = 64 - offset;
		
		// put at most len bits into the buffer
		if (len < remaining_row_bits)
			remaining_row_bits = len;

		// convert from little endian to host format
		tmp = le64toh(*buffer_row);
		
		// shift and concatenate data by offset bits
		tmp |= ((*data) << offset);
		
		// convert back from host to little endian format
		*buffer_row = htole64(tmp);

		// update the bit file structure
		bf->next += remaining_row_bits;
		
		// compute how many bits still to write
		len -= remaining_row_bits;
		
		// shift right data of remaining_row_bits
		*data >>= remaining_row_bits;
		
	}

	return 0;
}

int bit_flush(BIT_FILE* bf)
{
	int size, align, result, i;

	// Checking parameters
	if (bf == NULL)
			return -1;

	// chack that we are in write mode
	if (bf->reading == true)
		return -1;

	// align to byte
	align = ((bf->next % 8) > 0)?(1):(0);
	
	// data size in byte
	size = (bf->next / 8) + align;

	// write to file the remaining data and check that the operation succeed
	result = (int)write(bf->fd, bf->buf, size);
	if (result < 0)
		return -1;

	// update the value of the data structure
	bf->next = 0;
	bf->end = bf->size;
	
	// set the buffer to 0
	for (i = 0; i < (bf->size / 64); i++)
		bf->buf[i] = 0;

	return 0;
}

void bit_print_data(uint64_t data, char* format, int* count)
{
	printf(format, data);
	(*count)++;
	if ((*count) < 16) {
		printf(" ");
	}
	else {
		printf("\n");
		(*count) = 0;
	}
}

void bit_print(char* name, char* format, int len)
{
	BIT_FILE* bf;
	uint64_t data;
	int count, res;

	// opening the bit file in reading mode and check that the operation succeed
	bf = bit_open(name, "r");
	if (bf == NULL) {
		printf("Ops: error during opening\n");
	}
	
	res = 0;
	count = 0;
	
	// while EOS is reached (2 is an internal code for EOS)
	while ((res = bit_read(bf, &data, len)) != 2 ) {
		
		// check that no errors occur
		if (res < 0) {
			printf("bit_print: error read\n");
			break;
		}
		
		// prints data in the specified format and update the counter
		bit_print_data(data, format, &count);
	}
		// print the remaining data
	if (res >= 0) {
		printf(format, data);
		printf("\n");
	}
	
	// closing the bit file structure
	if (bit_close(bf) == -1) {
		printf("bit_print: error during closing\n");
	}
}
