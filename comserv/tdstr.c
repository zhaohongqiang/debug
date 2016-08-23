#include "comserv.h"
//#include "tdstr.h"

static char* gCommonEmptyBuffer=0;


#define KSHIFTLEFT  (0)
#define KSHIFTRIGHT (1)

#ifndef XP_PC
TDM_COMSERVAPI TDCHAR 
toupper(TDCHAR ch)
{
	if(ch>'a' && ch<'z')
		return ch+'A'-'a';
	return ch;
}
TDM_COMSERVAPI TDCHAR 
tolower(TDCHAR ch)
{
	if(ch>'A' && ch<'Z')
		return ch+'a'-'A';
	return ch;
}

#endif
#if defined(_WIN32_WCE)||defined(XP_CCUBE)||defined(XP_UNIX)
TDM_COMSERVAPI TDINT stricmp(const TDCHAR *dst,const TDCHAR *src)
{
TDINT f,l;
   do{


         if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
              f -= ('A' - 'a');
         if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
              l -= ('A' - 'a');
   }while ( f && (f == l) );
   return(f - l);
}
TDM_COMSERVAPI TDINT 
strnicmp(const TDCHAR *dst,const TDCHAR *src,TDINT aCount)
{
TDINT f,l,i=0;
   do{
         if ( ((f = (unsigned char)(*(dst+i))) >= 'A') && (f <= 'Z') )
              f -= ('A' - 'a');
         if ( ((l = (unsigned char)(*(src+i))) >= 'A') && (l <= 'Z') )
              l -= ('A' - 'a');
		 i++;
   }while ( f && (f == l) && i<aCount );
   return(f - l);
}
#endif

TDM_COMSERVAPI TDDWORD TDStrLen(const TDCHAR *str)
{
	TDINT	cycle;
	cycle = 0;
	while(str[cycle] != 0)
		cycle++;
	return cycle;
}




void ShiftCharsLeft(char* aDest,TDINT aLength,TDINT anOffset,TDINT aCount) { 
  TDINT theMax=aLength-anOffset;
  TDINT theLength=(theMax<aCount) ? theMax : aCount;

  char* first= aDest+anOffset+aCount;
  char* last = aDest+aLength;
  char* to   = aDest+anOffset;

  while(first<=last) {
    *to=*first;  
    to++;
    first++;
  }
}

/**
 * This method shifts single byte characters right by a given amount from an given offset.
 */
void ShiftCharsRight(char* aDest,TDINT aLength,TDINT anOffset,TDINT aCount) { 
  char* last = aDest+aLength;
  char* first= aDest+anOffset-1;
  char* to = aDest+aLength+aCount;

  while(first<=last) {
    *to=*last;  
    to--;
    last--;


  }
}

typedef void (*ShiftChars)(char* aDest,TDINT aLength,TDINT anOffset,TDINT aCount);
ShiftChars gShiftChars[2]=  {&ShiftCharsLeft,&ShiftCharsRight};

/**
 * Going 1 to 1 is easy, since we assume ascii. No conversions are necessary.
 */
void CopyChars(char* aDest,TDINT anDestOffset,const char* aSource,TDINT anOffset,TDINT aCount) { 

  char*       to   = aDest+anDestOffset;
  const char* first= aSource+anOffset;
  const char* last = first+aCount;

    while(first<last) {
    *to=*first;  
    to++;
    first++;
  }
} 


/**
 *  This methods cans the given buffer for the given char
 */
 TDINT FindChar(const char* aDest,TDINT aLength,TDINT anOffset,const TDCHAR aChar,TDBOOL aIgnoreCase) {
  TDCHAR theCmpChar=(aIgnoreCase ? toupper(aChar) : aChar);
  TDINT  theIndex=0;
  TDINT  theLength=(TDINT)aLength;
  for(theIndex=(TDINT)anOffset;theIndex<theLength;theIndex++){
    TDCHAR theChar=aDest[theIndex];
    if(aIgnoreCase)
      theChar=toupper(theChar);
    if(theChar==theCmpChar)
      return theIndex;
  }
  return kNotFound;
}


