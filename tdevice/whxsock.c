#include "../comserv/comserv.h"
#include	"../platabs/platabs.h"
#include	"../js/tdmalloc.h"
#include "../platabs/tcomdef.h"
//szj #include "../js/tstypes.h"

#include "tdevice.h"
#include "whxfile.h"
#include "whxsock.h"

#ifdef XP_UNIX
#include <netinet/in.h>    //for socket
#include <netdb.h>
#define __need_timeval
#include <bits/time.h>    //for struct "timeval"
#endif

#define closesocket(s)  close(s)
#define SOCKET_ERROR -1

#undef  XP_PC

#ifdef XP_PC

#ifndef UNICODE
#define _T(a) a
#endif

#include <winsock.h>
#elif defined(XP_CCUBE)
#include "socket.h"             /*socket*/
#include "sockLib.h"             /*socket*/
#include "in.h"
#include "inetlib.h"
#include "netdb.h"
#include "hostlib.h"
#define closesocket(s)  close(s)
#define SOCKET_ERROR ERROR
#endif


#ifdef XP_UNIX

TDPString  TDGetProxyInfo(TDPCHAR info)
{
   	 FILE *fp;
	 TDPString  reStr,tmpStr;
	 TDBOOL bCreateTmpStr;
	 TDINT i;
	 TDCHAR *s;
	 TDCHAR buffer[10000];

	 fp = fopen("/usr/local/thunder/config.in","r");
	 if(!fp)
	 {
		TDDebug(" /usr/local/thunder/config.in open error");
		return TDNULL;
	 }
 	 reStr = TDString_Create();
  	 tmpStr=TDString_Create();

	while(1)
	{
		 s = fgets(buffer,sizeof(buffer) ,fp);
	 	if(!s)
			break;

	 	TDString_Cut(tmpStr,0,tmpStr->mLength);
   		for(i=0;i<strlen(s);i++)
	 	{
     			TDString_AppendChar(tmpStr,buffer[i]);

	 	}
	 	if(-1==TDString_FindPChar(tmpStr,info,TDTRUE))
      			continue;

   		else
	 	{
			TDINT start;
  			start = TDString_FindPChar(tmpStr,"=",TDTRUE);
   			if(start >0)
			{
				TDString_Cut(tmpStr,0,start+1);
				TDString_AppendString(reStr,tmpStr,tmpStr->mLength);
				TDString_StripWhitespace(reStr);
				break;
			}
	 	}
	}

	 fclose(fp);
 	 TDString_Destroy(tmpStr);
	 return reStr;
}
TDINT  TDGetProxyInfoInt(TDPCHAR info)
{
   	 FILE *fp;
	 TDPString  reStr,tmpStr;
	 TDBOOL bCreateTmpStr;
	 TDINT i;
	 TDCHAR *s;
	 TDCHAR buffer[10000];

	 fp = fopen("/usr/local/thunder/config.in","r");
	 if(!fp)
	 {
		TDDebug(" /usr/local/thunder/config.in open error");
		return TDNULL;
	 }
	 reStr = TDString_Create();
	 tmpStr=TDString_Create();

	while(1)
	{
		 s = fgets(buffer,sizeof(buffer) ,fp);
		 if(!s)
			break;

	 	TDString_Cut(tmpStr,0,tmpStr->mLength);
  		 for(i=0;i<strlen(s);i++)
		 {
     			TDString_AppendChar(tmpStr,buffer[i]);

	 	}
	 	if(-1==TDString_FindPChar(tmpStr,info,TDTRUE))
      			continue;

  		else
	 	{
			TDINT start;
  			start = TDString_FindPChar(tmpStr,"=",TDTRUE);
      			if(start >0)
			{
				TDString_Cut(tmpStr,0,start+1);
				TDString_AppendString(reStr,tmpStr,tmpStr->mLength);
				TDString_StripWhitespace(reStr);
				break;
			}
	 	}
}

	 fclose(fp);
   	 TDString_Destroy(tmpStr);	 
	 return atoi(reStr->mStr);
}

