
#include <stdio.h>
#include "tdmalloc.h"

int gHaveInitMalloc=0;
void*
Malloc(TDUINT size)
{
	void* t;
	t=malloc(size);	
	return t;
}


void TDFree(void *ptr)
{
	free(ptr);
}

void* Realloc(void *ptr, TDUINT size)
{
	return realloc(ptr,size);
}

void*
Calloc(TDUINT n, TDUINT size)
{
	return TDNULL;
}

void
TD_InitMem()
{
}


