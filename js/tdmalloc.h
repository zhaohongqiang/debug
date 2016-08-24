#ifndef tdmem_h___
#define tdmem_h___


#define TDM_MALLOCVAPI
#include "../platabs/tcomdef.h"


extern 		void *	Malloc(TDUINT size);
TDM_MALLOCVAPI	void *	Calloc(TDUINT nelem, TDUINT elsize);
TDM_MALLOCVAPI	void *	Realloc(void *ptr, TDUINT size);
extern void		TDFree(void *ptr);


#endif /* tdmem_h___ */