TDPString  TDGetNetworkInfo(TDPCHAR info)
{
   	 FILE *fp;
	 TDPString  reStr,tmpStr;
	 TDBOOL bCreateTmpStr;
	 TDINT i;
	 TDCHAR *s;
	 TDCHAR buffer[10000];

	 fp = fopen("/etc/network","r");
	 if(!fp)
	 {
		TDDebug(" /etc/network open error in info\n");
		return TDNULL;
	 }
 	 reStr = TDString_Create();
  	 tmpStr=TDString_Create();

	while(1)
	{
		s = fgets(buffer,sizeof(buffer) ,fp);
	 	if(!s)
			break;

	 	TDString_Cut(tmpStr,0,tmpStr->mLength);
   		for(i=0;i<strlen(s);i++)
	 	{
     			TDString_AppendChar(tmpStr,buffer[i]);

	 	}
	 	if(-1==TDString_FindPChar(tmpStr,info,TDTRUE))
      			continue;

   		else
	 	{
			TDINT start;
  			start = TDString_FindPChar(tmpStr,"=",TDTRUE);
   			if(start >0)
			{
				TDString_Cut(tmpStr,0,start+1);
				TDString_AppendString(reStr,tmpStr,tmpStr->mLength);
				TDString_StripWhitespace(reStr);
				break;
			}
	 	}
	}

	 fclose(fp);
 	 TDString_Destroy(tmpStr);
	 return reStr;
}




#endif




#define IPPORT_DEFAULT_URL 11111
char gFindDefaultUrlCmd[] = "looking for default url";
char gFindHttpProxyCmd[] = "looking for http proxy";
char gFindDefaultUrlResult[] = "Default URL:::::";
char gEnableHttpProxyResult[] = "HTTP Proxy:::::";
char gDisableHttpProxyResult[] = "No HTTP Proxy";

typedef struct TDSocket{
	TDINT		mType;			/*added by zk*/
	TDINT		m_nSendTimeout;
	TDINT		m_nRecvTimeout;
	TDPString	mHost;
#ifdef XP_PC
	SOCKET       Socket;
#elif defined(XP_CCUBE)
	TDINT		Socket;
#else
	TDINT   Socket;
#endif
	struct sockaddr_in  sinSocket;
	struct sockaddr_in  sinServer;
}TDSocket;
#ifdef XP_PC
WSADATA           gwsaData;
#endif
TDBOOL gSocketInited=TDFALSE;


extern TDBOOL gUsingProxy;
extern TDPString gProxy;
extern TDWORD gProxyPort;

TDBOOL 
TDSocketInit()
{
#ifdef XP_PC
	HKEY key;
	TCHAR path[200]=_T("SOFTWARE\\THUNDER");
#ifdef UNICODE
	WORD buf[100];
#else
	char buf[100];
#endif
	DWORD type,s,*pDword;
	if(!WSAStartup(MAKEWORD(1,0),&gwsaData)==0)
	{
		WSACleanup();
		return TDFALSE;
	}

	if(ERROR_SUCCESS ==RegOpenKeyEx(HKEY_CURRENT_USER,path, 0,KEY_READ ,&key))
	{
		s=100;
		if(ERROR_SUCCESS == RegQueryValueEx(key,_T("proxyEnable"), 0, &type, (LPBYTE)buf,&s))
		{
			pDword=(DWORD*)buf;
			if(type==REG_SZ && _T('1')==buf[0])
			{
				s=100;
				if(ERROR_SUCCESS == RegQueryValueEx(key,_T("ProxyServer"), 0, &type, (LPBYTE)buf,&s))
				{
					if(type==REG_SZ )
					{
		#ifdef UNICODE
						gProxy=TD_PTCharToPString(buf);
		#else
						gProxy=TDString_Create();
						TDString_AssignPChar(gProxy,buf);
		#endif
						gUsingProxy=TDTRUE;
					}
				}
			}
		}
		if(ERROR_SUCCESS == RegQueryValueEx(key,_T("DebugEnable"), 0, &type, (LPBYTE)buf,&s))
		{	
			if(type==REG_SZ && _T('1')==buf[0])
			{
				gDebugEnable=1;
			}
		}
		RegCloseKey (key);
	}
	if(gProxy)
		TDDebug(gProxy->mStr);
#elif defined(XP_UNIX)
/*		TDPString prStr;
		prStr =TDGetProxyInfo("proxyEnable");		
	  if(TDString_ComparePChar(prStr,"1",TDTRUE,1)==0)
		{
			gUsingProxy=TDTRUE;
			gProxy=TDGetProxyInfo("ProxyServer");
//			TDDebug(gProxy->mStr);
	  }*/
	
#endif
	gSocketInited=TDTRUE;
	return TDTRUE;
}

