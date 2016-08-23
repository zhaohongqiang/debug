#ifndef		TDM_SI_INCLUDED_COMTYPE_H
#define		TDM_SI_INCLUDED_COMTYPE_H

#include	"comserv.h"


#ifdef		TDM_SI_CPLUSPLUS
extern "C" {
#endif		/*TDM_SI_CPLUSPLUS*/

#define		TDM_BETWEEN(a,b1,b2)		((a >= TDM_MINVAL(b1,b2)) && (a <= TDM_MAXVAL(b1,b2)))
//#define		TDM_PTEQUAL(pt1,pt2)		((((TDPOINT)pt1).x == pt2.x) && (((TDPOINT)pt1).y == pt2.y))

//#define		TDM_RECTWIDTH(rect)			TDM_ABS(((rect).right - (rect).left))
//#define		TDM_RECTHEIGHT(rect)		TDM_ABS(((rect).top - (rect).bottom))



TDM_COMSERVAPI	TDBOOL	TDF_PtEqual(TDPOINT pt1,TDPOINT pt2);
TDM_COMSERVAPI	TDBOOL	TDF_PtInPolygon(TDPOINT pt,TDPOLYGON *pPoly);

TDM_COMSERVAPI	TDVOID  TDNormalRect(TDPRECT pRect);
TDM_COMSERVAPI	TDBOOL	TDIntersectRect(TDPRECT rc1,TDPRECT rc2,TDPRECT rt);
TDM_COMSERVAPI	TDVOID	TDF_SetRect(TDconst TDPRECT rc1,TDPRECT rc2);
TDM_COMSERVAPI	TDVOID	TDCopyRect(TDPRECT dst,TDconst TDPRECT src);
TDM_COMSERVAPI	TDVOID	TDOffsetRect(TDPRECT r,TDINT dx,TDINT dy);
TDM_COMSERVAPI	TDBOOL	TDPtInRect(TDPRECT rt,TDPOINT pt);


TDM_COMSERVAPI	TDPCIRCLE	TDCircleCreate();
TDM_COMSERVAPI	TDBOOL		TDCircleDestroy(TDPCIRCLE pThis);
TDM_COMSERVAPI	TDPAREA		TDAreaCreate();
TDM_COMSERVAPI	TDBOOL		TDAreaDestroy(TDPAREA pThis);
TDM_COMSERVAPI	TDPRECT		TDRectCreate();
TDM_COMSERVAPI	TDBOOL		TDRectDestroy(TDPRECT pThis);
TDM_COMSERVAPI	TDVOID		TDRectSet(TDPRECT pThis, LONG left, LONG top, LONG right, LONG bottom);

TDM_COMSERVAPI	TDPPOLYGON	TDPolyCreate(TDINT num);
TDM_COMSERVAPI	TDBOOL		TDPolyDestroy(TDPPOLYGON pThis);

#ifdef		TDM_SI_CPLUSPLUS
	}
#endif		/*TDM_SI_CPLUSPLUS*/

#endif		/*TDM_SI_INCLUDED_COMTYPE_H*/