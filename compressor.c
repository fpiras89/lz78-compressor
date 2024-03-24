/*
 * compressor.c
 * Francesco Piras - Dario Varano
 * June 2015
 */

#include "compressor.h"
#include "dictionary.h"
#include "bitio.h"


/**
 * @brief It actually performs the compression
 *
 * @param input the pointer to the data structure of the input file
 * @param output the pointer to the data structure of the output bit file
 * @return int a flag indicating if the compression operation has been completed successfully (0) or if an error occurs (-1)
 */
int compressor_impl (FILE* input, BIT_FILE* output, int bits, int dict_size);

/**
 * @brief It performs closing operations for the compressor, in particular it writes EOS and closes the bit file
 *
 * @param dictionary the pointer to the dictionary
 * @param output the pointer to the data structure of the output file
 * @return int a flag indicating if the closing operation has been completed successfully (0) or if an error occurs (-1)
 */
int compressor_close (dictionary* dictionary, BIT_FILE* output, int bits);

int compress(char* input, char* output, int bits, int dict_size) {
	FILE* fd;
	BIT_FILE* bf;
	
	// opening the input file in reading mode
	fd = fopen(input,"r");
	
	//opening the output bit file in writing mode
	bf = bit_open(output,"w");
	
	// checking if the opening operations succeed
	if ((fd != NULL) && (bf != NULL)) {
		return compressor_impl(fd, bf, bits, dict_size);
	}
	return -1;
}

int compressor_impl(FILE* input, BIT_FILE* output, int bits, int dict_size) {
	CODE next_code;			// next node
	CODE current_code;		// current node
	CODE index;				// node of the found character
	int character;
	int ret, dictionary_counter;
	uint64_t data;
	uint16_t max_code;
	dictionary* dictionary;
	
	// computing the values for the maximum rapresentable code
	max_code = (1 << bits)-1;
	
	next_code = FIRST_CODE;
	
	// dictionary allocation
	dictionary = dictionary_alloc(dict_size);
	if (dictionary == NULL)
		return -1;
	
	// initialization of the compressor's dictionary
	dictionary_compressor_init(dictionary);
	
	// checking if the file is empty
	if ((current_code = getc(input)) == EOF)
		goto end;
	
	while ((character = getc(input)) != EOF) {
		
		// performing a look up of the extracted character
		index  = dictionary_lookup(dictionary, current_code, (SYMBOL)character);
		
		// if it's already in the dictionary, set current code as the code of the entry found using the dictionary_lookup
		if (dictionary_is_entry_unused(dictionary, index) == false) {
			current_code = dictionary_get_entry_code(dictionary, index);
		}
		// the code is not in the dictionary
		else {
			// emit code
			data = (uint64_t)current_code;
			ret = bit_write(output, &data, bits);
			if( ret < 0) {
				goto error;
			}
			
			// init dictonary entry
			ret = dictionary_insert(dictionary, index, current_code, next_code, (SYMBOL)character);
			
			next_code++;
			
			// set current_code as code of node child of the root with symbol == character
			current_code = (CODE)character;
			
			// getting the number of used entries in the dictionary
			dictionary_counter = dictionary_count(dictionary);
			
			// check if next_code has reached max admissible value, or half of the table is filled (optimization)
			if ((next_code > max_code) || (dictionary_counter > dict_size/2)) {
				// reinit the dictionary
				dictionary_compressor_init(dictionary);
				
				next_code = FIRST_CODE;
			}
		}
	}
	
	// writing the last code extracted
	data = (uint64_t)current_code;
	ret = bit_write (output, &data, bits);
	if (ret < 0) {
		goto error;
	}
	
end:
	return compressor_close (dictionary, output, bits);
	
error:
	dictionary_free(dictionary);
	return -1;
}

int compressor_close (dictionary* dictionary, BIT_FILE* output, int bits) {
	
	int ret;
	uint64_t eos_code;
	
	eos_code = EOS;
	// writing EOS
	ret = bit_write(output, &eos_code, bits);
	if (ret < 0) {
		goto error;
	}
	
	// closing the bit file
	if (bit_close(output) < 0) {
		printf("Ops: error during closing\n");
		goto error;
	}
	
	// deallocation of the data structure
	dictionary_free(dictionary);
	
	return 0;
	
error:	
	dictionary_free(dictionary);
	return -1;
}