TDBOOL TDSocketDestroy()
{
#ifdef XP_PC
	if(gSocketInited)
		WSACleanup();
#elif defined(XP_CCUBE)
	if(gSocketInited){}
#endif
	return TDTRUE;
}

TDHANDLE
TDSocketOpen(TDPString pHost,TDINT in_port,TDINT svr_port,TDINT type)
{
	TDSocket* pThis;
	TDINT socket_type,recvbuflen = 65512;
	struct hostent * heServHostInfo=NULL;

	if(!pHost||svr_port<0)
		return TDNULL;
	switch(type)
	{
	case SOCKET_STREAM_TYPE:
		socket_type = SOCK_STREAM;
		break;
	case SOCKET_DGRAM_TYPE:
		socket_type = SOCK_DGRAM;
		break;
	default:
		return TDNULL;
	}
	pThis = (TDSocket*)Malloc(sizeof(TDSocket));
	pThis->mHost=TDString_Create();
	TDString_Assign(pThis->mHost,pHost,-1);
	pThis->sinServer.sin_port=htons(svr_port);
	pThis->sinSocket.sin_port=htons(in_port);
	pThis->m_nSendTimeout=30000;
	pThis->m_nRecvTimeout=30000;
	pThis->mType = type;
	pThis->Socket=socket(AF_INET,socket_type,0);
#ifdef XP_PC
	if (pThis->Socket==INVALID_SOCKET)
#elif defined(XP_CCUBE)
	if (pThis->Socket<=0)
#elif defined(XP_UNIX)
	if(pThis->Socket <0)
#endif
	{
		TDFree(pThis);
		return TDNULL;
	}


    //local settings
	#ifdef XP_PC
		pThis->sinSocket.sin_addr.S_un.S_addr=htonl (INADDR_ANY);
	#elif defined(XP_CCUBE)
		pThis->sinSocket.sin_addr.s_addr=htonl (INADDR_ANY);
	#elif defined(XP_UNIX)
		pThis->sinSocket.sin_addr.s_addr =  htonl(INADDR_ANY);
	#endif
    pThis->sinSocket.sin_family=AF_INET;

    //bind socket
	if (bind (pThis->Socket,                          
              ( struct sockaddr *)&pThis->sinSocket,  
              sizeof(pThis->sinSocket))==  SOCKET_ERROR)
	{
	   	closesocket(pThis->Socket);
	   	TDFree(pThis);
		return TDNULL;
	}
	if(in_port==0)
	{
		TDINT socksize;
		socksize = sizeof(pThis->sinSocket);
		/*get actual port number*/
		getsockname(pThis->Socket,(struct sockaddr *)&pThis->sinSocket,&socksize);
	}

	
	{
		unsigned long tmpIAddr=0;
		tmpIAddr=inet_addr(pThis->mHost->mStr);
		if(tmpIAddr!=INADDR_NONE)
		{
			#ifdef XP_PC
				pThis->sinServer.sin_addr.S_un.S_addr=tmpIAddr;
			#elif defined(XP_CCUBE)
				pThis->sinServer.sin_addr.s_addr=tmpIAddr;
			#elif defined(XP_UNIX)
				pThis->sinServer.sin_addr.s_addr = tmpIAddr;
			#endif
		}
		else
		{
			//try find by name
		#ifdef XP_PC
			heServHostInfo=gethostbyname((char *)pThis->mHost->mStr);
		#elif defined(XP_CCUBE)
			heServHostInfo=hostGetByName((char *)pThis->mHost->mStr);
		#elif defined(XP_UNIX)
			heServHostInfo=gethostbyname((char *)pThis->mHost->mStr);
		#endif
			if(heServHostInfo==NULL 
		#ifdef XP_CCUBE
				||heServHostInfo==0xffffffff
		#endif
				)
			{
					closesocket(pThis->Socket);
					TDFree(pThis);
					return TDNULL;
			}
			else/*found by name*/
		#ifdef XP_PC
				pThis->sinServer.sin_addr.S_un.S_addr=*((long *)(heServHostInfo->h_addr));
		#elif defined(XP_CCUBE)
			pThis->sinServer.sin_addr.s_addr=*((long *)(heServHostInfo->h_addr));
		#elif defined(XP_UNIX)
{
			pThis->sinServer.sin_addr.s_addr= *((long *)(heServHostInfo->h_addr));
//	unsigned char * pt= heServHostInfo->h_addr_list[0];
//	memcpy(&pThis->sinServer.sin_addr,pt, 4 )   ;
                      //Will be DO!!!!1  uint32_t    s_addr      h_addr_list[0]
}
		#endif
		
		}
		pThis->sinServer.sin_family=AF_INET;
		
	}
	
	if(type==SOCKET_STREAM_TYPE)
	{
		if(connect(pThis->Socket
			,( struct sockaddr  *)&pThis->sinServer
			,sizeof(pThis->sinServer))==SOCKET_ERROR)
		{
			closesocket(pThis->Socket);
			TDFree(pThis);
			return TDNULL;
		}
	}
	if(type==SOCKET_DGRAM_TYPE)
	{
		if(setsockopt(pThis->Socket,SOL_SOCKET,SO_RCVBUF
			,(char *)&recvbuflen,sizeof(TDINT))!=0)
		{
//			closesocket(pThis->Socket);
			TDFree(pThis);
//			return TDNULL;
		}

	}	return (TDHANDLE)pThis;

}

