/*
 * main.c
 * Francesco Piras - Dario Varano
 * June 2015
 */

#include <time.h>
#include <unistd.h>
#include "definitions.h"
#include "compressor.h"
#include "decompressor.h"

int main(int argc, char** argv)
{
	int arg;
	char* input;
	char* output;
	bool compression_flag;
	
	int ret;
	uint32_t dict_size, bits;
	clock_t start, end;
	double diff;
	
	/* TO DELETE */
	COLLISIONS = 0;
	LOOKUP_COUNT = 0;


	// checking the number of the argument (they have to be at least 2)
	if (argc < 2) {
		fprintf(stderr, "Too few arguments specified\n");
		return -1;
	}
	
	// set compression as default operation
	compression_flag = true;

	// bits init
	bits = 0;
	// table size init
	dict_size = 0;

	// initialization of input and output filename
	input = output = NULL;
	
	// analyzing the arguments
	while ((arg = getopt(argc, argv, "b:cdi:o:s:")) != -1) {
		switch (arg) {
			
			// number of bits used for encoding
			case 'b':
				if (optarg != NULL) {
					long int bits_tmp;
					
					// casting string to int
					bits_tmp = strtol(optarg, NULL, 10);
					if (bits_tmp == 0L) {
						fprintf(stderr, "Bad bits number\n");
						return -1;
					}
					bits = bits_tmp;
				}
				break;
				
			// dictionary size
			case 's':
				if (optarg != NULL){
					long int dict_size_tmp;
					dict_size_tmp = strtol(optarg, NULL, 10);
					if (dict_size_tmp == 0L || dict_size_tmp > UINT32_MAX) {
						fprintf(stderr, "Bad dictionary size\n");
						return -1;
					}
					dict_size = dict_size_tmp;
				}
				break;
				
			// case of compression
			case 'c':
				compression_flag = true;
				break;
				
			// case of decompression
			case 'd':
				compression_flag = false;
				break;
			
			// input file 
			case 'i':
				input = optarg;
				break;
				
			// output file
			case 'o':
				output = optarg;
				break;

			default:
				break;
		}
	}
	
	// check that if no input file specified, if it's the case an error occurs
	if (input == NULL) {
		fprintf(stderr, "Missing input file\n");
		return -1;
	}
	
	// check that if no output file specified, if it's the case a default name is assigned
	if (output == NULL) {
		fprintf(stdout, "Missing output file. Default file will be used\n");
		if (compression_flag == true) {
			output = "./compressed";
		}
		else {
			output = "./decompressed";
		}
	}
	
	// if not '-b' nor the number of bits are specified, we use a default value
	if (bits == 0) {
		fprintf(stdout, "Missing bits number. Default value (12) will be used\n");
		bits = 12;
	}
	// check that 9 <= BITS <= 15
	else if (bits > 15) {
		fprintf(stderr, "Too much bits specified. Max bits number is 15\n");
		return -1;
	}
	else if (bits < 9) {
		fprintf(stderr, "Too few bits specified. Min bits number is 9\n");
		return -1;
	}
	
	// checking table size
	if (dict_size == 0){
		dict_size = (1 << bits);
		fprintf(stdout, "Missing dictionary size. Default value (%d) will be used\n", dict_size);
	}
	
	// case of compression
	if (compression_flag == true) {
		printf ("Starting compression: symbols %d bits - dictionary size %d bytes\n", bits, dict_size);
		start = clock();
		ret = compress (input, output, bits, dict_size);
		end = clock();
		
		// computation time
		diff = ((double)(end-start)/CLOCKS_PER_SEC);
		if	(ret == -1)
			printf ("Ops: error during compression\n");
		else
			printf ("Compressed in %f s\n", diff);

		printf ("\nTotal collisions %d\nTotal Lookup %d\nAverage collisions %f\n", COLLISIONS, LOOKUP_COUNT, ((double)COLLISIONS/(double)LOOKUP_COUNT));
	}
	// case of decompression
	else {
		printf ("Starting decompression: %d bits symbols - %d bytes dictionary size\n", bits, dict_size);
		start = clock();
		ret = decompress (input, output, bits, dict_size);
		end = clock();
		
		// computation time
		diff = ((double)(end-start)/CLOCKS_PER_SEC);
		if	(ret == -1)
			printf ("Ops: error during decompression\n");
		else
			printf ("Decompressed in %f s\n", diff);
	}
	
	return ret;
}
