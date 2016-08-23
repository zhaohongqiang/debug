/*TCP VERSION*/
 
#include "../comserv/comserv.h"
#include	"../platabs/platabs.h"
#include	"../js/tdmalloc.h"
#include "../platabs/tcomdef.h"
#include "tdevice.h"
#include "whxfile.h"
#include "whxsock.h"

#include "netinet/in.h"   /*for ntohs(int)*/
#include <pthread.h>
#include "semaphore.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

TDBOOL	 TDCookieSet(TDPString host,TDPString cookie);
TDPString	TDCookieGet(TDPString host,TDPString path);


typedef enum
{
	WHX_SYSTEM_FILE =0,
	WHX_FILE,
	WHX_HTTP_FILE,
	WHX_VOD_FILE,
	WHX_VOD2_FILE,
	WHX_AWB_FILE,
	MAX_FILE_TYPE
} TDFileType;


#define gURLFileStart	2
typedef		TDBOOL		(*TDC_FILEREAD)(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes);
typedef		TDVOID		(*TDC_FILECLOSE)(TDHANDLE hFile);
typedef		TDHANDLE	(*TDC_FILEOPEN)( TDPString dstFile);

typedef struct TDURLFile{
	TDCHAR	mName[16];
	TDINT	mDefaultPort;
	TDINT	mDefaultPortType;/*0:TCP;1:UDP*/
	TDC_FILEOPEN	mOpenFile;
	TDC_FILEREAD	mReadFile;
	TDC_FILECLOSE	mCloseFile;
}TDURLFile,*TDPURLFile;

TDHANDLE	TDHttpFileOpen( TDPString dstFile);
TDBOOL		TDHttpFileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes);
TDVOID		TDHttpFileClose( TDHANDLE hFile);
TDHANDLE	TDVodFileOpen(TDPString dstFile);
TDBOOL		TDVodFileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes);
TDVOID		TDVodFileClose( TDHANDLE hFile);
TDHANDLE	TDVod2FileOpen(TDPString dstFile);
TDBOOL		TDVod2FileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes);
TDVOID		TDVod2FileClose( TDHANDLE hFile);
TDHANDLE	TDAwbFileOpen(TDPString dstFile);
TDBOOL		TDAwbFileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes);
TDVOID		TDAwbFileClose( TDHANDLE hFile);

TDURLFile	gUrlFileArray[4]=
{
	{"http://",80,0,TDHttpFileOpen,TDHttpFileRead,TDHttpFileClose},
	{"vod://",2058,0,TDVodFileOpen,TDVodFileRead,TDVodFileClose},
	{"VOD2://",3000,1,TDVod2FileOpen,TDVod2FileRead,TDVod2FileClose},
	{"awb://",5527,0,TDAwbFileOpen,TDAwbFileRead,TDAwbFileClose},
//	{"awb://",5527,0,TDNULL,TDNULL,TDNULL},
};


/*奥维博*/
//#include <winsock.h>

#define PROTO_AWB		5527
#define AWB_TIMEOUT		5000


/* 数据包及数据块尺寸定义 */
#define	PGDTSIZE	2024	// 数据包数据尺寸
//#define	BKSIZE		65536	// 数据块尺寸
#define PGNUM		8
#define PGNUMOFBK	7*PGNUM
#define BKSIZE		PGDTSIZE*PGNUMOFBK
#define	MINPGSIZE	10	// 最小包尺寸
#define	MAXPGSIZE	24+PGDTSIZE	// 最大包尺寸
#define	MINQUPG		10	// 最小请求包尺寸
#define	MAXQUPG		272	// 最大请求包尺寸
#define	MINAWPG		10	// 最小应答包尺寸
#define	MAXAWPG		MAXPGSIZE	// 最大应答包尺寸
#define	MAXAWNDPG	24	// 最大无信息部分应答包尺寸
#define	PGHDSIZE	10	// 数据包头尺寸
#define	QUFLPGSIZE	262	// 请求文件 (1300)信息部分尺寸
#define	QUDTPGSIZE	10	// 请求文件数据(1400)信息部分尺寸
#define	FLNAMESIZE	200		// 文件名尺寸


/* 服务器错误定义 */
#define	ER_SNOF	-1100	// 无此文件错误
#define	ER_SRDF	-1200	// 读出文件错误
#define	ER_SWRF	-1300	// 写入文件错误
#define	ER_SCLF	-1400	// 关闭文件错误
#define	ER_SESS	-2500	// 建socket错误
#define	ER_SBND	-2600	//  bind错误
#define	ER_SCON	-2700	//  connect错误
#define	ER_SRDS	-2800	// 读socket错误
#define	ER_SWRS	-2900	// 写socket错误
#define	ER_SPKL	-3100	// 数据包长度不符
#define	ER_SDBD	-3200	// 数据部分错误
#define	ER_SNOT	-3300	// 无此类型错误
#define	ER_STNI	-3400	// 类型与信息不符



/*请求类型定义*/
#define	QU_SERV	1100		//要求服务申请请求
#define	QU_DTFL	1300	//传送数据文件请求 
#define	QU_FLDT	1400		//传送文件数据请求
#define	QU_ENDF	1500		//传送文件结束请求
#define	QU_STOP	1600		//停止服务请求

/* 应答类型定义*/
#define	AW_SERV	3100		//接受服务申请应答
#define	AW_DTFL 3300		//传送数据文件应答
#define	AW_FLDT 3400		//传送文件数据应答
#define	AW_ENDF	3500		//传送文件结束应答
#define	AW_STOP	3600		//接收停止服务应答

#define	RF_SERV	4100		//拒绝服务申请应答
#define	RF_DTFL	4300		//拒绝传送文件应答

/*awb file data*/
typedef struct TDAwbFileData{
	TDHANDLE	mSocket;
	TDINT		mSeqNo;
	TDINT		mBlockNum;	
	TDINT		mBlockNo;
	TDINT		mBlockOff;
	TDINT		mBlockLen;
#ifdef XP_PC
	HANDLE		mReadEvent;
	HANDLE		mReadThread;
#elif defined  XP_UNIX
	void*  mReadEvent;
	pthread_t  mReadThread;

//	TDHANDLE	 mReadEvent;
//	TDHANDLE   mReadThread;
#endif
	TDBOOL		mThreadStop;
	TDCHAR		mBuf[BKSIZE];
	TD_Array		mFileNames;
	TDINT		mFileNums;
	TDINT		mFileIndex;
}TDAwbFileData,*TDPAwbFileData;



#define PROTO_HTTP  80
#define PROTO_VOD	2058
#define PROTO_VOD2	3000


#define PACKHEAD_LENGTH 5
#define TCP_PACK_LENGTH (32*1024)


#define VOD2_HEAD 40
#define VOD2_DATAPACK 1400
#define VOD2_PACK (VOD2_HEAD+VOD2_DATAPACK)
#define VOD2_MAXLEN (VOD2_DATAPACK*8)
#define VOD2_TIMEOUT 100
#define VOD2_OVERTIMES 10

#define HTTP_GET	0
#define HTTP_POST	1
#define VOD_NULL	2


#define HTTPBUFFERCACHESIZE (1024*10)

enum{
	FILENAME_CS=0X01,
	FILELENGTH_SC,
	REQUEST_CS,
	FILEEND_SC,
	SEEK_CS,
	ID_SC,
	ID_CS,
	IDERROR_CS,
	PAYMENT_SC,
	DATA_SC,
	GET_IT
};//the commands in c/s communication


#define CMD_VIDEOREQUEST_CS 0x0001
#define CMD_VIDEODATA_CS	0x0002
#define CMD_VIDEOPLAY_CS	0x0003
#define CMD_VIDEOJUMP_CS	0x0004
#define CMD_VIDEOTIME_CS	0x0005

#define CMD_VIDEOCLOSE_CS	0x0006

#define CMD_VIDEOREPLY_SC	0x0101
#define CMD_VIDEOERROR_SC	0x0102
#define CMD_REDIRECTION_SC	0x0103
#define CMD_VIDEOEND_SC		0x0104

#define CMD_VIDEODATA_SC	0x0202
#define CMD_VIDEOPLAY_SC	0x0203
#define CMD_VIDEOJUMP_SC	0x0204
#define CMD_VIDEOTIME_SC	0x0205

TDINT gVideoID=0;

TDBOOL gVideoCmdExist = TDFALSE;
TDVideoCommand gVideoCmd = {0,0};


#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2   /*3 lines added by wanglq*/


TDBYTE gFileBuffer[SECTION_LENGTH];
TDINT  gFileStart=0,gFileSize=0,gSectionNumber=0,gRootNumber=0;
TDBOOL gHighOrder=TDFALSE;
#define SIGN_START 0

/*struct of file*/
typedef struct TDFile{
	TDINT		mLength;
	TDINT		mCurPos;
	TDPString	mFileName;
	TDDWORD		mFileType;
	TDHANDLE	mFileData;
}TDFile,*TDPFile;

/*struct of System file data*/
typedef struct TDSystemFileData{

	TDHANDLE mFileHandle;	

}TDSystemFileData,*TDPSystemFileData;

/*http file data*/
typedef struct TDHttpFileData{
	TDHANDLE	mSocket;
	TDPBYTE		mBuffer;
	TDINT		mCurLen;
	TDBOOL		mHTMLContent;
}TDHttpFileData,*TDPHttpFileData;

/*vod file data*/
typedef struct TDVodFileData{
	TDHANDLE	mTcpSocket;
	TDHANDLE	mUdpSocket;
	TDBOOL		mFileRequest;
	TDINT		mBufferRemain;
}TDVodFileData,*TDPVodFileData;


/*vod2 file data*/
typedef struct TDVod2FileData{
	TDHANDLE	mSocket;
	TDINT		mVideoID;
	TDINT		mTaskID;
	TDINT		mTimeID;
	TDINT		mVideoTime;
	TDINT		mVideoType;
	TDINT		mPlayMode;
	TDDWORD		mSeq;
	TDINT		mReadLen;
	TDBOOL		mReadEnd;

	void* mReadEvent;	
	pthread_t mReadThread;
//	TDHANDLE  mReadEvent;
//	TDHANDLE  mReadThread;

	TDBOOL		mThreadStop;
	TDCHAR		mBuf[VOD2_MAXLEN];
}TDVod2FileData,*TDPVod2FileData;

typedef struct TDParam{
	TDString mName,mValue;
}TDParam;

typedef struct TDURL{
	TDINT		mProto;
	TDINT		mTcpPort;
	TDINT		mUdpPort;
	TDINT		mMethod;
	TDString	mHost;
	TDString	mPath;
	TDString	mParam;
}TDURL,*TDPURL;



TDBOOL
TDURL_Destroy(TDPURL pURL)
{
	if(!pURL)
		return TDFALSE;
	TDString_DTDString(&pURL->mHost);
	TDString_DTDString(&pURL->mPath);
	TDString_DTDString(&pURL->mParam);
	TDFree(pURL);
	return TDTRUE;
}

TDPURL		
TDParseURL(TDCHAR * url)
{
	TDPURL  pThis;
	TDINT   nPort,end=0,proto=PROTO_HTTP,method;
	TDString src,host,path,param;
	if(!url )
		return TDNULL;
	TDString_Init(&src);
	TDString_InitByChar(&src,url);

	if(src.mLength<=0)
		return TDNULL;
	TDString_StripWhitespace(&src);
	/*protocal*/
	end=TDString_FindPChar(&src,"://",TDTRUE);
	if(end>0)
	{
		TDString pro;
		TDString_Init(&pro);
		TDString_Mid(&src,&pro,0,end);
		if(TDString_EqPChar(&pro,"vod",TDTRUE))
		{
			proto=PROTO_VOD;
		}
		
		if(TDString_EqPChar(&pro,"vod2",TDTRUE))
		{
			proto=PROTO_VOD2;
		}

		if(TDString_EqPChar(&pro,"http",TDTRUE))
		{
			proto=PROTO_HTTP;
			//TDDebug("3");
		}
		if(TDString_EqPChar(&pro,"awb",TDTRUE))
		{
			proto=PROTO_AWB;
		}
		nPort = proto;
		TDString_DTDString(&pro);
		TDString_Cut(&src,0,end+3);
		//TDDebug("4");

	}

	if(!src.mLength)
		return TDNULL;
	/*host*/
	TDString_Init(&host);
	end=TDString_FindChar(&src,'/',0,TDTRUE);
	if(end<1)
	{
		TDString_Assign(&host,&src,-1);
		TDString_Cut(&src,0,src.mLength);
	}
	else
	{
		TDString_Left(&src,&host,end);
		TDString_Cut(&src,0,end);
	}
	/*port*/
	if(host.mLength)
	{
		end=TDString_FindChar(&host,':',0,TDTRUE);
		if(end>0)
		{
			TDString port;
			TDINT err;
			TDString_Init(&port);
			TDString_Mid(&port,&host,end+1,host.mLength);
			TDString_Cut(&host,end,host.mLength);
			nPort=TDString_ToInteger(&port,&err,10);
			if(nPort<0 || nPort>=65535)
				nPort=proto;
			TDString_DTDString(&port);
		}
	}
	/*path*/
	TDString_Init(&path);
	TDString_Init(&param);
	if(src.mLength)
	{
		end=TDString_FindPChar(&src,"??",TDTRUE);
		if(end<0)
		{
			TDString_Assign(&path,&src,-1);
			TDString_Cut(&src,0,src.mLength);
		}
		else
		{
			TDString_Left(&src,&path,end);
			TDString_Cut(&src,0,end);
			TDString_Assign(&param,&src,-1);
			TDString_Cut(&param,0,2);
		}
	}

	switch(proto)
	{
		case PROTO_HTTP:
			if(param.mLength)
				method = HTTP_POST;
			else 
				method = HTTP_GET;
			break;
	}

	TDString_DTDString(&src);
	pThis=(TDPURL)Malloc(sizeof(TDURL));
	pThis->mTcpPort=nPort;
	if(proto == PROTO_VOD)
		pThis->mUdpPort = nPort+1;
	
	if(proto == PROTO_VOD2)
		pThis->mUdpPort = nPort;
	if(proto == PROTO_AWB)
		pThis->mUdpPort = nPort;
	pThis->mProto=proto;
	pThis->mMethod = method;
	TDString_Init(&pThis->mHost);
	TDString_Init(&pThis->mPath);
	TDString_Init(&pThis->mParam);
	TDString_Assign(&pThis->mPath,&path,-1);
	if(path.mLength<=0)
		TDString_AssignPChar(&pThis->mPath,"/");
	TDString_Assign(&pThis->mHost,&host,-1);
	TDString_Assign(&pThis->mParam,&param,-1);
	TDString_DTDString(&path);
	TDString_DTDString(&host);
	TDString_DTDString(&param);

	return pThis;
}
TDBOOL gUsingProxy=TDFALSE;
TDPString gProxy=0;
TDWORD gProxyPort = 80;

