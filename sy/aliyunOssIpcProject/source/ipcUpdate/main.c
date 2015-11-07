#include <stdio.h>
#include <stdlib.h>
#include "json/cJSON.h"
#include "curl/curl.h"
#include <string.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iconv.h>
#include <error.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "Common/getPid.h"
#include "update/update.h"
#include "LogOut/LogOut.h"
#include "Common/configdef.h"

//
char g_szHardVersion[20] = {0};
char g_szSoftVersion[20] = {0};
char g_szServerNO[80] = {0};


void GetVersionCfg()
{
	int ret = 0;
	ret = GetHardVersion(DEVICECONFIGDIR, g_szHardVersion, sizeof(g_szHardVersion));
	if(ret == 0)
	{
		LOGOUT("GetHardVersion success over iRet=%d, hardVersion:%s", ret, g_szHardVersion);
	}
	else
	{
		LOGOUT("GetHardVersion failed over iRet=%d, hardVersion:%s", ret, g_szHardVersion);
	}
	//
	ret = GetSoftVersion(DEVICECONFIGDIR, g_szSoftVersion, sizeof(g_szSoftVersion));
	if(ret == 0)
	{
		LOGOUT("GetSoftVersion success over iRet=%d, hardVersion:%s", ret, g_szSoftVersion);
	}
	else
	{
		LOGOUT("GetSoftVersion failed over iRet=%d, hardVersion:%s", ret, g_szSoftVersion);
	}
	//
	ret = GetServerNo(DEVICECONFIGDIR, g_szServerNO, sizeof(g_szServerNO));
	if(ret == 0)
	{
		LOGOUT("GetSoftVersion success over iRet=%d, hardVersion:%s", ret, g_szServerNO);
	}
	else
	{
		LOGOUT("GetSoftVersion failed over iRet=%d, hardVersion:%s", ret, g_szServerNO);
	}
}
//
int main()
{
	int errorNumber = 0;
	int ret = 0;
	char timeBuf[20] = {0};
	Init_LogOut(LOGSIZE,LOGDIR,TRUE,TEMPDIR);
	GetVersionCfg();
	ret = InitUpdate();
	if(ret != 0)
	{
		LOGOUT("InitUpdate failed ret:%s  \n", ret);
	}
	else
	{
		LOGOUT("InitUpdate success ret:%s  \n", ret);
	}
	memset(timeBuf, 0, sizeof(timeBuf));
	getStrTime(timeBuf);
	ipcRestartReportImageUpdateResult(timeBuf);
	while(1)
	{
		sleep(5);
		pid_t pid = GetPIDbyName_Wrapper(SDK_PROCESS_NAME); // If -1 = not found, if -2 = proc fs access error
		if(pid != -1)
		{
			errorNumber = 0;
		}
		else
		{
			errorNumber ++;
		}

		if(errorNumber >20)
		{
			errorNumber = 0;
			updateVersion();
		}
	    printf("PID %d\n", pid);
	}
	return 0;
}

