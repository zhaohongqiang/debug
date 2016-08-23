#ifndef TD_VOIDARRAY
#define TD_VOIDARRAY
#include "comserv.h"

typedef struct _TD_Array{
  TDINT mArraySize;
  TDINT	mCount;
  void** mArray;
}TD_Array,*TDP_Array;



TDM_COMSERVAPI TDBOOL	TD_ArrayInit(TDP_Array pArray,TDINT nCount);
TDM_COMSERVAPI TDINT	TD_ArrayCount1(TDP_Array pArray);
TDM_COMSERVAPI TDVOID*  TD_ArrayElementAt1(TDP_Array pArray,TDINT aIndex);
TDM_COMSERVAPI TDINT	TD_ArrayIndexOf(TDP_Array pArray,void* aPossibleElement);
TDM_COMSERVAPI TDBOOL	TD_ArrayInsertElementAt(TDP_Array pArray,void* aElement, TDINT aIndex);
TDM_COMSERVAPI TDBOOL	TD_ArrayReplaceElementAt(TDP_Array pArray,void* aElement, TDINT aIndex);
TDM_COMSERVAPI TDBOOL	TD_ArrayAppendElement(TDP_Array pArray,void* aElement);
TDM_COMSERVAPI TDBOOL	TD_ArrayRemoveElement(TDP_Array pArray,void* aElement);
TDM_COMSERVAPI TDBOOL	TD_ArrayRemoveElementAt(TDP_Array pArray,TDINT aIndex);
TDM_COMSERVAPI void		TD_ArrayClear(TDP_Array pArray);




#define TD_ArrayCount(Array) ((Array)?(Array)->mCount:0)

#define TD_ArrayElementAt(Array,aIndex) ((Array)?(((aIndex)>=(Array)->mCount)?TDNULL:(Array)->mArray[(aIndex)]):TDNULL)

#endif 

