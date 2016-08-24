/*
Ansi C compatible
*/
#define APPENDMEDIA		0
#define GETSONGLIST		1
#define GETCURSONG		2
#define INSERT			3
#define DEL				4
#define NEXT			5
#define REPLAY			6
#define PAUSEPLAY		7
#define QUERYPAUSEPLAY	8
#define YUANBANDAO		9
#define QUERYYUANBANDAO	10
#define MUTESOUND		11
#define QUERYMUTESOUND	12
#define MODIFYVOL		13
#define QUERYVOL		14
#define MODIFYTONE		15
#define QUERYTONE		16
#define GETALLSTATE		17
#define CALLSERVICE		18
#define MODIFYMIC		19
#define CAPTIONINFO	    20
#define PLAYPERCENT		21
#define HUNYIN			22
#define	XIAOGUO			23
#define MONITORMODIFY 	24
#define AWOKE1		    25
#define AWOKE2			26
#define CLEANROOM		27
#define BROADCAST		28
#define RECORD			29
#define MEMBERCARD		30
#define CLOSEROOMCMD	32
#define FIREALARM	34
#define MRSONGCMD		37
#define MRSONGSET		38
#ifndef		TDM_SI_INCLUDED_TCOMDEF_H
#define		TDM_SI_INCLUDED_TCOMDEF_H

/*defined switch macro in this file.*/

/*the internal version of this struct*/
#define		TDM_VERSION		0X100

/*call conversion definition*/
#define		TDM_CC_DEFCAL
#define		TDM_CC_STDCAL	_cdecl
#define		TDM_CC_WINCAL	_stdcall
#define		TDM_CC_PASCAL	_pascal

/*modifier definition*/
#define		TDunsigned		unsigned
#define		TDconst			const
#define		TDextern		extern

/*base type definition*/
typedef		signed short		TDINT16;
typedef		signed int		TDINT32;

typedef		TDINT16*		TDPINT16;
typedef		TDINT32*		TDPINT32;

typedef		char			TDCHAR;
typedef		TDCHAR			*TDPCHAR;
typedef		unsigned short		TDWCHAR;
typedef		unsigned char		TDBYTE;
typedef		unsigned short		TDWORD;
typedef		unsigned int		TDUINT;
typedef		unsigned long		TDDWORD;



typedef		signed short		TDSHORT;
typedef		signed int		TDINT;
typedef		long			TDLONG;
#ifndef XP_ESS
typedef		double			TDDOUBLE;
#else
typedef		long			TDDOUBLE;
#endif
typedef		void			TDVOID;
typedef		int			TDBOOL;

////////////////////////////
typedef		signed short		INT16;
typedef		signed int		INT32;

typedef		unsigned char		BYTE;
typedef		unsigned short		WORD;
typedef		unsigned int		UINT;
typedef		unsigned long	        DWORD;

typedef		signed short		SHORT;
typedef		signed int		INT;
typedef		long			LONG;
typedef		void			VOID;
typedef		int			BOOL;

////////////////////////////


/*check TString.h get more definition.
#ifdef		_UNICODE_
#define		TSTCHAR		TSWCHAR
#else
#define		TSTCHAR		TSCHAR
#endif
*/

typedef		TDBYTE				*TDPBYTE;
typedef		TDWORD				*TDPWORD;
typedef		TDDWORD				*TDPDWORD;
typedef		TDSHORT				*TDPSHORT;
typedef		TDINT				*TDPINT;
typedef		TDLONG				*TDPLONG;


typedef		TDVOID				*TDPVOID;
typedef		TDPVOID				*TDPPVOID;

typedef		TDCHAR				*TDPSTR;
typedef		TDWCHAR				*TDPWSTR;
typedef		TDconst TDCHAR			*TDPCSTR;
typedef		TDconst TDWCHAR			*TDPCWSTR;

typedef		TDBOOL				*TDPBOOL;


typedef		TDDWORD				TDLPARAM;
typedef		TDDWORD				TDHPARAM;
typedef	    TDDWORD				TDHANDLE;

