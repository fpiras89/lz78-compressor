#  README
#  Francesco Piras - Dario Varano
#  June 2015

This program implements an LZ78 compressor and decompressor.

COMMAND: lz78

OPTIONS:

	-b [N] number of bits used for encoding the symbols

	-c compression mode

	-d decompression mode

	-i [input_file] the input file

	-o [output_file] the output file

	-s [N] the dictionary size


DEFAULT BEHAVIOR:

	if -b is not specified, a default value will be used (12 bits);
	
	if both -c and -d options are used, or no one of them, the default behavior is the compression mode
	
	if the output file is not specified, a default name will be used;
	
	if the dictionary size is not specified, a default value will be computed from the number of bits 
		used for encoding symbols (2^bits).
		
