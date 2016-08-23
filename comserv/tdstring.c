
#include "comserv.h"

/*static */const char* kWhitespace="\b\t\r\n ";
/**
 * Default constructor. Note that we actually allocate a small buffer
 * to begin with. This is because the "philosophy" of the string class
 * was to allow developers direct access to the underlying buffer for
 * performance reasons. 
 */
void TDString_Init(TDPString pThis){
  TDStr_Initialize1(pThis);
}

/*TDM_COMSERVAPI	*/
TDPString	TDString_Create()
{
	TDPString pThis;
	pThis=(TDPString)Malloc(sizeof(TDString));
	memset(pThis, 0, sizeof(TDString));
	TDString_Init(pThis);
	return pThis;
}

TDM_COMSERVAPI	TDBOOL	
TDString_Destroy(TDPString pThis)
{
	if(!pThis)
		return TDFALSE;
	TDString_DTDString(pThis);
	TDFree(pThis);
	return TDTRUE;;
}

void TDString_Subsume(TDStr* aDest,TDStr* aSource)
{
  if(aSource->mStr && aSource->mLength) 
  {
      TDStr_AssignStr(aDest,aSource,0,aSource->mLength);
  } 
  else 
	  TDStr_Truncate(aDest,0);
}


/** * This constructor accepts an ascii string */
void TDString_InitByChar(TDPString pThis,const char* aCString)
{  
//szj  TDStr_Initialize1(pThis);
  TDStr_Initialize3(pThis);
  TDStr_AssignChar(pThis,aCString);
}

/** * This is our copy constructor */
void TDString_InitByStr(TDPString pThis,const TDStr *aString ) 
{
//  TDStr_Initialize1(pThis);
  TDStr_Initialize3(pThis);
  TDStr_AssignStr(pThis,aString,0,aString->mLength);
}

/** * This is our copy constructor */
void TDString_InitByString(TDPString pThis,const TDPString aString) 
{
//szj  TDStr_Initialize1(pThis);
  TDStr_Initialize3(pThis);
  TDStr_AssignStr(pThis,aString,0,aString->mLength);
}

/** * Destructor * Make sure we call TDStr_Destroy. */
void TDString_DTDString(TDPString pThis) 
{
  TDStr_Destroy(pThis);
}

/** * This method truncates pThis string to given length. */
void TDString_Truncate(TDPString pThis,TDINT anIndex)
{
//  TDStr_Truncate(pThis,anIndex);
//	register int iLoop;
	int iLoop;

	if (anIndex > pThis->mLength)
		return;

	for (iLoop = 0; iLoop < anIndex; )
	{
		if (pThis->mStr[iLoop] > 128)
		{
			if ((iLoop + 2) > anIndex)
			{
				break;
			}
			iLoop += 2;
		}
		else
		{
			iLoop ++;
		}
	}

	TDStr_Truncate(pThis, iLoop);
}

/***  Determine whether or not the characters in pThis */
TDBOOL TDString_IsOrdered(TDPString pThis) 
{
  TDBOOL  result=TDTRUE;
  if(pThis->mLength>1) {
    TDINT theIndex;
    TDCHAR c1=0;
    TDCHAR c2=TDStr_GetCharAt(pThis,0);
    for(theIndex=1;theIndex<pThis->mLength;theIndex++) {
      c1=c2;
      c2=TDStr_GetCharAt(pThis,theIndex);
      if(c1>c2) {
        result=TDFALSE;
        break;
      }
    }
  }
  return result;
}


/** * Call pThis method if you want to force the string to a certain capacity */
void TDString_SetCapacity(TDPString pThis,TDINT aLength) 
{
  if(aLength>pThis->mLength) {
    TDStr_GrowCapacity(pThis,aLength);
  }
  pThis->mLength=aLength;
  AddNullTerminator(pThis);
}

/**********************************************************************
  Accessor methods...
 *********************************************************************/
/** */
char* TDString_GetBuffer(TDPString pThis) 
{
	if(pThis)
		return pThis->mStr;
	else
		return TDNULL;
}


TDCHAR TDString_CharAt(TDPString pThis,TDINT anIndex) 
{
  return TDStr_GetCharAt(pThis,anIndex);
}

TDCHAR TDString_First(TDPString pThis) 
{
  return TDStr_GetCharAt(pThis,0);
}