#pragma pack(1)
/* windows style*/
typedef struct {
	/* BITMAPFILEHEADER*/
	BYTE	bfType[2];
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
	/* BITMAPINFOHEADER*/
	DWORD	BiSize;
	LONG	BiWidth;
	LONG	BiHeight;
	WORD	BiPlanes;
	WORD	BiBitCount;
	DWORD	BiCompression;
	DWORD	BiSizeImage;
	LONG	BiXpelsPerMeter;
	LONG	BiYpelsPerMeter;
	DWORD	BiClrUsed;
	DWORD	BiClrImportant;
} BMPHEAD;

/* os/2 style*/
typedef struct {
	/* BITMAPFILEHEADER*/
	BYTE	bfType[2];
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
	/* BITMAPCOREHEADER*/
	DWORD	bcSize;
	WORD	bcWidth;
	WORD	bcHeight;
	WORD	bcPlanes;
	WORD	bcBitCount;
} BMPCOREHEAD;
#pragma pack()

/*extended type definition.*/
typedef		TDLONG				TDRESULT;

#ifndef WIN32
typedef struct _tagTDPOINT{
	TDINT		x;
	TDINT		y;
}TDPOINT,*TDPPOINT;

typedef struct _tagTDSize{
	TDINT	cx;
	TDINT	cy;
}TDSIZE,*TDPSIZE;

typedef	struct _tagTDRECT{
	TDINT	left;
	TDINT	top;
	TDINT	right;
	TDINT	bottom;

}TDRECT,*TDPRECT;

typedef TDDWORD  TDRGB;
typedef TDDWORD* TDPRGB;
#else

typedef POINT		TDPOINT;
typedef POINT*		TDPPOINT;
typedef SIZE		TDSIZE;
typedef SIZE*		TDPSIZE;
typedef RECT		TDRECT;
typedef RECT*		TDPRECT;
typedef COLORREF	TDRGB;

#endif /*WIN32*/

typedef struct _tagTDCIRCLE{
	TDINT	x,y,radius;
}TDCIRCLE,*TDPCIRCLE;

typedef	struct _tagTDPOLYGON{
	TDINT		nPtNum;
	TDPPOINT	pPtArray;
}TDPOLYGON,*TDPPOLYGON;


typedef struct	_tagTDFONT{
  TDCHAR 	fName[252];
  TDINT		fFontID;
  TDINT		fSize; 
  TDINT		fWeight;
  TDDWORD	fStyle; 
  TDRGB		fColor;
  TDRGB		fBgColor;
  TDINT		fMode;
}TDFONT,*TDPFONT;


#define TD_FONT_STYLE_NORMAL				0x0
#define TD_FONT_STYLE_ITALIC				0x1
#define TD_FONT_STYLE_OBLIQUE				0x2
#define TD_FONT_STYLE_UNDERLINE				0x4
#define TD_FONT_STYLE_OVERLINE				0x8
#define TD_FONT_STYLE_THROUGHLINE			0x10

#define TD_FONT_WEIGHT_NORMAL             400
#define TD_FONT_WEIGHT_BOLD               700

typedef	struct	_tagTDcolorKey{
	TDRGB		StartColor;
	TDRGB		EndColor;
}TDcolorKey,TDCOLORKEY,*TDPCOLORKEY;

/*help macro to TDArea*/

#define AREA_TYPE_RECT   1
#define AREA_TYPE_POLY   2
#define AREA_TYPE_CIRCLE 3

#define RectArea	Area.pRect
#define PolyArea	Area.pPoly
#define CircleArea	Area.pCircle

typedef struct _TDAREA{
	TDDWORD		nType;
	union{
		TDPRECT			pRect;
		TDPCIRCLE		pCircle;
		TDPPOLYGON		pPoly;
	}Area;
}TDArea,*TDPArea,TDAREA,*TDPAREA;

/*const value definition.*/
#define		TDTRUE				1
#define		TDFALSE				0

#define		TDNULL				0X0L

#define		TDM_CV_SEVERITY_SUCCESS		0
#define		TDM_CV_SEVERITY_ERROR		1
#define		TDM_CV_DEFAULT_FACILITY		0

/*value operator macro.*/
#define		TDM_MAXVAL(a,b)            (((a) > (b)) ? (a) : (b))
#define		TDM_MINVAL(a,b)            (((a) < (b)) ? (a) : (b))
#define		TDM_ABS(a)				   (((a) > 0) ? (a) : (-a))

