#ifndef		TDM_SI_INCLUDED_CONSNORM_H
#define		TDM_SI_INCLUDED_CONSNORM_H

typedef		TDDWORD			TDPOSITION;

#ifdef		TDM_SI_UNICODE
	typedef		TDWCHAR			TDTCHAR;
	typedef		TDPWSTR			TDPTSTR;
	typedef		TDPCWSTR		TDPCTSTR;
	#define		_TT(x)			L ## x
#else
	typedef		TDCHAR			TDTCHAR;
	typedef		TDPSTR			TDPTSTR;
	typedef		TDPCSTR			TDPCTSTR;
	#define		_TT(x)			x
#endif		/*TDM_SI_UNICODE*/
	#define		TTEXT(x)		_TT(x)

#endif		/*TDM_SI_INCLUDED_CONSNORM_H*/

