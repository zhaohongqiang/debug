

#ifndef _TDRGN
#define _TDRGN
#include "comserv.h"

typedef struct  _TDRgn{
  TD_Array      mData;
}TDRgn,*TDPRgn;
#ifdef		TDM_SI_CPLUSPLUS
extern	"C"{
#endif		/*TDM_SI_CPLUSPLUS*/

TDM_COMSERVAPI	TDPRgn		TDRgn_Create();
TDM_COMSERVAPI	TDPRgn		TDRgn_CreateRect(TDPRECT pRect);
TDM_COMSERVAPI	TDPRgn		TDCreateRectRgn(TDINT x,TDINT y,TDINT w,TDINT h);
TDM_COMSERVAPI  TDINT		TDRgn_Destroy(TDPRgn pThis);
TDM_COMSERVAPI  TDINT		TDRgn_GetSize(TDPRgn pThis);
TDM_COMSERVAPI  TDPRECT		TDRgn_GetAt(TDPRgn pThis,TDINT index);
TDM_COMSERVAPI  TDBOOL		TDRgn_Empty(TDPRgn pThis);
TDM_COMSERVAPI  TDBOOL		TDRgn_AndRgn(TDPRgn pThis,TDPRgn pSec,TDPRgn pDst);
TDM_COMSERVAPI  TDBOOL		TDRgn_AndRect(TDPRgn pThis,TDPRECT pSec,TDPRgn pDst);
TDM_COMSERVAPI  TDBOOL		TDRgn_OrRgn(TDPRgn pThis,TDPRgn pSec,TDPRgn pDst);
TDM_COMSERVAPI  TDBOOL		TDRgn_OrRect(TDPRgn pThis,TDPRECT pSec,TDPRgn pDst);
TDM_COMSERVAPI  TDBOOL		TDRgn_SubRgn(TDPRgn pThis,TDPRgn pSec,TDPRgn pDst);
TDM_COMSERVAPI  TDBOOL		TDRgn_SubRect(TDPRgn pThis,TDPRECT pSec,TDPRgn pDst);
TDM_COMSERVAPI  TDBOOL		TDRgn_Copy(TDPRgn pThis,TDPRgn pSec);

#ifdef		TDM_SI_CPLUSPLUS
}
#endif		/*TDM_SI_CPLUSPLUS*/

#endif