TDVOID		
TDSocketClose(TDHANDLE hFile)
{
	TDSocket * pThis=(TDSocket * )hFile;
	if(!pThis)
		return ;
	closesocket(pThis->Socket);
	TDString_Destroy(pThis->mHost);
	TDFree(pThis);
	return;
}

TDM_TDEVICEAPI	TDINT		TDSetNoBlockSatus(TDHANDLE hFile)
{
	TDSocket *pThis;
	TDINT iRet;
	int    b_on = 1;
	pThis =	(TDSocket*)hFile;
	if(!pThis)
		return -1;
        iRet = ioctl(pThis->Socket,FIONBIO,&b_on);
	return iRet;
}
TDDWORD
TDSocketRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead)
{
	TDSocket *pThis;
	fd_set readfds;
	TDINT curSize=-1;
	TDINT ret = -1;
	TDINT nErrorNum = 0;
	char temp[56];


#ifdef XP_PC
	TIMEVAL  interval;
#elif defined XP_UNIX
	struct timeval	 interval;
#endif

	pThis =	(TDSocket*)hFile;
	if(!pThis)
		return -1;
error:	
	FD_ZERO(&readfds);
	FD_SET(pThis->Socket,&readfds);
	interval.tv_sec = 0 ;
#ifdef XP_PC
	interval.tv_usec = pThis->m_nRecvTimeout*1000;
	ret = select(0,&readfds,0,0,&interval);
	
#elif defined(XP_UNIX)
//szj	interval.tv_sec = pThis->m_nRecvTimeout;
	interval.tv_sec = 8;
	interval.tv_usec = 0;
	
	ret = select((pThis->Socket)+1,&readfds,NULL,NULL,&interval);
	
	
#endif
	if(ret ==SOCKET_ERROR/* || ret==0*/)
	{
		
		//if(nErrorNum > 5)
		//{
			sprintf(temp, "pThis->Socket:%d, error = %d!  %s.", pThis->Socket, ret, pThis->mHost->mStr);
			TDDebug(temp);
			TDDebug("select error");
			return -1;
		//}
		//else
	//	{
	//		nErrorNum++;
	//		goto error;
	//	}
	}

	if(ret == 0)
	{
		TDDebug("ret == 0\n");
		return 0;
	}



	if(pThis->mType == SOCKET_STREAM_TYPE)
	{
		
		curSize=recv (pThis->Socket
                    ,(char*)pBuf
                     ,BytesToRead
			        ,0);
		
		if(curSize==SOCKET_ERROR)
		{
			TDDebug("recv error");
			return -1;
		}
		
	}
	if(pThis->mType == SOCKET_DGRAM_TYPE)
	{
		curSize=recvfrom (pThis->Socket
                    ,(char *)pBuf
                     ,BytesToRead
			        ,0,NULL,NULL);
		if(curSize==SOCKET_ERROR)
			return -1;
	}
	
	
	return curSize;
}

