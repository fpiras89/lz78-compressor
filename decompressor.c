/*
 * decompressor.c
 * Francesco Piras - Dario Varano
 * June 2015
 */

#include "decompressor.h"
#include "bitio.h"
#include "dictionary.h"


/**
 * @brief Move up in the tree starting from code node to root node, storing the encountered symbols
 *
 * @param stack the pointer to the symbols' stack
 * @param stack_index the index of the stack from which it starts storing symbols
 * @param code the code of the node from which we start to move up
 * @return int size of the stack
 */
int decode_string (dictionary* dictionary, SYMBOL* stack, int stack_index, CODE code);

/**
 * @brief It actually performs the decompression
 *
 * @param input the pointer to the data structure of the input bit file
 * @param output the pointer to the data structure of the output file
 * @param bits the number of bits used for encoding
 * @param tab_size the size of the dictionary
 * @return int a flag indicating if the decompression operation has been completed successfully (0) or if an error occurs (-1)
 */
int decompressor_impl (BIT_FILE* input, FILE* output, int bits, int dict_size);

int decompress (char* input, char* output, int bits, int dict_size)
{
	FILE* fd;
	BIT_FILE* bf;
	
	// opening the input file in writing mode
	fd = fopen(output,"w");
	
	// opening the output file in reading mode
	bf = bit_open(input,"r");
	
	// checking if the opening operations succeed
	if (fd && bf != NULL)
		return decompressor_impl(bf, fd, bits, dict_size);
	
	return -1;
}

int decode_string (dictionary* dictionary, SYMBOL* stack, int stack_index, CODE code)
{
	while(code != ROOT_CODE) {
		stack[stack_index++] = dictionary_get_entry_symbol(dictionary, code);
		code = dictionary_get_entry_parent(dictionary, code);
	}
	return stack_index;
}

int decompressor_impl (BIT_FILE* input, FILE* output, int bits, int dict_size)
{
	
	CODE old_code, next_code, new_code;
	SYMBOL character;
	uint64_t data;
	uint16_t max_code;
	int res, count, dictionary_counter;
	dictionary* dictionary;
	
	SYMBOL symbols_stack[dict_size];
	
	// dictionary allocation
	dictionary = dictionary_alloc(dict_size);
	if (dictionary == NULL)
		return -1;
	
	// initialization of the decompressor's dictionary
	dictionary_decompressor_init(dictionary);
	next_code = FIRST_CODE;
	count = 0;
	
	// reading the first code
	res = bit_read(input, &data, bits);
	if (res < 0) {
		goto error;
	}
	
	// checking that the first code read is EOS
	old_code = (CODE)data;
	if (old_code == EOS) {
		goto end;
	}
	// computing the values for the maximum rapresentable code
	max_code = (1 << bits) - 1;
	
	character = (SYMBOL)old_code;
	
	// writing the first code to the output file
	res = fputc(old_code, output);
	if (res < 0) {
		goto error;
	}
	
	// read untill EOS is reached (2 is an internal code for EOS)
	while (bit_read(input, &data, bits) != 2) {
		new_code = (CODE)data;
		
		// regular case:
		// checking that the node labeled with the next code is still in the dicitonary
		if (new_code < next_code) {
			
			// build the symbols' stack, to be write in the output file
			count = decode_string(dictionary, symbols_stack, 0, new_code);
		}
		// special case:
		// the node labeled with the next code is not still in the dictionary
		else {
			
			// initialization of the stack
			symbols_stack[0] = (SYMBOL)character;
			
			// in this case we start from the position 1 of the stack, 
			// because position 0 was previously initialized
			count = decode_string(dictionary, symbols_stack, 1, old_code);
		}
		
		// child of the root
		character = symbols_stack[count -1];
		
		while (count > 0) {
			
			// writing of the symbols contained in the stack, in the output file
			res = fputc(symbols_stack[--count], output);
			if (res < 0) {
				goto error;
			}
		}
		
		// adding a new entry in the dictionary
		res = dictionary_insert(dictionary, (uint32_t)next_code, old_code, next_code, character);
		
		next_code++;
		
		// getting the number of used entries in the dictionary
		dictionary_counter = dictionary_count(dictionary);
		
		//check if next_code has reached max admissible value
		if ((next_code > max_code) || (dictionary_counter > dict_size/2)) {

			next_code = FIRST_CODE;
			
			// reinit the dictionary
			dictionary_decompressor_init(dictionary);
		}
		
		old_code = new_code;
	}
	
	// closing the bit file
	if (bit_close(input) < 0) {
		printf("decompress2: error during closing\n");
		goto error;
	}
	
end:	
	dictionary_free(dictionary);
	return 0;
	
error:	
	dictionary_free(dictionary);
	return -1;
}

