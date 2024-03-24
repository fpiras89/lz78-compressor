/*
 * definitions.h
 * Francesco Piras - Dario Varano
 * June 2015
 */

#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#define EOS 		256 				// end of stream symbol
#define SYMBOLS		257					// number of symbols
#define UNUSED 		-1					// code that represents an unused dictionary entry

#define FIRST_CODE 	257					// first code to use as next code
#define ROOT_CODE	-1					// root node code

#ifndef bool
typedef enum { false = 0, true = 1 } bool;
#endif

typedef uint16_t SYMBOL;
typedef uint32_t CODE;

// Performance analysis variables
int COLLISIONS;
int LOOKUP_COUNT;

#endif