/**
 *  This methods cans the given buffer (in reverse) for the given char
 */
 TDINT RFindChar(const char* aDest,TDINT aLength,TDINT anOffset,const TDCHAR aChar,TDBOOL aIgnoreCase) {
  TDCHAR theCmpChar=(aIgnoreCase ? toupper(aChar) : aChar);
  TDINT theIndex=0;
  TDINT thePos=(TDINT)aLength-anOffset-1;
  for(theIndex=thePos;theIndex>=0;theIndex--){
    TDCHAR theChar=aDest[theIndex];
    if(aIgnoreCase)
      theChar=toupper(theChar);
    if(theChar==theCmpChar)
      return theIndex;
  }
  return kNotFound;
}

/**
 * This method compares the data in one buffer with another
 */
TDINT Compare(const char* aStr1,const char* aStr2,TDINT aCount,TDBOOL aIgnoreCase){ 
  TDINT result=0;
  if(aIgnoreCase)
    result=strnicmp(aStr1,aStr2,aCount);    //   should be "strnicmp"
  else 
	  result=strncmp(aStr1,aStr2,aCount);
  return result;
}




/**
 * This method performs a case conversion the data in the given buffer 
 */
TDM_COMSERVAPI TDINT ConvertCase(char* aString,TDINT aCount,TDBOOL atoupper)
{ 
  TDINT result=0;

  typedef char  chartype;
  chartype* cp = (chartype*)aString;
  chartype* end = cp + aCount-1;
  while (cp <= end) {
    chartype ch = *cp;
    if(atoupper) {
      if ((ch >= 'a') && (ch <= 'z')) {
        *cp = 'A' + (ch - 'a');
      }
    }
    else {
      if ((ch >= 'A') && (ch <= 'Z')) {
        *cp = 'a' + (ch - 'A');
      }
    }
    cp++;
  }
  return result;
}

/**
 * This method removes chars (given in aSet) from the given buffer 
 */
TDINT StripChars(char* aString,TDINT anOffset,TDINT aCount,const char* aSet){ 
  TDINT result=0;

  typedef char  chartype;
  chartype*  from = (chartype*)&aString[anOffset];
  chartype*  end  = (chartype*)from + aCount-1;
  chartype*  to   = from;

  if(aSet){
    TDINT aSetLen=TDStrLen(aSet);
    while (from <= end) {
      chartype ch = *from;
      if(kNotFound==FindChar(aSet,aSetLen,0,ch,TDFALSE)){
        *to++=*from;

      }
      from++;
    }
    *to = 0;
  }
  return to - (chartype*)aString;
}



/**
 * This method trims chars (given in aSet) from the edges of given buffer 
 */
TDINT TrimChars(char* aString,TDINT aLength,const char* aSet,TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing){ 
  TDINT result=0;

  typedef char  chartype;
  chartype*  from = (chartype*)aString;
  chartype*  end =  from + aLength -1;
  chartype*  to = from;

  if(aSet) {
    TDINT aSetLen=TDStrLen(aSet);
          if(aEliminateLeading) {
      while (from <= end) {
        chartype ch = *from;
        if(kNotFound==FindChar(aSet,aSetLen,0,ch,TDFALSE)){
          break;
        }
        from++;
      }
    }
    if(aEliminateTrailing) {
      while(from<=end) {
        chartype ch = *end;
        if(kNotFound==FindChar(aSet,aSetLen,0,ch,TDFALSE)){
          break;
        }
        end--;
      }
    }
    if (from != to) {
      while (from <= end) {
        *to++ = *from++;
      }
    }
    else {
      to = ++end;
    }

    *to = 0;
  }
  return to - (chartype*)aString;
}

/**
 * This method compresses duplicate runs of a given char from the given buffer 
 */
  typedef char  chartype;