TDDWORD
TDSocketReadUntilDone(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead)
{
	TDSocket * pThis=(TDSocket * )hFile;
	TDDWORD curSize=0,recSize=-1;
	char temp[56];
	if(!pThis||BytesToRead<=0)
		return -1;
	
	while(curSize<BytesToRead)
    	{
		
		recSize=TDSocketRead(hFile,
					((char *)pBuf)+curSize,
					BytesToRead-curSize);
		
		if(-1==recSize || recSize == 0)
			return -1;
		curSize+=recSize;
	}
#if 0
	{
		HANDLE handle;
		TDDWORD hSize;
		handle= CreateFile("c:\\read.txt",GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_WRITE_THROUGH,NULL);
		if (handle == INVALID_HANDLE_VALUE)
		{
			 return TDFALSE;
		}
		SetFilePointer(handle,0,NULL,FILE_END);
		WriteFile(handle,pBuf,BytesToRead,&hSize,TDNULL);
		CloseHandle(handle);
	}
#endif
	return curSize;
}

TDDWORD
TDSocketWrite(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToWrite)
{
	TDSocket *pThis;
	TDINT  curSize;
	pThis = (TDSocket * )hFile;
	if(!pThis)
		return -1;
	if(pThis->mType == SOCKET_STREAM_TYPE)
	{
		if(BytesToWrite<=0 || 
			(curSize = send (pThis->Socket,(char*)pBuf,
			BytesToWrite,0))==SOCKET_ERROR)
			return -1;
	}

	if(pThis->mType == SOCKET_DGRAM_TYPE)
	{
		curSize=sendto(pThis->Socket,
					(char *)pBuf,
					BytesToWrite,
					0,
					(struct sockaddr*)&pThis->sinServer,
					sizeof(pThis->sinServer));
		if(curSize==SOCKET_ERROR)
			return -1;
	}
	return curSize;
}

TDDWORD
TDSocketWriteUntilDone(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToWrite)
{
	TDSocket * pThis=(TDSocket * )hFile;
	TDDWORD  curSize=0,sndSize;
	if(!pThis||BytesToWrite<=0)
		return -1;
	while(curSize<BytesToWrite)
	{
		if(BytesToWrite<=0 || 
			(sndSize = TDSocketWrite(hFile,
			((char *)pBuf)+curSize,
			BytesToWrite-curSize))==-1)
			return -1;
		curSize+=sndSize;
	}
#if 0
	{
		HANDLE handle;
		TDDWORD hSize;
		handle= CreateFile("c:\\write.txt",GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_WRITE_THROUGH,NULL);
		if (handle == INVALID_HANDLE_VALUE)
		{
			 return TDFALSE;
		}
		SetFilePointer(handle,0,NULL,FILE_END);
		WriteFile(handle,pBuf,BytesToWrite,&hSize,TDNULL);
		CloseHandle(handle);
	}
#endif


	return curSize;
}

