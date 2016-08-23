#if !defined(_WHX_LIST_)
#define _WHX_LIST_

#include	"comserv.h"
/*/=============================================================================
//  The LINK structure is used to chain structures together into a list.
//=============================================================================
*/
typedef struct _TDLINK *TDPLINK;
typedef  TDPLINK	TDLISTPOSITION;
typedef struct _TDLINK
{
    TDPLINK     PrevLink;                    /*... Previous or back pointer.*/
    TDPLINK     NextLink;                    /*... Next or forward pointer.*/
	TDDWORD	    mData;
} TDLINK;

/*=============================================================================
  The LIST data structure.
=============================================================================*/

typedef struct _TDC_List
{
    TDPLINK      Tail;                        
    TDPLINK      Head;                        
    TDDWORD       Length;                      
} TDC_List;

typedef TDC_List *TDC_PList;

//#ifdef		TDM_SI_ENABLE_DIRECTCALL
TDM_COMSERVAPI	TDBOOL			TDC_List_List			(TDC_PList pThis);
TDM_COMSERVAPI	TDBOOL			TDC_List_DList			(TDC_PList pThis);
TDM_COMSERVAPI	TDDWORD			TDC_List_GetLength		(TDC_PList pThis);
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_GetHead		(TDC_PList pThis);
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_GetTail		(TDC_PList pThis);
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_GetNext		(TDLISTPOSITION pPos);
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_GetPrev		(TDLISTPOSITION pPos);
TDM_COMSERVAPI	TDDWORD			TDC_List_GetData		(TDLISTPOSITION pPos);
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_AddTail		(TDC_PList pThis,TDDWORD nValue);
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_AddHead		(TDC_PList pThis,TDDWORD nValue);
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_Insert			(TDC_PList pThis,TDDWORD nValue,TDLISTPOSITION BeforePos);
TDM_COMSERVAPI	TDBOOL			TDC_List_RemoveTail		(TDC_PList pThis);
TDM_COMSERVAPI	TDBOOL			TDC_List_RemoveHead		(TDC_PList pThis);
TDM_COMSERVAPI	TDBOOL			TDC_List_RemoveAll		(TDC_PList pThis);
TDM_COMSERVAPI	TDBOOL			TDC_List_RemoveAt		(TDC_PList pThis,TDLISTPOSITION Pos);
TDM_COMSERVAPI	TDBOOL			TDC_List_Remove			(TDC_PList pThis,TDDWORD nValue);
//#endif		/*TDM_SI_ENABLE_DIRECTCALL*/




#endif /*_WHX_LIST_*/

