//////////////////////////////////////////////////////////
//
// File Name: thundermouse_7413.c
//
// Description:
// 3D	
// ______________________________________________________
//
// Create by: scm
// Create Date:2010-07-28
//
/////////////////////////////////////////////////////////
#include <signal.h>

#include "../global/global.h"
#include "ModuleDef.h"
#include "../ktv/ktv.h"

#include "../windows/th_define.h"
#include "../tdevice/tdevice.h"
#include "../PY_Input/PYmain.h"
#include "CommFunction.h"
#include "TopMenu.h"
#include "ViewAlbum.h"
#include "ViewVIP.h"
#include "ViewGlobes.h"
#include "ViewDiscount.h"
#include "sp_def.h"
#include "sp_g2d_api.h"
#include "sp_fb_api.h"
#include "sp_mem_api.h"
#include "StreamPlay.h"
#include "ViewSearchResult.h"

//#include "ViewShowAlbum.h"
TDVOID 	TDKTVCallback(TDVOID* pvoid);
TDVOID 	TDKTVCommandCallback(TDVOID* pnull,TDCHAR* command,char* page);
static 	TDVOID MessageHInit();
static 	void *MessageHThread(void *pNULL);
static 	void *DrawMessageHThread(void *pNULL);

int TDDoRecordPlay(BYTE btMediaGroup,TDDWORD dwMediaID);

void ShowMem();
void TDMessageBox(char* strMsg);
void TDSecondaryMessageBox(char* strMsg);

static 	TDBOOL		gbIsRevCallBackMsg = FALSE;

//showvgapic thread
static	DWORD		gdwPicShowTime = 0;
static 	TDBOOL		nDoublePause = TDFALSE;
static 	int		nDouble = 0;
static 	int		gNoChange = 0;
static	void 		RedrawWindow();
static 	TDDWORD		dwTCount = 0;
static	pthread_t 	gpDoubleHandle = NULL;
static 	pthread_t	gpSpellHandle = NULL;
TDINT   ShowVGAPic();
static 	TDLONG		glTPState = 0x8000;
static pthread_mutex_t 		gsViewIdleMutex;
static	TDDWORD		TimePrompt(TDINT iLeftTime, TDINT iTPState);
static TDDWORD  gnStaffTime = 0;
static  long long dwCardTickCount = 0;

//showvgapic thread

//fang yuehan add
static	TDBOOL		gbShowLoaded = FALSE;
static 	void		ShowPic();
static 	void		LoadFirstPic();
static  void 		SetPicShowStatus(TDBOOL bPicShowStatus);
static	void		GetPicData();
static  void        SwitchVideo();
static	TDPImage	pPicImage;
static	PBITMAP		NewBMP = NULL;
static	HCURSOR	    ghCursor[2];
//..

static DWORD ConnectThread()
{
	int whxktverror = CONNECT_OK_STATUS;

	whxktverror = ConnectServerS();
#if 0//pengzhongkai for test
	unsigned long nTime1 = 0;
	unsigned long nTime2 = 0;
//	int nThreadExit = 0;
	TDKtvNext1();

//	sleep(1);
	
	while(1)
	{
//		nThreadExit = 1;
		TDKtvAudioSwitch(NULL);//TDKtvNext();//TDKtvPause();//TD_AudioMute(0);//TDKtvReplay(0);//TDKtvAudioSwitch(NULL);//TDKtvPause();//TDKtvNext();  //TD_AudioMute(0);
//		sleep(1);

		nTime1 = GetMyTickCount();
		while(1)
		{
			//printf("wait nThreadExit ======================== 0\n");
//			usleep(200);
			nTime2 = GetMyTickCount();
			if(nTime2 - nTime1 > 10)
			{
				TDDebug("120s time out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				break;
				//nTime1 = GetMyTickCount();
			}
		}
	}
#endif
	
	switch(whxktverror)
	{
	case CONNECT_OK_STATUS:
//		printf("======================loadconfig==========\n");
		if (!LoadingConfig())
		{
			break;
		}
//#ifdef WILLDO
		GetPicData();
//#endif
		if (!gbLoginPhoto)
		{
			dwTCount = GetMyTickCount();
			if(gBoxType == MAIN_BOX)
			{
				//pthread_create(&gpDoubleHandle, NULL, ShowVGAPic, TDNULL);//delete by  zhq  12.26
			}
		}
	//	gbIsRevMsg = TRUE;
		gbIsRevCallBackMsg = TRUE;
		if(bShowCursor == TRUE)
		{
			TDDebug("before set showCursor\n");
			ShowCursor(TDTRUE);
			TDDebug("after set showCursor\n");
		}
		if(gBoxType == MAIN_BOX)
			CreateMainBoxListenThread();
		break;

	case AUTHORIZE_INVALID_STATUS:
		//FastFillRect(ghDC, &rect, 0);
		gbKTVError = STB_LCN_INVALID;
		ShowError(STB_LCN_INVALID);
		break;

	case AUTHORIZE_ERROR_STATUS:
		//FastFillRect(ghDC, &rect, 0);
		gbKTVError = STB_LCN_ERROR;
		ShowError(STB_LCN_ERROR);
		break;

	case NAME_ERROR_STATUS:
		//FastFillRect(ghDC, &rect, 0);
		gbKTVError = USER_NAME_ERROR;
		ShowError(USER_NAME_ERROR);
		break;
	case DOG_LECENSE_ERROR:
		break;
//	case CONNECT_ERROR_STATUS:
//	case NETWORK_ERROR_STATUS:
//	case SETUP_REFUSE_STATUS:
	default:
		//FastFillRect(ghDC, &rect, 0);
		gbKTVError = CONNECT_ERROR;
		ShowError(CONNECT_ERROR);
		break;
	}
}
#if 0//pengzhongkai for debug
#include <signal.h>
void dump(int signo)
{
    void *array[30];
    size_t size;
    char **strings;
    size_t i;

	char tmpchar[215];
 
    size = backtrace (array, 30);
    strings = backtrace_symbols (array, size);
 
	TDDebug("dumpppppppp\n");
    //fprintf (stderr,"Obtained %zd stack frames.nm", size);
	printf ("Obtained %d stack frames.\n", size);
	sprintf(tmpchar, "Obtained %d stack frames.\n", size);
	TDDebug(tmpchar);
 
    for (i = 0; i < size; i++)
	{
        //fprintf (stderr,"%sn", strings[i]);
		printf ("%s\n", strings[i]);

		sprintf(tmpchar, "%s\n", strings[i]);
		TDDebug(tmpchar);
	}
 
    free (strings);
 
    exit(0);
}
#endif
int  main(int argc, char *argv[])
{
//	signal(SIGSEGV, &dump); //pengzhongkai for debug
	TDPString 	pInfoString = NULL;
	TDPString 	pUseVipCard = NULL;
	TDPString	pCardType = NULL;

	long long dwConnectCount = 0;
	int nGetTouch = 0;
	long long nowtime = 0;
	//MSG		Msg;
	int  has_message;
	int  has_tpinput;
	int  touched     = 0;
	int  touched_old = 0;
	int  rawx        = 0;
	int  rawy        = 0;
	int  x           = 0;
	int  y           = 0;
/*	int	 i			 = 0;
	int	nFlag = 0;*/
	uint  tpmsg;
	WPARAM  tpwparam;
	LPARAM  tplparam;
	int		nMaxVol = 0;
	char czTmp[256];

	pInfoString = TDGetNetworkInfo("debugenable");
	if(pInfoString)
	{
		gDebugEnable = atoi(pInfoString->mStr);
		TDString_Destroy(pInfoString);
	}

	TDDebug("------ Thunder stone's process is RUNNING !!! ------\r\n");

	pInfoString = TDGetNetworkInfo("karaokviewconfig");
	if(pInfoString)
	{
		gnGetKaraokViewConfig = atoi(pInfoString->mStr);
		TDString_Destroy(pInfoString);
	} 
	sprintf(czTmp, "gnGetKaraokViewConfig = %d\n", gnGetKaraokViewConfig);
	TDDebug(czTmp);

	pInfoString = TDGetNetworkInfo("notcheckpanel");
	if(pInfoString)
	{
		gNotCheckPanel = atoi(pInfoString->mStr);
		TDString_Destroy(pInfoString);
	} 	
	
	pInfoString = TDGetNetworkInfo("disabledisco");
	if(pInfoString)
	{
		gnDisableDisco = atoi(pInfoString->mStr);
		TDString_Destroy(pInfoString);
	}

	TDHDC_InitCriticalDrawString();

	TH_Init();

	//ShowMem_beiyang();
	//Show2DMem();

	ghWindowDC = GetDC(NULL);
	//ghWindowDC = GetWindowDC(NULL);
	ghDC = GetWindowDC(NULL);

	ghOSDDC = CreateOsdDC(NULL);

#if 0
	ghAlbumDC = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);
	SetBrushColor(ghAlbumDC, 0x00000000);
   	FillBox(ghAlbumDC, 0, 0, SM_XSCREEN, SM_YSCREEN);
#endif

//yang add for theme
	ghThemeDC = CreateCompatibleDCEx(ghDC, 128, 128);
	SetBrushColor(ghThemeDC, 0x00000000);
   	FillBox(ghThemeDC, 0, 0, 128, 128);
//..

	ghScrMemDC = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);
	SetBrushColor(ghScrMemDC, 0x00000000);
   	FillBox(ghScrMemDC, 0, 0, SM_XSCREEN, SM_YSCREEN);

	ghScrMemDC1 = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);
	SetBrushColor(ghScrMemDC1, 0x00000000);
   	FillBox(ghScrMemDC1, 0, 0, SM_XSCREEN, SM_YSCREEN);

	ghScrMemDC2 = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);
	SetBrushColor(ghScrMemDC2, 0x00000000);
   	FillBox(ghScrMemDC2, 0, 0, SM_XSCREEN, SM_YSCREEN);

	ghScrMemDC3 = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);

	ghScrMemDC4 = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);

	ghScrMemDC5 = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);

	ghScrMemDC6 = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);
	SetBrushColor(ghScrMemDC6, 0x00000000);
   	FillBox(ghScrMemDC6, 0, 0, SM_XSCREEN, SM_YSCREEN);

	ghTmpDC = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);

	ghScrMemBackgroundDC = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);
	SetBrushColor(ghScrMemBackgroundDC, 0xFF000000);
   	FillBox(ghScrMemBackgroundDC, 0, 0, SM_XSCREEN, SM_YSCREEN);

	ghScrMemBackgroundTmpDC = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);
	SetBrushColor(ghScrMemBackgroundTmpDC, 0xFF000000);
   	FillBox(ghScrMemBackgroundTmpDC, 0, 0, SM_XSCREEN, SM_YSCREEN);

	ghScrWithOutBGMemDC = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);
	SetBrushColor(ghScrWithOutBGMemDC, 0xFF000000);
   	FillBox(ghScrWithOutBGMemDC, 0, 0, SM_XSCREEN, SM_YSCREEN);
	
	ghNewSongViewMemDC = CreateCompatibleDCEx(ghDC, SM_XCLIENTVIEW, SM_YCLIENTVIEW);

	ghTmpWordShowMemDC = CreateCompatibleDCEx(ghDC, 100, 40);   //pengzhongkai

	ghScrMemBlackDC = CreateCompatibleDCEx(ghDC, BLACKDC_WIDTH, BLACKDC_HEIGHT);
	SetBrushColor(ghScrMemBlackDC, 0xFF000000);
	FillBox(ghScrMemBlackDC, 0, 0, BLACKDC_WIDTH, BLACKDC_HEIGHT);

	ghScrMemRecordBgDC3D = CreateCompatibleDCEx(ghDC, 512, 512);
	SetBrushColor(ghScrMemRecordBgDC3D, 0x000000ff);
   	FillBox(ghScrMemRecordBgDC3D, 0, 0, 512, 512);

	ghScrClientViewMemDC = CreateCompatibleDCEx(ghDC, SM_XSCREEN, SM_YSCREEN);

#if 0
	{
			FILE		*file = NULL;
			BMPHEAD			bmp;
			char 		*pdata;
			char 	szFileName[128];
			BITMAP bitmap;
			//sprintf(szFileName, "./jiemian.bmp");
			//sprintf(szFileName, "./jiemian0728.bmp");
			//sprintf(szFileName, "./001.bmp");
			sprintf(szFileName, "./jiemian4.bmp");
			file = fopen(szFileName, "r");
			if(file)
			{
				fread(&bmp, 54,1,file);

				pdata = malloc(1280*720*4);
				memset(pdata,0,1280*720*4);
				int i = 0;
				for(i = 1; i <= 720; i++)
				{
					fread(pdata + (720 - i)*1280*4, 1280*4, 1, file);
				}
		//		free(pdata);
				fclose(file);
				memset(&bitmap,0, sizeof(BITMAP));
				bitmap.bmBits 		= pdata;
				bitmap.bmType		= BMP_TYPE_NORMAL;
				bitmap.bmBitsPerPixel 	= 32;
				bitmap.bmBytesPerPixel 	= 4;
				bitmap.bmAlpha         	= 0;
				bitmap.bmColorKey      	= 0;
				bitmap.bmHeight		= 720;
				bitmap.bmWidth		= 1280;
				bitmap.bmPitch        	= bitmap.bmWidth*4;
				bitmap.bmAlphaPixelFormat = NULL;

				TDFillBoxWithBitmap(ghScrMemDC,
						0,
						0,
						1280,
						720,
						&bitmap,
						NULL);

				TDBitBltEx(ghWindowDC,
						0,
						0,
						1280,
						720,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
						ghScrMemDC,
						0,
						0,
						1280,
						720,
						ghScrMemDC,
						0,
						0,
						1280,
						720,
						0x0,
						0xFF);
			}
	}
	sleep(1000);
#endif

	pthread_mutex_init(&gAlbumDestoryMutex, NULL);
	pthread_mutex_init(&gKaraokThreadMutex, NULL);

	Recv3DPicDelayBegin1Event = CreateEvent();
	Recv3DPicDelayBegin2Event = CreateEvent();

	pthread_mutex_init(&gActorMutex, NULL);
	pthread_mutex_init(&gAllActorMutex1, NULL);
	pthread_mutex_init(&gAllActorMutex2, NULL);

	pthread_mutex_init(&gUpdateActorPicMutexEnd, NULL);

	ActorUpdatePicEvent = CreateEvent();
	ActorUpdatePicEvent1 = CreateEvent();

	pthread_mutex_init(&gUpdateNewsongMutex, NULL);   //add by huanguo 20110420

//////////////////////////////////////////////////////////////////////////

	pInfoString = TDGetNetworkInfo("maxvol");
	if(pInfoString)
	{
		if(!strcmp(pInfoString->mStr,"yes"))
		{
			nMaxVol = 1;
		}
		TDString_Destroy(pInfoString);
	}
	TDSetMaxVol(nMaxVol);

	pInfoString = TDGetNetworkInfo("initvol");
	if(pInfoString)
	{
		int nTmpVol = 0;
		nTmpVol = atoi(pInfoString->mStr);

		if(nTmpVol > 0)
		{
			if(nTmpVol > 99)
			{
				nTmpVol = 99;
			}
			else if(nTmpVol < 51)
			{
//				nTmpVol = 51;
			}
			gInitVol = nTmpVol;
		}
		gInitVol = (gInitVol - (gInitVol % 3));
		TDString_Destroy(pInfoString);
	}
//Disable BroadCase msl
	pInfoString = TDGetNetworkInfo("UseBroadcast");
	if(pInfoString)
	{
		gbCanBroadcast = atoi(pInfoString->mStr);
		TDString_Destroy(pInfoString);
	}

	//yang fot test video
	pInfoString = TDGetNetworkInfo("TestVideo");
	if(pInfoString)
	{
		gbCanTestVideo = atoi(pInfoString->mStr);
		TDString_Destroy(pInfoString);
	}
	//..

	//yang for dalei
	pInfoString = TDGetNetworkInfo("UseDaLei");
	if(pInfoString)
	{
		//printf("支持大类\n");
		gbUseDaLei = atoi(pInfoString->mStr);
		//printf("gbUseDaLei = %d\n",gbUseDaLei);
		TDString_Destroy(pInfoString);
	}
	//..

	TD_VideoInit();

//	pHandWriteUseTPanel = TDGetNetworkInfo("HandWriteUseTPanel");
//	if(pHandWriteUseTPanel)
//	{
//		if(atoi(pHandWriteUseTPanel->mStr) == 1)
//		{
//			TDDebug("HandWriteUseTPanel == 1");
//			HandWriteUseTPanel(1);
//		}
//		TDString_Destroy(pHandWriteUseTPanel);
//	}
	
	gIsExistTVBox = FALSE;
	gnCurTVChannel = 0;
	gbTVStatus = 0x16;
	TDDebug("tv is not exit");
	
	pUseVipCard = TDGetNetworkInfo("usevipcard");
	if(pUseVipCard)
	{
		if(atoi(pUseVipCard->mStr) == 1)
		{
			gnUseVipCard = 1;
			TDDebug("gnUseVipCard == 1");
		}
		TDString_Destroy(pUseVipCard);
	}

	pInfoString = TDGetNetworkInfo("usecard");
	if(pInfoString)
	{
		TDDebug(pInfoString->mStr);
		if(!strcmp(pInfoString->mStr,"Key_Port"))
		{
			gnUseCard = 1;
			TDDebug("gnUseCard == 1"); //键盘口
		}
		else if(!strcmp(pInfoString->mStr,"Seaial_Port"))
		{
			if(gIsExistTVBox)
			{
				gnUseCard = 0;
				TDDebug("gnUseCard == 2   but have TVbox");

			}
			else
			{
				gnUseCard = 2;
				TDDebug("gnUseCard == 2"); //串口
			}
		}
		else
		{
			gnUseCard = 0;
			TDDebug("gnUseCard == 0"); //未设置刷卡
		}
	}
	TDString_Destroy(pInfoString);

	pCardType = TDGetNetworkInfo("cardtype");
	if(pCardType)
	{
		TDDebug(pCardType->mStr);
		if(!strcmp(pCardType->mStr,"ci_tiao"))
		{
			gnCardType = 1;
			TDDebug("ci_tiao 111"); //磁条
		}
		else if(!strcmp(pCardType->mStr,"gan_ying"))
		{
			gnCardType = 0;
			TDDebug("gan_ying 222"); //感应
		}
		else
		{
			gnCardType = 0;
			TDDebug("cardtype 000"); //未设置刷卡
		}
	}
	TDString_Destroy(pCardType);
	pInfoString = TDGetNetworkInfo("NoAudioMix");
	if(pInfoString && (atoi(pInfoString->mStr) == 1))
	{
		bAudioMix = TDFALSE;
		TDDebug("bAudioMix == FALSE\n");
	}
	TDString_Destroy(pInfoString);

	pInfoString = TDGetNetworkInfo("DanRuAudio");
	if(pInfoString && (atoi(pInfoString->mStr) == 1))
	{
		bDanRuAudio = TRUE;
		TDDebug("bAudioMix == FALSE");
	}
	TDString_Destroy(pInfoString);


//////////////////////////////////////////////////////////////////////////
	TDSocketInit();

	if (!GetOwnIP())
	{
		return FALSE;
	}

	TDKtvInitial();
	TDKtvSetCallback(TDKTVCallback,0);
	TDKtvSetCommandCallback(TDKTVCommandCallback,(char *)NULL);

	signal(SIGPIPE, SIG_IGN);
//////////////////////////////////////////////////////////////////////////
//	TH_SetTopTVOut(1);
//	TH_SetBottomTVOut(1);
//	TH_SetSpeakerOut(1);
//	TH_SetHeadphoneOut(1);
//////////////////////////////////////////////////////////////////////////
	ConnectThread();

	TH_CleanOSDStart();
	TDDebug("begin ktvnext\n");
//	TH_MpegDecoderSetAudioOutput(0);
	nVideoOutPut = 0;
	
//	pthread_create(&gpSpellHandle,NULL,main_PY,TDNULL);


	// KGy TouchPanel >
	// Things about the new message loop:
	// First there are two message sources now,
	//   So we use non-blocking mode to get message.
	// There are two pointing devices but we don't
	//   have to combine their output together, they
	//   simply work separately and the GUI will
	//   process all the messages they generate.
	// There shall not be any priority between the two
	//   message sources, so we will process only one
	//   message for each source in one loop. If we don't
	//   do this, there can be messages that can not get
	//   processed in time due to message torrent from
	//   the source with higher priority.
	// And we will keep processing message until there
	//   are no message from both sources, then do a
	//   short sleep to release CPU.

	MessageHInit();	////
//	ShowMem();
//	ShowSurfaceMem();

	dwConnectCount = GetMyTickCount(); // - long long dwConnectCount;

	while (1)
	{
		has_message = 0;
	  	has_tpinput = 0;
		// We depend on the fail of TPRead() to check if we have touchpanel.
		has_tpinput = TPRead(&touched,&rawx,&rawy);
		if(nGetTouch == 0)
		{
			nowtime =  GetMyTickCount() - dwConnectCount;
			if(nowtime >= 1)
				nGetTouch = 1;
		}
		else if (has_tpinput)
		{
			TPCorrect(rawx, rawy, &x, &y);

			//sprintf(tpinfo,"TP: rawx: %d, rawy: %d, x: %d, y: %d, touched: %d.\n",
			//    rawx, rawy, x, y, touched);
			//TDDebug(tpinfo);

			// Now build the message.
			// ToDo: Do I have to make a mouse move message before the mouse down?

			if (touched == 0)
			{
				// Due to device design, this is always up.
				tpmsg    = MSG_LBUTTONUP;
				tpwparam = 0;
			}
			else if ((touched_old == 0) && (touched ==1))
			{
				// Then this is down.
				tpmsg    = MSG_LBUTTONDOWN;
				tpwparam = 0;//MK_LBUTTON;
			}
			else
			{
				// Now touched_old and touched are both true, move.
				// We will generate mouse move message no matter
				//   whether the position changed or not. Then touch
				//   panel can rob focus from mouse.
				tpmsg = MSG_MOUSEMOVE;
				tpwparam = 0;//MK_LBUTTON;
				//continue;
			}

			if(tpmsg == MSG_LBUTTONUP)
			{
				tplparam = (x & 0xFFFF) | ((y & 0xFFFF) <<16);
				tpwparam = GetTickCount();

				touched_old = touched;
				TDDebug("TOUCH MSG_LBUTTONUP");

#ifdef	STANDARDVER
				Msg.hwnd    = NULL;//hMainWnd;
				Msg.message = tpmsg;
				Msg.wParam  = tpwparam;
				Msg.lParam  = tplparam;

				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
#else
				TH_SendNotifyMessage(NULL, tpmsg, tpwparam, tplparam);
#endif
			}
			else if(tpmsg == MSG_MOUSEMOVE)
			{
				tplparam = (x & 0xFFFF) | ((y & 0xFFFF) <<16);
				tpwparam = GetTickCount();
#ifdef	STANDARDVER
				Msg.hwnd    = NULL;
				Msg.message = tpmsg;
				Msg.wParam  = tpwparam;
				Msg.lParam  = tplparam;
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
#else
				TH_SendNotifyMessage(NULL, tpmsg, tpwparam, tplparam);
#endif
			}
			else if(tpmsg == MSG_LBUTTONDOWN)
			{
				touched_old = touched;
				tplparam = (x & 0xFFFF) | ((y & 0xFFFF) <<16);
				tpwparam = GetTickCount();
				TDDebug("TOUCH MSG_LBUTTONDOWN");
#ifdef	STANDARDVER
				Msg.hwnd    = NULL;
				Msg.message = tpmsg;
				Msg.wParam  = tpwparam;
				Msg.lParam  = tplparam;

				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
#else
				TH_SendNotifyMessage(NULL, tpmsg, tpwparam, tplparam);
#endif
			}
	  	}

		if(!has_tpinput)
		{
			usleep(20 * 1000);
		}

#ifdef	STANDARDVER
		Msg.message = MSG_NULLMSG;
#endif

#ifdef		STANDARDVER
		// A non-blocking GetMessage.
		has_message = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
		if (has_message)
		{
		/*	if (Msg.message == MSG_QUIT)
			{
				break;
			}
			else if(Msg.message == MSG_IDLE)
			{
				has_message = FALSE;
			}
			else*/
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}

		if(!has_tpinput && !has_message)
		{
			// Don't sleep so that we can redraw cursor quickly enough.
			// CPU usage is not too high. Infact, this implmemtation
			//   is simply the same as using the blocking getmessage,
			//   if the low level MiniGUI design is considered.
	 		// Keep awake if there are any message.
			usleep(30 * 1000);  // Release the CPU.
		}
		/*else
			usleep(10 * 1000);  // Release the CPU.*/
#endif
	}

	return ;//Msg.wParam;
}

static void TransactCardID(char *szCardID);

TDINT   ShowVGAPic()
{
	static DWORD dwTempCount = 0;
	static TDINT nFirst = 0;
#if 0
	do//modify by PLM for ProcessBar.
	{
		gBottomPanel.Idle();
		usleep(100*1000);
	}
	while(1);
	//return 0;
#else
	if (gbLoginPhoto)
		dwTCount = GetMyTickCount();
	do
	{
		if(gNoChange)
		{
			dwTCount = GetMyTickCount();
			usleep(1000 * 1000);
			continue;//return 0;
		}

		if(nDoublePause)
		{
			dwTCount = GetMyTickCount();
			usleep(1000 * 1000);
			continue;//return 0;
		}
		if(!gbIsReloadSkinOver)
		{
			dwTCount = GetMyTickCount();
			usleep(1000 * 1000);
			continue;//return 0;
		}
//#ifdef	WILLDO
		if(gnStaffTime >= 1)
		{
			if(GetMyTickCount() - gnStaffTime > 90)	//90 秒后 即 30 + 90 = 120 秒
			{
/*				if(((int)((gpCurClientView->dwModuleID & 0xF0000000) >> 28)) == 0x3) //酒水管理所有页面
				{
					if(strlen(gszUserSerialNO) > 0) //如果当前账号变量存有工号
					{
						SwitchVideo();
						gszUserSerialNO[0] = 0;
						if(gpCurTopMenu)
						{
							gpCurTopMenu->Destroy();
							gpCurTopMenu = NULL;
						}
						ViewHandleLBtnUpShell(0x310);
						TDMessageBox(gczInfo[168]);
					}
				}
				gnStaffTime = 0;*/
//          	SendNotifyMessage(NULL, WM_DRINKMSG_CMD, 0, 0); //add fangyuehan
				usleep(100 * 1000);
			}
		}
//#endif
		if(gnDouble == 3)
		{
			if (gbIsFullScreen == 1)
			{
				dwTCount = GetMyTickCount();
				usleep(100 * 1000);
				continue;
			}
//#ifdef	WILLDO
			if (GetMyTickCount() - dwTCount > gdwPicShowTime && gbShowLoaded && !gUseADEnable)
			{
				if(gpCurTopMenu)
				{
					gpCurTopMenu->Destroy();
					gpCurTopMenu = NULL;
				}
				dwTCount = GetMyTickCount();
				ShowPic();
				LoadFirstPic();
			}
			else
//#endif
			{
				usleep(100 * 1000);
			}
			continue;//return 0;
		}

		//if(GetMyTickCount() - dwTCount > 30 && gpCurClientView != &gClientView[37]&&gnShouldShow3D!=1)
		//if(GetMyTickCount() - dwTCount > 30 && gpCurClientView != &gClientView[37])
		
		if(GetMyTickCount() - dwTCount > 30)
		{
			pthread_mutex_lock(&gDCMutex);
			
			if(gnStaffTime == 0)
			{
				gnStaffTime = GetMyTickCount();
				
			}
			
			if(gbUsePic && !gUseADEnable)
			{
				
				//#ifdef WILLDO
				//printf("Picture !!!!!!!!!!\n");
				
				if (gpCurSecondaryTopMenu)
				{
					gpCurSecondaryTopMenu->Destroy();
					gpCurSecondaryTopMenu = NULL;
				}
				if(gpCurTopMenu)
				{
					if( gpCurTopMenu->dwModuleID == MODULE_TOPMENU_MOVIE || gpCurTopMenu->dwModuleID == MODULE_TOPMENU_PREVIEW || gpCurTopMenu->dwModuleID == MODULE_TOPMENU_MTV || gpCurTopMenu->dwModuleID == MODULE_TOPMENU_TV )//if(gpCurTopMenu->dwModuleID == 0xF2D00000)//preview top menu
					{
						dwTCount = GetMyTickCount();
						pthread_mutex_unlock(&gDCMutex);
						continue;
					}
					else
					{
						gpCurTopMenu->Destroy();
						gpCurTopMenu = NULL;

						gRightTopPanel.Initialize(0,0);
						gRightTopPanel.DrawWindow(0);
						gRightTopPanel.ShowWindow(0,0);
						
						if(gpRightTopPanelSelect)
							gpRightTopPanelSelect->Destroy();
						if(gpGreetingTopPanel)
							gpGreetingTopPanel->Destroy();
					}
				}
				
				//				ShowTV(FALSE);
				//bActorImageIdleInvalid = TRUE;
				
				
//				if(gpCurClientView && gpCurClientView->dwModuleID == 0x10100000)//karaok view
				{
					PauseThreadLock();
				}
				
				gnDouble = 3;//轮播标志
				dwTCount = GetMyTickCount();

				ShowPic();
				LoadFirstPic();
				//#endif
			}
			else
			{
				//				printf("MTV fullscreen !!!!!\n");
				
				if (gpCurSecondaryTopMenu)
				{
					gpCurSecondaryTopMenu->Destroy();
					gpCurSecondaryTopMenu = NULL;
				}
				
				if(gpCurTopMenu)
				{
					if( gpCurTopMenu->dwModuleID == MODULE_TOPMENU_MOVIE || gpCurTopMenu->dwModuleID == MODULE_TOPMENU_PREVIEW || gpCurTopMenu->dwModuleID == MODULE_TOPMENU_MTV || gpCurTopMenu->dwModuleID == MODULE_TOPMENU_TV )//if(gpCurTopMenu->dwModuleID == 0xF2D00000)//preview top menu
					{
						dwTCount = GetMyTickCount();
						pthread_mutex_unlock(&gDCMutex);
						continue;
					}
					else
					{
						gpCurTopMenu->Destroy();
						gpCurTopMenu = NULL;

						gRightTopPanel.Initialize(0,0);
						gRightTopPanel.DrawWindow(0);
						gRightTopPanel.ShowWindow(0,0);
						
						if(gpRightTopPanelSelect)
							gpRightTopPanelSelect->Destroy();
						if(gpGreetingTopPanel)
							gpGreetingTopPanel->Destroy();
					}
				}
				
				//SetScrMemDCwithghDC();
				UpdateScrMemDCAndStaticObject();
				gnDouble = 3;
				dwTCount = GetMyTickCount();
				
				PauseThreadLock();
				
				SetSmallWindowToFullScreen(1);
				/*
				if(gBoxType == MAIN_BOX)
				{
				#if 0
				#if 0
				if(gpCurExpandView && ((gpCurExpandView->dwModuleID & 0xFFF00000)
				!= 0x20200000))
				#else
				if(gpCurTopMenu && (gpCurTopMenu->dwModuleID && 0xFFF00000) == 0xF2500000)
				{
				dwTCount = GetMyTickCount();
				continue;
				}
				
				  #endif
				  {
				  #if 0
				  if((gbTVStatus & 0x9) == 0x8)
				  {
				  dwTCount = GetMyTickCount();
				  continue;
				  //return 0;
				  }
				  #endif
				  gbPicIsShow = 1;
				  gnDouble = 3;
				  dwTCount = GetMyTickCount();
				  
					if(gpCurTopMenu && ((gpCurTopMenu->dwModuleID & 0xFFF00000)
					!= 0x20300000))
					{
					gpCurTopMenu->Destroy();
					gpCurTopMenu = NULL;
					}
					
					  SetMasterWindow(2,
					  2,
					  1280 - 4,
					  720 - 4,
					  1,
					  1);
					  //SetWindowA(0 + 2, 0 + 2, 1280 - 2, 720 - 2);
					  int i = 0;
					  #if 0
					  if(gbCanTestVideo && gpCurExpandView && ((gpCurExpandView->dwModuleID & 0xFFF00000) != 0x20200000))
					  {
					  TDKtvTestNext(1);
					  }
					  #endif
					  for(i = 0; i <= 25; i++)
					  {
					  if(gnDouble == 3)
					  {
					  
						TDBitBltEx(ghDC,
						0,
						0,
						1280,
						720,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
						ghDC,
						0,
						0,
						1280,
						720,
						ghDC,
						0,
						0,
						1280,
						720,
						0x0,
						255 - i * 10 - 5);
						
						  usleep(70 * 1000);
						  }
						  else
						  break;
						  }
						  }
						  #endif
						  #if 0
						  else
						  {
						  dwTCount = GetMyTickCount();
						  }
						  #endif
						  }
				*/
				
			}
			pthread_mutex_unlock(&gDCMutex);
		}
		else
		{
			if(gnDouble == 3 || gbPreviewIsMove)
			{
				usleep(100*1000);
				continue;
			}
//			printf("will do gBottomPanel.Idle.\n");
#if 0 //add fangyuehan
#ifdef	WILLDO
			gBottomPanel.Idle();
			

			if(gpCurTopMenu == &gTopMenu[28] && gpCurTopMenu->Idle != NULL)
			{
				if(!gbPreviewIsMove)
					gpCurTopMenu->Idle();
			}
			//if(/*gpCurClientView == &gClientView[CLIENTVIEW_MYLIB] || */gpCurClientView == &gClientView[CLIENTVIEW_ACTORSEL] || gpCurClientView == &gClientView[37] ||gnShouldShow3D || gpCurTopMenu == &gTopMenu[TOPMENUVIEW_RANK])
			if(/*gpCurClientView == &gClientView[CLIENTVIEW_MYLIB] || */gpCurClientView == &gClientView[CLIENTVIEW_ACTORSEL] || gpCurClientView == &gClientView[37] || gpCurTopMenu == &gTopMenu[TOPMENUVIEW_RANK])
			{
				if((GetMyTickCount() - dwTCount > 6) ||(GetMyTickCount() - dwTCount > 2 && gpCurClientView == &gClientView[37]) )
				{
					if(gpCurTopMenu == NULL || gpCurTopMenu== &gTopMenu[18])
					{
						if(pthread_mutex_trylock(&gDrawDCMutex) != EBUSY)
						{
							nFirst = 1;
							gnDouble = 1;
							pthread_mutex_unlock(&gDrawDCMutex);
						}
						else
						{
							gnDouble = 0;
							nFirst = 0;
							gnStaffTime = 0;
							dwTCount = GetMyTickCount();
						}
					}
				}

				if(gnDouble == 1)
				{
					if(gpCurTopMenu == NULL)
					{
						gpCurClientView->Idle();
					}
					else if(gpCurTopMenu == &gTopMenu[TOPMENUVIEW_RANK])
					{
						gpCurTopMenu->Idle();
					}
					else
					{
						/////////////////////////////////////////////////////////////test
						dwTCount = GetMyTickCount() + 4;
						//////////////////////////////////////////////////////////////
						gnDouble = 0;
						gnStaffTime = 0;
						nFirst = 0;
					}
				}
				else
				{
					nFirst = 0;
				}
			}
#endif
#endif //add fangyuehan
			//usleep(500*1000);
			gBottomPanel.Idle(); //add fangyuehan 
			usleep(100*1000);
		}
#ifdef	WILLDO
		ShowADImage();
#endif
	}
	while(1);
#endif
}

TDINT   SetSmallWindowToFullScreen(int iShowType)
{
	if (gbLoginPhoto)
		dwTCount = GetMyTickCount();

	{
		if(gNoChange)
		{
			dwTCount = GetMyTickCount();
			return 0;
		}

		if(nDoublePause)
		{
			dwTCount = GetMyTickCount();
			return 0;
		}

			{
				if(gBoxType == MAIN_BOX)
				{
						gbPicIsShow = 1;
						gnDouble = 3;
						gbIsFullScreen = 1;
						dwTCount = GetMyTickCount();
/*
						SetMasterWindow(2,
								2,
								1280 - 4,
								720 - 4,
								1,
								1);
		
						SetBrushColor(ghDC, 0x00000000);
						FillBox(ghDC, 0, 0, 1280, 720);
*/
#define   offsetNum  15	
                        int i = 0;					
                        float   x ;
                        float   y ;
                        float   xWidth ;
                        float   yHeight ;
                        float   xOffset ;
                        float   yOffset ;
                        float   WideOffset ;
                        float   HeightOffset ;
						float   nowWide = 0 ;
						float   nowHeight = 0 ;
						if(iShowType ==1)
						{	
							nowWide   = 401.0;
							nowHeight = 304.0;		
							x = 824.0 + 14.0;
							y = 130.0 + 93.0 ;
							xWidth = 1280.0  ;
							yHeight = 720.0  ;
							xOffset = (x - 0) /offsetNum;
							yOffset = (y - 0) /offsetNum ;
							WideOffset = (xWidth - nowWide ) /offsetNum ;
							HeightOffset = (yHeight -nowHeight) /offsetNum ;
						}
						else
						{
							nowWide = 54.0 ;//60.0;
							nowHeight = 33.0 ;//39.0;		
							x = 428;//434.0 ;//
							y = 21.0;//27.0 ;
							xWidth = 1280.0  ;
							yHeight = 720.0 ;
							xOffset = (x - 0) /offsetNum;
							yOffset = (y - 0) /offsetNum ;
							WideOffset = (xWidth - nowWide) /offsetNum ;
							HeightOffset = (yHeight -nowHeight)/offsetNum ;
						}
						
                        SetMasterWindow(
                                (int)(x - xOffset * offsetNum),
                                (int)(y - yOffset * offsetNum),
                                (int)(WideOffset * offsetNum + nowWide),
                                (int)(HeightOffset * offsetNum+ nowHeight),
								0,
								1);
//						printf("FULLsrean !!!!!!!!\n");
                        SetMasterWindow(
                                (int)(x - xOffset * offsetNum),
                                (int)(y - yOffset * offsetNum),
                                (int)(WideOffset * offsetNum + nowWide),
                                (int)(HeightOffset * offsetNum+ nowHeight),
								1,
								1);
						
						TDBitBltEx(ghDC,
                                0,
                                0,
                                1280,
                                720,
                                BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
                                ghDC,
                                0,
								0,
								1280,
								720,
								ghScrMemBackgroundDC,
								0,
								0,
								1280,
								720,
                                0x0,
                                0x0
                                ); 
				}
			}
	}
	return 1;
}

void SwitchVideo()
{
	if(gnDouble == 3)
	{
		dwTCount = GetMyTickCount();
		usleep(10);
	
		RedrawWindow();
		
		gnStaffTime = 0;
		gnDouble = 0;
		nDouble = 0;
	}
	else
	{
		dwTCount = GetMyTickCount();
		gnDouble = 0;
		gnStaffTime = 0;
	}
}

#if 0
int WndProcShell(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	char			szTmpTxt[128];

	switch (message)
	{
	case MSG_PAINT:
		break;
	case MSG_RBUTTONUP:
		break;
	case MSG_KEYUP:
	//case TH_MSG_KEYDOWN:
		if(wParam == 0)
			break;
		gnStaffTime = 0;
		if(lParam != 0x8000)
		{
			sprintf(szTmpTxt, "wParam = 0x%x\n", wParam);
			TDDebug(szTmpTxt);
			if(wParam == 0x5)
				wParam = 0x70;    //call waiter
			else if(wParam == 0x29)
				wParam = 0x20;    //vol -
			else if(wParam == 0x66)
				wParam = 0x08;    //vol +
			else if(wParam == 0x4e)
				wParam = 0xbd;    //org/accpy
			else if(wParam == 0x4a)   //home ??
			{
				//wParam = 0x6d;
				wParam = 0x6F;
			}
			else if(wParam == 0xa)    //yiyuan
				wParam = 0x09;
			else if(wParam == 0x4c)   //mute
				wParam = 0xba;
			else if(wParam == 0x52)   //replay
				wParam = 0xde;
			else if(wParam == 0x3d)
				wParam = 0x37;
			else if(wParam == 0x3e)
				wParam = 0x38;
			else if(wParam == 0x46)
				wParam = 0x39;
			else if(wParam == 0x78)
				wParam = 0x7a;    //page up
			else if(wParam == 0x25)
				wParam = 0x34;
			else if(wParam == 0x2e)
				wParam = 0x35;
			else if(wParam == 0x36)
				wParam = 0x36;
			else if(wParam == 0x7)
				wParam = 0x7b;   //page down
			else if(wParam == 0x16)
				wParam = 0x31;
			else if(wParam == 0x1e)
				wParam = 0x32;
			else if(wParam == 0x26)
				wParam = 0x33;
			else if(wParam == 0x5d)
				wParam = 0xdc;    //next
			else if(wParam == 0x45)
				wParam = 0x30;
			else if(wParam == 0x76)    //esc
				wParam = 0x1b;
			else if(wParam == 0x5a)
				wParam = 0xd;      //enter
			else if(wParam == 0x41)
				wParam = 0xbc;      //film
			else if(wParam == 0x4e)
				wParam = 0xbe;      //wine
			else if(wParam == 0x4a)
				wParam = 0xbf;      //yule
			else if(wParam == 0x06)
				wParam = 0x71;      //change screen
			else if(wParam == 0x4)
				wParam = 0x72;      //geyin
			else if(wParam == 0xc)
				wParam = 0x73;      //py
			else if(wParam == 0x3)
				wParam = 0x74;      //zs
			else if(wParam == 0xb)
				wParam = 0x75;      //ba
			else if(wParam == 0x83)
				wParam = 0x76;      //yy
			else if(wParam == 0xa)
				wParam = 0x77;      //qz
			else if(wParam == 0x1)
				wParam = 0x78;      //ph
			else if(wParam == 0x9)
				wParam = 0x79;      //gk
			else if(wParam == 0xe)
				wParam = 0xC0;      //delete
			else if(wParam == 0x55)
				wParam = 0xbb;      //play
			/*else if(wParam == 0x71)
				wParam = 0x2E;      //biaozhundiao   delete*/
			else if(wParam == 0x75)
				wParam = 0x26;      //mic inc     top arrow
			else if(wParam == 0x72)
				wParam = 0x28;      //mic dec     bottom arrow
			else if(wParam == 0x6b)
				wParam = 0x25;      //tune inc    left arrow
			else if(wParam == 0x74)
				wParam = 0x27;      //tune dec    right arrow
			else if(wParam == 0x7c)
				wParam = 0x6A;      //dao chang   little key *
			else if(wParam == 0x71)
				wParam = 0xFC;      //dao chang   little key del
			else if(wParam == 0x1c)     //A -Z
				wParam = 0x61;
			else if(wParam == 0x32)
				wParam = 0x62;
			else if(wParam == 0x21)
				wParam = 0x63;
			else if(wParam == 0x23)
				wParam = 0x64;
			else if(wParam == 0x24)
				wParam = 0x65;
			else if(wParam == 0x2b)
				wParam = 0x66;
			else if(wParam == 0x34)
				wParam = 0x67;
			else if(wParam == 0x33)
				wParam = 0x68;
			else if(wParam == 0x43)
				wParam = 0x69;
			else if(wParam == 0x3b)
				wParam = 0x6A;
			else if(wParam == 0x42)
				wParam = 0x6B;
			else if(wParam == 0x4b)
				wParam = 0x6c;
			else if(wParam == 0x3a)
				wParam = 0x6d;
			else if(wParam == 0x31)
				wParam = 0x6e;
			else if(wParam == 0x44)
				wParam = 0x6f;
			else if(wParam == 0x4d)
				wParam = 0x70;
			else if(wParam == 0x15)
				wParam = 0x71;
			else if(wParam == 0x2d)
				wParam = 0x72;
			else if(wParam == 0x1b)
				wParam = 0x73;
			else if(wParam == 0x2c)
				wParam = 0x74;
			else if(wParam == 0x3c)
				wParam = 0x75;
			else if(wParam == 0x2a)
				wParam = 0x76;
			else if(wParam == 0x1d)
				wParam = 0x77;
			else if(wParam == 0x22)
				wParam = 0x78;
			else if(wParam == 0x35)
				wParam = 0x79;
			else if(wParam == 0x1a)
				wParam = 0x7a;
			else if(wParam == 0x7e)
				wParam = 0x20;         //??
			else if(wParam == 0x0E)
				wParam = 0xC0;         //
			else if(wParam == 0x12)
				wParam = 0x2c;         //delete   print screen sys
			else if(wParam == 0x7a)
				wParam = 0x91;         //sound   scroll lock
			else if(wParam == 0x11)        // L alt
				wParam = 0x13;
			else if(wParam == 0x7c)        // *
				wParam = 0x6a;
			else if(wParam == 0x79)        // +
				wParam = 0x6b;
			else if(wParam == 0x70)
				wParam = 0x2D;//褰曢煶
			else if(wParam == 0x69)
				wParam = 0x23;//鍥炴斁
			else if(wParam == 0x77)
				wParam = 0x90;//鍋滄

			if(gnUseCard == 1)
			{//Key Port
				if((wParam >= 0x30 && wParam <= 0x39) || wParam == 0xd)
				{
					if(nCardIndex >= 19 || (GetMyTickCount() - dwCardTickCount > 2))
					{
						nCardIndex = 0;
						memset(czCardID,0,20);
					}
					if(nCardIndex == 0)
						dwCardTickCount = GetMyTickCount();
					czCardID[nCardIndex] = wParam;
					if(wParam == 0xd || nCardIndex == 9)
					{
						if(nCardIndex == 8 || nCardIndex == 9)
						{
							if((GetMyTickCount() - dwCardTickCount) <= 2)  //msl
							{
								if(nCardIndex == 9)
									czCardID[nCardIndex + 1] = '\0';
								else
									czCardID[nCardIndex] = '\0';
								dwCardTickCount = GetMyTickCount();
								if(gbIsReloadSkinOver == TRUE)
								{
									char* tmpcard;
									tmpcard = (char *)malloc(strlen(czCardID) + 1);
									memset(tmpcard,0,(strlen(czCardID) + 1));
									strcpy(tmpcard,czCardID);
									SendNotifyMessage(NULL, WM_INDUCECARD, tmpcard, 0);
								}
							}
						}
						nCardIndex = 0;
						memset(czCardID,0,20);
						break;
					}
					nCardIndex ++;
					break;
				}
			}
		}
		else
		{
			if(wParam == 0x6f)
				wParam = 0x38;    //call waiter
		}
	case MSG_LBUTTONUP:
	case MSG_LBUTTONDOWN:
	case MSG_VIDEOREPLAY:
	case MSG_TDCALLBACK:
	case MSG_SHOWSOUNDVIEW:
	case CLOSEROOM:
	case OPENROOM:
	case WM_CHANGEROOM:
	case WM_CLEANROOM:
	case REDRAWCURRENTVIEW:
	case REDRAWSELECTVIEW:
	case MSG_REFRESHHANDWRITE:
	case WM_TIMEPROMPT:
	case WM_MEDIAKTVOK:
	case MEDIACLIENTAPPEND:
	case WM_TABLETGETRESULT:
	case WM_FIREALARM:
	case MSG_MOUSEMOVE:
	case WM_SAVEADTIMES:
	case WM_REDRAWBROADCASTVIEW:
	case WM_DRINKMSG_CMD:
	case WM_INDUCECARD:
	case WM_UDP_APPENDMEDIA:
	case WM_UDP_GETSONGLIST:
	case WM_UDP_GETCURSONG:
	case WM_UDP_INSERT:
	case WM_UDP_DELETE:
	case WM_UDP_NEXT:
	case WM_UDP_REPLAY:
	case WM_UDP_PAUSEPLAY:
	case WM_UDP_QUERYPAUSEPLAY:
	case WM_UDP_YUANBANDAO:
	case WM_UDP_QUERYYUANBANDAO:
	case WM_UDP_MUTESOUND:
	case WM_UDP_QUERYMUTESOUND:
	case WM_UDP_MODIFYVOL:
	case WM_UDP_QUERYVOL:
	case WM_UDP_MODIFYTONE:
	case WM_UDP_QUERYTONE:
	case WM_UDP_GETALLSTATE:
	case WM_UDP_CALLSERVICE:
	case WM_UDP_MICRADDSUB:
	case WM_UDP_CAPTION:
	case WM_UDP_HUNYIN:
	case WM_UDP_XIAOGUO:
	case WM_MONITORMODIFY:
	case WM_UDP_AWOKE1:
	case WM_UDP_AWOKE2:
	case WM_UDP_CLEANROOM:
	case WM_UDP_BROADCAST:
	case WM_UDP_CLOSEROOMCMD:
	case WM_UDP_RECORD:
	case WM_UDP_MRSONGCMD:
	case WM_UDP_MRSONGSET:
	
	case WM_CLEANCLIENT:              //清客
	case WM_CANCLECLEANCLIENT:        //取消清客
	
		//TDDebug("ADD MESSAGE TO MESSAGE LIST");
		if (gbKTVError)
			break;
/*		if (bGameing && (message == MSG_LBUTTONUP ||
				message == MSG_LBUTTONDOWN ||
				message == MSG_MOUSEMOVE ||
				message == MSG_KEYUP))
		{
		       TDDebug("Is Gameing");
		       break;
		}
		if(!gbIsRevMsg && message == MSG_KEYUP)
		{
			if(!(wParam == 0x08 || wParam == 0x20 || wParam == 0x26 || wParam == 0x28
					|| wParam == 0x25 || wParam == 0x27 || wParam == 0x2E))
			{
				break;
			}
		}*/

		if ((MsgStack.iTailStack + 1) % 512 != MsgStack.iHeadStack)
		{
			MsgStack.Message[MsgStack.iTailStack].hWnd		= hWnd;
			MsgStack.Message[MsgStack.iTailStack].message	= message;
			MsgStack.Message[MsgStack.iTailStack].wParam	= wParam;
			MsgStack.Message[MsgStack.iTailStack].lParam	= lParam;
			MsgStack.iTailStack	= (MsgStack.iTailStack + 1) % 512;
			SetEvent(MsgStack.hMHEvent);
		}
		break;
	case MSG_SETMYCURSOR:
		break;
	case MSG_DESTROY:
		break;
	default:
		break;
	}
	return 0;
}

#define GAPWIDTH 30

#endif
//yang add for shuaka
#define TOUCHDOWN	1
#define TOUCHUP		0
static int gnTouchState = TOUCHUP;
//..
int WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	TDWORD				wdTmp;
	TDDWORD 			dwID;
	static TDINT			iIDFlage = -1;
	char cmdbuf[128];
	char	szTmpTxt[256];

	sprintf(szTmpTxt,"In WndProc msg = %d", message);
	TDDebug(szTmpTxt);
	if (gbKTVError)
		return;

	if (gbLoginPhoto)
	{
		if (message == MSG_TDCALLBACK)
		{
			NextMedia(0);
			return 0;
		}
		if(message == WM_SAVEADTIMES)
		{
			return 0;
		}
#ifdef	WILLDO
		if (message == MSG_LBUTTONUP || message == /*MSG_KEYDOWN*/MSG_KEYUP || (WM_UDP_CLOSEROOMCMD >= message &&  message >= WM_UDP_APPENDMEDIA))
		{
			TDDebug("begin draw back");
			FrameBackgroundLoad(gdwCurrentSkinID);
			TDDebug("end draw back");

			ResumeADBarThreads();

			gbLoginPhoto = FALSE;
#ifdef	WILLDO
			//yang
			if(gBoxType == MAIN_BOX)
			{
				pthread_create(&gpDoubleHandle, NULL, ShowVGAPic, TDNULL);
			}
			//yang..
			dwTCount = GetMyTickCount();
#endif

			if(message == MSG_LBUTTONUP || message == MSG_KEYUP)
				return 0;
		}
		else if(message == WM_MEDIAKTVOK)
		{
		}
		else if(message == MEDIACLIENTAPPEND)//jzn
		{
		}
		else if(message == OPENROOM)
		{
			gnRoomStates = ROOMSTATES_OPEN;
			return 0;
		}
		else if(message == CLOSEROOM)
		{
			if(gBoxType == MAIN_BOX)
			{
				DestroySongList();
				DestroyActorsChain();
				SetCleanSign();
				if(wParam != 1)
					gnRoomStates = ROOMSTATES_CLOSE;
			}
			return 0;
		}
		else if(message ==  WM_CHANGEROOM)
		{
			int nTotal1;
			int nTotal2;
			char szDebug[100];
			nTotal1 = GetSongListTotalCount(FALSE);
			sprintf(szDebug,"nTotal1 = %d",nTotal1);
			TDDebug(szDebug);

			ReCreateSongList(nTotal1);

			nTotal2 = GetSongListTotalCount(FALSE);
			sprintf(szDebug,"nTotal2 = %d",nTotal2);
			TDDebug(szDebug);
			gnRoomStates = ROOMSTATES_OPEN;
			return 0;
		}
		else if(message == WM_CLEANROOM)
		{
			gnRoomStates = ROOMSTATES_CLOSE;
			return 0;
		}
		else
#endif
		{
			return 0;
		}

	}

	switch (message)
	{
		//yang add for preview
	case WM_DESTROY_PREVIEW:
		TDDebug("wndproc WM_DESTROY_PREVIEW\n");
#if 0
		if(gpCurTopMenu && gpCurTopMenu->dwModuleID == MODULE_TOPMENU_PREVIEW)
		{
			gpCurTopMenu->Destroy();
			gpCurTopMenu = NULL;
		}
#endif
		break;
	case WM_FIREALARM:
		SwitchVideo();
		gFireStatus = 1;
		nDoublePause = TDTRUE;

		PauseADBarThreads();
		gADBarInfo.bIsDraw = FALSE;
//		gCompanyLogo.bIsDraw = FALSE;
		gbKTVError = TRUE;
		nPauseDrawThread = -1;

		nFireAlarm = 1;
		gADBarInfo.bIsRunning = 0;
		usleep(1000 * 1000);

//		pthread_mutex_lock(&gLinkGameMutex);   //Stop game rate
//		bLinkGameStart = TDFALSE;
//		nLinkStartTime = 0;
//		pthread_mutex_unlock(&gLinkGameMutex);
		TDSetRecordStop();

		TDDebug("scm TimePrompt,so nDouble=0");
//		dwTCount = GetMyTickCount();

		while(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		pthread_mutex_unlock(&gDrawDCMutex);
		//WaitForMessageClean(0);
		pthread_mutex_lock(&gsViewIdleMutex);

		STBSendInfo(FIREALARM,0);
		FireAlarm(TDTRUE);
		ResumeADBarThreads();
		pthread_mutex_unlock(&gsViewIdleMutex);
		break;
	case MSG_SERIALPANEL:
		sprintf(szTmpTxt, "ttyAMA2 ++++ wParam = 0x%x\n", wParam);
		TDDebug(szTmpTxt);
		//yang
		if(wParam == 0x01)			//service
			wParam = 0x70;
		else if(wParam == 0x02)			//next
			wParam = 0xDC;
		else if(wParam == 0x03)			//play\pause
			wParam = 0xBB;
		else if(wParam == 0x04)			//vol +
			wParam = 0x08;
		else if(wParam == 0x05)			//vol -
			wParam = 0x20;
		else if(wParam == 0x06)			//mic +
			wParam = 0x26;
		else if(wParam == 0x07)			//mic -
			wParam = 0x28;
		else if(wParam == 0x08)			//sheng diao 
			wParam = 0x25;
		else if(wParam == 0x09)			//biaozhundiao
			wParam = 0x2E;
		else if(wParam == 0x0a)			//jiangdiao
			wParam = 0x27;
		else if(wParam == 0x0b)			//replay
			wParam = 0xDE;
		else if(wParam == 0x0c)			//yuan\ban\dao
			wParam = 0xBD;
		else if(wParam == 0x0d)			//mute
			wParam = 0xBA;
		else if(wParam == 0x12)			//welcome
			wParam = 0x13;
		else if(wParam == 0x13)			//love
			wParam = 0x6A;
		else if(wParam == 0x14)			//laugh
			wParam = 0x6B;
		else
			wParam = 0x00;
		
		//		else if(wParam == 0x4c)	//mute
		//			wParam = 0xba;
		
		wdTmp = wParam & 0xFF;
		switch(wdTmp)
		{
		case 0x08:	//vol+
			if(gBoxType == MAIN_BOX && (TDGetRecordCommandType() != 2))
			{
				TD_VideoIncreaseVolume();
				if(gpCurTopMenu && gpCurTopMenu->dwModuleID == 0xF0100000)
				{
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0);
				}
				PauseThreadLock();
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}
			break;
		case 0x20:	//vol-
			if(gBoxType == MAIN_BOX && (TDGetRecordCommandType() != 2))
			{
				TD_VideoDecreaseVolume();
				if(gpCurTopMenu && gpCurTopMenu->dwModuleID == 0xF0100000)
				{
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0);
				}
				PauseThreadLock();
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}
			break;
		case 0x70://call waiter
			if(gBoxType != MAIN_BOX)
			       	TDKtvCallWaiter();
			if(!gnNotUseSoftCallWaiter)
			{
#if 1
				SwitchVideo();
#else
				if(gnDouble == 3)
				{
					dwTCount = GetMyTickCount();
					nDouble = 0;
					gnDouble = 0;
					gnStaffTime = 0;
					usleep(10);
					RedrawWindow();
				}
				else
				{
					dwTCount = GetMyTickCount();
					gnDouble = 0;
					gnStaffTime = 0;
				}
#endif
#if 0
				if (!CallWaiter())
				{
					TDDebug("call waite ok");
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, gczInfo[90]);
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.nPriority = 2;
				}
				else
				{
					TDDebug("call waite error");
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, gczInfo[89]);
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.nPriority = 2;
				}
#else
				if(!gnNotUseSoftCallWaiter)
				{
					TDPBYTE pData = NULL;
					TDLONG lDBTotal = 0;
					PTAGERRORCODES pErrorCodes;
					char czSet[128];
					sprintf(czSet ,"Call MachineIpAddress:%s RoomIpAddress:%s CallTypeID:0 ",m_szMyIP,m_szMyIP);
					lDBTotal = GetDatabaseForNewDrink(czSet, (void **)&(pData), TAGERRORCODESSIZE);
					
					if (lDBTotal  == 0)
					{
						TDDebug("call waite ok");
						gADBarInfo.bIsDraw = FALSE;
						strcpy(gADBarInfo.szNewTxt, gczInfo[90]);
						gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
						gADBarInfo.nPriority = 2;
						TDSecondaryMessageBox("呼叫成功!");
					}
					else 
					{
						TDDebug("call waite error");
						gADBarInfo.bIsDraw = FALSE;
						strcpy(gADBarInfo.szNewTxt, gczInfo[89]);
						gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
						gADBarInfo.nPriority = 2;
						if(pData)
						{
							pErrorCodes = (PTAGERRORCODES)pData;
							TDSecondaryMessageBox(pErrorCodes ->szErrorDescribe);
							free(pData);
							pData = NULL;
						}
						else
							TDSecondaryMessageBox("呼叫失败!");
					}
				}
#endif
			}
			return 0;
		case 0xDC://next song
			if((TDGetRecordCommandType() == 1) || (TDGetRecordCommandType() == 2))
			{
				TDSetRecordStop();
				StopRecordingMedia();
			}
		case 0xDE://replay
			if(TDGetRecordCommandType() == 1)
			{
				TDSetRecordStop();
				StopRecordingMedia();
			}
		case 0xBD://change audiostream
		case 0xBA://mute/unmute
		case 0xBB://pause/play
			gBottomPanel.OnPress(0, wParam);
			break;
		case 0x26://mic +
			if(gBoxType == MAIN_BOX)
			{
				TD_InCreaseMicVolume();
			}
			break;
		case 0x28://mic -
			if(gBoxType == MAIN_BOX)
			{
				TD_DeCreaseMicVolume();
			}
			break;
		case 0x13:
			if (gRecordDevInited == 0)
			{
				break;
			}
			if(TDGetRecordCommandType() == 0)
			{
				TDDebug("press 0x13,we will play Love");
				TDSetRecordCommandType(3);
				//WhxPlayShowWav(0);
				WhxShowWelcome();
			}
			return 0;
		case 0x6A:
			if (gRecordDevInited == 0)
			{
				break;
			}
			if(TDGetRecordCommandType() == 0)
			{
				TDDebug("press 0x6A,we will play Love");
				TDSetRecordCommandType(3);
				//WhxPlayShowWav(1);
				WhxShowLove();
			}
			return 0;
		case 0x6B:
			if (gRecordDevInited == 0)
			{
				break;
			}
			if(TDGetRecordCommandType() == 0)
			{
				TDDebug("press 0x6B,we will play Laugh");
				TDSetRecordCommandType(3);
				//WhxPlayShowWav(2);
				WhxShowLaugh();
			}
			return 0;
		case 0xFC:
		case 0x6F://dao chang
			if(gBoxType == MAIN_BOX)
			{
				AudioSetDaoChangSwitch(wdTmp);
			}
			return 0;
		default:
			return 0;
		}
		break;
	case MSG_KEYUP:
		if(wParam == 0)
			break;
		gnStaffTime = 0;
		if(lParam != 0x8000)
		{
			sprintf(szTmpTxt, "wParam = 0x%x\n", wParam);
			TDDebug(szTmpTxt);
			if(wParam == 0x5)
				wParam = 0x70;    //call waiter
			else if(wParam == 0x29)
				wParam = 0x20;    //vol -
			else if(wParam == 0x66)
				wParam = 0x08;    //vol +
			else if(wParam == 0x4e)
				wParam = 0xbd;    //org/accpy
			else if(wParam == 0x4a)   //home ??
			{
				//wParam = 0x6d;
				wParam = 0x6F;
			}
			else if(wParam == 0xa)    //yiyuan
				wParam = 0x09;
			else if(wParam == 0x4c)   //mute
				wParam = 0xba;
			else if(wParam == 0x52)   //replay
				wParam = 0xde;
			else if(wParam == 0x3d)
				wParam = 0x37;
			else if(wParam == 0x3e)
				wParam = 0x38;
			else if(wParam == 0x46)
				wParam = 0x39;
			else if(wParam == 0x78)
				wParam = 0x7a;    //page up
			else if(wParam == 0x25)
				wParam = 0x34;
			else if(wParam == 0x2e)
				wParam = 0x35;
			else if(wParam == 0x36)
				wParam = 0x36;
			else if(wParam == 0x7)
				wParam = 0x7b;   //page down
			else if(wParam == 0x16)
				wParam = 0x31;
			else if(wParam == 0x1e)
				wParam = 0x32;
			else if(wParam == 0x26)
				wParam = 0x33;
			else if(wParam == 0x5d)
				wParam = 0xdc;    //next
			else if(wParam == 0x45)
				wParam = 0x30;
			else if(wParam == 0x76)    //esc
				wParam = 0x1b;
			else if(wParam == 0x5a)
				wParam = 0xd;      //enter
			else if(wParam == 0x41)
				wParam = 0xbc;      //film
			else if(wParam == 0x4e)
				wParam = 0xbe;      //wine
			else if(wParam == 0x4a)
				wParam = 0xbf;      //yule
			else if(wParam == 0x06)
				wParam = 0x71;      //change screen
			else if(wParam == 0x4)
				wParam = 0x72;      //geyin
			else if(wParam == 0xc)
				wParam = 0x73;      //py
			else if(wParam == 0x3)
				wParam = 0x74;      //zs
			else if(wParam == 0xb)
				wParam = 0x75;      //ba
			else if(wParam == 0x83)
				wParam = 0x76;      //yy
			else if(wParam == 0xa)
				wParam = 0x77;      //qz
			else if(wParam == 0x1)
				wParam = 0x78;      //ph
			else if(wParam == 0x9)
				wParam = 0x79;      //gk
			else if(wParam == 0xe)
				wParam = 0xC0;      //delete
			else if(wParam == 0x55)
				wParam = 0xbb;      //play
			/*else if(wParam == 0x71)
				wParam = 0x2E;      //biaozhundiao   delete*/
			else if(wParam == 0x75)
				wParam = 0x26;      //mic inc     top arrow
			else if(wParam == 0x72)
				wParam = 0x28;      //mic dec     bottom arrow
			else if(wParam == 0x6b)
				wParam = 0x25;      //tune inc    left arrow
			else if(wParam == 0x74)
				wParam = 0x27;      //tune dec    right arrow
			else if(wParam == 0x7c)
				wParam = 0x6A;      //dao chang   little key *
			else if(wParam == 0x71)
				wParam = 0xFC;      //dao chang   little key del
			else if(wParam == 0x1c)     //A -Z
				wParam = 0x61;
			else if(wParam == 0x32)
				wParam = 0x62;
			else if(wParam == 0x21)
				wParam = 0x63;
			else if(wParam == 0x23)
				wParam = 0x64;
			else if(wParam == 0x24)
				wParam = 0x65;
			else if(wParam == 0x2b)
				wParam = 0x66;
			else if(wParam == 0x34)
				wParam = 0x67;
			else if(wParam == 0x33)
				wParam = 0x68;
			else if(wParam == 0x43)
				wParam = 0x69;
			else if(wParam == 0x3b)
				wParam = 0x6A;
			else if(wParam == 0x42)
				wParam = 0x6B;
			else if(wParam == 0x4b)
				wParam = 0x6c;
			else if(wParam == 0x3a)
				wParam = 0x6d;
			else if(wParam == 0x31)
				wParam = 0x6e;
			else if(wParam == 0x44)
				wParam = 0x6f;
			else if(wParam == 0x4d)
				wParam = 0x70;
			else if(wParam == 0x15)
				wParam = 0x71;
			else if(wParam == 0x2d)
				wParam = 0x72;
			else if(wParam == 0x1b)
				wParam = 0x73;
			else if(wParam == 0x2c)
				wParam = 0x74;
			else if(wParam == 0x3c)
				wParam = 0x75;
			else if(wParam == 0x2a)
				wParam = 0x76;
			else if(wParam == 0x1d)
				wParam = 0x77;
			else if(wParam == 0x22)
				wParam = 0x78;
			else if(wParam == 0x35)
				wParam = 0x79;
			else if(wParam == 0x1a)
				wParam = 0x7a;
			else if(wParam == 0x7e)
				wParam = 0x20;         //??
			else if(wParam == 0x0E)
				wParam = 0xC0;         //
			else if(wParam == 0x12)
				wParam = 0x2c;         //delete   print screen sys
			else if(wParam == 0x7a)
				wParam = 0x91;         //sound   scroll lock
			else if(wParam == 0x11)        // L alt
				wParam = 0x13;
			else if(wParam == 0x7c)        // *
				wParam = 0x6a;
			else if(wParam == 0x79)        // +
				wParam = 0x6b;
			else if(wParam == 0x70)
				wParam = 0x2D;//褰曢煶
			else if(wParam == 0x69)
				wParam = 0x23;//鍥炴斁
			else if(wParam == 0x77)
				wParam = 0x90;//鍋滄

			if(gnUseCard == 1)
			{//Key Port
				if((wParam >= 0x30 && wParam <= 0x39) || wParam == 0xd)
				{
					if(nCardIndex >= 19 || (GetMyTickCount() - dwCardTickCount > 2))
					{
						nCardIndex = 0;
						memset(czCardID,0,20);
					}
					if(nCardIndex == 0)
						dwCardTickCount = GetMyTickCount();
					czCardID[nCardIndex] = wParam;
					if(wParam == 0xd || nCardIndex == 9)
					{
						if(nCardIndex == 8 || nCardIndex == 9)
						{
							if((GetMyTickCount() - dwCardTickCount) <= 2)  //msl
							{
								if(nCardIndex == 9)
									czCardID[nCardIndex + 1] = '\0';
								else
									czCardID[nCardIndex] = '\0';
								dwCardTickCount = GetMyTickCount();
								if(gbIsReloadSkinOver == TRUE)
								{
									char* tmpcard;
									tmpcard = (char *)malloc(strlen(czCardID) + 1);
									memset(tmpcard,0,(strlen(czCardID) + 1));
									strcpy(tmpcard,czCardID);
									SendNotifyMessage(NULL, WM_INDUCECARD, tmpcard, 0);
								}
							}
						}
						nCardIndex = 0;
						memset(czCardID,0,20);
						break;
					}
					nCardIndex ++;
					break;
				}
			}
		}
		else
		{
			if(wParam == 0x6f)
				wParam = 0x38;    //call waiter
		}
		wdTmp = wParam & 0xFF;
		switch(wdTmp)
		{
		case 0x08:	//vol+
			if(gBoxType == MAIN_BOX && (TDGetRecordCommandType() != 2))
			{
				TD_VideoIncreaseVolume();
				if(gpCurTopMenu && gpCurTopMenu->dwModuleID == MODULE_TOPMENU_TUNE)
				{
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0);
				}
				PauseThreadLock();
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}
			break;
		case 0x20:	//vol-
			if(gBoxType == MAIN_BOX && (TDGetRecordCommandType() != 2))
			{
				TD_VideoDecreaseVolume();
				if(gpCurTopMenu && gpCurTopMenu->dwModuleID == MODULE_TOPMENU_TUNE)
				{
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0);
				}
				PauseThreadLock();
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}
			break;
		case 0x70://call waiter
			if(gBoxType != MAIN_BOX)
			       	TDKtvCallWaiter();
			if(!gnNotUseSoftCallWaiter)
			{
#if 1
				SwitchVideo();
#else
				if(gnDouble == 3)
				{
					dwTCount = GetMyTickCount();
					nDouble = 0;
					gnDouble = 0;
					gnStaffTime = 0;
					usleep(10);
					RedrawWindow();
				}
				else
				{
					dwTCount = GetMyTickCount();
					gnDouble = 0;
					gnStaffTime = 0;
				}
#endif
#if 0
				if (!CallWaiter())
				{
					TDDebug("call waite ok");
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, gczInfo[90]);
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.nPriority = 2;
				}
				else
				{
					TDDebug("call waite error");
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, gczInfo[89]);
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.nPriority = 2;
				}
#else
				if(!gnNotUseSoftCallWaiter)
				{
					TDPBYTE pData = NULL;
					TDLONG lDBTotal = 0;
					PTAGERRORCODES pErrorCodes;
					char czSet[128];
					sprintf(czSet ,"Call MachineIpAddress:%s RoomIpAddress:%s CallTypeID:0 ",m_szMyIP,m_szMyIP);
					lDBTotal = GetDatabaseForNewDrink(czSet, (void **)&(pData), TAGERRORCODESSIZE);
					
					if (lDBTotal  == 0)
					{
						TDDebug("call waite ok");
						gADBarInfo.bIsDraw = FALSE;
						strcpy(gADBarInfo.szNewTxt, gczInfo[90]);
						gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
						gADBarInfo.nPriority = 2;
//pengzhongkai						TDSecondaryMessageBox("呼叫成功!");
					}
					else 
					{
						TDDebug("call waite error");
						gADBarInfo.bIsDraw = FALSE;
						strcpy(gADBarInfo.szNewTxt, gczInfo[89]);
						gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
						gADBarInfo.nPriority = 2;
						if(pData)
						{
							pErrorCodes = (PTAGERRORCODES)pData;
//pengzhongkai							TDSecondaryMessageBox(pErrorCodes ->szErrorDescribe);
							free(pData);
							pData = NULL;
						}
						else
						{
//pengzhongkai							TDSecondaryMessageBox("呼叫失败!");
						}
					}
				}
#endif
			}
			return 0;
		case 0xDC://next song
			if((TDGetRecordCommandType() == 1) || (TDGetRecordCommandType() == 2))
			{
				TDSetRecordStop();
				StopRecordingMedia();
			}
		case 0xDE://replay
			if(TDGetRecordCommandType() == 1)
			{
				TDSetRecordStop();
				StopRecordingMedia();
			}
		case 0xBD://change audiostream
		case 0xBA://mute/unmute
		case 0xBB://pause/play
			gBottomPanel.OnPress(0, wParam);
			break;
		case 0x26://mic +
			if(gBoxType == MAIN_BOX)
			{
				TD_InCreaseMicVolume();
			}
			break;
		case 0x28://mic -
			if(gBoxType == MAIN_BOX)
			{
				TD_DeCreaseMicVolume();
			}
			break;
		case 0x13:
			if (gRecordDevInited == 0)
			{
				break;
			}
			if(TDGetRecordCommandType() == 0)
			{
				TDDebug("press 0x13,we will play Love");
				TDSetRecordCommandType(3);
				//WhxPlayShowWav(0);
				WhxShowWelcome();
			}
			return 0;
		case 0x6A:
			if (gRecordDevInited == 0)
			{
				break;
			}
			if(TDGetRecordCommandType() == 0)
			{
				TDDebug("press 0x6A,we will play Love");
				TDSetRecordCommandType(3);
				//WhxPlayShowWav(1);
				WhxShowLove();
			}
			return 0;
		case 0x6B:
			if (gRecordDevInited == 0)
			{
				break;
			}
			if(TDGetRecordCommandType() == 0)
			{
				TDDebug("press 0x6B,we will play Laugh");
				TDSetRecordCommandType(3);
				//WhxPlayShowWav(2);
				WhxShowLaugh();
			}
			return 0;
		case 0xFC:
		case 0x6F://dao chang
			if(gBoxType == MAIN_BOX)
			{
				AudioSetDaoChangSwitch(wdTmp);
			}
			return 0;
		default:
			return 0;
#ifdef WILLDO
		case 0xBC:   //movie
		case 0xBE:  //drink
		case 0x72:  //F3 (Songer)
		case 0x74:  // ;F5 (Length)
		case 0x79:  // ;F10 (My Lib)
		case 0x76:  // ;F7 (Language)
		case 0x77:  // ;F8 (Type)
		case 0x78:  // ;F9 (Top List)
		case 0x91:  //;
		case 0x5B:  //;
		case 0xA2:  //;
			if(wdTmp == 0xBC)
				dwID = 0x201;
			else if(wdTmp == 0xBE)
				dwID = 0x310;
			else if(wdTmp == 0x72)
				dwID = 0x102;
			else if(wdTmp == 0x73)
				dwID = 0x104;
			else if(wdTmp == 0x74)
				dwID = 0x105;
			else if(wdTmp == 0x79)
				dwID = 0x103;//0x120;
			else if(wdTmp == 0x75)
				dwID = 0x119;
			else if(wdTmp == 0x76)
				dwID = 0x106;
			else if(wdTmp == 0x77)
				dwID = 0x107;
			else if(wdTmp == 0x78)
				dwID = 0x108;
			else if(wdTmp == 0x91)
				dwID = 0x103;
			else if(wdTmp == 0xA2)
				dwID = 0x118;
			else if(wdTmp == 0x5B)
				dwID = 0x109;

			if(gpCurTopMenu != NULL)
			{
				gpCurTopMenu->Destroy();
				gpCurTopMenu = NULL;
			}
			ResetCurLButtonDownView();
			ViewHandleLBtnUpShell(dwID);
			return 0;
#endif
		}
#ifdef WILLDO
		if (wdTmp >= 0x30 && wdTmp <= 0x39)
		{
			char	czTemp[60];
			sprintf(czTemp, "wdFlags is:0x%X", iIDFlage);
			TDDebug(czTemp);

			iIDFlage <<= 4;
			iIDFlage |= ((wdTmp - 0x30) & 0xF);

			sprintf(czTemp, "wdFlags is:0x%X", iIDFlage);
			TDDebug(czTemp);
		}

		if(gpCurTopMenu != NULL)
		{
			if(gpCurTopMenu->OnPress(0, wdTmp))
			{
				iIDFlage = -1;
				break;
			}
		}
		else if(gpCurClientView->OnPress(iIDFlage, wdTmp))
		{
			iIDFlage = -1;
			break;
		}
#endif
		break;
	case MSG_LBUTTONDOWN:
		if(gbChangeSkin == FALSE)
		{
#if 0
			bDropMsg = TRUE;
#endif
			return;
		}
		if(gnDouble == 3)
		{
			TDDebug("gnDouble = 3, break");
			break;
		}
		if(gnDouble == 4)
		{
			gpCurTopMenu->ShowWindow(0,0);
			
			SetSlaveWindow(427,
				227,
				360,
				267,
				1,
				0);
			gnDouble = 0;
			return 0;
		}
		nDouble = 0;
		gnDouble = 0;
		gnStaffTime = 0;
		dwTCount = GetMyTickCount();

		if(gFireStatus)
		{
			TDDebug("firealarm, return");
			return ;
		}
		TDDebug("MSG_LBUTTONDOWN");
#ifdef	WILLDO

		if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == 0x66D00000))
		{
			gpCurTopMenu->OnLButtonDown(wParam, MAKETDPOINT(lParam));
		}
		else if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == 0xF0900000))
		{
			gpCurTopMenu->OnLButtonDown(wParam, MAKETDPOINT(lParam));
		}
		else if(TDGetInBroadcast())
		{
			if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == 0xF2800000) && TDPtInRect((RECT*)&(gpCurTopMenu->Rect), MAKETDPOINT(lParam)))
				gpCurTopMenu->OnLButtonDown(wParam, MAKETDPOINT(lParam));

			if(gpCurExpandView && gpCurExpandView->dwModuleID == 0x20300000 && TDPtInRect((RECT*)&(gpCurExpandView->Rect), MAKETDPOINT(lParam)))
				gpCurExpandView->OnLButtonDown(wParam, MAKETDPOINT(lParam));
		}
		else
#endif
		if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == MODULE_TOPMENU_AWOKETIMEOUT))
		{
			gpCurTopMenu->OnLButtonDown(wParam, MAKETDPOINT(lParam));
		}
		else
		{
			gpMainFrame->OnLButtonDown(wParam, MAKETDPOINT(lParam));
		}
		gnTouchState = TOUCHDOWN;
		TDDebug("MSG_LBUTTONDOWN END");
		break;
	case MSG_MOUSEMOVE:
		if(gnDouble == 3)
		{
			break;
		}
		nDouble = 0;
		gnDouble = 0;
		gnStaffTime = 0;
		dwTCount = GetMyTickCount();

#ifdef	WILLDO
		if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == 0x66D00000))
		{
			gpCurTopMenu->OnMouseMove(wParam, MAKETDPOINT(lParam));
		}
		else if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == 0xF0900000))
		{
			gpCurTopMenu->OnMouseMove(wParam, MAKETDPOINT(lParam));
		}
		else if(TDGetInBroadcast())
		{
		}
		else
#endif
			if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == MODULE_TOPMENU_AWOKETIMEOUT))
			{
				gpCurTopMenu->OnMouseMove(wParam, MAKETDPOINT(lParam));
			}
			else
			{
				gpMainFrame->OnMouseMove(wParam, MAKETDPOINT(lParam));
			}
		break;
	case MSG_LBUTTONUP:
#if 0
		if(bDropMsg)
		{
			bDropMsg = FALSE;
			return;
		}
#endif
		TDDebug("MSG_LBUTTONUP");
		if(gnDouble == 3)
		{
			TDDebug("gnDouble = 3, SwitchVideo()");
			SwitchVideo();
			TDDebug("gnDouble = 3, SwitchVideo() break");
			break;
		}

		if(gFireStatus)
			return ;

#ifdef	WILLDO
		if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == 0x66D00000))
		{
			gpCurTopMenu->OnLButtonUp(wParam, MAKETDPOINT(lParam));
		}
		else if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == 0xF0900000))
		{
			gpCurTopMenu->OnLButtonUp(wParam, MAKETDPOINT(lParam));
		}
		else if(TDGetInBroadcast())//wj 2008-03-29
		{
			if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == 0xF2800000) && TDPtInRect((RECT*)&(gpCurTopMenu->Rect), MAKETDPOINT(lParam)))
				gpCurTopMenu->OnLButtonUp(wParam, MAKETDPOINT(lParam));

			if(gpCurExpandView && gpCurExpandView->dwModuleID == 0x20300000 && TDPtInRect((RECT*)&(gpCurExpandView->Rect), MAKETDPOINT(lParam)))
				gpCurExpandView->OnLButtonUp(wParam, MAKETDPOINT(lParam));
		}
 		else
#endif
		if(gpCurTopMenu && (gpCurTopMenu->dwModuleID == MODULE_TOPMENU_AWOKETIMEOUT || gpCurTopMenu->dwModuleID == MODULE_TOPMENU_LOGIN_CLEANROOM))
		{
			gpCurTopMenu->OnLButtonUp(wParam, MAKETDPOINT(lParam));
		}
		else
		{
			gpMainFrame->OnLButtonUp(wParam, MAKETDPOINT(lParam));
		}
		gnTouchState = TOUCHUP;
		TDDebug("MSG_LBUTTONUP END");
		break;
	case MSG_TDCALLBACK:
		TDDebug("MSG_TDCALLBACK");
		if(gpCurClientView->dwModuleID != 0x20100000)
		{
			SetOsdShowWord("test", OSD_INFOSTRINGSTOP, 1, SkinID2LangID(gi8LanguageID), 0, 0, 200);
			TDDebug("MSG_TDCALLBACK 1");
			NextMedia(0);
			TDDebug("MSG_TDCALLBACK 2");
#if 1      
			PauseThreadLock();
			TDDebug("MSG_TDCALLBACK 3");
			gBottomPanel.i64DgtInfo = 0;
			gBottomPanel.Initialize(0, 0);
			TDDebug("MSG_TDCALLBACK 4");
			gBottomPanel.DrawWindow(0);
			TDDebug("MSG_TDCALLBACK 5");
			//gExpandViewCtrl.DrawWindow(0);

			if (!gbPicIsShow)
			{
				gBottomPanel.ShowWindow(NULL, 0);
				TDDebug("MSG_TDCALLBACK 6");
				//gExpandViewCtrl.ShowWindow(NULL, 0);
			}
			TDDebug("MSG_TDCALLBACK 7");
			ResumeThreadLock();
			TDDebug("MSG_TDCALLBACK 8");
#endif
			gADBarInfo.bIsDraw = FALSE;
			strcpy(gADBarInfo.szNewTxt, gczInfo[80]);
			gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
		}
		TDDebug("END MSG_TDCALLBACK");
		break;
	case MSG_REFRESHHANDWRITE:
		TDDebug("MSG_REFRESHHANDWRITE start\n");
		printf("11111111111111111111\n");
		if(gpCurTopMenu != NULL &&(!gnPageHasChanged))
		{
			TDDebug("MSG_REFRESHHANDWRITE 1111111\n");
			int nDrawType = 0 ;
			int	nLoop = 0;
			int	nOffset = 0;

			if(wParam != 2 && wParam != 3)
			{
				gnHandWriteCharIsEditing = 0;
			}
			if(gpCurTopMenu->dwModuleID == MODULE_TOPMENU_NORMALHANDWRITE)
			{printf("222222222222\n");
				gnSongSource = 6;
//				gpCurClientView->Initialize(10, 0);
//				PageStack.i64Spell = 0;
//				PageStack.i64HeadStroke = 0;
//				PageStack.bStrokeNum = 0;
				if (gpCurClientView->dwModuleID ==0x11000000)
				{
					nDrawType =ACTORDRAWSTATE_REDRAW_TOPMENU ;
				}
				else 
					nDrawType =0;
				//huanguo 20110315
				sprintf(szTmpTxt, "MSG_REFRESHHANDWRITE gpCurClientView->dwModuleID = 0x%x\n", gpCurClientView->dwModuleID);
				TDDebug(szTmpTxt);
				if(gpCurClientView->dwModuleID == 0x11800000 /*NewSongView*/\
					|| gpCurClientView->dwModuleID == 0x10200000 /*MyLibSongView*/\
					||gpCurClientView->dwModuleID == 0x19200000/*SearchSongView*/ \
					||gpCurClientView->dwModuleID == 0x11200000 /*ActorSongView*/ \
					||gpCurClientView->dwModuleID == 0x11600000 /*TypeSongView*/
					)
				{
//					printf("thundermouse7413  gpCurClientView->lDBTotal  =%d  \n",gpCurClientView->lDBTotal);
					if (gpCurClientView->lDBTotal <= 0)
					{
						gnActorNumState   = ACTOR_NUM_STATE_SMALL ;
					}
					else if (gpCurClientView->lDBTotal > 0 && gpCurClientView->lDBTotal <= SONG_PAGE_SONGNUM)	//modify by tonykong
					{
						gnActorNumState   = ACTOR_NUM_STATE_SMALL ;
					}
					else if (gpCurClientView->lDBTotal >SONG_PAGE_SONGNUM && gpCurClientView->lDBTotal <=SONG_PAGE_SONGNUM*2)	//modify by tonykong
					{
						gnActorNumState   = ACTOR_NUM_STATE_MID1 ;
					}
					else if (gpCurClientView->lDBTotal >SONG_PAGE_SONGNUM*2 && gpCurClientView->lDBTotal <=SONG_PAGE_SONGNUM*3)	//modify by tonykong
					{
						gnActorNumState   = ACTOR_NUM_STATE_MID2;
					}
					else if (gpCurClientView->lDBTotal >SONG_PAGE_SONGNUM*3)	//modify by tonykong
					{
						gnActorNumState   = ACTOR_NUM_STATE_BIG  ;
					}

					//add by tonykong
					iNewSongObjectNum = gpCurClientView->lDBTotal / gpCurClientView->iCPP +
										(gpCurClientView->lDBTotal % gpCurClientView->iCPP ? 1 : 0);
					iNewSongObjectNum = iNewSongObjectNum > SONG_DC_NUM ? SONG_DC_NUM : iNewSongObjectNum;
					gpCurClientView->lCurObject = 0 ;
					//end..
					
					iNewSongLastPageNum = gpCurClientView->lDBTotal%gpCurClientView->iCPP;
					giFirstEnterNewsong  = 1 ;
					iNewsongObjPosMoveIndex = 0 ;
					gnNewsongOffset = 0 ; 
					gpCurClientView->lCurData = 0;
				}
				//huanguo 20110315 end 
//				printf("thunder mouser will call draw \n");
				gpCurClientView->DrawWindow(nDrawType);
//				printf("thunder mouser will call draw end \n");
			}
			gpCurTopMenu->DrawWindow(0);

			if(gpCurTopMenu->dwTopID == MODULE_TOPMENU_INPUTHANDWRITE)
			{printf("333333333333333333\n");
				gpCurTopMenu->ShowWindow(0, 1);
			}

			if(gpCurTopMenu->dwModuleID == MODULE_TOPMENU_NORMALHANDWRITE)
			{
				pthread_mutex_lock(&gMemSongInfoMutex);
				gpCurClientView->ShowWindow(0, KEEPSHOWING|SHOWNEXTANDPROCPAGE);
				pthread_mutex_unlock(&gMemSongInfoMutex);
			}

			for(nLoop = 0; nLoop < gHandWriteBufferLen; nLoop++)
			{
				if((gHandWriteTable[nLoop].nSendFlag == 0) &&
						(gHandWriteTable[nLoop].nPrepareFlag == 0) &&
						(gHandWriteTable[nLoop].nPointNum > 0))
				{
					TDDebug("writing!!!!!!!!!! return");
					return 0;//DefaultMainWinProc(hWnd, message, wParam, lParam);
				}
			}
			gnCannotModifyStatus = 0;
		}
		else if (gpCurTopMenu != NULL )
		{
			TDDebug("MSG_REFRESHHANDWRITE 11111112\n");
			gpCurTopMenu->DrawWindow(0);
			gpCurTopMenu->ShowWindow(0, 1);
		}
		else
		{
			int nDrawType = 0 ;
			TDDebug("MSG_REFRESHHANDWRITE 11111113\n");
			if(gpCurClientView->dwModuleID == 0x11800000 /*NewSongView*/\
				|| gpCurClientView->dwModuleID == 0x10200000 /*MyLibSongView*/\
				||gpCurClientView->dwModuleID == 0x19200000/*SearchSongView*/ \
				||gpCurClientView->dwModuleID == 0x11200000 /*ActorSongView*/ \
				||gpCurClientView->dwModuleID == 0x11600000 /*TypeSongView*/\
				||gpCurClientView->dwModuleID == 0x11000000/*actorselview*/)
			{
				TDDebug("MSG_REFRESHHANDWRITE 11111114\n");
				//yang add for handwrite
				if (gpCurClientView->dwModuleID ==0x11000000)
				{
					nDrawType =ACTORDRAWSTATE_REDRAW_TOPMENU ;
				}
				else 
					nDrawType =0;
				//huanguo 20110315
				sprintf(szTmpTxt, "MSG_REFRESHHANDWRITE gpCurClientView->dwModuleID = 0x%x\n", gpCurClientView->dwModuleID);
				TDDebug(szTmpTxt);
				if(gpCurClientView->dwModuleID == 0x11800000 /*NewSongView*/\
					|| gpCurClientView->dwModuleID == 0x10200000 /*MyLibSongView*/\
					||gpCurClientView->dwModuleID == 0x19200000/*SearchSongView*/ \
					||gpCurClientView->dwModuleID == 0x11200000 /*ActorSongView*/ \
					||gpCurClientView->dwModuleID == 0x11600000 /*TypeSongView*/
					)
				{
					//					printf("thundermouse7413  gpCurClientView->lDBTotal  =%d  \n",gpCurClientView->lDBTotal);
					if (gpCurClientView->lDBTotal <= 0)
					{
						gnActorNumState   = ACTOR_NUM_STATE_SMALL ;
					}
					else if (gpCurClientView->lDBTotal > 0 && gpCurClientView->lDBTotal <= SONG_PAGE_SONGNUM)	//modify by tonykong
					{
						gnActorNumState   = ACTOR_NUM_STATE_SMALL ;
					}
					else if (gpCurClientView->lDBTotal >SONG_PAGE_SONGNUM && gpCurClientView->lDBTotal <=SONG_PAGE_SONGNUM*2)	//modify by tonykong
					{
						gnActorNumState   = ACTOR_NUM_STATE_MID1 ;
					}
					else if (gpCurClientView->lDBTotal >SONG_PAGE_SONGNUM*2 && gpCurClientView->lDBTotal <=SONG_PAGE_SONGNUM*3)	//modify by tonykong
					{
						gnActorNumState   = ACTOR_NUM_STATE_MID2;
					}
					else if (gpCurClientView->lDBTotal >SONG_PAGE_SONGNUM*3)	//modify by tonykong
					{
						gnActorNumState   = ACTOR_NUM_STATE_BIG  ;
					}

					//add by tonykong
					iNewSongObjectNum = gpCurClientView->lDBTotal / gpCurClientView->iCPP +
										(gpCurClientView->lDBTotal % gpCurClientView->iCPP ? 1 : 0);
					iNewSongObjectNum = iNewSongObjectNum > SONG_DC_NUM ? SONG_DC_NUM : iNewSongObjectNum;
					gpCurClientView->lCurObject = 0 ;
					//end..
					
					iNewSongLastPageNum = gpCurClientView->lDBTotal%gpCurClientView->iCPP;
					giFirstEnterNewsong  = 1 ;
					iNewsongObjPosMoveIndex = 0 ;
					gnNewsongOffset = 0 ; 
					gpCurClientView->lCurData = 0;
				}
				//huanguo 20110315 end 
				//				printf("thunder mouser will call draw \n");
				gpCurClientView->DrawWindow(nDrawType);
				//..
			}
			gpCurClientView->ShowWindow(NULL, 0);
			TDDebug("MSG_REFRESHHANDWRITE 11111115\n");
		}
		TDDebug("MSG_REFRESHHANDWRITE end\n");
		break;
	case REDRAWCURRENTVIEW:
		if(gpCurTopMenu == NULL || gbLoginPhoto)
		{
			break;
		}
		if(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
		{
			break;
		}
		pthread_mutex_unlock(&gDrawDCMutex);

		if(gBoxType == MAIN_BOX/* && gpCurTopMenu == NULL*/)
		{
			if(gpCurTopMenu->dwTopID == MODULE_TOPMENU_RECORDING)
			{
				pthread_mutex_lock(&gsViewIdleMutex);
				gpCurTopMenu->DrawWindow(0);
				gpCurTopMenu->ShowWindow(0, SHOWNEXTANDPROCPAGE);
#if 0
				if(!wParam)
				{
					//if wParam == 0, we will reinit,else we only to show
					gpCurTopMenu->Initialize(0, 0);
				}
				else
				{
					switch(lParam)
					{
						case 0:
							sprintf(gpCurTopMenu->czNavigation + 64, "%s.", gczInfo[173]);
							break;
						case 1:
							sprintf(gpCurTopMenu->czNavigation + 64, "%s..", gczInfo[173]);
							break;
						case 2:
							sprintf(gpCurTopMenu->czNavigation + 64, "%s...", gczInfo[173]);
							break;
						case 3:
							sprintf(gpCurTopMenu->czNavigation + 64, "%s....", gczInfo[173]);
							break;
						case 4:
							sprintf(gpCurTopMenu->czNavigation + 64, "%s.....", gczInfo[173]);
							break;
						case 5:
							sprintf(gpCurTopMenu->czNavigation + 64, "%s......", gczInfo[173]);
							break;
						case 7:
							sprintf(gpCurTopMenu->czNavigation + 64, "%s", gczInfo[175]);
							break;
						case 8:
							gpCurTopMenu->czNavigation[64] = 0;
							break;
						/*case 9:
							sprintf(gpCurClientView->czNavigation + 64, "%s", gczInfo[171]);
							TDMessageBox(gczInfo[171]);
							pthread_mutex_unlock(&gsViewIdleMutex);
							return;
							break;*/
						case 10:
							sprintf(gpCurTopMenu->czNavigation + 64, "%s", gczInfo[174]);
							break;
						/*case 11:
							sprintf(gpCurClientView->czNavigation + 64, "%s", gczInfo[172]);
							TDMessageBox(gczInfo[172]);
							pthread_mutex_unlock(&gsViewIdleMutex);
							return;
							break;*/
						default:
							break;
					}
				}
				gpCurTopMenu->DrawWindow(0);
				if (!gbPicIsShow && gnDouble != 3)
					gpCurTopMenu->ShowWindow(0,SHOWNEXTANDPROCPAGE);
				else
				{
					TDBitBlt(ghScrMemDC,
						gpCurTopMenu->Rect.left,
						gpCurTopMenu->Rect.top,
						gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left,
						gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
						ghScrMemDC2,
						0,
						0,
						NULL
						);
				}
#endif
				pthread_mutex_unlock(&gsViewIdleMutex);
//				gpCurTopMenu->czNavigation[64] = 0;
			}
			else
			{
				//gpCurClientView->ShowWindow(NULL, SHOWNEXTANDPROCPAGE);
			}
			/*  deleted by pengzhongkai
			gpCurRightCtrlPanel->Initialize(0, 0);
			gpCurRightCtrlPanel->DrawWindow(0);
			gpCurRightCtrlPanel->ShowWindow(NULL, 0);
			*/
		}
		else
		{
			if(gADBarInfo.nPriority <= 1)
			{
				gADBarInfo.bIsDraw = FALSE;
				gADBarInfo.nIsRefresh = ADB_SHOWTXTREFRESH;
				gADBarInfo.nPriority = 1;
			}
		}
		break;

	case REDRAWSELECTVIEW:
		if(!gbLoginPhoto)
		{
			if(gpCurTopMenu)
			{
				if(gpCurTopMenu->dwModuleID == 0x20100000 || gpCurTopMenu->dwModuleID == 0x20200000 || gpCurTopMenu->dwModuleID == 0x20300000)
				{
//#ifdef	WILLDO
					pthread_mutex_lock(&gsViewIdleMutex);
//#endif
					gpCurTopMenu->Initialize(0, 1);
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0);
//#ifdef	WILLDO
					pthread_mutex_unlock(&gsViewIdleMutex);
//#endif
				}
			}

			gBottomPanel.DrawWindow(0);
			gBottomPanel.ShowWindow(NULL, 0);
			if(gADBarInfo.nPriority <= 2)
			{
				gADBarInfo.bIsDraw = FALSE;
				gADBarInfo.nIsRefresh = ADB_SHOWTXTREFRESH;
				gADBarInfo.nPriority = 2;
			}
		}

#if 0
		if(!gbLoginPhoto)
		{
			if(!gpCurTopMenu)
			{
				if(gpCurTopMenu->dwModuleID == 0x20100000)
				{
#ifdef	WILLDO
					pthread_mutex_lock(&gsViewIdleMutex);
#endif
					gpCurTopMenu->Initialize(0, 1);
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0);
#ifdef	WILLDO
					pthread_mutex_unlock(&gsViewIdleMutex);
#endif
				}
			}

			gBottomPanel.DrawWindow(0);
			gBottomPanel.ShowWindow(NULL, 0);
			if(gADBarInfo.nPriority <= 2)
			{
				gADBarInfo.bIsDraw = FALSE;
				gADBarInfo.nIsRefresh = ADB_SHOWTXTREFRESH;
				gADBarInfo.nPriority = 2;
			}
		}
#endif
		break;
	case OPENROOM:
		gnRoomStates = ROOMSTATES_OPEN;
		nDoublePause = FALSE;
		gBottomPanel.Initialize(dwID, 0);
		gBottomPanel.DrawWindow(0);
		gBottomPanel.ShowWindow(NULL, 0);

		break;
	case CLOSEROOM:
		TDDebug("get close room message\n");
		if(lParam != 1)
		{
			STBSendInfo(CLOSEROOMCMD,0);
		}
		while(TH_HasDrawMessage(NULL))
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		while(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		pthread_mutex_unlock(&gDrawDCMutex);

		pthread_mutex_lock(&gsViewIdleMutex);
		ResetCurLButtonDownView();
#if 1

		SwitchVideo();
		sleep(1);
#else
		if(gnDouble == 3)
		{
			dwTCount = GetMyTickCount();
			nDouble = 0;
			gnDouble = 0;
			gnStaffTime = 0;
			usleep(10);
			RedrawWindow();
			sleep(1);
		}
		else
		{

			dwTCount = GetMyTickCount();
			gnDouble = 0;
			gnStaffTime = 0;
		}
#endif
		
		if(gBoxType == MAIN_BOX)
		{
			if(TDGetInBroadcast())
			{
				if(TDGetBroadcastStatus())
				{
					TDDebug("was in Broadcast so next");
					TDKTVCloseBroadcast();
					TDSetInBroadcast(0);
					TDKtvNext();
				}
			}

			{
				TDDebug("was cleanroom so we will stop record or recordplay");
				gRecordInfo.nUndoCommand = 0;
				TDSetRecordStop();
				memset(szVideoURL,0,sizeof(szVideoURL));
				memset(gszCurrentSongName,0,sizeof(gszCurrentSongName));
//				DeleteRecordInfo();
				TDSetRecordCommandType(0);
				TDSetRecordStatusType(0);
			}

			if(TDGetInBroadcast())
			{
				if(TDGetBroadcastStatus())
				{
					TDDebug("was in Broadcast so next");
					TDKTVCloseBroadcast();
					TDSetInBroadcast(0);
					TDKtvNext();
				}
			}

			DestroySongList();
			DestroyActorsChain();
#if 0
			memset(dwSelectRecipes, 0, sizeof(TDDWORD) * 2048);

			int n_note_3d_closeroom = 0;
			if( gpCurClientView == &gClientView[37] || gpCurClientView == &gClientView[49] )
			{
				n_note_3d_closeroom = 1;

			}
#endif
			TimePrompt(0, 0);

			SetCleanSign();

			nDoublePause = FALSE;
			if(gpCurClientView->dwModuleID != 0x10100000)
			{
				gnEffectState = EFFECT_FLAT_TO_IN;
				if (gpCurClientView)
				{
					gpCurClientView->Destroy();
					gpCurClientView = NULL;
				}
				gnEffectState = EFFECT_OUT_TO_FLAT;
				gpCurClientView = &gClientView[CLIENTVIEW_KARAOK];
				gpCurClientView->Create();
				gpCurClientView->Initialize(0, 0);
				gpCurClientView->DrawWindow(0);
				/*	LeftCtrlPanelLReset();
				gLeftCtrlPanel.DrawWindow(0);
				gLeftCtrlPanel.ShowWindow(NULL, 0);*/
				gRightTopPanel.Initialize(0,0);
				gRightTopPanel.DrawWindow(0);
				gRightTopPanel.ShowWindow(0,0);
				gpCurClientView->ShowWindow(NULL, SHOW3D);
			}
			if(ActorPage.bEnabled)
				memset(&ActorPage, 0, sizeof(PAGESTACK));
			if(PageStack.bEnabled)
				memset(&PageStack, 0, sizeof(PAGESTACK));

			strcpy(gADBarInfo.szNewTxt, "");
			gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
			gADBarInfo.btPauseCount = 0;
			gnProcessRoom = 0;
			gnIsInCleanRoom = 0;
			gnDelayMessage = 0;
			if(wParam != 1)
			{
				gnRoomStates = ROOMSTATES_CLOSE;
				//yang add for new drink
				ClearSelectList();
			}
	/*		if(!gbLoginPhoto)
			{
				if((gpCurExpandView->dwModuleID & 0xFFF00000) == 0x20100000)
				{
					gpCurExpandView->Initialize(0, 1);
					gpCurExpandView->DrawWindow(0);
					gpCurExpandView->ShowWindow(NULL, 0);
				}

				gExpandViewCtrl.DrawWindow(0);
				gExpandViewCtrl.ShowWindow(NULL, 0);
			}*/

			ClientCloseRoom();
		}
		else
		{
			if(gpCurTopMenu != NULL)
			{
				gpCurTopMenu->Destroy();
				gpCurTopMenu = NULL;
			/*	LeftCtrlPanelLReset();
				gLeftCtrlPanel.DrawWindow(0);
				gLeftCtrlPanel.ShowWindow(NULL, 0);*/
			}

			TDDebug("close room bbbbbbbbbbbbb\n");
				//	ViewHandleLBtnUpShell(0x101);
				///////////////yanbo add for close room

			if(gpCurClientView->dwModuleID != 0x10100000)
			{
				gnEffectState = EFFECT_FLAT_TO_IN;
				if (gpCurClientView)
				{
					gpCurClientView->Destroy();
					gpCurClientView = NULL;
				}
				gnEffectState = EFFECT_OUT_TO_FLAT;
				gpCurClientView = &gClientView[CLIENTVIEW_KARAOK];
				gpCurClientView->Create();
				gpCurClientView->Initialize(0, 0);
				gpCurClientView->DrawWindow(0);
				gRightTopPanel.Initialize(0,0);
				gRightTopPanel.DrawWindow(0);
				gRightTopPanel.ShowWindow(0,0);
				gpCurClientView->ShowWindow(NULL, SHOW3D);
			}
				///yanbo add end
			if(ActorPage.bEnabled)
				memset(&ActorPage, 0, sizeof(PAGESTACK));
			if(PageStack.bEnabled)
				memset(&PageStack, 0, sizeof(PAGESTACK));

			strcpy(gADBarInfo.szNewTxt, "");
			gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
			gADBarInfo.btPauseCount = 0;
		}
		memset(szMyLibNO, 0, sizeof(szMyLibNO));
		pthread_mutex_unlock(&gsViewIdleMutex);

		PauseThreadLock();
		gBottomPanel.Initialize(dwID, 0);
		gBottomPanel.DrawWindow(0);
		gBottomPanel.ShowWindow(NULL, 0);
		ResumeThreadLock();

		break;
	case WM_REDRAWBROADCASTVIEW:
#ifdef	WILLDO
		if(TH_HasDrawMessage(NULL))
		{
			break ;
		}
		if(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
		{
			break;
		}
		pthread_mutex_unlock(&gDrawDCMutex);
		//WaitForMessageClean(0);
		TDDebug("RedrawBroadCastView1111111111");
		if(gpCurExpandView)
		{
		TDDebug("RedrawBroadCastView1111111112");
			if((gpCurExpandView->dwModuleID & 0xFFF00000) == 0x20300000)
			{
		TDDebug("RedrawBroadCastView1111111113");
				gpCurExpandView->DrawWindow(0);
		TDDebug("RedrawBroadCastView1111111114");
				if(gnDouble != 3)
					gpCurExpandView->ShowWindow(NULL, 0);
		TDDebug("RedrawBroadCastView1111111115");
			}
		TDDebug("RedrawBroadCastView1111111116");
		}
		TDDebug("RedrawBroadCastView1111111117");
#endif
		break;
	case WM_CHANGEROOM:
		{
			int nTotal1;
			int nTotal2;
			char szDebug[100];
			nTotal1 = GetSongListTotalCount(FALSE);
			sprintf(szDebug,"nTotal1 = %d",nTotal1);
			TDDebug(szDebug);

			ReCreateSongList(nTotal1);

			nTotal2 = GetSongListTotalCount(FALSE);
			sprintf(szDebug,"nTotal2 = %d",nTotal2);
			TDDebug(szDebug);
			if(nTotal2 > 0 && nTotal1 == 0)
			{
				gbIsClearRoom = FALSE;
			}
			gnRoomStates = ROOMSTATES_OPEN;
		}
		//	if(gpCurClientView)
		//		ShowSelectedSongCount();
#ifdef	WILLDO
		if(!gbLoginPhoto)
		{

			if((gpCurExpandView->dwModuleID & 0xFFF00000) == 0x20100000)
			{
				gpCurExpandView->Initialize(0, 1);
				gpCurExpandView->DrawWindow(0);
				gpCurExpandView->ShowWindow(NULL, 0);
			}

			gExpandViewCtrl.DrawWindow(0);
			gExpandViewCtrl.ShowWindow(NULL, 0);
		}
#endif
#if 1
		SwitchVideo();
		sleep(1);
#else
		RedrawWindow();
#endif
		gnProcessRoom = 0;
		break;

	case WM_SAVEADTIMES:
		break;
	case WM_CLEANROOM:
#if 1//add fangyuehan
		TDDebug("get clean room message\n");
		while(TH_HasDrawMessage(NULL))
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		while(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		pthread_mutex_unlock(&gDrawDCMutex);

		pthread_mutex_lock(&gsViewIdleMutex);
		ResetCurLButtonDownView();
		if (!gnIsInCleanRoom)
		{
#if 1
			SwitchVideo();
#else
			if(gnDouble == 3)
			{
				dwTCount = GetMyTickCount();
				nDouble = 0;
				gnDouble = 0;
				gnStaffTime = 0;
				usleep(10);
				RedrawWindow();
			}
			else
			{
				dwTCount = GetMyTickCount();
				gnDouble = 0;
				gnStaffTime = 0;
			}
#endif

			//SwitchVideo();
	/*		if (gbUsePic)
			{
				SetPicShowStatus(FALSE);
			}*/
			gnRoomStates = ROOMSTATES_CLOSE;
		/*	pthread_mutex_lock(&gLinkGameMutex);
			bLinkGameStart = TDFALSE;
			nLinkStartTime = 0;
			pthread_mutex_unlock(&gLinkGameMutex);*/
			if(TDGetInBroadcast())
			{
				if(TDGetBroadcastStatus())
				{
					TDDebug("clean was in Broadcast so next");
					TDKTVCloseBroadcast();
					TDSetInBroadcast(0);
					TDKtvNext();
				}
			}
			{
				TDDebug("was cleanroom so we will stop record or recordplay");
				TDSetRecordStop();
				memset(szVideoURL,0,sizeof(szVideoURL));
				memset(gszCurrentSongName,0,sizeof(gszCurrentSongName));
			//	DeleteRecordInfo();
			}
			Sleep(1000);     //if clean room stop game rate <msl 2006.09.29>

			nDoublePause = TRUE;
			gnIsInCleanRoom = 1;
			sprintf(cmdbuf,"STBCMD:%2d: TIME:%d",CLEANROOM,0);
			STBSendUDPMsg(cmdbuf);
			STBSendUDPMsg(cmdbuf); //msl

			if (gpCurTopMenu)
			{
				gpCurTopMenu->Destroy();
				gpCurTopMenu = NULL;
/*				LeftCtrlPanelLReset();
				gLeftCtrlPanel.DrawWindow(0);
				gLeftCtrlPanel.ShowWindow(NULL, 0);*/
			}
			TDDebug("Show Before");
			gpCurTopMenu = &gTopMenu[TOPMENUVIEW_LOGIN];
			gpCurTopMenu->Create();
			gpCurTopMenu->Initialize(0, 0);
			gpCurTopMenu->dwModuleID = MODULE_TOPMENU_LOGIN_CLEANROOM;
			gpCurTopMenu->DrawWindow(0);
			//gpCurTopMenu->ShowWindow(NULL, 0);
			gpCurTopMenu->ShowWindow(NULL, SHOW_TOPMENUTOTOPMENU);
			TDDebug("Show After");

			gnProcessRoom = 0;
		}
		else
		{
			TDDebug("is In CleanRoom,so we will not recv WM_CLEANROOM");
		}
		pthread_mutex_unlock(&gsViewIdleMutex);
#endif
		break;


case WM_CLEANCLIENT:
//        printf("CLEANCLIENT\n\n");
		TDDebug("get cleanclient message\n");
		while(TH_HasDrawMessage(NULL))
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		while(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		pthread_mutex_unlock(&gDrawDCMutex);

		pthread_mutex_lock(&gsViewIdleMutex);
		ResetCurLButtonDownView();
#if 1
		SwitchVideo();
		sleep(1);
#else
		if(gnDouble == 3)
		{
			dwTCount = GetMyTickCount();
			nDouble = 0;
			gnDouble = 0;
			gnStaffTime = 0;
			usleep(10);
			RedrawWindow();
			sleep(1);
		}
		else
		{
			dwTCount = GetMyTickCount();
			gnDouble = 0;
			gnStaffTime = 0;
		}
#endif
		if(gBoxType == MAIN_BOX)  //主机顶盒
		{
			if(TDGetInBroadcast())  //系统广播
			{
//				printf("系统广播中\n\n");
				if(TDGetBroadcastStatus())
				{   
					TDDebug("was in Broadcast so next");
					TDKTVCloseBroadcast();
					TDSetInBroadcast(0);
					TDKtvNext();
				}
			}

			{
				//清扫房间  清空记录和停止播歌
				TDDebug("was cleanroom so we will stop record or recordplay");
				gRecordInfo.nUndoCommand = 0;
				TDSetRecordStop();
				memset(szVideoURL,0,sizeof(szVideoURL));   //为新申请的内存做初始化，初始化为 0
				memset(gszCurrentSongName,0,sizeof(gszCurrentSongName));
				TDSetRecordCommandType(0);
				TDSetRecordStatusType(0);
			}

			if(TDGetInBroadcast())  //广播
			{
				if(TDGetBroadcastStatus())
				{
					TDDebug("was in Broadcast so next");
					TDKTVCloseBroadcast();
					TDSetInBroadcast(0);
					TDKtvNext();
				}
			}
			DestroySongList();  //清空列表

			DestroyActorsChain();

            TDKtvNext();  //取消一首。这样电视播放的就切换到默认的MTV
			TimePrompt(0, 0);  //清掉到时提醒
			nDoublePause = FALSE;  //双路暂停，电视跟触摸屏

			if(gpCurClientView->dwModuleID != 0x10100000)
			{
				gnEffectState = EFFECT_FLAT_TO_IN;
				if (gpCurClientView)
				{
					gpCurClientView->Destroy();
					gpCurClientView = NULL;
				}
				gnEffectState = EFFECT_OUT_TO_FLAT;
				gpCurClientView = &gClientView[CLIENTVIEW_KARAOK];  //原屏幕清除
				gpCurClientView->Create();
				gpCurClientView->Initialize(0, 0);
				gpCurClientView->DrawWindow(0);
				
				gRightTopPanel.Initialize(0,0);  //主界面出来
				gRightTopPanel.DrawWindow(0);
				gRightTopPanel.ShowWindow(0,0);
				gpCurClientView->ShowWindow(NULL, SHOW3D);
			}
			if(ActorPage.bEnabled)
				memset(&ActorPage, 0, sizeof(PAGESTACK));
			if(PageStack.bEnabled)
				memset(&PageStack, 0, sizeof(PAGESTACK));

			strcpy(gADBarInfo.szNewTxt, "");
			gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
			gADBarInfo.btPauseCount = 0;
			gnDelayMessage = 0;
            
            gnIsInCleanClient = 1;
		}
		else
		{
			if(gpCurTopMenu != NULL)
			{
				gpCurTopMenu->Destroy();
				gpCurTopMenu = NULL;
			}
#ifdef   WILLDO
			if( gpCurClientView == &gClientView[37] || gpCurClientView == &gClientView[49] )
			{
				if (gpCurClientView)
					gpCurClientView->Destroy();

				gpCurClientView = &gClientView[CLIENTVIEW_KARAOK];
				gpCurClientView->Create();
	    		gpCurClientView->Initialize(0, 0);
	    		gpCurClientView->DrawWindow(0);

				TDBitBlt(ghDC,
						0,
						0,
						838,
						653,
						ghScrMemBackgroundDC, 
						0,
						0,
						0x0
						);
				LeftCtrlPanelLReset();
				gLeftCtrlPanel.DrawWindow(0);
				gLeftCtrlPanel.ShowWindow(NULL, 0);
				gpCurClientView->ShowWindow(NULL, SHOW_BOXIN);
			}
			else
#endif
			if(gpCurClientView->dwModuleID != 0x10100000)
			{
				gnEffectState = EFFECT_FLAT_TO_IN;
				if (gpCurClientView)
				{
					gpCurClientView->Destroy();
					gpCurClientView = NULL;
				}
				gnEffectState = EFFECT_OUT_TO_FLAT;
				gpCurClientView = &gClientView[CLIENTVIEW_KARAOK];
				gpCurClientView->Create();
				gpCurClientView->Initialize(0, 0);
				gpCurClientView->DrawWindow(0);
				gRightTopPanel.Initialize(0,0);
				gRightTopPanel.DrawWindow(0);
				gRightTopPanel.ShowWindow(0,0);
				gpCurClientView->ShowWindow(NULL, SHOW3D);
			}
			
			if(ActorPage.bEnabled)
				memset(&ActorPage, 0, sizeof(PAGESTACK));
			if(PageStack.bEnabled)
				memset(&PageStack, 0, sizeof(PAGESTACK));

			strcpy(gADBarInfo.szNewTxt, "");
			gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
			gADBarInfo.btPauseCount = 0;
		}
		memset(szMyLibNO, 0, sizeof(szMyLibNO));
		pthread_mutex_unlock(&gsViewIdleMutex);

		PauseThreadLock();
		gBottomPanel.Initialize(dwID, 0);  //底部工具条
		gBottomPanel.DrawWindow(0);
		gBottomPanel.ShowWindow(NULL, 0);
		ResumeThreadLock(); 
		break;
  case WM_CANCLECLEANCLIENT:
//        printf("CANCLE CLEANCLIENT\n\n");
		gnIsInCleanClient = 0;  //取消清客标志
		nDoublePause = FALSE;
		gBottomPanel.Initialize(dwID, 0);
		break;



	case WM_TIMEPROMPT:
		TDDebug("enter WM_TIMEPROMPT!!!!!\n");
		printf("enter WM_TIMEPROMPT!!!!!\n");
#if 1
		SwitchVideo();
#else
		if(gnDouble == 3)
		{
			dwTCount = GetMyTickCount();
			nDouble = 0;
			gnDouble = 0;
			gnStaffTime = 0;
			usleep(10);
			RedrawWindow();
		}
		else
		{
			dwTCount = GetMyTickCount();
			gnDouble = 0;
			gnStaffTime = 0;
		}
#endif
//		if(gnRoomStates != ROOMSTATES_CLOSE)
		{
			TDDebug("enter TimePrompt(LOWORD(wParam), HIWORD(wParam))\n");
			TimePrompt(LOWORD(wParam), HIWORD(wParam));
		}
		break;
	case WM_MEDIAKTVOK:
		{
			TDPString strText;
			TDPString strIndex;
			TDPString strMediaID;
			TDPString strMediaName;
			TDPString strMediaType;
			TDPString strMediaGroup;
			TDPString strMessage;
			TDPString strSonger1;
			TDPString strSonger2;
			TDPString strLang;
			TDPString strSort;

			int nLang = -1;
			char *pText = NULL;
			TDBOOL bResult = FALSE;
			char * page = (char *)wParam;
			char	*pActorName = NULL;//

			TDDebug(page);
			strText = TDString_Create();
			strIndex = TDString_Create();
			strMediaID = TDString_Create();
			strMediaName = TDString_Create();
			strMediaType = TDString_Create();
			strMediaGroup = TDString_Create();
			strMessage = TDString_Create();
			strSonger1 = TDString_Create();
			strSonger2 = TDString_Create();
			strLang = TDString_Create();
			strSort = TDString_Create();

			TDDebug(page);
			TDString_AppendPChar(strText, page);
			TDKtvGetValueFromResponse(strText, strIndex, "nIndex");
			TDKtvGetValueFromResponse(strText, strMediaID, "nInSongID");
			TDKtvGetValueFromResponse(strText, strMediaName, "pInSongName");

			if(TDKtvGetValueFromResponse(strText, strMessage, "nText"))
				pText = strMessage->mStr;

			if(TDKtvGetValueFromResponse(strText, strLang, "nTextLang"))
				nLang = atoi(strLang->mStr);

			if(TDKtvGetValueFromResponse(strText, strSonger1, "sSongerName1"))
			{
				pActorName = strSonger1->mStr;
			}
			TDKtvGetValueFromResponse(strText, strMediaGroup, "btMediaGroup");

			if(TDKtvGetValueFromResponse(strText, strSort, "sSort"))
			{
				if(TDKtvGetValueFromResponse(strText, strMediaType, "btMediaType"))
				{
					bResult = InsertSendSong(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, strMediaType->mStr[0], pText, nLang, pActorName/*strSonger1->mStr*/,atoi(strMediaGroup->mStr) & 0xFF);
				}
				else
				{
					bResult = InsertSendSong(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, '\0', pText, nLang, pActorName/*strSonger1->mStr*/,atoi(strMediaGroup->mStr) & 0xFF);
				}
			}
			else
			{
				if(TDKtvGetValueFromResponse(strText, strMediaType, "btMediaType"))
					bResult = AppendMedia(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, strMediaType->mStr[0], pText, nLang, pActorName/*strSonger1->mStr*/,atoi(strMediaGroup->mStr) & 0xFF);
				else
					bResult = AppendMedia(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, '\0', pText, nLang, pActorName/*strSonger1->mStr*/,atoi(strMediaGroup->mStr) & 0xFF);
			}

			TDDebug(strIndex->mStr);
			TDDebug(strMediaID->mStr);
			TDDebug(strMediaName->mStr);
			TDDebug(strMessage->mStr);
			TDDebug(strLang->mStr);
			TDDebug(strMediaID->mStr);
			TDDebug(strMediaType->mStr);

			if(bResult)
			{
				//append actor
				if(TDKtvGetValueFromResponse(strText, strSonger1, "sSongerName1"))
					InsertActors2Chain(strSonger1->mStr);
				if(TDKtvGetValueFromResponse(strText, strSonger2, "sSongerName2"))
					InsertActors2Chain(strSonger2->mStr);
			 }
			if(!gbLoginPhoto)
			{
				if(gpCurTopMenu && (gpCurTopMenu->dwTopID == MODULE_TOPMENU_SELECTED))
				{
					gpCurTopMenu->Initialize(0, 1);
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0);
				}

				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
			}
			TDString_Destroy(strIndex);
			TDString_Destroy(strText);
			TDString_Destroy(strMediaID);
			TDString_Destroy(strMediaName);
			TDString_Destroy(strMediaType);
			TDString_Destroy(strMediaGroup);
			TDString_Destroy(strMessage);
			TDString_Destroy(strLang);
			TDString_Destroy(strSort);
			free(page);

			break;
		}
	case MEDIACLIENTAPPEND:
		{
			TDPString strText;
			TDPString strIndex;
			TDPString strMediaID;
			TDPString strMediaName;
			TDPString strMediaType;
			TDPString strMediaGroup;
			TDPString strMessage;
			TDPString strSonger1;
			TDPString strSonger2;
			TDPString strLang;

			int nLang = -1;
			char *pText = NULL;
			char * page = (char *)wParam;
			TDBOOL bResult = FALSE;

			TDDebug(page);
			strText = TDString_Create();
			strIndex = TDString_Create();
			strMediaID = TDString_Create();
			strMediaName = TDString_Create();
			strMediaType = TDString_Create();
			strMediaGroup = TDString_Create();
			strMessage = TDString_Create();
			strSonger1 = TDString_Create();
			strSonger2 = TDString_Create();
			strLang = TDString_Create();

			TDDebug(page);
			TDString_AppendPChar(strText, page);
			TDKtvGetValueFromResponse(strText, strIndex, "nIndex");
			TDKtvGetValueFromResponse(strText, strMediaID, "nInSongID");
			TDKtvGetValueFromResponse(strText, strMediaName, "pInSongName");

			if(TDKtvGetValueFromResponse(strText, strMessage, "nText"))
				pText = strMessage->mStr;

			if(TDKtvGetValueFromResponse(strText, strLang, "nTextLang"))
				nLang = atoi(strLang->mStr);

			TDKtvGetValueFromResponse(strText, strSonger1, "pActorName");
			TDKtvGetValueFromResponse(strText, strMediaGroup, "btMediaGroup");

			if(TDKtvGetValueFromResponse(strText, strMediaType, "btMediaType"))
				bResult = AppendMedia(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, strMediaType->mStr[0], pText, nLang,strSonger1->mStr, strMediaGroup->mStr[0]);
			else
				bResult = AppendMedia(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, '\0', pText, nLang, strSonger1->mStr, strMediaGroup->mStr[0]);

			TDDebug(strIndex->mStr);
			TDDebug(strMediaID->mStr);
			TDDebug(strMediaName->mStr);
			TDDebug(strMessage->mStr);
			TDDebug(strLang->mStr);
			TDDebug(strMediaID->mStr);
			TDDebug(strMediaType->mStr);

			if(!gbLoginPhoto)
			{
				if(gpCurTopMenu && (gpCurTopMenu->dwModuleID & 0xFFF00000) == 0x20100000)
				{
					gpCurTopMenu->Initialize(0, 1);
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0);
				}

				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
			}
			TDString_Destroy(strIndex);
			TDString_Destroy(strText);
			TDString_Destroy(strMediaID);
			TDString_Destroy(strMediaName);
			TDString_Destroy(strMediaType);
			TDString_Destroy(strMediaGroup);
			TDString_Destroy(strMessage);
			TDString_Destroy(strLang);

			TDString_Destroy(strSonger1);
			TDString_Destroy(strSonger2);
			free(page);
			break;
		}
		break;
	case WM_INDUCECARD:
		{
			char * strcard = (char *)wParam;
			if(TH_HasDrawMessage(NULL))
			{
				if(strcard)
				{
					free(strcard);
					strcard = NULL;
				}
				break;
			}
			if(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
			{
				if(strcard)
				{
					free(strcard);
					strcard = NULL;
				}
				break;
			}
			pthread_mutex_unlock(&gDrawDCMutex);

			if(gnTouchState == TOUCHDOWN)
			{
				if(strcard)
				{
					free(strcard);
					strcard = NULL;
				}
				break;
			}
			if(!(TDGetInBroadcast()) && !(TDGetBroadcastStatus()))
			{
				pthread_mutex_lock(&gsViewIdleMutex);
				SwitchVideo();
				TransactCardID(strcard);
				pthread_mutex_unlock(&gsViewIdleMutex);
			}
			if(strcard)
			{
				free(strcard);
				strcard = NULL;
			}
		}
		break;
#ifdef	WILLDO
	case WM_DRINKMSG_CMD:
		gnStaffTime = 0;
		if(((int)((gpCurClientView->dwModuleID & 0xF0000000) >> 28)) == 0x3) //酒水管理所有页面
		{
			if(strlen(gszUserSerialNO) > 0) //如果当前账号变量存有工号
			{
				SwitchVideo();
				gszUserSerialNO[0] = 0;
				if(gpCurTopMenu)
				{
					gpCurTopMenu->Destroy();
					gpCurTopMenu = NULL;
				}
				ViewHandleLBtnUpShell(0x310);
				TDMessageBox(gczInfo[168]);
			}
		}
		break;
#endif

	case WM_UDP_GETVERTIONIP:
		{
		char* pTmpBuf = (char*)wParam;
		int DeviceIP = 0;
		int	CheckSum = 0;
		int VerifyNum = 0;
		char strVertionIP[16];
		PUDPSOCKET	pCurUDPSocket = NULL;
		PUDPSOCKET	pPreUDPSocket = NULL;
        PUDPSOCKET	pNewUDPSocket = NULL;
		TDBOOL  VertionAddExist = FALSE;

		sscanf(pTmpBuf+2,":DeviceIP:%d CheckSum:%d",&DeviceIP,&CheckSum);
    
		VerifyNum = 0;
		VerifyNum += (DeviceIP) & 0xff;
		VerifyNum += (DeviceIP >> 8) & 0xff;
		VerifyNum += (DeviceIP >> 16) & 0xff;
		VerifyNum += (DeviceIP >> 24) & 0xff;
        printf("VERTION IP:  device: %d    checknum:%d    verify:%d\n",DeviceIP,CheckSum,VerifyNum);
		
		if(VerifyNum == CheckSum){//解析成功
			sprintf(strVertionIP,"%d.%d.%d.%d",(DeviceIP) & 0xff,(DeviceIP >> 8) & 0xff,(DeviceIP >> 16) & 0xff,(DeviceIP >> 24) & 0xff);
			//printf("IP: %s\n",strVertionIP);
            
			if(TagUDPSocketList->pNext == NULL){
				pNewUDPSocket = malloc(sizeof(UDPSOCKET));
				memset(pNewUDPSocket, 0, sizeof(UDPSOCKET));
				TagUDPSocketList->pNext = pNewUDPSocket;
            	pCurUDPSocket = pNewUDPSocket;
				strcpy(pCurUDPSocket->UDPSocketIP,strVertionIP);
				pCurUDPSocket->UDPSocketD = 1;
				pCurUDPSocket->UDPSocketAdd.sin_family = AF_INET;
				pCurUDPSocket->UDPSocketAdd.sin_port = htons(2008);
				pCurUDPSocket->UDPSocketAdd.sin_addr.s_addr = inet_addr(strVertionIP);
				
			}else{
                pCurUDPSocket = TagUDPSocketList->pNext;
				while (pCurUDPSocket)
				{
					printf("遍历SOCKET: %s\n",pCurUDPSocket->UDPSocketIP);
					if (strcmp(pCurUDPSocket->UDPSocketIP,strVertionIP) != 0)
					{
                        pCurUDPSocket = pCurUDPSocket->pNext;
						continue;
					}else{
						VertionAddExist = TRUE;
						pCurUDPSocket->UDPSocketD ++;
						break;
					}

					
				} 
				
				if (!VertionAddExist)
				{
					pNewUDPSocket = malloc(sizeof(UDPSOCKET));
					memset(pNewUDPSocket, 0, sizeof(UDPSOCKET));
					pNewUDPSocket->pNext = TagUDPSocketList->pNext;
					TagUDPSocketList->pNext = pNewUDPSocket;
					pCurUDPSocket = pNewUDPSocket;
					strcpy(pCurUDPSocket->UDPSocketIP,strVertionIP);
					pCurUDPSocket->UDPSocketD = 0;
					pCurUDPSocket->UDPSocketAdd.sin_family = AF_INET;
					pCurUDPSocket->UDPSocketAdd.sin_port = htons(2008);
					pCurUDPSocket->UDPSocketAdd.sin_addr.s_addr = inet_addr(strVertionIP);
				}
		}
        
		//printf("TimeEEEEE:%d\n",GetTickCount() - CheckupVertionTime);
		//CheckupVertionTime = GetTickCount();
	    

		pCurUDPSocket = TagUDPSocketList->pNext;
    	while (pCurUDPSocket)
		{
			printf("HHHHHSOCKET: %s\n",pCurUDPSocket->UDPSocketIP);
			pCurUDPSocket = pCurUDPSocket->pNext;
		}
		}
		break;
		}
	case WM_UDP_REMOVEVERTIONIP:
		{
			char* pTmpBuf = (char*)wParam;
			int DeviceIP = 0;
			int	CheckSum = 0;
			int VerifyNum = 0;
			char strVertionIP[16];
			PUDPSOCKET	pCurUDPSocket = NULL;
			PUDPSOCKET	pPreUDPSocket = NULL;
			PUDPSOCKET	pOldUDPSocket = NULL;
			TDBOOL  VertionAddExist = FALSE;
			
			sscanf(pTmpBuf+2,":DeviceIP:%d CheckSum:%d",&DeviceIP,&CheckSum);
			
			VerifyNum = 0;
			VerifyNum += (DeviceIP) & 0xff;
			VerifyNum += (DeviceIP >> 8) & 0xff;
			VerifyNum += (DeviceIP >> 16) & 0xff;
			VerifyNum += (DeviceIP >> 24) & 0xff;
		//	printf("REMOVEVERTION IP:  device: %d    checknum:%d    verify:%d\n",DeviceIP,CheckSum,VerifyNum);
			
			if(VerifyNum == CheckSum){//解析成功
				sprintf(strVertionIP,"%d.%d.%d.%d",(DeviceIP) & 0xff,(DeviceIP >> 8) & 0xff,(DeviceIP >> 16) & 0xff,(DeviceIP >> 24) & 0xff);
				//printf("IP: %s\n",strVertionIP);
				if (TagUDPSocketList->pNext != NULL)
				{
					pCurUDPSocket = TagUDPSocketList->pNext;
					pPreUDPSocket = TagUDPSocketList;
					
					while (pCurUDPSocket)
					{
						if(strcmp(pCurUDPSocket->UDPSocketIP,strVertionIP) == 0){//说明没有接收到该ip的心跳脉冲,从链表中删除
							printf("removeIP: %s\n",pCurUDPSocket->UDPSocketIP);
							pOldUDPSocket = pCurUDPSocket;
							pPreUDPSocket->pNext = pCurUDPSocket->pNext;
							pCurUDPSocket = pCurUDPSocket->pNext;
							free(pOldUDPSocket);
							break;
						}else{
							pCurUDPSocket->UDPSocketD = 0;
							pPreUDPSocket = pCurUDPSocket;
							pCurUDPSocket = pCurUDPSocket->pNext;
						}
						
					}
					
					pCurUDPSocket = TagUDPSocketList->pNext;
					while (pCurUDPSocket)
					{
						printf("allIP: %s\n",pCurUDPSocket->UDPSocketIP);
						pCurUDPSocket = pCurUDPSocket->pNext;
					}
				}
			}
		break;
		}
	case WM_UDP_APPENDMEDIA:
		{
			TDPString strText;
			TDPString strIndex;
			TDPString strMediaID;
			TDPString strMediaName;
			TDPString strMediaType;
			TDPString strMediaGroup;
			TDPString strMessage;
			TDPString strSonger1;
			TDPString strSonger2;
			TDPString strLang;
			TDPString strSort;

			int nLang = -1;
			char *pText = NULL;
			TDBOOL bResult = FALSE;
			char * page = (char *)wParam;
			char	*pActorName = NULL;//

			TDDebug(page);
			SwitchVideo();
			strText = TDString_Create();
			strIndex = TDString_Create();
			strMediaID = TDString_Create();
			strMediaName = TDString_Create();
			strMediaType = TDString_Create();
			strMediaGroup = TDString_Create();
			strMessage = TDString_Create();
			strSonger1 = TDString_Create();
			strSonger2 = TDString_Create();
			strLang = TDString_Create();
			strSort = TDString_Create();

			TDDebug(page);
			TDString_AppendPChar(strText, page);
			TDKtvGetValueFromResponse(strText, strIndex, "nIndex");
			TDKtvGetValueFromResponse(strText, strMediaID, "nInSongID");
			TDKtvGetValueFromResponse(strText, strMediaName, "pInSongName");

			if(TDKtvGetValueFromResponse(strText, strMessage, "nText"))
				pText = strMessage->mStr;

			if(TDKtvGetValueFromResponse(strText, strLang, "nTextLang"))
				nLang = atoi(strLang->mStr);

			if(TDKtvGetValueFromResponse(strText, strSonger1, "sSongerName1"))
			{
				pActorName = strSonger1->mStr;
			}
			TDKtvGetValueFromResponse(strText, strMediaGroup, "btMediaGroup");

			if(TDKtvGetValueFromResponse(strText, strSort, "sSort"))
			{
				if(TDKtvGetValueFromResponse(strText, strMediaType, "btMediaType"))
				{
					bResult = InsertSendSong(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, atoi(strMediaType->mStr), pText, nLang, pActorName/*strSonger1->mStr*/,atoi(strMediaGroup->mStr) & 0xFF);
				}
				else
				{
					bResult = InsertSendSong(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, '\0', pText, nLang, pActorName/*strSonger1->mStr*/,atoi(strMediaGroup->mStr) & 0xFF);
				}
			}
			else
			{
				if(TDKtvGetValueFromResponse(strText, strMediaType, "btMediaType"))
					bResult = AppendMedia(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, atoi(strMediaType->mStr), pText, nLang, pActorName/*strSonger1->mStr*/,atoi(strMediaGroup->mStr) & 0xFF);
				else
					bResult = AppendMedia(atoi(strIndex->mStr), atoi(strMediaID->mStr), strMediaName->mStr, '\0', pText, nLang, pActorName/*strSonger1->mStr*/,atoi(strMediaGroup->mStr) & 0xFF);
			}

			TDDebug(strIndex->mStr);
			TDDebug(strMediaID->mStr);
			TDDebug(strMediaName->mStr);
			TDDebug(strMessage->mStr);
			TDDebug(strLang->mStr);
			TDDebug(strMediaID->mStr);
			TDDebug(strMediaType->mStr);


			if(bResult)
			{
				//append actor
				if(TDKtvGetValueFromResponse(strText, strSonger1, "sSongerName1"))
					InsertActors2Chain(strSonger1->mStr);
				if(TDKtvGetValueFromResponse(strText, strSonger2, "sSongerName2"))
					InsertActors2Chain(strSonger2->mStr);
			 }
			if(!gbLoginPhoto)
			{
				if(gpCurTopMenu && (gpCurTopMenu->dwTopID == MODULE_TOPMENU_SELECTED))
				{
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, SHOWNEXTANDPROCPAGE);
				}
				PauseThreadLock();
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}
			TDString_Destroy(strIndex);
			TDString_Destroy(strText);
			TDString_Destroy(strMediaID);
			TDString_Destroy(strMediaName);
			TDString_Destroy(strMediaType);
			TDString_Destroy(strMediaGroup);
			TDString_Destroy(strMessage);
			TDString_Destroy(strLang);
			TDString_Destroy(strSort);
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}

		}
		break;
	case WM_UDP_GETSONGLIST:
		{
			STBSendInfo(GETSONGLIST,0);
			STBSendInfo(MODIFYVOL,0);
			STBSendInfo(MUTESOUND,0);
			STBSendInfo(MODIFYTONE,0);
			STBSendInfo(MODIFYMIC,0);
			STBSendInfo(YUANBANDAO,0);
			STBSendInfo(PAUSEPLAY,0);
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_GETCURSONG:
		{
			STBSendInfo(GETCURSONG,0);
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_INSERT:
		{
			char* pTmpBuf = (char*)wParam;
			int nIndex = 0;
			int nClientID = 0;
			int	nNewIndex = 0;
			SwitchVideo();
			sscanf(pTmpBuf+2,":nIndex:%d nNewIndex:%d ClientID:%d",&nIndex,&nNewIndex,&nClientID);
			InsertMedia(nIndex,nNewIndex,gbNeedPlayed);
			if(!gbLoginPhoto)
			{
				if(gpCurTopMenu && (gpCurTopMenu->dwTopID & 0xFFF0000) == MODULE_TOPMENU_SELECTED)
				{
				
					gpCurTopMenu->Initialize(gpCurTopMenu->lCurData, 0);
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, SHOWNEXTANDPROCPAGE);
				}

				PauseThreadLock();
				gBottomPanel.Initialize(0, 0);
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}

			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_DELETE:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			int nIndex = 0;
			SwitchVideo();
			sscanf(pTmpBuf+2,":nIndex:%d ClientID:%d",&nIndex,&nClientID);
			DeleteMedia(nIndex,gbNeedPlayed);

			if(!gbLoginPhoto)
			{
				if(gpCurTopMenu && (gpCurTopMenu->dwTopID & 0xFFF0000) == MODULE_TOPMENU_SELECTED)
				{
					gpCurTopMenu->Initialize(gpCurTopMenu->lCurData, 0);
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, SHOWNEXTANDPROCPAGE);
				}
				PauseThreadLock();
				gBottomPanel.Initialize(0, 0);
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_NEXT:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			TDDebug(pTmpBuf);
			sscanf(pTmpBuf+2,":ClientID:%d",&nClientID);
			TDKtvNext();
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_REPLAY:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			TDDebug(pTmpBuf);
			sscanf(pTmpBuf+2,":ClientID:%d",&nClientID);
			if(TDGetRecordStatusType() != 2)
			{
				gBottomPanel.i64DgtInfo = 0;
				TDKtvReplay(0);
				gBottomPanel.Initialize(0, 0);
				STBSendInfo(PAUSEPLAY, 0);
				if(gpCurClientView && gpCurClientView->dwModuleID == 0x40200000)
					SendNotifyMessage(NULL, REDRAWCURRENTVIEW, NULL, NULL);
			}
			PauseThreadLock();
			gBottomPanel.DrawWindow(0);
			gBottomPanel.ShowWindow(NULL, 0);
			ResumeThreadLock();
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_PAUSEPLAY:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			TDDebug(pTmpBuf);
			sscanf(pTmpBuf+2,":ClientID:%d",&nClientID);
			if((TDGetRecordCommandType() != 1) && (TDGetRecordCommandType() != 2))
			{
				gBottomPanel.i64DgtInfo &= 0xFFE7;
				gBottomPanel.i64DgtInfo |= 0x1;
				TDKtvPause();
				PauseThreadLock();
				gBottomPanel.Initialize(0, 0);
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_QUERYPAUSEPLAY:
		if(wParam)
		{
			free(wParam);
			wParam = NULL;
		}
		break;
	case WM_UDP_YUANBANDAO:
		{
			if(TDGetRecordCommandType() != 2)
			{
				gBottomPanel.i64DgtInfo |= 0x4;
				TDKtvAudioSwitch(NULL);
				gADBarInfo.bIsDraw = FALSE;
				if(AudioChannelStatus() == 1)
					strcpy(gADBarInfo.szNewTxt, gczInfo[78]);
				else if(AudioChannelStatus() == 3)
					strcpy(gADBarInfo.szNewTxt, gczInfo[106]);
				else
					strcpy(gADBarInfo.szNewTxt, gczInfo[77]);
				gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
				PauseThreadLock();
				gBottomPanel.Initialize(0, 0);
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_QUERYYUANBANDAO:
		if(wParam)
		{
			free(wParam);
			wParam = NULL;
		}
		break;
	case WM_UDP_MUTESOUND:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			TDDebug(pTmpBuf);
			sscanf(pTmpBuf+2,":ClientID:%d",&nClientID);
			if((TDGetRecordCommandType() != 1) && (TDGetRecordCommandType() != 2))
			{
				gBottomPanel.i64DgtInfo |= 0x2;
				TD_AudioMute(0);			
				gBottomPanel.Initialize(0, 0);    
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);


			}
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_QUERYMUTESOUND:
		if(wParam)
		{
			free(wParam);
			wParam = NULL;
		}
		break;
	case WM_UDP_MODIFYVOL:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			BOOL bModifyType = 0;
			sscanf(pTmpBuf+2,":ModfiyType:%d ClientID:%d",&bModifyType,&nClientID);

			if(TDGetRecordCommandType() != 2)
			{
				if(bModifyType)
				{
					TDDebug("\n vol add \n");
					char ComComm;
					ComComm = 0x04;
					IAL_SetLeds(ComComm);
					TD_VideoIncreaseVolume();
				}
				else
				{
					TDDebug("\n vol sub \n");
					char ComComm;
					ComComm = 0x05;
					IAL_SetLeds(ComComm);
					TD_VideoDecreaseVolume();
				}
				if(gpCurTopMenu)
				{
					if(gpCurTopMenu->dwTopID == MODULE_TOPMENU_TUNE)
					{
						gpCurTopMenu->DrawWindow(0);
						gpCurTopMenu->ShowWindow(0,0);
					}
				}
				PauseThreadLock();
				gBottomPanel.Initialize(0, 0);
				gBottomPanel.DrawWindow(0);
				gBottomPanel.ShowWindow(NULL, 0);
				ResumeThreadLock();
			}

			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_QUERYVOL:
		if(wParam)
		{
			free(wParam);
			wParam = NULL;
		}
		break;
	case WM_UDP_MODIFYTONE:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			int nToneType = 0;
			TDDebug(pTmpBuf);
			char ComComm;
			sscanf(pTmpBuf+2,":ToneType:%d ClientID:%d",&nToneType,&nClientID);
			switch(nToneType)
			{
			case 1:
				TDDebug("Yindiao + \n");
				if(gnUseSoftYinDiao)
				{
					TD_InCreaseYinDiao();
				}
				else
				{
					ComComm = 0x08;
					IAL_SetLeds(ComComm);
					TD_InCreaseYinDiao();
				}
				break;
			case 0:
				TDDebug("Yindiao def \n");
				if(gnUseSoftYinDiao)
				{
					TD_DefaultYinDiao();
				}
				else
				{
					ComComm = 0x09;
					IAL_SetLeds(ComComm);
					TD_DefaultYinDiao();
				}
				break;
			case -1:
				TDDebug("Yindiao - \n");
				if(gnUseSoftYinDiao)
				{
					TD_DeCreaseYinDiao();
				}
				else
				{
					ComComm = 0x0A;
					IAL_SetLeds(ComComm);
					TD_DeCreaseYinDiao();
				}
				break;
			}
			if(gpCurTopMenu)
			{
				if(gpCurTopMenu->dwTopID == MODULE_TOPMENU_TUNE)
				{
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(0,0);
				}
			}
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_QUERYTONE:
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
			break;
	case WM_UDP_GETALLSTATE:
		{
			char* pTmpBuf = (char*)wParam;
			TDDebug(pTmpBuf);
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_CALLSERVICE:
		{
			//分类呼叫服务生 modif by ghy
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			int nCallType = 0;
			TDDebug(pTmpBuf);
			char ComComm;
//			sscanf(pTmpBuf+2,":ClientID:%d",&nClientID);
			sscanf(pTmpBuf+2,":CallType:%d ClientID:%d",&nCallType,&nClientID);
			if(gBoxType == MAIN_BOX)
			{
				ComComm = 0x01;
				IAL_SetLeds(ComComm);
			}
			else
			       	TDKtvCallWaiter();
#if 0
			if(!gnNotUseSoftCallWaiter)
			{
				if (!CallWaiter(nCallType))
				{
					TDDebug("call waite ok");
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, gczInfo[90]);
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.nPriority = 2;
				}
				else
				{
					TDDebug("call waite error");
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, gczInfo[89]);
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.nPriority = 2;
				}
			}
#else
			if(!gnNotUseSoftCallWaiter)
			{
				TDPBYTE pData = NULL;
				TDLONG lDBTotal = 0;
				PTAGERRORCODES pErrorCodes;
				char czSet[128];
				sprintf(czSet ,"Call MachineIpAddress:%s RoomIpAddress:%s CallTypeID:0 ",m_szMyIP,m_szMyIP);
				lDBTotal = GetDatabaseForNewDrink(czSet, (void **)&(pData), TAGERRORCODESSIZE);
				
				/*yang
				if(gpCurTopMenu)
				{
					gpCurTopMenu->Destroy();
					gpCurTopMenu=NULL;
				}*/

				if (lDBTotal  == 0)
				{
					TDDebug("call waite ok");
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, gczInfo[90]);
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.nPriority = 2;
					//yang TDSecondaryMessageBox("呼叫成功!");
				}
				else 
				{
					TDDebug("call waite error");
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, gczInfo[89]);
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.nPriority = 2;
					if(pData)
					{
						pErrorCodes = (PTAGERRORCODES)pData;
						//yang TDSecondaryMessageBox(pErrorCodes ->szErrorDescribe);
						free(pData);
						pData = NULL;
					}
					else
					{
						//yang TDSecondaryMessageBox("呼叫失败!");
					}
				}
			}
#endif
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_MICRADDSUB:
		{
		char* pTmpBuf = (char*)wParam;
		int nClientID = 0;
		BOOL bModifyType = 0;
			sscanf(pTmpBuf+2,":ModfiyType:%d ClientID:%d",&bModifyType,&nClientID);
			if(bModifyType)
			{
				TDDebug("\n mic add \n");
				char ComComm;
				ComComm = 0x06;
				IAL_SetLeds(ComComm);
				TD_InCreaseMicVolume();
				gnCurrentMicState = 2;
			}
			else
			{
				TDDebug("\n mic sub \n");
				char ComComm;
				ComComm = 0x07;
				IAL_SetLeds(ComComm);
				TD_DeCreaseMicVolume();
				gnCurrentMicState = 0;
			}
			if(gpCurTopMenu)
			{
				if(gpCurTopMenu->dwTopID == MODULE_TOPMENU_TUNE)
				{
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(0,0);
				}
			}
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
	case WM_UDP_CAPTION:
		if(wParam)
		{
			free(wParam);
			wParam = NULL;
		}
		break;
	case WM_UDP_HUNYIN:
		{
			static int nSecond = 0;
			if(nSecond == 0)
			{
				nSecond = GetMyTickCount();
			}
			else
			{
				if(GetMyTickCount() - nSecond > 2)
				{
					nSecond = GetMyTickCount();
				}
				else
					return FALSE;
			}

			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			int nHunYinType = 0;
			sscanf(pTmpBuf+2,":HunYinType:%d ClientID:%d",&nHunYinType,&nClientID);
			if(nHunYinType < 3)
			{
				if (gRecordDevInited != 0)
				{
					if(TDGetRecordCommandType() == 0)
					{
						TDSetRecordCommandType(3);
						switch(nHunYinType)
						{
						case 0:
							WhxShowWelcome();
							break;
						case 1:
							WhxShowLaugh();
							break;
						case 2:
							WhxShowLove();
							break;
						}
					}
				}
			}
			else
			{
				if(TDGetRecordCommandType() != 3)//在放气氛音效时不放情景音效
				{
					// 修改人：苗三龙    ghy[3/11/2009] 录音或者回放则停止
					if (TDGetRecordCommandType() == 1 || TDGetRecordCommandType() == 2 || TDGetRecordCommandType() == 5)
					{
						TDStopDoRecord_AND_Play();
						/*
						if(gpCurTopMenu && gpCurTopMenu == &gTopMenu[30])
						{
							gpCurTopMenu->DrawWindow(NULL);
							gpCurTopMenu->ShowWindow(NULL,NULL);
						}*////pengzhongkai
					}
					if(GetVideoStatus() == 1) //暂停现在歌曲
					{
						TDKtvPause();
						/*
						if (&gBottomPanel)
						{
							gBottomPanel.Initialize(0, 0);    
							gBottomPanel.DrawWindow(0);
							gBottomPanel.ShowWindow(NULL, 0);
						}*///pengzhongkai		
					}
					WaitForRecordOrPlayThreadExit(); //等待录音回放彻底结束
					TDSetRecordCommandType(5); //设置为命令5
					switch(nHunYinType)
					{
						case 3:
							sprintf(RecordSongName,"C:\\thunder\\video\\cs%d.wav",GetMyTickCount() % 2);
							break;
						case 4:
							sprintf(RecordSongName,"C:\\thunder\\video\\cz%d.wav",GetMyTickCount() % 2);
							break;
						case 5:
							sprintf(RecordSongName,"C:\\thunder\\video\\mj%d.wav",GetMyTickCount() % 2);
							break;
						case 6:
							sprintf(RecordSongName,"C:\\thunder\\video\\ns%d.wav",GetMyTickCount() % 2);
							break;
					}
					StartHostTestPlayer(); //启动线程播放相应文件
				}
			}
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
			gBottomPanel.Initialize(0, 0);    
			gBottomPanel.DrawWindow(0);
			gBottomPanel.ShowWindow(NULL, 0);
		}
		break;
	case WM_UDP_XIAOGUO:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			char ComComm;
			int nXiaoGuoType = 0;
			sscanf(pTmpBuf+2,":XiaoGuoType:%d ClientID:%d",&nXiaoGuoType,&nClientID);
			switch(nXiaoGuoType)
			{
			case 0:  //light rouhe
				ComComm = 0x20;
				break;
			case 1: //light yaogun
				ComComm = 0x22;
				break;
			case 2: //light shuqing
				ComComm = 0x21;
				break;
			case 3: //light mingliang
				ComComm = 0x23;
				break;
			case 4: //tune changjiang
				ComComm = 0x0F;
				break;
			case 5: // tune yaogun
				ComComm = 0x0E;
				break;
			case 6://tune shuqing
				ComComm = 0x10;
				break;
			case 7://tune liuxing
				ComComm = 0x11;
				break;
			}
			IAL_SetLeds(ComComm);
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
#ifdef	WILLDO
	case WM_UDP_BROADCAST:
		{
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			char ComComm;
			int nBroadCaseType = 0;
			SwitchVideo();
			sscanf(pTmpBuf+2,":BroadCast:%d ClientID:%d",&nBroadCaseType,&nClientID);
//			printf("nBroadCaseType = %d\n",nBroadCaseType);
			TDDebug(pTmpBuf);
			//收到大厅转播命令后自动转到大厅播放页面
			//Add BY Ghy
			//yang STBSendInfo(BROADCAST,0);//modify by ghy 横版无大厅播放返回0值

			////////////////////////add by yanglei
			if(gpCurExpandView->dwModuleID != 0x20300000)
			{
				gpCurExpandView->Destroy();
				gpCurExpandView = &gExpandView[2];
				gpCurExpandView->Create();
				gpCurExpandView->Initialize(0, 0);
				gpCurExpandView->DrawWindow(0);
				gpCurExpandView->ShowWindow(NULL, 0);

				gExpandViewCtrl.Initialize(0, 0);
				gExpandViewCtrl.DrawWindow(0);
				gExpandViewCtrl.ShowWindow(NULL, 0);
			}
			if(nBroadCaseType)
			{
				TDDebug("开始大厅广播");
				TVBoxViewHandleLBtnUp(0x30000);
			}
			else
			{
				TDDebug("结束大厅广播  ");
				TVBoxViewHandleLBtnUp(0x39000);
			}
			//..
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
		break;
#endif
	case WM_UDP_RECORD:
#if 1//yang
		{
			if(gbPreviewIsMove)
			{
				if(wParam)
				{
					free(wParam);
					wParam = NULL;
				}
				break;
			}
			char* pTmpBuf = (char*)wParam;
			int nClientID = 0;
			PHOTSPOT	pCurHotspot = NULL;
			char ComComm;
			int nRecordType = 0;
			char strTmp[32];
			sscanf(pTmpBuf+2,":Record:%d ClientID:%d",&nRecordType,&nClientID);
//			printf(pTmpBuf);
//			printf("\n");
			//修改人:msl 日期 2009-2-17 原因:录音回放有广告则切屏
			if(nRecordType != -3)
			{
				SwitchVideo();

				if(!gpCurTopMenu)
				{
					gpCurTopMenu=&gTopMenu[TOPMENUVIEW_TUNE];
					gpCurTopMenu->dwTopID = MODULE_TOPMENU_RECORDING;
					gpCurTopMenu->Create();
					gpCurTopMenu->Initialize(0,0);
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(0, SHOW3D | SHOWNEXTANDPROCPAGE);
				}
				else if(gpCurTopMenu->dwTopID != MODULE_TOPMENU_RECORDING)
				{
					if (gpCurTopMenu) 
					{
						gpCurTopMenu->Destroy();
					}
					gpCurTopMenu=&gTopMenu[TOPMENUVIEW_TUNE];
					gpCurTopMenu->dwTopID = MODULE_TOPMENU_RECORDING;
					gpCurTopMenu->Create();
					gpCurTopMenu->Initialize(0,0);
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(0, SHOW3D | SHOWNEXTANDPROCPAGE);
				}
			}
			switch(nRecordType)
			{
			case -3:
					sprintf(cmdbuf,"STBCMD:%2d: Record:",RECORD);
					if(gnRecordIndex >= 0)
					{
						sprintf(strTmp,"%d;",-gnRecordIndex);
						strcat(cmdbuf,strTmp);
					}
					if(TDGetRecordCommandType() == 1)
						strcat(cmdbuf,"2;");
					else
						strcat(cmdbuf,"1;");
					STBSendUDPMsg(cmdbuf);
				break;
			case -2:

				if(TDGetRecordCommandType() == 1 ||
					TDGetRecordCommandType() == 2)
				{
					TDStopDoRecord_AND_Play();
					gnRecordIndex = -1;
//					gpCurTopMenu->DrawWindow(0);
//					gpCurTopMenu->ShowWindow(NULL, 0/*SHOWNEXTANDPROCPAGE*/);	
					SendNotifyMessage(NULL, REDRAWCURRENTVIEW, 1, 8);
				}
				sprintf(cmdbuf,"STBCMD:%2d: Record:",RECORD);
				if(gnRecordIndex >= 0)
				{
					sprintf(strTmp,"%d;",-gnRecordIndex);
					strcat(cmdbuf,strTmp);
				}
				if(TDGetRecordCommandType() == 1)
					strcat(cmdbuf,"2;");
				else
					strcat(cmdbuf,"1;");
				STBSendUDPMsg(cmdbuf);
				break;
			case -1:
				TDDoRecord();

				gpCurTopMenu->Initialize(0, 0);
				gpCurTopMenu->DrawWindow(0);
				gpCurTopMenu->ShowWindow(0, 0 | SHOWNEXTANDPROCPAGE);

				sprintf(cmdbuf,"STBCMD:%2d: Record:",RECORD);
				if(gnRecordIndex >= 0)
				{
					sprintf(strTmp,"%d;",-gnRecordIndex);
					strcat(cmdbuf,strTmp);
				}
				if(TDGetRecordCommandType() == 1)
					strcat(cmdbuf,"2;");
				else
					strcat(cmdbuf,"1;");
				STBSendUDPMsg(cmdbuf);
				break;
			default:
				{
					TDCHAR		czParam[256];
					TDINT		iSelItem;
					char				czDebug[100];
					TDINT			iLoop;
					static struct tagRecord
					{
						char	czMediaName[200];
						char	czVideoUrl[100];
						char	czVideoFormat[10];
						char	czAudioFormat[10];
						int 	nVolume;
						int 	nOriginal;
						int 	nAccompanny;
						int 	nAudioStatus;
						BYTE	btMediaType;
					} *pMediaName=NULL;
					gnRecordIndex = nRecordType;

					sprintf(cmdbuf,"STBCMD:%2d: Record:",RECORD);
					if(gnRecordIndex >= 0)
					{
						sprintf(strTmp,"%d;",-gnRecordIndex);
						strcat(cmdbuf,strTmp);
					}
					if(TDGetRecordCommandType() == 1)
						strcat(cmdbuf,"2;");
					else
						strcat(cmdbuf,"1;");
					STBSendUDPMsg(cmdbuf);
					char cmdbuf[526];
					char strTmp[32];
					iSelItem = nRecordType;
					if(gpCurTopMenu->lDBTotal > 0)
					{
						if (TDGetRecordCommandType() == 2)
							TDStopDoRecord_AND_Play();
						pMediaName = (struct tagMediaName *)gpCurTopMenu->pData;
						if(strlen(gszRecordSvrIP) <= 0)
						{
							TDDebug("no RecordSvrIP info");
							return FALSE;
						}
						TDDebug("test URl......................");
						sprintf(czDebug,"Select ID:%d",iSelItem);
						TDDebug(czDebug);
						sprintf(czDebug,"URL:%s",pMediaName[iSelItem].czVideoUrl);
						TDDebug(czDebug);
						if (strlen(pMediaName[iSelItem].czVideoUrl) > 0)//if szVideoURL's len is 0,return;
						{
							TDDebug("play begin");
							strcpy(gRecordInfo.szVideoURL,pMediaName[iSelItem].czVideoUrl);
							strcpy(gRecordInfo.szVideoFormat,pMediaName[iSelItem].czVideoFormat);
							strcpy(gRecordInfo.szAudioFormat,pMediaName[iSelItem].czAudioFormat);
							gRecordInfo.DataBaseVolume=pMediaName[iSelItem].nVolume;
							gRecordInfo.OriginalChannel=pMediaName[iSelItem].nOriginal;
							gRecordInfo.AccompannyChannel=pMediaName[iSelItem].nAccompanny;
							gRecordInfo.AudioChannelStatus=pMediaName[iSelItem].nAudioStatus;
							strcpy(gszCurrentSongName,pMediaName[iSelItem].czMediaName + 4);
							gszCurrentSongName[strlen(gszCurrentSongName) - 4] = '\0';
							strcpy(RecordSongName,pMediaName[iSelItem].czMediaName);
//							printf("record SongName = %s  pMedia[%d].czMedia = %s \n",RecordSongName,iSelItem,pMediaName[iSelItem].czMediaName);
							BYTE btMediaGroup =0;
							TDDWORD dwMediaID = 0;
							unsigned long nTmp = 0;
							TDDebug(gRecordInfo.szVideoURL);
							nTmp = atol(gRecordInfo.szVideoURL);
							dwMediaID = nTmp /10;
							btMediaGroup = nTmp - dwMediaID * 10;
							char strTmp[128];
							sprintf(strTmp," ++++++++++++++++++  pVideoUrl = %s btMediaGroup = %d nMediaID = %d",gRecordInfo.szVideoURL,btMediaGroup,dwMediaID);
							TDDebug("++++++++++++++++");
							TDDebug(strTmp);
							if(TDDoRecordPlay(btMediaGroup,dwMediaID))
							{
								return FALSE;
							}
						}
						gBottomPanel.i64DgtInfo = 0;
						gBottomPanel.Initialize(0, 0);    
						gBottomPanel.DrawWindow(0);
						gBottomPanel.ShowWindow(NULL, 0);

					}
					gpCurTopMenu->DrawWindow(0);
					gpCurTopMenu->ShowWindow(NULL, 0/*SHOWNEXTANDPROCPAGE*/);	
				}
				break;
			}
//			printf("end  cmd = %d \n",nRecordType);
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
			}
		}
#endif
		break;
	case WM_UDP_CLOSEROOMCMD:
	{
		gnIsInCleanRoom = 0;
		SendNotifyMessage(NULL, CLOSEROOM, 0, 1);
		if(wParam)
		{
			free(wParam);
			wParam = NULL;
		}
		break;
	}
#if 0
	case WM_UDP_MRSONGCMD:
	{
		if(gbPreviewIsMove)
		{
			STBSendInfo(MRSONGCMD,2);
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
				gszMRCmd = NULL;
			}
				break;
		}
		if(TDGetRecordCommandType() == 0)
		{
			gszMRCmd = (char*)wParam;
			gbMRCmdEnble = TRUE;
			TDDebug(gszMRCmd);
			char sendBuff[256];
			pthread_mutex_lock(&hKeyStatusMutex);
			if (!RecordInfo.nKeyStatus)
			{
				TDDebug("RecordInfo.nKeyStatus = 0");
				if (!RecordInfo.nThreadBusy)
				{
					TDDebug("RecordInfo.nThreadBusy = 0");
					RecordInfo.nRecordType = 1;
					RecordInfo.nKeyStatus = 1;
					RecordInfo.nThreadBusy = 1;
					TDDebug("SetEvent ");
					SetEvent(RecordInfo.RecordEvent);
				}
				else
				{
					TDDebug("RecordInfo.nThreadBusy = 1");
					RecordInfo.nRecordType = 1;
					RecordInfo.nKeyStatus = 1;
				}
			}
			else
			{
				TDDebug("RecordInfo.nKeyStatus = 1");
				RecordInfo.nRecordType = 1;
			}
			pthread_mutex_unlock(&hKeyStatusMutex);
		}
		else
		{
			STBSendInfo(MRSONGCMD,2);
			if(wParam)
			{
				free(wParam);
				wParam = NULL;
				gszMRCmd = NULL;
			}
		}
		break;
	}
	case WM_UDP_MRSONGSET:
	{
		int Yuyin = 1;
		int nClientID = 0;
		sscanf(wParam+2,":CT:%d ClientID:%d",&Yuyin,&nClientID);
		if(Yuyin == 3)
		{
			Yuyin = 1;
		}
		if(strlen(gszRMSvrIP) <= 0)
		{
			Yuyin = 3;
		}
		gbYuYingEnable = Yuyin;
		if(gbYuYingEnable)
		{
			WriteToVoiceBox(0x00, gfdTVBox);
		}
		else
		{
			WriteToVoiceBox(0x06, gfdTVBox);
		}
		STBSendInfo(MRSONGSET,Yuyin);
		if(wParam)
		{
			free(wParam);
			wParam = NULL;
		}
		break;
	}
#endif
	default:
		/*if(wParam)
		{
			free(wParam);
			wParam = NULL;
		}*/
		break;
	}
	return 0;
}

TDVOID TDKTVCallback(TDVOID* pvoid)
{
	SendNotifyMessage(NULL, MSG_TDCALLBACK, 0, 0);
}

TDVOID TDKTVCommandCallback(TDVOID* pnull,TDCHAR* command,char* page)
{
	static TDDWORD  dwCleanDelay = 0;
	BITMAP hMemBitmap;
	char ComComm;
	if(!gbIsRevCallBackMsg)
	{
		if(!strcmp(command, "FireAlarm"))
		{
			nFireAlarm = 1;
			gDelayFire = 1;
		}
		else if(!strcmp(command, "ERRORSTOP"))
		{
			TDDebug("error stop");
			switch(atoi(page))
			{
				case 140:
					TDDebug("show error");
					ShowError(14);
					break;
			};
		}
		TDDebug("gbIsRevCallBackMsg == FALSE");
		return;
	}
	TDDebug(command);
	if(!strcmp(command, "FireAlarm"))
	{
		if (!strcmp(page, "TRUE"))
		{
			TDDebug("Send Fire Alarm Message");
			SendNotifyMessage(NULL, WM_FIREALARM, 0, 0); //msl
		}
	}
	else if (!strcmp(command, "ClientTimePrompt"))
	{
		TDDebug("Client Box Time Prompt!");
		SendNotifyMessage(NULL, WM_TIMEPROMPT, 0x50000, NULL);
	}
	else if (!strcmp(command, "ClientCloseRoom"))
	{
		TDDebug("Client Box Close Room!");
		SendNotifyMessage(NULL, CLOSEROOM, 0, 0);
	}
	else if (!strcmp(command, "ClearRoomInfo"))
	{
		if (!strcmp(page, m_szMyIP))
		{
			TDDebug("CLOSEROOM");
			SendNotifyMessage(NULL, CLOSEROOM, 0, 0); //szj
		}
	}
	else if (!strcmp(command, "OpenRoom"))
	{
		if (!strcmp(page, m_szMyIP))
			SendNotifyMessage(NULL, OPENROOM, 0, 0); //szj
	}
	else if (!strcmp(command, "CanSelectSong"))
	{
		SendNotifyMessage(NULL, OPENROOM, 0, 0); //szj
	}
	else if (!strcmp(command, "DeleteSongList"))
	{
		if (!strcmp(page, m_szMyIP))
		{
			SendNotifyMessage(NULL, CLOSEROOM, 1, 0); //szj
		}
	}
	else if(!strcmp(command, "REDRAWSELECTVIEW"))
	{
		SendNotifyMessage(NULL, REDRAWSELECTVIEW, NULL, NULL);//jzn
	}
	else if(!strcmp(command,"video"))
	{
		TDKtvNext();
	}
	else if (!strcmp(command, CLIENT_CMD_APPENDMEDIA) && gBoxType == MAIN_BOX)
	{
		char * szCommand = NULL;

		//add for broadcast
		//if was in broadcast,can't appendmedia
		if(TDGetInBroadcast())
			return;
		//add for broadcast

		szCommand = (char *)malloc(strlen(page) + 1);
		strcpy(szCommand,page);
		TDDebug(page);
		TDDebug("PostMessage WM_CLIENTAPPENDMEDIA");
		TDDebug(szCommand);
		SendNotifyMessage(NULL, MEDIACLIENTAPPEND, szCommand, NULL);//jzn
	}
	else if(!strcmp(command, "SERVERAPPENDMEDIA") && gBoxType == MAIN_BOX)
	{
		char * szCommand = NULL;
		//add for broadcast
		//if was in broadcast,can't appendmedia
		if(TDGetInBroadcast())
			return;
			//add for broadcast
		SwitchVideo();

		szCommand = (char *)malloc(strlen(page) + 1);
		strcpy(szCommand,page);
		TDDebug(page);
		TDDebug("PostMessage WM_SERVERAPPENDMEDIA");
		TDDebug(szCommand);
		SendNotifyMessage(NULL, WM_MEDIAKTVOK, szCommand, NULL);//jzn
	}
	else if (!strcmp(command, CLIENT_CMD_APPENDACTOR))
	{
		TDPString strText;
		TDPString strSongerName;

		strText = TDString_Create();
		strSongerName = TDString_Create();

		TDDebug(page);
		TDString_AppendPChar(strText, page);

		if(TDKtvGetValueFromResponse(strText, strSongerName, "songer"))
			InsertActors2Chain(strSongerName->mStr);


		TDString_Destroy(strText);
		TDString_Destroy(strSongerName);
	}
	else if (!strcmp(command, CLIENT_CMD_CALLWAITER))
	{
		if(gBoxType == MAIN_BOX)
		{
			ComComm = 0x01;
			IAL_SetLeds(ComComm);
		}
	}
	else if(!strcmp(command, "UDP_CMD"))
	{
			char nTmp[3];
			memset(nTmp,0,sizeof(nTmp));
			strncpy(nTmp,page,2);
		TDDebug(page);
		if(gpCurTopMenu && gpCurTopMenu->dwModuleID == MODULE_TOPMENU_AWOKETIMEOUT && (WM_UDP_APPENDMEDIA + atoi(nTmp)) != WM_UDP_CALLSERVICE)
		{
			free(page);
			page = NULL;
		}
		else if(!strncmp(page+2,":",1)/* && !(TDGetInBroadcast())*/)
		{
			TDDebug("ok package");
			SendNotifyMessage(NULL, WM_UDP_APPENDMEDIA + atoi(nTmp), page, NULL);
		}
		else
		{
			TDDebug("Invalid UDP package");
			free(page);
			page = NULL;
		}
	}
	else if(!strcmp(command, "caption"))
	{
		int time =0;
		char text[1024];
		UINT color = 0;
		char *pTxtPos;
		char *pColorPos;
		char *pTimesPos;
		char *pLanguagePos;
		int nLanguage = -1;
		int 	nTimes;
		int	nTxtLen = strlen(page);
		char tt[50];
		int nColor;
		char *sColor;
		char sTemp;

		if (nTxtLen > 1024)
		{
			return;
		}
		pTxtPos = strstr(page, "Text:");
		if (pTxtPos == NULL)
		{
			return;
		}
		if (nTxtLen < (page - pTxtPos + 5))
		{
			return;
		}
		strcpy(text, pTxtPos + 5);
		pTimesPos = strstr(text, "Time:");
		pColorPos = strstr(text, "Color:");
		pLanguagePos = strstr(text, "Language:");
		if (pTimesPos)
		{
			nTimes = atoi(pTimesPos + 5);
    //        		gADBarInfo.nShowADTimes = nTimes;
			*pTimesPos = 0;
		}
		if (pLanguagePos)
		{
			nLanguage = atoi(pLanguagePos + 9);
			if(nLanguage < 0 || nLanguage > 2)
				nLanguage = 0;
    //        		gADBarInfo.nShowADTimes = nTimes;
			*pLanguagePos = 0;
		}

		if (pColorPos)
		{
			nColor = atoi(pColorPos + 6);
			sColor = (char *)(&nColor);
			sTemp = sColor[0];
			sColor[0] = sColor[2];
			sColor[2] = sTemp;
			//gOsdInfo.dwFontColor = nColor;
			if(nColor <= 0)
				nColor = 1;
			*pColorPos = 0;
		}
		else
		nColor = 0xD4F4D2;
		//nColor = OSD_TEXTCOLOR;
		sprintf(tt, "Color: %d, Times: %d, nLanguage: %d", nColor, nTimes,nLanguage);
		TDDebug(tt);

		SetOsdShowWord(text, OSD_ROCKSTRING, nTimes, nLanguage, 0, 0, nColor);

		TDDebug("after SetOsdShowWord");

		if(gADBarInfo.nPriority <= 3)
		{
			SwitchVideo();
			gADBarInfo.bIsDraw = FALSE;
			strcpy(gADBarInfo.szADTxt, text);
			gADBarInfo.nLanguage = nLanguage;
			gADBarInfo.nIsRefresh = ADB_ADTXTREFRESH;
			gADBarInfo.nPriority = 3;
		}
	}
	else if (!strcmp(command, "TimePrompt"))//到时提醒
	{
		TDDebug("发送到时提醒消息 TimePrompt\n");
		if ((GetMyTickCount() - dwCleanDelay) > 10000 && !gnIsInCleanRoom)
		{
			TDDWORD dwParam = atol(page);
			if (HIWORD(dwParam) == 5)
			{
				nDoublePause = TRUE;
				//PauseADBarThreads();
				Sleep(2000);
			}
			TDDebug("scm TimePrompt,so nDouble=0");
/*			if(gnDouble == 3)
			dwTCount = GetMyTickCount();
			SwitchVideo();*/
			SendNotifyMessage(NULL, WM_TIMEPROMPT, dwParam, NULL);
			dwCleanDelay = 0;
		}
	}
	else if(!strcmp(command, "ERRORSTOP"))
	{
		SwitchVideo();
		nDoublePause = TDTRUE;

		gbKTVError = atoi(page);
		gbIsRevCallBackMsg = FALSE;
//		PauseLogoThreads();
		PauseADBarThreads();
		//bDoubleHandle = FALSE;
		//DCISetAlphaBlendFactor(255);
		ShowCursor(TDFALSE);
		//PauseOsdThreads();
		sleep(1);
		switch(atoi(page))
		{
		case ERROR_GET_LICENSE:
		case ERROR_GET_DOGINFO:
		case ERROR_GET_STBINFOLIST:
			{
				TDDebug("//1、无法同服务器正常通讯，请检查网络连接是否正常，雷石KTV服务是否正常运行。");
				ShowError(15);
				break;
			}
		case ERROR_OVER_CONCURRENTUSER:
		case ERROR_STB_IS_NOT_EXIST:
			{
				TDDebug("2、超过并发流，请联系经销商购买新的并发流授权。");
				ShowError(16);
				break;
			}
		case ERROR_LICENSE_INVALID:
			{
				TDDebug("//3、非法License文件");
				ShowError(3);
				break;
			}
		case ERROR_PUT_STBINFOLIST:
			{
				TDDebug("//4、上传机顶盒信息出错，请检查网络连接是否正常，雷石KTV服务是否正常运行。");
				ShowError(17);
				break;
			}
		case ERROR_DOGINFO_INVALID:
			{
				TDDebug("//5、非法加密狗信息。");
				ShowError(18);
				break;
			}
		case ERROR_DOG_IS_NOT_EXIST:
			{
				TDDebug("//6、检测不到雷石加密狗，请检查加密狗是否插好。");
				ShowError(19);
				break;
			}
		case ERROR_TIME_INVALID:
		case ERROR_DAYS_INVALID:
			{
				TDDebug("//7、非法系统时间。");
				ShowError(20);
				break;
			}
		case ERROR_TIME_CHANGED:
			{
				TDDebug("//8、请不要更改服务器时间，修改服务器时间将导致系统异常。");
				ShowError(21);
				break;
			}
		case ERROR_OVER_TIME:
			{
				TDDebug("//9、认证已到期，请联系经销商获取新的认证。");
				ShowError(22);
				break;
			}
		case ERROR_USERNAME:
			{
				TDDebug("//10ERROR_USERNAME");
				ShowError(4);
				break;
			}
		case ERROR_PASSWORD_INVALID:
			{
				TDDebug("//10ERROR_USERNAME");
				ShowError(23);
				break;
			}
		case 140:
			TDDebug("show error");
			ShowError(14);
			break;
		};
	}
	else if (!strcmp(command, "CLEANROOM"))
	{
		if (!strcmp(page, m_szMyIP))
		{
			if(gBoxType == MAIN_BOX)
			{
				gnProcessRoom = 1;
				//dwTCount = GetMyTickCount();
				TDDebug("PostMessage WM_CLEANROOM");
				SendNotifyMessage(NULL, WM_CLEANROOM, NULL, NULL);
			}
		}
		else
		{
			TDDebug("page != m_szMyIP");
		}
	}
	else if (!strcmp(command, "ChangeRoomInfo"))
	{
		if(gBoxType == MAIN_BOX)
		{
			gnProcessRoom =2;
			SwitchVideo();
			TDDebug("PostMessage WM_CHANGEROOM");
			SendNotifyMessage(NULL, WM_CHANGEROOM, NULL, NULL);
		}
	}
	else if(!strcmp(command, "REDRAWBROADCASTVIEW"))
	{
		TDDebug("PostMessage REDRAWBROCAST");
		SendNotifyMessage(NULL, WM_REDRAWBROADCASTVIEW, NULL, NULL);
	}
	else if(!strcmp(command,"SaveRecord"))
	{
//pengzhongkai
		TDDebug("Command = SaveRecord");
		SaveToDatabase(RecordSongName,gRecordInfo.szVideoFormat,gRecordInfo.szAudioFormat,gRecordInfo.DataBaseVolume,gRecordInfo.OriginalChannel,gRecordInfo.AccompannyChannel,gRecordInfo.AudioChannelStatus,gRecordSongLangID,gRecordInfo.szVideoURL);
#if 0//yang
		if(gpCurTopMenu && gpCurTopMenu->dwModuleID == MODULE_TOPMENU_RECBOARD/* && gpCurTopMenu == NULL*/)
		{


			gpCurTopMenu->Initialize(0, 1);
			gpCurTopMenu->DrawWindow(0);
			gpCurTopMenu->ShowWindow(NULL ,0);


			gpCurTopMenu->lCurData--;
			gpCurTopMenu->DrawWindow(-1);
			gpCurTopMenu->lCurData++;
			if (gpCurTopMenu->lCurData * gpCurTopMenu->iCPP >= gpCurTopMenu->lDBTotal)
			{
				gpCurTopMenu->lCurData = 0;
			}
			gpCurTopMenu->lCurData++;
			gpCurTopMenu->DrawWindow(1);
			gpCurTopMenu->lCurData--;
			if (gpCurTopMenu->lCurData < 0)
			{
				gpCurTopMenu->lCurData = (gpCurTopMenu->lDBTotal + gpCurTopMenu->iCPP - 1) / gpCurTopMenu->iCPP-1;
			}
			gpCurTopMenu->DrawWindow(3);
		}
#endif
		SendNotifyMessage(NULL, REDRAWCURRENTVIEW, NULL, NULL);//jzn

#ifdef	WILLDO
		TDDebug("Command = SaveRecord");
		SaveToDatabase(RecordSongName,gRecordInfo.szVideoFormat,gRecordInfo.szAudioFormat,gRecordInfo.DataBaseVolume,gRecordInfo.OriginalChannel,gRecordInfo.AccompannyChannel,gRecordInfo.AudioChannelStatus,gRecordSongLangID,gRecordInfo.szVideoURL);

		if(gpCurClientView && gpCurClientView->dwModuleID == 0x40200000 && gpCurTopMenu == NULL)
		{
			gpCurClientView->Initialize(0, 0);
			gpCurClientView->DrawWindow(0);


			gpCurClientView->lCurData--;
			gpCurClientView->DrawWindow(-1);
			gpCurClientView->lCurData++;
			if (gpCurClientView->lCurData * gpCurClientView->iCPP >= gpCurClientView->lDBTotal)
			{
				gpCurClientView->lCurData = 0;
			}
			gpCurClientView->lCurData++;
			gpCurClientView->DrawWindow(1);
			gpCurClientView->lCurData--;
			if (gpCurClientView->lCurData < 0)
			{
				gpCurClientView->lCurData = (gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP-1;
			}
			gpCurClientView->DrawWindow(3);
		}
		SendNotifyMessage(NULL, REDRAWCURRENTVIEW, NULL, NULL);//jzn
#endif
	}
	else if(!strcmp(command, "SHUTDOWN"))
	{
		if (!strcmp(page, m_szMyIP))
		{
			TDDebug("Recv SHUTDOWN");
//			printf("before poweroff\n");
			/*
#define	LINUX_REBOOT_CMD_RESTART	0x01234567
#define	LINUX_REBOOT_CMD_HALT		0xCDEF0123
#define	LINUX_REBOOT_CMD_CAD_ON		0x89ABCDEF
#define	LINUX_REBOOT_CMD_CAD_OFF	0x00000000
#define	LINUX_REBOOT_CMD_POWER_OFF	0x4321FEDC
#define	LINUX_REBOOT_CMD_RESTART2	0xA1B2C3D4
#define	LINUX_REBOOT_CMD_SW_SUSPEND	0xD000FCE2
#define	LINUX_REBOOT_CMD_KEXEC		0x45584543
*/
			//system("halt");
			//reboot(LINUX_REBOOT_CMD_RESTART);
			//reboot(LINUX_REBOOT_CMD_HALT);
			//reboot(LINUX_REBOOT_CMD_POWER_OFF);
			//reboot(LINUX_REBOOT_CMD_CAD_ON);
//			printf("after poweroff\n");
		}
		else
		{
			TDDebug("page != m_szMyIP");
		}
	}
	else if(!strcmp(command,"qingke"))
	{
		if (!strcmp(page, m_szMyIP))
		{  
			if(gBoxType == MAIN_BOX)
			{
//				printf("clean client\n\n");
				TDDebug("PostMessage WM_CLEANCLIENT");
				SendNotifyMessage(NULL, WM_CLEANCLIENT, NULL, NULL);
			}
		}
		else
		{
			TDDebug("page != m_szMyIP");
		}
	}
	else if(!strcmp(command,"quxiaoqingke"))
	{
		if (!strcmp(page, m_szMyIP))
		{   
			if(gBoxType == MAIN_BOX)
			{
//				printf("cancle clean client\n\n");
				TDDebug("PostMessage WM_CANCLECLEANCLIENT");
				SendNotifyMessage(NULL, WM_CANCLECLEANCLIENT, NULL, NULL);
			}
		}
		else
		{
			TDDebug("page != m_szMyIP");
		}
	}



}

static TDVOID MessageHInit()
{
	pthread_t hMessage;
	pthread_t hDrawMessage;

	MsgStack.hMHEvent = CreateEvent();
	MsgStack.iHeadStack = 0;
	MsgStack.iTailStack = 0;
	MsgStack.bIsRunning = TRUE;

	pthread_create(&hMessage, NULL, MessageHThread, NULL);

	DrawMsg.hMHEvent = CreateEvent();
	DrawMsg.iHeadStack = 0;
	DrawMsg.iTailStack = 0;
	DrawMsg.bIsRunning = TRUE;

	gpDrawEndEvent = CreateEvent();

	pthread_create(&hDrawMessage, NULL, DrawMessageHThread, NULL);
}

static void *MessageHThread(void *pNULL)
{
	MSG		Msg;
#ifdef	STANDARDVER
	TDLONG		lPreTime = 0;
	TDLONG		lCurTime = 0;

	while (MsgStack.bIsRunning)
	{
		if (WaitForSingleObject(MsgStack.hMHEvent, 0) == WAIT_TIMEOUT)
		{
			Sleep(100 * 1000);
			continue;
		}

		while (MsgStack.iHeadStack != MsgStack.iTailStack)
		{
			WndProc(MsgStack.Message[MsgStack.iHeadStack].hWnd,
				MsgStack.Message[MsgStack.iHeadStack].message,
				MsgStack.Message[MsgStack.iHeadStack].wParam,
				MsgStack.Message[MsgStack.iHeadStack].lParam);
			MsgStack.Message[MsgStack.iHeadStack].hWnd = NULL;
			MsgStack.Message[MsgStack.iHeadStack].message = 0;
			MsgStack.Message[MsgStack.iHeadStack].wParam = 0;
			MsgStack.Message[MsgStack.iHeadStack].lParam = 0;
			MsgStack.iHeadStack = (MsgStack.iHeadStack + 1) % 512;
		}
		ResetEvent(MsgStack.hMHEvent);
	}
#else
	while (MsgStack.bIsRunning)
	{
		TH_GetMessage(&Msg, NULL);
		//PauseADBarThreads();
		pthread_mutex_lock(&gDCMutex);

		WndProc(NULL,
			Msg.message,
			Msg.wParam,
			Msg.lParam);
		pthread_mutex_unlock(&gDCMutex);
	}

#endif

	return 0;
}

#if 1

static int WndDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char	szTmpTxt[256];
	TDINT nOffset = 0;
	TDINT i = 5;
	TDINT j = 0;
	TDINT	nDrawPreNextFlag = 0;
	TDINT	nOffsetLen = 0;
	if(nPauseDrawThread == -1)
		return 0;
	if(nPauseDrawThread == 0 && DRAWMSG_SHOWSELECT != message && DRAWMSG_SHOWPHOTO != message)
		return 0;

/*
	dwTCount = GetMyTickCount();
	nDouble = 0;
	gnDouble = 0;
	gnStaffTime = 0;
*/

	switch(message)
	{
//yang add
	case DRAWMSG_KARAOKMOVE:
		if((int)wParam == MOVELEFTTORIGHT)
		{
			if(gpCurClientView && gpCurClientView->dwModuleID == 0x10100000)//karaok
			{
				//			printf("l to r gnLastMoveLength = %d\n",(int)lParam);
				for(i = (int)lParam; i >= 0; i--)
				{
					usleep(1);
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;
					
					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}
					
					if(nOffsetLen == 0)
					{
						break;
					}
					
					if(gnKaraokOffset - nOffsetLen < MINOFFSET - BUFFERLEN)//到底了
					{
						gnKaraokOffset = MINOFFSET - BUFFERLEN;
					}
					else
					{
						gnKaraokOffset = gnKaraokOffset - nOffsetLen;
					}
					//				printf("drawproc 111 gnKaraokOffset = %d, nOffsetLen = %d\n",gnKaraokOffset,nOffsetLen);
					//				printf("thread 1\n");
					gpCurClientView->ShowWindow(NULL, NULL);
					
					if(nPauseDrawThread != 1)
						break;
					
					if(gnKaraokOffset - nOffsetLen < MINOFFSET - BUFFERLEN)//到底了
					{
						gnKaraokOffset = MINOFFSET - BUFFERLEN;
					}
					else
					{
						gnKaraokOffset = gnKaraokOffset - nOffsetLen;
					}
					//				printf("drawproc 111 gnKaraokOffset = %d, nOffsetLen = %d\n",gnKaraokOffset,nOffsetLen);
					//				printf("thread 12\n");
					gpCurClientView->ShowWindow(NULL, NULL);
					
					
					if(gnKaraokOffset < MINOFFSET)
					{
						gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFTLASTPAGE);
						return;
					}
					//				usleep(100);
				}
			}
			else if(gpCurClientView && gpCurClientView->dwModuleID == 0x11000000)//actor
			{
				if((int)lParam > 38)
				{
					lParam = 38;
				}
				for(i = (int)lParam; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;
					
					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}
					
					if(nOffsetLen == 0)
					{
						break;
					}

					gnKaraokOffset = gnKaraokOffset - nOffsetLen;

					gnBGOffset = gnBGOffset - nOffsetLen;
					if(gnBGOffset <= (-720 / BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 720 * BGMOVEOFFSET;
					}
					
					if(gnKaraokOffset <= -SONGERPICOFFSET)
					{
						TD3D_UpdateActorObjectIndex(MOVELEFTTORIGHT);
						gnKaraokOffset += SONGERPICOFFSET;
						//TD3D_UpdateActorNameToObject();
					}

					gnActorOffset = gnActorOffset - nOffsetLen;
					if(gnActorOffset <= -SONGERUPDATEOFFSET)
					{
						gnActorOffset += SONGERUPDATEOFFSET;
#if 0
						pthread_mutex_lock(&gUpdateActorPicMutexEnd);
						SetEvent(ActorUpdatePicEvent);
						pthread_mutex_unlock(&gUpdateActorPicMutexEnd);
#else
						if(gnActorNumState == ACTOR_NUM_STATE_BIG)
						{
							gnActorCurPageState = STATE_USEING;
							while(gnSendEventActorState != STATE_NOUSE)
							{
								usleep(100);
							}
							SetEvent(ActorUpdatePicEvent);
						}
						//ActorCheckAllObjectToUpdate(MOVELEFTTORIGHT);
#endif
					}

					gpCurClientView->ShowWindow(NULL, NULL);
					
					if(nPauseDrawThread != 1)
						break;

					gnBGOffset = gnBGOffset - nOffsetLen;
					if(gnBGOffset <= (-720 / BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 720 * BGMOVEOFFSET;
					}

					gnKaraokOffset = gnKaraokOffset - nOffsetLen;
					if(gnKaraokOffset <= -SONGERPICOFFSET)
					{
						TD3D_UpdateActorObjectIndex(MOVELEFTTORIGHT);
						gnKaraokOffset += SONGERPICOFFSET;
					}

					gnActorOffset = gnActorOffset - nOffsetLen;
					if(gnActorOffset <= -SONGERUPDATEOFFSET)
					{
						gnActorOffset += SONGERUPDATEOFFSET;
#if 0
						pthread_mutex_lock(&gUpdateActorPicMutexEnd);
						SetEvent(ActorUpdatePicEvent);
						pthread_mutex_unlock(&gUpdateActorPicMutexEnd);
#else
						if(gnActorNumState == ACTOR_NUM_STATE_BIG)
						{
							gnActorCurPageState = STATE_USEING;
							while(gnSendEventActorState != STATE_NOUSE)
							{
								usleep(100);
							}
							SetEvent(ActorUpdatePicEvent);
						}
						//ActorCheckAllObjectToUpdate(MOVELEFTTORIGHT);
#endif
					}

					gpCurClientView->ShowWindow(NULL, NULL);

					if(i <= (int)lParam / 4)
					{
						for(j = 0; j < (((int)lParam - i + 1) > 5 ? 5 : (int)lParam - i + 1) ; j++)
						{
							if(nPauseDrawThread != 1)
								break;
							
							gnBGOffset = gnBGOffset - nOffsetLen;
							if(gnBGOffset <= (-720 / BGMOVEOFFSET))
							{
								gnBGOffset = gnBGOffset + 720 * BGMOVEOFFSET;
							}
							
							gnKaraokOffset = gnKaraokOffset - nOffsetLen;
							if(gnKaraokOffset <= -SONGERPICOFFSET)
							{
								TD3D_UpdateActorObjectIndex(MOVELEFTTORIGHT);
								gnKaraokOffset += SONGERPICOFFSET;
							}
							
							gnActorOffset = gnActorOffset - nOffsetLen;
							if(gnActorOffset <= -SONGERUPDATEOFFSET)
							{
								gnActorOffset += SONGERUPDATEOFFSET;
#if 0
								pthread_mutex_lock(&gUpdateActorPicMutexEnd);
								SetEvent(ActorUpdatePicEvent);
								pthread_mutex_unlock(&gUpdateActorPicMutexEnd);
#else
								if(gnActorNumState == ACTOR_NUM_STATE_BIG)
								{
									gnActorCurPageState = STATE_USEING;
									while(gnSendEventActorState != STATE_NOUSE)
									{
										usleep(100);
									}
									SetEvent(ActorUpdatePicEvent);
								}
								//ActorCheckAllObjectToUpdate(MOVELEFTTORIGHT);
#endif
							}
							gpCurClientView->ShowWindow(NULL, NULL);
						}
					}
				}

				if(nPauseDrawThread == 1)
				{
//					ActorCheckAllObjectToUpdate(MOVELEFTTORIGHT);
//					SetEvent(ActorUpdatePicEvent);
					gpCurClientView->ShowWindow(NULL, NULL);
				}
			}
		}
		else if((int)wParam == MOVERIGHTTOLEFT)
		{
			if(gpCurClientView && gpCurClientView->dwModuleID == 0x10100000)//karaok
			{
				for(i = (int)lParam; i >= 0; i--)
				{
					usleep(1);
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;
					
					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}
					
					if(nOffsetLen == 0)
					{
						break;
					}
					
					if(gnKaraokOffset + nOffsetLen >= gnMaxOffset + BUFFERLEN)//到底了
					{
						gnKaraokOffset = gnMaxOffset + BUFFERLEN;
					}
					else
					{
						gnKaraokOffset = gnKaraokOffset + nOffsetLen;
					}
					
					gpCurClientView->ShowWindow(NULL, NULL);
					
					if(nPauseDrawThread != 1)
						break;
					
					if(gnKaraokOffset + nOffsetLen >= gnMaxOffset + BUFFERLEN)//到底了
					{
						gnKaraokOffset = gnMaxOffset + BUFFERLEN;
					}
					else
					{
						gnKaraokOffset = gnKaraokOffset + nOffsetLen;
					}
					
					gpCurClientView->ShowWindow(NULL, NULL);
					
					if(gnKaraokOffset > gnMaxOffset)
					{
						gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHTLASTPAGE);
						return;
					}
				}
			}
			else if(gpCurClientView && gpCurClientView->dwModuleID == 0x11000000)//actor
			{
//				printf("(int)lParam = %d\n",(int)lParam);
				if((int)lParam > 38)
				{
					lParam = 38;
				}
				for(i = (int)lParam; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;
					
					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}
					
					if(nOffsetLen == 0)
					{
						break;
					}

					gnBGOffset = gnBGOffset + nOffsetLen;
					if(gnBGOffset >= (720 / BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 720 * BGMOVEOFFSET;
					}

					gnKaraokOffset = gnKaraokOffset + nOffsetLen;
					if(gnKaraokOffset >= SONGERPICOFFSET)
					{
						TD3D_UpdateActorObjectIndex(MOVERIGHTTOLEFT);
						gnKaraokOffset -= SONGERPICOFFSET;
					}
					
					gnActorOffset = gnActorOffset + nOffsetLen;
					if(gnActorOffset >= SONGERUPDATEOFFSET)
					{
						gnActorOffset -= SONGERUPDATEOFFSET;
#if 0
						pthread_mutex_lock(&gUpdateActorPicMutexEnd);
						SetEvent(ActorUpdatePicEvent1);
						pthread_mutex_unlock(&gUpdateActorPicMutexEnd);
#else
						if(gnActorNumState == ACTOR_NUM_STATE_BIG)
						{
							gnActorCurPageState1 = STATE_USEING;
							while(gnSendEventActorState1 != STATE_NOUSE)
							{
								usleep(100);
							}
							SetEvent(ActorUpdatePicEvent1);
						}
						//ActorCheckAllObjectToUpdate(MOVERIGHTTOLEFT);
#endif
					}
					
					gpCurClientView->ShowWindow(NULL, NULL);
					
					if(nPauseDrawThread != 1)
						break;

					gnBGOffset = gnBGOffset + nOffsetLen;
					if(gnBGOffset >= (720 / BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 720 * BGMOVEOFFSET;
					}

					gnKaraokOffset = gnKaraokOffset + nOffsetLen;
					if(gnKaraokOffset >= SONGERPICOFFSET)
					{
						TD3D_UpdateActorObjectIndex(MOVERIGHTTOLEFT);
						gnKaraokOffset -= SONGERPICOFFSET;
					}

					gnActorOffset = gnActorOffset + nOffsetLen;
					if(gnActorOffset >= SONGERUPDATEOFFSET)
					{
						gnActorOffset -= SONGERUPDATEOFFSET;
#if 0
						pthread_mutex_lock(&gUpdateActorPicMutexEnd);
						SetEvent(ActorUpdatePicEvent1);
						pthread_mutex_unlock(&gUpdateActorPicMutexEnd);
#else
						if(gnActorNumState == ACTOR_NUM_STATE_BIG)
						{
							gnActorCurPageState1 = STATE_USEING;
							while(gnSendEventActorState1 != STATE_NOUSE)
							{
								usleep(100);
							}
							SetEvent(ActorUpdatePicEvent1);
						}
						//ActorCheckAllObjectToUpdate(MOVERIGHTTOLEFT);
#endif
					}

					gpCurClientView->ShowWindow(NULL, NULL);

					if(i <= (int)lParam / 4)
					{
						for(j = 0; j < (((int)lParam - i + 1) > 5 ? 5 : (int)lParam - i + 1) ; j++)
						{
							if(nPauseDrawThread != 1)
								break;

							gnBGOffset = gnBGOffset + nOffsetLen;
							if(gnBGOffset >= (720 / BGMOVEOFFSET))
							{
								gnBGOffset = gnBGOffset - 720 * BGMOVEOFFSET;
							}
							
							gnKaraokOffset = gnKaraokOffset + nOffsetLen;
							if(gnKaraokOffset >= SONGERPICOFFSET)
							{
								TD3D_UpdateActorObjectIndex(MOVERIGHTTOLEFT);
								gnKaraokOffset -= SONGERPICOFFSET;
							}
							
							gnActorOffset = gnActorOffset + nOffsetLen;
							if(gnActorOffset >= SONGERUPDATEOFFSET)
							{
								gnActorOffset -= SONGERUPDATEOFFSET;
#if 0
								pthread_mutex_lock(&gUpdateActorPicMutexEnd);
								SetEvent(ActorUpdatePicEvent1);
								pthread_mutex_unlock(&gUpdateActorPicMutexEnd);
#else
								if(gnActorNumState == ACTOR_NUM_STATE_BIG)
								{
									gnActorCurPageState1 = STATE_USEING;
									while(gnSendEventActorState1 != STATE_NOUSE)
									{
										usleep(100);
									}
									SetEvent(ActorUpdatePicEvent1);
								}
								//ActorCheckAllObjectToUpdate(MOVERIGHTTOLEFT);
#endif
							}
							gpCurClientView->ShowWindow(NULL, NULL);
						}
					}
				}

				if(nPauseDrawThread == 1)
				{
//					ActorCheckAllObjectToUpdate(MOVERIGHTTOLEFT);
//					SetEvent(ActorUpdatePicEvent1);
					gpCurClientView->ShowWindow(NULL, NULL);
				}
			}
		}

		if(nPauseDrawThread != 1)
			return;

//		gpCurClientView->ShowWindow(NULL,NULL);
		break;
//..
	case DRAWMSG_NEWSONGMOVE:
		{
			int    iMaxPage = 0 ;
			int    iNewCurDate = 0;
			int    iNewCurObject = 0;
			if ((int)lParam >40) 
			{
				lParam = 40;
			}
			iMaxPage=(gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP;
			if((int)wParam == MOVELEFTTORIGHT)
			{
				for(i = (int)lParam; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;

					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}

					if(nOffsetLen == 0)
					{
						break;
					}

					gnNewsongOffset = gnNewsongOffset - nOffsetLen;	
					gnBGOffset = gnBGOffset - nOffsetLen;	

					if(gnBGOffset <= (-720 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 720 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (720 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 720 * BGMOVEOFFSET;
					}
					if(abs(gnNewsongOffset)>SONGBGBMPXOFFSET)
					{
						if (gnNewsongOffset > 0 ) 
						{
						//	printf("gnNewsongOffset > 0 \n");
							iNewsongObjPosMoveIndex --;	
							gnNewsongOffset = gnNewsongOffset - SONGBGBMPXOFFSET;
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
						//	printf("gnNewsongOffset <0 \n");
							iNewsongObjPosMoveIndex ++;
							gnNewsongOffset = gnNewsongOffset + SONGBGBMPXOFFSET;
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject<0 || gpCurClientView->lCurObject>iNewSongObjectNum -1)
						{
							gpCurClientView->lCurObject= ( gpCurClientView->lCurObject + iNewSongObjectNum ) % iNewSongObjectNum;
						}

						
						if (gpCurClientView->lCurData <0) 
						{
							gpCurClientView->lCurData =(gpCurClientView->lCurData+ iMaxPage)%iMaxPage;
						}
						if (gpCurClientView->lCurData >iMaxPage)
						{
							gpCurClientView->lCurData %=iMaxPage;
						}
						
						if (iNewsongObjPosMoveIndex >=iNewSongObjectNum)
						{
							iNewsongObjPosMoveIndex -=iNewSongObjectNum;
						}
						else if (iNewsongObjPosMoveIndex < 0 )
						{
							iNewsongObjPosMoveIndex += iNewSongObjectNum;
						}

						//yang add
						UpdatePageInfoToSurface();
						//..

						if (gnActorNumState == ACTOR_NUM_STATE_BIG)
						{
							//if (gpCurClientView->dwModuleID == 0x10200000) //myLib
							//{
							//	SetEvent(ThreadUpdateMyLibEvent);
							//}
							//else //newsong
							//{	
								if (gnNewSongObjMoveState == MOVERIGHTTOLEFT) 
								{
//  								iNewCurDate = (gpCurClientView->lCurData + 5 + iMaxPage)%iMaxPage;
//  								iNewCurObject = (gpCurClientView->lCurObject + 5 + iNewSongObjectNum)%iNewSongObjectNum;
									//modify by tonykong
									iNewCurDate = (gpCurClientView->lCurData + (SONG_PAGE_DCNUM*2-1) + iMaxPage)%iMaxPage;
									iNewCurObject = (gpCurClientView->lCurObject + (SONG_PAGE_DCNUM*2-1) + iNewSongObjectNum)%iNewSongObjectNum;
									//end..
									gnNewsongImageID[iNewCurObject] = iNewCurDate;
								}
								else if (gnNewSongObjMoveState == MOVELEFTTORIGHT)
								{
//  								iNewCurDate = (gpCurClientView->lCurData -3  + iMaxPage)%iMaxPage;
//  								iNewCurObject = (gpCurClientView->lCurObject -3 + iNewSongObjectNum)%iNewSongObjectNum;
									//modify by tonykong
									iNewCurDate = (gpCurClientView->lCurData -SONG_PAGE_DCNUM  + iMaxPage)%iMaxPage;
									iNewCurObject = (gpCurClientView->lCurObject -SONG_PAGE_DCNUM + iNewSongObjectNum)%iNewSongObjectNum;
									//end..
									gnNewsongImageID[iNewCurObject] = iNewCurDate;
								}
								gnNewsongImageID[iNewCurObject] = iNewCurDate;
							//	pthread_mutex_lock(&gUpdateNewsongMutex);
								SetEvent(ThreadUpdateNewsongEvent);
							//	pthread_mutex_unlock(&gUpdateNewsongMutex);
							//}
						}
					}
					if (gnActorNumState!= ACTOR_NUM_STATE_SMALL) {
						gpCurClientView->ShowWindow(NULL,NULL);
					}
				}
			}
			else if((int)wParam == MOVERIGHTTOLEFT)
			{
				for(i = (int)lParam; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;

					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}
					
					if(nOffsetLen == 0)
					{
						break;
					}
					gnNewsongOffset = gnNewsongOffset + nOffsetLen;	
					gnBGOffset = gnBGOffset + nOffsetLen;		
					if(gnBGOffset <= (-720 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 720 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (720 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 720 * BGMOVEOFFSET;
					}
					if(abs(gnNewsongOffset)>SONGBGBMPXOFFSET )
					{

						if (gnNewsongOffset > 0 ) 
						{
						//	printf("gnNewsongOffset > 0 \n");
							iNewsongObjPosMoveIndex --;	
							gnNewsongOffset = gnNewsongOffset - SONGBGBMPXOFFSET;
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
						//	printf("gnNewsongOffset <0 \n");
							iNewsongObjPosMoveIndex ++;
							gnNewsongOffset = gnNewsongOffset + SONGBGBMPXOFFSET;
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject<0 || gpCurClientView->lCurObject>iNewSongObjectNum - 1)
						{
							gpCurClientView->lCurObject= (gpCurClientView->lCurObject + iNewSongObjectNum)%iNewSongObjectNum;
						}

						if (gpCurClientView->lCurData <0 ) 
						{
							gpCurClientView->lCurData =(gpCurClientView->lCurData+ iMaxPage)%iMaxPage;
						}
						if (gpCurClientView->lCurData >iMaxPage)
						{
							gpCurClientView->lCurData %=iMaxPage;
						}
						
						if (iNewsongObjPosMoveIndex >=iNewSongObjectNum)
						{
							iNewsongObjPosMoveIndex -=iNewSongObjectNum;
						}
						else if (iNewsongObjPosMoveIndex < 0 )
						{
							iNewsongObjPosMoveIndex += iNewSongObjectNum;
						}

						//yang add
						UpdatePageInfoToSurface();
						//..

						if (gnActorNumState == ACTOR_NUM_STATE_BIG)
						{
							//if (gpCurClientView->dwModuleID == 0x10200000) //myLib
							//{
							//	SetEvent(ThreadUpdateMyLibEvent);
							//}
							//else //newsong
						//{
								if (gnNewSongObjMoveState == MOVERIGHTTOLEFT) 
								{
//  								iNewCurDate = (gpCurClientView->lCurData + 5 + iMaxPage)%iMaxPage;
//  								iNewCurObject = (gpCurClientView->lCurObject + 5 + iNewSongObjectNum)%iNewSongObjectNum;
									iNewCurDate = (gpCurClientView->lCurData + (SONG_PAGE_DCNUM*2-1) + iMaxPage)%iMaxPage;
									iNewCurObject = (gpCurClientView->lCurObject + (SONG_PAGE_DCNUM*2-1) + iNewSongObjectNum)%iNewSongObjectNum;
									gnNewsongImageID[iNewCurObject] = iNewCurDate;
								}
								else if (gnNewSongObjMoveState == MOVELEFTTORIGHT)
								{
//  								iNewCurDate = (gpCurClientView->lCurData -3  + iMaxPage)%iMaxPage;
//  								iNewCurObject = (gpCurClientView->lCurObject -3 + iNewSongObjectNum)%iNewSongObjectNum;
									iNewCurDate = (gpCurClientView->lCurData -SONG_PAGE_DCNUM + iMaxPage)%iMaxPage;
									iNewCurObject = (gpCurClientView->lCurObject -SONG_PAGE_DCNUM + iNewSongObjectNum)%iNewSongObjectNum;
									gnNewsongImageID[iNewCurObject] = iNewCurDate;
								}
								gnNewsongImageID[iNewCurObject] = iNewCurDate;
							//	pthread_mutex_lock(&gUpdateNewsongMutex);
								SetEvent(ThreadUpdateNewsongEvent);
							//	pthread_mutex_unlock(&gUpdateNewsongMutex);
							//}
						}
					}
					if (gnActorNumState!= ACTOR_NUM_STATE_SMALL) 
					{
						gpCurClientView->ShowWindow(NULL,NULL);
					}
				}
			}
			if(nPauseDrawThread != 1)
			return;
			nPauseDrawThread = 0 ;
		}
		break;
	case DRAWMSG_VIPMOVE:
		{
			int    iMaxPage = 0 ;
			TDINT  iNewCurDate = 0;
			TDINT  iNewCurObject = 0;
			if ((int)lParam > 60) 
			{
				lParam = 60 ;
			}
			iMaxPage=gpCurClientView->lDBTotal ;
			if((int)wParam == MOVELEFTTORIGHT)
			{
				for(i = (int)lParam; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;

					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}

					if(nOffsetLen == 0)
					{
						break;
					}
					
					gnNewsongOffset = gnNewsongOffset - nOffsetLen;	
					
					if(gpCurClientView->lCurData >= (gpCurClientView->lDBTotal-5) && gnNewsongOffset >= 216)
					{
						gnNewsongOffset = 216;
						VIPNormalMove();
						break;
					}

					if(abs(gnNewsongOffset)>VIP_OBJ_XOFFSET)
					{
						if (gnNewsongOffset > 0 ) 
						{
						//	printf("gnNewsongOffset > 0 \n");
							iNewsongObjPosMoveIndex --;	
							gnNewsongOffset = gnNewsongOffset - VIP_OBJ_XOFFSET;
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
						//	printf("gnNewsongOffset <0 \n");
							iNewsongObjPosMoveIndex ++;
							gnNewsongOffset = gnNewsongOffset + VIP_OBJ_XOFFSET;
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject<0 || gpCurClientView->lCurObject>iNewSongObjectNum -1)
						{
							gpCurClientView->lCurObject= ( gpCurClientView->lCurObject + iNewSongObjectNum ) % iNewSongObjectNum;
						}

						
						if (gpCurClientView->lCurData <0) 
						{
							gpCurClientView->lCurData += iMaxPage;
						}
						if (gpCurClientView->lCurData > iMaxPage)
						{
							gpCurClientView->lCurData -=iMaxPage;
						}
#if 0
						if (iNewsongObjPosMoveIndex >=iNewSongObjectNum)
						{
							iNewsongObjPosMoveIndex -=iNewSongObjectNum;
						}
						else if (iNewsongObjPosMoveIndex < 0 )
						{
							iNewsongObjPosMoveIndex += iNewSongObjectNum;
						}
                        //printf("WWWW2:  CurDate = %d, CurObject = %d, MoveIndex = %d\n",gpCurClientView->lCurData, gpCurClientView->lCurObject, iNewsongObjPosMoveIndex);
						if (gnActorNumState == ACTOR_NUM_STATE_BIG)
						{
							//while (gnInUpdateVIPThreadFun == 1);
							//setcount1 ++;
//							printf("EEEEEEEEE: setcount = %d\n",setcount1);
							SetEvent(ThreadUpdateVIPEvent);
						}
#endif
						if (gnActorNumState == VIPRUNSTATE_CIRCLE)
						{
							if (iNewsongObjPosMoveIndex >= iNewSongObjectNum)
							{
								iNewsongObjPosMoveIndex -= iNewSongObjectNum;
							}
							else if (iNewsongObjPosMoveIndex < 0 )
							{
								iNewsongObjPosMoveIndex += iNewSongObjectNum;
							}
							iNewCurDate = (gpCurClientView->lCurData - 4 + gpCurClientView->lDBTotal)%gpCurClientView->lDBTotal;
							iNewCurObject = (gpCurClientView->lCurObject - 4 + iNewSongObjectNum)%iNewSongObjectNum;
							gnVIPImageID[iNewCurObject] = iNewCurDate;
						}
						else if (gnActorNumState == VIPRUNSTATE_LINE)
						{
                            if (iNewsongObjPosMoveIndex >= 12 + 2*iNewSongObjectNum)
							{
								iNewsongObjPosMoveIndex = 12 + 2*iNewSongObjectNum;
							}
							else if (iNewsongObjPosMoveIndex <= 12 - iNewSongObjectNum )
							{
								iNewsongObjPosMoveIndex = 12 - iNewSongObjectNum;
							}
						}

						//while(gnInUpdateVIPThreadFun == 1);
						SetEvent(ThreadUpdateVIPEvent);
					}
#if 0
					if (gnActorNumState!= ACTOR_NUM_STATE_SMALL) {
						gpCurClientView->ShowWindow(NULL,NULL);
					}
#endif
					if(gpCurClientView->lCurData <= 0 && gnNewsongOffset <= 0)
					{
						gnNewsongOffset = 0;
						VIPNormalMove();
						break;
					}
					gnBGOffset = gnBGOffset - nOffsetLen;	
					if(gnBGOffset <= (-1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 1280 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 1280 * BGMOVEOFFSET;
					}
					
					VIPNormalMove();
					usleep(100);
				}
			}
			else if((int)wParam == MOVERIGHTTOLEFT)
			{
				for(i = (int)lParam; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;

					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}
					
					if(nOffsetLen == 0)
					{
						break;
					}
					gnNewsongOffset = gnNewsongOffset + nOffsetLen;	
					
					if(gpCurClientView->lCurData >= (gpCurClientView->lDBTotal-5) && gnNewsongOffset >= 216)
					{
						gnNewsongOffset = 216;
						VIPNormalMove();
						break;
					}

					if(abs(gnNewsongOffset)>VIP_OBJ_XOFFSET )
					{
						if (gnNewsongOffset > 0 ) 
						{
						//	printf("gnNewsongOffset > 0 \n");
							iNewsongObjPosMoveIndex --;	
							gnNewsongOffset = gnNewsongOffset - VIP_OBJ_XOFFSET;
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
						//	printf("gnNewsongOffset <0 \n");
							iNewsongObjPosMoveIndex ++;
							gnNewsongOffset = gnNewsongOffset + VIP_OBJ_XOFFSET;
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject<0 || gpCurClientView->lCurObject>iNewSongObjectNum - 1)
						{
							gpCurClientView->lCurObject= (gpCurClientView->lCurObject + iNewSongObjectNum)%iNewSongObjectNum;
						}

						if (gpCurClientView->lCurData <0 ) 
						{
							gpCurClientView->lCurData += iMaxPage;
						}
						if (gpCurClientView->lCurData > iMaxPage)
						{
							gpCurClientView->lCurData -= iMaxPage;
						}
#if 0
						if (iNewsongObjPosMoveIndex >=iNewSongObjectNum)
						{
							iNewsongObjPosMoveIndex -=iNewSongObjectNum;
						}
						else if (iNewsongObjPosMoveIndex < 0 )
						{
							iNewsongObjPosMoveIndex += iNewSongObjectNum;
						}
						//printf("WWWW3:  CurDate = %d, CurObject = %d, MoveIndex = %d\n",gpCurClientView->lCurData, gpCurClientView->lCurObject, iNewsongObjPosMoveIndex);
						if (gnActorNumState == ACTOR_NUM_STATE_BIG)
						{
							//while (gnInUpdateVIPThreadFun == 1);
							//setcount1 ++;
							//							printf("EEEEEEEEE: setcount = %d\n",setcount1);
							SetEvent(ThreadUpdateVIPEvent);
							
						}
#endif
						if (gnActorNumState == VIPRUNSTATE_CIRCLE)
						{
							if (iNewsongObjPosMoveIndex >= iNewSongObjectNum)
							{
								iNewsongObjPosMoveIndex -= iNewSongObjectNum;
							}
							else if (iNewsongObjPosMoveIndex < 0 )
							{
								iNewsongObjPosMoveIndex += iNewSongObjectNum;
							}
							iNewCurDate = (gpCurClientView->lCurData + 7 + gpCurClientView->lDBTotal)%gpCurClientView->lDBTotal;
							iNewCurObject = (gpCurClientView->lCurObject + 7 + iNewSongObjectNum)%iNewSongObjectNum;
							gnVIPImageID[iNewCurObject] = iNewCurDate;
						
						}
						else if (gnActorNumState == VIPRUNSTATE_LINE)
						{
                            if (iNewsongObjPosMoveIndex >= 12 + 2*iNewSongObjectNum)
							{
								iNewsongObjPosMoveIndex = 12 + 2*iNewSongObjectNum;
							}
							else if (iNewsongObjPosMoveIndex <= 12 - iNewSongObjectNum )
							{
								iNewsongObjPosMoveIndex = 12 - iNewSongObjectNum;
							}
						}


						//while(gnInUpdateVIPThreadFun == 1);
						SetEvent(ThreadUpdateVIPEvent);
					}
#if 0
					if (gnActorNumState!= ACTOR_NUM_STATE_SMALL) 
					{
						gpCurClientView->ShowWindow(NULL,NULL);
					}
#endif
					if(gpCurClientView->lCurData <= 0 && gnNewsongOffset <= 0)
					{
						gnNewsongOffset = 0;
						VIPNormalMove();
						break;
					}

					gnBGOffset = gnBGOffset + nOffsetLen;		
					if(gnBGOffset <= (-1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 1280 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 1280 * BGMOVEOFFSET;
					}

					VIPNormalMove();
					usleep(100);
				}
			}
			if(nPauseDrawThread != 1)
			return;
		}
		break;
	case DRAWMSG_SEARCHRESULTMOVE:
		{
			//printf("gntopliboffetold = %d\n",gnTopLibOffset);
			int    iMaxPage = 0;
			int    iUpdateObjectNum = 0;
			int    iUpdateObjectLargeNum = 0;
			int    iUpdateObjectItem[2] = {0,0};
			int    iUpdateCurdateItem[2] = {0,0};

			if ((int)lParam > 60) 
			{
				lParam = 60;
			}
            
			int sum2 = 0;
			int sum3 = 0;
			int j = 0;	
			int sum = 0;

            for (i= 0; i < lParam; i ++)
            {
				sum = sum + i * i / lParam;
				//printf("num = %d, vel = %d, sum =%d\n",i, i*i/lParam, sum);
            }
			iUpdateObjectLargeNum = sum / 640 + 1;

			for (i=1; i < 100; i ++)
			{
				for (j = 0; j < i;j++)
				{	
					sum2 = sum2 +j*j/i;
					//printf("num = %d, vel = %d, sum =%d\n",j, j*j/i,sum2);
				}
				if(((int)wParam == MOVELEFTTORIGHT && gnSearchResultOffset > 0) || ((int)wParam == MOVERIGHTTOLEFT && gnSearchResultOffset < 0))
				{
#if 1
					if (abs(gnSearchResultOffset)-sum2 < 0)
					{
						lParam = i -1;
						gnSearchResultOffset = -sum3 + sum/640;
						//printf("B lparam = %d,gntopliboffset = %d\n",lParam,gnTopLibOffset);
						break;
					}
#endif
				}
				else
				{
					if (( sum / 640 + 1 ) * 640-abs(gnSearchResultOffset)-sum2 < 0)
					{
						lParam = i -1;
						//gnTopLibOffset = gnTopLibOffset > 0 ? 640 - sum3 : -(640 - sum3);
						gnSearchResultOffset = ( sum / 640 + 1 ) * 640 - sum3;
						//printf("C lparam = %d,gntopliboffset = %d\n",lParam,gnTopLibOffset);
						break;
					}
				}
				sum3 = sum2;
				sum2 = 0;
			}
			
			iMaxPage=(gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP;
			//printf("@@@@ lparm = %d\n",lParam);
			if((int)wParam == MOVELEFTTORIGHT)
			{
				//printf("MOVELEFTTORIGHT\n");
				gnSearchResultOffset = gnSearchResultOffset * (-1);
				for(i = (int)lParam - 1; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
					{
						if (iUpdateObjectNum != 0 )
						{
							for (j = 0; j < iUpdateObjectNum; j ++)
							{
								//printf("function6 uption in\n");
								UpdateSearchResultDraw(6, iUpdateCurdateItem[j] - 2, iUpdateObjectItem[j] - 2);
								//printf("function6 uption out\n");
							}
						}
						break;
					}
					//printf("FFF: %d\n",(int)lParam);
					nOffsetLen = i * i / (int)lParam;
                    
					

					if(nOffsetLen == 0)
					{
						//printf("重绘 更新\n");
						//gpCurClientView->DrawWindow(0);
						SearchResultUpdateHotspot(0);
						UpdateSearchResultPageCount();
						//printf("GG\n");
						gpCurClientView->ShowWindow(NULL,NULL);
						nPauseDrawThread = 0;
						break;
					}
                    //printf("GGG: %d\n",nOffsetLen);
                    //背景移动
					gnBGOffset = gnBGOffset - nOffsetLen;		
					if(gnBGOffset <= (-1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 1280 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 1280 * BGMOVEOFFSET;
					}
					gnSearchResultOffset = gnSearchResultOffset - nOffsetLen;
					//printf("WWW : gnSearchResultOffset = %d, nOffsetLen = %d\n",gnSearchResultOffset,nOffsetLen);
					//处理三页的情况
					if ( iSearchResultObjNum == 3)
					{
						if (gnSearchResultOffset > 0)//向左
						{
							gSearchResultObjectPosition[0].x = 1280 + 36;
							gSearchResultObjectPosition[8].x = 1280 + 559;
						}
						else if(gnSearchResultOffset < 0)//向右
						{
							gSearchResultObjectPosition[0].x = -640+ 36;
							gSearchResultObjectPosition[8].x = -640 + 559;
						}
					}

					if (abs(gnSearchResultOffset)>=TOPLIBBGBMPXOFFSET) 
					{
						if (gnSearchResultOffset > 0 ) 
						{
							ignSearchResultObjPosMoveIndex --;	
							gnSearchResultOffset = gnSearchResultOffset - TOPLIBBGBMPXOFFSET;
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
							ignSearchResultObjPosMoveIndex ++;
							gnSearchResultOffset = gnSearchResultOffset + TOPLIBBGBMPXOFFSET;
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject<0 || gpCurClientView->lCurObject>(iSearchResultObjNum-1))
						{
							gpCurClientView->lCurObject= ( gpCurClientView->lCurObject + iSearchResultObjNum ) % iSearchResultObjNum;
						}

						if (gpCurClientView->lCurData <= (-1) *iMaxPage)
						{
							gpCurClientView->lCurData += iMaxPage;
						}
						if (gpCurClientView->lCurData < 0) 
						{
							gpCurClientView->lCurData += iMaxPage;
						}
						if (gpCurClientView->lCurData >= iMaxPage)
						{
							gpCurClientView->lCurData -= iMaxPage;
						}
						
						if (ignSearchResultObjPosMoveIndex >= iSearchResultObjNum)
						{
							ignSearchResultObjPosMoveIndex -= iSearchResultObjNum;
						}
						else if (ignSearchResultObjPosMoveIndex < 0 )
						{
							ignSearchResultObjPosMoveIndex += iSearchResultObjNum;
						}
						
						iUpdateObjectItem[iUpdateObjectNum] = gpCurClientView->lCurObject;
						iUpdateCurdateItem[iUpdateObjectNum] = gpCurClientView->lCurData;
                        //printf("@@@ iUpdateObjectItem[%d] = %d, iUpdateCurdateItem[%d] = %d \n",iUpdateObjectNum,iUpdateObjectItem[iUpdateObjectNum],iUpdateObjectNum,iUpdateCurdateItem[iUpdateObjectNum]);
						iUpdateObjectNum ++;
						
						if (gnUpdateSearchFashion == UPDATESEARCH_USEFUNCTION )
						{
							//printf("gnUpdateFashion == UPDATETOPLIB_USEFUNCTION\n");
							gpCurClientView->ShowWindow(NULL,NULL);
							if (iUpdateObjectNum == iUpdateObjectLargeNum)
							{
								for (j = 0; j < iUpdateObjectLargeNum; j ++)
								{
                                    //printf("function1 uption in\n");
									//UpdateTopLibDraw(6, gpCurClientView->lCurData - 2 - j, gpCurClientView->lCurObject - 2 - j);
									UpdateSearchResultDraw(6, iUpdateCurdateItem[j] - 2, iUpdateObjectItem[j] - 2);
									//printf("function1 uption out\n");
								}
								iUpdateObjectNum = 0;
							}
							
						}
						else
						{
							//printf("gnUpdateFashion == UPDATETOPLIB_USETREAD\n");
							//printf("thundermouce1  treath\n");
							SetEvent(ThreadUpdateSearchResultEvent);
							iUpdateObjectNum = 0;
						}
						
						

					}
				//	printf("gnTopLibOffset3 = %d\n",gnTopLibOffset);
					gpCurClientView->ShowWindow(NULL,NULL);
				}
			}
			else if((int)wParam == MOVERIGHTTOLEFT)
			{
				//printf("MOVERIGHTTOLEFT\n");
				for(i = (int)lParam - 1; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
					{
						if (iUpdateObjectNum != 0 )
						{
							for (j = 0; j < iUpdateObjectNum; j ++)
							{
								//printf("function7 uption in\n");
								UpdateSearchResultDraw(6, iUpdateCurdateItem[j] + 3, iUpdateObjectItem[j] + 3);
								//printf("function7 uption out\n");
							}
						}
						break;
					}
                    //printf("CCC: %d\n",(int)lParam);
					nOffsetLen = i * i / (int)lParam;

					if(nOffsetLen == 0)
					{
						//printf("重绘 更新\n");
						//gpCurClientView->DrawWindow(0);
						SearchResultUpdateHotspot(0);
						UpdateSearchResultPageCount();
						//printf("XXX\n");
						gpCurClientView->ShowWindow(NULL,NULL);
						nPauseDrawThread = 0;
						break;
					}
					//printf("VVV: %d\n",nOffsetLen);
                    //背景移动
					gnBGOffset = gnBGOffset + nOffsetLen;		
					if(gnBGOffset <= (-1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 1280 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 1280 * BGMOVEOFFSET;
					}

					gnSearchResultOffset = gnSearchResultOffset + nOffsetLen;
					//printf("HHH : gnSearchResultOffset = %d, nOffsetLen = %d\n",gnSearchResultOffset,nOffsetLen);
					//处理三页的情况
					if ( iSearchResultObjNum == 3)
					{
						if (gnSearchResultOffset > 0)//向左
						{
							gSearchResultObjectPosition[0].x = 1280 + 36;
							gSearchResultObjectPosition[8].x = 1280 + 559;
						}
						else if(gnSearchResultOffset < 0)//向右
						{
							gSearchResultObjectPosition[0].x = -640+ 36;
							gSearchResultObjectPosition[8].x = -640 + 559;
						}
					}
					
					if (abs(gnSearchResultOffset)>=TOPLIBBGBMPXOFFSET) 
					{
						if (gnSearchResultOffset > 0 ) 
						{
							ignSearchResultObjPosMoveIndex --;	
							gnSearchResultOffset = gnSearchResultOffset - TOPLIBBGBMPXOFFSET;
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
							ignSearchResultObjPosMoveIndex ++;
							gnSearchResultOffset = gnSearchResultOffset + TOPLIBBGBMPXOFFSET;
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject<0 || gpCurClientView->lCurObject>(iSearchResultObjNum-1))
						{
							gpCurClientView->lCurObject= (gpCurClientView->lCurObject + iSearchResultObjNum)%iSearchResultObjNum;
						}

						if (gpCurClientView->lCurData <= (-1) * iMaxPage)
						{
							gpCurClientView->lCurData += iMaxPage;
						}
						if (gpCurClientView->lCurData <0 ) 
						{
							gpCurClientView->lCurData += iMaxPage;
						}
						if (gpCurClientView->lCurData >= iMaxPage)
						{
							gpCurClientView->lCurData -= iMaxPage;
						}
						
						if (ignSearchResultObjPosMoveIndex >= iSearchResultObjNum)
						{
							ignSearchResultObjPosMoveIndex -= iSearchResultObjNum;
						}
						else if (ignSearchResultObjPosMoveIndex < 0)
						{
							ignSearchResultObjPosMoveIndex += iSearchResultObjNum;
						}

						iUpdateObjectItem[iUpdateObjectNum] = gpCurClientView->lCurObject;
						iUpdateCurdateItem[iUpdateObjectNum] = gpCurClientView->lCurData;
                        //printf("@@@ iUpdateObjectItem[%d] = %d, iUpdateCurdateItem[%d] = %d \n",iUpdateObjectNum,iUpdateObjectItem[iUpdateObjectNum],iUpdateObjectNum,iUpdateCurdateItem[iUpdateObjectNum]);
                        iUpdateObjectNum ++;
						
						if (gnUpdateSearchFashion == UPDATESEARCH_USEFUNCTION)
						{
							//printf("gnUpdateFashion == UPDATETOPLIB_USEFUNCTION\n");
							gpCurClientView->ShowWindow(NULL,NULL);
							if (iUpdateObjectNum == iUpdateObjectLargeNum)
							{
								for (j = 0; j < iUpdateObjectLargeNum; j ++)
								{
									//printf("function2 uption in\n");
									//UpdateTopLibDraw(6, gpCurClientView->lCurData + 3 - j, gpCurClientView->lCurObject + 3 - j);
									UpdateSearchResultDraw(6, iUpdateCurdateItem[j] + 3, iUpdateObjectItem[j] + 3);
									//printf("function2 uption out\n");
								}
								iUpdateObjectNum = 0;
							}
							
						}
						else
						{
							//printf("gnUpdateFashion == UPDATETOPLIB_USETREAD\n");
							//printf("thundermouce2  treath\n");
							SetEvent(ThreadUpdateSearchResultEvent);
							iUpdateObjectNum = 0;
						}
						
						

					}
					gpCurClientView->ShowWindow(NULL,NULL);					
				}
			}
            
			gnSearchStopDuringMove = 0;
            
			if(nPauseDrawThread == 1)
				nPauseDrawThread = 0;
			gnSearchStopDuringMove = 0;
        //printf("out of DRAWMSG_SEARCHRESULTMOVE\n");
		}
		break;
	case DRAWMSG_TOPLIBMOVE:
		{
//			printf("wnddrawproc  DRAWMSG_TOPLIBMOVE  \n");
			//printf("gntopliboffetold = %d\n",gnTopLibOffset);
			int    iMaxPage = 0 ;
			int    iUpdateObjectNum = 0;
			int    iUpdateObjectLargeNum = 0;
			int    iUpdateObjectItem[2] = {0,0};
			int    iUpdateCurdateItem[2] = {0,0};
		
			if ((int)lParam >60) 
			{
				lParam = 60 ;
			}
            
			int sum2 = 0;
			int sum3 = 0;
			int j    = 0;
			int sum  = 0;
			//int iLoopNum = 0;
			
			for (i= 0; i < lParam; i ++)
            {
				sum = sum + i * i / lParam;
				//printf("num = %d, vel = %d, sum =%d\n",i, i*i/lParam, sum);
            }
			iUpdateObjectLargeNum = sum / 640 + 1;
            //printf("@@@  iUpdateObjectLargeNum = %d\n",iUpdateObjectLargeNum);
            //printf("AAAAA:  lParam = %d,  sum = %d\n",lParam, sum);
            //确定移动距离
			//gnTopLibOffset = ( sum / 640 + 1 ) * 640 - sum;
			
			for (i=1; i < 100; i ++)
			{
				for (j = 0; j < i;j++)
				{	
					sum2 = sum2 +j*j/i;
					//printf("num = %d, vel = %d, sum =%d\n",j, j*j/i,sum2);
				}
				if(((int)wParam == MOVELEFTTORIGHT && gnTopLibOffset > 0) || ((int)wParam == MOVERIGHTTOLEFT && gnTopLibOffset < 0))
				{
#if 1
					if (abs(gnTopLibOffset)-sum2 < 0)
					{
						lParam = i -1;
						gnTopLibOffset = -sum3 + sum/640;
						//printf("B lparam = %d,gntopliboffset = %d\n",lParam,gnTopLibOffset);
						break;
					}
#endif
				}
				else
				{
					if (( sum / 640 + 1 ) * 640-abs(gnTopLibOffset)-sum2 < 0)
					{
						lParam = i -1;
						//gnTopLibOffset = gnTopLibOffset > 0 ? 640 - sum3 : -(640 - sum3);
						gnTopLibOffset = ( sum / 640 + 1 ) * 640 - sum3;
						//printf("C lparam = %d,gntopliboffset = %d\n",lParam,gnTopLibOffset);
						break;
					}
				}
				sum3 = sum2;
				sum2 = 0;
			}
			//printf(" i = %d\n",i);
		    //printf("gntopliboffet = %d\n",gnTopLibOffset);

			//printf("lParam = %d, gnTopLibOffset = %d\n", lParam,gnTopLibOffset);
			iMaxPage=(gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP;

			if((int)wParam == MOVELEFTTORIGHT)
			{
				gnTopLibOffset = gnTopLibOffset * (-1);
				for(i = (int)lParam - 1; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
					{
						if (iUpdateObjectNum != 0 )
						{
							for (j = 0; j < iUpdateObjectNum; j ++)
							{
								//printf("function6 uption in\n");
								UpdateTopLibDraw(6, iUpdateCurdateItem[j] - 2, iUpdateObjectItem[j] - 2);
								//printf("function6 uption out\n");
							}
						}
						break;
					}
					nOffsetLen = i * i / (int)lParam;
                    
					if(nOffsetLen == 0)
					{
						//printf("重绘 更新\n");
						//gpCurClientView->DrawWindow(0);
						TopLibUpdateHotspot();
						UpdateTopLibPageCount();
						gpCurClientView->ShowWindow(NULL,NULL);
						nPauseDrawThread = 0;
						break;
					}
                    
                    //背景移动
					gnBGOffset = gnBGOffset - nOffsetLen;		
					if(gnBGOffset <= (-1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 1280 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 1280 * BGMOVEOFFSET;
					}
					gnTopLibOffset = gnTopLibOffset - nOffsetLen;
					//printf("左 到 右 gnTopLibOffset = %d\n",gnTopLibOffset);
					//处理三页的情况
					if ( iTopLibObjectNum == 3)
					{
						if (gnTopLibOffset > 0)//向左
						{
							gTopLibObjectPosition[0].x = 1280 + 36;
							gTopLibObjectPosition[8].x = 1280 + 559;
						}
						else if(gnTopLibOffset < 0)//向右
						{
							gTopLibObjectPosition[0].x = -640+ 36;
							gTopLibObjectPosition[8].x = -640 + 559;
						}
					}
					if (abs(gnTopLibOffset)>=TOPLIBBGBMPXOFFSET) 
					{
											
						if (gnTopLibOffset > 0 ) 
						{
						//	printf("gnNewsongOffset > 0 \n");
							iTopLibObjPosMoveIndex --;	
							gnTopLibOffset = gnTopLibOffset - TOPLIBBGBMPXOFFSET;//SONGBGBMPXOFFSET;
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
						//	printf("gnNewsongOffset <0 \n");
							iTopLibObjPosMoveIndex ++;
							gnTopLibOffset = gnTopLibOffset + TOPLIBBGBMPXOFFSET;//SONGBGBMPXOFFSET;
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject<0 || gpCurClientView->lCurObject>(iTopLibObjectNum-1))
						{
							gpCurClientView->lCurObject= ( gpCurClientView->lCurObject + iTopLibObjectNum ) % iTopLibObjectNum;
						}

						if (gpCurClientView->lCurData <= (-1) *iMaxPage)
						{
							gpCurClientView->lCurData +=iMaxPage;
						}
						if (gpCurClientView->lCurData < 0) 
						{
							gpCurClientView->lCurData += iMaxPage;
						}
						if (gpCurClientView->lCurData >=iMaxPage)
						{
							gpCurClientView->lCurData -=iMaxPage;
						}
						
						if (iTopLibObjPosMoveIndex >=iTopLibObjectNum)
						{
							iTopLibObjPosMoveIndex -=iTopLibObjectNum;
						}
						else if (iTopLibObjPosMoveIndex < 0 )
						{
							iTopLibObjPosMoveIndex += iTopLibObjectNum;
						}
						
						iUpdateObjectItem[iUpdateObjectNum] = gpCurClientView->lCurObject;
						iUpdateCurdateItem[iUpdateObjectNum] = gpCurClientView->lCurData;
                        //printf("@@@ iUpdateObjectItem[%d] = %d, iUpdateCurdateItem[%d] = %d \n",iUpdateObjectNum,iUpdateObjectItem[iUpdateObjectNum],iUpdateObjectNum,iUpdateCurdateItem[iUpdateObjectNum]);
						iUpdateObjectNum ++;
					
						if (gnUpdateFashion == UPDATETOPLIB_USEFUNCTION )
						{
							//printf("gnUpdateFashion == UPDATETOPLIB_USEFUNCTION\n");
							gpCurClientView->ShowWindow(NULL,NULL);
							if (iUpdateObjectNum == iUpdateObjectLargeNum)
							{
								for (j = 0; j < iUpdateObjectLargeNum; j ++)
								{
                                    //printf("function1 uption in\n");
									//UpdateTopLibDraw(6, gpCurClientView->lCurData - 2 - j, gpCurClientView->lCurObject - 2 - j);
									UpdateTopLibDraw(6, iUpdateCurdateItem[j] - 2, iUpdateObjectItem[j] - 2);
									//printf("function1 uption out\n");
								}
								iUpdateObjectNum = 0;
							}
							
						}
						else
						{
							//printf("gnUpdateFashion == UPDATETOPLIB_USETREAD\n");
							//printf("thundermouce1  treath\n");
							SetEvent(ThreadUpdateTopLibEvent);
							iUpdateObjectNum = 0;
						}
						
					}
				    //printf("gnTopLibOffset3 = %d\n",gnTopLibOffset);
					//TopLibNormalMove(-gnTopLibOffset);
					gpCurClientView->ShowWindow(NULL,NULL);
					
				}
			}
			else if((int)wParam == MOVERIGHTTOLEFT)
			{
				for(i = (int)lParam - 1; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
					{
						if (iUpdateObjectNum != 0 )
						{
							for (j = 0; j < iUpdateObjectNum; j ++)
							{
								//printf("function7 uption in\n");
								UpdateTopLibDraw(6, iUpdateCurdateItem[j] + 3, iUpdateObjectItem[j] + 3);
								//printf("function7 uption out\n");
							}
						}
						break;
					}
					nOffsetLen = i * i / (int)lParam;
           
					if(nOffsetLen == 0)
					{
//						printf("重绘 更新\n");
						//gpCurClientView->DrawWindow(0);
						TopLibUpdateHotspot();
						UpdateTopLibPageCount();
						gpCurClientView->ShowWindow(NULL,NULL);
						nPauseDrawThread = 0;
						break;
					}
					
                    //背景移动
					gnBGOffset = gnBGOffset + nOffsetLen;		
					if(gnBGOffset <= (-1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 1280 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 1280 * BGMOVEOFFSET;
					}
					gnTopLibOffset = gnTopLibOffset + nOffsetLen;
					//printf("右 到 左 gnTopLibOffset = %d\n",gnTopLibOffset);
					//处理三页的情况
					if ( iTopLibObjectNum == 3)
					{
						if (gnTopLibOffset > 0)//向左
						{
							gTopLibObjectPosition[0].x = 1280 + 36;
							gTopLibObjectPosition[8].x = 1280 + 559;
						}
						else if(gnTopLibOffset < 0)//向右
						{
							gTopLibObjectPosition[0].x = -640+ 36;
							gTopLibObjectPosition[8].x = -640 + 559;
						}
					}
					
					
					if (abs(gnTopLibOffset)>=TOPLIBBGBMPXOFFSET) 
					{
						//iUpdateObjectNum ++;
						if (gnTopLibOffset > 0 ) 
						{
						//	printf("gnNewsongOffset > 0 \n");
							iTopLibObjPosMoveIndex --;	
							gnTopLibOffset = gnTopLibOffset - TOPLIBBGBMPXOFFSET;
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
						//	printf("gnNewsongOffset <0 \n");
							iTopLibObjPosMoveIndex ++;
							gnTopLibOffset = gnTopLibOffset + TOPLIBBGBMPXOFFSET;
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject<0 || gpCurClientView->lCurObject>(iTopLibObjectNum-1))
						{
							gpCurClientView->lCurObject= (gpCurClientView->lCurObject + iTopLibObjectNum)%iTopLibObjectNum;
						}

						
						if (gpCurClientView->lCurData <= (-1) *iMaxPage)
						{
							gpCurClientView->lCurData +=iMaxPage;
						}
						if (gpCurClientView->lCurData <0 ) 
						{
							gpCurClientView->lCurData += iMaxPage;
						}
						if (gpCurClientView->lCurData >=iMaxPage)
						{
							gpCurClientView->lCurData -=iMaxPage;
						}
						
						if (iTopLibObjPosMoveIndex >=iTopLibObjectNum)
						{
							iTopLibObjPosMoveIndex -=iTopLibObjectNum;
						}
						else if (iTopLibObjPosMoveIndex < 0 )
						{
							iTopLibObjPosMoveIndex += iTopLibObjectNum;
						}
						
						iUpdateObjectItem[iUpdateObjectNum] = gpCurClientView->lCurObject;
						iUpdateCurdateItem[iUpdateObjectNum] = gpCurClientView->lCurData;
                        //printf("@@@ iUpdateObjectItem[%d] = %d, iUpdateCurdateItem[%d] = %d \n",iUpdateObjectNum,iUpdateObjectItem[iUpdateObjectNum],iUpdateObjectNum,iUpdateCurdateItem[iUpdateObjectNum]);
                        iUpdateObjectNum ++;

						if (gnUpdateFashion == UPDATETOPLIB_USEFUNCTION)
						{
							//printf("gnUpdateFashion == UPDATETOPLIB_USEFUNCTION\n");
							gpCurClientView->ShowWindow(NULL,NULL);
							if (iUpdateObjectNum == iUpdateObjectLargeNum)
							{
								for (j = 0; j < iUpdateObjectLargeNum; j ++)
								{
									//printf("function2 uption in\n");
								    //UpdateTopLibDraw(6, gpCurClientView->lCurData + 3 - j, gpCurClientView->lCurObject + 3 - j);
								    UpdateTopLibDraw(6, iUpdateCurdateItem[j] + 3, iUpdateObjectItem[j] + 3);
									//printf("function2 uption out\n");
								}
								iUpdateObjectNum = 0;
							}
						    
						}
						else
						{
							//printf("gnUpdateFashion == UPDATETOPLIB_USETREAD\n");
							//printf("thundermouce2  treath\n");
							SetEvent(ThreadUpdateTopLibEvent);
							iUpdateObjectNum = 0;
						}
					}
					//TopLibNormalMove(-gnTopLibOffset);
					gpCurClientView->ShowWindow(NULL,NULL);
				}
			}
            
            //gnTopLibOffset = 0;
			gnStopDuringMove = 0;
            
		//	if(nPauseDrawThread == 1)
			nPauseDrawThread = 0;

			gpCurClientView->ShowWindow(NULL,NULL);
			
//			printf("wnddrawproc  DRAWMSG_TOPLIBMOVE  EEEEEndd  \n");
		}
		break;
	case DRAWMSG_TOPMENUSINGERINFOMOVE:
		{
			int iLoop = 0;
			int jLoop = 0;
			
			if ((int)wParam == SINGERINFO_UPTODOWN || (int)wParam == SINGERINFO_DOWNTOUP)
			{
				
				if ((int)lParam > 36)
				{
					lParam = 36;
				}
				
				if ((int)lParam == 0)
					lParam = 1;  
		
				
			//	for(iLoop = (int)lParam; iLoop >= 0;iLoop--)
				iLoop = (int)lParam * 10;
				while(1)
				{
					
				//	nOffsetLen = iLoop * iLoop / lParam;//(int)lParam;
					
					if(nPauseDrawThread != 1)
					{
//						printf("移动过程停止!\n");
						break;
					}

				//	if(iLoop == (int)lParam)
				//	{
				//		nOffsetLen = nOffsetLen / 2;
				//	}
			
					nOffsetLen = iLoop / 15;
					iLoop = iLoop - nOffsetLen;
					if(nOffsetLen == 0)
					{
						jLoop = gnSingerInfoMoveLength%36;
						nOffsetLen = 1;
						iLoop = 0;
						if(jLoop == 0)
						break;
					}
					//printf("nOffsetLen = %d\n",nOffsetLen);
					if((int)wParam == SINGERINFO_UPTODOWN)
					{
						//定位总移动长度
						gnSingerInfoMoveLength = gnSingerInfoMoveLength - nOffsetLen;
						//定位页移动长度
						gnSingerInfoOffsetLength = gnSingerInfoOffsetLength - nOffsetLen;
						
					}
					else if ((int)wParam == SINGERINFO_DOWNTOUP)
					{
						gnSingerInfoMoveLength = gnSingerInfoMoveLength + nOffsetLen;
						gnSingerInfoOffsetLength = gnSingerInfoOffsetLength + nOffsetLen;	
					}
					
					if (gnSingerInfoMoveLength <= 0)
					{
#if 0
						if (gnSingerInfoOffsetLength <= -360)
						{
							gnSingerInfoOffsetLength = -360;
						}
						else
						{
							gnSingerInfoOffsetLength = 0;
						}
#endif
						gnSingerInfoOffsetLength = -gpCurTopMenu->lCurData*gpCurTopMenu->iCPP*36;
						gnSingerInfoMoveLength = 0;
					}
					if(gnSingerInfoMoveLength >= (gpCurTopMenu->lDBTotal - 10)*36)
					{
						
						gnSingerInfoOffsetLength = (gpCurTopMenu->lDBTotal - (gpCurTopMenu->lCurData+1)*gpCurTopMenu->iCPP)*36;//(gpCurTopMenu->lDBTotal%gpCurTopMenu->iCPP)*36;
						gnSingerInfoMoveLength = (gpCurTopMenu->lDBTotal - 10)*36;
					}

					//printf("GGGGG: %d, DDDDD: %d, HHHHH: %d\n",gnSingerInfoMoveLength, gnSingerInfoOffsetLength, nOffsetLen);
					if (abs(gnSingerInfoOffsetLength)>=(TOPMENUSINGERINFOENDY-TOPMENUSINGERINFOSTARTY)) 
					{
//						printf("更新数据\n");
						if (gnSingerInfoOffsetLength > 0 ) //从 下 往 上 移动
						{
							gnSingerInfoOffsetLength = gnSingerInfoOffsetLength - (TOPMENUSINGERINFOENDY-TOPMENUSINGERINFOSTARTY);
							gpCurTopMenu->lCurData ++;
							//更新数据
							TDBitBlt(ghScrMemDC4,
								gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
								gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
								TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX,
								TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY,
								ghScrMemDC2,
								gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
								gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
								0
								);
							TDBitBlt(ghScrMemDC2,
								gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
								gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
								TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX,
								TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY,
								ghScrMemDC3,
								gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
								gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
								0
								);
							
							if (gpCurTopMenu->lCurData * gpCurTopMenu->iCPP >= gpCurTopMenu->lDBTotal)
							{
								gpCurTopMenu->lCurData = 0;
							}
							gpCurTopMenu->lCurData++;
							//gpCurTopMenu->DrawWindow(1);
							SingerInfoUpDataDraw(1);
							gpCurTopMenu->lCurData--;
							
						}
						else  //从 上 往 下 移动
						{	
							gnSingerInfoOffsetLength = gnSingerInfoOffsetLength + (TOPMENUSINGERINFOENDY-TOPMENUSINGERINFOSTARTY);
							gpCurTopMenu->lCurData --;
							//printf("curDate = %d\n",gpCurTopMenu->lCurData);
							TDBitBlt(ghScrMemDC3,
								gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
								gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
								TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX,
								TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY,
								ghScrMemDC2,
								gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
								gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
								0
								);
							TDBitBlt(ghScrMemDC2,
								gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
								gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
								TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX,
								TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY,
								ghScrMemDC4,
								gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
								gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
								0
								);
							
							if (gpCurTopMenu->lCurData < 0)
							{
								gpCurTopMenu->lCurData = (gpCurTopMenu->lDBTotal + gpCurTopMenu->iCPP - 1) / gpCurTopMenu->iCPP-1;
							}
							gpCurTopMenu->lCurData--;
							//gpCurTopMenu->DrawWindow(-1);
							SingerInfoUpDataDraw(-1);
							gpCurTopMenu->lCurData++;
							//printf("curDate2 = %d\n",gpCurTopMenu->lCurData);
						}
					}
					
#if 0				
					
					if (gnSingerInfoMoveLength <= 0)
					{
						gnSingerInfoOffsetLength = 0;
						gnSingerInfoMoveLength = 0;
					}
					if(gnSingerInfoMoveLength >= (gpCurTopMenu->lDBTotal - 10)*36)
					{
						gnSingerInfoMoveLength = (gpCurTopMenu->lDBTotal - 10)*36;
						gnSingerInfoOffsetLength = (gpCurTopMenu->lDBTotal%gpCurTopMenu->iCPP)*36;
					}
#endif					
					//滚动条长度和位置定位
					if (gpCurTopMenu->lDBTotal > 10)
					{
						iScrollBarLength = (int)((10.0/gpCurTopMenu->lDBTotal)*(TOPMENUSINGERINFOENDY-TOPMENUSINGERINFOSTARTY));
						iScrollBarIndex = (int)((gnSingerInfoMoveLength/((gpCurTopMenu->lDBTotal - 10)*36.0))*(TOPMENUSINGERINFOENDY-TOPMENUSINGERINFOSTARTY - iScrollBarLength));
						
						TOPMENU_SCROLLBAR(ghDC, 222+iScrollBarIndex,905,iScrollBarLength);
					}
					
					if(gnSingerInfoOffsetLength >= 0)
					{
						
						//gpCurTopMenu->lCurData = gnSingerInfoMoveLength/(TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY); 
						//printf("v s1: = %d, s2 = %d, s3 = %d, s4 = %d\n",gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX, gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY, TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX, TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY);
						TH_SINGERINFO_MOVEPAGE(ghScrMemDC1, 
							ghScrMemDC2,
							gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
							gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
							TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX,
							TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY,
							ghScrMemDC3,
							gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
							gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
							TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX,
							TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY,
							-gnSingerInfoOffsetLength 
							);	
					}						
					else
					{
						//gpCurTopMenu->lCurData = gnSingerInfoMoveLength/(TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY);
						//printf("h s1: = %d, s2 = %d, s3 = %d, s4 = %d\n",gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX, gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY, TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX, TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY);
						TH_SINGERINFO_MOVEPAGE(ghScrMemDC1, 
							ghScrMemDC2,
							gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
							gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
							TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX,
							TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY,
							ghScrMemDC4,
							gpCurTopMenu->Rect.left + TOPMENUSINGERINFOSTARTX,
							gpCurTopMenu->Rect.top + TOPMENUSINGERINFOSTARTY,
							TOPMENUSINGERINFOENDX - TOPMENUSINGERINFOSTARTX,
							TOPMENUSINGERINFOENDY - TOPMENUSINGERINFOSTARTY,
							-gnSingerInfoOffsetLength 
							);
					}
					//usleep(4000);
			}
			}
			if(nPauseDrawThread == 1)
			nPauseDrawThread = 0;		
			}
			break;
	case DRAWMSG_DISCOUNTMOVE:
		{
			TDINT  iDiscountCurDate = 0;
			TDINT  iDiscountCurObject = 0;
			if ((int)lParam > 60) 
			{
				lParam = 60 ;
			}
			
			if((int)wParam == DISCOUNTMOVE_LEFT_RIGHT)
			{
				for(i = (int)lParam; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;

					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}

					if(nOffsetLen == 0)
					{
						break;
					}
					
					gnDiscountOffset = gnDiscountOffset - nOffsetLen;

					if( gpCurClientView->lCurData >= (gpCurClientView->lDBTotal-5) && gnDiscountOffset >= 216 )
					{
						gnDiscountOffset = 216;
						DiscountNormalMove(-gnDiscountOffset);
						break;
					}

					if(abs(gnDiscountOffset) > (DISCOUNTIMAGEWIDTH + DISCOUNTIMAGEINTERVAL))
					{
						if (gnDiscountOffset > 0 ) 
						{
							iDiscountObjMoveIndex --;	
							gnDiscountOffset = gnDiscountOffset - (DISCOUNTIMAGEWIDTH + DISCOUNTIMAGEINTERVAL);
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
							iDiscountObjMoveIndex ++;
							gnDiscountOffset = gnDiscountOffset + (DISCOUNTIMAGEWIDTH + DISCOUNTIMAGEINTERVAL);
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject < 0 || gpCurClientView->lCurObject > iDiscountObjectNum -1)
						{
							gpCurClientView->lCurObject = ( gpCurClientView->lCurObject + iDiscountObjectNum ) % iDiscountObjectNum;
						}

						
						if (gpCurClientView->lCurData < 0) 
						{
							gpCurClientView->lCurData += gpCurClientView->lDBTotal;
						}
						if (gpCurClientView->lCurData > gpCurClientView->lDBTotal)
						{
							gpCurClientView->lCurData -= gpCurClientView->lDBTotal;
						}
						
						if (gnDiscountRunState == DISCOUNTRUNSTATE_CIRCLE)
						{
							if (iDiscountObjMoveIndex >= iDiscountObjectNum)
							{
								iDiscountObjMoveIndex -= iDiscountObjectNum;
							}
							else if (iDiscountObjMoveIndex < 0 )
							{
								iDiscountObjMoveIndex += iDiscountObjectNum;
							}
							iDiscountCurDate = (gpCurClientView->lCurData - 4 + gpCurClientView->lDBTotal)%gpCurClientView->lDBTotal;
							iDiscountCurObject = (gpCurClientView->lCurObject - 4 + iDiscountObjectNum)%iDiscountObjectNum;
							gnDiscountImageID[iDiscountCurObject] = iDiscountCurDate;
							
						}
						else if (gnDiscountRunState == DISCOUNTRUNSTATE_LINE)
						{
                            if (iDiscountObjMoveIndex >= 12 + 2*iDiscountObjectNum)
							{
								iDiscountObjMoveIndex = 12 + 2*iDiscountObjectNum;
							}
							else if (iDiscountObjMoveIndex <= 12 - iDiscountObjectNum )
							{
								iDiscountObjMoveIndex = 12 - iDiscountObjectNum;
							}
						}


						//while(gnInDiscountThreadFun == 1);
						SetEvent(ThreadUpdateDiscountEvent);
					}
					
					if( gpCurClientView->lCurData <= 0 && gnDiscountOffset <= 0 ) 
					{
						gnDiscountOffset = 0;
						DiscountNormalMove(-gnDiscountOffset);
						break;
					}
					
					gnBGOffset = gnBGOffset - nOffsetLen;	
					
					if(gnBGOffset <= (-1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 1280 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 1280 * BGMOVEOFFSET;
					}

					DiscountNormalMove(-gnDiscountOffset);
					usleep(100);
				}
			}
			else if((int)wParam == DISCOUNTMOVE_RIGHT_LEFT)
			{
				for(i = (int)lParam; i >= 0; i--)
				{
					if(nPauseDrawThread != 1)
						break;
					
					nOffsetLen = i * i / (int)lParam;

					if(i == (int)lParam)
					{
						nOffsetLen = nOffsetLen / 2;
					}
					
					if(nOffsetLen == 0)
					{
						break;
					}
					gnDiscountOffset = gnDiscountOffset + nOffsetLen;
					
					if(gpCurClientView->lCurData >= (gpCurClientView->lDBTotal-5) && gnDiscountOffset >= 216)
					{
						gnDiscountOffset = 216;
						DiscountNormalMove(-gnDiscountOffset);
						break;
					}

					if(abs(gnDiscountOffset) > (DISCOUNTIMAGEWIDTH + DISCOUNTIMAGEINTERVAL) )
					{

						if (gnDiscountOffset > 0 ) 
						{
							iDiscountObjMoveIndex --;	
							gnDiscountOffset = gnDiscountOffset - (DISCOUNTIMAGEWIDTH + DISCOUNTIMAGEINTERVAL);
							gpCurClientView->lCurData ++;
							gpCurClientView->lCurObject ++;
						}
						else
						{	
							iDiscountObjMoveIndex ++;
							gnDiscountOffset = gnDiscountOffset + (DISCOUNTIMAGEWIDTH + DISCOUNTIMAGEINTERVAL);
							gpCurClientView->lCurData --;
							gpCurClientView->lCurObject --;
						}
						if (gpCurClientView->lCurObject < 0 || gpCurClientView->lCurObject > iDiscountObjectNum - 1)
						{
							gpCurClientView->lCurObject = (gpCurClientView->lCurObject + iDiscountObjectNum)%iDiscountObjectNum;
						}

						if (gpCurClientView->lCurData < 0 ) 
						{
							gpCurClientView->lCurData += gpCurClientView->lDBTotal;
						}
						if (gpCurClientView->lCurData > gpCurClientView->lDBTotal)
						{
							gpCurClientView->lCurData -= gpCurClientView->lDBTotal;
						}
						
						if (gnDiscountRunState == DISCOUNTRUNSTATE_CIRCLE)
						{
							if (iDiscountObjMoveIndex >= iDiscountObjectNum)
							{
								iDiscountObjMoveIndex -= iDiscountObjectNum;
							}
							else if (iDiscountObjMoveIndex < 0 )
							{
								iDiscountObjMoveIndex += iDiscountObjectNum;
							}
							iDiscountCurDate = (gpCurClientView->lCurData + 7 + gpCurClientView->lDBTotal)%gpCurClientView->lDBTotal;
							iDiscountCurObject = (gpCurClientView->lCurObject + 7 + iDiscountObjectNum)%iDiscountObjectNum;
							gnDiscountImageID[iDiscountCurObject] = iDiscountCurDate;
							
						}
						else if (gnDiscountRunState == DISCOUNTRUNSTATE_LINE)
						{
                            if (iDiscountObjMoveIndex >= 12 + 2*iDiscountObjectNum)
							{
								iDiscountObjMoveIndex = 12 + 2*iDiscountObjectNum;
							}
							else if (iDiscountObjMoveIndex <= 12 - iDiscountObjectNum )
							{
								iDiscountObjMoveIndex = 12 - iDiscountObjectNum;
							}
						}

						
						//while(gnInDiscountThreadFun == 1);
						SetEvent(ThreadUpdateDiscountEvent);
					}

					if(gpCurClientView->lCurData <= 0 && gnDiscountOffset <= 0)
					{
						gnDiscountOffset = 0;
						DiscountNormalMove(-gnDiscountOffset);
						break;
					}
					
					gnBGOffset = gnBGOffset + nOffsetLen;	
					
					if(gnBGOffset <= (-1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset + 1280 * BGMOVEOFFSET;
					}
					if(gnBGOffset >= (1280 * BGMOVEOFFSET))
					{
						gnBGOffset = gnBGOffset - 1280 * BGMOVEOFFSET;
					}

					DiscountNormalMove(-gnDiscountOffset);
					usleep(100);
				}
			}
			if(nPauseDrawThread != 1)
			return;
			}
		    break;
	case DRAWMSG_ALBUMMOVE:
		gnAlbumDestoryFlag = 1;
		if((int)wParam == MOVETOPTOBOTTOM)
		{
			if(gpCurClientView && gpCurClientView->dwModuleID == 0x30300000)
			{
				if(gnAlbumDownState == ALBUM_DOWN_STATE_PIC)
				{
					if((int)lParam > 25)
					{
						lParam = 25;
					}
					for(i = (int)lParam; i >= 0; i--)
					{
						if(nPauseDrawThread != 1)
							break;
						
						nOffsetLen = i * i / (int)lParam;
						
						if(i == (int)lParam)
						{
							nOffsetLen = nOffsetLen / 2;
						}
						
						if(nOffsetLen == 0)
						{
							break;
						}
						
						gnAlbumPicOffset = gnAlbumPicOffset + nOffsetLen;
						
						if(gnAlbumPicOffset >= ALBUM_IMGW + ALBUM_OFFSET_Y)
						{
							TD3D_UpdateAlbumObjectIndex(MOVETOPTOBOTTOM);
							gnAlbumPicOffset = gnAlbumPicOffset - ALBUM_IMGW - ALBUM_OFFSET_Y;
							gnAlbumEventState = ALBUMEVENTSTATE_TOPTOBOTTOM;
						}

						//yang add for album update
						gnKaraokOffset = gnKaraokOffset + nOffsetLen;
						if(gnKaraokOffset >= ALBUM_UPDATE_OFFSET)
						{
							gnKaraokOffset -= ALBUM_UPDATE_OFFSET;
							
							gnActorCurPageState = STATE_USEING;
							while(gnSendEventActorState != STATE_NOUSE)
							{
								usleep(100);
							}
							SetEvent(Recv3DPicDelayBegin1Event);
						}
						//..
						
						gpCurClientView->ShowWindow(NULL, NULL);
						
						if(nPauseDrawThread != 1)
							break;
						
						gnAlbumPicOffset = gnAlbumPicOffset + nOffsetLen;
						
						if(gnAlbumPicOffset >= ALBUM_IMGW + ALBUM_OFFSET_Y)
						{
							TD3D_UpdateAlbumObjectIndex(MOVETOPTOBOTTOM);
							gnAlbumPicOffset = gnAlbumPicOffset - ALBUM_IMGW - ALBUM_OFFSET_Y;
							gnAlbumEventState = ALBUMEVENTSTATE_TOPTOBOTTOM;
						}

						//yang add for album update
						gnKaraokOffset = gnKaraokOffset + nOffsetLen;
						if(gnKaraokOffset >= ALBUM_UPDATE_OFFSET)
						{
							gnKaraokOffset -= ALBUM_UPDATE_OFFSET;
							
							gnActorCurPageState = STATE_USEING;
							while(gnSendEventActorState != STATE_NOUSE)
							{
								usleep(100);
							}
							SetEvent(Recv3DPicDelayBegin1Event);
						}
						//..

						gpCurClientView->ShowWindow(NULL, NULL);
						
						if(i <= (int)lParam / 4)
						{
							for(j = 0; j < (((int)lParam - i + 1) > 5 ? 5 : (int)lParam - i + 1) ; j++)
							{
								if(nPauseDrawThread != 1)
									break;
								
								gnAlbumPicOffset = gnAlbumPicOffset + nOffsetLen;
								
								if(gnAlbumPicOffset >= ALBUM_IMGW + ALBUM_OFFSET_Y)
								{
									TD3D_UpdateAlbumObjectIndex(MOVETOPTOBOTTOM);
									gnAlbumPicOffset = gnAlbumPicOffset - ALBUM_IMGW - ALBUM_OFFSET_Y;
									gnAlbumEventState = ALBUMEVENTSTATE_TOPTOBOTTOM;
								}

								//yang add for album update
								gnKaraokOffset = gnKaraokOffset + nOffsetLen;
								if(gnKaraokOffset >= ALBUM_UPDATE_OFFSET)
								{
									gnKaraokOffset -= ALBUM_UPDATE_OFFSET;
									
									gnActorCurPageState = STATE_USEING;
									while(gnSendEventActorState != STATE_NOUSE)
									{
										usleep(100);
									}
									SetEvent(Recv3DPicDelayBegin1Event);
								}
								//..
								gpCurClientView->ShowWindow(NULL, NULL);
							}
						}
					}
					
					if(nPauseDrawThread == 1)
					{
//						ActorCheckAllObjectToUpdate(MOVELEFTTORIGHT);
//						SetEvent(ActorUpdatePicEvent);
						gpCurClientView->ShowWindow(NULL, NULL);
					}
				}
				else if(gnAlbumDownState == ALBUM_DOWN_STATE_SONG)
				{
					if((int)lParam > 25)
					{
						lParam = 25;
					}
					for(i = (int)lParam; i >= 0; i--)
					{
						if(nPauseDrawThread != 1)
						{
							break;
						}
						
						nOffsetLen = i * i / (int)lParam;
						
						if(i == (int)lParam)
						{
							nOffsetLen = nOffsetLen / 2;
						}
						
						if(nOffsetLen == 0)
						{
							break;
						}
						
						if(gnSelectAlbumSongCurData != 0)
						{
							gnAlbumSongOffset = gnAlbumSongOffset + nOffsetLen;
							
							if(gnAlbumSongOffset >= ALBUMSONGIMGHEIGHT)
							{
								TD3D_UpdateAlbumObjectIndex(MOVETOPTOBOTTOM);
								if(gnSelectAlbumSongCurData == 0)
								{
									gnAlbumSongOffset = 0;
								}
								else
								{
									gnAlbumSongOffset -= ALBUMSONGIMGHEIGHT;
								}
							}
							
							TD3D_UpdateAlbumSongObjectSurface();
							gpCurClientView->ShowWindow(NULL, NULL);
						}
						else
						{
							if(gnAlbumSongOffset < 0)
							{
								gnAlbumSongOffset = gnAlbumSongOffset + nOffsetLen;
								
								if(gnAlbumSongOffset >= 0)
								{
									gnAlbumSongOffset = 0;
								}
								
								TD3D_UpdateAlbumSongObjectSurface();
								gpCurClientView->ShowWindow(NULL,NULL);
							}
							else
							{
								break;
							}
						}
						if(nPauseDrawThread != 1)
						{
							break;
						}
						
						if(gnSelectAlbumSongCurData != 0)
						{
							gnAlbumSongOffset = gnAlbumSongOffset + nOffsetLen;
							
							if(gnAlbumSongOffset >= ALBUMSONGIMGHEIGHT)
							{
								TD3D_UpdateAlbumObjectIndex(MOVETOPTOBOTTOM);
								if(gnSelectAlbumSongCurData == 0)
								{
									gnAlbumSongOffset = 0;
								}
								else
								{
									gnAlbumSongOffset -= ALBUMSONGIMGHEIGHT;
								}
							}
							
							TD3D_UpdateAlbumSongObjectSurface();
							gpCurClientView->ShowWindow(NULL, NULL);
						}
						else
						{
							if(gnAlbumSongOffset < 0)
							{
								gnAlbumSongOffset = gnAlbumSongOffset + nOffsetLen;
								
								if(gnAlbumSongOffset >= 0)
								{
									gnAlbumSongOffset = 0;
								}
								
								TD3D_UpdateAlbumSongObjectSurface();
								gpCurClientView->ShowWindow(NULL,NULL);
							}
							else
							{
								break;
							}
						}
						
						if(i <= (int)lParam / 4)
						{
							for(j = 0; j < (((int)lParam - i + 1) > 5 ? 5 : (int)lParam - i + 1) ; j++)
							{
								if(nPauseDrawThread != 1)
									break;
								
								if(gnSelectAlbumSongCurData != 0)
								{
									gnAlbumSongOffset = gnAlbumSongOffset + nOffsetLen;
									
									if(gnAlbumSongOffset >= ALBUMSONGIMGHEIGHT)
									{
										TD3D_UpdateAlbumObjectIndex(MOVETOPTOBOTTOM);
										if(gnSelectAlbumSongCurData == 0)
										{
											gnAlbumSongOffset = 0;
											i = 0;
											break;
										}
										else
										{
											gnAlbumSongOffset -= ALBUMSONGIMGHEIGHT;
										}
									}
									
									TD3D_UpdateAlbumSongObjectSurface();
									gpCurClientView->ShowWindow(NULL, NULL);
								}
								else
								{
									if(gnAlbumSongOffset < 0)
									{
										gnAlbumSongOffset = gnAlbumSongOffset + nOffsetLen;
										
										if(gnAlbumSongOffset >= 0)
										{
											gnAlbumSongOffset = 0;
										}
										
										TD3D_UpdateAlbumSongObjectSurface();
										gpCurClientView->ShowWindow(NULL,NULL);
									}
									else
									{
										i = 0;
										break;
									}
								}
							}
						}
					}
					
					if(nPauseDrawThread == 1)
					{
						AlbumAlignmentSongPosition(MOVETOPTOBOTTOM, 1);
					}
				}
			}
		}
		else if((int)wParam == MOVEBOTTOMTOTOP)
		{
			if(gpCurClientView && gpCurClientView->dwModuleID == 0x30300000)
			{
				if(gnAlbumDownState == ALBUM_DOWN_STATE_PIC)
				{
					if((int)lParam > 25)
					{
						lParam = 25;
					}
					for(i = (int)lParam; i >= 0; i--)
					{
						if(nPauseDrawThread != 1)
							break;
						
						nOffsetLen = i * i / (int)lParam;
						
						if(i == (int)lParam)
						{
							nOffsetLen = nOffsetLen / 2;
						}
						
						if(nOffsetLen == 0)
						{
							break;
						}
						
						gnAlbumPicOffset = gnAlbumPicOffset - nOffsetLen;
						if(gnAlbumPicOffset <= -ALBUM_IMGW - ALBUM_OFFSET_Y)
						{
							TD3D_UpdateAlbumObjectIndex(MOVEBOTTOMTOTOP);
							gnAlbumPicOffset = gnAlbumPicOffset + ALBUM_IMGW + ALBUM_OFFSET_Y;
							gnAlbumEventState = ALBUMEVENTSTATE_BOTTOMTOTOP;
						}
						
						//yang add for album update
						gnKaraokOffset = gnKaraokOffset - nOffsetLen;
						if(gnKaraokOffset <= -ALBUM_UPDATE_OFFSET)
						{
							gnKaraokOffset += ALBUM_UPDATE_OFFSET;
								
							gnActorCurPageState = STATE_USEING;
							while(gnSendEventActorState != STATE_NOUSE)
							{
								usleep(100);
							}
							SetEvent(Recv3DPicDelayBegin1Event);
						}
						//..

						gpCurClientView->ShowWindow(NULL, NULL);
						
						if(nPauseDrawThread != 1)
							break;
						
						gnAlbumPicOffset = gnAlbumPicOffset - nOffsetLen;
						if(gnAlbumPicOffset <= -ALBUM_IMGW - ALBUM_OFFSET_Y)
						{
							TD3D_UpdateAlbumObjectIndex(MOVEBOTTOMTOTOP);
							gnAlbumPicOffset = gnAlbumPicOffset + ALBUM_IMGW + ALBUM_OFFSET_Y;
							gnAlbumEventState = ALBUMEVENTSTATE_BOTTOMTOTOP;
						}

						//yang add for album update
						gnKaraokOffset = gnKaraokOffset - nOffsetLen;
						if(gnKaraokOffset <= -ALBUM_UPDATE_OFFSET)
						{
							gnKaraokOffset += ALBUM_UPDATE_OFFSET;
								
							gnActorCurPageState = STATE_USEING;
							while(gnSendEventActorState != STATE_NOUSE)
							{
								usleep(100);
							}
							SetEvent(Recv3DPicDelayBegin1Event);
						}
						//..

						gpCurClientView->ShowWindow(NULL, NULL);
						
						if(i <= (int)lParam / 4)
						{
							for(j = 0; j < (((int)lParam - i + 1) > 5 ? 5 : (int)lParam - i + 1) ; j++)
							{
								if(nPauseDrawThread != 1)
									break;
								
								gnAlbumPicOffset = gnAlbumPicOffset - nOffsetLen;
								if(gnAlbumPicOffset <= -ALBUM_IMGW - ALBUM_OFFSET_Y)
								{
									TD3D_UpdateAlbumObjectIndex(MOVEBOTTOMTOTOP);
									gnAlbumPicOffset = gnAlbumPicOffset + ALBUM_IMGW + ALBUM_OFFSET_Y;
									gnAlbumEventState = ALBUMEVENTSTATE_BOTTOMTOTOP;
								}
								
								//yang add for album update
								gnKaraokOffset = gnKaraokOffset - nOffsetLen;
								if(gnKaraokOffset <= -ALBUM_UPDATE_OFFSET)
								{
									gnKaraokOffset += ALBUM_UPDATE_OFFSET;
									
									gnActorCurPageState = STATE_USEING;
									while(gnSendEventActorState != STATE_NOUSE)
									{
										usleep(100);
									}
									SetEvent(Recv3DPicDelayBegin1Event);
								}
								//..
								gpCurClientView->ShowWindow(NULL, NULL);
							}
						}
					}
					
					if(nPauseDrawThread == 1)
					{
						gpCurClientView->ShowWindow(NULL, NULL);
					}
				}
				else if(gnAlbumDownState == ALBUM_DOWN_STATE_SONG)
				{
					if((int)lParam > 45)
					{
						lParam = 45;
					}

					int nSongNum = 0;
					nSongNum = gnSelectAlbumSongCount % gpCurClientView->iCPP;

					for(i = (int)lParam; i >= 0; i--)
					{
						if(nPauseDrawThread != 1)
						{
							break;
						}
						
						nOffsetLen = i * i / (int)lParam;
						
						if(i == (int)lParam)
						{
							nOffsetLen = nOffsetLen / 2;
						}
						
						if(nOffsetLen == 0)
						{
							break;
						}
						
						if(gnSelectAlbumSongCurData == gnSelectAlbumSongCount - nSongNum - gpCurClientView->iCPP)
						{
							gnAlbumSongOffset = gnAlbumSongOffset - nOffsetLen;
							
							if(nSongNum == 0 && gnAlbumSongOffset < 0)//total 是 icpp 的整数倍
							{
								gnAlbumSongOffset = 0;
								break;
							}
							else
							{
								if(gnAlbumSongOffset <= -(nSongNum * ALBUMSONGIMGHEIGHT / gpCurClientView->iCPP))
								{
									gnAlbumSongOffset = -(nSongNum * ALBUMSONGIMGHEIGHT / gpCurClientView->iCPP);
									TD3D_UpdateAlbumSongObjectSurface();
									gpCurClientView->ShowWindow(NULL,NULL);
									break;
								}
							}
							
							TD3D_UpdateAlbumSongObjectSurface();
							
							gpCurClientView->ShowWindow(NULL,NULL);
						}
						else
						{
							gnAlbumSongOffset = gnAlbumSongOffset - nOffsetLen;
							
							if(gnAlbumSongOffset <= -ALBUMSONGIMGHEIGHT)
							{
								TD3D_UpdateAlbumObjectIndex(MOVEBOTTOMTOTOP);
								if(nSongNum == 0 && gnSelectAlbumSongCurData == gnSelectAlbumSongCount - gpCurClientView->iCPP)
								{
									gnAlbumSongOffset = 0;
								}
								else
								{
									gnAlbumSongOffset += ALBUMSONGIMGHEIGHT;
								}
							}
							
							TD3D_UpdateAlbumSongObjectSurface();
							
							gpCurClientView->ShowWindow(NULL,NULL);
						}
						
						if(nPauseDrawThread != 1)
						{
							break;
						}
						
						if(gnSelectAlbumSongCurData == gnSelectAlbumSongCount - nSongNum - gpCurClientView->iCPP)
						{
							gnAlbumSongOffset = gnAlbumSongOffset - nOffsetLen;
							
							if(nSongNum == 0 && gnAlbumSongOffset < 0)//total 是 icpp 的整数倍
							{
								gnAlbumSongOffset = 0;
								break;
							}
							else
							{
								if(gnAlbumSongOffset <= -(nSongNum * ALBUMSONGIMGHEIGHT / gpCurClientView->iCPP))
								{
									gnAlbumSongOffset = -(nSongNum * ALBUMSONGIMGHEIGHT / gpCurClientView->iCPP);
									TD3D_UpdateAlbumSongObjectSurface();
									gpCurClientView->ShowWindow(NULL,NULL);
									break;
								}
							}
							
							TD3D_UpdateAlbumSongObjectSurface();
							
							gpCurClientView->ShowWindow(NULL,NULL);
						}
						else
						{
							gnAlbumSongOffset = gnAlbumSongOffset - nOffsetLen;
							
							if(gnAlbumSongOffset <= -ALBUMSONGIMGHEIGHT)
							{
								TD3D_UpdateAlbumObjectIndex(MOVEBOTTOMTOTOP);
								if(nSongNum == 0 && gnSelectAlbumSongCurData == gnSelectAlbumSongCount - gpCurClientView->iCPP)
								{
									gnAlbumSongOffset = 0;
								}
								else
								{
									gnAlbumSongOffset += ALBUMSONGIMGHEIGHT;
								}
							}
							
							TD3D_UpdateAlbumSongObjectSurface();
							
							gpCurClientView->ShowWindow(NULL,NULL);
						}
						
						if(i <= (int)lParam / 4)
						{
							for(j = 0; j < (((int)lParam - i + 1) > 5 ? 5 : (int)lParam - i + 1) ; j++)
							{
								if(nPauseDrawThread != 1)
									break;
								
								if(gnSelectAlbumSongCurData == gnSelectAlbumSongCount - nSongNum - gpCurClientView->iCPP)
								{
									gnAlbumSongOffset = gnAlbumSongOffset - nOffsetLen;
									
									if(nSongNum == 0 && gnAlbumSongOffset < 0)//total 是 icpp 的整数倍
									{
										gnAlbumSongOffset = 0;
										i = 0;
										break;
									}
									else
									{
										if(gnAlbumSongOffset <= -(nSongNum * ALBUMSONGIMGHEIGHT / gpCurClientView->iCPP))
										{
											gnAlbumSongOffset = -(nSongNum * ALBUMSONGIMGHEIGHT / gpCurClientView->iCPP);
											TD3D_UpdateAlbumSongObjectSurface();
											gpCurClientView->ShowWindow(NULL,NULL);
											i = 0;
											break;
										}
									}
									
									TD3D_UpdateAlbumSongObjectSurface();
									
									gpCurClientView->ShowWindow(NULL,NULL);
								}
								else
								{
									gnAlbumSongOffset = gnAlbumSongOffset - nOffsetLen;
									
									if(gnAlbumSongOffset <= -ALBUMSONGIMGHEIGHT)
									{
										TD3D_UpdateAlbumObjectIndex(MOVEBOTTOMTOTOP);
										if(nSongNum == 0 && gnSelectAlbumSongCurData == gnSelectAlbumSongCount - gpCurClientView->iCPP)
										{
											gnAlbumSongOffset = 0;
										}
										else
										{
											gnAlbumSongOffset += ALBUMSONGIMGHEIGHT;
										}
									}
									
									TD3D_UpdateAlbumSongObjectSurface();
									
									gpCurClientView->ShowWindow(NULL,NULL);
								}
							}
						}
					}
					
					if(nPauseDrawThread == 1)
					{
						AlbumAlignmentSongPosition(MOVEBOTTOMTOTOP, 1);
					}
				}
			}
		}
		gnAlbumDestoryFlag = 0;
		break;
	case DRAWMSG_SHOWSELECT:
		nPauseDrawThread = gnSavePauseDrawThread;
		switch(gnSaveShowType)
		{
		case SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL:
			gnDrawType = 2;
			gpCurTopMenu->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL);

			if(nPauseDrawThread != 1)
				break;

			gpCurTopMenu->lCurData++;
			gpCurTopMenu->DrawWindow(1);
			gpCurTopMenu->lCurData--;
			if (gpCurTopMenu->lCurData < 0)
			{
				gpCurTopMenu->lCurData = (gpCurTopMenu->lDBTotal + gpCurTopMenu->iCPP - 1) / gpCurTopMenu->iCPP-1;
			}
			gpCurTopMenu->DrawWindow(3);
			break;
		case SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL:
			gnDrawType = 2;
			gpCurTopMenu->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL);
			if(nPauseDrawThread != 1)
				break;

			gpCurTopMenu->lCurData--;
			gpCurTopMenu->DrawWindow(-1);
			gpCurTopMenu->lCurData++;
			if (gpCurTopMenu->lCurData * gpCurTopMenu->iCPP >= gpCurTopMenu->lDBTotal)
			{
				gpCurTopMenu->lCurData = 0;
			}
			gpCurTopMenu->DrawWindow(3);
			break;
		case SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE:
		case SHOW_SHUTTERLEFTTORIGHTLASTPAGE | SHOWNEXTANDPROCPAGE:
			gnDrawType = 6;
			gpCurTopMenu->DrawWindow(0);
			gpCurTopMenu->ShowWindow(NULL, gnSaveShowType);
			if(nPauseDrawThread == 2 || nPauseDrawThread == 3)
				nPauseDrawThread = 1;
			break;
		default:
			break;
		};
		bActorImageIdleInvalid = FALSE;
		gnISSelectViewEffect = 0;
		break;
	case	DRAWMSG_SELECTROLLPAGE:
#if 0
		if(nPauseDrawThread != 1)
		{
			gnISSelectViewEffect = 0;
			break;
		}
		gnDrawType = 0;

		if(abs(lParam) > TURNPAGELONG/2)
			gnDrawType = 5;

		if(MODULE_TOPMENU_RECBOARD == gpCurTopMenu->dwModuleID)
		{
			if((int)wParam > 0)
			{
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC2,
					gpCurTopMenu->Rect.left + 115,
					gpCurTopMenu->Rect.top + 115,
					gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - 115 - 40,
					gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top - 115,
					ghScrMemDC3,
					gpCurTopMenu->Rect.left + 115,
					gpCurTopMenu->Rect.top + 115,
					gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - 115 - 40,
					gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top - 115,
					lParam);
				if(gnDrawType == 5)
					gpCurTopMenu->HandleLBtnUp(0x02001);
			}
			else if((int)wParam < 0)
			{
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC2,
					gpCurTopMenu->Rect.left + 115,
					gpCurTopMenu->Rect.top + 115,
					gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - 115 - 40,
					gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top - 115,
					ghScrMemDC4,
					gpCurTopMenu->Rect.left + 115,
					gpCurTopMenu->Rect.top + 115,
					gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - 115 -40,
					gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top - 115,
					lParam);
				if(gnDrawType == 5)
					gpCurTopMenu->HandleLBtnUp(0x01001);
			}
		}
		else
		{
			if((int)wParam > 0)
			{
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC2,
					gpCurTopMenu->Rect.left + SELECTVIEWMOVEPAGESTARTX,
					gpCurTopMenu->Rect.top + SELECTVIEWFONTSTARTY,
					SELECTVIEWMOVEPAGEENDX - SELECTVIEWMOVEPAGESTARTX,
					SELECTVIEWFONTENDY - SELECTVIEWFONTSTARTY,
					ghScrMemDC3,
					gpCurTopMenu->Rect.left + SELECTVIEWMOVEPAGESTARTX,
					gpCurTopMenu->Rect.top + SELECTVIEWFONTSTARTY,
					SELECTVIEWMOVEPAGEENDX - SELECTVIEWMOVEPAGESTARTX,
					SELECTVIEWFONTENDY - SELECTVIEWFONTSTARTY,
					lParam);
				if(gnDrawType == 5)
					gpCurTopMenu->HandleLBtnUp(0x02001);
			}
			else if((int)wParam < 0)
			{
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC2,
					gpCurTopMenu->Rect.left + SELECTVIEWMOVEPAGESTARTX,
					gpCurTopMenu->Rect.top + SELECTVIEWFONTSTARTY,
					SELECTVIEWMOVEPAGEENDX - SELECTVIEWMOVEPAGESTARTX,
					SELECTVIEWFONTENDY - SELECTVIEWFONTSTARTY,
					ghScrMemDC4,
					gpCurTopMenu->Rect.left + SELECTVIEWMOVEPAGESTARTX,
					gpCurTopMenu->Rect.top + SELECTVIEWFONTSTARTY,
					SELECTVIEWMOVEPAGEENDX - SELECTVIEWMOVEPAGESTARTX,
					SELECTVIEWFONTENDY - SELECTVIEWFONTSTARTY,
					lParam);
				if(gnDrawType == 5)
					gpCurTopMenu->HandleLBtnUp(0x01001);
			}

		}
		bActorImageIdleInvalid = FALSE;
		gnISSelectViewEffect = 0;
#endif
		break;
	case	DRAWMSG_SELECTTRUEPAGE:
		gnDrawType = 4;
		gnSavePauseDrawThread = 1;
		for(i = 0; i < (wParam > 4 ? 4 : wParam ) * 3; i++)
		{
			if(nPauseDrawThread != 1)
			{
				gnISSelectViewEffect = 0;
				break;
			}
			if(gpCurTopMenu->lDBTotal < gpCurTopMenu->iCPP)
			{
				gnISSelectViewEffect = 0;
				break;
			}
 			if(((int)lParam) < 0)
 				gpCurTopMenu->lCurData ++;
 			else
 				gpCurTopMenu->lCurData --;
/*
			TDBitBlt(ghScrMemDC,
				gpCurTopMenu->Rect.left,
				gpCurTopMenu->Rect.top,
				gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left ,
				gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
				ghScrMemDC2,
				gpCurTopMenu->Rect.left,
				gpCurTopMenu->Rect.top,
				NULL);
*/
			gpCurTopMenu->DrawWindow(0);
			if(((int)lParam) < 0)
			{
				gnSaveShowType = (SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE);
				gpCurTopMenu->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFT);
			}
			else
			{
				gnSaveShowType = (SHOW_SHUTTERLEFTTORIGHTLASTPAGE | SHOWNEXTANDPROCPAGE);
				gpCurTopMenu->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHT);
			}
		}
		break;
	case	DRAWMSG_SELECTSHOWPHOTO:
		gnDrawType = 6;
		if(nPauseDrawThread != 1)
		{
			gnISSelectViewEffect = 0;
			break;
		}
		if(gpCurTopMenu->lDBTotal < gpCurTopMenu->iCPP)  // KGy Don't do this if there is only one page.
		{
			gnISSelectViewEffect = 0;
			break;
		}
		if(((int)lParam) < 0)
			gpCurTopMenu->lCurData ++;
		else
			gpCurTopMenu->lCurData --;
		/*
		TDBitBlt(ghScrMemDC,
			gpCurTopMenu->Rect.left,
			gpCurTopMenu->Rect.top,
			gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left ,
			gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
			ghScrMemDC,
			gpCurTopMenu->Rect.left,
			gpCurTopMenu->Rect.top,
			NULL);
			*/
		gpCurTopMenu->DrawWindow(0);
		if(((int)lParam) < 0)
		{
			gnSaveShowType = (SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE);
			gpCurTopMenu->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE);
		}
		else
		{
			gnSaveShowType = (SHOW_SHUTTERLEFTTORIGHTLASTPAGE | SHOWNEXTANDPROCPAGE);
			gpCurTopMenu->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHTLASTPAGE | SHOWNEXTANDPROCPAGE);
		}
		if(nPauseDrawThread == 2 || nPauseDrawThread == 3)
			nPauseDrawThread = 1;

		bActorImageIdleInvalid = FALSE;
		gnISSelectViewEffect = 0;
		break;
	case	DRAWMSG_SELECTONEPAGE:
		gnDrawType = 2;
		if(nPauseDrawThread != 1)
		{
			gnISSelectViewEffect = 0;
			break;
		}
 		if(gpCurTopMenu->lDBTotal < gpCurTopMenu->iCPP)
		{
			gnISSelectViewEffect = 0;
			break;
		}
		if(((int)lParam) > 0)
		{
			gpCurTopMenu->lCurData --;
			if (gpCurTopMenu->lCurData < 0)
			{
				gpCurTopMenu->lCurData = (gpCurTopMenu->lDBTotal + gpCurTopMenu->iCPP - 1) / gpCurTopMenu->iCPP-1;
			}

			TDBitBlt(ghScrMemDC3,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top + 0,
				gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - nOffset,
				gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
				ghScrMemDC2,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				NULL);
			/*
			TDBitBlt(ghScrMemDC,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - nOffset,
				gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
				ghScrMemDC2,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				NULL);
				*/
			TDBitBlt(ghScrMemDC2,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - nOffset,
				gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
				ghScrMemDC4,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				NULL);

			gnSaveShowType = SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL;
			gpCurTopMenu->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL);

			if(nPauseDrawThread != 1)
			{
				gnISSelectViewEffect = 0;
				break;
			}

			gpCurTopMenu->lCurData--;
			gpCurTopMenu->DrawWindow(-1);

			gpCurTopMenu->lCurData++;
			if (gpCurTopMenu->lCurData * gpCurTopMenu->iCPP >= gpCurTopMenu->lDBTotal)
			{
				gpCurTopMenu->lCurData = 0;
			}
			gpCurTopMenu->DrawWindow(3);
		}
		else
		{
			gpCurTopMenu->lCurData ++;
			if (gpCurTopMenu->lCurData * gpCurTopMenu->iCPP >= gpCurTopMenu->lDBTotal)
			{
				gpCurTopMenu->lCurData = 0;
			}
/*
			TDBitBlt(ghScrMemDC,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - nOffset,
				gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
				ghScrMemDC2,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				NULL);
				*/
			TDBitBlt(ghScrMemDC4,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - nOffset,
				gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
				ghScrMemDC2,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				NULL);
			TDBitBlt(ghScrMemDC2,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left - nOffset,
				gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
				ghScrMemDC3,
				gpCurTopMenu->Rect.left + nOffset,
				gpCurTopMenu->Rect.top,
				NULL);
			gnSaveShowType = SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL;
			gpCurTopMenu->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL);

			if(nPauseDrawThread != 1)
			{
				gnISSelectViewEffect = 0;
				break;
			}

			gpCurTopMenu->lCurData++;
			gpCurTopMenu->DrawWindow(1);
			gpCurTopMenu->lCurData--;
			if (gpCurTopMenu->lCurData < 0)
			{
				gpCurTopMenu->lCurData = (gpCurTopMenu->lDBTotal + gpCurTopMenu->iCPP - 1) / gpCurTopMenu->iCPP-1;
			}
			gpCurTopMenu->DrawWindow(3);
			gnSaveShowType = 0;
		}
		bActorImageIdleInvalid = FALSE;
		gnISSelectViewEffect = 0;
		break;
	case DRAWMSG_SHOWPHOTO:
#if 0
		//printf("enter  case DRAWMSG_SHOWPHOTO  \n");
		if(gpCurClientView != &gClientView[CLIENTVIEW_SEARCHRESULT])
		{
			TD3D_ActorSelView_ChangeBackFromCoverFlowShow(ghDisplay, 
				gh3DObject,
				gpCurClientView->lCurData, 
				gpCurClientView->lDBTotal,
				30);
		}
		else
		{
			nPauseDrawThread = gnSavePauseDrawThread;
			switch(gnSaveShowType)
			{
			case SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL:
				gnDrawType = 2;
				gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL);
				
				if(nPauseDrawThread != 1)
					break;
				
				gpCurClientView->lCurData++;
				gpCurClientView->DrawWindow(1);
				gpCurClientView->lCurData--;
				if (gpCurClientView->lCurData < 0)
				{
					gpCurClientView->lCurData = (gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP-1;
				}
				gpCurClientView->DrawWindow(3);
				break;
			case SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL:
				gnDrawType = 2;
				gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL);
				
				if(nPauseDrawThread != 1)
					break;
				
				gpCurClientView->lCurData--;
				gpCurClientView->DrawWindow(-1);
				
				gpCurClientView->lCurData++;
				if (gpCurClientView->lCurData * gpCurClientView->iCPP >= gpCurClientView->lDBTotal)
				{
					gpCurClientView->lCurData = 0;
				}
				gpCurClientView->DrawWindow(3);
				break;
			case SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE:
			case SHOW_SHUTTERLEFTTORIGHTLASTPAGE | SHOWNEXTANDPROCPAGE:
				gnDrawType = 6;
				//if(!gnShouldShow3D)
				if(0)
				{
					gpCurClientView->DrawWindow(0);
					gpCurClientView->ShowWindow(NULL, gnSaveShowType);
				}
				else
				{
					TDDebug("SHOW_SHUTTERLEFTTORIGHTLASTPAGE|SHOWNEXTANDPROCPAGE");
					{
						//printf("back fro flag 1\n");
					TD3D_ActorSelView_ChangeBackFromCoverFlowShow(ghDisplay, 
						gh3DObject,
						gpCurClientView->lCurData, 
						gpCurClientView->lDBTotal,
						30);
					}
				}
				if(nPauseDrawThread == 2 || nPauseDrawThread == 3)
					nPauseDrawThread = 1;
				break;
			default:
				break;
			};
		}
		bActorImageIdleInvalid = FALSE;
#endif
		break;
	case DRAWMSG_ACTORTRUEPAGE:
#if 0//yang
		if(gpCurClientView == &gClientView[CLIENTVIEW_ACTORSEL])
			nOffset = 210;
		gnDrawType = 4;
		gnSavePauseDrawThread = 1;
		
		//for(i = 0; i < (wParam > 4 ? 4 : wParam ) * 3; i++)
		for(i = 0; i < (wParam > 8 ? 8 : wParam ) * 3; i++)
		{
//			printf("this a for gfWorldAngleY =%f  lParam=%d \n",gfWorldAngleY ,(int)lParam);
			if(nPauseDrawThread != 1)
				break;
			if(gpCurClientView->lDBTotal < gpCurClientView->iCPP)
				break;
			if(gpCurClientView == &gClientView[CLIENTVIEW_SEARCHRESULT])
			{
				if(((int)lParam) < 0)
					gpCurClientView->lCurData ++;
				else
					gpCurClientView->lCurData --;
				
				TDBitBlt(ghScrMemDC,
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrClientViewMemDC,
					nOffset,
					0,
					NULL);
				gpCurClientView->DrawWindow(0);
				if(((int)lParam) < 0)
				{
					gnSaveShowType = (SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE);
					gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFT);
				}
				else
				{
					gnSaveShowType = (SHOW_SHUTTERLEFTTORIGHTLASTPAGE | SHOWNEXTANDPROCPAGE);
					gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHT);
				}
			}
			else
			{
				if(((int)lParam) < 0)
				{
					gnSaveShowType = (SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE);
						
					if(abs(gfWorldAngleY==0.0f)||gfWorldAngleY==-13.0f)
					{
						TD3D_ActorSelView_ChangeToCoverFlowShow(
							ghDisplay, 
							gh3DObject,
							gpCurClientView->lCurData, 
							gpCurClientView->lDBTotal,
							20,//30,
							-1);
						//printf("gfWorldAngleY1112 =%f \n",gfWorldAngleY);
					}
					else /*if(gfWorldAngleY==13.0f)*/
					{
						//TD3D_ActorSelView_RollOnePage(
						TD3D_ActorSelView_RollOnePage_ForCoverFlow(
							ghDisplay, 
							gh3DObject,
							gpCurClientView->lCurData, 
							gpCurClientView->lDBTotal,
							5,//10,//25,//30,
							(int)lParam,
							-1);
					}
				}
				else
				{
					//printf("will call TD3D_ActorSelView_RollOnePage flag 4 \n ");
					if(abs(gfWorldAngleY==0.0f)||gfWorldAngleY==13.0f)
					{
						TD3D_ActorSelView_ChangeToCoverFlowShow(
							ghDisplay, 
							gh3DObject,
							gpCurClientView->lCurData, 
							gpCurClientView->lDBTotal,
							20,//30,
							1);
//						printf("gfWorldAngleY1111 =%f \n",gfWorldAngleY);
					}
					else /*if(gfWorldAngleY == -13.0)*/
					{
						//printf("will call TD3D_ActorSelView_RollOnePage flag 1 \n ");
						//TD3D_ActorSelView_RollOnePage(
						TD3D_ActorSelView_RollOnePage_ForCoverFlow(
							ghDisplay, 
							gh3DObject,
							gpCurClientView->lCurData, 
							gpCurClientView->lDBTotal,
							5,//10,//25,//30,
							(int)lParam,
							1);
					}
				}
			}
		}
#endif
		break;
	case DRAWMSG_ACTORSHOWPHOTO:
#if 0//yang
		if(gpCurClientView == &gClientView[CLIENTVIEW_ACTORSEL])
		{
			nOffset = 210;
		}
		gnDrawType = 6;
		if(nPauseDrawThread != 1)
			break;
		if(gpCurClientView->lDBTotal < gpCurClientView->iCPP)  // KGy Don't do this if there is only one page.
			break;
		//if(!gnShouldShow3D)
		if(gpCurClientView == &gClientView[CLIENTVIEW_SEARCHRESULT])
		{
			break;
			if(((int)lParam) < 0)
				gpCurClientView->lCurData ++;
			else
				gpCurClientView->lCurData --;
			TDBitBlt(ghScrMemDC,
				gpCurClientView->Rect.left + nOffset,
				gpCurClientView->Rect.top,
				gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
				gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
				ghScrClientViewMemDC,
				nOffset,
				0,
				NULL);
			gpCurClientView->DrawWindow(0);
			if(((int)lParam) < 0)
			{
				gnSaveShowType = (SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE);
				gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFTLASTPAGE | SHOWNEXTANDPROCPAGE);
			}
			else
			{
				gnSaveShowType = (SHOW_SHUTTERLEFTTORIGHTLASTPAGE | SHOWNEXTANDPROCPAGE);
				gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHTLASTPAGE | SHOWNEXTANDPROCPAGE);
			}
		}
		else
		{
			//printf("back fro flag 2\n");//TD3D_ActorSelView_ChangeBackFromCoverFlowShow_level
			TD3D_ActorSelView_ChangeBackFromCoverFlowShow(ghDisplay, 
					gh3DObject,
					gpCurClientView->lCurData, 
					gpCurClientView->lDBTotal,
					30);
		}
		if(nPauseDrawThread == 2 || nPauseDrawThread == 3)
			nPauseDrawThread = 1;

		bActorImageIdleInvalid = FALSE;
#endif
		break;
	case DRAWMSG_ACTORONEPAGE:
#if 0//yang
		if(gpCurClientView == &gClientView[CLIENTVIEW_ACTORSEL])
			nOffset = 210;
		gnDrawType = 2;
		if(nPauseDrawThread != 1)
			break;
		if(gpCurClientView->lDBTotal < gpCurClientView->iCPP)
			break;
		if(gpCurClientView == &gClientView[CLIENTVIEW_SEARCHRESULT])
		{
			if(((int)lParam) > 0)
			{
				gpCurClientView->lCurData --;
				if (gpCurClientView->lCurData < 0)
				{
					gpCurClientView->lCurData = (gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP-1;
				}
				
				TDBitBlt(ghScrMemDC3,
					nOffset,
					0,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrMemDC,
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					NULL);
				TDBitBlt(ghScrClientViewMemDC,
					nOffset,
					0,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrMemDC4,
					nOffset,
					0,
					NULL);
				
				gnSaveShowType = SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL;
				gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL);
				
				if(nPauseDrawThread != 1)
					break;
				
				gpCurClientView->lCurData--;
				gpCurClientView->DrawWindow(-1);
				
				gpCurClientView->lCurData++;
				if (gpCurClientView->lCurData * gpCurClientView->iCPP >= gpCurClientView->lDBTotal)
				{
					gpCurClientView->lCurData = 0;
				}
				gpCurClientView->DrawWindow(3);
			}
			else
			{
				gpCurClientView->lCurData ++;
				if (gpCurClientView->lCurData * gpCurClientView->iCPP >= gpCurClientView->lDBTotal)
				{
					gpCurClientView->lCurData = 0;
				}
				
				TDBitBlt(ghScrMemDC4,
					nOffset,
					0,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrMemDC,
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					NULL);
				TDBitBlt(ghScrClientViewMemDC,
					nOffset,
					0,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrMemDC3,
					nOffset,
					0,
					NULL);
				gnSaveShowType = SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL;
				gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL);
				
				if(nPauseDrawThread != 1)
					break;
				
				gpCurClientView->lCurData++;
				gpCurClientView->DrawWindow(1);
				gpCurClientView->lCurData--;
				if (gpCurClientView->lCurData < 0)
				{
					gpCurClientView->lCurData = (gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP-1;
				}
				gpCurClientView->DrawWindow(3);
				gnSaveShowType = 0;
			}
		}
		else
		{
			if(gfWorldAngleY != 0.0)
			{
				nDrawPreNextFlag = 1;
			}
			if(((int)lParam) > 0)
			{
				TDDebug("DRAWMSG_ACTORONEPAGE l ro r 1");
//	printf("left  to right before  change xuanze gfWorldAngleY =%f *********############\n", gfWorldAngleY );
				for(i = 0; i < (int)wParam; i++)
				{
					if(nPauseDrawThread != 1)
						break;
				/*	if(abs(gfWorldAngleY==0.0f)||gfWorldAngleY==13.0f)
					{
						TD3D_ActorSelView_ChangeToCoverFlowShow(
							ghDisplay, 
							gh3DObject,
							gpCurClientView->lCurData, 
							gpCurClientView->lDBTotal,
							20,//30,
							1);
//						printf("gfWorldAngleY1111 =%f \n",gfWorldAngleY);
					}
					else*/
					{
						//printf("will call TD3D_ActorSelView_RollOnePage flag 1 \n ");
						TD3D_ActorSelView_RollOnePage(
							ghDisplay, 
							gh3DObject,
							gpCurClientView->lCurData, 
							gpCurClientView->lDBTotal,
							2,//10,//25,//30,
							(int)lParam,
							1);
					}
				}
				TDDebug("DRAWMSG_ACTORONEPAGE l ro r 2");
				TD3D_ActorSelView_DrawNextActor(1, 2);
				TDDebug("DRAWMSG_ACTORONEPAGE l ro r 3");
			}
			else
			{//r to l 
//		printf("before r to l  change xuanze gfWorldAngleY =%f *********############\n", gfWorldAngleY );
				TDDebug("DRAWMSG_ACTORONEPAGE r ro l 1");
				for(i = 0; i < (int)wParam; i++)
				{
					if(nPauseDrawThread != 1)
						break;
					/*if(abs(gfWorldAngleY==0.0f)||gfWorldAngleY==-13.0f)//if(gfWorldAngleY <= 0.000002)
					{
						TD3D_ActorSelView_ChangeToCoverFlowShow(
							ghDisplay, 
							gh3DObject,
							gpCurClientView->lCurData, 
							gpCurClientView->lDBTotal,
							20,//30,
							-1);
//						printf("gfWorldAngleY1112 =%f \n",gfWorldAngleY);
					}
					else*/
					{
						TD3D_ActorSelView_RollOnePage(
							ghDisplay, 
							gh3DObject,
							gpCurClientView->lCurData, 
							gpCurClientView->lDBTotal,
							2,//10,//25,//30,
							(int)lParam,
							-1);
					}
				}
				TDDebug("DRAWMSG_ACTORONEPAGE r ro l 2");
				TD3D_ActorSelView_DrawNextActor(-1, 2);
				TDDebug("DRAWMSG_ACTORONEPAGE r ro l 3");
			}
			if(nDrawPreNextFlag)
			{
				TDDebug("DRAWMSG_ACTORONEPAGE r ro l 4");
				TD3D_ActorSelView_ShowEnd(1);
				TDDebug("DRAWMSG_ACTORONEPAGE r ro l 5");
			}
		}
		gnSaveShowType = 0;
		bActorImageIdleInvalid = FALSE;
#else  //huanguo  20110414
		{
			gnDrawType = 2;
		//	if(nPauseDrawThread != 1)
		//		break;
			if(gpCurClientView->lDBTotal < gpCurClientView->iCPP)
				break;
		//	nOffset = 210;
			if(((int)lParam) > 0)
			{
				gpCurClientView->lCurData --;
				if (gpCurClientView->lCurData < 0)
				{
					gpCurClientView->lCurData = (gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP-1;
				}

				TDBitBlt(ghScrMemDC3, 
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrWithOutBGMemDC, 
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					NULL);
				TDBitBlt(ghScrClientViewMemDC, 
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrMemDC4, 
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					NULL);

				gnSaveShowType = SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL;
				gpCurClientView->ShowWindow(NULL, SHOW_SHUTTERLEFTTORIGHTLASTPAGENOMAL);
			
				if(nPauseDrawThread != 1)
					break;
					
				gpCurClientView->lCurData--;
				gpCurClientView->DrawWindow(-1);

				gpCurClientView->lCurData++;
				if (gpCurClientView->lCurData * gpCurClientView->iCPP >= gpCurClientView->lDBTotal)
				{
					gpCurClientView->lCurData = 0;
				}
				gpCurClientView->DrawWindow(3);
			}
			else
			{
				gpCurClientView->lCurData ++;
				if (gpCurClientView->lCurData * gpCurClientView->iCPP >= gpCurClientView->lDBTotal)
				{
					gpCurClientView->lCurData = 0;
				}
			
				TDBitBlt(ghScrMemDC4, 
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrWithOutBGMemDC, 
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					NULL);
				TDBitBlt(ghScrWithOutBGMemDC, 
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - nOffset,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrMemDC3, 
					gpCurClientView->Rect.left + nOffset,
					gpCurClientView->Rect.top,
					NULL);
				gnSaveShowType = SHOW_SHUTTERRIGHTTOLEFTLASTPAGENOMAL;
				gpCurClientView->ShowWindow(NULL, gnSaveShowType);
					
				if(nPauseDrawThread != 1)
					break;
				gpCurClientView->lCurData++;
				gpCurClientView->DrawWindow(1);
				gpCurClientView->lCurData--;
				if (gpCurClientView->lCurData < 0)
				{
					gpCurClientView->lCurData = (gpCurClientView->lDBTotal + gpCurClientView->iCPP - 1) / gpCurClientView->iCPP-1;
				}	
				gpCurClientView->DrawWindow(3);
				gnSaveShowType = 0;
			}	
		}
#endif
		break;
	case DRAWMSG_ACTORROLLPAGE:
#if 0//yang
		if(nPauseDrawThread != 1)
			break;
		gnDrawType = 0;
		if(abs(lParam) > TURNPAGELONG)
			gnDrawType = 5;
		if(gpCurClientView == &gClientView[CLIENTVIEW_ACTORSEL])
			nOffset = 210;
		if((int)wParam > 0)
		{
			gnSaveShowType = SHOW_ROLLPAGE;
			switch(gpCurClientView->dwModuleID & 0xFFF00000)
			{		
			case 0x11200000:
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC,
					gpCurClientView->Rect.left + ACTORSONGVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - ACTORSONGVIEWSTARTX - RIGHTSPELLOFFSET,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					ghScrMemDC3,
					ACTORSONGVIEWSTARTX,
					CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - ACTORSONGVIEWSTARTX - RIGHTSPELLOFFSET,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					lParam
					);
				break;
			//case 0x10900000:
			//case 0x12100000:
/*			case 0x12200000:
			//case 0x19400000:
			//case 0x20100000:
				TH_TRAGBUTTONUP(ghScrMemDC1, ghScrMemDC,
					202,
					gpCurClientView->Rect.top,
					280,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrMemDC3,
					202,
					gpCurClientView->Rect.top,
					280,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					lParam
					);
				break;*/
			case 0x40200000:
			case 0x10900000:
			case 0x31400000://drink taocan
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC,
					gpCurClientView->Rect.left + CLIENTVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					ghScrMemDC3,
					CLIENTVIEWSTARTX,
					CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					lParam);
				break;
			case 0x30000000: //drink
			case 0x31200000: //drink
			case 0x31300000: //drink
			case 0x31500000: //taocan drink
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC,
					gpCurClientView->Rect.left + CLIENTVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY + 52,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET - 52,
					ghScrMemDC3,
					CLIENTVIEWSTARTX,
					CLIENTVIEWSTARTY + 52,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET - 52,
					lParam);
				break;
			case 0x11000000:
				{
					//printf("will call TD3D_ActorSelView_TRAGBUTTONUP flag 1!\n");
				TD3D_ActorSelView_TRAGBUTTONUP(ghDisplay,
						gh3DObject,
						gpCurClientView->lCurData, 
						gpCurClientView->lDBTotal,
						10,
						lParam);
				break;
				}
			default:
				//printf("TH_TRAGBUTTONUP 11111\n");
				TH_TRAGBUTTONUPFORCLIENTVIEW(ghScrMemDC1,
					ghScrMemDC,
					gpCurClientView->Rect.left + CLIENTVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX - RIGHTSPELLOFFSET,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET , 
					ghScrMemDC3,
					CLIENTVIEWSTARTX,
					gpCurClientView->Rect.top +CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX - RIGHTSPELLOFFSET,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					lParam
					);
				//printf("TH_TRAGBUTTONUP 3\n");
				break;
			}
			//printf("gnDrawType = %d\n",gnDrawType);
			if(gnDrawType == 5)
			{
				if(gpCurClientView->dwModuleID != 0x11000000)
				{
						gpCurClientView->DrawWindow(0);
						gpCurClientView->ShowWindow(NULL,SHOWNEXTANDPROCPAGE);
				}
				//		gpCurClientView->HandleLBtnUp(0x02001);
			}
		}
		else if((int)wParam < 0)
		{
			gnSaveShowType = SHOW_ROLLPAGE;
			switch(gpCurClientView->dwModuleID & 0xFFF00000)
			{
			case 0x11200000:
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC,
					gpCurClientView->Rect.left + ACTORSONGVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - ACTORSONGVIEWSTARTX - RIGHTSPELLOFFSET,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					ghScrMemDC4,
					ACTORSONGVIEWSTARTX,
					CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - ACTORSONGVIEWSTARTX - RIGHTSPELLOFFSET,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					lParam
					);
				break;
			//case 0x10900000:
			//case 0x12100000:
/*			case 0x12200000:
			//case 0x19400000:
			//case 0x20100000:
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC,
					202,
					gpCurClientView->Rect.top,
					280,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					ghScrMemDC4,
					202,
					gpCurClientView->Rect.top,
					280,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top,
					lParam
					);
				break;*/
			case 0x40200000:
			case 0x10900000:
			case 0x31400000://drink taocan
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC,
					gpCurClientView->Rect.left + CLIENTVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					ghScrMemDC4,
					CLIENTVIEWSTARTX,
					CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					lParam
					);
				break;
			case 0x30000000: //drink
			case 0x31200000:
			case 0x31300000:
			case 0x31500000://taocan drink
				TH_TRAGBUTTONUP(ghScrMemDC1,
					ghScrMemDC,
					gpCurClientView->Rect.left + CLIENTVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY + 52,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET - 52,
					ghScrMemDC4,
					CLIENTVIEWSTARTX,
					CLIENTVIEWSTARTY + 52,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET - 52,
					lParam
					);
				break;
			case 0x11000000:
				//if(gnShouldShow3D)
				{
			//	printf("will call TD3D_ActorSelView_TRAGBUTTONUP flag 2!\n");
				TD3D_ActorSelView_TRAGBUTTONUP(ghDisplay,
						gh3DObject,
						gpCurClientView->lCurData, 
						gpCurClientView->lDBTotal,
						10,
						lParam);
				break;
				}
			default:
		//		printf("TH_TRAGBUTTONUP 22222\n");
				TH_TRAGBUTTONUPFORCLIENTVIEW(ghScrMemDC1,
					ghScrMemDC,
					gpCurClientView->Rect.left + CLIENTVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX - RIGHTSPELLOFFSET,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					ghScrMemDC4,
					CLIENTVIEWSTARTX,
					gpCurClientView->Rect.top + CLIENTVIEWSTARTY,
					gpCurClientView->Rect.right - gpCurClientView->Rect.left - CLIENTVIEWSTARTX - RIGHTSPELLOFFSET,
					gpCurClientView->Rect.bottom - gpCurClientView->Rect.top - CLIENTVIEWSTARTY - BOTTOMBUTTONOFFSET,
					lParam
					);
				break;
			}
			if(gnDrawType == 5)
			{
				if(gpCurClientView->dwModuleID != 0x11000000)
			 	{
						gpCurClientView->DrawWindow(0);
						gpCurClientView->ShowWindow(NULL,SHOWNEXTANDPROCPAGE);
				}
					//	gpCurClientView->HandleLBtnUp(0x01001);
			}
		}
		bActorImageIdleInvalid = FALSE;
#endif
		break;
	}
	gnAlbumDestoryFlag = 0;
	return 0;
}
#endif

void *DrawMessageHThread(void *pNULL)
{
#if 1
#ifdef STANDARDVER
	TDBOOL bHasMessage = TDFALSE;
	TDINT nSleepMTime = 50;

	while(DrawMsg.bIsRunning)
	{
		if(WaitForSingleObject(DrawMsg.hMHEvent, nSleepMTime) == WAIT_OBJECT_0)
		{
			struct DrawMessage *pCur = pMessageHead;

			while (DrawMsg.iHeadStack != DrawMsg.iTailStack)
			{
				if(DrawMsg.Message[DrawMsg.iHeadStack].message == VIEW_SHOWTURNPAGE)
				{
					gnTurnPageNum = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
					gnShowPhotoFrame = 0;
				}
				else if(DrawMsg.Message[DrawMsg.iHeadStack].message == VIEW_SHOWTURNACTORPAGE)
				{
					gnTurnPageNum = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
					gnShowPhotoFrame = 0;
					TDShowNextActor(DrawMsg.Message[DrawMsg.iHeadStack].lParam);
				}
				else if(DrawMsg.Message[DrawMsg.iHeadStack].message == VIEW_SHOWTURNPHOTO)
				{
					gnTurnPageNum = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
					TDShowNextActor(DrawMsg.Message[DrawMsg.iHeadStack].lParam);
				}

				pCur = pMessageHead;
				while(pCur)
				{
					if(pCur->messagewindow.hWnd == DrawMsg.Message[DrawMsg.iHeadStack].hWnd)
					{
						switch(DrawMsg.Message[DrawMsg.iHeadStack].message)
						{
						case VIEW_SHOWBUTTONDOWN:
							if(pCur->message == VIEW_SHOWAPPENDMEDIA)
							{
								DrawMsg.Message[DrawMsg.iHeadStack].message = VIEW_SHOWAPPENDDOWN;
								continue;
							}
							else if(pCur->message == VIEW_SHOWTURNPHOTO)
							{
								pCur->message =	VIEW_SHOWSTOPPHOTO;
								pCur->wParam = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
								pCur->lParam = DrawMsg.Message[DrawMsg.iHeadStack].lParam;
							}
							else
							{
								pCur->message =	VIEW_SHOWSTOP;
								pCur->wParam = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
								pCur->lParam = DrawMsg.Message[DrawMsg.iHeadStack].lParam;
							}
							break;
						case VIEW_SHOWBUTTONMOVE:
							if(pCur->message == VIEW_SHOWBUTTONMOVE)
							{
								pCur->wParam = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
								pCur->lParam = DrawMsg.Message[DrawMsg.iHeadStack].lParam;
							}
							break;
						case VIEW_SHOWBUTTONUP:
							if(pCur->message == VIEW_SHOWAPPENDMEDIA)
							{
								if(pCur->next == NULL)
								{
									struct DrawMessage *pNewMessage = (struct DrawMessage *)malloc(sizeof(struct DrawMessage));

									pNewMessage->messagewindow.hWnd = DrawMsg.Message[DrawMsg.iHeadStack].hWnd;
									pNewMessage->message =	DrawMsg.Message[DrawMsg.iHeadStack].message;
									pNewMessage->wParam = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
									pNewMessage->lParam = DrawMsg.Message[DrawMsg.iHeadStack].lParam;
									pNewMessage->next = NULL;

									pCur->next = pNewMessage;
									break;
								}
								pCur = pCur->next;
								continue;
							}
							else
							{
								pCur->message = VIEW_SHOWBUTTONUP;
								pCur->wParam = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
								pCur->lParam = DrawMsg.Message[DrawMsg.iHeadStack].lParam;
							}
							break;
						case VIEW_SHOWAPPENDDOWN:
							if(pCur->next == NULL)
							{
								struct DrawMessage *pNewMessage = (struct DrawMessage *)malloc(sizeof(struct DrawMessage));

								pNewMessage->messagewindow.hWnd = DrawMsg.Message[DrawMsg.iHeadStack].hWnd;
								pNewMessage->message =	DrawMsg.Message[DrawMsg.iHeadStack].message;
								pNewMessage->wParam = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
								pNewMessage->lParam = DrawMsg.Message[DrawMsg.iHeadStack].lParam;
								pNewMessage->next = NULL;

								pCur->next = pNewMessage;
								break;
							}
							pCur = pCur->next;
							continue;
						default:
							break;
						}
						break;
					}
					else
					{
						if(pCur->message == VIEW_SHOWTURNPHOTO)
						{
							pCur->message = VIEW_SHOWPHOTOEND;
						}
						else if(pCur->message == VIEW_SHOWTURNACTORPAGE)
						{
							pCur->message = VIEW_SHOWPHOTOEND;
							gnTurnPageNum = 0;
						}
						else if(pCur->message != VIEW_SHOWAPPENDMEDIA)
							pCur->messagewindow.hWnd = NULL;
					}

					if(pCur->next == NULL)
					{
						struct DrawMessage *pNewMessage = (struct DrawMessage *)malloc(sizeof(struct DrawMessage));

						pNewMessage->messagewindow.hWnd = DrawMsg.Message[DrawMsg.iHeadStack].hWnd;
						pNewMessage->message =	DrawMsg.Message[DrawMsg.iHeadStack].message;
						pNewMessage->wParam = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
						pNewMessage->lParam = DrawMsg.Message[DrawMsg.iHeadStack].lParam;
						pNewMessage->next = NULL;

						pCur->next = pNewMessage;
						break;
					}
					pCur = pCur->next;
				}

				if(pCur == NULL)
				{
					pMessageHead = (struct DrawMessage *)malloc(sizeof(struct DrawMessage));

					pMessageHead->messagewindow.hWnd = DrawMsg.Message[DrawMsg.iHeadStack].hWnd;
					pMessageHead->message =	DrawMsg.Message[DrawMsg.iHeadStack].message;
					pMessageHead->wParam = DrawMsg.Message[DrawMsg.iHeadStack].wParam;
					pMessageHead->lParam = DrawMsg.Message[DrawMsg.iHeadStack].lParam;
					pMessageHead->next = NULL;
				}

				DrawMsg.Message[DrawMsg.iHeadStack].hWnd = NULL;
				DrawMsg.Message[DrawMsg.iHeadStack].message = 0;
				DrawMsg.Message[DrawMsg.iHeadStack].wParam = 0;
				DrawMsg.Message[DrawMsg.iHeadStack].lParam = 0;
				DrawMsg.iHeadStack = (DrawMsg.iHeadStack + 1) % 512;
			}
		}

		if(gpMainFrame && gpMainFrame ->bIsDraw != TDFALSE)
		{
			struct DrawMessage *pCur = pMessageHead;
			struct DrawMessage *pPrev = pMessageHead;

			bHasMessage = TDFALSE;
			pthread_mutex_lock(&gsViewIdleMutex);
			while(pCur)
			{
				bHasMessage = TDTRUE;

				if(pCur->messagewindow.pEffectView != NULL)
				{
					if(pCur->messagewindow.pEffectView->WindowProc(pCur->messagewindow.hWnd,
							pCur->message,
							pCur->wParam,
							pCur->lParam) == 0)
					{
						pCur->messagewindow.pEffectView = NULL;
					}
				}
				else
				{
					if(pMessageHead == pCur)
					{
						pMessageHead = pMessageHead->next;
						free(pCur);
						pPrev = pMessageHead;
						pCur = pMessageHead;
					}
					else
					{
						pPrev->next = pCur->next;
						free(pCur);
						pCur = pPrev->next;
					}
					continue;
				}

				pPrev = pCur;
				pCur = pCur->next;
			}

			//pthread_mutex_lock(&gsViewIdleMutex);
			if(!bHasMessage)
				nSleepMTime = 50;
			else
				nSleepMTime = 0;
			if(gnDouble != 3 && nFireAlarm != 1)
			{
				gpMainFrame->Idle();
			}
			else
			{
				if(nFireAlarm != 1 && gnDouble == 3 && !gbUsePic)
				{
					if(gpCurTopMenu == &gTopMenu[19])
					{
						if(gpCurRightCtrlPanel && gpCurRightCtrlPanel->Idle && !gbPreviewIsMove)
						{
							gpCurRightCtrlPanel->Idle();
						}
					}
					else//add by ghy 不管何时都计算当前的进度条
					{
						TDKtvGetVideoLengthOrCurLength(&nContentLength, &nCurLength);
					}
				}
			}
			ShowVGAPic();
			ShowADImage();
			pthread_mutex_unlock(&gsViewIdleMutex);
		}
		//usleep(10 * 1000);
	}
#else
	TDLONG		lPreTime = 0;
	TDLONG		lCurTime = 0;
	UINT 		message;
	WPARAM 		wParam;
	LPARAM 		lParam;
	MSG		Msg;
	nPauseDrawThread = 1;

	setpriority(PRIO_PROCESS,0,-20);

	while (DrawMsg.bIsRunning)
	{
		TH_GetDrawLock();
		nPauseDrawThread = 1;
		message = -1;
		wParam = 0;
		lParam = 0;

		TDDebug("before lock gDrawDCMutex");
		pthread_mutex_lock(&gDrawDCMutex);
		TDDebug("lock gDrawDCMutex success");
		//PauseADBarThreads();
		while(TH_GetDrawMessage(&Msg, NULL))
		{
			message = Msg.message;
			wParam = Msg.wParam;
			lParam = Msg.lParam;
			TDDebug("before WndDrawProc");
			WndDrawProc(NULL,
				Msg.message,
				Msg.wParam,
				Msg.lParam);
			TDDebug("after WndDrawProc");
		}
		if(message == DRAWMSG_ACTORTRUEPAGE)
		{
			TDDebug("Before DrawMSG_ACTORSHOWPHOTO");
			WndDrawProc(NULL,
				DRAWMSG_ACTORSHOWPHOTO,
				wParam,
				lParam);
			TDDebug("after DrawMSG_ACTORSHOWPHOTO");
		}
		/*else if(message == DRAWMSG_ACTORONEPAGE)
		{
			WndDrawProc(NULL,
				DRAWMSG_ACTORSHOWPHOTO,
				wParam,
				lParam);
		}*/
		else if(message == DRAWMSG_SELECTTRUEPAGE)
		{
			TDDebug("Before DrawMSG_SELECTTRUEPAGE");
			WndDrawProc(NULL,
				DRAWMSG_SELECTSHOWPHOTO,
				wParam,
				lParam);
			TDDebug("after DrawMSG_SELECTTRUEPAGE");
		}
		//ResumeADBarThreads();
		pthread_mutex_unlock(&gDrawDCMutex);
		TDDebug("unlock gDrawDCMutex");
	}
#endif

#endif
	return 0;
}


static	TDDWORD TimePrompt(TDINT iLeftTime, TDINT iTPState)
{
	char	szOSDTxt[256];
	sprintf(szOSDTxt,"LeftTime = %d,iTPState = %d\n",iLeftTime, iTPState);
	TDDebug(szOSDTxt);
	switch (iTPState)
	{
	case 0:
		glTPState = 0x8000;

		if(gpCurTopMenu)
		{
			gpCurTopMenu->Destroy();
			gpCurTopMenu = NULL;
		/*	LeftCtrlPanelLReset();
			gLeftCtrlPanel.DrawWindow(0);
			gLeftCtrlPanel.ShowWindow(NULL, 0);*/
		}
		ResetCurLButtonDownView();
		//gnEffectState = EFFECT_FLAT_TO_IN;
		//if (gpCurClientView)
		//{
		//	gpCurClientView->Destroy();
		//	gpCurClientView = NULL;
		//}
		gnEffectState = EFFECT_OUT_TO_FLAT;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		TDDebug("enter 1,2,3,4\n");
		if(TH_HasDrawMessage(NULL))
		{
			break ;
		}
		if(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
		{
			break;
		}
		pthread_mutex_unlock(&gDrawDCMutex);

		if(iTPState == 3 )
			glTPState |= 0x8000;
		
		sprintf(szOSDTxt,"glTPState = 0x%x\n",glTPState);
		TDDebug(szOSDTxt);
		
		if (gpCurTopMenu && (gpCurTopMenu->dwModuleID == MODULE_TOPMENU_AWOKE))
		{
			STBSendInfo(AWOKE1,iLeftTime);
			gpCurTopMenu->i64DgtInfo = iLeftTime;
			gpCurTopMenu->DrawWindow(0);
			gpCurTopMenu->ShowWindow(0,SHOW_TOPMENUTOTOPMENU);
		/*	TDBitBlt(ghDC, 370 + gpCurTopMenu->Rect.left,
					gpCurTopMenu->Rect.top + 40,
					90,
					70,
					ghScrClientViewMemDC,
					370,
					TOPMENUOLD + 40,
					0xff);*/
		}
		else if(glTPState & 0x8000)
		{
			STBSendInfo(AWOKE1,iLeftTime);
			if (gpCurTopMenu)
			{
				gpCurTopMenu->Destroy();
				gpCurTopMenu = NULL;
			}
			ResetCurLButtonDownView();
			//printf("走这里 1\n");
			TDDebug("弹出 TOPMENUVIEW_AWOKE\n");
			gpCurTopMenu = &gTopMenu[TOPMENUVIEW_AWOKE];
			gpCurTopMenu->Create();
			gpCurTopMenu->dwModuleID = MODULE_TOPMENU_AWOKE;
			//printf("DDD\n");
			gpCurTopMenu->Initialize(iLeftTime, 0);
			//printf("GGGGG\n");
			gpCurTopMenu->DrawWindow(0);
            //printf("HHHH\n");
			gpCurTopMenu->ShowWindow(NULL, SHOW3D/*SHOW_TOPMENUTOTOPMENU*/);
			//printf("VVVV\n");
			glTPState &= 0xFFF;
		}
		else
		{
			gADBarInfo.bIsDraw = FALSE;
			sprintf(gADBarInfo.szNewTxt, "%s%d%s", gczInfo[92], iLeftTime, gczInfo[93]);
			gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
			gADBarInfo.nShowADTimes = 3;
		}
		sprintf(szOSDTxt, "%s%d%s", gczInfo[92], iLeftTime, gczInfo[93]);
		SetOsdShowWord(szOSDTxt, OSD_INFOSTRING, 1, SkinID2LangID(gi8LanguageID), 0, 0, 0xFFFFFFFF);
		glTPState = (glTPState & 0x8000) | (iLeftTime & 0xFFF);
		break;
	case 5:
		while(TH_HasDrawMessage(NULL))
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		while(pthread_mutex_trylock(&gDrawDCMutex) == EBUSY)
		{
			nPauseDrawThread = 0;
			usleep(1000);
		}
		pthread_mutex_unlock(&gDrawDCMutex);
		pthread_mutex_lock(&gsViewIdleMutex);

		ClientTimePrompt();
		STBSendInfo(AWOKE2,iLeftTime);
		nDoublePause = TDTRUE;
		if (gpCurTopMenu)
		{
			if(gpCurTopMenu->dwModuleID == MODULE_TOPMENU_AWOKETIMEOUT)
			{
				pthread_mutex_unlock(&gsViewIdleMutex);
				break;
			}
			else
			{
				gpCurTopMenu->Destroy();
				gpCurTopMenu = NULL;
				ResetCurLButtonDownView();
			}
		}
		
/*
		pthread_mutex_lock(&gLinkGameMutex);
		bLinkGameStart = TDFALSE;
		nLinkStartTime = 0;
		pthread_mutex_unlock(&gLinkGameMutex);
		Sleep(1000);     //if clean room stop game rate <msl 2006.09.29>

*/
		//printf("走这里 2\n");
		TDDebug("弹出 MODULE_TOPMENU_AWOKETIMEOUT\n");
		gpCurTopMenu = &gTopMenu[TOPMENUVIEW_AWOKE];
		gpCurTopMenu->Create();
		gpCurTopMenu->dwModuleID = MODULE_TOPMENU_AWOKETIMEOUT;
		gpCurTopMenu->Initialize(0, 0);
		gpCurTopMenu->DrawWindow(0);
		gpCurTopMenu->ShowWindow(NULL, SHOW3D/*SHOW_TOPMENUTOTOPMENU*/);
		glTPState = -1;
	/*	if (gpCurClientView)
		{
			if(TDGetInBroadcast())
			{
				if(TDGetBroadcastStatus())
				{
					TDDebug("was in Broadcast so next");
					TDKTVCloseBroadcast();
					TDSetInBroadcast(0);
					TDKtvNext();
				}
			}
		}*/
		pthread_mutex_unlock(&gsViewIdleMutex);
		break;
	}
	return 0;
}

void ShowMem()
{
//	return;
	TDCHAR 		*pMem = TDNULL;
	TDINT 		size = 128*1024*1024;
	TDINT 		remainsize = 0;
	char tt[128];
	TDDebug("show memery.");
	while(TRUE)
	{
		pMem = (char *)Malloc(size);
		if(!pMem)
		{

			size -= 2*1024;
			remainsize = size;
			continue;
		}
		else
		{
			TDFree(pMem);
			pMem = TDNULL;
			break;
		}
	}
	//if(remainsize)
	{
		sprintf(tt, "!!!!!!!!!!! memory  %dM%dK%dB  %dB can use now \r\n",remainsize/1024/1024,(remainsize/1024)%1024,remainsize%1024,remainsize);
		TDDebug(tt);
//		printf("%s",tt);
	}
}

void ShowMem_beiyang()
{
//	return;
	int			nLoop = 0;
	int			i = 0;
	long		nOneSize = 1280 * 720 * 4;
	TDCHAR 		*pMem = TDNULL;
	TDINT 		size = 180*1024*1024;
	TDINT 		remainsize = 0;
	char tt[128];
	TDDebug("show memery.");
#if 0
	while(TRUE)
	{
		pMem = (char *)Malloc(size);
		if(!pMem)
		{

			size -= 2*1024;
			remainsize = size;
			continue;
		}
		else
		{
			TDFree(pMem);
			pMem = TDNULL;
			break;
		}
	}
#else
	int nCount = 800;
	typedef   struct tagTestMem
	{
		char * pTmp;
	} TAGTTESTMEM,*PTTESTMEM;
	#define  TAGTTESTMEMSIZE  sizeof(struct tagTestMem)
	
	PTTESTMEM gpTestMem = NULL;
	gpTestMem = (PTTESTMEM)malloc(TAGTTESTMEMSIZE * nCount);
	memset(gpTestMem, 0, TAGTTESTMEMSIZE * nCount);
	char szTmp[256] = "";

	while(1)
	{
		gpTestMem[nLoop].pTmp = NULL;
		
		gpTestMem[nLoop].pTmp = (char *)Malloc(nOneSize);
		
		if(!gpTestMem[nLoop].pTmp)
		{
			sprintf(szTmp,"error nLoop = %d, size = %dM%dK%dB\n", nLoop, (nOneSize * nLoop)/1024/1024,((nOneSize * nLoop)/1024)%1024,(nOneSize * nLoop)%1024);
			printf("%s", szTmp);
			TDDebug(szTmp);
			for(i = 0; i< nLoop; i ++)
			{
				free(gpTestMem[i].pTmp);
				gpTestMem[i].pTmp = NULL;
			}
			free(gpTestMem);
			gpTestMem = NULL;
			
			return;
		}
		else
		{
			sprintf(szTmp,"ok nLoop = %d, size = %dM%dK%dB\n", nLoop, (nOneSize * nLoop)/1024/1024,((nOneSize * nLoop)/1024)%1024,(nOneSize * nLoop)%1024);
			printf("%s", szTmp);
			TDDebug(szTmp);

			memset(gpTestMem[nLoop].pTmp, 0, nOneSize);
			//free(pTemp);
			//pTemp = NULL;
		}

		nLoop ++;
	}
#endif
	//if(remainsize)
	{
		sprintf(tt, "!!!!!!!!!!! memory  %dM%dK%dB  %dB can use now \r\n",remainsize/1024/1024,(remainsize/1024)%1024,remainsize%1024,remainsize);
		TDDebug(tt);
		printf("%s",tt);
	}
}

#if 0
#if 0
TDVOID GetVersion()
{
	strcpy(gczVersion, "Version information is NULL");
}
#endif


#endif

static	void SetPicShowStatus(TDBOOL bPicShowStatus)
{
	gbPicIsShow = bPicShowStatus;
}


static void ShowPic()
{
	SetPicShowStatus(TRUE);
	gADBarInfo.bIsDraw = FALSE;
	//gCompanyLogo.bIsDraw = FALSE;
	if (gbShowLoaded)
	{
		gbShowLoaded = FALSE;
		if(gUseADEnable)
		{
			SetBrushColor(ghDC, 0xff000000);
			if(pPlayBmp != NULL)
			{
				ShowMemBMP2(0, 0, 0,pPlayBmp,1);
				bPlayBmped = TRUE;
				free(pPlayBmp);
				pPlayBmp = NULL;

			}
			else if(bLocationEnable == TDFALSE)
			{
				if(bPlayBmped)
				{
					RedrawWindow();
				}
				else
				{
					//ShowTV(TRUE);

					SetPicShowStatus(FALSE);
					dwTCount = GetMyTickCount();
					nDouble = 0;
					gnDouble = 0;
					gnStaffTime = 0;
					gADBarInfo.bIsDraw = FALSE;
					strcpy(gADBarInfo.szNewTxt, " ");
					gADBarInfo.nIsRefresh = ADB_NEWTXTREFRESH;
					gADBarInfo.bIsDraw = TRUE;
					bActorImageIdleInvalid = FALSE;
					nActorImageIndex = -1;
				}
				bPlayBmped = FALSE;
			}
		}
		else
		{
			if (!pPicImage)
			{
				dwTCount = GetMyTickCount();
				return;
			}
			NewBMP= pPicImage ->mBitmap;
			if(NewBMP)
			{
				SetBrushColor(ghDC, 0xff000000);
				if(gbPicIsShow)// && gpMainFrame)
				{
					int nXPos = 0;
					int nYPos = 0;
					int nWidth = SM_XSCREEN;
					int nHight = SM_YSCREEN;
					if (SM_XSCREEN > NewBMP->bmWidth)
					{
						nXPos = (SM_XSCREEN - NewBMP->bmWidth) / 2;
					}
					if (SM_YSCREEN > NewBMP->bmHeight)
					{
						nYPos = (SM_YSCREEN - NewBMP->bmHeight) / 2;
					}
					if (NewBMP->bmWidth < SM_XSCREEN - 1)
					{
						nWidth = NewBMP->bmWidth;
					}
					if (NewBMP->bmHeight < SM_YSCREEN - 1)
					{
						nHight = NewBMP->bmHeight;
					}
#if 0
					if(gnDouble == 3)
						FillBox(ghDC, 0, 0, SM_XSCREEN, SM_YSCREEN);
					if(gnDouble == 3)
						TDFillBoxWithBitmap(ghDC, nXPos, nYPos, nWidth, nHight, NewBMP, NULL);
#endif
					if(gnDouble == 3)
						FillBox(ghScrMemDC1, 0, 0, SM_XSCREEN, SM_YSCREEN);
					if(gnDouble == 3)
						TDFillBoxWithBitmap(ghScrMemDC1, nXPos, nYPos, nWidth, nHight, NewBMP, NULL);

					if(gnDouble == 3)
					{
						TDBitBlt(ghDC,
							0,
							0,
							SM_XSCREEN,
							SM_YSCREEN,
							ghScrMemDC1,
							0,
							0,
							NULL);
					}
				}
				TDImageDestroy(pPicImage);
				pPicImage = NULL;
			}
		}
		return;
	}
	return;
}

static	void LoadFirstPic()
{
	
	static	int		nCurPic = 0;
	TDCHAR		    szImagePath[512];
	if(gUseADEnable == FALSE)
	{
		if (pPicImage)
		{
			TDImageDestroy(pPicImage);
			pPicImage = NULL;
		}
		sprintf(szImagePath, "http://%s/image_pic/%d.bmp", gKtvsvrip, nCurPic++);
		
		pPicImage = TDImageCreate();
		if (!TDImageLoadSync(pPicImage, szImagePath))
		{
			nCurPic = 0;
			TDImageDestroy(pPicImage);
			pPicImage = TDImageCreate();
			sprintf(szImagePath, "http://%s/image_pic/%d.bmp", gKtvsvrip, nCurPic++);
			if (!TDImageLoadSync(pPicImage, szImagePath))
			{
				TDImageDestroy(pPicImage);
				pPicImage = NULL;
				return;
			}
		}
	}
	else
	{
		if(pPlayBmp)
		{
			free(pPlayBmp);
			pPlayBmp = NULL;
		}
//		GetADInfo(3);
	}
	gbShowLoaded = TRUE;
	return;
}

static void GetPicData()
{
	TDPString pStrUsePic = NULL;
	TDPString pStrPicShowTime = NULL;
	pStrUsePic = TDGetNetworkInfo("showpicture");
	pStrPicShowTime = TDGetNetworkInfo("showtime");
	if(pStrUsePic || gUseADEnable == TRUE)
	{
		if (atoi(pStrUsePic->mStr) == 1 ||  gUseADEnable == TRUE)
		{
			LoadFirstPic();
			if(gbShowLoaded)
				gbUsePic = TRUE;
			else
				gbUsePic = FALSE;
		}
	}
	else
	{
		gbUsePic = FALSE;
	}
	if(pStrPicShowTime)
	{
		if (atoi(pStrPicShowTime->mStr) < 5)
		{
			gdwPicShowTime = 4;
		}
		else
		{
			gdwPicShowTime = (atoi(pStrPicShowTime->mStr) - 1);
		}
	}
}


static	void RedrawWindow()
{
	if(gbUsePic && !gUseADEnable && gbIsFullScreen == 0 )
	{
		SetPicShowStatus(FALSE);
		if(gpCurClientView)
		{
			TDBitBlt(ghDC,
				0,
				0,
				SM_XSCREEN,
				SM_YSCREEN,
				ghScrMemDC,
				0,
				0,
				NULL);
			if(gpCurClientView->dwModuleID == 0x10100000)
			{
				ResumeThreadLock();	
			}
		}
	}
	else
	{
		if(gpCurClientView)
		{
			int i = 0;			
            float   x ;
            float   y ;
            float   xWidth ;
            float   yHeight ;
            float   xOffset ;
            float   yOffset ;
            float   WideOffset ;
            float   HeightOffset ;
			float   nowWide = 0 ;
			float   nowHeight = 0 ;

			if(gpCurTopMenu && gpCurTopMenu->dwModuleID == MODULE_TOPMENU_SELECTED)
			{
				//printf("MTV windows!!!\n");
				
				SetBrushColor(ghScrMemDC1, 0x00000000);
				FillBox(ghScrMemDC1, 0, 0, SM_XSCREEN, SM_YSCREEN);

				TDBitBltEx(ghScrMemDC1,
					gpCurTopMenu->Rect.left,
					gpCurTopMenu->Rect.top,
					gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left,
					gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
					BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
					ghScrMemDC2,
					gpCurTopMenu->Rect.left,
					gpCurTopMenu->Rect.top,
					gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left,
					gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
					gpCurTopMenu->hBGBitmap,
					0,
					0,
					gpCurTopMenu->Rect.right - gpCurTopMenu->Rect.left,
					gpCurTopMenu->Rect.bottom - gpCurTopMenu->Rect.top,
					0x0,
					0x0);

				TDBitBltEx(ghScrMemDC1,
					0,
					0,
					SM_XSCREEN,
					SM_YSCREEN,
					BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
					ghScrMemDC1,
					0,
					0,
					SM_XSCREEN,
					SM_YSCREEN,
					ghScrMemDC,
					0,
					0,
					SM_XSCREEN,
					SM_YSCREEN,
					0x0,
					0x0);
				TDBitBltEx(ghScrMemDC1,
					gpCurTopMenu->Rect.left + MTVVEDIOLEFT,
					gpCurTopMenu->Rect.top + MTVVEDIOTOP,
					MTVVEDIOWIDTH,
					MTVVEDIOHEIGHT,
					BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
					ghScrMemDC1,
					gpCurTopMenu->Rect.left + MTVVEDIOLEFT,
					gpCurTopMenu->Rect.top + MTVVEDIOTOP,
					MTVVEDIOWIDTH,
					MTVVEDIOHEIGHT,
					ghScrMemDC, 
					gpCurTopMenu->Rect.left + MTVVEDIOLEFT,
					gpCurTopMenu->Rect.top + MTVVEDIOTOP,
					MTVVEDIOWIDTH,
					MTVVEDIOHEIGHT,
					0x0,
					0x00
					);
//////////////////////////////////////////////////////////////////////////
				{	
					nowWide   = 1280.0 -4.0;
					nowHeight = 720.0 -2.0;		
					x = 0.0;
					y = 0.0  ;
					xWidth = 505.0 ;
					yHeight = 379.0 ;
					xOffset = (x - (733.0 + 3.0)) /offsetNum;
					yOffset = (y - (87.0 + 89.0 )) /offsetNum ;
					WideOffset = (xWidth - nowWide ) /offsetNum ;
					HeightOffset = (yHeight -nowHeight) /offsetNum ;
				}
				SetBrushColor(ghScrMemDC4, 0x00000000);
                FillBox(ghScrMemDC4,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN
                        );
				TDBitBlt(ghScrMemDC4,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						ghScrMemDC1,
						0,
						0,
						NULL);

				SetMasterWindow(gpCurTopMenu->Rect.left + MTVVEDIOLEFT,
					gpCurTopMenu->Rect.top + MTVVEDIOTOP,
					MTVVEDIOWIDTH,
					MTVVEDIOHEIGHT,
					1,
					1);
				/*
				SetMasterWindow(
						(int)(x - xOffset * offsetNum),
						(int)(y - yOffset * offsetNum),
						(int)(WideOffset * offsetNum + nowWide),
						(int)(HeightOffset * offsetNum+ nowHeight),
						1,
						1); */

				TDBitBlt(ghDC,
					0,
					0,
					SM_XSCREEN,
					SM_YSCREEN,
					ghScrMemDC1,
					0,
					0,
					NULL);
				//////////////////////////////////////////////////////////////////////////
			}
			else if( gbIsFullScreen == 1 )
			{
				//printf("auto MTVredraw!!!\n");
				
				{	
					nowWide   = 1280.0 -4.0;
					nowHeight = 720.0 -2.0;		
					x = 0.0;
					y = 0.0  ;
					xWidth = 447.0 ;
					yHeight = 335.0 ;
					xOffset = (x - (768.0 + 19.0)) /offsetNum;
					yOffset = (y - (54.0 + 93.0 )) /offsetNum ;
					WideOffset = (xWidth - nowWide ) /offsetNum ;
					HeightOffset = (yHeight -nowHeight) /offsetNum ;
				}
				SetMasterWindow(
					(int)(x - xOffset * offsetNum),
					(int)(y - yOffset * offsetNum),
					(int)(WideOffset * offsetNum + nowWide),
					(int)(HeightOffset * offsetNum+ nowHeight),
					0,
					1); 
#if 1
				if(gpCurClientView)
				{
					TDBitBlt(ghDC,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						ghScrMemDC,
						0,
						0,
						NULL);

//					if(gpCurClientView->dwModuleID == 0x10100000)
					{
						ResumeThreadLock();	
					}
				}
#endif
			}
			else
			{
				//printf("other windows!!!\n");
				if(gpCurClientView && gpCurClientView->dwModuleID == 0x10300001)//paopao
				{
#if 0
					TDBitBltEx(ghScrMemDC1,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC1,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						ghScrMemDCFanZhuan,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						0x0,
						0x0);

					TDBitBltEx(ghScrMemDC1,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC1,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						ghScrMemDC,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						0x0,
						0x0);
#endif
				}
				else
				{
					TDBitBlt(ghScrMemDC1,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN,
						ghScrMemDC,
						0,
						0,
						NULL);
				}
//////////////////////////////////////////////////////////////////////////
				{	
					nowWide = 54.0 ;
					nowHeight = 33.0 ;		
					x = 428.0 ;
					y = 21.0 ;

					nowWide   = 1280.0 -4.0;
					nowHeight = 720.0 -2.0;		
					x = 0.0;
					y = 0.0  ;
					xWidth = 54.0;//60.0 ;
					yHeight =33.0;// 39.0 ;
					xOffset = (x - (gLeftTopPanel.Rect.left + 428.0)) /offsetNum;//425.0
					yOffset = (y - (gLeftTopPanel.Rect.top + 21.0 )) /offsetNum ;//18.0
					WideOffset = (xWidth - nowWide ) /offsetNum ;
					HeightOffset = (yHeight -nowHeight) /offsetNum ;
				}
				SetBrushColor(ghScrMemDC4, 0x00000000);
                FillBox(ghScrMemDC4,
						0,
						0,
						SM_XSCREEN,
						SM_YSCREEN
                        );

				TDBitBlt(ghDC,
					0,
					0,
					SM_XSCREEN,
					SM_YSCREEN,
					ghScrMemDC1,
					0,
					0,
					NULL);
			}
			gLeftTopPanel.DrawWindow(0);
			gLeftTopPanel.ShowWindow(NULL, 0);	
		}
		//ShowTV(TRUE);
		SetPicShowStatus(FALSE);
		gbIsFullScreen = 0;
	}

	dwTCount = GetMyTickCount();
	nDouble = 0;
	gnDouble = 0;
	gnStaffTime = 0;
	return;
}

TDDWORD TimeDelay1Thread()
{
	int 	nLoop = 0;
	char	szTmpTxt[128];
	TabletDelayBegin1Event = CreateEvent();
	TabletSend1Event = CreateEvent();
	TabletSend1OKEvent = CreateEvent();
	TabletDelayBegin2Event = CreateEvent();
	TabletSend2Event = CreateEvent();
	TabletSend2OKEvent = CreateEvent();
	TabletPrePareOKEvent = CreateEvent();
	TDDebug("1WAIT TABLET BEGIN");
	while(1)
	{
		sleep(3);
#if 1//yang
		TDDebug("1!1 WAIT TABLET TABLETDELAYBEGINEVENT");
		WaitForSingleObject(TabletDelayBegin1Event, INFINITE);//HandWrite TopMenu LButtonDown;
		TDDebug("1!3 WAIT TABLET TABLETDELAYBEGINEVENT ok");

		TDDebug("1!4 WAIT TABLET SendEvent ");

		while(WAIT_OBJECT_0 == WaitForSingleObject(TabletSend1Event, 1000))//2seconds delay;HandWriteTopMenuLButtonMove;
		{	 //when move not timeout;
			//TDDebug("1!6 WAIT TABLET TABLESendEvent continue");
			;
		}
		TDDebug("1!8 WAIT TABLET TABLESendEvent time out");

		/*if(gFireStatus)
		{
			continue;
		}*/

		TDDebug("1before send WM_TABLETGETRESULT");
		pthread_mutex_lock(&gDrawDCMutex);
		pthread_mutex_lock(&gMemSongInfoMutex);	
		for(nLoop = 0; nLoop < gHandWriteBufferLen; nLoop++)
		{
			if((gHandWriteTable[nLoop].nHandWriteTablet == 1) &&
					(gHandWriteTable[nLoop].nSendFlag == 0) &&
					(gHandWriteTable[nLoop].nPrepareFlag == 0) &&
					(gHandWriteTable[nLoop].nPointNum > 0))
			{
				gHandWriteTable[nLoop].nSendFlag = 0;
				gHandWriteTable[nLoop].nPrepareFlag = 1;
				sprintf(szTmpTxt,"gHandWriteTable[%d].nSendFlag = %d, nPrePareFlag = %d, nHandWriteTablet = %d, nPointNum = %d",nLoop,gHandWriteTable[nLoop].nSendFlag,gHandWriteTable[nLoop].nPrepareFlag,gHandWriteTable[nLoop].nHandWriteTablet,gHandWriteTable[nLoop].nPointNum);
				TDDebug(szTmpTxt);
				if(gpCurTopMenu && (gpCurTopMenu == &gTopMenu[TOPMENUVIEW_HANDWRITE] || gpCurTopMenu->dwTopID == MODULE_TOPMENU_INPUTHANDWRITE) && !gnPageHasChanged)
				{printf("enter 1111111111TimeDelay1Thread\n");
					TDBitBltEx(ghScrMemDC1,
						gpFirstTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpFirstTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC2,
						gpFirstTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpFirstTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						gpCurTopMenu->hBGBitmap, 
						gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						0x0,
						0x00);
					
					TDBitBltEx(ghDC,
						gpFirstTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpFirstTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC1,
						gpFirstTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpFirstTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						ghScrMemDC, 
						gpFirstTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpFirstTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						0x0,
						0x00);

				}
#if 1
				else if(gpKeyBoard && ( gpKeyBoard->dwTopID == MODULE_TOPMENU_INPUTHANDWRITE) && !gnPageHasChanged)
				{
					printf("enter gpKeyBoard TimeDelay1Thread\n");
					TDBitBltEx(ghScrMemDC1,
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC2,
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						gpKeyBoard->hBGBitmap, 
						gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						0x0,
						0x00);
					
					TDBitBltEx(ghDC,
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC1,
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						ghScrMemDC, 
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						0x0,
						0x00);
				
				}
#endif				
			}
		}
		pthread_mutex_unlock(&gMemSongInfoMutex);
		pthread_mutex_unlock(&gDrawDCMutex);
		SetEvent(TabletPrePareOKEvent);//in this function HandWriteRecogniseThread
		nMouseType1 = MOUSEFIRST;//init nMouseType1 default again;
		TDDebug("1set nMouseType1 = MOUSEFIRST");
		TDDebug("1after send WM_TABLETGETRESULT");
#endif
	}
}

TDDWORD TimeDelay2Thread()
{
	char	szTmpTxt[128];
	int 	nLoop = 0;
	TDDebug("2WAIT TABLET BEGIN");

	usleep(1000 * 1000);
	while(1)
	{
		sleep(3);//yang test
#if 1//yang
		TDDebug("2!1 WAIT TABLET TABLETDELAYBEGINEVENT");
		WaitForSingleObject(TabletDelayBegin2Event, INFINITE);
		TDDebug("2!3 WAIT TABLET TABLETDELAYBEGINEVENT ok");

		TDDebug("2!4 WAIT TABLET SendEvent ");

		while(WAIT_OBJECT_0 == WaitForSingleObject(TabletSend2Event, 1000))
		{
		//	TDDebug("2!6 WAIT TABLET TABLESendEvent continue");
			;
		}
		TDDebug("2!8 WAIT TABLET TABLESendEvent time out");

		/*if(gFireStatus)
		{
			continue;
		}*/

		TDDebug("2before send WM_TABLETGETRESULT");
		pthread_mutex_lock(&gDrawDCMutex);
		pthread_mutex_lock(&gMemSongInfoMutex);
		for(nLoop = 0; nLoop < gHandWriteBufferLen; nLoop++)
		{
			if((gHandWriteTable[nLoop].nHandWriteTablet == 2) &&
					(gHandWriteTable[nLoop].nSendFlag == 0) &&
					(gHandWriteTable[nLoop].nPrepareFlag == 0) &&
					(gHandWriteTable[nLoop].nPointNum > 0))
			{
				gHandWriteTable[nLoop].nSendFlag = 0;
				gHandWriteTable[nLoop].nPrepareFlag = 1;
				sprintf(szTmpTxt,"gHandWriteTable[%d].nSendFlag = %d, nPrePareFlag = %d, nHandWriteTablet = %d, nPointNum = %d",nLoop,gHandWriteTable[nLoop].nSendFlag,gHandWriteTable[nLoop].nPrepareFlag,gHandWriteTable[nLoop].nHandWriteTablet,gHandWriteTable[nLoop].nPointNum);
				TDDebug(szTmpTxt);
//				if(gpCurTopMenu == &gTopMenu[TOPMENUVIEW_HANDWRITE] && !gnPageHasChanged)
				if(gpCurTopMenu && (gpCurTopMenu == &gTopMenu[TOPMENUVIEW_HANDWRITE] || gpCurTopMenu->dwTopID == MODULE_TOPMENU_INPUTHANDWRITE) && !gnPageHasChanged)
				{printf("enter 22222222222 TimeDelay2Thread\n");
					TDDebug("gpCurTopMenu && (gpCurTopMenu == &gTopMenu[TOPMENUVIEW_HANDWRITE] || gpCurTopMenu->dwTopID == MODULE_TOPMENU_INPUTHANDWRITE) && !gnPageHasChanged");
					TDBitBltEx(ghScrMemDC1,
						gpSecondTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpSecondTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpSecondTabletHotspot->rect.right - gpSecondTabletHotspot->rect.left,
						gpSecondTabletHotspot->rect.bottom - gpSecondTabletHotspot->rect.top,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC2,
						gpSecondTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpSecondTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpSecondTabletHotspot->rect.right - gpSecondTabletHotspot->rect.left,
						gpSecondTabletHotspot->rect.bottom - gpSecondTabletHotspot->rect.top,
						gpCurTopMenu->hBGBitmap, 
						gpSecondTabletHotspot->rect.left,
						gpSecondTabletHotspot->rect.top,
						gpSecondTabletHotspot->rect.right - gpSecondTabletHotspot->rect.left,
						gpSecondTabletHotspot->rect.bottom - gpSecondTabletHotspot->rect.top,
						0x0,
						0x00);
					
					TDBitBltEx(ghDC,
						gpSecondTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpSecondTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpSecondTabletHotspot->rect.right - gpSecondTabletHotspot->rect.left,
						gpSecondTabletHotspot->rect.bottom - gpSecondTabletHotspot->rect.top,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC1,
						gpSecondTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpSecondTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpSecondTabletHotspot->rect.right - gpSecondTabletHotspot->rect.left,
						gpSecondTabletHotspot->rect.bottom - gpSecondTabletHotspot->rect.top,
						ghScrMemDC, 
						gpSecondTabletHotspot->rect.left + gpCurTopMenu->Rect.left,
						gpSecondTabletHotspot->rect.top + gpCurTopMenu->Rect.top,
						gpSecondTabletHotspot->rect.right - gpSecondTabletHotspot->rect.left,
						gpSecondTabletHotspot->rect.bottom - gpSecondTabletHotspot->rect.top,
						0x0,
						0x00);
				}
#if 1
				else if(gpKeyBoard && ( gpKeyBoard->dwTopID == MODULE_TOPMENU_INPUTHANDWRITE) && !gnPageHasChanged)
				{
					printf("enter gpKeyBoard TimeDelay2Thread\n");
					TDBitBltEx(ghScrMemDC1,
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC2,
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						gpCurClientView->hBGBitmap, 
						gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						0x0,
						0x00);
					
					TDBitBltEx(ghDC,
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
						ghScrMemDC1,
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						ghScrMemDC, 
						gpFirstTabletHotspot->rect.left + gpKeyBoard->Rect.left,
						gpFirstTabletHotspot->rect.top + gpKeyBoard->Rect.top,
						gpFirstTabletHotspot->rect.right - gpFirstTabletHotspot->rect.left,
						gpFirstTabletHotspot->rect.bottom - gpFirstTabletHotspot->rect.top,
						0x0,
						0x00);
				
				}
	#endif			
			}
		}
		pthread_mutex_unlock(&gMemSongInfoMutex);
		pthread_mutex_unlock(&gDrawDCMutex);
		SetEvent(TabletPrePareOKEvent);
		nMouseType2 = MOUSEFIRST;
		TDDebug("2set nMouseType2 = MOUSEFIRST");
#endif
	}
}

TDDWORD HandWriteRecogniseThread()
{
	char		szTmpTxt[128];
	int 		nLoop = 0;
	int		nMediaTypeID = 0;
	usleep(1000 * 1000);
	while(1)
	{
		WaitForSingleObject(TabletPrePareOKEvent, INFINITE);
#if 1//yang	
		if(gpCurTopMenu == NULL)
		{
			TDDebug("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy topmenu is null");			
			continue;			
		}
		//printf("@@@@@@@@@@ = %d\n",gHandWriteBufferLen);
		for(nLoop = 0; nLoop < gHandWriteBufferLen; nLoop++)
		{
			if((gHandWriteTable[nLoop].nSendFlag == 0) && (gHandWriteTable[nLoop].nPrepareFlag == 1))
			{
				if(gnPageHasChanged)
				{
					gHandWriteTable[nLoop].nPointNum = 0;
					gHandWriteTable[nLoop].nSendFlag = 0;
					gHandWriteTable[nLoop].nPrepareFlag = 0;
					gHandWriteTable[nLoop].nHandWriteTablet = 0;
					continue;
				}
				gHandWriteTable[nLoop].nSendFlag = 1;
				TDDebug("to send point message to server and recv the result\n");
				gpCurClientView->dwRightSearchID = 0;
				if((nLoop < (gHandWriteBufferLen - 1) ) && (gHandWriteTable[nLoop + 1].nPointNum > 0))
				{
					TDDebug("GetTabletData & nNotSearch = 1\n");
					if(gpCurTopMenu && gpCurTopMenu->dwTopID == MODULE_TOPMENU_INPUTHANDWRITE)
					{
						GetTabletData(0, nLoop, 1);
					}
					else
					{						
						switch((gpCurClientView->dwModuleID) & 0xFFF00000)
						{
						case 0x19200000://song name
							GetTabletDataSearchByType(0, nLoop, 1, 0, 0, 0, PageStack.wSongLength, 0);
							break;
						case 0x11300000://length
							GetTabletDataSearchByType(0, nLoop, 1, 0, 0, 0, PageStack.bCurPageInfo, 0);
							break;
						case 0x11500000://language
							GetTabletDataSearchByType(0, nLoop, 1, 0, 0, PageStack.bCurPageInfo, PageStack.wSongLength, 0);
							break;
						case 0x11600000://type
							nMediaTypeID = PageStack.bCurPageInfo;
							GetTabletDataSearchByType(0, nLoop, 1, 0, nMediaTypeID, 0, PageStack.wSongLength, 0);
							break;
						case 0x11700000:
							GetTabletData(0, nLoop, 1);
							break;
						case 0x11800000://newsong
							//GetTabletDataSearchByType(0, nLoop, 1, PageStack.bCurPageInfo, 0, 0, PageStack.wSongLength, 0);
							GetTabletDataSearchByType(0, nLoop, 1, 1, 0, 0, PageStack.wSongLength, 0);
							break;
						case 0x11200000:
							GetTabletDataSearchByType(0, nLoop, 1, 0, 0, 0, ActorPage.wSongLength, ActorPage.bCurPageInfo);
							break;
						case 0x10900000:
							GetTabletData(0, nLoop, 1);
							break;
						case 0x11000000:
							printf("enter  0x11000000\n");
							GetTabletData(0, nLoop, 1);
							break;
						default:
							break;
						}
					}
					//GetTabletData(0, nLoop, 1);
				}
				else
				{
					TDDebug("GetTabletData & nNotSearch = 0\n");
					if(gpCurTopMenu && gpCurTopMenu->dwTopID == MODULE_TOPMENU_INPUTHANDWRITE)
					{printf("enter   11159  MODULE_TOPMENU_INPUTHANDWRITE \n");
						GetTabletData(0, nLoop, 1);
					}
					else
					{							
						switch((gpCurClientView->dwModuleID) & 0xFFF00000)
						{
						case 0x19200000://song name
							GetTabletDataSearchByType(0, nLoop, 0, 0, 0, 0, PageStack.wSongLength, 0);
							break;
						case 0x11300000://length
							GetTabletDataSearchByType(0, nLoop, 0, 0, 0, 0, PageStack.bCurPageInfo, 0);
							break;
						case 0x11500000://language
							GetTabletDataSearchByType(0, nLoop, 0, 0, 0, PageStack.bCurPageInfo, PageStack.wSongLength, 0);
							break;
						case 0x11600000://type
							nMediaTypeID = PageStack.bCurPageInfo;
							GetTabletDataSearchByType(0, nLoop, 0, 0, nMediaTypeID, 0, PageStack.wSongLength, 0);
							break;
						case 0x11700000:
							GetTabletData(0, nLoop, 0);
							break;
						case 0x11800000://newsong
							//GetTabletDataSearchByType(0, nLoop, 0, PageStack.bCurPageInfo, 0, 0, PageStack.wSongLength, 0);
							GetTabletDataSearchByType(0, nLoop, 0, 1, 0, 0, PageStack.wSongLength, 0);
							break;
						case 0x11200000:
							GetTabletDataSearchByType(0, nLoop, 0, 0, 0, 0, ActorPage.wSongLength, ActorPage.bCurPageInfo);
							break;
						case 0x10900000:
							GetTabletData(0, nLoop, 1);
							break;
						case 0x11000000:
							printf("enter  22222222222222222222 0x11000000 \n");
							GetTabletData(0, nLoop, 0);
							break;
						default:
							break;
						}						
					}
					//GetTabletData(0, nLoop, 0);
				}
				TDDebug("to send point message to server and recv the result === end\n");
				gHandWriteTable[nLoop].nPointNum = 0;
				gHandWriteTable[nLoop].nSendFlag = 0;
				gHandWriteTable[nLoop].nPrepareFlag = 0;
				gHandWriteTable[nLoop].nHandWriteTablet = 0;
				sprintf(szTmpTxt,"gHandWriteTable[%d].nSendFlag = %d, nPrePareFlag = %d, nPointNum = %d",nLoop,gHandWriteTable[nLoop].nSendFlag,gHandWriteTable[nLoop].nPrepareFlag, gHandWriteTable[nLoop].nPointNum);
				TDDebug(szTmpTxt);
				printf("szTmpTxt ======%s\n",szTmpTxt);
			}
		}
#endif
	}
}

//yang 
void TDMessageBox(char* strMsg)
{
	if(gpCurTopMenu)
	{
		gpCurTopMenu->Destroy();
		gpCurTopMenu = NULL;
	}
//	printf("TDMessageBox strMsg = %s\n",strMsg);
	gpCurTopMenu = &gTopMenu[TOPMENUVIEW_MSGBOX];
	gpCurTopMenu->Create();
	gpCurTopMenu->Initialize(0, 0);
	strcpy(gpCurTopMenu->czNavigation,strMsg);
	gpCurTopMenu->DrawWindow(0);
	gpCurTopMenu->ShowWindow(NULL, NULL);
}

void TDSecondaryMessageBox(char* strMsg)
{

//	printf("TDMessageBox strMsg = %s\n",strMsg);
	if (gpCurTopMenu) 
	{
		if(gpCurTopMenu->dwModuleID == MODULE_TOPMENU_SELECTED || gpCurTopMenu->dwModuleID == MODULE_TOPMENU_MSGBOX)
		{
			gpCurTopMenu->Destroy();
			gpCurTopMenu = NULL;

			TDMessageBox(strMsg);
			return;
		}
		gpCurSecondaryTopMenu = &gTopMenu[TOPMENUVIEW_MSGBOX];
		gpCurSecondaryTopMenu->Create();
		gpCurSecondaryTopMenu->Initialize(0, 0);
		strcpy(gpCurSecondaryTopMenu->czNavigation,strMsg);
		gpCurSecondaryTopMenu->DrawWindow(0);
		gpCurSecondaryTopMenu->ShowWindow(NULL, 0);
	}
	else
	{
		if (gbPicIsShow || gnDouble == 3)
		{
			SwitchVideo();
		}
		TDMessageBox(strMsg);
	}
}
//..
#if 1
int TDDoRecord()
{
	if (!gRecordDevInited)
	{
		return 1;
	}

	if ((TDGetRecordCommandType() == 1) || (TDGetRecordCommandType() == 2) || (TDGetRecordCommandType() == 3) || (TDGetRecordCommandType() == 5))
		return 1;

	if(strlen(gszRecordSvrIP) <= 0)//未找到录音服务器
	{
		TDDebug("no RecordSvrIP info");
		return 1;
	}


	if (TDGetRecordStatusType() == 0)
	{
	//如果此时处于停止状态，则取当前播放歌曲名称
	//否则则处于回放歌曲且回放被停止，此时正在回放的歌曲的名称为录音名称
//		GetCurrentSongName(gszCurrentSongName,&gRecordSongLangID);
		TDDWORD dwMediaNO;
		GetPlayingMedia(&dwMediaNO, gszCurrentSongName,&gRecordSongLangID);
	}

	if (strlen(gszCurrentSongName) <= 0)
	{
	//如果取得的歌曲名称为空，则不进行录音
		TDDebug("GetCurrentSongName return false:(");
		return 1;
	}

	if(GetVideoStatus() == 2)  //暂停
	{
		gBottomPanel.OnPress(0, 0xBB);
	}

	if(GetMuteStatus())//voice   //静音
	{
		gBottomPanel.OnPress(0, 0xba);
	}

	if(TDGetRecordIsInNextMedia())
	{
		TDDebug("IsInNextMedia");
		return 1;
	}
	else
	{
		TDSetRecordIsInNextMedia(1);
	}

	TDPrepareRecord(gszCurrentSongName);//更改结构信息为准备录音状态。

	//TD_AudioMute(1);

	TDKTVRecord();
	RecordingMedia();

	sleep(4);

	ClientGetRecordStatus();

	return 0;
}
#endif
int TDDoRecordPlay(BYTE btMediaGroup,TDDWORD dwMediaID)
{
	if (!gRecordDevInited)
	{
		return 1;
	}

	if ((TDGetRecordCommandType() == 2) || (TDGetRecordCommandType() == 3))
		return 1;

	if(strlen(gszRecordSvrIP) <= 0)
	{
		//如果没有录音服务器，不能进行录音
		TDDebug("no RecordSvrIP info");
		return 1;
	}

	if(strlen(gszCurrentSongName) <= 0)
	{
		TDDebug("gszCurrentSongName is NULL");
		return 1;
	}
	else
	{
		if(TDGetRecordIsInNextMedia())
		{
			TDDebug("IsInNextMedia");
			return 1;
		}
		else
		{
			TDSetRecordIsInNextMedia(1);
		}

		if(TDGetRecordCommandType() == 1)
		{
			StopRecordingMedia();
		}

		if(GetVideoStatus() == 2)  //暂停
		{
			gBottomPanel.OnPress(0, 0xBB);
		}
		
		if(GetMuteStatus())//voice   //静音
		{
			gBottomPanel.OnPress(0, 0xba);
		}

		TDDebug("begin play");
		TDKTVRecordPlay(btMediaGroup,dwMediaID);
		RecordPlayingMedia();
		gADBarInfo.ADBarShowTxtID = ADB_NEWTXTSHOW;

		sleep(4);

		ClientGetRecordStatus();

		if(GetSongListTotalCount(TDFALSE) == 1)
		{
			SendNotifyMessage(NULL, REDRAWSELECTVIEW, NULL, NULL);
		}
	}

	return 0;
}

#if 1

int TDStopDoRecord_AND_Play()
{
	if (TDGetRecordCommandType() == 0)
		return 1;
	if(TDGetRecordIsInNextMedia())
	{
		TDDebug("IsInNextMedia");
		return 1;
	}

	TDSetRecordStop();
	StopRecordingMedia();
	return 1;
}
#endif
#if 1
void TransactCardID(char *szCardID)
{
	char 		szSet[128];
	int			nCardType = 0;
	TDINT 	nRetValue = 0; //Temp Value
	int		nTotal = 0;
	TDPBYTE pData = NULL;
	sprintf(szSet,"Get_CardType Card_ID:%s \r\n\r\n",szCardID);
	nCardType = GetDatabaseForNewDrink(szSet,NULL,0);

	sprintf(szSet,"nCardType = %d",nCardType);
	TDDebug(szSet);
	switch(nCardType)
	{
	case 1: //员工
		TDDebug("yuangong kaaaaaaaaaaaaaaaaaaaaaaaaa\n");
		if (strlen(szCardID) > 0)
		{
			strcpy(gszCardID, szCardID);
			gbIsCard = TDTRUE;
		}
		else
		{
			gszUserSerialNO[0] = 0;
		}
		if(gpCurTopMenu == &gTopMenu[TOPMENUVIEW_LOGIN])
		{
			gpCurTopMenu->HandleLBtnUp(0x80004);
		}
		else
		{
			if(gpCurTopMenu != NULL)
			{
				gpCurTopMenu->Destroy();
				gpCurTopMenu = NULL;
			}
			if(ActorPage.bEnabled)
				memset(&ActorPage, 0, sizeof(PAGESTACK));
			if(PageStack.bEnabled)
				memset(&PageStack, 0, sizeof(PAGESTACK));

			/////////
			sprintf(szSet , "Checking_Staff SerialNo:%s StaffPassword:%s Sign:1 \r\n\r\n",gszCardID,"CARDID");
			gnStaffPermissions = GetDatabaseForNewDrink(szSet, NULL, 0);

//			printf("gnStaffPermissions = 0x%x\n",gnStaffPermissions);
			if(gnStaffPermissions <= 0)
			{
				gbIsCard = FALSE;
				memset(gszCardID, 0, sizeof(gszCardID));
				TDDebug("the staff Permissions is NULL\n");
				TDMessageBox("此员工无权限");
				return;
			}
			strcpy(gszUserSerialNO,gszCardID);
			bIsStaffCard = TRUE;
			
			gbIsCard = FALSE;
			memset(gszCardID, 0, sizeof(gszCardID));

			//get staff name
			sprintf(szSet, "GetStaffName StaffNo:%s Sign:%d\r\n\r\n", gszUserSerialNO,bIsStaffCard);
			nTotal = GetDatabaseForNewDrink(szSet, (void **)&pData, 64);
			
			if(nTotal > 0)
			{
				YangPrintf(pData);
				strcpy(gszUserName,(char *)pData);
			}
			else
			{
				strcpy(gszUserName,gszUserSerialNO);
			}
			if(pData)
			{
				free(pData);
				pData = NULL;
			}
			
			//..
			if (gpCurClientView)
			{
				gnEffectState = EFFECT_FLAT_TO_IN;
				gpCurClientView->Destroy();
				gpCurClientView = NULL;
			}
			memset(gDrinkToBackInfo,0,TAGDRINKTOBACKINFOSIZE * CLIENTVIEWCOUNT);
			gDrinkViewNum = 1;
			sprintf(gDrinkToBackInfo[0].szCurNavigation,"%s>>%s", gczInfo[0], gczInfo[231]);

			gDrinkToBackInfo[0].dwClientViewIndex = CLIENTVIEW_NEWDRINKMAIN;
			
			gpCurClientView = &gClientView[CLIENTVIEW_NEWDRINKMAINSTAFF];
			gpCurClientView->Create();
			gpCurClientView->Initialize(0, 0);
			gpCurClientView->DrawWindow(0);
			gpCurClientView->ShowWindow(NULL, SHOW3D | SHOWNEXTANDPROCPAGE);
		}
		break;
	case 2://会员
		TDDebug("huiyuan kaaaaaaaaaaaaaaaaaaaaaaaaa\n");
		STBSendInfo(MEMBERCARD,szCardID);
		if(gpCurTopMenu != NULL)
		{
			gpCurTopMenu->Destroy();
			gpCurTopMenu = NULL;
		}
		if(ActorPage.bEnabled)
			memset(&ActorPage, 0, sizeof(PAGESTACK));
		if(PageStack.bEnabled)
			memset(&PageStack, 0, sizeof(PAGESTACK));
		if (gpCurClientView)
		{
			gnEffectState = EFFECT_FLAT_TO_IN;
			gpCurClientView->Destroy();
			gnEffectState = EFFECT_OUT_TO_FLAT;
			gpCurClientView = NULL;
		}

		memset(gDrinkToBackInfo,0,TAGDRINKTOBACKINFOSIZE * CLIENTVIEWCOUNT);
		gDrinkViewNum = 1;
		sprintf(gDrinkToBackInfo[0].szCurNavigation, "%s>>%s", gczInfo[0], gczInfo[231]);
		
		gDrinkToBackInfo[0].dwClientViewIndex = CLIENTVIEW_NEWDRINKMAIN;

		gpCurClientView = &gClientView[CLIENTVIEW_NEWDRINKMEMBLE];//47
		gpCurClientView->Create();
		gpCurClientView->Initialize(0, szCardID);
		gpCurClientView->DrawWindow(0);
		gpCurClientView->ShowWindow(NULL, SHOW3D);
		break;
	case 3: //VIP
		TDDebug("VIP kaaaaaaaaaaaaaaaaaaaaaaaaa\n");
		CallStageWork(szCardID);
		break;
	case 5://重复卡号
		TDDebug("chongfu kaaaaaaaaaaaaaaaaaaaaaaaaa\n");
		TDMessageBox(gczInfo[161]);
		break;
	default:
		TDDebug("wuxiao kaaaaaaaaaaaaaaaaaaaaaaaaa\n");
		TDMessageBox("卡号无效，请确认");
		break;
	}
}
#endif
#if 1
int SendMessage(THHWND hWnd, int iMsg, THWPARAM wParam, THLPARAM lParam)
{
	int	nRet = 0;

	//this function will lock gDCMutex, then call TH_SendMessage
	//in TH_SendMessage will call WndProc.so we can lock all user message
	//pthread_mutex_lock(&gsViewIdleMutex);
	pthread_mutex_lock(&gDCMutex);
	nRet = TH_SendMessage(hWnd, iMsg, wParam, lParam);
	pthread_mutex_unlock(&gDCMutex);
	//pthread_mutex_unlock(&gsViewIdleMutex);

	return nRet;
}
#endif