TDBOOL TDSocketSetServeraddr(TDHANDLE hFile,TDDWORD IPaddr,TDSHORT nPort)
{
	TDSocket *pThis;

	pThis = (TDSocket * )hFile;
	if(!pThis)
		return TDFALSE;

	if(IPaddr!=0)
	{
		#ifdef XP_PC
				pThis->sinServer.sin_addr.S_un.S_addr=IPaddr;
		#elif defined(XP_CCUBE)
				pThis->sinServer.sin_addr.s_addr=IPaddr;
		#elif defined(XP_UNIX)
				pThis->sinServer.sin_addr.s_addr=IPaddr;
		#endif
	}
	if(nPort!=0)
	{
		pThis->sinServer.sin_port = htons(nPort);
	}
	return TDTRUE;
}

TDBOOL TDSocketSetRecvTimeout(TDHANDLE hFile,TDINT nms)
{
	TDSocket *pThis;

	pThis = (TDSocket * )hFile;
	if(!pThis)
		return TDFALSE;
	pThis->m_nRecvTimeout = nms;
	return TDTRUE;
}


TDWORD
TDSocketGetLocalPort(TDHANDLE hFile)
{
	TDSocket* pThis = (TDSocket*)hFile;

	return ntohs(pThis->sinSocket.sin_port);
}



TDINT TDSocketGetMsgLen(TDHANDLE hFile)
{
	TDSocket *pThis;
	TDDWORD length;
	TDINT sizeoflength;

	pThis = (TDSocket * )hFile;
	if(!pThis)
		return 0;
	sizeoflength = sizeof(length);
	getsockopt(pThis->Socket,SOL_SOCKET,SO_RCVBUF,(char*)&length,&sizeoflength);

	return length;

}

/*
TDVOID TDDebugInfo(TDCHAR *buf)
{
	SOCKET tddebug_socket;
	struct sockaddr_in sinServer,sinSocket;
	TDINT opt = 1,optlen;

	tddebug_socket = socket(AF_INET,SOCK_DGRAM,0);

#ifdef XP_PC
	if(tddebug_socket == INVALID_SOCKET)
#elif defined(XP_CCUBE)
	if (tddebug_socket<=0)
#endif
		return;

#ifdef XP_PC
	sinSocket.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#elif defined(XP_CCUBE)
	sinSocket.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
	sinSocket.sin_family=AF_INET;
	sinSocket.sin_port = htons(0);

    if ( SOCKET_ERROR == bind ( tddebug_socket, (SOCKADDR FAR *)&sinSocket, sizeof ( SOCKADDR_IN ) ) ) 
    {
		closesocket(tddebug_socket);
		return ;
    }


#ifdef XP_PC
	sinServer.sin_addr.S_un.S_addr = INADDR_BROADCAST;//inet_addr("192.168.0.255");
#elif defined(XP_CCUBE)
	sinServer.sin_addr.s_addr = inet_addr("192.168.0.255");
#endif
	sinServer.sin_family=AF_INET;
	sinServer.sin_port = htons(5000);

	optlen = sizeof(opt);
	setsockopt(tddebug_socket,SOL_SOCKET,SO_BROADCAST,(char*)&opt,optlen);
	sendto(tddebug_socket,buf,TDStrLen(buf)+1,0,(struct sockaddr*)&sinServer,sizeof(sinServer));
	closesocket(tddebug_socket);
}
*/

