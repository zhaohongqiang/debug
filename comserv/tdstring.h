
#ifndef _tdString
#define _tdString

#include "comserv.h"

#define kRadix10        (10)
#define kRadix16        (16)
#define kAutoDetect     (100)
#define kRadixUnknown   (kAutoDetect+1)

typedef TDStr  TDString;
typedef TDPStr TDPString;
/*
#define TDString TDStr   
#define TDPString TDPStr  	
*/
#ifdef		TDM_SI_CPLUSPLUS
extern	"C"{
#endif		/*TDM_SI_CPLUSPLUS*/
TDM_COMSERVAPI	TDPString	TDString_Create			();
TDM_COMSERVAPI	TDBOOL	TDString_Destroy		(TDPString pThis);

TDM_COMSERVAPI	void		TDString_Subsume		(TDPString aDest,TDPString aSource);
TDM_COMSERVAPI	void		TDString_Init			(TDPString pThis);
TDM_COMSERVAPI	void		TDString_InitByChar		(TDPString pThis,const char* aCString);
TDM_COMSERVAPI	void		TDString_InitByStr		(TDPString pThis,const TDStr *aString ) ;
TDM_COMSERVAPI	void		TDString_InitByString	(TDPString pThis,const TDPString aString) ;
TDM_COMSERVAPI	void		TDString_DTDString		(TDPString pThis) ;
TDM_COMSERVAPI	void		TDString_Truncate		(TDPString pThis,TDINT anIndex);
TDM_COMSERVAPI	TDBOOL		TDString_IsOrdered		(TDPString pThis) ;
TDM_COMSERVAPI	void		TDString_SetCapacity	(TDPString pThis,TDINT aLength) ;
TDM_COMSERVAPI	char*		TDString_GetBuffer		(TDPString pThis) ;
TDM_COMSERVAPI	TDCHAR		TDString_CharAt			(TDPString pThis,TDINT anIndex) ;
TDM_COMSERVAPI	TDCHAR		TDString_First			(TDPString pThis) ;
TDM_COMSERVAPI	TDCHAR		TDString_Last			(TDPString pThis) ;
TDM_COMSERVAPI	TDBOOL		TDString_SetCharAt		(TDPString pThis,TDCHAR aChar,TDINT anIndex);
TDM_COMSERVAPI	void		TDString_ToLowerCase	(TDPString pThis);
TDM_COMSERVAPI	void		TDString_ToUpperCase	(TDPString pThis) ;
TDM_COMSERVAPI	TDPString	TDString_StripChars		(TDPString pThis,const char* aSet);
TDM_COMSERVAPI	TDPString	TDString_StripWhitespace(TDPString pThis) ;
TDM_COMSERVAPI	TDPString	TDString_ReplaceChar	(TDPString pThis,TDCHAR aSourceChar, TDCHAR aDestChar) ;
TDM_COMSERVAPI	TDPString	TDString_Trim			(TDPString pThis,const char* aTrimSet, TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing);
TDM_COMSERVAPI	TDPString	TDString_CompressSet	(TDPString pThis,const char* aSet, char aChar, TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing);
TDM_COMSERVAPI	TDPString	TDString_CompressWhitespace(TDPString pThis, TDBOOL aEliminateLeading,TDBOOL aEliminateTrailing);
TDM_COMSERVAPI	char*		TDString_ToNewCString	(TDPString pThis) ;
TDM_COMSERVAPI	char*		TDString_ToCString		(TDPString pThis,char* aBuf, TDINT aBufLength,TDINT anOffset) ;
TDM_COMSERVAPI	float		TDString_ToFloat		(TDPString pThis,TDINT* aErrorCode) ;
TDM_COMSERVAPI	TDINT		TDString_ToInteger		(TDPString pThis,TDINT* anErrorCode,TDINT aRadix);
TDM_COMSERVAPI	TDPString	TDString_Assign			(TDPString  pThis,const TDStr* aString,TDINT aCount) ;
TDM_COMSERVAPI	TDPString	TDString_AssignString	(TDPString  pThis,TDPString aString,TDINT aCount) ;
TDM_COMSERVAPI	TDPString	TDString_AssignPChar	(TDPString  pThis,const char* aCString) ;
TDM_COMSERVAPI	TDPString	TDString_AssignPCharN	(TDPString  pThis,const char* aCString,TDINT nCount) ;
TDM_COMSERVAPI	TDPString	TDString_AssignChar		(TDPString  pThis,char aChar) ;
TDM_COMSERVAPI	TDPString	TDString_AppendStr		(TDPString  pThis,const TDStr* aString,TDINT aCount) ;
TDM_COMSERVAPI	TDPString	TDString_AppendString	(TDPString  pThis,const TDPString aString,TDINT aCount) ;
TDM_COMSERVAPI	TDPString	TDString_AppendPChar	(TDPString  pThis,const char* aCString) ;
TDM_COMSERVAPI	TDPString   TDString_AppendPCharN	(TDPString  pThis,const char* aCString,TDINT nCount);
TDM_COMSERVAPI	TDPString	TDString_AppendChar		(TDPString  pThis,char aChar);
TDM_COMSERVAPI	TDPString	TDString_AppendInt		(TDPString  pThis,TDINT aInteger,TDINT aRadix) ;
TDM_COMSERVAPI	TDPString	TDString_AppendFload	(TDPString  pThis,float aFloat);
TDM_COMSERVAPI	TDINT		TDString_Left			(TDPString  pThis,TDPString aDest,TDINT aCount) ;
TDM_COMSERVAPI	TDINT		TDString_Mid			(TDPString  pThis,TDPString aDest,TDINT anOffset,TDINT aCount);
TDM_COMSERVAPI	TDINT		TDString_Right			(TDPString  pThis,TDPString aCopy,TDINT aCount) ;
TDM_COMSERVAPI	TDPString	TDString_Insert			(TDPString  pThis,const TDPString aCopy,TDINT anOffset,TDINT aCount) ;
TDM_COMSERVAPI	TDPString	TDString_InsertChar		(TDPString  pThis,TDCHAR aChar,TDINT anOffset);
TDM_COMSERVAPI	TDPString	TDString_InsertPChar	(TDPString  pThis,TDCHAR *pChar,TDINT anOffset,TDINT aCount);
TDM_COMSERVAPI	TDPString	TDString_Cut			(TDPString  pThis,TDINT anOffset, TDINT aCount);
TDM_COMSERVAPI	TDINT		TDString_BinarySearch	(TDPString  pThis,TDCHAR aChar) ;
TDM_COMSERVAPI	TDINT		TDString_FindPChar		(TDPString  pThis,const char* aCString,TDBOOL aIgnoreCase) ;
TDM_COMSERVAPI	TDINT		TDString_FindStr		(TDPString  pThis,const TDStr* aString,TDBOOL aIgnoreCase) ;
TDM_COMSERVAPI	TDINT		TDString_FindString		(TDPString  pThis,const TDPString aString,TDBOOL aIgnoreCase) ;
TDM_COMSERVAPI	TDINT		TDString_FindChar		(TDPString  pThis,TDCHAR aChar,TDINT anOffset,TDBOOL aIgnoreCase);
TDM_COMSERVAPI	TDINT		TDString_FindPCharFrom	(TDPString  pThis,const char* aCString,TDBOOL aIgnoreCase,TDINT aOffset);
TDM_COMSERVAPI	TDINT		TDString_FindCharInSet	(TDPString  pThis,const char* aCStringSet,TDINT anOffset) ;
TDM_COMSERVAPI	TDINT		TDString_FindStringInSet(TDPString  pThis,const TDPString aSet,TDINT anOffset);
TDM_COMSERVAPI	TDINT		TDString_RFindCharInSet	(TDPString  pThis,const char* aCStringSet,TDINT anOffset) ;
TDM_COMSERVAPI	TDINT		TDString_RFindStringInSet(TDPString  pThis,const TDPString aSet,TDINT anOffset);
TDM_COMSERVAPI	TDINT		TDString_RFindStr		(TDPString  pThis,const TDStr* aString,TDBOOL aIgnoreCase);
TDM_COMSERVAPI	TDINT		TDString_RFindString	(TDPString  pThis,const TDPString aString,TDBOOL aIgnoreCase) ;
TDM_COMSERVAPI	TDINT		TDString_RFindChar		(TDPString  pThis,TDCHAR aChar,TDINT anOffset,TDBOOL aIgnoreCase);
TDM_COMSERVAPI	TDINT		TDString_CompareString	(TDPString  pThis,const TDPString aString,TDBOOL aIgnoreCase,TDINT aLength) ;
TDM_COMSERVAPI	TDINT		TDString_ComparePChar	(TDPString  pThis,const char *aCString,TDBOOL aIgnoreCase,TDINT aLength) ;
TDM_COMSERVAPI	TDINT		TDString_CompareStr		(TDPString  pThis,const TDStr* aString,TDBOOL aIgnoreCase,TDINT aLength);
TDM_COMSERVAPI	TDBOOL		TDString_EqIgnCaseString(TDPString  pThis,const TDPString aString);
TDM_COMSERVAPI	TDBOOL		TDString_EqIgnCasePChar	(TDPString  pThis,const char* aString,TDINT aLength) ;
TDM_COMSERVAPI	TDBOOL		TDString_EqString		(TDPString  pThis,const TDPString aString,TDBOOL aIgnoreCase) ;
TDM_COMSERVAPI 	TDBOOL		TDString_EqStr			(TDPString  pThis,const TDStr* aString,TDBOOL aIgnoreCase) ;
TDM_COMSERVAPI	TDBOOL		TDString_EqPChar		(TDPString  pThis,const char* aString,TDBOOL aIgnoreCase);
TDM_COMSERVAPI	TDBOOL		TDString_IsAlpha		(TDCHAR aChar);
TDM_COMSERVAPI	TDBOOL		TDString_IsSpace		(TDCHAR aChar);
TDM_COMSERVAPI 	TDBOOL		TDString_IsDigit		(TDCHAR aChar);

TDM_COMSERVAPI	TDBOOL		TDString_GetRect		(TDPString pThis,TDPRECT pRect);
TDM_COMSERVAPI	TDPRECT		TDString_ToRect			(TDPString pThis);
TDM_COMSERVAPI	TDPPOLYGON	TDString_ToPoly			(TDPString pThis);
TDM_COMSERVAPI	TDPCIRCLE	TDString_ToCircle		(TDPString pThis);
TDM_COMSERVAPI	TDPSIZE		TDString_ToSize			(TDPString pThis);
TDM_COMSERVAPI	TDRGB		TDString_ToColor		(TDPString pThis);

TDM_COMSERVAPI	TDBOOL		TDString_AppendRect			(TDPString pThis,TDPRECT pRect);
TDM_COMSERVAPI	TDBOOL		TDString_AppendPoly			(TDPString pThis,TDPPOLYGON pPoly);
TDM_COMSERVAPI	TDBOOL		TDString_AppendCircle		(TDPString pThis,TDPCIRCLE pCircle);
TDM_COMSERVAPI	TDBOOL		TDString_AppendSize			(TDPString pThis,TDPSIZE pSize);
TDM_COMSERVAPI	TDBOOL		TDString_AppendColor		(TDPString pThis,TDRGB pRGB);

TDM_COMSERVAPI	TDPWORD		TDString_GetUChar	(TDPString  pThis);
TDM_COMSERVAPI	TDPWORD		TDString_PCharToPUChar	(TDCHAR * aSrc,TDINT len);
TDM_COMSERVAPI	TDCHAR*		TDString_PUCharToPChar	(TDWORD * aSrc,TDINT len);
TDM_COMSERVAPI	TDWORD		TDString_CharToUChar	(TDCHAR  aSrc);
TDM_COMSERVAPI	TDCHAR		TDString_UCharToChar	(TDWORD  aSrc);
TDM_COMSERVAPI	TDINT		TDString_GetIntegerFrom(TDPString pThis,TDINT offset,TDPINT num);

#ifdef		TDM_SI_CPLUSPLUS
}
#endif		/*TDM_SI_CPLUSPLUS*/

#endif


