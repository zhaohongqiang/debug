#ifndef tdmem_h___
#define tdmem_h___

#include "../platabs/platabs.h"

#define TDM_MALLOCVAPI


extern 		void *	Malloc(TDUINT size);
TDM_MALLOCVAPI	void *	Calloc(TDUINT nelem, TDUINT elsize);
TDM_MALLOCVAPI	void *	Realloc(void *ptr, TDUINT size);
extern void		TDFree(void *ptr);


#endif /* tdmem_h___ */