TDCHAR TDString_Last(TDPString pThis) 
{
  return TDStr_GetCharAt(pThis,pThis->mLength-1);
}

TDBOOL TDString_SetCharAt(TDPString pThis,TDCHAR aChar,TDINT anIndex)
{
  TDBOOL result=TDFALSE;
  if(anIndex<pThis->mLength){
      pThis->mStr[anIndex]=(char)aChar;
    result=TDTRUE;
  }
  return result;
}

void TDString_ToLowerCase(TDPString pThis)
{
  TDStr_ChangeCase(pThis,TDFALSE);
}

void TDString_ToUpperCase(TDPString pThis) 
{
  TDStr_ChangeCase(pThis,TDTRUE);
}

TDPString TDString_StripChars(TDPString pThis,const char* aSet){
  if(aSet){
    TDStr_StripChars(pThis,0,pThis->mLength,aSet);
  }
  return pThis;
}


TDPString TDString_StripWhitespace(TDPString pThis) 
{
  TDStr_StripChars(pThis,0,pThis->mLength,kWhitespace);
  return pThis;
}

TDPString TDString_ReplaceChar(TDPString pThis,TDCHAR aSourceChar, TDCHAR aDestChar) 
{
  TDINT theIndex=0;

    for(theIndex=0;theIndex<pThis->mLength;theIndex++)
	{
      if(pThis->mStr[theIndex]==(char)aSourceChar) 
	  {
        pThis->mStr[theIndex]=(char)aDestChar;
      }
    }
  return pThis;
}

TDPString TDString_Trim(TDPString pThis,const char* aTrimSet, TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing){
  if(aTrimSet){
    TDStr_Trim(pThis,aTrimSet,aEliminateLeading,aEliminateTrailing);
  }
  return pThis;
}

TDPString TDString_CompressSet(TDPString pThis,const char* aSet, char aChar, TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing){
  if(aSet){
    TDStr_CompressSet(pThis,aSet,aChar,aEliminateLeading,aEliminateTrailing);
  }
  return pThis;
}

TDPString TDString_CompressWhitespace(TDPString pThis, TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing)
{
  TDStr_CompressSet(pThis,kWhitespace,' ',aEliminateLeading,aEliminateTrailing);
  return pThis;
}

char* TDString_ToNewCString(TDPString pThis) 
{
  TDINT  theLength=pThis->mLength+2; 
  char*     result=(char *)Malloc(theLength);
	memset(result, 0, theLength);
  if(result)
        TDstrcpy(result,pThis->mStr);
  return result;
}

char* TDString_ToCString(TDPString pThis,char* aBuf, TDINT aBufLength,TDINT anOffset) 
{
/*  TDINT theLength=0;
 */
	if(0<aBufLength && pThis->mLength<aBufLength)
	{
      if(aBuf && (0<pThis->mLength)) 
	  {
        TDstrcpy(aBuf,pThis->mStr);
      }
    }
  return aBuf;
}
#if 0

float TDString_ToFloat(TDPString pThis,TDINT* aErrorCode) 
{
  char buf[100];
  char* cp ;
  float f ; 
  if (pThis->mLength > (TDINT)(sizeof(buf)-1) )
    return 0.0f;
  cp=TDString_ToCString(pThis,buf, sizeof(buf),0);
  sscanf(cp,"%f",&f);
  return f;
}
#endif 

TDINT TDString_ToInteger(TDPString pThis,TDINT* anErrorCode,TDINT aRadix) {

  TDINT result=0;
  char*   cp = pThis->mStr + pThis->mLength;
  TDINT theMult=1;
/*  char theChar=0;*/
  char theDigit=0;
  while(--cp>=pThis->mStr){
    char theChar=*cp;
/**/
	if ((theChar >= 'a') && (theChar <= 'f')) 
        theChar = 'A' + (theChar - 'a');
/**/
    if((theChar>='0') && (theChar<='9')){
      theDigit=theChar-'0';
    }
    else if((theChar>='A') && (theChar<='F')) {
      if(10==aRadix){
        result=0;
        break;
      }
      theDigit=(theChar-'A')+10;
    }
    else if('-'==theChar) {
      result=-result;
      break;
    }
    else if(('+'==theChar) || (' '==theChar)) { 
      break;
    }
    else {
      //result=0;
	  *anErrorCode=-1;
      return 0;
    }

    result+=theDigit*theMult;
    theMult*=aRadix;
  }
  
  return result;
}