TDINT CompressChars(char* aString,TDINT aLength,const char* aSet,TDINT aChar,TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing)
{ 
  TDINT result=0;
  chartype*  from = aString;
  chartype*  end =  aString + aLength-1;
  chartype*  to = from;

 aLength=TrimChars(aString,aLength,aSet,aEliminateLeading,aEliminateTrailing);
 end =  aString + aLength-1;

  if(aSet){
    TDINT aSetLen=TDStrLen(aSet);
    while (from <= end) {
      chartype ch = *from++;
      if(kNotFound!=FindChar(aSet,aSetLen,0,ch,TDFALSE)){
        *to++ = (char)aChar;
        while (from <= end) {
          ch = *from++;
          if(kNotFound==FindChar(aSet,aSetLen,0,ch,TDFALSE)){
            *to++ = ch;
            break;
          }
        }
      } else {
        *to++ = ch;
      }
    }
    *to = 0;
  }
  return to - (chartype*)aString;
}

/**************************************************************
  A couple of tiny helper methods used in the string classes.
 **************************************************************/

 TDINT MinInt(TDINT anInt1,TDINT anInt2){
  return (anInt1<anInt2) ? anInt1 : anInt2;
}

TDINT MaxInt(TDINT anInt1,TDINT anInt2){
  return (anInt1<anInt2) ? anInt2 : anInt1;
}

void AddNullTerminator(TDStr* aDest) {
    aDest->mStr[aDest->mLength]=0;
}

char* GetSharedEmptyBuffer() {
  if(!gCommonEmptyBuffer) {
    const size_t theDfltSize=5;
    gCommonEmptyBuffer=(TDCHAR *)Malloc(theDfltSize);
    if(gCommonEmptyBuffer){
      gCommonEmptyBuffer[0]=0;
    }
    else {
    }
  }
  return gCommonEmptyBuffer;
}

/** */
void TDStr_Initialize1(TDStr* aDest) 
{
  aDest->mStr=GetSharedEmptyBuffer();
  aDest->mLength=0;
  aDest->mCapacity=0;
}
//szj
void TDStr_Initialize3(TDStr* aDest) 
{
  aDest->mLength=0;
  aDest->mCapacity=0;
}

/** */
void TDStr_Initialize2(TDStr* aDest,char* aCString,TDINT aCapacity,TDINT aLength,TDBOOL aOwnsBuffer)
{
  aDest->mStr=(aCString) ? aCString : GetSharedEmptyBuffer();
  aDest->mLength=aLength;
  aDest->mCapacity=aCapacity;
}

void TDStr_Destroy(TDStr* aDest) 
{
  if((aDest->mStr) && (aDest->mStr!=GetSharedEmptyBuffer())) 
	  TDFree(aDest->mStr);
  aDest->mStr=GetSharedEmptyBuffer();
  aDest->mCapacity=0;
  aDest->mLength=0;
}


/**
* This method is used to access a given char in the given string
*/
/*
TDCHAR TDStr_GetCharAt(const TDStr* aDest,TDINT anIndex)
{
  if(anIndex<aDest->mLength)  
  {
		return  aDest->mStr[anIndex] ;
  }
  return 0;
}
*/
/**
 * This method gets called when the internal buffer needs
 * to grow to a given size. The original contents are not preserved.
 */
void TDStr_EnsureCapacity(TDStr* aString,TDINT aNewLength) 
{
  if(aNewLength>aString->mCapacity) 
  {
	  char *ptr=TDNULL;
	  if(aString->mStr == GetSharedEmptyBuffer())
	  {
		  ptr=(char*)Malloc(aNewLength + 1);
		  if(!ptr)
			  return;
		  memset(ptr,0,aNewLength);
		  aString->mStr=ptr;
		  aString->mCapacity=aNewLength;
	  }
	  else
	  {
		  ptr=(char*)Malloc(aNewLength + 1);
		  if(!ptr)
			  return;
		 memcpy(ptr,aString->mStr,aString->mLength);
		 TDFree(aString->mStr);
		 aString->mStr=ptr;
		 aString->mCapacity=aNewLength;
		 AddNullTerminator(aString);
	  }
  }
}

/**
 * This method gets called when the internal buffer needs
 * to grow to a given size. The original contents ARE preserved.
 */
