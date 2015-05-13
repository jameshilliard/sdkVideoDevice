#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "LogOut/LogOut.h"
#include "Common/Typedef.h"
#include "Common/Configdef.h"
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
	Init_LogOut(LOGSIZE,LOGDIR,TRUE,TEMPDIR);
	LOGOUT("Init_LogOut over %d",sizeof(long));
	iRet=InitHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel %d over",iRet);
	iRet=InitNetwork(CMDBUFFER);
	LOGOUT("InitNetwork %d over",iRet);
	iRet=InitControlServer();
	LOGOUT("InitControlServer %d over",iRet);
	iRet=InitUdpSearch();
	LOGOUT("InitUdpSearch %d over",iRet);
	while(1)
	{
		sleep(10);
		//LOGOUT("client sleep");
	}
	iRet=ReleaseHiSDKVideoAllChannel();
	LOGOUT("InitHiSDKVideoAllChannel %d over",iRet);
	iRet=ReleaseControlServer();
	ReleaseNetwork();
	return 0;
}

