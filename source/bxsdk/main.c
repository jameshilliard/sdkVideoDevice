#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "LogOut/LogOut.h"
#include "Common/Typedef.h"
#include "Common/ConfigMng.h"
#include "Common/ClientSocket.h"
#include "Server/RouteServer.h"
#include "Server/ControlServer.h"
#include "hisdk/hi_sdk.h"
#include "hisdk/source_sdk.h"
#include "UdpSearch/UdpSearch.h"

		
int main()
{
	int iRet = 0;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	sigprocmask(SIG_BLOCK, &set, NULL); 
	Init_LogOut(LOGSIZE,LOGDIR,FALSE,TEMPDIR);
	LOGOUT("Init_LogOut over");
	BOOL bRet=InitCfgMng(DEVICECONFIGDIR);
	LOGOUT("InitCfgMng over bRet=%d",bRet);
	iRet=InitHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel iRet=%d over",iRet);
	iRet=InitNetwork(CMDBUFFER);
	LOGOUT("InitNetwork iRet=%d over",iRet);
	iRet=InitControlServer();
	LOGOUT("InitControlServer iRet=%d over",iRet);
	iRet=InitUdpSearch();
	LOGOUT("InitUdpSearch iRet=%d over",iRet);
	while(1)
	{
		sleep(10);
		//LOGOUT("client sleep");
	}
	iRet=ReleaseHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel %d over",iRet);
	iRet=ReleaseControlServer();
	ReleaseNetwork();
	ReleaseCfgFile();
	Release_LogOut();
	return 0;
}

