/*
* >> srxk_gapbuffer.h 0.0.0
* A C header only gap buffer implementation
*
* >> Usage
* ```
* #include <srxk_gapbuffer.h>
* ```
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

// <ABC> FUNCTIONS
/* 
* Description:
* 	abc
* Parameters:
* 	abc
* Return Value:
* 	abc
*/
static GAPBUFFER *function(new)(int size)
{
	// Create our gap buffer
	GAPBUFFER *gb = (GAPBUFFER*)malloc(sizeof(GAPBUFFER));
	if (gb == NULL) {
		GAPBUFFER_ERR = ENOMEM;
		return NULL; }

	// Create our gap buffer buffer, and set values
	gb->buf = malloc(sizeof(GAPBUFFER_TYPE) * size);
	gb->len = size;
	gb->gap_strt = 0;
	gb->gap_len = size;

	if (gb->buf == NULL) {
		GAPBUFFER_ERR = ENOMEM;
		return NULL; }

	return gb;
}

// <ABC> FUNCTIONS
/* 
* Description:
* 	abc
* Parameters:
* 	abc
* Return Value:
* 	abc
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

// <ABC> FUNCTIONS
/* 
* Description:
* 	abc
* Parameters:
* 	abc
* Return Value:
* 	abc
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

static void function(grow)(GAPBUFFER *gb, int amount)
{
	GAPBUFFER_TYPE *tmp = realloc(gb->buf, gb->len + (amount * sizeof(GAPBUFFER_TYPE)));
	if (tmp == NULL) {
		GAPBUFFER_ERR = ENOMEM;
		return; }

	gb->buf = tmp;
	gb->len += amount;
	gb->gap_len += amount;
}

static void function(insert)(GAPBUFFER *gb, GAPBUFFER_TYPE data)
{
	// See if we need to grow buffer
	if (gb->gap_len == 0)
		function(grow)(gb, GAPBUFFER_GROW_SIZE); // grow buffer
	// Check that we grew the buffer
	if (GAPBUFFER_ERR == ENOMEM)
		return;
	 
	gb->buf[gb->gap_strt++] = data;
	--gb->gap_len;
}

static GAPBUFFER_TYPE function(index)(GAPBUFFER *gb, int index)
{
	if (index < 0 || index >= gb->len - gb->gap_len) {
		GAPBUFFER_ERR = ENODATA; // out bounds 
		return gb->buf[0]; }
	 
	// Return at index, skip gap if index is after it
	if (index >= gb->gap_strt)
		return gb->buf[gb->gap_len+index];
	else
		return gb->buf[index];
}

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