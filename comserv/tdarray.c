#include "comserv.h"

static TDINT kGrowArrayBy = 8;

TDM_COMSERVAPI TDBOOL	TD_ArrayInit(TDP_Array pArray,TDINT nCount)
{
	if(!pArray)
		return TDFALSE;

	if(nCount<kGrowArrayBy)
		nCount=kGrowArrayBy;
	pArray->mCount =0;
	pArray->mArraySize = nCount;
	pArray->mArray = Malloc(sizeof(void*)*nCount);
	memset(pArray->mArray, 0, nCount * sizeof(void*));
	return TDTRUE;
}
TDM_COMSERVAPI TDVOID TD_ArrayDestroy(TDP_Array pArray)
{
  if (TDNULL != pArray && pArray->mArray) 
  {
    TDFree(pArray->mArray);
  }
}
TDM_COMSERVAPI TDINT	TD_ArrayCount1(TDP_Array pArray)
{
	if(!pArray)
		return 0;
	return pArray->mCount;
}

TDM_COMSERVAPI TDVOID*  TD_ArrayElementAt1(TDP_Array pArray,TDINT aIndex)
{
	if(!pArray)
		return TDNULL;
	if (aIndex >= pArray->mCount) 
	  {
		return TDNULL;
	  }
	  return pArray->mArray[aIndex];
}

TDM_COMSERVAPI TDINT	TD_ArrayIndexOf(TDP_Array pArray,void* aPossibleElement)
{
	void** ap ;
	void** end;
	if(!pArray)
		return -1;
	ap=pArray->mArray;
	end=ap+pArray->mCount;
	while (ap < end) {
		if (*ap == aPossibleElement) {
		  return ap - pArray->mArray;
		}
		ap++;
	}
	return -1;
}

TDM_COMSERVAPI TDBOOL	TD_ArrayInsertElementAt(TDP_Array pArray,void* aElement, TDINT aIndex)
{
	TDINT oldCount ;
	if(!pArray)
		return TDNULL;
  	oldCount=pArray->mCount;
  	if (aIndex > oldCount) 
	  return TDFALSE;
  if (oldCount + 1 > pArray->mArraySize) 
  {
    void** newArray;
    TDINT	slide;
    TDINT newCount = oldCount + kGrowArrayBy;
    newArray = Malloc( sizeof(void*)*newCount);
	if(!newArray)
		return TDFALSE;
    if (pArray->mArray != TDNULL && aIndex != 0)
      memcpy(newArray, pArray->mArray, aIndex * sizeof(void*));
    slide = oldCount - aIndex;
    if (0 != slide) 
	{
      memcpy(newArray + aIndex + 1, pArray->mArray + aIndex,
                    slide * sizeof(void*));
    }
    if (pArray->mArray != TDNULL)
      TDFree(pArray->mArray);
    pArray->mArray = newArray;
    pArray->mArraySize = newCount;
  } 
  else {
    TDINT slide = oldCount - aIndex;
    if (0 != slide) {
      memmove(pArray->mArray + aIndex + 1, pArray->mArray + aIndex,
                     slide * sizeof(void*));
    }
  }
  pArray->mArray[aIndex] = aElement;
  pArray->mCount++;

  return TDTRUE;

}
TDM_COMSERVAPI TDBOOL	TD_ArrayReplaceElementAt(TDP_Array pArray,void* aElement, TDINT aIndex)
{
 	if(!pArray)
		return TDFALSE;
	if (aIndex >= pArray->mArraySize) 
	{
    TDINT requestedCount = aIndex + 1;
    TDINT growDelta = requestedCount - pArray->mCount;
    TDINT newCount = pArray->mCount + (growDelta > kGrowArrayBy ? growDelta : kGrowArrayBy);
    void** newArray = Malloc(sizeof(void*)*newCount);
    memset(newArray, 0, newCount * sizeof(void*));
    if (newArray==TDNULL)
       return TDFALSE;
    if (pArray->mArray != TDNULL && aIndex != 0) 
	{
      memcpy(newArray, pArray->mArray, pArray->mCount * sizeof(void*));
      if (pArray->mArray != TDNULL)
         TDFree(pArray->mArray);
    }
    pArray->mArray = newArray;
    pArray->mArraySize = newCount;
  }
  pArray->mArray[aIndex] = aElement;
  if (aIndex >= pArray->mCount)
     pArray->mCount = aIndex+1;
  return TDTRUE;
}
TDM_COMSERVAPI TDBOOL	TD_ArrayAppendElement(TDP_Array pArray,void* aElement)
{
    return TD_ArrayInsertElementAt(pArray,aElement, pArray->mCount);
}
TDM_COMSERVAPI TDBOOL	TD_ArrayRemoveElement(TDP_Array pArray,void* aElement)
{
	TDINT aIndex=-1;
 	if(!pArray)
		return TDFALSE;
	aIndex=TD_ArrayIndexOf(pArray,aElement);
	if(aIndex >=0)
		return TD_ArrayRemoveElementAt(pArray,aIndex);
	return TDFALSE;

}
TDM_COMSERVAPI TDBOOL	TD_ArrayRemoveElementAt(TDP_Array pArray,TDINT aIndex)
{
	TDINT oldCount;
 	if(!pArray)
		return TDFALSE;
	oldCount = pArray->mCount;
	if (aIndex >= oldCount)
	{
	  return TDFALSE;
	}

	if (aIndex < (oldCount - 1)) {
	  memmove(pArray->mArray + aIndex, pArray->mArray + aIndex + 1,
					 (oldCount - 1 - aIndex) * sizeof(void*));
	}

	pArray->mCount--;
	return TDTRUE;
}
TDM_COMSERVAPI void		TD_ArrayClear(TDP_Array pArray)
{
 	if(!pArray)
		return;
  if (pArray->mArray) 
    TDFree(pArray->mArray);
	pArray->mArraySize =0;
	pArray->mArray = 0;
	pArray->mCount = 0;
}

