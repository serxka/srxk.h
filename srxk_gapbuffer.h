/*
* >> srxk_gapbuffer.h 0.1.0
* A generic C header only gap buffer implementation
*
* >> Usage
* ```
* #define GAPBUFFER_TYPE char
* //                     ^ you can put any valid c type here
* #include <srxk_gapbuffer.h>
* ```
* If you want to use a pointer or struct you must first typedef it like so:
* `typedef struct object* objectp;`
*
* Custom memory allocators are also supported via by defining CUSTOM_MALLOC,
* _REALLOC, *_FREE
* If an error ocurrs an integer called `gb_<type>_err` will be set
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
#include <stdlib.h> // malloc, realloc, free

// CONSTANTS
/*These can be tweaked for your needs*/
#ifndef GAPBUFFER_GROW_SIZE
	#define GAPBUFFER_GROW_SIZE 10
#endif // GAPBUFFER_GROW_SIZE

// THE MACRO MAGIC
#ifndef GAPBUFFER_TYPE
	#define GAPBUFFER_TYPE char
#endif

#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)

#define type(struct,type) EVALUATOR(struct, type)
#define function(name) EVALUATOR(GAPBUFFER, name)

#define GAPBUFFER type(gb, GAPBUFFER_TYPE)
#define GAPBUFFER_ERR EVALUATOR(GAPBUFFER, err)

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

// GAPBUFFER TYPE
typedef struct GAPBUFFER
{
	GAPBUFFER_TYPE *buf;
	int len;
	int gap_strt;
	int gap_len;
} GAPBUFFER;

// ERROR NUMBER
static int GAPBUFFER_ERR = 0;

// GAP BUFFER FUNCTIONS
/* 
* Description:
* 	Creates a new heap allocated gap buffer
* Parameters:
* 	int size - the initial size of the gap buffer
* Return Value:
* 	Returns the newly allocated gap buffer
*/
static GAPBUFFER *function(new)(int size)
{
	// Create our gap buffer
	GAPBUFFER *gb = (GAPBUFFER*)malloc(sizeof(GAPBUFFER));
	if (gb == NULL) {
		GAPBUFFER_ERR = ENOMEM;
		return NULL; }

	// Create our gap buffer buffer, and set values
	gb->buf = (GAPBUFFER_TYPE*)malloc(sizeof(GAPBUFFER_TYPE) * size);
	gb->len = size;
	gb->gap_strt = 0;
	gb->gap_len = size;

	// Check that it didn't fail
	if (gb->buf == NULL) {
		GAPBUFFER_ERR = ENOMEM;
		return NULL; }

	return gb;
}

/* 
* Description:
* 	Shifts the gap buffers' cursor to the left
* Parameters:
* 	GAPBUFFER *gb - The gap buffer to be operated on
* Return Value:
* 	None
*/
static void function(left)(GAPBUFFER *gb)
{
	// Check to see if we can still shift left
	if (gb->gap_strt == 0)
		return;
	 
	// Move down our gap start
	--gb->gap_strt;
	// Copy data to other end of gap
	gb->buf[gb->gap_strt+gb->gap_len] = gb->buf[gb->gap_strt];
}

/* 
* Description:
* 	Shifts the gap buffers' cursor to the right
* Parameters:
* 	GAPBUFFER *gb - The gap buffer to be operated on
* Return Value:
* 	None
*/
static void function(right)(GAPBUFFER *gb)
{
	// Check if can still shift right
	if (gb->gap_strt+gb->gap_len == gb->len)
		return;
	 
	// Copy data to other end of gap
	gb->buf[gb->gap_strt] = gb->buf[gb->gap_strt+gb->gap_len];
	// Move up our gap start
	++gb->gap_strt;
}

/* 
* Description:
* 	Grows the gap buffer by x units
* Parameters:
* 	GAPBUFFER *gb - The gap buffer to be operated on
*  int amount - The amount of elements to increase the gap by
* Return Value:
* 	None
*/
static void function(grow)(GAPBUFFER *gb, int amount)
{
	// Reallocate the buffer
	GAPBUFFER_TYPE *tmp = (GAPBUFFER_TYPE*)realloc(gb->buf, gb->len
			+ (amount * sizeof(GAPBUFFER_TYPE)));
	if (tmp == NULL) { // Check that it didn't fail
		GAPBUFFER_ERR = ENOMEM;
		return; }

	// Update values
	gb->buf = tmp;
	gb->len += amount;
	gb->gap_len += amount;
}

/* 
* Description:
* 	Inserts a new element at the cursor, grows if needed
* Parameters:
* 	GAPBUFFER *gb - The gap buffer to be operated on
*  GAPBUFFER_TYPE data - The data to be inserted at the cursor
* Return Value:
* 	None
*/
static void function(insert)(GAPBUFFER *gb, GAPBUFFER_TYPE data)
{
	// See if we need to grow buffer
	if (gb->gap_len == 0)
	{
		function(grow)(gb, GAPBUFFER_GROW_SIZE);
		// Check that if there an error growing the buffer
		if (GAPBUFFER_ERR == ENOMEM)
			return;
	}
	 
	// Copy the data to the buffer
	gb->buf[gb->gap_strt++] = data;
	--gb->gap_len;
}

/* 
* Description:
* 	Inserts an array of new elements at the cursor, grows if needed
* Parameters:
* 	GAPBUFFER *gb - The gap buffer to be operated on
*  GAPBUFFER_TYPE *data - A pointer to the data to be inserted at the cursor
*  int len - The amount of data to be inserted
* Return Value:
* 	None
*/
static void function(inserts)(GAPBUFFER *gb, GAPBUFFER_TYPE *data, int len)
{
	// See if we need to grow buffer
	if (gb->gap_len-len <= 0)
	{
		function(grow)(gb, len);
		// Check that if there an error growing the buffer
		if (GAPBUFFER_ERR == ENOMEM)
			return;
	}
	
	// Loop through data and copy to buffer
	for (int i = 0; i < len; ++i)
	{
		gb->buf[gb->gap_strt++] = data[i];
		--gb->gap_len;	
	}
}

/* 
* Description:
* 	Return the value in the buffer at an index, act like the gap doesnt exist
* Parameters:
* 	GAPBUFFER *gb - The gap buffer to be operated on
*  int index - The index to read
* Return Value:
* 	The value at the index, or if invalid index the first element and sets 
*  GAPBUFFER_ERR to ENODATA
*/
static GAPBUFFER_TYPE function(index)(GAPBUFFER *gb, int index)
{
	// Bounds check
	if (index < 0 || index >= gb->len - gb->gap_len) {
		GAPBUFFER_ERR = ENODATA; // Set an error value
		return gb->buf[0]; }
	 
	// Return at index, skip gap if index is after it
	if (index >= gb->gap_strt)
		return gb->buf[gb->gap_len+index];
	else
		return gb->buf[index];
}

/* 
* Description:
* 	Free's a heap allocated gap buffer
* Parameters:
* 	GAPBUFFER *gb - The gap buffer to be operated on
* Return Value:
* 	None
*/
static void function(free)(GAPBUFFER *gb)
{
	free(gb->buf);
	free(gb);
}

// Undefine the macros to keep things clean
#undef GAPBUFFER
#undef GAPBUFFER_TYPE
#undef GAPBUFFER_ERR
#undef PASTER
#undef EVALUATOR
#undef function
#undef type

#ifdef __cplusplus
} // extern "C" closing brace
#endif	// __cplusplus