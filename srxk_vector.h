/*
* >> srxk_vector.h V0.1.1
* A generic C header only vector implementation
*
* >> Usage
* ```
* #define VECTOR_TYPE int
* //                    ^ you can put any valid c type here
* #include <srxk_vector.h>
* ```
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

// TODO: switch away from assert to proper error handling

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// INCLUDES
#include <stdlib.h> // malloc, alloc, size_t
#include <assert.h> // assert

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

// VECTOR TYPE
typedef struct VECTOR
{
	VECTOR_TYPE *data;
	int capacity;
	int len;
} VECTOR;

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
	VECTOR *t;
	t = (VECTOR*)malloc(sizeof(VECTOR));
	assert(t != NULL);

	t->data = (void*)malloc(sizeof(VECTOR_TYPE) * VECTOR_START_LENGTH);
	t->capacity = VECTOR_START_LENGTH;
	t->len = 0;
	assert(t->data != NULL);

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
		size_t newsize = v->capacity * VECTOR_GROWTH_FACTOR;
		// If the new size is larger then our growth cap, then just add
		if (newsize > VECTOR_GROWTH_CAP)
			v->capacity += VECTOR_GROWTH_CAP_GROWTH;
		else
			v->capacity = newsize;

		v->data = (void*)realloc(v->data, sizeof(VECTOR_TYPE) * v->capacity);
		assert(v->data != NULL);
	}
	// Set the new data
	v->data[v->len++] = data;
	// Return for convenience
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
	// If empty return null
	if (v->len == 0)
		return 0;

	// Decrease vector length	
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
