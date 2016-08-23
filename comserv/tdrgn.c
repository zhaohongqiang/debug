#include "comserv.h"


TDM_COMSERVAPI	TDVOID TDNormalRect(TDPRECT rect);
TDM_COMSERVAPI	TDVOID	TDF_SetRect(TDconst TDPRECT rc1,TDPRECT rc2);
TDM_COMSERVAPI	TDPRECT	TDRectCreate();




TDM_COMSERVAPI	TDPRgn		
TDRgn_Create()
{
	TDPRgn pThis;
	pThis=(TDPRgn)Malloc(sizeof(TDRgn));
	TD_ArrayInit(&pThis->mData,1);
	return pThis;
}
TDM_COMSERVAPI	TDPRgn		
TDRgn_CreateRect(TDPRECT pRect)
{
	TDPRgn pThis;
	TDPRECT pTmp;
	pThis=(TDPRgn)Malloc(sizeof(TDRgn));
	TD_ArrayInit(&pThis->mData,1);
	pTmp=(TDPRECT)Malloc(sizeof(TDRECT));
	TDF_SetRect(pRect,pTmp);
	TD_ArrayAppendElement(&pThis->mData,pTmp);
	return pThis;
}
TDM_COMSERVAPI	TDPRgn		
TDCreateRectRgn(TDINT x,TDINT y,TDINT x1,TDINT y1)
{
	TDPRgn pThis;
	TDPRECT pTmp;
	pThis=(TDPRgn)Malloc(sizeof(TDRgn));
	TD_ArrayInit(&pThis->mData,1);
	pTmp=(TDPRECT)Malloc(sizeof(TDRECT));
	pTmp->left=x;pTmp->top=y;
	pTmp->right=x1;pTmp->bottom=y1;
	TD_ArrayAppendElement(&pThis->mData,pTmp);
	return pThis;
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_Destroy(TDPRgn pThis)
{
	TDINT i;
	if(!pThis)
		return TDFALSE;
	for(i=0;i<TD_ArrayCount(&pThis->mData);i++)
	{
		TDPRECT pRect;
		pRect=TD_ArrayElementAt(&pThis->mData,i);
		if(pRect)
			TDFree(pRect);
	}
	TD_ArrayClear(&pThis->mData);
	TDFree(pThis);
	return TDTRUE;
}

TDM_COMSERVAPI  TDINT		
TDRgn_GetSize(TDPRgn pThis)
{
	if(!pThis)
		return TDFALSE;
	return pThis->mData.mCount;
}

TDM_COMSERVAPI  TDPRECT		
TDRgn_GetAt(TDPRgn pThis,TDINT index)
{
	if(!pThis)
		return TDNULL;
	return TD_ArrayElementAt(&pThis->mData,index);
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_Empty(TDPRgn pThis)
{
	TDINT i;
	if(!pThis)
		return TDFALSE;
	for(i=0;i<pThis->mData.mCount;i++)
	{
		TDFree(TD_ArrayElementAt(&pThis->mData,i));
	}
	TD_ArrayClear(&pThis->mData);
	return TDTRUE;
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_AndRect(TDPRgn pThis,TDPRECT pRect1,TDPRgn pDst)
{
	TDINT i;
	TD_Array array;
	TDRECT rect;
	if(!pThis||!pRect1||!pDst)
		return TDFALSE;
	TD_ArrayInit(&array,1);
	for(i=0;i<pThis->mData.mCount;i++)
	{
		TDPRECT pRect,pTmp;
		pRect=TD_ArrayElementAt(&pThis->mData,i);
	
		TDNormalRect(pRect);
		TDNormalRect(pRect1);
		rect.left=TDM_MAXVAL(pRect->left,pRect1->left);
		rect.top=TDM_MAXVAL(pRect->top,pRect1->top);
		rect.right=TDM_MINVAL(pRect->right,pRect1->right);
		rect.bottom=TDM_MINVAL(pRect->bottom,pRect1->bottom);
		if(rect.left < rect.right && rect.top < rect.bottom)
		{
			pTmp=TDRectCreate();
			pTmp->left=rect.left;
			pTmp->right=rect.right;
			pTmp->top=rect.top;
			pTmp->bottom=rect.bottom;
			TD_ArrayAppendElement(&array,pTmp);
		}
	}
	TDRgn_Empty(pDst);
/*	for(i=0;i<array.mCount;i++)
	{
		TD_ArrayAppendElement(&pDst->mData,TD_ArrayElementAt(&array,i));
	}
	TD_ArrayClear(&array);*/
	memcpy(&pDst->mData,&array,sizeof(TD_Array));
	return TDTRUE;
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_AndRgn(TDPRgn pThis,TDPRgn pSec,TDPRgn pDst)
{
	TDINT i,j;
	TD_Array array;
	if(!pThis||!pSec || !pDst)
		return TDFALSE;
	TD_ArrayInit(&array,1);
	for(i=0;i<pSec->mData.mCount;i++)
	{
		TDPRECT pRect;
		pRect=TD_ArrayElementAt(&pSec->mData,i);
		for(j=0;j<pThis->mData.mCount;j++)
		{
			TDPRECT pRect1,pTmp;
			TDRECT rect;
			pRect1=TD_ArrayElementAt(&pThis->mData,j);

			TDNormalRect(pRect);
			TDNormalRect(pRect1);
			rect.left=TDM_MAXVAL(pRect->left,pRect1->left);
			rect.top=TDM_MAXVAL(pRect->top,pRect1->top);
			rect.right=TDM_MINVAL(pRect->right,pRect1->right);
			rect.bottom=TDM_MINVAL(pRect->bottom,pRect1->bottom);
			if(rect.left < rect.right && rect.top < rect.bottom)
			{
				pTmp=TDRectCreate();
				pTmp->left=rect.left;
				pTmp->right=rect.right;
				pTmp->top=rect.top;
				pTmp->bottom=rect.bottom;
				TD_ArrayAppendElement(&array,pTmp);
			}
		}
	}
	TDRgn_Empty(pDst);
/*	for(i=0;i<array.mCount;i++)
	{
		TD_ArrayAppendElement(&pDst->mData,TD_ArrayElementAt(&array,i));
	}
	TD_ArrayClear(&array);
	*/
	memcpy(&pDst->mData,&array,sizeof(TD_Array));
	return TDTRUE;
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_OrRect(TDPRgn pThis,TDPRECT pSec,TDPRgn pDst)
{
	TDPRgn pAdd;
	TDPRECT pTmp;
	if(!pThis||!pSec||!pDst)
		return TDFALSE;
	pAdd=TDRgn_Create();
	TDRgn_AndRect(pThis,pSec,pAdd);
	TDRgn_SubRgn(pThis,pAdd,pDst);
	TDRgn_Destroy(pAdd);
	pTmp=TDRectCreate();
	TDF_SetRect(pSec,pTmp);
	TD_ArrayAppendElement(&pDst->mData,pTmp);
	return TDTRUE;
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_OrRgn(TDPRgn pThis,TDPRgn pSec,TDPRgn pDst)
{
	TDINT i;
	TDPRgn pAdd,pt;
	if(!pThis||!pSec||!pDst)
		return TDFALSE;
	pAdd=TDRgn_Create();
	pt=TDRgn_Create();
	TDRgn_AndRgn(pThis,pSec,pAdd);
	TDRgn_SubRgn(pSec,pAdd,pt);
	for(i=0;i<pThis->mData.mCount;i++)
	{
		TDPRECT pRect,pTmp;
		pRect=TD_ArrayElementAt(&pThis->mData,i);
		pTmp=TDRectCreate();
		TDF_SetRect(pRect,pTmp);
		TD_ArrayAppendElement(&pt->mData,pTmp);
	}
	TDRgn_Empty(pDst);
//	TDRgn_Copy(pt,pDst);
//	TDRgn_Destroy(pt);
	memcpy(&pDst->mData,&pt->mData,sizeof(TD_Array));
	TDFree(pt);
	return TDTRUE;
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_SubRect(TDPRgn pThis,TDPRECT pSec,TDPRgn pDst)
{
	TDINT i;
	TD_Array array;
	TDRECT rect;
	if(!pThis||!pSec||!pDst)
		return TDFALSE;
	TD_ArrayInit(&array,1);
	for(i=0;i<pThis->mData.mCount;i++)
	{
		TDPRECT pRect,pTmp;
		pRect=TD_ArrayElementAt(&pThis->mData,i);
		TDNormalRect(pRect);
		TDNormalRect(pSec);
		rect.left=TDM_MAXVAL(pRect->left,pSec->left);
		rect.top=TDM_MAXVAL(pRect->top,pSec->top);
		rect.right=TDM_MINVAL(pRect->right,pSec->right);
		rect.bottom=TDM_MINVAL(pRect->bottom,pSec->bottom);
		if(rect.left < rect.right && rect.top < rect.bottom)
		{/*follow will make the (rect-pSec)*/
			TDF_SetRect(pRect,&rect);
			/*left*/
			if(rect.left < pSec->left )
			{
				pTmp=TDRectCreate();
				pTmp->left=rect.left;
				pTmp->right=pSec->left;
				pTmp->top=rect.top;
				pTmp->bottom=rect.bottom;
				rect.left=pSec->left;
				TD_ArrayAppendElement(&array,pTmp);
			}
			/*right*/
			if(rect.right > pSec->right)
			{
				pTmp=TDRectCreate();
				pTmp->left=pSec->right;
				pTmp->right=rect.right;
				pTmp->top=rect.top;
				pTmp->bottom=rect.bottom;
				rect.right=pSec->right;
				TD_ArrayAppendElement(&array,pTmp);
			}
			/*top*/
			if(rect.top < pSec->top)
			{
				pTmp=TDRectCreate();
				pTmp->left=rect.left;
				pTmp->right=rect.right;
				pTmp->top=rect.top;
				pTmp->bottom=pSec->top;
				rect.top=pSec->top;
				TD_ArrayAppendElement(&array,pTmp);
			}
			/*bottom*/
			if(rect.bottom > pSec->bottom)
			{
				pTmp=TDRectCreate();
				pTmp->left=rect.left;
				pTmp->right=rect.right;
				pTmp->top=pSec->bottom;
				pTmp->bottom=rect.bottom;
				TD_ArrayAppendElement(&array,pTmp);
			}
		}
		else
		{
			pTmp=TDRectCreate();
			TDF_SetRect(pRect,pTmp);
			TD_ArrayAppendElement(&array,pTmp);
		}
	}
	TDRgn_Empty(pDst);
/*	for(i=0;i<array.mCount;i++)
		TD_ArrayAppendElement(&pDst->mData,TD_ArrayElementAt(&array,i));
	TD_ArrayClear(&array);
*/
	memcpy(&pDst->mData,&array,sizeof(TD_Array));
	return TDTRUE;
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_SubRgn(TDPRgn pThis,TDPRgn pSec,TDPRgn pDst)
{
	TDPRgn pTmp;
	TDINT i;
	if(!pThis||!pSec||!pDst)
		return TDFALSE;
	pTmp=TDRgn_Create();
	TDRgn_AndRgn(pThis,pSec,pTmp);

	TDRgn_Copy(pThis,pDst);
	for(i=0;i<pTmp->mData.mCount;i++)
	{
		TDPRECT pRect;
		pRect=TD_ArrayElementAt(&pTmp->mData,i);
		TDRgn_SubRect(pDst,pRect,pDst);
	}
	TDRgn_Destroy(pTmp);
	return TDTRUE;
}

TDM_COMSERVAPI  TDBOOL		
TDRgn_Copy(TDPRgn pThis,TDPRgn pSec)
{
	TDPRECT pTmp;
	TDINT i;
	if(!pThis||!pSec)
		return TDFALSE;
	if(pThis==pSec)
		return TDTRUE;
	TDRgn_Empty(pSec);
	for(i=0;i<pThis->mData.mCount;i++)
	{
		TDPRECT pRect;
		pRect=TD_ArrayElementAt(&pThis->mData,i);
		pTmp=TDRectCreate();
		TDF_SetRect(pRect,pTmp);
		TD_ArrayAppendElement(&pSec->mData,pTmp);
	}
	return TDTRUE;
}


TDM_COMSERVAPI	TDBOOL	TDF_PtEqual(TDPOINT pt1,TDPOINT pt2)
{
	return ((pt1.x == pt2.x) && (pt1.y == pt2.y));
}

#define		TDM_MAX_INTER		120
TDM_COMSERVAPI	TDBOOL	TDF_PtInPolygon(TDPOINT pt,TDPOLYGON *pPoly)
{
	TDINT		i,next,pointNum,bIn,maxY,interNum,inter[TDM_MAX_INTER];
	TDPOINT		intersectPoint;
    TDPOINT		*pPoint;
	
    if(!pPoly)
		return	TDFALSE;
	pointNum	= (TDINT)pPoly->nPtNum;
	pPoint		= pPoly->pPtArray;

	if(pointNum < 2)
		return TDFALSE;
	if(pointNum == 2)	/*is rect*/
	{
		return	((TDM_BETWEEN(pt.x,pPoly->pPtArray[0].x,pPoly->pPtArray[1].x)) && \
			(TDM_BETWEEN(pt.y,pPoly->pPtArray[0].y,pPoly->pPtArray[1].y)));
	}

	i			= 0;    
	interNum	= 0;
	do
	{
		next = (i + 1) % pointNum;

		if(pPoint[i].y != pPoint[next].y)
		{
#ifndef TS_NO_DOUBLE
			TDDOUBLE			k,b;
#else
			TDINT			k,b;
#endif
			intersectPoint.y  = pt.y;
			if(pPoint[i].x == pPoint[next].x)
			{
				intersectPoint.x = pPoint[i].x;
			}
			else
			{
#ifndef TS_NO_DOUBLE
				k = (TDDOUBLE)(pPoint[next].y - pPoint[i].y) / (TDDOUBLE)(pPoint[next].x - pPoint[i].x);
				b = (TDDOUBLE)pPoint[i].y - (TDDOUBLE)pPoint[i].x * k;
#else
				k = (pPoint[next].y - pPoint[i].y) / (pPoint[next].x - pPoint[i].x);
				b = pPoint[i].y - pPoint[i].x * k;
#endif
				if(k != 0)
#ifndef TS_NO_DOUBLE
					intersectPoint.x = (TDINT)(((TDDOUBLE)pt.y - b) / k);
#else
					intersectPoint.x = (TDINT)((pt.y - b) / k);
#endif
				else if(pt.y == pPoint[i].y)
					intersectPoint.x = pPoint[i].x;
				else /*if( y == y2)*/
					intersectPoint.x = pPoint[next].x;
			}
			
			if(TDM_BETWEEN(intersectPoint.x,pPoint[i].x,pPoint[next].x)
				&& TDM_BETWEEN(intersectPoint.y,pPoint[i].y,pPoint[next].y))
			{
				maxY = TDM_MAXVAL(pPoint[i].y,pPoint[next].y);
				if(intersectPoint.y != maxY)
				{
					int i = 0,j;
					if((interNum + 1 )>= TDM_MAX_INTER)
						return TDFALSE;
					while((i < interNum) && (inter[i] < intersectPoint.x))
						i++;
					if( i != interNum)
					{
						for(j = 0; j < interNum - i; j++)
							inter[interNum - j] = inter[interNum - 1 - j];
					}
					inter[i]  = intersectPoint.x;
					interNum++;
				}
			}	/*if between*/
		}	/*if not parellel*/
		i++;
	}while(i < pointNum);

	/* according to inter array sort ,see pt.x if between valid area*/
	bIn = 1;
	for( i = 0 ; i < interNum - 1; i++)
	{
		if(bIn && TDM_BETWEEN(pt.x,inter[i],inter[i + 1]))

			return TDTRUE;
		bIn = !bIn;
	}
	return TDFALSE;
}

TDM_COMSERVAPI	TDVOID 
TDNormalRect(TDPRECT rect)
{
	TDINT tmp;

	if(!rect)
		return ;
	if(rect->left > rect->right)
	{
		tmp=rect->left;
		rect->left=rect->right;
		rect->right=tmp;
	}
	if(rect->top > rect->bottom)
	{
		tmp=rect->top;
		rect->top=rect->bottom;
		rect->bottom=tmp;
	}
}

TDM_COMSERVAPI	TDBOOL	
TDPtInRect(TDPRECT rt,TDPOINT pt)
{
	if(pt.x>=rt->left&&pt.x<=rt->right&&
		pt.y>=rt->top&&pt.y<=rt->bottom)
		return TDTRUE;
	return TDFALSE;
}

TDM_COMSERVAPI	TDBOOL	
TDIntersectRect(TDPRECT rt,TDPRECT rc1,TDPRECT rc2)
{
	TDRECT rect;
	if(!rc1 || !rc2)
		return TDFALSE;
	TDNormalRect(rc1);
	TDNormalRect(rc2);
	rect.left=TDM_MAXVAL(rc1->left,rc2->left);
	rect.top=TDM_MAXVAL(rc1->top,rc2->top);
	rect.right=TDM_MINVAL(rc1->right,rc2->right);
	rect.bottom=TDM_MINVAL(rc1->bottom,rc2->bottom);
	if(rect.left <= rect.right && rect.top <= rect.bottom)
	{
		TDF_SetRect(&rect,rt);
		return TDTRUE;
	}
	else
	{
		rt->left=0;rt->right=0;
		rt->top=0;rt->bottom=0;
	}
	return TDFALSE;
}
TDM_COMSERVAPI	TDVOID	
TDF_SetRect(TDconst TDPRECT rc1,TDPRECT rc2)
{
		rc2->left=rc1->left;
		rc2->right=rc1->right;
		rc2->top=rc1->top;
		rc2->bottom=rc1->bottom;
}
TDM_COMSERVAPI	TDVOID	
TDCopyRect(TDPRECT dst,TDconst TDPRECT src)
{
		dst->left=src->left;
		dst->right=src->right;
		dst->top=src->top;
		dst->bottom=src->bottom;
}
TDM_COMSERVAPI	TDVOID	
TDOffsetRect(TDPRECT r,int dx,int dy)
{
	r->left+=dx;r->right+=dx;
	r->top+=dy;r->bottom+=dy;
}
TDM_COMSERVAPI	TDPCIRCLE		
TDCircleCreate()
{
	TDPCIRCLE pThis;
	pThis=(TDPCIRCLE)Malloc(sizeof(TDCIRCLE));
	return pThis;
}

TDM_COMSERVAPI	TDBOOL		
TDCircleDestroy(TDPCIRCLE pThis)
{
	if(pThis)
		TDFree(pThis);
	return TDTRUE;
}

TDM_COMSERVAPI	TDPAREA
TDAreaCreate()
{
	TDPAREA pThis;
	pThis=(TDPAREA)Malloc(sizeof(TDAREA));
	return pThis;
}

TDM_COMSERVAPI	TDBOOL
TDAreaDestroy(TDPAREA pThis)
{
	if(pThis->Area.pRect)
		TDFree(pThis->Area.pRect);
	if(pThis)
		TDFree(pThis);
	return TDTRUE;
}

TDM_COMSERVAPI	TDPRECT
TDRectCreate()
{
	TDPRECT pThis;
	pThis=(TDPRECT)Malloc(sizeof(TDRECT));
	return pThis;
}

TDM_COMSERVAPI	TDBOOL
TDRectDestroy(TDPRECT pThis)
{
	if(pThis)
		TDFree(pThis);
	return TDTRUE;
}

TDM_COMSERVAPI	TDPPOLYGON
TDPolyCreate(TDINT num)
{
	TDPPOLYGON pThis;
	pThis=(TDPPOLYGON)Malloc(sizeof(TDPOLYGON)+num*sizeof(TDPOINT));
	if(pThis)
	{
		pThis->nPtNum=num;
		pThis->pPtArray=(TDPPOINT)(pThis+1);
	}
	return pThis;
}

TDM_COMSERVAPI	TDBOOL
TDPolyDestroy(TDPPOLYGON pThis)
{
	if(pThis)
		TDFree(pThis);
	return TDTRUE;
}

TDM_COMSERVAPI	TDVOID
TDRectSet(TDPRECT pThis, long left, long top, long right, long bottom)
{
	pThis->left = left;
	pThis->right = right;
	pThis->top = top;
	pThis->bottom = bottom;
}

