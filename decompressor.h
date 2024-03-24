/*
 * decompressor.h
 * Francesco Piras - Dario Varano
 * June 2015
 */

#ifndef _DECOMPRESSOR_H
#define _DECOMPRESSOR_H

/**
 * @brief It performs the decompression of the input file, by producing the output file
 * 
 * @param input the input file name
 * @param output the output file name
 * @return int a flag indicating if the decompression operation has been completed successfully (0) or if an error occurs (-1)
 */
int decompress (char* input, char* output, int bits, int dict_size);


#endif