void TDStr_GrowCapacity(TDStr* aDest,TDINT aNewLength) 
{
  if(aNewLength>aDest->mCapacity) 
    TDStr_EnsureCapacity(aDest,aNewLength);
}

/**
 * Replaces the contents of aDest with aSource, up to aCount of chars.
 */
void TDStr_AssignStr(TDStr* aDest,const TDStr* aSource,TDINT anOffset,TDINT aCount)
{
  if(aDest!=aSource){
    TDStr_Truncate(aDest,0);
    TDStr_AppendStr(aDest,aSource,anOffset,aCount);
  }
}

void TDStr_AssignChar(TDStr* aDest,const char* aSource)
{
  if(aDest&&aSource){
    TDStr_Truncate(aDest,0);
    TDStr_AppendChar(aDest,aSource);
  }
}
/**
 * This method appends the given TDStr to this one. Note that we have to 
 * pay attention to the underlying char-size of both structs.
 */
void TDStr_AppendStr(TDStr* aDest,const TDStr* aSource,TDINT anOffset,TDINT aCount)
{
  if(anOffset<aSource->mLength){
    TDINT theRealLen=(aCount<0) ? aSource->mLength : MinInt(aCount,aSource->mLength);
    TDINT theLength=(anOffset+theRealLen<aSource->mLength) ? theRealLen : (aSource->mLength-anOffset);
    if(0<theLength)

	{
      if(aDest->mLength+theLength > aDest->mCapacity) 
	  {
        TDStr_GrowCapacity(aDest,aDest->mLength+theLength);
	  }
      CopyChars(aDest->mStr,aDest->mLength,aSource->mStr,anOffset,theLength);
      aDest->mLength+=theLength;
    }
  }
  AddNullTerminator(aDest);
}
void TDStr_AppendChar(TDStr* aDest,const char* aSource )
{
  if(aSource && 0<TDStrLen(aSource))
  {
    TDINT theRealLen=TDStrLen(aSource);
      if(aDest->mLength+theRealLen > aDest->mCapacity) 
	  {
        TDStr_GrowCapacity(aDest,aDest->mLength+theRealLen);
      }
      CopyChars(aDest->mStr,aDest->mLength,aSource,0,theRealLen);

      aDest->mLength+=theRealLen;
  }
  AddNullTerminator(aDest);
}


/**
 * This method inserts up to "aCount" chars from a source TDStr into a dest TDStr.
 */
void TDStr_Insert( TDStr* aDest,TDINT aDestOffset,const TDStr* aSource,TDINT aSrcOffset,TDINT aCount)
{
  if(0<aSource->mLength){
    if(aDest->mLength){
      if(aDestOffset<aDest->mLength){
        TDINT theRealLen=(aCount<0) ? aSource->mLength : MinInt(aCount,aSource->mLength);
        TDINT theLength=(aSrcOffset+theRealLen<aSource->mLength) ? theRealLen : (aSource->mLength-aSrcOffset);

        if(aSrcOffset<aSource->mLength) {
          TDStr_GrowCapacity(aDest,aDest->mLength+theLength);

          ShiftCharsRight(aDest->mStr,aDest->mLength,aDestOffset,theLength);
      
          CopyChars(aDest->mStr,aDestOffset,aSource->mStr,aSrcOffset,theLength);

          aDest->mLength+=theLength;
          AddNullTerminator(aDest);

        }
        /*else nothing to do!*/
      }
      else TDStr_AppendStr(aDest,aSource,0,aCount);
    }
    else TDStr_AppendStr(aDest,aSource,0,aCount);
  }
}


/**
 * This method deletes up to aCount chars from aDest
 */
void TDStr_Delete(TDStr* aDest,TDINT aDestOffset,TDINT aCount )
{
  if(aDestOffset<aDest->mLength){

    TDINT theDelta=aDest->mLength-aDestOffset;
    TDINT theLength=(theDelta<aCount) ? theDelta : aCount;

    if(aDestOffset+theLength<aDest->mLength) {

      ShiftCharsLeft(aDest->mStr,aDest->mLength,aDestOffset,theLength);
      aDest->mLength-=theLength;
    }
    else TDStr_Truncate(aDest,aDestOffset);
  }
}

