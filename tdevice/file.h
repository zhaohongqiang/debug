#ifndef		TDM_SI_INCLUDED_FILE_H
#define		TDM_SI_INCLUDED_FILE_H
#include "tdevice.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define  CRITICAL_SECTION pthread_mutex_t
TDM_TDEVICEAPI void InitializeCriticalSection(CRITICAL_SECTION* gSection);
TDM_TDEVICEAPI TDBOOL WHXEnterCriticalSection(CRITICAL_SECTION * gSection);
TDM_TDEVICEAPI TDBOOL WHXLeaveCriticalSection(CRITICAL_SECTION* gSection);
#define WHXEnterCriticalSection EnterCriticalSection
#define WHXLeaveCriticalSection LeaveCriticalSection


TDM_TDEVICEAPI TDVOID	TDDebugFunction(char * file,int  line,char * message);


#define HELLO  3
void TDDebugW();

//#define TDDebug(msg) TDDebugFunction(__FILE__,__LINE__,(msg))
//#define TDDebug(msg)
/*
 *     TDDebug Plus (can input arguments now)
 *     TonyKong 2012.10.15
*/
#define TDDebug_MAXSIZE 512
char gTDMsg[TDDebug_MAXSIZE];

#define TDDebug(msg, args...)  \
{\
memset(gTDMsg,0,TDDebug_MAXSIZE); \
sprintf(gTDMsg,msg,##args); \
TDDebugFunction(__FILE__,__LINE__,(gTDMsg)); \
}
//end..

#endif		/*TDM_SI_INCLUDED_FILE_H*/

