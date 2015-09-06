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
#include "toolComm/UdpSearch.h"
#include "controlServer/ConServer.h"



BOOL g_main_start=TRUE;

void InitAllConfig()
{
	INT32S iRet=InitDeviceConfig(DEVICECONFIGDIR,&g_stConfigCfg);
	LOGOUT("InitCfgMng over iRet=%d",iRet);
	iRet=GetServerNo(DEVICECONFIGDIR,g_szServerNO,sizeof(g_szServerNO));
	if(0==strlen(g_szServerNO))
	{
		LOGOUT("GetServerNo over iRet=%d",iRet);
	}
	else
	{
		LOGOUT("GetServerNo %s over iRet=%d",g_szServerNO,iRet);
	}	
	iRet=GetProductId(DEVICECONFIGDIR,g_szProductId,sizeof(g_szProductId));
	if(0==strlen(g_szProductId))
	{
		LOGOUT("GetProductId over iRet=%d",iRet);
	}
	else
	{
		LOGOUT("GetProductId %s over iRet=%d",g_szProductId,iRet);
	}
}

void ReleaseAllConfig()
{
	ReleaseDeviceConfig();
}

int main()
{
	int iRet = 0;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	sigprocmask(SIG_BLOCK, &set, NULL); 
	InitAllConfig();
	InitOSSConfig(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szBuctetName,
				  g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szOssEndPoint,
				  g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeyId,
				  g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeySecret);
	Init_LogOut(LOGSIZE,LOGDIR,FALSE,TEMPDIR);	
	LOGOUT("Init_LogOut over");
	//InitConServer();
	iRet=InitHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel iRet=%d over",iRet);
	iRet=initAliyunOssTask();
	LOGOUT("initAliyunOssTask iRet=%d over",iRet);
	iRet=InitUdpSearch();
	LOGOUT("InitUdpSearch iRet=%d over",iRet);
	iRet=InitTcpServer();
	LOGOUT("InitTcpServer iRet=%d over",iRet);
	while(g_main_start)
	{
		sleep(2);
		//LOGOUT("client sleep");
	}
	ReleaseTcpServer();
	LOGOUT("ReleaseTcpServer over");
	RealseUdpSearch();
	LOGOUT("RealseUdpSearch over");
	iRet=ReleaseHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel %d over",iRet);
	Release_LogOut();
	LOGOUT("Release_LogOut over");
	ReleaseAllConfig();
	LOGOUT("ReleaseAllConfig over");
	return 0;
}

