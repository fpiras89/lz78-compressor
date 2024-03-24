/*
 * dictionary.c
 * Francesco Piras - Dario Varano
 * June 2015
 */

#include "dictionary.h"

/**
 * @brief A dictionary entry
 * 
 */
typedef struct dictionary_entry_struct {
	CODE parent;
	CODE code;
	SYMBOL symbol;
} dictionary_entry;

/**
 * @brief The dictionary structure
 * 
 */
typedef struct dictionary_struct {
	int size;						// number of total entries
	int counter;					// number of used entries
	dictionary_entry* entries;		// entries array pointer
} DICTIONARY;


/**
 * @brief An implementation of the modified Bernstein hash function
 * 
 * @param key The key used for the hashing
 * @param len The length of the key
 * @return uint32_t An index inside the dictionary
 */
uint32_t hash (void* key, int len);

void dictionary_compressor_init (DICTIONARY* dictionary)
{
	// mark all dictionary entries as UNUSED
	int i;
	for (i = 0; i < dictionary->size; i++)
		dictionary->entries[i].code = UNUSED;
	
	dictionary->counter = 0;
	
	// load first 256 symbols into the dictionary
	SYMBOL s;
	for (s = 0; s < SYMBOLS; s++) {
		uint32_t index;
		index = dictionary_lookup(dictionary, ROOT_CODE, s);
		dictionary_insert(dictionary, index, ROOT_CODE, (CODE)s, (SYMBOL)s);
	}
}

void dictionary_decompressor_init (DICTIONARY* dictionary)
{
	// mark all dictionary entries as UNUSED
	int i;
	for (i = 0; i < dictionary->size; i++)
		dictionary->entries[i].code = UNUSED;
	
	dictionary->counter = 0;
	
	// load first 256 symbols into the dictionary
	SYMBOL s;
	for (s = 0; s < SYMBOLS; s++) {
		dictionary_insert(dictionary, s, ROOT_CODE, (CODE)s, (SYMBOL)s);
	}
}

DICTIONARY* dictionary_alloc (int size)
{	
	DICTIONARY* dictionary;
	
	dictionary = malloc(sizeof(DICTIONARY));
	if(dictionary != NULL) {
		dictionary->size = size;
		dictionary->counter = 0;
		dictionary->entries = malloc(size * sizeof(dictionary_entry));
	}
	
	return dictionary;
}

void dictionary_free (DICTIONARY* dictionary)
{	
	if (dictionary != NULL) {
		if (dictionary->entries != NULL) {
			free(dictionary->entries);
			dictionary->entries = NULL;
		}
		free(dictionary);
	}
	dictionary = NULL;
}

uint32_t dictionary_lookup (DICTIONARY* dictionary, CODE parent, SYMBOL symbol)
{
	CODE key;
	uint32_t mask, index, symbol32;
	
	LOOKUP_COUNT++;
	
	// 0x0000FFFF
	mask = (1 << (sizeof(symbol) * 8))-1;
	
	// 0xXXXX0000 shit symbol left by 16 bits
	symbol32 = (symbol << (sizeof(symbol)*8));
	
	// concatenation of symbol and parent --> symbol | parent
	key = (symbol32 | (mask & parent));
	
	// get index using hash function
	index = hash((void*)&key, sizeof(key)) % dictionary->size;
	
	while (1) {
		
		// check if the current entry is unused
		if (dictionary->entries[index].code == UNUSED)
			break;
		
		// check if the current entry matches parent and symbol
		if (dictionary->entries[index].parent == parent && dictionary->entries[index].symbol == symbol)
			break;
		
		// collision occurs -> linear search jumping by 1
		index = (index + 1) % dictionary->size;
		
		COLLISIONS++;
	}
	
	return index;
}

uint32_t hash (void* key, int len)
{
	unsigned char* p;
	uint32_t h;
	int i;
	
	p = key;
	h = 5381;
	
	for (i = 0; i < len; i++) {
		h = ((h << 5) + h) ^ p[i];	// h = h*33^p[i]
	}
	
	return h;
}

int dictionary_insert(DICTIONARY* dictionary, uint32_t index, CODE parent, CODE code, SYMBOL symbol)
{
	if (dictionary == NULL)
		return -1;
	
	dictionary->entries[index].parent = parent;
	dictionary->entries[index].code = code;
	dictionary->entries[index].symbol = symbol;
	dictionary->counter++;
	
	return 0;
}

CODE dictionary_get_entry_code(DICTIONARY* dictionary, uint32_t index)
{
	if (dictionary == NULL)
		return -1;
	
	return dictionary->entries[index].code;
}

CODE dictionary_get_entry_parent(DICTIONARY* dictionary, uint32_t index)
{
	if (dictionary == NULL)
		return -2;
	
	return dictionary->entries[index].parent;
}

SYMBOL dictionary_get_entry_symbol(DICTIONARY* dictionary, uint32_t index)
{
	if (dictionary == NULL)
		return -1;
	
	return dictionary->entries[index].symbol;
}

int dictionary_size(DICTIONARY* dictionary)
{
	if (dictionary == NULL)
		return -1;
	
	return dictionary->size;
}

int dictionary_count(DICTIONARY* dictionary)
{
	if (dictionary == NULL)
		return -1;
	
	return dictionary->counter;
}

int dictionary_availables(DICTIONARY* dictionary)
{
	if (dictionary == NULL)
		return -1;
	
	return (dictionary->size - dictionary->counter);
}

bool dictionary_is_entry_unused(DICTIONARY* dictionary, uint32_t index)
{
	if (dictionary != NULL) {
		if (dictionary->entries[index].code == UNUSED)
			return true;
	}
	
	return false;
}

void dictionary_print (DICTIONARY* dictionary)
{
	int i;
	FILE* fp;
	
	fp = fopen("./dictionary.txt","w");
	if (fp == NULL) {
		printf("Error opening file\n");
		return;
	}
	
	fprintf(fp,"ENTRY\tCODE\tPARENT\tSYMBOL\n");
	
	for(i = 0; i < dictionary->size; i++) {
		fprintf(fp,"%d\t%d\t%d\t0x%02X\n", i,
				dictionary->entries[i].code,
		  dictionary->entries[i].parent,
		  dictionary->entries[i].symbol);
	}
	
	if (fclose(fp) != 0)
		printf("Error closing file\n");
}