TDPString TDString_Assign(TDPString  pThis,const TDStr* aString,TDINT aCount) 
{
  if(pThis && aString){
    if(-1==aCount) 
		aCount=aString->mLength;
    TDStr_AssignStr(pThis,aString,0,aCount);
  }
  return pThis;
}

TDPString TDString_AssignString(TDPString  pThis,TDPString aString,TDINT aCount) 
{
  if(pThis && aString)
  {
    if(-1==aCount) 
		aCount=aString->mLength;
    TDStr_AssignStr(pThis,aString,0,aCount);
  }
  return pThis;
}

TDPString TDString_AssignPChar(TDPString  pThis,const char* aCString) 
{
  TDStr_Truncate(pThis,0);
  if(aCString){
    TDString_AppendPChar(pThis,aCString);
  }
  return pThis;
}


TDPString TDString_AssignPCharN(TDPString  pThis,const char* aCString,TDINT nCount) 
{
  TDStr_Truncate(pThis,0);
  if(aCString){
    TDString_AppendPCharN(pThis,aCString,nCount);
  }
  return pThis;
}

TDPString TDString_AssignChar(TDPString  pThis,char aChar) 
{
  TDStr_Truncate(pThis,0 );
  return TDString_AppendChar(pThis,aChar);
}
 
TDPString TDString_AppendStr(TDPString  pThis,const TDStr* aString,TDINT aCount) 
{
  if(-1==aCount) 
	  aCount=aString->mLength;
  TDStr_AppendStr(pThis,aString,0,aCount);
  return pThis;
}


TDPString TDString_AppendString(TDPString  pThis,const TDPString aString,TDINT aCount) 
{
  if(-1==aCount) 
	  aCount=aString->mLength;
  TDStr_AppendStr(pThis,aString,0,aCount);
  return pThis;
}


TDPString TDString_AppendPChar(TDPString  pThis,const char* aCString) 
{
	TDINT aCount=0;
  if(aCString)
  {
    TDStr temp= {0, 0, NULL};
    TDStr_Initialize1(&temp);
    temp.mStr=(char*)aCString;
	aCount=  TDStrLen(aCString);
    temp.mLength = aCount;
    TDStr_AppendStr(pThis,&temp,0,aCount);
  }
  return pThis;
}
 
TDPString TDString_AppendPCharN(TDPString  pThis,const char* aCString,TDINT nCount) 
{
/*	TDINT aCount=0;*/
  if(aCString)
  {
    TDStr temp= {0, 0, NULL};
    TDStr_Initialize1(&temp);
    temp.mStr=(char*)aCString;
    temp.mLength = nCount;
    TDStr_AppendStr(pThis,&temp,0,nCount);
  }
  return pThis;
}
 
TDPString TDString_AppendChar(TDPString  pThis,char aChar) 
{
  char buf[2]={0,0};
  TDStr temp= {0, 0, NULL};
  buf[0]=aChar;

  TDStr_Initialize1(&temp);
  temp.mStr=buf;
  temp.mLength=1;
  TDStr_AppendStr(pThis,&temp,0,1);
  return pThis;
}


TDPString TDString_AppendInt(TDPString  pThis,TDINT aInteger,TDINT aRadix) 
{
	TDINT i,tmp=0;
	if(aInteger<0)
	{
		TDString_AppendChar(pThis,'-');
		aInteger=-aInteger;
	}
	if(aInteger==0)
	{
		TDString_AppendChar(pThis,'0');
		return pThis;
	}
	while(aInteger>0)
	{
		i=aInteger-aRadix*(aInteger/aRadix);
		if(aRadix==10)
			TDString_InsertChar(pThis,'0'+i,pThis->mLength-tmp);
		else
		{
			if(i<10)
				TDString_InsertChar(pThis,'0'+i,pThis->mLength-tmp);
			else
				TDString_InsertChar(pThis,'A'+i-10,pThis->mLength-tmp);
		}
		if(aInteger==0)
			break;
		aInteger/=aRadix;
		tmp++;
	}
	return pThis;
}

TDINT TDString_Left(TDPString  pThis,TDPString aDest,TDINT aCount) 
{
  TDStr_AssignStr(aDest,pThis,0,aCount);
  return aDest->mLength;
}