TDHANDLE
TDHttpFileOpen(const TDPString dstFile)
{
	TDPFile	pFile;
	TDPURL  pUrl;
	TDHANDLE	http_socket;
	TDPBYTE http_buf;TDINT http_buf_len;
	TDINT i,curlen,filelength=0,headlen;
	TDString pBuf;
	TDBOOL	bhtmlcontent = TDFALSE;
	TDBOOL bEnd  = TDFALSE;
	TDPString pdstFile,pCookie;
	pdstFile = TDString_Create();
	TDString_InitByString(pdstFile,dstFile);
	
	pUrl = TDParseURL(pdstFile->mStr);
	if(!pUrl)
	{
		TDDebug(pdstFile->mStr);
		TDDebug("url parse error");
		TDString_Destroy(pdstFile);
		return TDNULL;
	}
	
	if(gUsingProxy)
	{
		//TDDebug("gUsingProxy");	
		http_socket = TDSocketOpen(gProxy,0,gProxyPort,SOCKET_STREAM_TYPE);
	}
	else
	{
		//TDDebug("!gUsingProxy");	
		http_socket = TDSocketOpen(&pUrl->mHost,0,pUrl->mTcpPort,SOCKET_STREAM_TYPE);
	}
	//TDDebug("!!gUsingProxy");	
	
	if(!http_socket)
	{
		TDDebug(pUrl->mHost.mStr);
		TDDebug("socket open error");
		TDURL_Destroy(pUrl);
		TDString_Destroy(pdstFile);
		return TDNULL;
	}
	//TDDebug("!!!gUsingProxy");	

	TDString_Init(&pBuf);
	if(pUrl->mMethod==HTTP_GET)
		TDString_AppendPChar(&pBuf,"GET ");
	else
		TDString_AppendPChar(&pBuf,"POST ");
	
	if(gUsingProxy)
	{
		TDString_AppendPChar(&pBuf,"http://");
		TDString_AppendString(&pBuf,&pUrl->mHost,-1);
	}

	TDString_AppendString(&pBuf,&pUrl->mPath,-1);
	TDString_AppendPChar(&pBuf," HTTP/1.0\r\n");

	TDString_AppendPChar(&pBuf,"Host: ");
	TDString_AppendString(&pBuf,&pUrl->mHost,-1);
	TDString_AppendPChar(&pBuf,"\r\n");
	TDString_AppendPChar(&pBuf,"Accept: text/html, text/plain, text/sgml, */*;q=0.01\r\n");


/*szj	pCookie=TDCookieGet(&pUrl->mHost,&pUrl->mPath);
	if(pCookie)
	{
		TDString_AppendPChar(&pBuf,"Cookie2: $Version=\"1\"\r\nCookie:");
		TDString_AppendString(&pBuf,pCookie,-1);
		TDString_AppendPChar(&pBuf,"\r\n");
	}
*/
	
	if(pUrl->mMethod==HTTP_POST)
	{
		/*should send the param length and the parameter*/
		TDString_AppendPChar(&pBuf,"Content-type: application/x-www-form-urlencoded\r\n");
		TDString_AppendPChar(&pBuf,"Content-length: ");
		TDString_AppendInt(&pBuf,pUrl->mParam.mLength,10);
		TDString_AppendPChar(&pBuf,"\r\n\r\n");
		TDString_AppendString(&pBuf,&pUrl->mParam,-1);
	}
	else
	{
		TDString_AppendPChar(&pBuf,"Pragma: no-cache\r\nCache-Control: no-cache\r\n");
		TDString_AppendPChar(&pBuf,"User-Agent: Lynx/2.8.3dev.20 libwww-FM/2.14FM");
		TDString_AppendPChar(&pBuf,"\r\n\r\n");
	}

	TDSocketWriteUntilDone((TDHANDLE)http_socket,pBuf.mStr,pBuf.mLength);
        
	http_buf = (TDPBYTE)Malloc(HTTPBUFFERCACHESIZE+1);
	http_buf_len=HTTPBUFFERCACHESIZE;
	TDString_DTDString(&pBuf);
	headlen=0;
	while(1)
	{
//		http_buf = (TDPBYTE)Realloc(http_buf,headlen+HTTPBUFFERCACHESIZE+1);
		if(http_buf_len<headlen+HTTPBUFFERCACHESIZE+1)
		{
			TDPBYTE tmp;
			tmp = (TDPBYTE)Malloc(headlen+HTTPBUFFERCACHESIZE+16*1024);
			memcpy(tmp,http_buf,headlen);
			TDFree(http_buf);
			http_buf=tmp;
			http_buf_len=headlen+HTTPBUFFERCACHESIZE+16*1024;
		}
		
		curlen=TDSocketRead(http_socket,http_buf+headlen,HTTPBUFFERCACHESIZE);
		
		if(curlen==-1)
		{
			TDString_DTDString(&pBuf);
			TDSocketClose(http_socket);
			TDString_Destroy(pdstFile);
			TDFree(http_buf);
			TDURL_Destroy(pUrl);
			TDDebug("socket read error");
			return TDNULL;
		}
		if(curlen==0)
			bEnd = TDTRUE;
		if(!bEnd)
		{
			headlen+=curlen;
			http_buf[headlen]=0;
			TDString_AssignPChar(&pBuf,(TDPCHAR)http_buf);
//			TDDebug(pBuf.mStr);
		}

		if(bEnd)
			break;
		if(-1!=TDString_FindPChar(&pBuf,"\r\n\r\n",TDTRUE))
		{
			if(-1!=TDString_FindPChar(&pBuf,"Content-Length:",TDTRUE))
				break;
		}
	}

	if(0==TDString_FindPChar(&pBuf,"HTTP/1.",TDTRUE))
	{/*this is a http response*/
		TDINT status=0;
		
		i=TDStrLen("HTTP/1.");
		while(TDString_IsDigit(TDString_CharAt(&pBuf,i)))i++;
		while(TDString_IsSpace(TDString_CharAt(&pBuf,i)))i++;
		
		i = TDString_GetIntegerFrom(&pBuf,i,&status);
		switch(status)
		{
		case 302:
				{/*Cookie*/
					TDINT start=0,end=0;
					start=TDString_FindPChar(&pBuf,"Set-Cookie",TDTRUE);
					while(start>=0)
					{
//						TDDebug(pBuf.mStr);
						start+=11;
						while(TDString_IsSpace(TDString_CharAt(&pBuf,start)))start++;
						end=TDString_FindPCharFrom(&pBuf,"\n",TDTRUE,start);
						if(end>=0)
						{
							TDString cook;
							TDString_Init(&cook);
							TDString_AssignPCharN(&cook,pBuf.mStr+start,end-start);
//szj							TDCookieSet(&pUrl->mHost,&cook);
						}
						start=TDString_FindPCharFrom(&pBuf,"Set-Cookie",TDTRUE,end);
					}
				}
			
			if((i=TDString_FindPChar(&pBuf,"\r\nReferer:",TDTRUE))>0)
			{
				TDINT lend;
				TDPString newurl;
				i+=TDStrLen("\r\nReferer:");
				while(TDString_IsSpace(TDString_CharAt(&pBuf,i)))i++;
				lend = TDString_FindPCharFrom(&pBuf,"\r\n",TDFALSE,i);
				if(lend>i)
				{
					newurl = TDString_Create();
					TDString_AppendString(newurl,&pBuf,lend);
					TDString_Cut(newurl,0,i);
					//				TDString_Cut(newurl,lend-i,-1);
					if(TDString_FindPChar(newurl,"http://",TDTRUE)<0)
					{
						/*Referer is not absolute url*/
						TDString_InsertChar(newurl,'/',0);
						TDString_Insert(newurl,&pUrl->mHost,0,-1);
						TDString_InsertPChar(newurl,"http://",0,-1);
					}
					TDString_DTDString(&pBuf);
					TDSocketClose(http_socket);
					TDString_Destroy(pdstFile);
					TDFree(http_buf);
					return TDHttpFileOpen(newurl);
				}
			}		
			if((i=TDString_FindPChar(&pBuf,"\r\nLocation: ",TDTRUE))>0)
			{
				TDINT lend;
				TDPString newurl;
				i+=12;
				while(TDString_IsSpace(TDString_CharAt(&pBuf,i)))i++;
				lend = TDString_FindPCharFrom(&pBuf,"\r\n",TDFALSE,i);
				if(lend>i)
				{
					TDPString p;
					p=TDString_Create();
					TDString_AssignPChar(p,"http://");
					TDString_AppendString(p,&pUrl->mHost,-1);
					TDString_AppendString(p,&pUrl->mPath,-1);
					TDFileMakePathDir(p);
					newurl = TDString_Create();
					TDString_AppendString(newurl,&pBuf,lend);
					TDString_Cut(newurl,0,i);
	/*				//				TDString_Cut(newurl,lend-i,-1);
					if(TDString_FindPChar(newurl,"http://",TDTRUE)<0)
					{

						TDString_InsertChar(newurl,'/',0);
						TDString_Insert(newurl,&pUrl->mHost,0,-1);
						TDString_InsertPChar(newurl,"http://",0,-1);
					}
					*/
					TDFileMakeSureAbsPath(p,newurl);
					TDString_Destroy(p);
					TDString_DTDString(&pBuf);
					TDSocketClose(http_socket);
					TDString_Destroy(pdstFile);
					TDFree(http_buf);
					TDDebug("Location To: ");
					TDDebug(newurl->mStr);
					return TDHttpFileOpen(newurl);
				}
			}
			break;
		case 200:
		default:
			while(TDString_IsSpace(TDString_CharAt(&pBuf,i)))i++;
			if(i==TDString_FindPCharFrom(&pBuf,"OK",TDTRUE,i))
			{
				/*the response is ok*/
				TDINT content=0;
				content=TDString_FindPCharFrom(&pBuf,"Content-Type:",TDTRUE,i);
				if(content!=-1)
				{
					content=TDString_FindPCharFrom(&pBuf,"text/html",TDTRUE,content);
					if(content!=-1)
					{
						bhtmlcontent=TDTRUE;
					}
				}
				i=TDString_FindPCharFrom(&pBuf,"Content-Length:",TDTRUE,i);
				{/*Cookie*/
					TDINT start=0,end=0;
					start=TDString_FindPChar(&pBuf,"Set-Cookie",TDTRUE);
					while(start>=0)
					{
//						TDDebug(pBuf.mStr);
						start+=11;
						while(TDString_IsSpace(TDString_CharAt(&pBuf,start)))start++;
						end=TDString_FindPCharFrom(&pBuf,"\n",TDTRUE,start);
						if(end>=0)
						{
							TDString cook;
							TDString_Init(&cook);
							TDString_AssignPCharN(&cook,pBuf.mStr+start,end-start);
//szj						TDCookieSet(&pUrl->mHost,&cook);
						}
						start=TDString_FindPCharFrom(&pBuf,"Set-Cookie",TDTRUE,end);
					}
				}
				if(i!=-1)
				{
					TDINT end;
					TDString len;
					i+=15;
					while(TDString_IsSpace(TDString_CharAt(&pBuf,i)))i++;
					end=i;
					while(TDString_IsDigit(TDString_CharAt(&pBuf,end)))end++;
					TDString_Init(&len);
					TDString_Mid(&pBuf,&len,i,end-i);
					filelength=TDString_ToInteger(&len,&end,10);
					end=TDString_FindPCharFrom(&pBuf,"\r\n\r\n",TDFALSE,end);
					end+=4;
					TDString_DTDString(&len);
					headlen-=end;
					memcpy(http_buf,http_buf+end,headlen);
				}
				else
				{
					TDINT end;
					end=TDString_FindPChar(&pBuf,"\r\n\r\n",TDFALSE);
					end+=4;
					headlen-=end;
					memcpy(http_buf,http_buf+end,headlen);
					filelength = headlen;
				}
			}
			break;
		}
	}
	TDString_DTDString(&pBuf);
	pFile = (TDPFile)Malloc(sizeof(TDFile));
	pFile->mFileType = WHX_HTTP_FILE;
	pFile->mCurPos =0;
	pFile->mFileName = pdstFile;
	pFile->mLength = filelength;
	pFile->mFileData = (TDHANDLE)Malloc(sizeof(TDHttpFileData));
	((TDHttpFileData*)pFile->mFileData)->mSocket = http_socket;
	((TDHttpFileData*)pFile->mFileData)->mBuffer = http_buf;
	((TDHttpFileData*)pFile->mFileData)->mCurLen = headlen;
	((TDHttpFileData*)pFile->mFileData)->mHTMLContent = bhtmlcontent;

	TDURL_Destroy(pUrl);

	return (TDHANDLE)pFile;
}

TDHANDLE
TDVodFileOpen(TDPString dstFile)
{
	TDPFile pFile;
	TDPString pdstFile;
	TDPURL  pUrl;
	TDHANDLE	tcp_socket;
	TDPBYTE pBuf;
	TDINT filelength;
	TDBOOL success;
//TDDebug(dstFile->mStr);
	pdstFile = TDString_Create();
	TDString_InitByString(pdstFile,dstFile);
	pUrl = TDParseURL(pdstFile->mStr);
	
	if(!pUrl)
	{
		TDString_Destroy(pdstFile);
		return TDNULL;
	}
	{
		TDINT end;
		end=TDString_FindPChar(pdstFile,"|||/",TDTRUE);
		if(end!=-1)
		{
			TDPString newfile;
			TDHANDLE handle;

			TDString_Cut(pdstFile,0,end+3);
			newfile = TDString_Create();
			TDString_InitByChar(newfile,"vod2://");
			TDString_AppendString(newfile,&pUrl->mHost,-1);
			TDString_AppendString(newfile,pdstFile,-1);
			handle = TDFileOpen(newfile->mStr);
			TDString_Destroy(newfile);
			TDString_Destroy(pdstFile);
			return handle;
		}
	}
	pBuf =(TDPBYTE)Malloc(1024);
	tcp_socket = TDSocketOpen(&pUrl->mHost,0,pUrl->mTcpPort,SOCKET_STREAM_TYPE);

	if(!tcp_socket)
	{
		TDString_Destroy(pdstFile);
		TDFree(pBuf);
		return TDNULL;
	}
	/*send request file name*/
	{
		TDSHORT namelength;
		pBuf[0] =  FILENAME_CS;
		namelength = (TDSHORT)pUrl->mPath.mLength;
		memcpy(pBuf+1,&namelength,4);
		//*((TDPSHORT)(pBuf+1)) = pUrl->mPath.mLength;
		memcpy(pBuf+PACKHEAD_LENGTH,pUrl->mPath.mStr,namelength);
		if(-1==TDSocketWriteUntilDone(tcp_socket,pBuf,PACKHEAD_LENGTH+namelength))

		{
			TDSocketClose(tcp_socket);
			TDString_Destroy(pdstFile);
			TDFree(pBuf);
			return TDNULL;
		}
	}

	success = TDFALSE;
	while(!success)
	{
		if(-1==TDSocketReadUntilDone(tcp_socket,pBuf,PACKHEAD_LENGTH))
		{
			TDSocketClose(tcp_socket);
			TDString_Destroy(pdstFile);
			TDFree(pBuf);
			return TDNULL;
		}
		switch(pBuf[0])
		{
		case ID_SC:
			//return id;
#ifdef HAVE_IC
			{
				WORD wRet;
				TDDWORD uid;
				TDCHAR btemp[9];

				if(MCS_SUCCESS == MCS_TestDoor())
				{
					if(MCS_SUCCESS == AT24C_OpenCard(AT24C04))
					{
						wRet = AT24C_ReadValue(88,&uid);
						AT24C_CloseCard();
						if(wRet==MCS_SUCCESS)
						{
							btemp[0] = ID_CS;
							memcpy(btemp+PACKHEAD_LENGTH,(TDPCHAR)&uid,4);
							if(-1==TDSocketWriteUntilDone(tcp_socket,btemp,4+PACKHEAD_LENGTH))
							{
								TDSocketClose(tcp_socket);
								TDString_Destroy(pdstFile);
								TDFree(pBuf);
								return TDNULL;
							}
							break;
						}
					}
				}
				
				btemp[0] = IDERROR_CS;
				if(-1==TDSocketReadUntilDone(tcp_socket,btemp,PACKHEAD_LENGTH))
				{
					TDSocketClose(tcp_socket);
					TDString_Destroy(pdstFile);
					TDFree(pBuf);
					return TDNULL;
				}

			}
#endif		
			break;
		case FILELENGTH_SC:
			if(-1==TDSocketReadUntilDone(tcp_socket,pBuf,4))
			{
				TDSocketClose(tcp_socket);
				TDString_Destroy(pdstFile);
				TDFree(pBuf);
				return TDNULL;
			}
			filelength = *((TDINT*)pBuf);
			success = TDTRUE;
			break;
		default:
			TDSocketClose(tcp_socket);
			TDString_Destroy(pdstFile);
			TDFree(pBuf);
			return TDNULL;
		}
		
	}

	pFile = (TDPFile)Malloc(sizeof(TDFile));
	pFile->mFileType = WHX_VOD_FILE;
	pFile->mCurPos =0;
	pFile->mFileName = pdstFile;
	pFile->mLength = filelength;
	pFile->mFileData = (TDHANDLE)Malloc(sizeof(TDVodFileData));
	((TDVodFileData*)pFile->mFileData)->mTcpSocket = tcp_socket;
	((TDVodFileData*)pFile->mFileData)->mUdpSocket = TDNULL;
	((TDVodFileData*)pFile->mFileData)->mFileRequest = TDFALSE;
	((TDVodFileData*)pFile->mFileData)->mBufferRemain = 0;

	TDFree(pBuf);
	return (TDHANDLE)pFile;
}

