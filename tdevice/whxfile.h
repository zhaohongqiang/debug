#ifndef WHX_FILE_H
#define WHX_FILE_H
#include "tdevice.h"
#ifdef XP_UNIX
#include <pthread.h>
#include "semaphore.h"
#endif

/*wparam of async message*/
#define TD_ASYNC_ERROR		0x0
#define TD_ASYNC_READY		0x1
#define TD_ASYNC_OVER		0x2

/*lparam of async message*/
typedef struct _tagasynccontent{
	TDINT	mlength;
	TDPBYTE mBuf;
} TDAsyncContent,*TDPAsyncContent;

typedef struct _tdasyncfile{

	pthread_t mThread;
	void* mSem;

	TDBOOL		mbOver;
	TDPString	filename;
	TDPVOID		mClient;
	TDPVOID		parameter;
}TDAsyncFile,*TDPAsyncFile;

#define VIDEOCMD_PLAY	0x01
#define VIDEOCMD_JUMP	0x02
#define VIDEOCMD_TIME	0x03

#define INVALID_VIDEO_TYPE		0x0
#define MPEG1_VIDEO_TYPE		0x1
#define VOB_VIDEO_TYPE			0x2	
#define MPEGAUDIO_VIDEO_TYPE	0x3	
#define MPG2_VIDEO_TYPE			0x4	

#define SOUND_AC3               0x5
#define SOUND_MPEG_2CH          0x6
#define SVCD_VIDEO_TYPE			0x7
#define MP3_TYPE				0x8
#define VCD_VIDEO_TYPE			0x9
	


typedef struct tagtdvideocommand{
	TDINT mCmdType;
	TDINT mCmdContent;
}TDVideoCommand,*TDPVideoCommand;


extern TDBOOL gVideoCmdExist;
extern TDVideoCommand gVideoCmd;


#define SECTION_LENGTH	2048
#define SECTION_SHIFT	11
#define RECORD_SIZE		64
typedef struct TDFileRecord{
	TDINT	flag; /*the 0 bit indicate if this is a dir or file*/
	TDINT	start;	/*start sect*/
	TDINT	length; /*file length or number of the dir's entrys*/
	TDCHAR	name[32];
}TDFileRecord,*TDPFileRecord;

#ifdef		TDM_SI_CPLUSPLUS
extern	"C"{
#endif		/*TDM_SI_CPLUSPLUS*/
#define  TDM_TDEVICEAPI
TDM_TDEVICEAPI	TDBOOL		TDFileInit(TDPVOID start);
TDM_TDEVICEAPI	TDBOOL		TDFileDestroy();
TDM_TDEVICEAPI	TDBOOL		TDFileLoadSection(TDPVOID pBuffer,TDINT start,TDINT num);
TDM_TDEVICEAPI	TDHANDLE		TDFileOpen(TDCHAR * filename);
TDM_TDEVICEAPI	TDBOOL		TDFileExist(TDCHAR* filename);
TDM_TDEVICEAPI	TDVOID		TDFileClose(TDHANDLE hFile);
TDM_TDEVICEAPI	TDDWORD		TDFileSeek(TDHANDLE hFile,TDINT offset,TDINT seekmode);
TDM_TDEVICEAPI	TDDWORD		TDFileLength(TDHANDLE hFile);
TDM_TDEVICEAPI	TDBOOL		TDFileRead(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes);
TDM_TDEVICEAPI	TDBOOL		TDFileReadAny(TDHANDLE hFile,TDPVOID pBuf,TDDWORD BytesToRead,TDPDWORD ReadBytes);
TDM_TDEVICEAPI	TDBOOL		TDFileIsHTML(TDHANDLE handle);
TDM_TDEVICEAPI	TDPAsyncFile		TDAsyncFileReceive(TDCHAR* filename,TDPVOID client);
TDM_TDEVICEAPI	TDVOID		TDFileSetCommand(TDINT cmdtype,TDINT cmdcontent);
TDM_TDEVICEAPI	TDINT		TDFileGetPlayMode(TDHANDLE hfile);
TDM_TDEVICEAPI	TDBOOL		TDFileSendCommand(TDHANDLE hFile);
TDM_TDEVICEAPI	TDINT		TDFileGetVideoType(TDHANDLE hFile);
TDM_TDEVICEAPI	TDBOOL		TDFileIsAbsPath(TDPString pPath);
TDM_TDEVICEAPI	TDBOOL		TDFileMakeSureAbsPath(TDPString pDirect,TDPString pPath);
TDM_TDEVICEAPI	TDBOOL		TDFileMakePathDir(TDPString pPath);
TDM_TDEVICEAPI	TDPString	TDFileGetName(TDHANDLE hFile);

#ifdef XP_UNIX
#if 0
#include "semaphore.h"
TDM_TDEVICEAPI	TDBOOL    SetEvent(Semaphore s);
TDM_TDEVICEAPI	TDINT     CloseHandle(void *handle);
TDM_TDEVICEAPI	TDINT     WaitForSingleObject(Semaphore *s,TDINT num);
#endif


#include <errno.h>

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>


#define CLIENT_INPUT 123

#define   WAIT_OBJECT_0   0
#define   WAIT_TIMEOUT    1

struct _EVENT
{
	int ref;	
	pthread_mutex_t ref_mutex;
	pthread_mutex_t mutex;
	pthread_cond_t cond;	
};

typedef struct _EVENT EVENT;
typedef struct _EVENT* PEVENT;


TDM_TDEVICEAPI void* CreateEvent();
TDM_TDEVICEAPI int SetEvent(void* event);
TDM_TDEVICEAPI int WaitForSingleObject(void* event, int timeout);
TDM_TDEVICEAPI void CloseHandle(void* event);

#endif

#ifdef		TDM_SI_CPLUSPLUS
}
#endif		/*TDM_SI_CPLUSPLUS*/
#endif