TDINT TDString_Mid(TDPString  pThis,TDPString aDest,TDINT anOffset,TDINT aCount)
{
  TDStr_AssignStr(aDest,pThis,anOffset,aCount);
  return aDest->mLength;
}

TDINT TDString_Right(TDPString  pThis,TDPString aCopy,TDINT aCount) 
{
  TDINT offset=pThis->mLength-aCount;
  if(offset<0)
	  offset=0;
  return TDString_Mid(pThis,aCopy,offset,aCount);
}


TDPString TDString_Insert(TDPString  pThis,const TDPString aCopy,TDINT anOffset,TDINT aCount) 
{
  TDStr_Insert(pThis,anOffset,aCopy,0,aCount);
  return pThis;
}

TDPString TDString_InsertChar(TDPString  pThis,TDCHAR aChar,TDINT anOffset) 
{
  TDString str = {0, 0, NULL};
  TDString_Init(&str);
  TDString_AssignChar(&str,aChar);
  TDStr_Insert(pThis,anOffset,&str,0,1);
  TDString_DTDString(&str);
  return pThis;
}

TDPString TDString_InsertPChar(TDPString  pThis,TDCHAR *pChar,TDINT anOffset,TDINT aCount) 
{
  TDString str = {0, 0, NULL};
  TDString_InitByChar(&str,pChar);
  TDStr_Insert(pThis,anOffset,&str,0,str.mLength<aCount?str.mLength:aCount);
  return pThis;
}

TDPString TDString_Cut(TDPString  pThis,TDINT anOffset, TDINT aCount)
{
  TDStr_Delete(pThis,anOffset,aCount);
  return pThis;
}

TDINT TDString_BinarySearch(TDPString  pThis,TDCHAR aChar) 
{
  TDINT low=0;
  TDINT high=pThis->mLength-1;

  while (low <= high) {
    int middle = (low + high) >> 1;
    TDCHAR theChar=TDStr_GetCharAt(pThis,middle);
    if (theChar==aChar)
      return middle;
    if (theChar>aChar)
      high = middle - 1; 
    else
      low = middle + 1; 
  }
  return kNotFound;
}

TDINT TDString_FindPChar(TDPString  pThis,const char* aCString,TDBOOL aIgnoreCase) 
{

  TDINT result=kNotFound;
  if(aCString) {
		TDStr temp = {0, 0, NULL};

    TDStr_Initialize1(&temp);
    temp.mLength= TDStrLen(aCString);
    temp.mStr=(char*)aCString;
    result=TDStr_FindSubstr(pThis,&temp,aIgnoreCase,0);
  }
  return result;
}

TDINT TDString_FindPCharFrom(TDPString  pThis,const char* aCString,TDBOOL aIgnoreCase,TDINT aOffset) 
{

  TDINT result=kNotFound;
  if(aCString) {
    TDStr temp= {0, 0, NULL};
    TDStr_Initialize1(&temp);
    temp.mLength= TDStrLen(aCString);
    temp.mStr=(char*)aCString;
    result=TDStr_FindSubstr(pThis,&temp,aIgnoreCase,aOffset);
  }
  return result;
}


TDINT TDString_FindStr(TDPString  pThis,const TDStr* aString,TDBOOL aIgnoreCase) 
{
  TDINT result=TDStr_FindSubstr(pThis,aString,aIgnoreCase,0);
  return result;
}

TDINT TDString_FindChar(TDPString  pThis,TDCHAR aChar,TDINT anOffset,TDBOOL aIgnoreCase) 
{
  TDINT result=TDStr_FindChar(pThis,aChar,aIgnoreCase,anOffset);
  return result;
}

TDINT TDString_FindCharInSet(TDPString  pThis,const char* aCStringSet,TDINT anOffset) 
{

  TDINT result=kNotFound;
  if(aCStringSet) {
    TDStr temp= {0, 0, NULL};
    TDStr_Initialize1(&temp);
    temp.mLength= TDStrLen(aCStringSet);
    temp.mStr=(char*)aCStringSet;
    result=TDStr_FindCharInSet(pThis,&temp,TDFALSE,anOffset);
  }
  return result;
}

TDINT TDString_FindStrInSet(TDPString  pThis,const TDPString aSet,TDINT anOffset) 
{
  TDINT result=TDStr_FindCharInSet(pThis,aSet,TDFALSE,anOffset);
  return result;
}