/**
 * This method truncates the given TDStr at given offset
 */
void TDStr_Truncate(TDStr* aDest,TDINT aDestOffset)
{
  if(aDestOffset<aDest->mLength){
    aDest->mLength=aDestOffset;
    AddNullTerminator(aDest);
  }
}


/**
 * 
 * @update	gess1/7/99
 * @param 
 * @return
 */
void TDStr_ChangeCase(TDStr* aDest,TDBOOL atoupper) {
  ConvertCase(aDest->mStr,aDest->mLength,atoupper);
}

/***/
void TDStr_StripChars(TDStr* aDest,TDINT aDestOffset,TDINT aCount,const char* aCharSet){
  TDINT aNewLen=StripChars(aDest->mStr,aDestOffset,aCount,aCharSet);
  aDest->mLength=aNewLen;
}

/***/
void TDStr_Trim(TDStr* aDest,const char* aSet,TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing)
{
  TDINT aNewLen=TrimChars(aDest->mStr,aDest->mLength,aSet,aEliminateLeading,aEliminateTrailing);
  aDest->mLength=aNewLen;
}

/***/
void TDStr_CompressSet(TDStr* aDest,const char* aSet,TDINT aChar,TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing){
  TDINT aNewLen=CompressChars(aDest->mStr,aDest->mLength,aSet,aChar,aEliminateLeading,aEliminateTrailing);
  aDest->mLength=aNewLen;
}

  /**************************************************************
    Searching methods...
   **************************************************************/


TDINT TDStr_FindSubstr(const TDStr* aDest,const TDStr* aTarget, TDBOOL aIgnoreCase,TDINT anOffset) 
{
  if((aDest->mLength>0) && (aTarget->mLength>0) && (anOffset<aDest->mLength)){

    TDINT index=anOffset-1;
    TDINT theMax=aDest->mLength-aTarget->mLength;
    if((aDest->mLength>0) && (aTarget->mLength>0)){
      TDINT theTargetMax=aTarget->mLength;
      while(++index<=theMax) {
        TDINT theSubIndex=-1;
        TDBOOL  matches=TDTRUE;
        while((++theSubIndex<theTargetMax) && (matches)){
          TDCHAR theChar=(aIgnoreCase) ? tolower(TDStr_GetCharAt(aDest,index+theSubIndex)) : TDStr_GetCharAt(aDest,index+theSubIndex);
          TDCHAR theTargetChar=(aIgnoreCase) ? tolower(TDStr_GetCharAt(aTarget,theSubIndex)) : TDStr_GetCharAt(aTarget,theSubIndex);
          matches=(TDBOOL)(theChar==theTargetChar);
        }
        if(matches) { 
          return index;
        }
      }
    }
  }
  return kNotFound;
}

/***/
TDINT TDStr_FindChar(const TDStr* aDest,TDCHAR aChar, TDBOOL aIgnoreCase,TDINT anOffset) {

  TDINT result=FindChar(aDest->mStr,aDest->mLength,anOffset,aChar,aIgnoreCase);
  return result;
}


/***/
TDINT TDStr_FindCharInSet(const TDStr* aDest,const TDStr* aSet,TDBOOL aIgnoreCase,TDINT anOffset) 
{
  TDINT index=anOffset-1;
  TDINT  thePos;

  while(++index<aDest->mLength) {
    TDCHAR theChar=TDStr_GetCharAt(aDest,index);
    thePos=FindChar(aSet->mStr,aSet->mLength,0,theChar,aIgnoreCase);
    if(kNotFound!=thePos)
      return index;
  } 
  return kNotFound;
}

  /**************************************************************
    Reverse Searching methods...
   **************************************************************/


