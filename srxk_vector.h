/*
* >> srxk_vector.h 0.1.2
* A generic C header only vector implementation
*
* >> Usage
* ```
* #define VECTOR_TYPE int
* //                  ^ you can put any valid c type here
* #include <srxk_vector.h>
* ```
* If you want to use a pointer or struct you must first typedef it like so:
* `typedef struct object* objectp;`
*
* Custom memory allocators are also supported via by defining VECTOR_MALLOC, _REALLOC, *_FREE
* If an error ocurrs an errno will be set relative the vec type convention
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
* "Software", to the rights to, use, copy, modify, merge, publish, distribute, and/or
* sell copies of the Software without restriction, this holds true as long as the
* software is not used to intentionally hurt people, and/or communities emotionally
* or physically.
* 
* Due to the nature of Open Source, no liability or warranty of any kind is provided
* with this Software.
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// INCLUDES
// This an optional header, you can define your own memory allocator to replace these
#include <stdlib.h> // malloc, realloc, free

// CONSTANTS
// These can be tweaked for your needs
#define VECTOR_START_LENGTH (4)
#define VECTOR_GROWTH_FACTOR (2)
#define VECTOR_GROWTH_CAP (32768)
#define VECTOR_GROWTH_CAP_GROWTH (4096)

// THE MACRO MAGIC
#ifndef VECTOR_TYPE
	#define VECTOR_TYPE int
#endif

// Thank STO for these two: stackoverflow.com/a/1489985
#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)

#define type(struct,type) EVALUATOR(struct, type)
#define function(name) EVALUATOR(VECTOR, name)

#define VECTOR type(vec, VECTOR_TYPE)
#define VECTOR_ERR EVALUATOR(VECTOR, err)

// CUSTOM MEMORY MANAGER
#ifdef CUSTOM_MALLOC
	// Make sure that realloc and free are also defined
	#if !defined(CUSTOM_REALLOC) | !defined(CUSTOM_FREE)
		#error "If a custom malloc is used a custom realloc and free must also be defined"
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

// VECTOR TYPE
typedef struct VECTOR
{
	VECTOR_TYPE *data;
	int capacity;
	int len;
} VECTOR;

// ERROR NUMBER
int VECTOR_ERR = 0;

// VECTOR FUNCTIONS
/* 
* Description:
* 	Create's a new vector
* Parameters:
* 	None
* Return Value:
* 	Return the newly heap allocated vector object
*/
static VECTOR *function(new)()
{
	// Create our vector
	VECTOR *t;
	t = (VECTOR*)malloc(sizeof(VECTOR));
	if (t == NULL) { // If failed set errno and return NULL
		VECTOR_ERR = ENOMEM;
		return NULL;}

	// Malloc the data and set limits
	t->data = (VECTOR_TYPE*)malloc(sizeof(VECTOR_TYPE) * VECTOR_START_LENGTH);
	t->capacity = VECTOR_START_LENGTH;
	t->len = 0;

	if (t->data == NULL) { // If failed set errno and return NULL
		VECTOR_ERR = ENOMEM;
		return NULL;}

	return t;
}

/* 
* Description:
* 	Adds a new item to the vector, realloc if
* 	there is not enough space
* Parameters:
* 	VECTOR *v - the vector to be operated on
	VECTOR_TYPE data - the data to be pushed onto
* Return Value:
* 	The vector operated on
*/
static VECTOR *function(push)(VECTOR *v, VECTOR_TYPE data)
{
	// See if we need more space
	if (v->len == v->capacity)
	{
		// Grow the capacitiy
		int newsize = v->capacity * VECTOR_GROWTH_FACTOR;
		// If the new size is larger then our growth cap, then just add
		if (newsize > VECTOR_GROWTH_CAP)
			v->capacity += VECTOR_GROWTH_CAP_GROWTH;
		// Other wise just our factor
		else
			v->capacity = newsize;

		// Resize our data section
		VECTOR t;
		t.data = (VECTOR_TYPE*)realloc(v->data, sizeof(VECTOR_TYPE) * v->capacity);
		if (t.data == NULL) { // If failed set errno and return NULL
			VECTOR_ERR = ENOMEM;
			return NULL;}
		else // Other wise set our updated data pointer
			v->data = t.data;
	}

	// Set the new data
	v->data[v->len++] = data;
	// Return vector for convenience
	return v;
}

/* 
* Description:
* 	Pop the last value of the vector for returning
* Parameters:
* 	VECTOR *v - the vector to be operated on
* Return Value:
* 	The pop value in the vector
*/
static VECTOR_TYPE function(pop)(VECTOR *v)
{ // TODO add shrink logic
	// What is the best way to do this?
	if(v->len == 0)
	{
		VECTOR_ERR = ENODATA;
		return v->data[0];
	} else
		return v->data[--v->len];
}

/* 
* Description:
* 	Get the last item in the vector
* Parameters:
* 	VECTOR *v - the vector to be operated on
* Return Value:
* 	Returns the last item
*/
static VECTOR_TYPE function(last)(const VECTOR *v)
{
	if(v->len == 0)
	{
		VECTOR_ERR = ENODATA;
		return v->data[0];
	} else
		return v->data[v->len - 1];
}

/* 
* Description:
* 	Frees a vectors data and its self
* Parameters:
* 	VECTOR *v - the vector to be operated on
* Return Value:
* 	None
*/
static void function(free)(VECTOR *v)
{
	free(v->data);
	free(v);
}

// Undefine the macros to keep things clean
#undef VECTOR
#undef VECTOR_TYPE
#undef PASTER
#undef EVALUATOR
#undef function
#undef type

#ifdef __cplusplus
} // extern "C" closing brace
#endif	// __cplusplus