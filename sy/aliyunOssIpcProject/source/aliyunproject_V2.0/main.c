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
	//设置硬件版本信息
	iRet= SetHardVersion(DEVICECONFIGDIR, SDK_HARD_FWVERSION, strlen(SDK_HARD_FWVERSION));//zmt
	if(0==iRet)
	{
		LOGOUT("SetHardVersion success over iRet=%d",iRet);
	}
	else
	{
		LOGOUT("SetHardVersion failed over iRet=%d",iRet);
	}
	//设置软件版本信息
	iRet= SetSoftVersion(DEVICECONFIGDIR, SDK_SYSTEM_FWVERSION, strlen(SDK_SYSTEM_FWVERSION));//zmt
	if(0==iRet)
	{
		LOGOUT("SetSoftVersion success over iRet=%d",iRet);
	}
	else
	{
		LOGOUT("SetSoftVersion failed over iRet=%d",iRet);
	}
}

void ReleaseAllConfig()
{
	ReleaseDeviceConfig();
}
//
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
	//
	ret = GetSoftVersion(DEVICECONFIGDIR, softVersion, sizeof(softVersion));
	if(ret == 0)
	{
		LOGOUT("GetSoftVersion success over iRet=%d, hardVersion:%s", ret, hardVersion);
	}
	else
	{
		LOGOUT("GetSoftVersion failed over iRet=%d, hardVersion:%s", ret, hardVersion);
	}
}
int main()
{

	int iRet = 0;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	sigprocmask(SIG_BLOCK, &set, NULL); 
	InitAllConfig();
	GetVersionCfg();//test
	InitOSSConfig(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szBuctetName,
				  g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szOssEndPoint,
				  g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeyId,
				  g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeySecret);
	Init_LogOut(LOGSIZE,LOGDIR,TRUE,TEMPDIR);	
	LOGOUT("Init_LogOut %s %s", SDK_HARD_FWVERSION, SDK_SYSTEM_FWVERSION);
	//InitConServer();
	iRet=InitHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel iRet=%d over",iRet);
	if(iRet!=0)
	{
		LOGOUT("error and return");
		return -1;
	}
	//test
	//	devInfo_test();
	//exit(1);
	//
	iRet=initAliyunOssTask();
	LOGOUT("initAliyunOssTask iRet=%d over",iRet);
	iRet=InitUdpSearch();
	LOGOUT("InitUdpSearch iRet=%d over",iRet);
	if(iRet!=0)
	{
		LOGOUT("error and return");
		return -1;
	}
	/*iRet=InitTcpServer();
	LOGOUT("InitTcpServer iRet=%d over",iRet);
	if(iRet!=0)
	{
		LOGOUT("error and return");
		return -1;
	}*/

	/*iRet=InitUpdate();
	LOGOUT("updateMoudle iRet=%d over",iRet);
	if(iRet!=0)
	{
		LOGOUT("error and return");
		return -1;
	}*/
	
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