TDINT TDStr_RFindSubstr(const TDStr* aDest,const TDStr* aTarget, TDBOOL aIgnoreCase,TDINT anOffset) {
  TDINT index=(anOffset ? anOffset : aDest->mLength-aTarget->mLength+1);
  TDINT result=kNotFound;

  if((aDest->mLength>0) && (aTarget->mLength>0)){

    TDStr theCopy;
	TDINT   theTargetMax;
    TDStr_Initialize1(&theCopy);
    TDStr_AssignStr(&theCopy,aTarget,0,aTarget->mLength);
    if(aIgnoreCase){
      TDStr_ChangeCase(&theCopy,TDFALSE); 
    }
    
    theTargetMax=theCopy.mLength;
    while(index--) {
      TDINT theSubIndex=-1;
      TDBOOL  matches=TDTRUE;
      if(anOffset+theCopy.mLength<=aDest->mLength) {
        while((++theSubIndex<theTargetMax) && (matches)){
          TDCHAR theDestChar=(aIgnoreCase) ? tolower(TDStr_GetCharAt(aDest,index+theSubIndex)) : TDStr_GetCharAt(aDest,index+theSubIndex);
          TDCHAR theTargetChar=TDStr_GetCharAt(&theCopy,theSubIndex);
          matches=(TDBOOL)(theDestChar==theTargetChar);
        } 
      } 
      if(matches) {
        result=index;
        break;
      }
    } 
    TDStr_Destroy(&theCopy);
  }
  return result;
}
 

/***/
TDINT TDStr_RFindChar(const TDStr* aDest,TDCHAR aChar, TDBOOL aIgnoreCase,TDINT anOffset) {
  TDINT result=RFindChar(aDest->mStr,aDest->mLength,anOffset,aChar,aIgnoreCase);
  return result;
}

/***/
TDINT TDStr_RFindCharInSet(const TDStr* aDest,const TDStr* aSet,TDBOOL aIgnoreCase,TDINT anOffset) {
  TDINT offset=aDest->mLength-anOffset;
  TDINT  thePos;

  while(--offset>=0) {
    TDCHAR theChar=TDStr_GetCharAt(aDest,offset);
    thePos=RFindChar(aSet->mStr,aSet->mLength,0,theChar,aIgnoreCase);
    if(kNotFound!=thePos)
      return offset;
  } 
  return kNotFound;
}


/***/
TDINT TDStr_Compare(const TDStr* aDest,const TDStr* aSource,TDINT aCount,TDBOOL aIgnoreCase) 
{
  int minlen=(aSource->mLength<aDest->mLength) ? aSource->mLength : aDest->mLength;
int maxlen;
TDINT result;
  if(0==minlen) {
    if ((aDest->mLength == 0) && (aSource->mLength == 0))
      return 0;
    if (aDest->mLength == 0)
      return -1;
    return 1;
  }

  maxlen=(aSource->mLength<aDest->mLength) ? aDest->mLength : aSource->mLength;
   result=Compare(aDest->mStr,aSource->mStr,maxlen,aIgnoreCase);
  return result;
}


TDM_COMSERVAPI
TDBOOL TDstrcpy(char *t,char *s)
{
	
	while(*s)
	{
		*t=*s;
		s++;
		t++;
	}
	*t='\0';
	return TDTRUE;
}

void TDStr_InvertedOrder(TDStr* aDest,const TDStr* aSource )
{
    TDINT  iLoop = 0;	
	TDString_Truncate(aDest, 0);
	if(aSource && 0< aSource->mLength)
	{
		TDINT theRealLen = aSource->mLength;
		if(theRealLen > aDest->mCapacity)//如果目标长度比源目标长度小。则改变内存大小
		{
			TDStr_GrowCapacity(aDest,theRealLen);
		}
		for (iLoop = theRealLen-1; iLoop >= 0; iLoop --)
		{
           if (aSource->mStr[iLoop] & 0x80)
           {
			   aDest->mStr[theRealLen - iLoop - 1] = aSource->mStr[iLoop -1];
			   aDest->mStr[theRealLen - iLoop] = aSource->mStr[iLoop];
			   iLoop --;
           }
		   else
		   {
			   aDest->mStr[theRealLen - iLoop - 1] = aSource->mStr[iLoop];
		   }
		}
		aDest->mStr[theRealLen] = '\0';
		aDest->mLength = theRealLen;
	}
	AddNullTerminator(aDest);
}