TDHANDLE
TDWhxFileOpen(TDPString dstFile)
{
	return TDNULL;
}

TDHANDLE //WINAPI
Vod2FileReadThread(TDPVOID pvoid)
{
	TDPFile pThis;
	TDPVod2FileData vod2filedata;
	TDBYTE pRecvBuf[VOD2_PACK];
	TDSHORT recvcmdtype;
	TDDWORD recvcmdlen;
	TDINT readpacks=0,ret;

	if(!pvoid)
		return 0;
	
	pThis = (TDPFile)pvoid;
	

	vod2filedata = (TDPVod2FileData)pThis->mFileData;

	while(!vod2filedata->mThreadStop)
	{
		ret=TDSocketBlockRead(vod2filedata->mSocket,pRecvBuf,VOD2_PACK);
		if(ret!=-1)
		{
			TDINT timeid,videoid,taskid,packnum,packindex;
			memcpy(&recvcmdtype,pRecvBuf,2);
			memcpy(&timeid,pRecvBuf+2,4);
			memcpy(&videoid,pRecvBuf+6,4);
			memcpy(&taskid,pRecvBuf+10,4);
			memcpy(&recvcmdlen,pRecvBuf+14,4);
			memcpy(&packnum,pRecvBuf+18,4);
			memcpy(&packindex,pRecvBuf+22,4);
		
			if(recvcmdtype== CMD_VIDEOPLAY_SC && vod2filedata->mTimeID==timeid && vod2filedata->mVideoID == videoid && vod2filedata->mTaskID==taskid )
				SetEvent(vod2filedata->mReadEvent);
 				
			if(recvcmdtype==CMD_REDIRECTION_SC && vod2filedata->mTimeID==timeid && vod2filedata->mVideoID == videoid && vod2filedata->mTaskID==taskid )
			{
				TDDWORD ipaddr;
				TDSHORT port;
				memcpy(&ipaddr,pRecvBuf+VOD2_HEAD,4);
				memcpy(&port,pRecvBuf+VOD2_HEAD+4,2);
				TDSocketSetServeraddr(vod2filedata->mSocket,ipaddr,port);
				vod2filedata->mReadLen = 0;				
				readpacks = 0;
				SetEvent(vod2filedata->mReadEvent);

			}
			
			if(recvcmdtype==CMD_VIDEOEND_SC && vod2filedata->mTimeID==timeid && vod2filedata->mVideoID == videoid
				&& vod2filedata->mTaskID==taskid )
			{
				vod2filedata->mReadLen = 0;				
				readpacks = 0;
				vod2filedata->mReadEnd = TDTRUE;
				SetEvent(vod2filedata->mReadEvent);
			}

			if(recvcmdtype==CMD_VIDEODATA_SC && vod2filedata->mTimeID==timeid && vod2filedata->mVideoID == videoid && vod2filedata->mTaskID==taskid )
			{
				memcpy(vod2filedata->mBuf+packindex*VOD2_DATAPACK,pRecvBuf+VOD2_HEAD,recvcmdlen);
				if(!vod2filedata->mReadLen)
					readpacks = 0;
				readpacks++;
				vod2filedata->mReadLen+=recvcmdlen;
				if(readpacks==packnum)
				{
					SetEvent(vod2filedata->mReadEvent);

				}
			}
		}
	}

	return 1;
}


TDHANDLE
TDVod2FileOpen(TDPString dstFile)
{
	TDPFile pFile;
	TDPString pdstFile;
	TDPURL  pUrl;
	TDHANDLE	file_socket;
	TDBYTE pBuf[VOD2_PACK],pRecvBuf[VOD2_PACK];
	TDINT videoid,timeid=1,taskid=0,cmdlen,videotime;
	TDINT namelength,overtime=0;
	TDWORD cmdtype;
	TDINT ret;
	TDCHAR videotype;

	pdstFile = TDString_Create();
	TDString_InitByString(pdstFile,dstFile);

	pUrl = TDParseURL(pdstFile->mStr);
	if(!pUrl)
	{
		TDString_Destroy(pdstFile);
		return TDNULL;
	}
	file_socket = TDSocketOpen(&pUrl->mHost,0,pUrl->mUdpPort,SOCKET_DGRAM_TYPE);
	if(!file_socket)
	{
		TDString_Destroy(pdstFile);
		return TDNULL;
	}
	TDSocketSetRecvTimeout(file_socket,1000);
	
	
	
	cmdtype = CMD_VIDEOREQUEST_CS;
	timeid++;
/*wanglq do */
//	videoid = ::GetTickCount();//gVideoID++;
	taskid=0;
	namelength = pUrl->mPath.mLength;
	
	memcpy(pBuf,&cmdtype,2);
	memcpy(pBuf+2,&timeid,4);
	memcpy(pBuf+6,&videoid,4);
	memcpy(pBuf+10,&taskid,4);
	memcpy(pBuf+14,&namelength,4);
	memcpy(pBuf+VOD2_HEAD,pUrl->mPath.mStr,namelength);
	
	if(-1==TDSocketWriteUntilDone(file_socket,pBuf,VOD2_PACK))
	{
		TDSocketClose(file_socket);
		TDString_Destroy(pdstFile);
		return TDNULL;
	}
	
	while(1)
	{
		ret=TDSocketRead(file_socket,pRecvBuf,VOD2_PACK);
		if(ret!=-1)
		{
			TDINT tid,vid;
			memcpy(&cmdtype,pRecvBuf,2);
			memcpy(&tid,pRecvBuf+2,4);
			memcpy(&vid,pRecvBuf+6,4);
			memcpy(&taskid,pRecvBuf+10,4);
			memcpy(&cmdlen,pRecvBuf+14,4);

			if(cmdlen>0)
				memcpy(pRecvBuf,pRecvBuf+VOD2_HEAD,cmdlen);
			//TDSocketRead(file_socket,pRecvBuf,cmdlen);
			
			if(cmdtype==CMD_VIDEOREPLY_SC && tid==timeid && vid==videoid)
				break;
			
			if(cmdtype==CMD_VIDEOERROR_SC && tid==timeid && vid==videoid)
			{
				TDSocketClose(file_socket);
				TDString_Destroy(pdstFile);
				return TDNULL;
			}	
		}
		
		timeid++;
		overtime++;
		if(overtime>3)
		{
			TDSocketClose(file_socket);
			TDString_Destroy(pdstFile);
			return TDNULL;
		}
		memcpy(pBuf+2,&timeid,4);	
		if(-1==TDSocketWriteUntilDone(file_socket,pBuf,VOD2_PACK))
		{
			TDSocketClose(file_socket);
			TDString_Destroy(pdstFile);
			return TDNULL;
		}
	}
	{
		TDDWORD ipaddr;
		TDSHORT port;
//		ret = TDSocketRead(file_socket,pRecvBuf,cmdlen);
		memcpy(&ipaddr,pRecvBuf,4);
		memcpy(&port,pRecvBuf+4,2);
		videotype = pRecvBuf[6];
		TDSocketSetServeraddr(file_socket,ipaddr,port);
		if(videotype)
			memcpy(&videotime,pRecvBuf+7,4);
		else
			videotime = 0;
	}
	TDSocketSetRecvTimeout(file_socket,VOD2_TIMEOUT);
	
	pFile = (TDPFile)Malloc(sizeof(TDFile));
	pFile->mFileType = WHX_VOD2_FILE;
	pFile->mCurPos =0;
	pFile->mFileName = pdstFile;
	pFile->mLength = 0;
	pFile->mFileData = (TDHANDLE)Malloc(sizeof(TDVod2FileData));
	((TDVod2FileData*)pFile->mFileData)->mSocket = file_socket;
	((TDVod2FileData*)pFile->mFileData)->mVideoID = videoid;
	((TDVod2FileData*)pFile->mFileData)->mTaskID = taskid;
	((TDVod2FileData*)pFile->mFileData)->mTimeID = timeid;
	((TDVod2FileData*)pFile->mFileData)->mVideoTime = videotime;
	((TDVod2FileData*)pFile->mFileData)->mVideoType = videotype;
	((TDVod2FileData*)pFile->mFileData)->mPlayMode = 1;
	((TDVod2FileData*)pFile->mFileData)->mReadLen = 0;
	((TDVod2FileData*)pFile->mFileData)->mSeq = 0;
	((TDVod2FileData*)pFile->mFileData)->mReadEnd = TDFALSE;
#ifdef XP_UNIX
	((TDVod2FileData*)pFile->mFileData)->mReadEvent = CreateEvent();
#elif defined XP_PC
	((TDVod2FileData*)pFile->mFileData)->mReadEvent = ::CreateEvent(TDNULL,TDFALSE,TDFALSE,NULL);
#endif

	((TDVod2FileData*)pFile->mFileData)->mThreadStop = TDFALSE;

#ifdef XP_UNIX
	pthread_create(&((TDVod2FileData*)pFile->mFileData)->mReadThread,NULL,(void*)&Vod2FileReadThread,(TDPVOID)pFile);
#if 0	
	((TDVod2FileData*)pFile->mFileData)->mReadThread = fork();	
	if(((TDVod2FileData*)pFile->mFileData)->mReadThread == 0)
	{
		Vod2FileReadThread(pFile);
		exit(0);
		
	}
	else if(((TDVod2FileData*)pFile->mFileData)->mReadThread > 0)
	{
	
		
	}
	else
	{
		return TDNULL;
	}
#endif

#elif defined XP_PC
	((TDVod2FileData*)pFile->mFileData)->mReadThread =::CreateThread(0,0,Vod2FileReadThread,(TDPVOID)pFile,0,0);
#endif

#ifdef XP_PC
	SetThreadPriority(((TDVod2FileData*)pFile->mFileData)->mReadThread,THREAD_PRIORITY_TIME_CRITICAL);
#endif
	gVideoCmdExist = TDFALSE;
	return (TDHANDLE)pFile;
}

TDVOID	TDVod2FileClose( TDHANDLE hFile)
{
	TDCHAR pBuf[VOD2_PACK];
	TDWORD cmdtype;
	TDINT  len = 0;
	TDPFile file;
	TDVod2FileData *vod2filedata;
	file = (TDPFile)hFile;
	vod2filedata = (TDVod2FileData*)file->mFileData;
	cmdtype = CMD_VIDEOCLOSE_CS;

	memcpy(pBuf,&cmdtype,2);
	memcpy(pBuf+10,&(vod2filedata->mTaskID),4);
	memcpy(pBuf+14,&len,4);

	vod2filedata->mThreadStop = TDTRUE;
	TDSocketWriteUntilDone(vod2filedata->mSocket,pBuf,VOD2_PACK);
#ifdef XP_PC
	CloseHandle(vod2filedata->mReadEvent);
#endif
//	::WaitForSingleObject(vod2filedata->mReadThread,VOD2_TIMEOUT*2);
#ifdef XP_PC
	CloseHandle(vod2filedata->mReadThread);
#endif
	TDSocketClose(vod2filedata->mSocket);
	TDFree (vod2filedata);
	return;
}


TDDWORD //WINAPI
AwbFileReadThread(TDPVOID pvoid)
{
	TDFile * pfile=(TDFile *)pvoid;
	TDPAwbFileData awbfiledata;
	TDBYTE pBuf[PGDTSIZE*PGNUM],pRecvBuf[MAXPGSIZE];
	TDINT ret,offset = 0,readtimes=0;
	
	if(!pfile)
		return 0;
	awbfiledata = (TDPAwbFileData)pfile->mFileData;
	while(!awbfiledata->mThreadStop)
	{
		ret=TDSocketBlockRead(awbfiledata->mSocket,pRecvBuf,MAXPGSIZE);
		if(ret!=-1)
		{
			TDINT recv_seqno,recv_offset;
			TDSHORT recv_cmd,recv_times,recv_len;
			
			memcpy(&recv_seqno,pRecvBuf,4);
			memcpy(&recv_cmd,pRecvBuf+8,2);
			
			if(recv_seqno== awbfiledata->mSeqNo && recv_cmd == AW_FLDT)
			{
				memcpy(&recv_len,pRecvBuf+22,2);
				memcpy(&recv_times,pRecvBuf+18,2);
				memcpy(&recv_offset,pRecvBuf+14,4);
				
				memcpy(pBuf+offset,pRecvBuf+24,recv_len);
				offset+=recv_len;
				readtimes++;
				if(readtimes>=recv_times)
				{
					memcpy(awbfiledata->mBuf+awbfiledata->mBlockLen,pBuf,offset);
					awbfiledata->mBlockLen += offset;
					offset = 0;
					readtimes=0;
					SetEvent(awbfiledata->mReadEvent);
				}
			}
		}
		
	}
	return 1;
}