#define		TDM_HIBYTE(w)				((TDBYTE)(((TDWORD)(w) >> 8) & 0xFF))
#define		TDM_LOBYTE(w)				((TDBYTE)(w))
#define		TDM_MAKEWORD(low,high)		((TDWORD)(((TDBYTE)(low)) | ((TDWORD)((TDBYTE)(high))) << 8))
#define		TDM_HIWORD(l)				((TDWORD)(((TDDWORD)(l) >> 16) & 0xFFFF))
#define		TDM_LOWORD(l)				((TDWORD)(l))
#define		TDM_MAKELONG(low,high)		((TDLONG)(((TDWORD)(low)) | ((TDDWORD)((TDWORD)(high))) << 16))

#define	TDM_GETWORD(ptr) ((TDWORD)(*((TDPBYTE)(ptr))) |\
						 ((TDWORD)((*(((TDPBYTE)(ptr))+1)) << 8)))
#define	TDM_GETDWORD(ptr) (((TDDWORD)((*(((TDPBYTE)(ptr))+0))))		|\
						 ((TDDWORD)((*(((TDPBYTE)(ptr))+1)) << 8))	|\
						 ((TDDWORD)((*(((TDPBYTE)(ptr))+2)) << 16))	|\
						 ((TDDWORD)((*(((TDPBYTE)(ptr))+3)) << 24)))
/* Generic test for success on any status value (non-negative numbers
 indicate success).
*/
#define	TDM_RESULT_SYSCONTINUE		0X0
#define	TDM_RESULT_SYSABORT			0X1


#define TDM_SUCCEEDED(Status)		((TDRESULT)(Status) >= 0)
/* and the inverse*/
#define TDM_FAILED(Status)			((TDRESULT)(Status)<0)
/* Return the code*/

#define TDM_RESULT_CODE(hr)			((hr) & 0xFFFF)
/*  Return the facility*/
#define TDM_RESULT_FACILITY(hr)		(((hr) >> 16) & 0X7FF)
#define	TDM_RESULT_SETFACILITY(hr,fac)		\
	((((TDRESULT)(hr)) & 0XF800FFFF) | ((TDDWORD)((fac) & 0x7ff)<<16))
#define	TDM_SYS_FACILITY			0X0
#define	TDM_ROUTER_FACILITY			0X1
/*	Return the system*/
#define TDM_RESULT_SYSTEM(hr)		(((hr) >> 27) & 0X7)
#define	TDM_RESULT_SETSYSTEM(hr,sys)		\
	(((TDRESULT)(hr)) = ((((TDRESULT)(hr)) & 0XC7FFFFFF) | ((TDDWORD)(sys & 0x7) << 27)))

/*  Return the severity*/
#define TDM_RESULT_SEVERITY(hr)		(((hr) >> 31) & 0X1)

/* Create an HRESULT value from component pieces*/
#define TDM_MAKE_RESULT(sev,sys,fac,code) \
    ((TDRESULT) (((TDDWORD)(sev & 0x1)<<31) | ((TDDWORD)(sys & 0x7) << 27) | ((TDDWORD)(fac & 0x7ff)<<16) | ((TDDWORD)(code & 0xffff))) )
#define	TDM_MAKE_RESULT3(sev,fac,code)		\
	TDM_MAKE_RESULT(sev,0,fac,code)
#define	TDM_MAKE_RESULT2(sev,code)			\
	TDM_MAKE_RESULT(sev,0,TDM_CV_DEFAULT_FACILITY,code)
	
/*dynamic cast relative macro*/
#define	TDM_OFFSET(st,member)		((TDDWORD)((TDPBYTE)&(((st)0).member)))
#define	TDM_RESTSIZE(st,member)		(sizeof(st)-TDM_OFFSET(st,member))

typedef enum TDColorMode{
  Color_RGB_565,
  Color_RGB_5551,
  Color_RGB_8888,
  Color_RGB_4444,
  Color_CLUT4,
  Color_CLUT8,
  Color_YUV
}TDColorMode;



#ifdef XP_UNIX
//   #define WHX_NEW_ARCHITECH
#endif

#define LOWORD(a) ((TDWORD)(a&0xffff))
#define HIWORD(a) ((TDWORD)((a>>16)&0xffff))



#endif		/*TDM_SI_INCLUDED_TCOMDEF_H*/