TDDWORD
TDSocketBlockRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead)
{
	TDSocket *pThis;
	TDINT curSize=0;
	
	pThis =	(TDSocket*)hFile;
	if(!pThis)
		return -1; 
	if(pThis->mType == SOCKET_STREAM_TYPE)
	{
		curSize=recv (pThis->Socket
                    ,(char*)pBuf
                     ,BytesToRead
			        ,0);
		if(curSize==SOCKET_ERROR)
			return -1;
	}
	if(pThis->mType == SOCKET_DGRAM_TYPE)
	{
		curSize=recvfrom (pThis->Socket
                    ,(char *)pBuf
                     ,BytesToRead
			        ,0,NULL,NULL);
		if(curSize==SOCKET_ERROR)
			return -1;
	}
	return curSize;
}

TDBOOL
TDFindDefaultUrl(TDPCHAR url)
{
	TDINT tdfind_socket;
	struct sockaddr_in sinServer,sinSocket;
	TDINT opt = 1,optlen;
	fd_set readfds;
	TDINT ret;
#ifdef XP_PC
	TIMEVAL interval;
#elif defined(XP_UNIX)
	struct timeval interval;
#endif
	TDCHAR pBuf[1024];
	
	if(!url)
		return TDFALSE;

	tdfind_socket = socket(AF_INET,SOCK_DGRAM,0);
	
#ifdef XP_PC
	if(tdfind_socket == INVALID_SOCKET)
#elif defined(XP_CCUBE)
		if (tdfind_socket<=0)
#elif defined(XP_UNIX)
		if(tdfind_socket <0)
#endif
			return TDFALSE;
		
#ifdef XP_PC
		sinSocket.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#elif defined(XP_CCUBE)
		sinSocket.sin_addr.s_addr = htonl(INADDR_ANY);
#elif defined(XP_UNIX)
		sinSocket.sin_addr.s_addr = htonl(INADDR_ANY);		
#endif
		sinSocket.sin_family=AF_INET;
		sinSocket.sin_port = htons(0);
		
		if ( SOCKET_ERROR == bind ( tdfind_socket, (struct sockaddr_in *)&sinSocket, sizeof ( struct sockaddr_in ) ) )
		{
			closesocket(tdfind_socket);
			return TDFALSE;
		}
		
		
#ifdef XP_PC
		sinServer.sin_addr.S_un.S_addr = INADDR_BROADCAST;//inet_addr("192.168.0.255");
#elif defined(XP_CCUBE)
		sinServer.sin_addr.s_addr = INADDR_BROADCAST;
#elif defined(XP_UNIX)
		sinServer.sin_addr.s_addr = INADDR_BROADCAST;
#endif
		sinServer.sin_family=AF_INET;
		sinServer.sin_port = htons(IPPORT_DEFAULT_URL);
		
		optlen = sizeof(opt);
		setsockopt(tdfind_socket,SOL_SOCKET,SO_BROADCAST,(char*)&opt,optlen);
	
		sendto(tdfind_socket,gFindDefaultUrlCmd,TDStrLen(gFindDefaultUrlCmd)+1,0,(struct sockaddr*)&sinServer,sizeof(sinServer));
	
getresult:
		FD_ZERO(&readfds);
		FD_SET(tdfind_socket,&readfds);
		interval.tv_sec = 0 ;
		interval.tv_usec = 1000*1000;
		
		ret = select(0,&readfds,0,0,&interval);
		if(ret ==SOCKET_ERROR || ret==0)
		{
			closesocket(tdfind_socket);
			return TDFALSE;
		}

		ret = recvfrom (tdfind_socket,pBuf,1024,0,NULL,NULL);
		if(ret==SOCKET_ERROR)
			return -1;
		pBuf[1023] = 0;

		{
			TDPString pResult;
			TDINT start;
			pResult = TDString_Create();
			TDString_InitByChar(pResult,pBuf);

			start = TDString_FindPChar(pResult,gFindDefaultUrlResult,TDFALSE);
			if(start!=0)
			{
				TDString_Destroy(pResult);
				goto getresult;
			}
			TDString_Cut(pResult,0,TDStrLen(gFindDefaultUrlResult));

			TDstrcpy(url,pResult->mStr);

			TDString_Destroy(pResult);
		}

		closesocket(tdfind_socket);
		return TDTRUE;
}