TDHANDLE
TDAwbFileOpen(TDPString dstFile)
{
	TDPFile pFile;
	TDPString pdstFile;
	TDPURL  pUrl;
	TDHANDLE	file_socket;
	TDBYTE *pSendBuf,*pRecvBuf;
	TDINT	seqno = 0,bksize,ret,filesize,overtime=0,bknum;
	TDSHORT cmd,pgdtsize;




	pdstFile = TDString_Create();
	TDString_InitByString(pdstFile,dstFile);

	pUrl = TDParseURL(pdstFile->mStr);
	if(!pUrl)
	{
		TDString_Destroy(pdstFile);
		return TDNULL;
	}


	file_socket = TDSocketOpen(&pUrl->mHost,0,pUrl->mUdpPort,SOCKET_DGRAM_TYPE);
	if(!file_socket)
	{
		TDString_Destroy(pdstFile);
		return TDNULL;
	}
	TDSocketSetRecvTimeout(file_socket,AWB_TIMEOUT);

	pSendBuf = (TDPBYTE)Malloc(MAXPGSIZE);
	pRecvBuf = (TDPBYTE)Malloc(MAXPGSIZE);

	cmd = QU_DTFL;
	pgdtsize = PGDTSIZE;
	bksize = BKSIZE;
	memcpy(pSendBuf+8,&cmd,2);
	memcpy(pSendBuf+10,&bksize,4);
	memcpy(pSendBuf+14,&pgdtsize,2);
	memcpy(pSendBuf+16,pUrl->mPath.mStr,pUrl->mPath.mLength);
	pSendBuf[16+pUrl->mPath.mLength] = 0;
/**/

/*	cmd = QU_SERV;
	pgdtsize = PGDTSIZE;
	bksize = BKSIZE;
	memcpy(pSendBuf+8,&cmd,2);
	memcpy(pSendBuf+10,pUrl->mPath.mStr,pUrl->mPath.mLength);
	pSendBuf[10+pUrl->mPath.mLength] = 0;
/**/

	while(1)
	{
		seqno++;
		memcpy(pSendBuf,&seqno,4);
		if(-1 == TDSocketWrite(file_socket,pSendBuf,17+pUrl->mPath.mLength))
//		if(-1 == TDSocketWrite(file_socket,pSendBuf,11+pUrl->mPath.mLength))
		{
			TDSocketClose(file_socket);
			TDString_Destroy(pdstFile);
			TDFree(pSendBuf);
			TDFree(pRecvBuf);
			return TDNULL;
		}
		ret=TDSocketRead(file_socket,pRecvBuf,MAXPGSIZE);
		if(ret!=-1)
		{
			TDINT recv_seqno;
			TDSHORT recv_cmd;
			memcpy(&recv_seqno,pRecvBuf,4);
			memcpy(&recv_cmd,pRecvBuf+8,2);
			
			if(recv_seqno==seqno && recv_cmd ==AW_DTFL)
				break;
		}
		overtime++;
		if(overtime>3)
		{
			TDSocketClose(file_socket);
			TDString_Destroy(pdstFile);
			TDFree(pSendBuf);
			TDFree(pRecvBuf);
			return TDNULL;
		}
	}

	{
		TDDWORD ipaddr;
		TDSHORT port;
		memcpy(&ipaddr,pRecvBuf+12,4);
		memcpy(&port,pRecvBuf+10,2);
		port = ntohs(port);
		TDSocketSetServeraddr(file_socket,ipaddr,port);
	}
	memcpy(&filesize,pRecvBuf+16,4);
	bknum = filesize/bksize;
	if(bknum*bksize<filesize)
		bknum++;
	pFile = (TDPFile)Malloc(sizeof(TDFile));
	pFile->mFileType = WHX_AWB_FILE;
	pFile->mCurPos =0;
	pFile->mFileName = pdstFile;
	pFile->mLength = filesize;
	pFile->mFileData = (TDHANDLE)Malloc(sizeof(TDAwbFileData));

	TD_ArrayInit(&((TDAwbFileData*)pFile->mFileData)->mFileNames,0);
	((TDAwbFileData*)pFile->mFileData)->mFileNums = 0;
	((TDAwbFileData*)pFile->mFileData)->mFileIndex = 0;
	{
		TDString str;
		TDString TempStr1;
		TDString TempStr2;
		TDINT end,errcode,filenum,fileindex;
		
		TDString_InitByChar(&TempStr1,"");
		TDString_InitByChar(&TempStr2,"");
		TDString_InitByString(&str,&pUrl->mPath);
		end = TDString_FindChar(&str,'.',0,TDTRUE);
		if(end>=2)
		{
			if(1==TDString_Mid(&str,&TempStr1,end-2,1))
			{
				filenum = TDString_ToInteger(&TempStr1,&errcode,10);
				if(filenum>0)
				{
					if(1==TDString_Mid(&str,&TempStr2,end-1,1))
					{
						fileindex = TDString_ToInteger(&TempStr2,&errcode,10);
						if(fileindex>0 && fileindex<=filenum)
						{
							TDINT i;
							TDString_Cut(&str,end-1,1);
							for(i=0;i<filenum-fileindex+1;i++)
							{
								TDPString pStr;
								pStr = TDString_Create();
								TDString_AssignString(pStr,&str,-1);
								TDString_InsertChar(pStr,TempStr2.mStr[0]+i,end-1);
								TD_ArrayAppendElement(&((TDAwbFileData*)pFile->mFileData)->mFileNames,pStr);
							}
							((TDAwbFileData*)pFile->mFileData)->mFileNums = filenum-fileindex+1;
							((TDAwbFileData*)pFile->mFileData)->mFileIndex = fileindex-1;	
						}
					}

				}
			}

		}
		TDString_DTDString(&TempStr1);
		TDString_DTDString(&TempStr2);
		TDString_DTDString(&str);
	}
	((TDAwbFileData*)pFile->mFileData)->mSocket = file_socket;
	((TDAwbFileData*)pFile->mFileData)->mSeqNo = seqno;
	((TDAwbFileData*)pFile->mFileData)->mBlockNum = bknum;
	((TDAwbFileData*)pFile->mFileData)->mBlockNo = 0;
	((TDAwbFileData*)pFile->mFileData)->mBlockOff = 0;
	((TDAwbFileData*)pFile->mFileData)->mBlockLen = 0;
//	((TDAwbFileData*)pFile->mFileData)->mReadEvent = ::CreateEvent(TDNULL,TDFALSE,TDFALSE,NULL);
	((TDAwbFileData*)pFile->mFileData)->mThreadStop = TDFALSE;
//	((TDAwbFileData*)pFile->mFileData)->mReadThread =::CreateThread(0,0,AwbFileReadThread,(TDPVOID)pFile,0,0);
//	SetThreadPriority(((TDAwbFileData*)pFile->mFileData)->mReadThread,THREAD_PRIORITY_TIME_CRITICAL);
	TDFree(pSendBuf);
	TDFree(pRecvBuf);
	return (TDHANDLE)pFile;
}

TDVOID	
TDAwbFileClose(TDHANDLE hFile)
{
	TDFile * pfile=(TDFile *)hFile;
	TDPAwbFileData awbfiledata;
	TDSHORT cmd;
	TDBYTE pSendBuf[MAXPGSIZE];
	TDINT i;
	if(!pfile)
		return;
	awbfiledata = (TDPAwbFileData)pfile->mFileData;
	awbfiledata->mThreadStop = TDTRUE;

	awbfiledata->mSeqNo++;
	cmd = QU_STOP;

	memcpy(pSendBuf,&awbfiledata->mSeqNo,4);
	memcpy(pSendBuf+8,&cmd,2);
	TDSocketWrite(awbfiledata->mSocket,pSendBuf,10);
/*	if(awbfiledata->mReadThread)
	{
		::WaitForSingleObject(awbfiledata->mReadThread,AWB_TIMEOUT*2);
		CloseHandle(awbfiledata->mReadThread);
	}
	CloseHandle(awbfiledata->mReadEvent);
*/	TDSocketClose(awbfiledata->mSocket);
	for(i=0;i<awbfiledata->mFileNums;i++)
	{
		TDPString pStr;
		pStr = (TDPString)TD_ArrayElementAt(&awbfiledata->mFileNames,i);
		TDString_Destroy(pStr);
	}
	TD_ArrayClear(&awbfiledata->mFileNames);
	TDFree (awbfiledata);
	return;
}

TDBOOL
TDAwbFileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes)
{
	TDFile * pfile=(TDFile *)hFile;
	TDPAwbFileData awbfiledata;
	TDSHORT cmd,pgnum;

	TDBYTE pSendBuf[MAXPGSIZE];
	TDCHAR pRecvBuf[MAXPGSIZE];

	TDWORD ret,overtime=0;    //wanglq change DWORD to TDWORD

	TDINT	bksize,filesize,bknum;
	TDSHORT pgdtsize;

	TDPURL pUrl;
	
	*ReadBytes = 0;
	if(!pfile)
		return TDFALSE;
	awbfiledata = (TDPAwbFileData)pfile->mFileData;

	/*file end*/
	if(pfile->mCurPos>=pfile->mLength)
	{
		if(awbfiledata->mFileIndex+1>=awbfiledata->mFileNums)
			return TDTRUE;
		else
		{
			TDPString pFileName;
			awbfiledata->mSeqNo++;
			cmd = QU_STOP;
			awbfiledata->mThreadStop = TDTRUE;

			memcpy(pSendBuf,&awbfiledata->mSeqNo,4);
			memcpy(pSendBuf+8,&cmd,2);
			TDSocketWrite(awbfiledata->mSocket,pSendBuf,10);
#ifdef XP_PC
			WaitForSingleObject(awbfiledata->mReadThread,AWB_TIMEOUT*2);
			CloseHandle(awbfiledata->mReadThread);
#endif
			awbfiledata->mReadThread = 0;//NULL;
//			::ResetEvent(awbfiledata->mReadEvent);
			TDSocketClose(awbfiledata->mSocket);
			awbfiledata->mSeqNo = 0;
			
			
			awbfiledata->mFileIndex++;
			pFileName = (TDString*)TD_ArrayElementAt(&awbfiledata->mFileNames,awbfiledata->mFileIndex);
			if(!pFileName)
				return TDTRUE;
			cmd = QU_DTFL;
			pgdtsize = PGDTSIZE;
			bksize = BKSIZE;
			memcpy(pSendBuf+8,&cmd,2);
			memcpy(pSendBuf+10,&bksize,4);
			memcpy(pSendBuf+14,&pgdtsize,2);
			memcpy(pSendBuf+16,pFileName->mStr,pFileName->mLength);
			pSendBuf[16+pFileName->mLength] = 0;
			pUrl = TDParseURL(pfile->mFileName->mStr);
			if(!pUrl)
			{
				return TDFALSE;
			}
			
			awbfiledata->mSocket = TDSocketOpen(&pUrl->mHost,0,pUrl->mUdpPort,SOCKET_DGRAM_TYPE);
			
			
			/**/
			
			while(1)
			{
				awbfiledata->mSeqNo++;
				memcpy(pSendBuf,&awbfiledata->mSeqNo,4);
				if(-1 == TDSocketWrite(awbfiledata->mSocket,pSendBuf,17+pFileName->mLength))
					//		if(-1 == TDSocketWrite(file_socket,pSendBuf,11+pUrl->mPath.mLength))
				{
					return TDFALSE;
				}
				ret=TDSocketRead(awbfiledata->mSocket,pRecvBuf,MAXPGSIZE);
				if(ret!=-1)
				{
					TDINT recv_seqno;
					TDSHORT recv_cmd;
					memcpy(&recv_seqno,pRecvBuf,4);
					memcpy(&recv_cmd,pRecvBuf+8,2);
					
					if(recv_seqno==awbfiledata->mSeqNo && recv_cmd ==AW_DTFL)
						break;
				}
				overtime++;
				if(overtime>3)
				{
					return TDFALSE;
				}
			}
			
			{
				TDDWORD ipaddr;
				TDSHORT port;
				memcpy(&ipaddr,pRecvBuf+12,4);
				memcpy(&port,pRecvBuf+10,2);
				port = ntohs(port);
				TDSocketSetServeraddr(awbfiledata->mSocket,ipaddr,port);
			}
			memcpy(&filesize,pRecvBuf+16,4);
			bknum = filesize/bksize;
			if(bknum*bksize<filesize)
				bknum++;

			pfile->mCurPos =0;
			pfile->mLength = filesize;
			awbfiledata->mBlockNum = bknum;
			awbfiledata->mBlockNo = 0;
			awbfiledata->mBlockOff = 0;
			awbfiledata->mBlockLen = 0;
			awbfiledata->mThreadStop = TDFALSE;
//			awbfiledata->mReadThread =::CreateThread(0,0,AwbFileReadThread,(TDPVOID)pfile,0,0);
//			SetThreadPriority(awbfiledata->mReadThread,THREAD_PRIORITY_TIME_CRITICAL);
		}
	}

	if(BytesToRead<=0)
		return TDFALSE;
	
	if(BytesToRead>(pfile->mLength-pfile->mCurPos))
		BytesToRead = pfile->mLength-pfile->mCurPos;


	while(BytesToRead>0)
	{
		if(awbfiledata->mBlockLen-awbfiledata->mBlockOff>0)
		{
			if(awbfiledata->mBlockLen-awbfiledata->mBlockOff>=BytesToRead)
			{
				memcpy((char*)pBuf+*ReadBytes,awbfiledata->mBuf+awbfiledata->mBlockOff,BytesToRead);
				awbfiledata->mBlockOff+=BytesToRead;
				*ReadBytes+=BytesToRead;
				BytesToRead = 0;
				break;
			}
			else
			{
				memcpy((char*)pBuf+*ReadBytes,awbfiledata->mBuf+awbfiledata->mBlockOff,awbfiledata->mBlockLen-awbfiledata->mBlockOff);
				*ReadBytes+=awbfiledata->mBlockLen-awbfiledata->mBlockOff;
				BytesToRead-=awbfiledata->mBlockLen-awbfiledata->mBlockOff;
				awbfiledata->mBlockOff = awbfiledata->mBlockLen;
			}
		}
		if(awbfiledata->mBlockLen>=BKSIZE)
		{
			awbfiledata->mBlockNo++;
			awbfiledata->mBlockLen = 0;
			awbfiledata->mBlockOff = 0;
		}
		cmd = QU_FLDT;
		pgnum = PGNUM;

		memcpy(pSendBuf+8,&cmd,2);
		memcpy(pSendBuf+10,&awbfiledata->mBlockNo,4);
		memcpy(pSendBuf+14,&awbfiledata->mBlockLen,4);			//offset
		memcpy(pSendBuf+18,&pgnum,2);

		while(1)
		{
			TDINT readtimes = 0,offset = 0;
			TDBOOL success = TDFALSE;
			awbfiledata->mSeqNo++;
			memcpy(pSendBuf,&awbfiledata->mSeqNo,4);
			if(-1 == TDSocketWrite(awbfiledata->mSocket,pSendBuf,20))
			{
				return TDFALSE;
			}
#ifdef XP_PC
			ret = WaitForSingleObject(awbfiledata->mReadEvent,AWB_TIMEOUT);
#endif
			if(ret == WAIT_OBJECT_0)
				break;
			
			if(ret==WAIT_TIMEOUT)
			{
				overtime++;
				if(overtime>3){
					return TDFALSE;
				}
				continue;
			}
		}
	}
	pfile->mCurPos+=*ReadBytes;
	return TDTRUE;
}
#ifdef XP_UNIX
TDDWORD GetFileSize(TDHANDLE handle)
{
  struct stat statbuff;
  size_t nbytes;

	fstat(handle,&statbuff);
	nbytes = statbuff.st_size;
	
	return nbytes;	
}
#endif