TDINT TDString_RFindPCharInSet(TDPString  pThis,const char* aCStringSet,TDINT anOffset) 
{

  TDINT result=kNotFound;
  if(aCStringSet) {
    TDStr temp= {0, 0, NULL};
    TDStr_Initialize1(&temp);
    temp.mLength=TDStrLen(aCStringSet);
    temp.mStr=(char*)aCStringSet;
    result=TDStr_RFindCharInSet(pThis,&temp,TDFALSE,anOffset);
  }
  return result;
}

TDINT TDString_RFindStrInSet(TDPString  pThis,const TDPString aSet,TDINT anOffset) 
{
  TDINT result=TDStr_RFindCharInSet(pThis,aSet,TDFALSE,anOffset);
  return result;
}

TDINT TDString_RFindStr(TDPString  pThis,const TDStr* aString,TDBOOL aIgnoreCase) 
{
  TDINT result=TDStr_RFindSubstr(pThis,aString,aIgnoreCase,0);
  return result;
}

TDINT TDString_RFindPChar(TDPString  pThis,const char* aString,TDBOOL aIgnoreCase) 
{
  TDINT result=kNotFound;
  if(aString) {
    TDStr temp= {0, 0, NULL};
    TDStr_Initialize1(&temp);
    temp.mLength= TDStrLen(aString);
    temp.mStr=(char*)aString;
    result=TDStr_RFindSubstr(pThis,&temp,aIgnoreCase,0);
  }
  return result;
}

TDINT TDString_RFindChar(TDPString  pThis,TDCHAR aChar,TDINT anOffset,TDBOOL aIgnoreCase) 
{
  TDINT result=TDStr_RFindChar(pThis,aChar,aIgnoreCase,anOffset);
  return result;
}


TDINT TDString_CompareStr(TDPString  pThis,const TDStr* aString,TDBOOL aIgnoreCase,TDINT aLength) 
{
  return TDStr_Compare(pThis,aString,aLength,aIgnoreCase);
}

TDINT TDString_ComparePChar(TDPString  pThis,const char *aCString,TDBOOL aIgnoreCase,TDINT aLength) 
 {
  if(aCString) {
    TDStr temp= {0, 0, NULL};
    TDStr_Initialize1(&temp);
    temp.mLength=TDStrLen(aCString);
    temp.mStr=(char*)aCString;
    return TDStr_Compare(pThis,&temp,aLength,aIgnoreCase);
  }
  return 0;
}


TDBOOL TDString_EqIgCaseString(TDPString  pThis,const TDPString aString) 
{
  return TDString_EqString(pThis,aString,TDTRUE);
}

TDBOOL TDString_EqIgCasePChar(TDPString  pThis,const char* aString,TDINT aLength) 
{
  return TDString_EqPChar(pThis,aString,TDTRUE);
}

TDBOOL TDString_EqString(TDPString  pThis,const TDPString aString,TDBOOL aIgnoreCase) 
{
  TDINT result=TDStr_Compare(pThis,aString,MinInt(pThis->mLength,aString->mLength),aIgnoreCase);
  return (TDBOOL)(0==result);
}

TDBOOL TDString_EqPChar(TDPString  pThis,const char* aCString,TDBOOL aIgnoreCase) 
{
  TDBOOL result=TDFALSE;
  if(aCString) {
    TDINT theAnswer=TDString_ComparePChar(pThis,aCString,aIgnoreCase,TDStrLen(aCString));
    result=(TDBOOL)(0==theAnswer);
  }
  return result;
}

/**
 *  Determine if given char in valid alpha range
 */
TDBOOL TDString_IsAlpha(TDCHAR aChar) {
  if (((aChar >= 'A') && (aChar <= 'Z')) || ((aChar >= 'a') && (aChar <= 'z'))) {
    return TDTRUE;
  }
  return TDFALSE;
}

/**
 *  Determine if given char is a valid space character
 */
TDBOOL TDString_IsSpace(TDCHAR aChar) {
  if ((aChar == ' ') || (aChar == '\r') || (aChar == 0x0A) || (aChar == 0x0D) || (aChar == '\t')) {
    return TDTRUE;
  }
  return TDFALSE;
}

/**
 *  Determine if given char is valid digit
 */
TDBOOL TDString_IsDigit(TDCHAR aChar) {
  return (TDBOOL)((aChar >= '0') && (aChar <= '9'));
}

