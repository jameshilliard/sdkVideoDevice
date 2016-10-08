#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "LogOut/LogOut.h"
#include "Common/Typedef.h"
#include "Common/DeviceConfig.h"
#include "Common/ClientSocket.h"
#include "Common/GlobVariable.h"
#include "hisdk/hi_sdk.h"
#include "hisdk/hi_param.h"
#include "toolComm/UdpSearch.h"
//#include "controlServer/ConServer.h"
#include "controlServer/ControlServer.h"
#include "Rtpsend/rtpManager.h"



BOOL g_main_start=TRUE;

#if 0
void testMain()
{
	char g711Buffer[100*1024]={0};
	char pcmBuffer[200*1024]={0};
	int  length=0;
	int  lengthOut=0;
	int iRet=readFile("old.g711",g711Buffer,100*1024,&length);
	lengthOut=200*1024;
	LOGOUT("length is %d",length);
	lengthOut=g711a_decode(pcmBuffer,&lengthOut,g711Buffer,length);
	LOGOUT("lengthOut is %d",lengthOut);
	int i=0;
	short int *ptrInt=pcmBuffer;
	short int temp16=0;
	short int maxInt=0;
	for(i=0;i<(lengthOut/2);i++)
	{
		temp16=*ptrInt;
		ptrInt++;
		if(temp16<0)
			temp16=-temp16;
		if(temp16>maxInt)
			maxInt=temp16;
	}
	LOGOUT("maxInt=%d",maxInt);
	float temp=(float)32700/(float)maxInt;
	LOGOUT("temp=%f",temp);
	ptrInt=pcmBuffer;
	for(i=0;i<(lengthOut/2);i++)
	{
		float tempF=*ptrInt*temp;
		*ptrInt=(short int)tempF;
		ptrInt++;
	}
	memset(g711Buffer,0,100*1024);
	length=lengthOut;
	lengthOut=100*1024;
	lengthOut=g711a_encode(g711Buffer,&lengthOut,pcmBuffer,length);
	writeFile("new.g711",g711Buffer,lengthOut);
	LOGOUT("test main over %d",lengthOut);
	exit(0);
}
#endif

void InitAllConfig()
{
	char p2pUidString[64]={0,};
	char secret[80]={0,};
	INT32S iRet=InitDeviceConfig(DEVICECONFIGDIR,&g_stConfigCfg);
	LOGOUT("InitCfgMng over iRet=%d",iRet);
	iRet=GetServerNo(DEVICECONFIGDIR,g_szServerNO,sizeof(g_szServerNO));
	if(0==strlen(g_szServerNO))
	{
		LOGOUT("GetServerNo over iRet=%d",iRet);
		memset(p2pUidString,0,sizeof(p2pUidString));
		iRet=getConfigParam(CONFIGPLATFORM,P2PUID,p2pUidString);
		if(iRet==0 && strlen(p2pUidString)>0)
		{
			strncpy(g_szServerNO,p2pUidString,sizeof(p2pUidString));
			LOGOUT("new serverNo is %s",g_szServerNO);
			SetServerNo(DEVICECONFIGDIR,g_szServerNO,strlen(g_szServerNO));
			iRet=calSecret(g_szServerNO,secret);
			if(0!=strcmp(g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,secret))
			{
				strncpy(g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,secret,sizeof(g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword));
				LOGOUT("new secret is %s",secret);
				SetSecret(DEVICECONFIGDIR,secret,sizeof(secret));
				g_iServerStatus=-1;
			}
		}
	}
	else
	{
		LOGOUT("GetServerNo %s over iRet=%d",g_szServerNO,iRet);
	}	

}

void ReleaseAllConfig()
{
	ReleaseDeviceConfig();
}

void GetVersionCfg()
{
	int ret = 0;
	char hardVersion[20] = {0};
	char softVersion[20] = {0};
	ret = GetHardVersion(DEVICECONFIGDIR, hardVersion, sizeof(hardVersion));
	if(ret == 0)
	{
		LOGOUT("GetHardVersion success over iRet=%d, hardVersion:%s", ret, hardVersion);
	}
	else
	{
		LOGOUT("GetHardVersion failed over iRet=%d, hardVersion:%s", ret, hardVersion);
	}
}

int main(int argc,char*   argv[])
{
	BOOL bOutToSerial=FALSE;
	int iRet = 0;
	
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	sigprocmask(SIG_BLOCK, &set, NULL); 
	InitAllConfig();
	GetVersionCfg();//test
	bOutToSerial=TRUE;
	Init_LogOut(LOGSIZE,LOGDIR,bOutToSerial);	
	LOGOUT("Init_LogOut %s %s", SDK_HARD_FWVERSION, SDK_SYSTEM_FWVERSION);
	Init_RtpSendTask();
	InitControlServer();
	#if 1
	iRet=InitHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel iRet=%d over",iRet);
	if(iRet!=0)
	{
		LOGOUT("error and return");
		return -1;
	}
	#endif
	#if 1
	iRet=InitUdpSearch();
	LOGOUT("InitUdpSearch iRet=%d over",iRet);
	if(iRet!=0)
	{
		LOGOUT("error and return");
		return -1;
	}
	iRet=InitTcpServer();
	LOGOUT("InitTcpServer iRet=%d over",iRet);
	if(iRet!=0)
	{
		LOGOUT("error and return");
		return -1;
	}
	#endif
	while(g_main_start)
	{
		sleep(2);
		//LOGOUT("client sleep");
	}
	ReleaseTcpServer();
	LOGOUT("ReleaseTcpServer over");
	RealseUdpSearch();
	LOGOUT("RealseUdpSearch over");
	#if 1
	iRet=ReleaseHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel %d over",iRet);
	#endif
	Release_LogOut();
	LOGOUT("Release_LogOut over");
	ReleaseAllConfig();
	LOGOUT("ReleaseAllConfig over");
	return 0;
}