TDHANDLE		
TDFileOpen(TDCHAR * filename)
{
	TDFile * pHandle;
	TDINT offset,i;
	TDPString dstFile;

	if(!filename)
		return TDNULL;
	//TDDebug(filename);	
	dstFile=TDString_Create();
	TDString_AssignPChar(dstFile,filename);
/*if this is system file*/
	offset=TDString_FindPChar(dstFile,"/",TDFALSE);
	if(offset==0)
	{
		TDHANDLE handle;

		handle=open(filename,O_RDONLY);//,NULL);
	 	if (handle != -1)
		{	
			pHandle=(TDFile*)Malloc(sizeof(TDFile));
			pHandle->mLength=GetFileSize(handle);
			pHandle->mCurPos=0;
			pHandle->mFileName=dstFile;
			pHandle->mFileType=WHX_SYSTEM_FILE;
			pHandle->mFileData = (TDHANDLE)Malloc(sizeof(TDSystemFileData));
			((TDPSystemFileData)pHandle->mFileData)->mFileHandle =handle;
			return (TDHANDLE)pHandle;
		}

	}

	whx:
	for(i=gURLFileStart;i<MAX_FILE_TYPE;i++)
	{
		TDPURLFile pFile;
		pFile=&gUrlFileArray[i-gURLFileStart];
		offset=TDString_FindPChar(dstFile,pFile->mName,TDTRUE);
		if(offset==0)
		{
			TDHANDLE handle;
			//TDDebug(pFile->mName);	

			handle = pFile->mOpenFile(dstFile);
			TDString_Destroy(dstFile);
			return handle;
		}
	}
	pHandle = (TDFile*)TDWhxFileOpen(dstFile);
	if(!pHandle)
	{
		TDString_InsertPChar(dstFile,"http://",0,-1);
		TDDebug("rewhx`````````````````````````````````````````````````````````");
		goto whx;
	}
	TDString_Destroy(dstFile);
	return (TDHANDLE)pHandle;

}
TDVOID	TDHttpFileClose( TDHANDLE hFile)
{
	TDFile * file=(TDFile *)hFile;
	TDHttpFileData *httpfiledata;
	if(!hFile)
		return;
	httpfiledata = (TDHttpFileData *)file->mFileData;
	TDFree(httpfiledata->mBuffer);
	TDSocketClose(httpfiledata->mSocket);
	TDFree(httpfiledata);
	return ;
}
TDVOID	TDVodFileClose( TDHANDLE hFile)
{
	TDFile * file=(TDFile *)hFile;
	TDVodFileData *vodfiledata;
	if(!hFile)
		return;
	vodfiledata = (TDVodFileData*)file->mFileData;
	TDSocketClose(vodfiledata->mTcpSocket);
	TDSocketClose(vodfiledata->mUdpSocket);
	TDFree (vodfiledata);
	return ;
}

TDVOID		
TDFileClose(TDHANDLE hFile)
{
	TDFile * file=(TDFile *)hFile;
	if(!hFile)
		return;
	switch(file->mFileType)
	{
#ifdef XP_PC
	case WHX_SYSTEM_FILE:
		if(file->mFileData)
			CloseHandle(((TDPSystemFileData)(file->mFileData))->mFileHandle);
		break;
#elif defined(XP_UNIX)
	case WHX_SYSTEM_FILE:
		if(file->mFileData)
			close(((TDPSystemFileData)(file->mFileData))->mFileHandle);
		break;
#endif
	default:
		{
			if( file->mFileType>=gURLFileStart &&
				file->mFileType<MAX_FILE_TYPE)
				/* http file*/
			{
				TDPURLFile pFile;
				pFile=&gUrlFileArray[file->mFileType-gURLFileStart];
				if(pFile->mCloseFile)
					pFile->mCloseFile(hFile);
			}


		}
		break;
	}

	if(file->mFileName)
	{
		TDString_Destroy(file->mFileName);
		file->mFileName=TDNULL;
	}
	TDFree(file);
	return;
}

TDVOID 
TDChangeOrder4(TDPBYTE src)
{
	TDBYTE tmp;
	tmp=src[0];
	src[0]=src[3];
	src[3]=tmp;
	tmp=src[1];
	src[1]=src[2];
	src[2]=tmp;
}

TDVOID 
TDChangeOrder2(TDPBYTE  src)
{
	TDBYTE tmp;
	tmp=src[0];
	src[0]=src[1];
	src[1]=tmp;
}

TDBOOL TDFileDestroy()
{
#ifdef XP_PC
	if(gFileHandle!=INVALID_HANDLE_VALUE)
		CloseHandle(gFileHandle);
	gFileHandle=INVALID_HANDLE_VALUE;
#endif
	return TDTRUE;
}

TDBOOL 
TDFileInit(TDPVOID start)
{
	TDINT i;
	TDCHAR Sign[11]="THUNER2000";
#ifdef XP_PC
	TDCHAR * file;
	file=(TDCHAR*)start;
	if(!file)
		return TDFALSE;
#ifdef UNICODE
	TDPWORD str;
	TD_PCharToPTChar(file,&str);
	gFileHandle=CreateFile(str,GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,
		OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	TDFree(str);
#else
	gFileHandle=CreateFile(file,GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,
		OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
#endif
	if(gFileHandle==INVALID_HANDLE_VALUE)
		return TDFALSE;
	gFileStart=0;
	gFileSize=GetFileSize(gFileHandle,TDNULL);
	gSectionNumber=gFileSize/SECTION_LENGTH;
#elif defined(XP_ESS)
	gFileStart=(TDINT)start;
	gSectionNumber=0xfffffff;
#elif defined(XP_CCUBE)
	gFileStart=(TDINT)start;
	gSectionNumber=0xfffffff;
#endif
	TDFileLoadSection(gFileBuffer,gFileStart,1);
	for(i=0;i<10;i++)
		if(Sign[i]!=gFileBuffer[i+SIGN_START])
		{
#ifdef XP_PC
			CloseHandle(gFileHandle);
			gFileHandle=INVALID_HANDLE_VALUE;
#endif
			return TDFALSE;
		}
#ifdef WHX_HIGHT_ORDER
		gRootNumber=(TDDWORD)((gFileBuffer[16])+(gFileBuffer[17]<<8)+(gFileBuffer[18]<<16)+(gFileBuffer[19]<<24));
#else
		gRootNumber=*((TDPDWORD)(gFileBuffer+16));
#endif
	return TDTRUE;
}

TDBOOL		
TDFileLoadSection(TDPVOID pBuffer,TDINT start,TDINT num)
{
	TDDWORD rd;
	if(!pBuffer || gSectionNumber<start)
		return TDFALSE;
#ifdef XP_PC
	if(gFileHandle==INVALID_HANDLE_VALUE)
		return TDFALSE;
	SetFilePointer(gFileHandle,start*SECTION_LENGTH,TDNULL,FILE_BEGIN);
	ReadFile(gFileHandle,pBuffer,SECTION_LENGTH*num,&rd,TDNULL);
	return TDTRUE;
#elif defined(XP_ESS)
	memcpy(pBuffer,whxBinData+start*SECTION_LENGTH,SECTION_LENGTH*num);
	return TDTRUE;
#elif defined(XP_CCUBE)
	memcpy(pBuffer,whxBinData+start*SECTION_LENGTH,SECTION_LENGTH*num);
/*	gFrontEnd->DumpData(gFrontEnd, start,num,pBuffer);
	*/
	return TDTRUE;
#endif
	return TDFALSE;
}


TDBOOL		
TDFileIsHTML(TDHANDLE handle)
{
	TDFile* file=(TDFile*)handle;
	if(file->mFileType==WHX_HTTP_FILE)
		return ((TDHttpFileData*)file->mFileData)->mHTMLContent;
	return TDFALSE;
}

TDBOOL		
TDFileExist(TDCHAR* filename)
{
	TDHANDLE handle;
	handle=TDFileOpen(filename);
	if(handle)
	{
		TDFileClose(handle);
		return TDTRUE;
	}
	return TDFALSE;
}

TDDWORD
TDHttpFileSeek(TDHANDLE hFile,TDINT offset,TDINT seekmode)
{
	TDFile* pfile  = (TDPFile)hFile;
	if(!hFile)
		return -1;
	switch(seekmode)
	{
	case FILE_BEGIN:
		pfile->mCurPos = offset;
		break;
	case FILE_END:
		pfile->mCurPos = pfile->mLength+offset;
		break;
	case FILE_CURRENT:
		pfile->mCurPos += offset;
		break;
	default:
		return -1;
	}
	if(pfile->mCurPos<0)
		pfile->mCurPos = 0;
	if(pfile->mCurPos>pfile->mLength)
		pfile->mCurPos = pfile->mLength;
	
	return pfile->mCurPos;
}

TDDWORD
TDVodFileSeek(TDHANDLE hFile,TDINT offset,TDINT seekmode)
{
	TDPBYTE tcpBuf;
	TDFile* pfile  = (TDPFile)hFile;
	TDPVodFileData vodfiledata;

	vodfiledata = (TDPVodFileData)pfile->mFileData;

	
	if(!hFile)
		return -1;
	switch(seekmode)
	{
	case FILE_BEGIN:
		pfile->mCurPos = offset;
		break;
	case FILE_END:
		pfile->mCurPos = pfile->mLength+offset;
		break;
	case FILE_CURRENT:
		pfile->mCurPos += offset;
		break;
	default:
		return -1;
	}
	if(pfile->mCurPos<0)
		pfile->mCurPos = 0;
	if(pfile->mCurPos>pfile->mLength)
		pfile->mCurPos = pfile->mLength;

	//seek file
	tcpBuf = (TDPBYTE)Malloc(1024);
	tcpBuf[0] = SEEK_CS;
	tcpBuf[1] = 0x04;
	tcpBuf[2] = 0;
	memcpy(tcpBuf+PACKHEAD_LENGTH,&(pfile->mCurPos),4);
	TDSocketWriteUntilDone(vodfiledata->mTcpSocket,tcpBuf,4+PACKHEAD_LENGTH);
	TDFree(tcpBuf);

	return pfile->mCurPos;
}


TDDWORD		
TDFileSeek(TDHANDLE hFile,TDINT offset,TDINT seekmode)
{
	TDFile * file=(TDFile *)hFile;
	if(!hFile)
		return -1;
	switch(file->mFileType)
	{
	case WHX_SYSTEM_FILE:
#ifdef XP_PC
		{
			TDDWORD method=FILE_BEGIN;
			TDSystemFileData *sfiledata;
			sfiledata = (TDSystemFileData*)file->mFileData;
			switch(seekmode)
			{
			case 0:
				method=FILE_BEGIN;
				break;
			case 1:
				method=FILE_END;
				break;
			case 2:
				method=FILE_CURRENT;
				break;
			}
			file->mCurPos = SetFilePointer(sfiledata->mFileHandle,offset,TDNULL,method);
		}
#endif
		break;
	case WHX_HTTP_FILE:
		return TDHttpFileSeek(hFile,offset,seekmode);	
		break;
	case WHX_VOD_FILE:
		return TDVodFileSeek(hFile,offset,seekmode);
		break;
	case WHX_FILE:
		break;
	}

	return file->mCurPos;
}

TDDWORD		
TDFileLength(TDHANDLE hFile)
{
	TDFile * file=(TDFile *)hFile;
	if(!hFile)
	{
	//printf("BBBBBB\n");
		return 0;
	}
	//printf("CCCCC %d\n",file->mLength);
	return file->mLength;
}

TDBOOL
TDHttpFileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes)
{
	TDPFile pfile = (TDPFile)hFile;
	TDPHttpFileData httpfiledata;
	if(!pfile)
		return TDFALSE;
	httpfiledata = (TDPHttpFileData)pfile->mFileData;
	*ReadBytes = 0;
	if(pfile->mCurPos>=pfile->mLength)
		return TDTRUE;

	if((BytesToRead+pfile->mCurPos)>pfile->mLength)
	{
		BytesToRead = pfile->mLength - pfile->mCurPos;
	}
	*ReadBytes = BytesToRead;

	if(pfile->mCurPos<httpfiledata->mCurLen)
	{/*partial content already in buffer*/
		if((BytesToRead+pfile->mCurPos)<=httpfiledata->mCurLen)
		{
			memcpy((TDPBYTE)pBuf,httpfiledata->mBuffer+pfile->mCurPos,BytesToRead);
			pfile->mCurPos += BytesToRead;
			return TDTRUE;
		}
		else
		{
			memcpy((TDPBYTE)pBuf,httpfiledata->mBuffer+pfile->mCurPos,httpfiledata->mCurLen-pfile->mCurPos);
			BytesToRead-=httpfiledata->mCurLen-pfile->mCurPos;
			pBuf=(TDPBYTE)pBuf+httpfiledata->mCurLen-pfile->mCurPos;
			pfile->mCurPos = httpfiledata->mCurLen;
		}
	}
	if(-1==TDSocketReadUntilDone(httpfiledata->mSocket,pBuf,
			BytesToRead))
			return TDFALSE;
	pfile->mCurPos += BytesToRead;

	return TDTRUE;
}

TDBOOL
TDVodFileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes)
{
	TDFile * pfile=(TDFile *)hFile;
	TDPVodFileData vodfiledata;
	TDINT readlen,packlen;
	TDCHAR tcpBuf[100];
	
	*ReadBytes = 0;
	if(!pfile)
		return TDFALSE;
	vodfiledata = (TDPVodFileData)pfile->mFileData;

	/*file end*/
	if(pfile->mCurPos>=pfile->mLength)
	{
		*ReadBytes = 0;
		return TDTRUE;
	}

	if(BytesToRead<=0)
		return TDFALSE;
	
	if(BytesToRead>(pfile->mLength-pfile->mCurPos))
		BytesToRead = pfile->mLength-pfile->mCurPos;

	//seek mode start
	while(1){
		if(BytesToRead>TCP_PACK_LENGTH){
			packlen = TCP_PACK_LENGTH;
			BytesToRead-=TCP_PACK_LENGTH;
		}
		else{
			packlen = BytesToRead;
			BytesToRead = 0;
		}

		tcpBuf[0] = REQUEST_CS;
		memcpy(tcpBuf+PACKHEAD_LENGTH,(TDCHAR*)&packlen,4);
		TDSocketWriteUntilDone(vodfiledata->mTcpSocket,tcpBuf,PACKHEAD_LENGTH+4);

		readlen=TDSocketReadUntilDone(vodfiledata->mTcpSocket,tcpBuf,PACKHEAD_LENGTH);
		if(readlen==-1)
				return TDFALSE;
		switch(tcpBuf[0])
		{
			case DATA_SC:
				memcpy((TDCHAR*)&packlen,tcpBuf+1,4);
				break;
			default:
				return TDFALSE;
		}

		if(packlen==0)
			return TDTRUE;
		readlen=TDSocketReadUntilDone(vodfiledata->mTcpSocket,(((char*)pBuf)+*ReadBytes),packlen);
		if(readlen==-1)
				return TDFALSE;
		pfile->mCurPos += packlen;
		*ReadBytes += packlen;
		if(BytesToRead==0)
			return TDTRUE;
	}
/**/	
	//seek mode end

/*	if(!vodfiledata->mFileRequest)
	{	
		tcpBuf[0] = REQUEST_CS;
		TDSocketWriteUntilDone(vodfiledata->mTcpSocket,tcpBuf,PACKHEAD_LENGTH);
		vodfiledata->mFileRequest = TDTRUE;
	}
#ifdef HAVE_IC1
	{
		tcpBuf[0] = GET_IT;
		*((TDPDWORD)(tcpBuf+1))=BytesToRead;
		TDSocketWriteUntilDone(vodfiledata->mTcpSocket,tcpBuf,PACKHEAD_LENGTH);
	}
#endif	
	while(1)
	{
		if(vodfiledata->mBufferRemain>0)
		{
			if(BytesToRead<=vodfiledata->mBufferRemain)
			{
				readlen=TDSocketReadUntilDone(vodfiledata->mTcpSocket,(((char*)pBuf)+*ReadBytes),BytesToRead);
				if(readlen==-1)
					return TDFALSE;
				vodfiledata->mBufferRemain-=BytesToRead;
				pfile->mCurPos += BytesToRead;
				*ReadBytes += BytesToRead;
				return TDTRUE;
			}
			else
			{
				readlen=TDSocketReadUntilDone(vodfiledata->mTcpSocket,(((char*)pBuf)+*ReadBytes),vodfiledata->mBufferRemain);
				if(readlen==-1)
					return TDFALSE;
				pfile->mCurPos += vodfiledata->mBufferRemain;
				*ReadBytes += vodfiledata->mBufferRemain;
				BytesToRead-=vodfiledata->mBufferRemain;
				vodfiledata->mBufferRemain = 0;
			}
		}
		else
		{
			readlen=TDSocketReadUntilDone(vodfiledata->mTcpSocket,tcpBuf,PACKHEAD_LENGTH);
			if(readlen==-1)
				return TDFALSE;
			switch(tcpBuf[0])
			{
			case PAYMENT_SC:
				//扣钱
				readlen=TDSocketReadUntilDone(vodfiledata->mTcpSocket,tcpBuf,4);
				if(readlen==-1)
					return TDFALSE;
#ifdef HAVE_IC1
				{
					TDDWORD cash;
					WORD wRet;
					
					memcpy((TDCHAR*)&cash,tcpBuf,4);
					
					if(MCS_SUCCESS == MCS_TestDoor())
					{
						if(MCS_SUCCESS == AT24C_OpenCard(AT24C04))
						{
							wRet = AT24C_WriteValue(80,cash);
							AT24C_CloseCard();
							if(wRet==MCS_SUCCESS)
								break;
						}
					}
					return TDFALSE;
					
				}
				
#endif
				
				break;
			case DATA_SC:
				memcpy((TDCHAR*)&packlen,tcpBuf+1,4);
				vodfiledata->mBufferRemain = packlen;
				break;
			case FILEEND_SC:
				pfile->mCurPos = pfile->mLength;
				return TDTRUE;
				break;
			default:
				break;
			}
		}
	}
/**/	
}

