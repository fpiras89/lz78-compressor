/*
 * dictionary.h
 * Francesco Piras - Dario Varano
 * June 2015
 */

#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include "definitions.h"

// dictionary structure
typedef struct dictionary_struct dictionary;

// dictionary functions
/**
 * @brief It initializes the dictionary of the compressor. It uses the dictionary_lookup to insert the entries
 * 
 * @param dictionary The pointer to the dictionary
 * @return void
 */
void dictionary_compressor_init (dictionary* dictionary);

/**
 * @brief It initializes the dictionary of the decompressor. It inserts entries in a sorted manner
 * 
 * @param dictionary The pointer to the dictionary
 * @return void
 */
void dictionary_decompressor_init (dictionary* dictionary);

/**
 * @brief It allocates the dictionary's structures in dynamic memory
 * 
 * @param size the size of the dictionary entries table
 * @return dictionary_entry* The pointer to the allocated dictionary
 */
dictionary* dictionary_alloc (int size);

/**
 * @brief It deallocates the dictionary's structures which are allocated in dynamic memory
 * 
 * @param dictionary The pointer to the dictionary
 * @return void
 */
void dictionary_free (dictionary* dictionary);

/**
 * @brief It looks for a node containing "symbol", which is child of to the node having code "parent"
 * 
 * @param dictionary The pointer to the dictionary
 * @param parent the parent node where we are going to look for the child node
 * @param symbol the node's symbol which we are looking for
 * @return the index of the node for which we find a match or, instead, the index of an UNUSED node inside the dictionary
 */
uint32_t dictionary_lookup (dictionary* dictionary, CODE parent, SYMBOL symbol);

/**
 * @brief It inserts an entry in the dictionary. In case of an already used entry, the fields will be overwritten
 * 
 * @param dictionary The pointer to the dictionary
 * @param index the index where to put the new entry
 * @param parent the new parent code
 * @param code the new code
 * @param symbol the new symbol
 * @return int a flag indicating if the insert operation has been completed successfully (0) or if an error occurs (-1)
 */
int dictionary_insert (dictionary* dictionary, uint32_t index, CODE parent, CODE code, SYMBOL symbol);

/**
 * @brief It returns the code field of en entry of the dictionary
 * 
 * @param dictionary The pointer to the dictionary
 * @param index the index of the dictionary to inspect
 * @return CODE the code of the entry or -1 in case of error
 */
CODE dictionary_get_entry_code (dictionary* dictionary, uint32_t index);

/**
 * @brief It returns the parent code of an entry of the dictionary
 * 
 * @param dictionary The pointer to the dictionary
 * @param index the index of the dictionary to inspect
 * @return CODE the parent code of the entry or -2 in case of error
 */
CODE dictionary_get_entry_parent (dictionary* dictionary, uint32_t index);

/**
 * @brief It returns the symbol of an entry of the dictionary
 * 
 * @param dictionary The pointer to the dictionary
 * @param index the index of the dictionary to inspect
 * @return CODE the symbol of the entry or -1 in case of error
 */
SYMBOL dictionary_get_entry_symbol (dictionary* dictionary, uint32_t index);

/**
 * @brief It returns the dictionary size
 * 
 * @param dictionary The pointer to the dictionary
 * @return int the dictionary's size or -1 in case of error
 */
int dictionary_size (dictionary* dictionary);

/**
 * @brief It returns the number of used entries in the dictionary
 * 
 * @param dictionary The pointer to the dictionary
 * @return int the number of used entries in the dictionary or -1 in case of error
 */
int dictionary_count (dictionary* dictionary);

/**
 * @brief It returns the number of available entries in the dictionary
 * 
 * @param dictionary The pointer to the dictionary
 * @return int the number of available entries in the dictionary or -1 in case of error
 */
int dictionary_availables (dictionary* dictionary);

/**
 * @brief It returns a flag telling if an entry of the dictionary is used or not
 * 
 * @param dictionary The pointer to the dictionary
 * @param index the index of the dictionary to inspect
 * @return bool a flag which tells if the entry is used (true) or not (false)
 */
bool dictionary_is_entry_unused (dictionary* dictionary, uint32_t index);

/**
 * @brief It prints the entire dictionary content on a file
 * 
 * @param dictionary The pointer to the dictionary
 * @return void
 */
void dictionary_print (dictionary* dictionary);

#endif