TDINT TDString_GetIntegerFrom(TDPString pThis,TDINT offset,TDPINT num)
{
	TDString aString = {0, 0, NULL};
	TDINT i,start;
	TDString_Init(&aString);
	start=offset;
	if(TDString_CharAt(pThis,offset)=='-' ||TDString_CharAt(pThis,offset)=='+')
		offset++;
	for(i=offset;i<pThis->mLength;i++)
	{
		if(!TDString_IsDigit(TDString_CharAt(pThis,i)))
			break;
	}
	if(i>start)
	{
		TDDWORD er;
		TDString_Mid(pThis,&aString,start,i-start);
		*num=TDString_ToInteger(&aString,&er,10);
		TDString_DTDString(&aString);
		return i;
	}
	TDString_DTDString(&aString);
	return 0;
}
TDBOOL TDString_GetRect(TDPString pThis,TDPRECT pRect)
{
	TDINT offset=0;
	TDString_CompressWhitespace(pThis,TDTRUE,TDTRUE);
	offset=TDString_GetIntegerFrom(pThis,offset,&pRect->left);
	if(offset==0)
		return TDFALSE;
	if(TDString_CharAt(pThis,offset)==','||TDString_CharAt(pThis,offset)==' ')
		offset++;
	offset=TDString_GetIntegerFrom(pThis,offset,&pRect->top);
	if(offset==0)
		return TDFALSE;
	if(TDString_CharAt(pThis,offset)==','||TDString_CharAt(pThis,offset)==' ')
		offset++;
	offset=TDString_GetIntegerFrom(pThis,offset,&pRect->right);
	if(offset==0)
		return TDFALSE;
	if(TDString_CharAt(pThis,offset)==','||TDString_CharAt(pThis,offset)==' ')
		offset++;
	offset=TDString_GetIntegerFrom(pThis,offset,&pRect->bottom);
	if(offset==0)
		return TDFALSE;
	return TDTRUE;
}

TDPRECT TDString_ToRect(TDPString pThis)
{
/*	TDINT offset=0;
*/	TDPRECT pRect;
	pRect=(TDPRECT)Malloc(sizeof(TDRECT));
	memset(pRect,0,sizeof(TDRECT));
	if(TDString_GetRect(pThis,pRect))
		return pRect;
	TDFree(pRect);
	return TDNULL;
}

TDPCIRCLE TDString_ToCircle(TDPString pThis)
{
	TDINT		offset=0;
	TDPCIRCLE	pCircle;
	pCircle=(TDPCIRCLE)Malloc(sizeof(TDCIRCLE));
	memset(pCircle,0,sizeof(TDCIRCLE));
	TDString_CompressWhitespace(pThis,TDTRUE,TDTRUE);
	offset=TDString_GetIntegerFrom(pThis,offset,&pCircle->x);
	if(offset==0)
		return TDFALSE;
	if(TDString_CharAt(pThis,offset)==','||TDString_CharAt(pThis,offset)==' ')
		offset++;
	offset=TDString_GetIntegerFrom(pThis,offset,&pCircle->y);
	if(offset==0)
		return TDFALSE;
	if(TDString_CharAt(pThis,offset)==','||TDString_CharAt(pThis,offset)==' ')
		offset++;
	offset=TDString_GetIntegerFrom(pThis,offset,&pCircle->radius);
	if(offset==0)
		return TDFALSE;
	return pCircle;
}

