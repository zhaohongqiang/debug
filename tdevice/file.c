
//#include "../global/global.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../platabs/tcomdef.h"
#include "../comserv/Tdstring.h"
#include "file.h"

TDDWORD usEndPoint=6000;
static pthread_mutex_t gSingleEvent;//event to sign if client can be accepted
static TDBOOL gbinit=TDFALSE;
int fFile;
int	gDebugEnable = 1;

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



TDVOID TDDebugFunction(char * file,int  line,char * message)
{
	TDPString 	str;
	int  		tddebug_socket;
	struct 		sockaddr_in 	sinServer,sinSocket;
	TDINT 		opt = 1, optlen, pack=0;
	struct 	timeval tim;
	char 		tmp[100];
	char		czSvrItem[32] = "SERVER";
	static char	czBroadcastIP[32] = "192.168.1.100";
	char		czSvrIP[32] = "";

	if(!gDebugEnable)
	{
		return;
	}

	if(!message)
	{
		return;
	}

	if(!gbinit)
	{
	 	pthread_mutex_init(&gSingleEvent, NULL);
		gbinit=1;
	}
	pthread_mutex_lock(&gSingleEvent);
	tddebug_socket = socket(AF_INET,SOCK_DGRAM,0);

	if(tddebug_socket == -1)
	{
		pthread_mutex_unlock(&gSingleEvent);
		return;
	}

	sinSocket.sin_addr.s_addr = htonl(INADDR_ANY);
	sinSocket.sin_family=AF_INET;
	sinSocket.sin_port = htons(0);
    	if ( -1 == bind ( tddebug_socket, (struct sockaddr_in  *)&sinSocket, sizeof ( struct sockaddr_in ) ) )
    	{
		close(tddebug_socket);
		pthread_mutex_unlock(&gSingleEvent);
		return ;
	}

	if(strlen(czBroadcastIP) <= 0)
	{
		TDPString pUrl=NULL;
		pUrl = TDGetNetworkInfo("SERVER");//TDGetProxyInfo(name);
		if(pUrl)
		{
			strcpy(czBroadcastIP, pUrl->mStr);

			if (*czBroadcastIP)
			{
				int 	i = 0;
				int     j = 0;
				int	nFlag = 0;

				char szHostIP[18];
            	char *szTmp;
            	BYTE bTmp = 0;
				int m_nNetType = 0;
            	strcpy(szHostIP, czBroadcastIP);
            	szTmp = strstr(szHostIP, ".");
            	szHostIP[szTmp - szHostIP] = 0;
            	bTmp = atoi(szHostIP);

            	switch(bTmp & 0xC0)
				{
	            case 0xC0:
            	    m_nNetType = 3;//C
	            break;
            	case 0x80:
	            	m_nNetType = 2;//B
        		break;
            	case 0:
         		m_nNetType = 1;//A
        		break;
				}


				for(i = 0; i < strlen(czBroadcastIP) && i < 15; i++)
				{
					if(czBroadcastIP[i] == '.')
					{
						nFlag++;
						if(nFlag == m_nNetType)
							break;
					}
				}
				for(j = 0; j < 4 - m_nNetType; j++){
				    czBroadcastIP[++i] = '2';
		    		czBroadcastIP[++i] = '5';
		    		czBroadcastIP[++i] = '5';
		    		czBroadcastIP[++i] = '.';
				}
				czBroadcastIP[i] = '\0';
				printf("DEBUGIP++++++++++++  %s\n",czBroadcastIP);
			}
		}
	}
	sinServer.sin_addr.s_addr = inet_addr(czBroadcastIP);
	sinServer.sin_addr.s_addr = inet_addr("192.168.1.100");
	sinServer.sin_family=AF_INET;
	sinServer.sin_port = htons(usEndPoint+1);
	optlen = sizeof(opt);
printf("DEBUGIP++++++++++++  %s\n",czBroadcastIP);

	setsockopt(tddebug_socket,SOL_SOCKET,SO_BROADCAST,(char*)&opt,optlen);
	str=TDString_Create();
	TDString_AppendPChar(str,message);
	TDString_AppendPChar(str,"  <file: ");
	TDString_AppendPChar(str,file);
	TDString_AppendPChar(str,"  >  <line: ");
	TDString_AppendInt(str,line,10);
	TDString_AppendPChar(str,"  >  ");
	//gettimeofday(&tim,NULL);
	//sprintf(tmp,"(%u|%u)",tim.tv_sec,tim.tv_usec);
	//TDString_AppendPChar(str,tmp);

	TDString_AppendPChar(str,"  >  < datetime: ");
	TDString_AppendPChar(str,__DATE__);
	TDString_AppendPChar(str,"  ");
	TDString_AppendPChar(str,__TIME__);
	TDString_AppendPChar(str,"  >");


	while(pack<str->mLength)
	{
		if( (str->mLength-pack)>16000)
			sendto(tddebug_socket,str->mStr+pack,16000,0,(struct sockaddr*)&sinServer,sizeof(sinServer));
		else
			sendto(tddebug_socket,str->mStr+pack,str->mLength-pack,0,(struct sockaddr*)&sinServer,sizeof(sinServer));
		pack+=16000;
	}
//	printf("%s\n", str->mStr);
 	close(tddebug_socket);
	TDString_Destroy(str);
	pthread_mutex_unlock(&gSingleEvent);
	return;
}



TDVOID
InitializeCriticalSection(CRITICAL_SECTION* mute)
{
	//if(!mute)
	if(mute)
	{
		pthread_mutex_init(mute, NULL);
	}
}


TDM_TDEVICEAPI TDBOOL
EnterCriticalSection(CRITICAL_SECTION* pmut)
{
	pthread_mutex_t *mute;
	mute = (pthread_mutex_t*)pmut;

 	if(!pmut)
	{
		return TDFALSE;
	}

	pthread_mutex_lock(mute);
	return TDTRUE;
}

TDM_TDEVICEAPI TDBOOL
LeaveCriticalSection(CRITICAL_SECTION* pmut)
{
	pthread_mutex_t *mute;
	mute = (pthread_mutex_t*)pmut;
	if(!pmut)
		return TDFALSE;

	pthread_mutex_unlock(mute);

	return TDTRUE;
}

TDM_TDEVICEAPI TDBOOL
DeleteCriticalSection(CRITICAL_SECTION* pmut)
{
	pthread_mutex_t *mute;
	mute = (pthread_mutex_t*)pmut;
	if(!pmut)
		return TDFALSE;

	pthread_mutex_destroy(mute);

	return TDTRUE;
}

