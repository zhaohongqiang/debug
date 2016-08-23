
#ifndef _TDStr
#define _TDStr
//#include "comserv.h"

TDM_COMSERVAPI TDDWORD	TDStrLen(const TDCHAR *str);
TDM_COMSERVAPI TDINT	ConvertCase(char* aString,TDINT aCount,TDBOOL atoupper);
/******************************************************/

typedef enum  _eCharSize 
{
	eOneByte=1,
	eTwoByte=2
}eCharSize;
#define kDefaultCharSize eTwoByte
#define kNotFound  -1

typedef struct _TDStr {  
  TDINT			mLength;
  TDINT			mCapacity;
  TDCHAR*		mStr;
}TDStr,*TDPStr;

TDINT	MinInt(TDINT anInt1,TDINT anInt2);
TDINT	MaxInt(TDINT anInt1,TDINT anInt2);
void	TDStr_Initialize1(TDStr* aDest);
void	TDStr_Initialize3(TDStr* aDest);//szj
void	TDStr_Initialize2(TDStr* aDest,char* aCString,TDINT aCapacity,TDINT aLength,TDBOOL aOwnsBuffer);
void	TDStr_Destroy(TDStr* aDest); 
void	TDStr_EnsureCapacity(TDStr* aString,TDINT aNewLength);
void	TDStr_GrowCapacity(TDStr* aString,TDINT aNewLength);
void	TDStr_AppendStr(TDStr* aDest,const TDStr* aSource,TDINT anOffset,TDINT aCount);
void	TDStr_AppendChar(TDStr* aDest,const char* aSource );
void	TDStr_AssignStr(TDStr* aDest,const TDStr* aSource,TDINT anOffset,TDINT aCount);
void	TDStr_AssignChar(TDStr* aDest,const char* aSource);
void	TDStr_Insert( TDStr* aDest,TDINT aDestOffset,const TDStr* aSource,TDINT aSrcOffset,TDINT aCount);
void	TDStr_Delete(TDStr* aDest,TDINT aDestOffset,TDINT aCount);
void	TDStr_Truncate(TDStr* aDest,TDINT aDestOffset);
void	TDStr_ChangeCase(TDStr* aDest,TDBOOL aToUpper);
void	TDStr_StripChars(TDStr* aDest,TDINT aDestOffset,TDINT aCount,const char* aCharSet);
void	TDStr_Trim(TDStr* aDest,const char* aSet,TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing);
void	TDStr_CompressSet(TDStr* aDest,const char* aSet,TDINT aChar,TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing);
TDINT	TDStr_Compare(const TDStr* aDest,const TDStr* aSource,TDINT aCount,TDBOOL aIgnoreCase);
TDINT	TDStr_FindSubstr(const TDStr* aDest,const TDStr* aSource, TDBOOL aIgnoreCase,TDINT anOffset);
TDINT	TDStr_FindChar(const TDStr* aDest,TDCHAR aChar, TDBOOL aIgnoreCase,TDINT anOffset);
TDINT	TDStr_FindCharInSet(const TDStr* aDest,const TDStr* aSet,TDBOOL aIgnoreCase,TDINT anOffset);
TDINT	TDStr_RFindSubstr(const TDStr* aDest,const TDStr* aSource, TDBOOL aIgnoreCase,TDINT anOffset);
TDINT	TDStr_RFindChar(const TDStr* aDest,TDCHAR aChar, TDBOOL aIgnoreCase,TDINT anOffset);
TDINT	TDStr_RFindCharInSet(const TDStr* aDest,const TDStr* aSet,TDBOOL aIgnoreCase,TDINT anOffset);

void	TDStr_InvertedOrder(TDStr* aDest,const TDStr* aSource );
//TDCHAR	TDStr_GetCharAt(const TDStr* aDest,TDINT anIndex);
#define TDStr_GetCharAt(pThis,anIndex)   ((pThis)?(((anIndex)>=(pThis)->mLength)?0:(pThis)->mStr[(anIndex)]):0)
TDM_COMSERVAPI TDBOOL TDstrcpy(char *t,char *s);

void AddNullTerminator(TDStr* aDest);

#endif