TDBOOL
TDFindHttpProxy()
{
	TDINT tdfind_socket;
	struct sockaddr_in sinServer,sinSocket;
	TDINT opt = 1,optlen;
	fd_set readfds;
	TDINT ret;
	struct timeval interval;
	TDCHAR pBuf[1024];
	

	tdfind_socket = socket(AF_INET,SOCK_DGRAM,0);
	
#ifdef XP_PC
	if(tdfind_socket == INVALID_SOCKET)
#elif defined(XP_CCUBE)
		if (tdfind_socket<=0)
#elif defined(XP_UNIX)
		if (tdfind_socket<0)
#endif
			return TDFALSE;
		
#ifdef XP_PC
		sinSocket.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#elif defined(XP_CCUBE)
		sinSocket.sin_addr.s_addr = htonl(INADDR_ANY);
#elif defined(XP_UNIX)
		sinSocket.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
		sinSocket.sin_family=AF_INET;
		sinSocket.sin_port = htons(0);
		
		if(SOCKET_ERROR == bind ( tdfind_socket, (struct sockaddr_in *)&sinSocket, sizeof ( struct sockaddr_in ) ) )
		{
			closesocket(tdfind_socket);
			return TDFALSE;
		}
		
		
#ifdef XP_PC
		sinServer.sin_addr.S_un.S_addr = INADDR_BROADCAST;//inet_addr("192.168.0.255");
#elif defined(XP_CCUBE)
		sinServer.sin_addr.s_addr = INADDR_BROADCAST;
#elif defined(XP_UNIX)
		sinServer.sin_addr.s_addr = INADDR_BROADCAST;
#endif
		sinServer.sin_family=AF_INET;
		sinServer.sin_port = htons(IPPORT_DEFAULT_URL);
		
		optlen = sizeof(opt);
		setsockopt(tdfind_socket,SOL_SOCKET,SO_BROADCAST,(char*)&opt,optlen);
	
		sendto(tdfind_socket,gFindHttpProxyCmd,TDStrLen(gFindHttpProxyCmd)+1,0,(struct sockaddr*)&sinServer,sizeof(sinServer));
	
		FD_ZERO(&readfds);
		FD_SET(tdfind_socket,&readfds);
		interval.tv_sec = 0 ;
		interval.tv_usec = 1000*1000;
		
		ret = select(0,&readfds,0,0,&interval);
		if(ret ==SOCKET_ERROR || ret==0)
		{
			closesocket(tdfind_socket);
			return TDFALSE;
		}

		ret = recvfrom (tdfind_socket,pBuf,1024,0,NULL,NULL);
		if(ret==SOCKET_ERROR)
			return -1;
		pBuf[1023] = 0;

		{
			TDPString pResult;
			TDINT start;
			pResult = TDString_Create();
			TDString_InitByChar(pResult,pBuf);
			start = TDString_FindPChar(pResult,gDisableHttpProxyResult,TDFALSE);
			if(start==0)
			{
				TDString_Destroy(pResult);
				gUsingProxy = TDFALSE;
				goto quit;
			}

			start = TDString_FindPChar(pResult,gEnableHttpProxyResult,TDFALSE);
			if(start!=0)
			{
				TDString_Destroy(pResult);
				goto quit;
			}
			TDString_Cut(pResult,0,TDStrLen(gEnableHttpProxyResult));

			start = TDString_FindChar(pResult,':',0,TDTRUE);
			if(start>0)
			{
				TDINT err;
				if(!gUsingProxy)
					gUsingProxy = TDTRUE;
				if(!gProxy)	
					gProxy = TDString_Create();
				TDString_Mid(pResult,gProxy,0,start);
				TDString_Cut(pResult,0,start+1);
				gProxyPort = (TDWORD)TDString_ToInteger(pResult,&err,10);
			}
			TDString_Destroy(pResult);
		}

quit:
		closesocket(tdfind_socket);
		return TDTRUE;
}

