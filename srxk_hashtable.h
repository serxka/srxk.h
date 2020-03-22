/*
'* >> srxk_hashtable.h 0.1.0
* A generic C header only hash table implementation
* This is an open addressing double hashing table
*
* >> Usage
* ```
* #define HT_TYPE int
* //              ^ you can put any valid c type here
* #include <srxk_hashtable.h>
* ```
* If you want to use a pointer or struct you must first typedef it like so:
* `typedef struct object* objectp;`
*
* You can allow define if the value should be freed by defining `HT_FREEVALUE`
* the default behaviour is to not free the value
*
* Custom memory allocators are also supported via by defining CUSTOM_MALLOC, 
* _REALLOC, *_FREE
* If an error ocurrs an errno will be set relative the ht type convention
*
* There some examples in `test/` if you need a guide
*
* >> License
* Be Nice Please Public License
* Version 2, FEBUARY 2020
* 
* Copyright (C) 2020 Milo Wheeler <milowheeler@protonmail.com>
* 
* Permission is granted, free of charge, to any person obtaining a copy of this
* "Software", to the rights to, use, copy, modify, merge, publish, distribute,
* and/or sell copies of the Software without restriction, this holds true as
* long as the software is not used to intentionally hurt people, and/or
* communities emotionally or physically.
* 
* Due to the nature of Open Source, no liability or warranty of any kind is
* provided with this Software.
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// INCLUDES
#include <stdlib.h> // malloc, calloc, free
#include <string.h> // strdup, strlen
#include <math.h> // pow

// CONSTANTS
// These can be tweaked for your needs


// THE MACRO MAGIC
#ifndef HT_TYPE
	typedef char* string;
	#define HT_TYPE string
#endif 
#ifndef HT_EMPTYVALUE
	#error "HT_EMPTYVALUE must be defined"
#endif

#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)

#define type(struct,type) EVALUATOR(struct, type)
#define function(name) EVALUATOR(HT, name)

#define HT type(ht, HT_TYPE)
#define HT_ITEM EVALUATOR(HT, item)
#define HT_ERR EVALUATOR(HT, err)
#define HT_EMPTY EVALUATOR(HT, EMPTY)

// CUSTOM MEMORY MANAGER
#ifdef CUSTOM_MALLOC
	// Make sure that realloc and free are also defined
	#if !defined(CUSTOM_REALLOC) | !defined(CUSTOM_FREE)
		#error "If a custom malloc is used a custom realloc and free must also\
				be defined"
	#endif
	#define malloc CUSTOM_MALLOC
	#define realloc CUSTOM_REALLOC
	#define free CUSTOM_FREE
#endif

// ERROR CODES
#ifndef ENOMEM
	#define ENOMEM 12
#endif
#ifndef ENODATA
	#define ENODATA 61
#endif

// HASH TABLE ITEM
typedef struct HT_ITEM
{
	char *k;
	HT_TYPE v;
} HT_ITEM;

// HASH TABLE TYPE
typedef struct HT
{
	HT_ITEM **data;
	int capacity;
	int count;
} HT;

// ERROR NUMBER
static int HT_ERR = 0;

// DELETED ITEM
static HT_ITEM HT_EMPTY = {NULL, HT_EMPTYVALUE};

// HASH FUNCTIONS
// The reason I use function() is to avoid namespace collision w/ users program
static int function(gen_hash)(const char* k, const int p, const int m)
{
	long hash = 0;
	const int len = strlen(k);
	for (int i = 0; i < len; ++i)
	{
		hash += (long)pow(p, len - (i+1)) * k[i];
		hash %= m;
	}
	return (int)hash;
}
static int function(hash)(const char* k, const int num_b, const int a)
{
	int ha = function(gen_hash)(k, 151, num_b);
	int hb = function(gen_hash)(k, 163, num_b);
	return (ha + (a * (hb + 1))) & num_b;
}

// HASH TABLE ITEM FUNCTIONS
// You shouldn't be calling these for any good reason
static HT_ITEM *function(item_new)(const char *k, HT_TYPE v)
{
	// Create a new item, copy the key and value
	HT_ITEM *t = malloc(sizeof(HT_ITEM));
	t->k = strdup(k);
	t->v = v;
	return t;
}

static void function(item_free)(HT_ITEM *i)
{
	// Free key and item
	free(i->k);
	// Free value if behaviour is defined
	#ifdef HT_FREEVALUE
		free(->v);
	#endif 
	free(i);
}

// HASH TABLE FUNCTIONS
// These are functions you are meant to call
/* 
* Description:
* 	Creates a new hash table
* Parameters:
* 	None
* Return Value:
* 	Returns an empty heap allocated hash table
*/
static HT *function(new)()
{
	HT *t = malloc(sizeof(HT));
	if (t == NULL) {
		HT_ERR = ENOMEM;
		return NULL;}

	// Make sure that data is zero'd out
	t->capacity = 53;
	t->count = 0;
	t->data = calloc((size_t)t->capacity, sizeof(HT_ITEM *));
	if (t->data == NULL) {
		HT_ERR = ENOMEM;
		return NULL;}
	return t;
}

/* 
* Description:
* 	Inserts/Updates a  
* Parameters:
* 	abc
* Return Value:
* 	abc
*/
static void function(insert)(HT *ht, const char *key, const HT_TYPE value)
{
	HT_ITEM *item = function(item_new)(key, value);
	int index = function(hash)(item->k, ht->capacity, 0);
	HT_ITEM *cur = ht->data[index];
	for (int i = 0; cur != NULL && cur != &HT_EMPTY; ++i)
	{
		if (!strcmp(item->k, key))
			break;
		index = function(hash)(item->k, ht->capacity, i);
		cur = ht->data[index];
	}
	ht->data[index] = item;
	++ht->count;
}

static HT_TYPE function(search)(const HT *ht, const char *key)
{
	int index = function(hash)(key, ht->capacity, 0);
	HT_ITEM *item = ht->data[index];
	for (int i = 0; item != NULL; ++i)
	{
		if (item != &HT_EMPTY && !strcmp(item->k, key))
			return item->v;

		index = function(hash)(key, ht->capacity, i);
		item = ht->data[index];
	}

	// Not found
	HT_ERR = ENODATA;
	return HT_EMPTYVALUE;
}

static void function(delete)(HT *ht, const char *key)
{
	int index = function(hash)(key, ht->capacity, 0);
	HT_ITEM *item = ht->data[index];
	for (int i = 0; item != NULL; ++i)
	{
		if (item != &HT_EMPTY && !strcmp(item->k, key) && (i || !i))
		{
			function(item_free)(item);
			ht->data[index] = &HT_EMPTY;
			--ht->count;
			return;
		}
		index = function(hash)(key, ht->capacity, i);
		item = ht->data[index];
	}
	
}

static void function(free)(HT *ht)
{
	// Free items
	for(int i = 0; i < ht->capacity; ++i)
	{
		HT_ITEM *it = ht->data[i];
		if (it != NULL && it->k != NULL)
			function(item_free)(it);
	}

	// Free table
	free(ht->data);
	free(ht);
}

#ifdef __cplusplus
} // extern "C" closing brace
#endif	// __cplusplus