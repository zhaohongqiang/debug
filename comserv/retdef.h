#ifndef			TDM_SI_INCLUDED_RETDEF_H
#define			TDM_SI_INCLUDED_RETDEF_H

/*********************************************************************
the follow is return value define for TDRESULT.
*********************************************************************/
#define			TDM_USER_DEFINE			0X1000
/*success*/
#define			TDM_NORMAL_SUCCESS		0X0
#define			TDM_NOT_RESPOSE			0X1		/*not dispatch to route*/
#define			TDM_LOAD_EXISTED		0X2
#define			TDM_CREATE_NEW			0X3
/*error*/
#define			TDM_NORMAL_ERROR		0X0
#define			TDM_PARAM_ERROR			0X1
#define			TDM_NOT_IMPLEMENT		0X2
#define			TDM_NOT_SUPPORT			0X3
#define			TDM_ATOM_EXISTING		0X4		/*require element(atom) already existting*/
#define			TDM_ATOM_NOTFOUND		0X5
#define			TDM_MEMORY_ERROR		0X6
#define			TDM_TYPE_ERROR			0X7



#endif			/*TDM_SI_INCLUDED_RETDEF_H*/