TDBOOL
TDVod2FileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes)
{
	TDPFile pThis;
	TDPVod2FileData vod2filedata;
	TDBYTE pSendBuf[VOD2_PACK];
	TDSHORT cmdtype;
	TDDWORD cmdlen;
	TDINT overtime = 0,ret;

	*ReadBytes=0;
	if(!hFile)

		return TDFALSE;
	pThis = (TDPFile)hFile;
	vod2filedata = (TDPVod2FileData)pThis->mFileData;
	
	if(BytesToRead<=0)
		return TDFALSE;

	if(vod2filedata->mReadEnd)
		return TDTRUE;
	cmdtype =CMD_VIDEODATA_CS ;
	cmdlen=8;
	
	memcpy(pSendBuf,&cmdtype,2);
	memcpy(pSendBuf+6,&vod2filedata->mVideoID,4);
	memcpy(pSendBuf+10,&vod2filedata->mTaskID,4);
	memcpy(pSendBuf+14,&cmdlen,4);

	while(BytesToRead>0)
	{
		TDDWORD Bytes;

		if(BytesToRead>VOD2_MAXLEN)
			Bytes = VOD2_MAXLEN;
		else
			Bytes = BytesToRead;
		BytesToRead -= Bytes;
		vod2filedata->mSeq++;
		while(1)
		{
 			vod2filedata->mTimeID++;
			vod2filedata->mReadLen=0;
			memcpy(pSendBuf+2,&vod2filedata->mTimeID,4);
			memcpy(pSendBuf+VOD2_HEAD,&Bytes,4);
			memcpy(pSendBuf+VOD2_HEAD+4,&vod2filedata->mSeq,4);
			if(-1==TDSocketWrite(vod2filedata->mSocket,pSendBuf,VOD2_PACK))
			{
				return TDFALSE;
			}
			
/*			ret = ::WaitForSingleObject(vod2filedata->mReadEvent,VOD2_TIMEOUT*(overtime+1));
			
			if(ret == WAIT_OBJECT_0)
				break;
			if(ret==WAIT_TIMEOUT)
			{
				overtime++;
				if(overtime>VOD2_OVERTIMES){
					return TDFALSE;
				}
				continue;
			}
*/		}
		if(vod2filedata->mReadEnd)
			break;
		memcpy((TDPCHAR)pBuf+*ReadBytes,vod2filedata->mBuf,vod2filedata->mReadLen);
		*ReadBytes += vod2filedata->mReadLen;
	}
	return TDTRUE;
}

TDVOID TDFileSetCommand(TDINT cmdtype,TDINT cmdcontent)
{
	if(gVideoCmdExist)
		return ;
	gVideoCmd.mCmdType = cmdtype;
	gVideoCmd.mCmdContent = cmdcontent;
	gVideoCmdExist = TDTRUE;
	return ;
}

TDBOOL TDFileSendCommand(TDHANDLE hFile)
{
	TDPFile pThis;
	TDPVod2FileData vod2filedata;
	TDSHORT cmdtype;
	TDINT content,cmdlen,ret,overtime=0;
	TDBYTE	pSendBuf[VOD2_PACK];

	if(!hFile)
		return TDFALSE;
	if(!gVideoCmdExist)
		return TDFALSE;
	pThis = (TDPFile)hFile;
	if(pThis->mFileType!=WHX_VOD2_FILE)
		return TDFALSE;
	vod2filedata = (TDPVod2FileData)pThis->mFileData;
	if(gVideoCmd.mCmdType == VIDEOCMD_PLAY && gVideoCmd.mCmdContent==vod2filedata->mPlayMode)
	{
		gVideoCmdExist = TDFALSE;
		return TDTRUE;
	}
	switch(gVideoCmd.mCmdType)
	{
	case VIDEOCMD_PLAY:
		cmdtype = CMD_VIDEOPLAY_CS;
		break;
	case VIDEOCMD_JUMP:
		cmdtype = CMD_VIDEOJUMP_CS;
		break;
	case VIDEOCMD_TIME:
		cmdtype = CMD_VIDEOTIME_CS;
		break;
	default:
		return TDFALSE;
	}
	content = gVideoCmd.mCmdContent;
	cmdlen = 4;
	
	memcpy(pSendBuf,&cmdtype,2);
	memcpy(pSendBuf+6,&vod2filedata->mVideoID,4);
	memcpy(pSendBuf+10,&vod2filedata->mTaskID,4);
	memcpy(pSendBuf+14,&cmdlen,4);
	memcpy(pSendBuf+VOD2_HEAD,&content,cmdlen);

	while(1)
	{
		vod2filedata->mTimeID++;
		memcpy(pSendBuf+2,&vod2filedata->mTimeID,4);
		if(-1==TDSocketWrite(vod2filedata->mSocket,pSendBuf,VOD2_PACK))
		{
			return TDFALSE;
		}
/*		ret = ::WaitForSingleObject(vod2filedata->mReadEvent,VOD2_TIMEOUT*(overtime+1));
		
		if(ret == WAIT_OBJECT_0)
			break;
		if(ret==WAIT_TIMEOUT)
		{
			overtime++;
			if(overtime>VOD2_OVERTIMES){
				return TDFALSE;
			}
			continue;
		}
*/
/*		ret=TDSocketRead(vod2filedata->mSocket,pRecvBuf,VOD2_PACK);
		if(ret>=VOD2_HEAD)
		{
			TDINT timeid,videoid,taskid;
			memcpy(&recvcmdtype,pRecvBuf,2);
			memcpy(&timeid,pRecvBuf+2,4);
			memcpy(&videoid,pRecvBuf+6,4);
			memcpy(&taskid,pRecvBuf+10,4);
			memcpy(&recvcmdlen,pRecvBuf+14,4);

			
			if(recvcmdlen==(ret-VOD2_HEAD))
				memcpy(pRecvBuf,pRecvBuf+VOD2_HEAD,recvcmdlen);
			if(recvcmdtype==(cmdtype+0x200) && vod2filedata->mTimeID==timeid && vod2filedata->mVideoID == videoid && vod2filedata->mTaskID==taskid )
				break;
			
		}
		overtime++;
		if(overtime>3)
		{
			return TDFALSE;
		}
*/	}
#ifndef NOVIDEO
#ifdef _WIN32_WCE
	FMPFlush(TRUE);
#endif
#endif
	if(gVideoCmd.mCmdType == VIDEOCMD_PLAY)
		vod2filedata->mPlayMode = gVideoCmd.mCmdContent; 
	gVideoCmdExist = TDFALSE;
//	MSG("SUCCESS TO SEND COMMAND");
	return TDTRUE;
}



TDBOOL		
TDFileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes)
{
	TDFile * file=(TDFile *)hFile;
	TDDWORD readlen=0;
	if(!hFile)
		return TDFALSE;
	switch(file->mFileType)
	{
	case WHX_SYSTEM_FILE:
#ifdef XP_PC
		{
			TDSystemFileData *sfiledata = (TDSystemFileData*)file->mFileData;
			if(sfiledata->mFileHandle!=INVALID_HANDLE_VALUE)
			{
				return ReadFile(sfiledata->mFileHandle,pBuf,BytesToRead,ReadBytes,TDNULL);
			}
			return TDFALSE;
		}
#endif

#ifdef XP_UNIX
   {
			TDSystemFileData *sfiledata = (TDSystemFileData*)file->mFileData;
			if(sfiledata->mFileHandle!=-1)
			{
				*ReadBytes = read(sfiledata->mFileHandle,pBuf,BytesToRead);
				return TDTRUE;
			}
			return TDFALSE;
	 }
	
#endif
		break;

	default:
		{
			if( file->mFileType>=gURLFileStart &&
				file->mFileType<MAX_FILE_TYPE)
				/* http file*/
			{
				TDPURLFile pFile;
				pFile=&gUrlFileArray[file->mFileType-gURLFileStart];
				if(pFile->mReadFile)
					return pFile->mReadFile(hFile,pBuf,BytesToRead,ReadBytes);
			}
		}

	}
	return TDTRUE;
}


TDBOOL		
TDFileReadAny(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes)
{
	return TDTRUE;
}

TDBOOL		
TDFileWrite(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes)
{
	return TDTRUE;
}
TDBOOL	
TDFileAsynRead(TDCHAR * FileName,TDPVOID pClient)
{
	return TDTRUE;
}




TDINT TDFileGetPlayMode(TDHANDLE hfile)
{
	TDFile *pThis;
	TDVod2FileData 	*vod2filedata;

	if(!hfile)
		return 1;
	pThis = (TDFile*)hfile;
	if(pThis->mFileType!=WHX_VOD2_FILE)
		return 1;
	vod2filedata = (TDVod2FileData*)pThis->mFileData;
	return vod2filedata->mPlayMode;
}

TDINT TDFileGetVideoType(TDHANDLE hFile)
{
	TDFile *pThis;
	TDINT videotype;
	TDPVod2FileData vod2filedata;
	if(!hFile)
		return INVALID_VIDEO_TYPE;
	pThis = (TDPFile)hFile;
	videotype = INVALID_VIDEO_TYPE;
	switch(pThis->mFileType)
	{
	case WHX_VOD_FILE:
	case WHX_HTTP_FILE:
	case WHX_SYSTEM_FILE:
	case WHX_AWB_FILE:
		if(-1!=TDString_FindPChar(pThis->mFileName,".mpg",TDTRUE))
			videotype = MPEG1_VIDEO_TYPE;
		else if(-1!=TDString_FindPChar(pThis->mFileName,".vob",TDTRUE))
			videotype = VOB_VIDEO_TYPE;
		else if(-1!=TDString_FindPChar(pThis->mFileName,".mp3",TDTRUE))
			videotype = MPEGAUDIO_VIDEO_TYPE;
		else if(-1!=TDString_FindPChar(pThis->mFileName,".mp2",TDTRUE))
			videotype = MPG2_VIDEO_TYPE;
		break;
	case WHX_VOD2_FILE:
		vod2filedata = (TDPVod2FileData)pThis->mFileData;
		videotype = vod2filedata->mVideoType;
		break;
	}
	return videotype;
}

TDBOOL TDFileIsAbsPath(TDPString pPath)
{
	TDINT l=0,i;
	TDINT offset;
	TDBOOL sign=TDTRUE;;
	if(!pPath)
		return TDFALSE;
	for(i=gURLFileStart;i<MAX_FILE_TYPE;i++)
	{
		TDPURLFile pFile;
		pFile=&gUrlFileArray[i-gURLFileStart];
		offset=TDString_FindPChar(pPath,pFile->mName,TDTRUE);
		if(offset==0)
		{
			return TDTRUE;
		}
	}

#ifdef _WIN32_WCE
	if(TDString_CharAt(pPath,0)=='\\')
		return TDTRUE;
#elif defined(WIN32)
	if(TDString_CharAt(pPath,0)=='\\')
		return TDTRUE;
	if(pPath->mLength>3 && TDString_IsAlpha(TDString_CharAt(pPath,0)) && TDString_CharAt(pPath,1)==':' && TDString_CharAt(pPath,2)=='\\')
		return TDTRUE;
	if(pPath->mLength>3 && TDString_CharAt(pPath,0)=='\\' && TDString_CharAt(pPath,1)=='\\')
		return TDTRUE;
#endif
	return TDFALSE;
}


