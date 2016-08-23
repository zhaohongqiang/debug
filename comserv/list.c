
#include "comserv.h"


TDPLINK   GetPrevLink(TDPLINK Link)
{
    return Link->PrevLink;
}

 TDPLINK   GetNextLink(TDPLINK Link)
{
    return Link->NextLink;
}

 TDPLINK   GetHeadOfList(TDC_PList List)
{
    return List->Head;
}

TDPLINK   GetTailOfList(TDC_PList List)
{
    return List->Tail;
}

TDDWORD   GetListLength(TDC_PList List)
{
    return List->Length;
}

TDC_PList   InitializeList(TDC_PList List)
{
    List->Head    = (TDPLINK) 0L;
    List->Tail    = (TDPLINK) 0L;
    List->Length  = 0;

    return List;
}


TDVOID   AddLinkToLink(TDPLINK DstLink, TDPLINK SrcLink)
{
    
    SrcLink->PrevLink = DstLink;
    SrcLink->NextLink = DstLink->NextLink;

	if(DstLink->NextLink)
		DstLink->NextLink->PrevLink = SrcLink;
    DstLink->NextLink = SrcLink;
}


TDPLINK   AddToList(TDC_PList List, TDPLINK DstLink, TDPLINK SrcLink)
{

    List->Length++;

    if ( SrcLink == (TDPLINK) 0L )
    {
        if ( List->Tail != (TDPLINK) 0L )
        {
            AddLinkToLink(List->Tail, DstLink);
        }
        else
        {
            DstLink->PrevLink = TDNULL;
            DstLink->NextLink = TDNULL;

            List->Head = DstLink;
        }

        return (List->Tail = DstLink);
    }


    if ( DstLink == (TDPLINK) 0L )
    {

        if ( List->Head != (TDPLINK) 0L )
        {
            AddLinkToLink(List->Head, SrcLink);
        }
        else
        {
            SrcLink->PrevLink = TDNULL;
            SrcLink->NextLink = TDNULL;

            List->Tail = SrcLink;
        }

        return (List->Head = SrcLink);
    }


    AddLinkToLink(DstLink, SrcLink);

    return SrcLink;
}


TDPLINK   DeleteFromList(TDC_PList List, TDPLINK Link)
{

    if ( List->Length != 0 )
    {
        if ( --List->Length != 0 )
        {

            if ( List->Head == Link )
            {
                List->Head = List->Head->NextLink;
            }


            if ( List->Tail == Link )
            {
                List->Tail = List->Tail->PrevLink;
            }


			if(Link->NextLink)
				Link->NextLink->PrevLink = Link->PrevLink;
			if(Link->PrevLink)
				Link->PrevLink->NextLink = Link->NextLink;
			TDFree((TDPVOID)Link);
        }
        else
        {
            List->Head = (TDPLINK) 0L;
            List->Tail = (TDPLINK) 0L;
			TDFree((TDPVOID)Link);/**/
        }

        return Link;
    }

    return (TDPLINK) 0L;
}

TDPLINK   AddToFrontOfList(TDC_PList List, TDPLINK Link)
{
    return AddToList(List, (TDPLINK) 0L, Link);
}

TDPLINK   AddToEndOfList(TDC_PList List, TDPLINK Link)
{
    return AddToList(List, Link, (TDPLINK) 0L);
}

TDPLINK   DeleteFromFrontOfList(TDC_PList List)
{
    return DeleteFromList(List, GetHeadOfList(List));
}

TDPLINK   DeleteFromEndOfList(TDC_PList List)
{
    return DeleteFromList(List, GetTailOfList(List));
}


/*
****************************
****************************
*/
/*initialize the list*/

TDM_COMSERVAPI	TDBOOL			TDC_List_List			(TDC_PList	pThis)
{
	if(pThis)
	{
		InitializeList(pThis);
		return TDTRUE;
	}
	else
		return TDFALSE;

}
TDM_COMSERVAPI	TDBOOL			TDC_List_DList			(TDC_PList pThis)
{
	if(pThis)
		return TDC_List_RemoveAll(pThis);
	return TDFALSE;
}

/*get the item number of the list*/

TDM_COMSERVAPI	TDDWORD			TDC_List_GetLength	(TDC_PList pThis)
{
	if(pThis)
		return pThis->Length;
	else
		return 0;
}

/*get the header node, if return TDNULL: the list is empty*/
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_GetHead		(TDC_PList pThis)
{
	if(pThis)
		return GetHeadOfList(pThis);	
	else
		return TDNULL;
}