TDPPOLYGON TDString_ToPoly(TDPString pThis)
{
	TDINT		offset=0/*,Num=0*/;
	TD_Array	array;
	TDPPOINT	pPoint;
	TDPPOLYGON	pPoly=TDNULL;
	TD_ArrayInit(&array,10);
	
	TDString_CompressWhitespace(pThis,TDTRUE,TDTRUE);
	/*left*/
	while(offset < pThis->mLength)
	{
		TDINT num1=0,num2=0;
		offset=TDString_GetIntegerFrom(pThis,offset,&num1);
		if(offset==0)
			break;
		if(TDString_CharAt(pThis,offset)==','||TDString_CharAt(pThis,offset)==' ')
			offset++;
		else
			break;
		offset=TDString_GetIntegerFrom(pThis,offset,&num2);
		if(offset==0)
			break;
		pPoint=(TDPPOINT)Malloc(sizeof(TDPOINT));
		TD_ArrayAppendElement(&array,pPoint);
		pPoint->x=num1;pPoint->y=num2;
		if(TDString_CharAt(pThis,offset)==','||TDString_CharAt(pThis,offset)==' ')
			offset++;
		else
			break;
	}
	pPoly=(TDPPOLYGON)Malloc(sizeof(TDPOLYGON)+array.mCount*sizeof(TDPOINT));
	memset(pPoly,0,sizeof(TDPOLYGON)+array.mCount*sizeof(TDPOINT));
	pPoly->pPtArray=(TDPPOINT)(pPoly+1);
	pPoly->nPtNum=array.mCount;
	for(offset=0;offset<array.mCount;offset++)
	{
		pPoint=TD_ArrayElementAt(&array,offset);
		pPoly->pPtArray[offset].x=pPoint->x;
		pPoly->pPtArray[offset].y=pPoint->y;
		TDFree(pPoint);
	}
	TD_ArrayClear(&array);
	return pPoly;
}

TDPSIZE TDString_ToSize(TDPString pThis)
{
	TDINT		offset=0;
	TDPSIZE		pSize;
	pSize=(TDPSIZE)Malloc(sizeof(TDSIZE));
	memset(pSize,0,sizeof(TDSIZE));
	TDString_CompressWhitespace(pThis,TDTRUE,TDTRUE);
	offset=TDString_GetIntegerFrom(pThis,offset,&pSize->cx);
	if(offset==0)
		return TDFALSE;
	if(TDString_CharAt(pThis,offset)==','||TDString_CharAt(pThis,offset)==' ')
		offset++;
	offset=TDString_GetIntegerFrom(pThis,offset,&pSize->cy);
	if(offset==0)
		return TDFALSE;
	return pSize;
}

TDM_COMSERVAPI	TDRGB		
TDString_ToColor(TDPString pThis)
{
	TDINT color,err;
	if(TDString_CharAt(pThis,0) == '#')
	{
		TDString_SetCharAt(pThis,'0',0);
		color=TDString_ToInteger(pThis,&err,16);
		TDString_SetCharAt(pThis,'#',0);
	}
	else 
		color=TDString_ToInteger(pThis,&err,10);
	return color;
}


TDM_COMSERVAPI	TDBOOL		
TDString_AppendRect(TDPString pThis,TDPRECT pRect)
{
	if(!pRect || !pThis)
		return TDFALSE;
	TDString_AppendInt(pThis,pRect->left,10);
	TDString_AppendChar(pThis,',');
	TDString_AppendInt(pThis,pRect->top,10);
	TDString_AppendChar(pThis,',');
	TDString_AppendInt(pThis,pRect->right,10);
	TDString_AppendChar(pThis,',');
	TDString_AppendInt(pThis,pRect->bottom,10);
	return TDTRUE;
}

TDM_COMSERVAPI	TDBOOL		
TDString_AppendPoly			(TDPString pThis,TDPPOLYGON pPoly)
{
	TDINT i;
	if(!pPoly || !pThis)
		return TDFALSE;
	for(i=0;i<pPoly->nPtNum;i++)
	{
		TDString_AppendInt(pThis,pPoly->pPtArray[i].x,10);
		TDString_AppendChar(pThis,',');
		TDString_AppendInt(pThis,pPoly->pPtArray[i].y,10);
		TDString_AppendChar(pThis,',');
	}
	TDString_Cut(pThis,pThis->mLength-1,1);
	return TDTRUE;
}

TDM_COMSERVAPI	TDBOOL		
TDString_AppendCircle		(TDPString pThis,TDPCIRCLE pCircle)
{
	if(!pCircle || !pThis)
		return TDFALSE;
	TDString_AppendInt(pThis,pCircle->x,10);
	TDString_AppendChar(pThis,',');
	TDString_AppendInt(pThis,pCircle->y,10);
	TDString_AppendChar(pThis,',');
	TDString_AppendInt(pThis,pCircle->radius,10);
	return TDTRUE;
}

TDM_COMSERVAPI	TDBOOL		
TDString_AppendSize			(TDPString pThis,TDPSIZE pSize)
{
	if(!pSize || !pThis)
		return TDFALSE;
	TDString_AppendInt(pThis,pSize->cx,10);
	TDString_AppendChar(pThis,',');
	TDString_AppendInt(pThis,pSize->cy,10);
	return TDTRUE;
}