TDBOOL TDFileMakeSureAbsPath(TDPString pDirect,TDPString pPath)
{
	TDINT i,l=0;
	TDINT offset,poffset;
	TDBOOL sign=TDTRUE;;
	if(!pDirect || !pPath)
		return TDFALSE;
	if(TDFileIsAbsPath(pPath))
		return TDTRUE;
	for(i=gURLFileStart;i<MAX_FILE_TYPE;i++)
	{
		TDPURLFile pFile;
		pFile=&gUrlFileArray[i-gURLFileStart];
		offset=TDString_FindPChar(pDirect,pFile->mName,TDTRUE);
		if(offset==0)
		{
			poffset=TDStrLen(pFile->mName);
			sign=TDFALSE;
			break;
		}
	}

	if(sign)
	{
		if(TDString_CharAt(pPath,0)!='/')
		{
			TDString_Insert(pPath,pDirect,0,-1);
		}
		else
		{
			offset=TDString_FindChar(pDirect,'\\',2,TDFALSE);
			if(offset>=2)
				TDString_Insert(pPath,pDirect,0,offset);
			else
				TDString_Insert(pPath,pDirect,0,-1);
		}
		TDString_ReplaceChar(pPath,'/','\\');
		/*cut all \XX\..\YY\  to \YY\ */
		{
			TDINT start=0,end;
			end=TDString_FindPChar(pPath,"\\..\\",TDTRUE);
			while(end>0&&end<(pPath->mLength-2))
			{
				start=end-1;
				while(TDString_CharAt(pPath,start)!='\\' && (--start)>=0);
				TDString_Cut(pPath,start,end-start+3);
				end=TDString_FindPCharFrom(pPath,"\\..\\",TDTRUE,start);
			}
		}
	}
	else
	{
		if(TDString_CharAt(pPath,0)!='/')
		{
			TDString_Insert(pPath,pDirect,0,-1);
		}
		else
		{
			offset=TDString_FindChar(pDirect,'/',7,TDFALSE);
			if(offset>=7)
				TDString_Insert(pPath,pDirect,0,offset);
			else
				TDString_Insert(pPath,pDirect,0,-1);
		}
		/*cut all /XX/../YY/  to /YY/ */
		{
			TDINT start=0,end;
			end=TDString_FindPChar(pPath,"/../",TDTRUE);
			while(end>0&&end<(pPath->mLength-2))
			{
				start=end-1;
				while(TDString_CharAt(pPath,start)!='/' && (--start)>=0);
				TDString_Cut(pPath,start,end-start+3);
				end=TDString_FindPCharFrom(pPath,"/../",TDTRUE,start);
			}
			poffset=TDString_FindPCharFrom(pPath,"//",TDTRUE,poffset);
			while(poffset>0&&poffset<(pPath->mLength-2))
			{
				TDString_Cut(pPath,poffset,1);
				poffset=TDString_FindPCharFrom(pPath,"//",TDTRUE,poffset);
			}
		}
	}

	i=TDString_FindChar(pPath,'#',0,TDFALSE);
	if(i>0)
	{
		TDString_Cut(pPath,i,pPath->mLength);
	}
	return TDTRUE;
}
TDBOOL
TDFileMakePathDir(TDPString pPath)
{
	TDINT offset,i;
	TDBOOL sign=TDFALSE;
	if(!pPath)
		return TDFALSE;
	for(i=gURLFileStart;i<MAX_FILE_TYPE;i++)
	{
		TDPURLFile pFile;
		pFile=&gUrlFileArray[i-gURLFileStart];
		offset=TDString_FindPChar(pPath,pFile->mName,TDTRUE);
		if(offset==0)
		{
			sign=TDTRUE;
			break;
		}
	}
	if(sign)
	{
/*cut all /XX/../YY/  to /YY/ */
		{
			TDINT start=0,end;
			end=TDString_FindPChar(pPath,"/../",TDTRUE);
			while(end>0&&end<(pPath->mLength-2))
			{
				start=end-1;
				while(TDString_CharAt(pPath,start)!='/' && (--start)>=0);
				TDString_Cut(pPath,start,end-start+3);
				end=TDString_FindPCharFrom(pPath,"/../",TDTRUE,start);
			}
		}
		offset=TDString_RFindChar(pPath,'/',0,TDTRUE);
		if(offset<7)
		{
			TDString_AppendChar(pPath,'/');
			offset=-1;
		}
	}
	else
		offset=TDString_RFindChar(pPath,'\\',0,TDTRUE);
	if(offset==-1)
		return TDTRUE;
	TDString_Cut(pPath,offset+1,pPath->mLength-offset+1);
	return TDTRUE;
}


TDPString
TDFileGetName(TDHANDLE hFile)
{
	TDPFile pThis;
	if(!hFile)
		return TDNULL;
	pThis = (TDPFile)hFile;
	return pThis->mFileName;
}

#if defined(XP_PC) && !defined(_WIN32_WCE)

WHXFile*
WHXFileCreate(TDPString filename)
{
	WHXFile *pFile=TDNULL;
	pFile=(WHXFile*)Malloc(sizeof(WHXFile));
	if(!pFile)
		return TDNULL;
	TD_ArrayInit(&pFile->mRoot.mChild,0);
	pFile->mRoot.flag=0;
	TDString_Init(&pFile->mRoot.mName);

	TDString_Init(&pFile->mName);
	TDString_AssignString(&pFile->mName,filename,-1);
	return pFile;
}

TDBOOL
WHXFileAdd(WHXFile* pThis,TDPString srcFile,TDPString dstFile)
{
	TDINT start=0,end=0;
	WHXDirRec*	curDir;
	if(!pThis || !dstFile ||!srcFile || srcFile->mLength<1)
		return TDFALSE;
	curDir=&pThis->mRoot;
	if(TDString_CharAt(dstFile,0)=='\\')
		start++,end++;
	end=TDString_FindPCharFrom(dstFile,"\\",TDFALSE,start);
	while(end!=-1)
	{
		TDINT i;
		WHXDirRec*	tmpDir;
		TDBOOL finddir=TDFALSE;
		TDString dir;
		TDString_Init(&dir);
		TDString_Mid(dstFile,&dir,start,end-start);
		for(i=0;i<curDir->mChild.mCount;i++)
		{
			tmpDir=(WHXDirRec*)TD_ArrayElementAt(&curDir->mChild,i);
			if(tmpDir->flag==0)
			{
				if(TDString_EqString(&tmpDir->mName,&dir,TDTRUE))
				{
					finddir=TDTRUE;
					curDir=tmpDir;
					break;
				}
			}
		}
		if(!finddir)
		{
			tmpDir=(WHXDirRec*)Malloc(sizeof(WHXDirRec));
			tmpDir->flag=0;
			tmpDir->mRecord=TDNULL;
			TD_ArrayInit(&tmpDir->mChild,0);
			TDString_Init(&tmpDir->mName);
			TDString_AssignString(&tmpDir->mName,&dir,-1);
			TD_ArrayAppendElement(&curDir->mChild,tmpDir);
			tmpDir->mParent=curDir;
			curDir=tmpDir;
		}
		TDString_DTDString(&dir);
		start=end+1;
		end=TDString_FindPCharFrom(dstFile,"\\",TDFALSE,start);
	}
	if(start<dstFile->mLength)
	{
		WHXFileRec*	tmp;
		tmp=(WHXFileRec*)Malloc(sizeof(WHXFileRec));
		tmp->flag=1;
		tmp->mRecord=TDNULL;
		TDString_Init(&tmp->mName);
		TDString_Init(&tmp->mSrc);
		TDString_Mid(dstFile,&tmp->mName,start,dstFile->mLength-start);
		TDString_AssignString(&tmp->mSrc,srcFile,-1);
		TD_ArrayAppendElement(&curDir->mChild,tmp);
		tmp->mParent=curDir;
	}
	return TDTRUE;
}
TDBOOL 
WHXFileAddDirect(WHXFile* pThis,TDPString path,TDPString dir)
{
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 
	TDBOOL fFinished = FALSE; 
	if(!pThis || !path || !dir)
		return TDFALSE;
	SetCurrentDirectory(path->mStr);
	hSearch = FindFirstFile("*.*", &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) 
		return TDFALSE; 
	while (!fFinished) 
	{     
		TDString src,dst;
		TDString_Init(&src);
		TDString_Init(&dst);
		if(FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if(strcmp(FileData.cFileName,".")!=0 &&
				strcmp(FileData.cFileName,"..")!=0)
			{
				TDString_Assign(&src,path,-1);
				TDString_AppendChar(&src,'\\');
				TDString_AppendPChar(&src,FileData.cFileName);
				TDString_Assign(&dst,dir,-1);
				TDString_AppendChar(&dst,'\\');
				TDString_AppendPChar(&dst,FileData.cFileName);
				WHXFileAddDirect(pThis,&src,&dst);
				TDString_DTDString(&src);
				TDString_DTDString(&dst);
			}
		}
		else
		{
				TDString_Assign(&src,path,-1);
				TDString_AppendChar(&src,'\\');
				TDString_AppendPChar(&src,FileData.cFileName);
				if(dir)
					TDString_Assign(&dst,dir,-1);
				TDString_AppendChar(&dst,'\\');
				TDString_AppendPChar(&dst,FileData.cFileName);
				WHXFileAdd(pThis,&src,&dst);
				TDString_DTDString(&src);
				TDString_DTDString(&dst);
		}
		if (!FindNextFile(hSearch, &FileData))     
			fFinished = TRUE; 
	}  
	return TDTRUE;
}


TDPBYTE WHXWriteFile(WHXDirRec * dir,TDPBYTE pStart,TDPBYTE pOrg);
TDPBYTE WHXWriteDir(WHXDirRec * dir,TDPBYTE pStart,TDPBYTE pOrg);
TDBOOL  WHXCalcFile(WHXDirRec * dir,TDPINT pSize);
TDBOOL  WHXCalcDir(WHXDirRec * dir,TDPINT pSize);

