/*
 * bitio.h
 * Francesco Piras - Dario Varano
 * June 2015
 */

#ifndef _BITIO_H
#define _BITIO_H

#include "definitions.h"

/**
 * NOTE ON REPRESENTATION OF DATA
 * 
 * The little endian format is used, so the most significant byte is at higher memory address.
 * 		The most significant bit inside a byte is placed on the left
 * 		Example: 
 * 		
 * 		The WORD 0xFEDCBA9876543210 will be represented as:
 * 	
 *												bits									   value
 * 
 * 						7		6		5		4		3		2		1		0
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 * 				1	|	0	|	0	|	0	|	1	|	0	|	0	|	0	|	0	|		0x10
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 * 				2	|	0	|	0	|	1	|	1	|	0	|	0	|	1	|	0	|		0x32
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 * 				3	|	0	|	1	|	0	|	1	|	0	|	1	|	0	|	0	|		0x54
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 * 		bytes	4	|	0	|	1	|	1	|	1	|	0	|	1	|	1	|	0	|		0x76
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 * 				5	|	1	|	0	|	0	|	1	|	1	|	0	|	0	|	0	|		0x98
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 * 				6	|	1	|	0	|	1	|	1	|	1	|	0	|	1	|	0	|		0xBA
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 * 				7	|	1	|	1	|	0	|	1	|	1	|	1	|	0	|	0	|		0xDC
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 * 				8	|	1	|	1	|	1	|	1	|	1	|	1	|	1	|	0	|		0xFE
 * 					+-------+-------+-------+-------+-------+-------+-------+-------+
 */

/**
 * @brief a descriptor for a bit file
 * 
 */
typedef struct bit_file BIT_FILE;

/**
 * @brief It opens the file using the specified mode
 * 
 * @param name name of the file
 * @param mode the desired opening mode, which can be "r" (read) or "w" (write)
 * @return BIT_FILE* pointer to the structure allocated in the dynamic memory, or NULL if an error occurs
 */
BIT_FILE* bit_open (char* name, char* mode);

/**
 * @brief It reads data from a bit file. 
 * 
 * @param bf the pointer to the structure to be read
 * @param data the pointer to the data where the read data will be placed
 * @param len the size (in bits) of the data to be read (at most 64 bits)
 * @return int a flag indicating if the read operation has been completed successfully (0) or if an error occurs (-1)
 */
int bit_read (BIT_FILE* bf, uint64_t* data, int len);

/**
 * @brief It writes data on a bit file
 * 
 * @param bf the pointer to the structure where the data will be written
 * @param data the the data to be written
 * @param len the size (in bits) of the data to be written (at most 64 bits)
 * @return int a flag indicating if the write operation has been completed successfully (0), if an error occurs (-1) or if EOS is reached (2)
 */
int bit_write (BIT_FILE* bf, uint64_t* data, int len);

/**
 * @brief It closes the bit file
 * 
 * @param bf the pointer to the data structure which will be closed
 * @return int a flag indicating if the close operation has been completed successfully (0) or if an error occurs (-1)
 */
int bit_close (BIT_FILE* bf);

/**
 * @brief print the content of a bit file to the standard output
 * 
 * @param name bit file name
 * @param format output format
 * @param len the size (in bits) of the data to be read (at most 64 bits)
 * @return void
 */
void bit_print(char* name, char* format, int len);


#endif