TDM_COMSERVAPI	TDBOOL		
TDString_AppendColor		(TDPString pThis,TDRGB pRGB)
{
	if(!pThis)
		return TDFALSE;
	TDString_AppendInt(pThis,(TDINT)pRGB,16);
	return TDTRUE;
}

/**/
#define NOT_USED 0xfffd 

static TDWORD gHackTable[32] = {
	NOT_USED,
	NOT_USED,
	0x201a,  /* SINGLE LOW-9 QUOTATION MARK */
	0x0192,  /* LATIN SMALL LETTER F WITH HOOK */
	0x201e,  /* DOUBLE LOW-9 QUOTATION MARK */
	0x2026,  /* HORIZONTAL ELLIPSIS */
	0x2020,  /* DAGGER */
	0x2021,  /* DOUBLE DAGGER */
	0x02c6,  /* MODIFIER LETTER CIRCUMFLEX ACCENT */
	0x2030,  /* PER MILLE SIGN */
	0x0160,  /* LATIN CAPITAL LETTER S WITH CARON */
	0x2039,  /* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
	0x0152,  /* LATIN CAPITAL LIGATURE OE */
	NOT_USED,
	NOT_USED,
	NOT_USED,
	NOT_USED,
	0x2018,  /* LEFT SINGLE QUOTATION MARK */
	0x2019,  /* RIGHT SINGLE QUOTATION MARK */
	0x201c,  /* LEFT DOUBLE QUOTATION MARK */
	0x201d,  /* RIGHT DOUBLE QUOTATION MARK */
	0x2022,  /* BULLET */
	0x2013,  /* EN DASH */
	0x2014,  /* EM DASH */
	0x02dc,  /* SMALL TILDE */
	0x2122,  /* TRADE MARK SIGN */
	0x0161,  /* LATIN SMALL LETTER S WITH CARON */
	0x203a,  /* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
	0x0153,  /* LATIN SMALL LIGATURE OE */
	NOT_USED,
	NOT_USED,
	0x0178   /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
};

TDM_COMSERVAPI	TDPWORD		TDString_GetUChar		(TDPString  pThis)
{
	if(!pThis )
		return TDNULL;
	return TDString_PCharToPUChar(pThis->mStr,pThis->mLength);
}
TDM_COMSERVAPI	TDPWORD		TDString_PCharToPUChar	(TDCHAR * aSrc,TDINT len)
{
	TDPWORD  ptr=TDNULL;
	TDPBYTE   src;
	TDINT i;
	if( !aSrc)
		return TDNULL;
	ptr=(TDPWORD)Malloc((len+1)*sizeof(TDWORD));
	memset(ptr, 0, (len+1)*sizeof(TDWORD));
	src=(TDPBYTE)aSrc;
	for(i=0;i<len;i++)
	{
		if(aSrc[i]>=0 && aSrc[i
			]<32 )
			ptr[i]=(TDCHAR)gHackTable[aSrc[i]];
		if( (TDWORD)aSrc[i] < 128 )
			ptr[i]=(TDCHAR)aSrc[i];
		if((TDWORD)aSrc[i] >= 128 )
		{
			ptr[i]=((aSrc[i]<<8) & 0xff00)|(0x00ff & aSrc[i+1]) ;
			i++;
		}
	}
	ptr[len]=0;
	return ptr;
}
TDM_COMSERVAPI	TDCHAR *	TDString_PUCharToPChar	(TDPWORD  aSrc,TDINT len)
{
	TDCHAR * ptr=TDNULL;
	TDINT i;
	if( !aSrc || len <=0)
		return TDNULL;
	ptr=(TDCHAR*)Malloc(len+1);
	memset(ptr, 0, len+1);
	for(i=0;i<len;i++)
		ptr[i]=(TDCHAR)aSrc[i];
	ptr[len]=0;
	return ptr;
}
TDM_COMSERVAPI	TDWORD		TDString_CharToUChar	(TDCHAR  aSrc)
{
	if(aSrc >=0 && aSrc <32)
		return gHackTable[aSrc];
	return (TDWORD)aSrc;
}
TDM_COMSERVAPI	TDCHAR		TDString_UCharToChar	(TDWORD  aSrc)
{
	return (TDCHAR)aSrc;
}