TDBOOL
WHXFileWrite(WHXFile* pThis)
{
	HANDLE File,FileMap;
	TDPVOID pFile;
	TDPBYTE pStart,pEnd;
	TDINT size=SECTION_LENGTH;
	if(!pThis)
		return TDFALSE;
	File=CreateFile(pThis->mName.mStr,GENERIC_READ |GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(File==INVALID_HANDLE_VALUE)
		return TDFALSE;
	if(!WHXCalcFile(&pThis->mRoot,&size))
	{
		CloseHandle(File);
		return TDFALSE;
	}
	if(!WHXCalcDir(&pThis->mRoot,&size))
	{
		CloseHandle(File);
		return TDFALSE;
	}

	FileMap=CreateFileMapping(File,NULL,PAGE_READWRITE,0,size,NULL);
	if (FileMap == NULL) 
	{
		TDDWORD err=GetLastError();
		CloseHandle(File);
		return TDFALSE;
	}
	pFile=MapViewOfFile(FileMap,FILE_MAP_ALL_ACCESS,0,0,0);
	if(pFile==NULL)
	{
		CloseHandle(File);
		CloseHandle(FileMap);
		return TDFALSE;
	}
	pStart=(TDPBYTE)pFile;
	{
		TDCHAR Sign[11]="THUNER2000";
		memcpy(pStart,Sign,11);
		*((TDPINT)(pStart+16))=pThis->mRoot.mChild.mCount;
	}
	
	pEnd=WHXWriteDir(&pThis->mRoot,pStart+SECTION_LENGTH,pStart);
	pEnd=WHXWriteFile(&pThis->mRoot,pEnd,pStart);

	UnmapViewOfFile(pFile);
	CloseHandle(File);
	CloseHandle(FileMap);
	return TDTRUE;;
}

TDPBYTE
WHXWriteDir(WHXDirRec * dir,TDPBYTE pStart,TDPBYTE pOrg)
{
	TDINT i;
	TDPBYTE pBuffer=pStart;;
	if(!dir || !pStart)
		return pStart;
	for(i=0;i<dir->mChild.mCount;i++)
	{
		WHXDirRec* rec;
		TDPFileRecord pRecord;
		rec=(WHXDirRec *)TD_ArrayElementAt(&dir->mChild,i);
		pRecord=(TDPFileRecord)(pBuffer+RECORD_SIZE*i);
		if(rec->flag&0x1)
		{/*this is a file*/
			WHXFileRec * file;
			file=(WHXFileRec*)rec;
			pRecord->flag=file->flag;
			TDstrcpy(pRecord->name,file->mName.mStr);
			file->mRecord=pRecord;
		}
		else
		{/*this is a direct*/
			WHXDirRec * file;
			file=rec;
			pRecord->flag=file->flag;
			TDstrcpy(pRecord->name,file->mName.mStr);
			pRecord->length=file->mChild.mCount;
			file->mRecord=pRecord;
		}
	}
	pStart+=(((RECORD_SIZE*dir->mChild.mCount)+SECTION_LENGTH-1)>>SECTION_SHIFT)<<SECTION_SHIFT;
	pBuffer=pStart;
	for(i=0;i<dir->mChild.mCount;i++)
	{
		WHXDirRec* rec;
		rec=(WHXDirRec *)TD_ArrayElementAt(&dir->mChild,i);
		if((rec->flag&0x1)==0)

		{
			rec->mRecord->start=(pStart-pOrg)>>SECTION_SHIFT;
			pBuffer=WHXWriteDir(rec,pStart,pOrg);
			pStart=pBuffer;
		}
	}
	return pBuffer;
}

TDPBYTE
WHXWriteFile(WHXDirRec * dir,TDPBYTE pStart,TDPBYTE pOrg)
{
	TDINT i;
	TDPBYTE pBuffer=pStart;;
	if(!dir || !pStart)
		return pStart;
	for(i=0;i<dir->mChild.mCount;i++)
	{
		WHXFileRec* file;
		TDPFileRecord pRecord;
		file=(WHXFileRec *)TD_ArrayElementAt(&dir->mChild,i);
		if(file->flag&0x1)
		{/*this is a file*/
			HANDLE handle;
			pRecord=file->mRecord;
			handle=CreateFile(file->mSrc.mStr,GENERIC_READ,0,NULL,
				OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL);
			if(handle==INVALID_HANDLE_VALUE)
			{
				pRecord->length=0;
				pRecord->start=0;
			}
			else
			{
				TDDWORD rd;
				pRecord->length=GetFileSize(handle,NULL);
				pRecord->start=(pBuffer-pOrg)>>SECTION_SHIFT;
				ReadFile(handle,pBuffer,pRecord->length,&rd,NULL);
				pBuffer+=((pRecord->length+SECTION_LENGTH-1)>>SECTION_SHIFT)<<SECTION_SHIFT;
				CloseHandle(handle);
			}

		}
	}
	pStart=pBuffer;
	for(i=0;i<dir->mChild.mCount;i++)
	{
		WHXDirRec* rec;
		rec=(WHXDirRec *)TD_ArrayElementAt(&dir->mChild,i);
		if((rec->flag&0x1)==0)
		{
			pBuffer=WHXWriteFile(rec,pStart,pOrg);
			pStart=pBuffer;
		}
	}
	return pBuffer;
}

TDBOOL
WHXCalcDir(WHXDirRec * dir,TDPINT pSize)
{
	TDINT i;
	if(!dir || !pSize)
		return TDFALSE;
	*pSize+=(((RECORD_SIZE*dir->mChild.mCount)+SECTION_LENGTH-1)>>SECTION_SHIFT)<<SECTION_SHIFT;
	for(i=0;i<dir->mChild.mCount;i++)
	{
		WHXDirRec* rec;
		rec=(WHXDirRec *)TD_ArrayElementAt(&dir->mChild,i);
		if((rec->flag&0x1)==0)
			WHXCalcDir(rec,pSize);
	}
	return TDTRUE;
}

TDBOOL
WHXCalcFile(WHXDirRec * dir,TDPINT pSize)
{
	TDINT i;
	if(!dir || !pSize)
		return TDTRUE;
	for(i=0;i<dir->mChild.mCount;i++)
	{
		WHXFileRec* file;
		file=(WHXFileRec *)TD_ArrayElementAt(&dir->mChild,i);
		if(file->flag&0x1)
		{/*this is a file*/
			HANDLE handle;
			handle=CreateFile(file->mSrc.mStr,GENERIC_READ,0,NULL,

				OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL);
			if(handle!=INVALID_HANDLE_VALUE)
			{
				TDDWORD len;
				len=GetFileSize(handle,NULL);
				*pSize+=((len+SECTION_LENGTH-1)>>SECTION_SHIFT)<<SECTION_SHIFT;
				CloseHandle(handle);
			}

		}
	}
	for(i=0;i<dir->mChild.mCount;i++)
	{
		WHXDirRec* rec;
		rec=(WHXDirRec *)TD_ArrayElementAt(&dir->mChild,i);
		if((rec->flag&0x1)==0)
			WHXCalcFile(rec,pSize);
	}
	return TDTRUE;
}

TDBOOL WHXMakeFile(TDCHAR * s,TDCHAR* bin)
{
	WHXFile* file;
	TDString path,src;
	if(!s || !bin)
		return TDFALSE;
	TDString_Init(&path);
	TDString_Init(&src);
	TDString_AssignPChar(&path,bin);
	file=WHXFileCreate(&path);
	TDString_AssignPChar(&path,s);
	TDString_AssignPChar(&src,"");
	WHXFileAddDirect(file,&path,&src);
	WHXFileWrite(file);

	return TDTRUE;
}
#endif /*WHX_FILE_CREATE*/

#ifdef XP_UNIX

# if 0
#include "semaphore.h"


TDM_TDEVICEAPI	TDBOOL    SetEvent(Semaphore s)
{
	tw_pthread_cond_signal(&(s.cond),&(s.mutex),&(s.wait_flag));
	return 1;
}

TDM_TDEVICEAPI	TDINT     CloseHandle(TDVOID *hand)
{
	semaphore_destroy(hand);	
	return 0;
}
TDM_TDEVICEAPI	TDINT    WaitForSingleObject(Semaphore *s,TDINT num)
{

	if(num == 0xffffffff)
	{
 		return tw_pthread_cond_wait(&(s->cond),&(s->mutex),&(s->wait_flag));	
	}
	else
	{
			
		return tw_pthread_cond_timedwait(&(s->cond),&(s->mutex),num,&(s->wait_flag));
	}		
}

#endif





/*
static int is_first = 1;
static int proj_id[255];

int SelectProjID()
{
	int a;
	if(is_first)
	{
		bzero(proj_id, sizeof(proj_id));
		is_first = 0;
	}
	for(a=1;a<255;a++)
	{
		if(!proj_id[a])
		{
			proj_id[a] = 1;
			break;
		}
	}
	return a;
}
*/


#if 1

//-------------event util , only support auto event ,do not support manul event-----------------------------
TDM_TDEVICEAPI void* CreateEvent()
{
	EVENT *pEvent =  (PEVENT)malloc(sizeof(EVENT));
	pEvent->ref = 0;

	if (pthread_mutex_init (&(pEvent->mutex),NULL) == -1)
	{
		TDDebug("CreateEvent before1");
     		return NULL;
     	}
	
	if (pthread_mutex_init (&(pEvent->ref_mutex),NULL) == -1)
	{
		TDDebug("CreateEvent before2");
     		return NULL;
     	}

        if (pthread_cond_init (&(pEvent->cond), NULL) == -1)
        {
		TDDebug("CreateEvent before3");
		return NULL;    
	}
	

	return pEvent;


}

TDM_TDEVICEAPI int ResetEvent(void* event)
{
	EVENT *pEvent = (PEVENT)event;
	int ret = 0;

	pthread_mutex_lock (&(pEvent->ref_mutex));
	pEvent->ref = 0;
        pthread_mutex_unlock (&(pEvent->ref_mutex));
	
	pthread_cond_wait(&(pEvent->cond), &(pEvent->mutex));
	return 0;
}

TDM_TDEVICEAPI int SetEvent(void* event)
{
	EVENT *pEvent = (PEVENT)event;
	int ret = 0;

	//TDDebug("SetEvent before");
	pthread_mutex_lock (&(pEvent->ref_mutex));
	if(!pEvent->ref)
	{
		pEvent->ref = 1;
	}
        pthread_mutex_unlock (&(pEvent->ref_mutex));
	
	pthread_cond_signal(&(pEvent->cond));	
	//TDDebug("SetEvent after");
	return 0;
}

TDM_TDEVICEAPI int SetEventTest(void* event)
{
	EVENT *pEvent = (PEVENT)event;
	int ret = 0;

	//TDDebug("SetEvent before");
	//pthread_mutex_lock (&(pEvent->ref_mutex));
	//if(!pEvent->ref)
	//{
	//	pEvent->ref = 1;
	//}
       // pthread_mutex_unlock (&(pEvent->ref_mutex));
	//usleep(10);
	//ret = 0;
	//usleep(10);
	//if(pEvent->ref)               //jzn test   not the best method
	//pthread_cond_signal(&(pEvent->cond));	
	//TDDebug("SetEvent after");

	while(pEvent->ref != 1)
	{
TDDebug("pEvent->ref != 1, so sleep\n");
		usleep(10);
	}

	pthread_mutex_lock (&(pEvent->mutex));
	pthread_cond_signal(&(pEvent->cond));	
       	pthread_mutex_unlock (&(pEvent->mutex));
	return 0;
}

TDM_TDEVICEAPI int WaitForSingleObjectTest(void* event, int timeout)
{
	EVENT *pEvent = (PEVENT)event;
	int ret = 0;

/////////////////////////////////////
	struct timeval tv;
	struct timespec waittime;
	gettimeofday(&tv, 0);
	waittime.tv_sec = tv.tv_sec + 8;//timeout / 1000;
	waittime.tv_nsec = tv.tv_usec * 1000;// + (timeout % 1000) * 1000000; 
/////////////////////////////////////

	pthread_mutex_lock (&(pEvent->mutex));
	pEvent->ref = 1;
	//ret = pthread_cond_wait(&(pEvent->cond), &(pEvent->mutex));
	ret = pthread_cond_timedwait(&(pEvent->cond), &(pEvent->mutex), &waittime);
	pEvent->ref = 0;
       	pthread_mutex_unlock (&(pEvent->mutex));
	switch(ret)
	{
	case ETIMEDOUT://time out
		return WAIT_TIMEOUT;
	case 0://success
		pthread_mutex_lock (&(pEvent->ref_mutex));
		pEvent->ref = 0;
		pthread_mutex_unlock(&(pEvent->ref_mutex));
		return WAIT_OBJECT_0;
	default://failed or fail for another reason suchas INVAL
		return -1;
	}		
	return WAIT_OBJECT_0;
}

TDM_TDEVICEAPI int WaitForSingleObject(void* event, int timeout)
{
	EVENT *pEvent = (PEVENT)event;
	int ret = 0;
	struct timeval tv;
	struct timespec waittime;

	gettimeofday(&tv, 0);

	//if(timeout < 1000)
	//{
	     //waittime.tv_sec = time(NULL);
   	    // waittime.tv_nsec = (time(NULL)*1000 + timeout)*1000;
			
	//}
  	//else
	//{
  	 //    waittime.tv_sec = time(NULL) + timeout/1000;
   	 //    waittime.tv_nsec = 0;	
	//}

	//modify by jiazhenning 2006-12-29
	//for wait for msec error
	waittime.tv_sec = tv.tv_sec + timeout / 1000;
	waittime.tv_nsec = tv.tv_usec * 1000 + (timeout % 1000) * 1000000; 

	if(waittime.tv_nsec / 1000000000)
	{
		waittime.tv_sec++;
		waittime.tv_nsec = waittime.tv_nsec % 1000000000;
	}

	
	if(pEvent->ref > 0)//there is a event before wait
	{
		//TDDebug("wait pEvent->ref > 0 before");
		pthread_mutex_lock (&(pEvent->ref_mutex));
		pEvent->ref = 0;
		pthread_mutex_unlock(&(pEvent->ref_mutex));
		//TDDebug("wait pEvent->ref > 0 after");
		return WAIT_OBJECT_0;
	}
	else if(pEvent->ref == 0)
	{
		switch(timeout)
		{
		case 0xffffffff://wait infinite
			//TDDebug("wait pEvent->ref = 0 begin");
			ret = pthread_cond_wait(&(pEvent->cond), &(pEvent->mutex));
			//TDDebug("wait pEvent->ref = 0 after");
			break;
		case 0://check status
//			TDDebug("wait pEvent->ref = 0 begin1");
			pthread_mutex_lock (&(pEvent->ref_mutex));
			if(pEvent->ref > 0)
			{
				pEvent->ref = 0;
				pthread_mutex_unlock(&(pEvent->ref_mutex));
//				TDDebug("wait pEvent->ref = 0 begin2");
				return WAIT_OBJECT_0;
			}
			pthread_mutex_unlock(&(pEvent->ref_mutex));
//			TDDebug("wait pEvent->ref = 0 begin3");
			return 1;
		default://wait with timeout
//			TDDebug("wait pEvent->ref = 0 begin4");
			ret = pthread_cond_timedwait(&(pEvent->cond), &(pEvent->mutex), &waittime);
//			TDDebug("wait pEvent->ref = 0 begin5");
			break;

		}
//		TDDebug("wait pEvent->ref = 0 begin6");
		
//		TDDebug("wait pEvent->ref = 0 begin8");

		switch(ret)
		{
		case ETIMEDOUT://time out
//			TDDebug("wait pEvent->ref = 0 begin9");
			return WAIT_TIMEOUT;
		case 0://success
//			TDDebug("wait pEvent->ref = 0 begin10");
			pthread_mutex_lock (&(pEvent->ref_mutex));
	//		TDDebug("wait pEvent->ref = 0 begin7");
			pEvent->ref = 0;
			pthread_mutex_unlock(&(pEvent->ref_mutex));
			break;
		default://failed or fail for another reason suchas INVAL
//			TDDebug("wait pEvent->ref = 0 begin11");
			return -1;
		}		
	}   
//	TDDebug("wait pEvent->ref = 0 after1");

	return WAIT_OBJECT_0;
}

TDM_TDEVICEAPI void CloseHandle(void* event)
{	
	EVENT *pEvent = (PEVENT)event;

	pthread_mutex_destroy (&(pEvent->mutex));
	pthread_mutex_destroy (&(pEvent->ref_mutex));
     
    	while (pthread_cond_destroy(&pEvent->cond) == EBUSY) 
	{
		TDDebug("CloseHandle begin1");
        	pthread_cond_broadcast(&pEvent->cond);
        	sched_yield();
	}

	free(pEvent);
	pEvent = NULL;	
}


#else


TDM_TDEVICEAPI void* CreateEvent()
{
	EVENT *pEvent =  (PEVENT)malloc(sizeof(EVENT));
	pEvent->ref = 0;
	TDDebug("CreateEvent---------test");
	if (pthread_mutex_init (&(pEvent->mutex),NULL) == -1)
	{
		TDDebug("pthread_mutex_init---------test fail");
     		return NULL;
	}
        if (pthread_cond_init (&(pEvent->cond), NULL) == -1)
	{
		TDDebug("pthread_cond_init---------test fail");
		return NULL;    
	}
	TDDebug("CreateEvent---------test ok");
	return pEvent;
}

TDM_TDEVICEAPI int SetEvent(void* event)
{
	EVENT *pEvent = (PEVENT)event;
	int ret = 0;	

	TDDebug("SetEvent  pthread_mutex_lock---------test enter");
	pthread_mutex_lock (&(pEvent->mutex));

	TDDebug("SetEvent  pthread_cond_signal---------test enter");
	
	ret = pthread_cond_signal(&(pEvent->cond));
	if(ret != 0)
	{
		TDDebug("SetEvent  pthread_cond_signal---------test fail");
		pthread_mutex_unlock(&(pEvent->mutex));
		return -1;
	}
	(pEvent->ref)++;
	
        pthread_mutex_unlock (&(pEvent->mutex));
	TDDebug("SetEvent  pthread_mutex_unlock---------test outer");
	
	return 0;
}

TDM_TDEVICEAPI int WaitForSingleObject(void* event, int timeout)
{
	EVENT *pEvent = (PEVENT)event;
	int ret = 0;

	struct timespec waittime;
	struct timespec now;

        gettimeofday(&now);
	if(timeout < 100)
	{
	     waittime.tv_sec  =  now.tv_sec;
   	     waittime.tv_nsec =  now.tv_nsec * 1000 + timeout*1000;	
	}
  	else
	{
  	     waittime.tv_sec  =  now.tv_sec + timeout/100;
   	     waittime.tv_nsec =  now.tv_nsec * 1000;	
	}
	TDDebug("WaitForSingleObject  pthread_mutex_lock---------test enter");
	pthread_mutex_lock (&(pEvent->mutex));
	TDDebug("WaitForSingleObject  pthread_mutex_lock---------test enter ok");

	{
		
		if(timeout == -1)//0xffffffff)
		{
			TDDebug("WaitForSingleObject  pthread_cond_wait---------test enter");
			ret = pthread_cond_wait(&(pEvent->cond), &(pEvent->mutex));
		}
		else
		{
			TDDebug("WaitForSingleObject  pthread_cond_timedwait---------test enter");
			ret = pthread_cond_timedwait(&(pEvent->cond), &(pEvent->mutex), &waittime);
		}
		TDDebug("WaitForSingleObject  pthread_cond_wait---------test enter ok");
		
		if(ret == -1)//failed 
		{
			TDDebug("WaitForSingleObject  pthread_cond_wait---------test fail");
			pthread_mutex_unlock(&(pEvent->mutex));
			return -1;
		}
		else if(ret == ETIMEDOUT)//time out
		{
			TDDebug("WaitForSingleObject  pthread_cond_wait---------test timeout");
			pthread_mutex_unlock(&(pEvent->mutex));
			return 1;
					
		}
		else if(ret == 0)//success
		{
			TDDebug("WaitForSingleObject  pthread_cond_wait---------test ok");
			(pEvent->ref)--;
		}
		else//fail for another reason suchas INVAL
		{
			TDDebug("WaitForSingleObject  pthread_cond_wait---------test fail1");
			pthread_mutex_unlock(&(pEvent->mutex));
			return -1;
		}
	}

        pthread_mutex_unlock (&(pEvent->mutex));
	return 0;
}

TDM_TDEVICEAPI void CloseHandle(void* event)
{
	EVENT *pEvent = (PEVENT)event;

	TDDebug("CloseHandle---------test");
	pthread_mutex_destroy (&(pEvent->mutex));
    	while (pthread_cond_destroy(&pEvent->cond) == EBUSY) 
	{
		TDDebug("CloseHandle---------test-----pthread_cond_destroy--------ebusy");
        	pthread_cond_broadcast(&pEvent->cond);
        	sched_yield();
	}
	free(pEvent);
	pEvent = NULL;	
}

#endif
TDSocketGetMacaddress(char *a)
{
}

#endif