/*get the tailer node, if return TDNULL: the list is empty*/
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_GetTail		(TDC_PList pThis)
{
	if(pThis)
		return GetTailOfList(pThis);	
	else
		return TDNULL;

}

/*get the next node, if return TDNULL: it's the list's end*/
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_GetNext		(TDLISTPOSITION pPos)
{
	if(pPos )
		return GetNextLink(pPos);
	return TDNULL;
}
/*get the Prev node, if return TDNULL: it's list's top*/

TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_GetPrev		(TDLISTPOSITION pPos)
{
	if(pPos )
		return GetPrevLink(pPos);
	return TDNULL;
}

/*get the saved data in the current node*/
TDM_COMSERVAPI	TDDWORD			TDC_List_GetData		(TDLISTPOSITION pPos)
{
	if(pPos)
		return pPos->mData;
	else
		return 0;
}
/*add a new data to the tail of the list*/
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_AddTail		(TDC_PList	pThis,TDDWORD nValue)
{
	TDPLINK link=TDNULL;
	if(pThis)
	{
		link=(TDPLINK)Malloc(sizeof(TDLINK));
		if(link)
		{
			link->mData=nValue;
			link->NextLink=TDNULL;
			link->PrevLink=TDNULL;
			return AddToEndOfList(pThis,link);
		}
		else
			return TDNULL;

	}
	return TDNULL;

}

/*add a new data to the header of the list*/
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_AddHead		(TDC_PList	pThis,TDDWORD nValue)
{
	TDPLINK link=TDNULL;
	if(pThis)
	{
		link=(TDPLINK)Malloc(sizeof(TDLINK));
		if(link)
		{
			link->mData=nValue;
			link->NextLink=TDNULL;
			link->PrevLink=TDNULL;
			return AddToFrontOfList(pThis,link);
		}
		else
			return TDNULL;

	}
	return TDNULL;
}

/*insert a new data before a special node,if the node is TDNULL, then add to the tail*/
TDM_COMSERVAPI	TDLISTPOSITION	TDC_List_Insert	(TDC_PList pThis,TDDWORD nValue,TDLISTPOSITION before)
{
	TDPLINK link=TDNULL;
	if(pThis )
	{
		link=(TDPLINK)Malloc(sizeof(TDLINK));
		if(link)
		{
			link->mData=nValue;
			link->NextLink=TDNULL;
			link->PrevLink=TDNULL;
			if(before )
			{
				if( before == pThis->Tail)
					return AddToEndOfList(pThis,link);
				return AddToList(pThis,before,link);
			}
			else
			    return AddToEndOfList(pThis,link);
		}
		else
			return TDNULL;

	}
	return TDNULL;
}
/*remove the tail node*/
TDM_COMSERVAPI	TDBOOL			TDC_List_RemoveTail	(TDC_PList pThis)
{
	if(pThis)
		return (TDBOOL)DeleteFromEndOfList(pThis);
	return TDFALSE;
}
/*remove the header node*/
TDM_COMSERVAPI	TDBOOL			TDC_List_RemoveHead	(TDC_PList pThis)
{
	if(pThis)
		return (TDBOOL)DeleteFromFrontOfList(pThis);
	return TDFALSE;

}
/*remove all node in the list*/
TDM_COMSERVAPI	TDBOOL			TDC_List_RemoveAll	(TDC_PList pThis)
{
	if(pThis)
	{
		while(TDC_List_RemoveHead(pThis));
		return TDTRUE;
	}
	return TDFALSE;
}
/*remove a special node in the list*/
TDM_COMSERVAPI	TDBOOL			TDC_List_RemoveAt		(TDC_PList pThis,TDLISTPOSITION Pos)
{
	if(pThis && Pos)
		return (TDBOOL)DeleteFromList(pThis,Pos);
	return TDFALSE;
}
/*remove a special data from the list if it saved in the list*/
TDM_COMSERVAPI	TDBOOL			TDC_List_Remove		(TDC_PList pThis,TDDWORD nValue)
{
	TDLISTPOSITION	pos;	
	if(!pThis)
		return TDFALSE;
	pos=TDC_List_GetHead(pThis);
	while(pos)
	{
		if(nValue == pos->mData)
			return TDC_List_RemoveAt(pThis,pos);
		pos=TDC_List_GetNext(pos);
	}
	return TDFALSE;
}

