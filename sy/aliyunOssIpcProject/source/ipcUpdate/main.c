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
char g_szHardVersion[128] = {0};
char g_szSoftVersion[128] = {0};
char g_szOtherServerNO[80] = {0};


int GetVersionCfg()
{
	char softVersion[256]={0};
	char *ptr=NULL;
	int ret = GetSoftVersion(DEVICECONFIGDIR, softVersion, sizeof(softVersion));
	if(ret == 0)
	{
		LOGOUT("GetSoftVersion success over iRet=%d, allVersion:%s", ret, softVersion);
		ptr=strchr(softVersion,'_');
		if(ptr)
		{	
			int flag=ptr-softVersion;
			memset(g_szHardVersion,0,sizeof(g_szHardVersion));
			memset(g_szSoftVersion,0,sizeof(g_szSoftVersion));
			memcpy(g_szHardVersion,softVersion,MIN(flag,sizeof(g_szHardVersion)));
			memcpy(g_szSoftVersion,softVersion+(flag+1),MIN(strlen(softVersion)-flag-1,sizeof(g_szSoftVersion)));
			LOGOUT("GetSoftVersion success over iRet=%d,hardVersion:%s,softVersion:%s", ret, g_szHardVersion,g_szSoftVersion);
		}
		else
		{
			LOGOUT("GetSoftVersion failed over iRet=%d, allVersion:%s", ret, softVersion);
			return -3;
		}
	}
	else
	{

		LOGOUT("GetSoftVersion failure over iRet=%d, allVersion:%s", ret, softVersion);
	}
	ret = GetServerNo(DEVICECONFIGDIR, g_szOtherServerNO, sizeof(g_szOtherServerNO));
	if(ret == 0)
	{
		LOGOUT("GetSoftVersion success over iRet=%d, hardVersion:%s", ret, g_szOtherServerNO);
	}
	else
	{
		LOGOUT("GetSoftVersion failed over iRet=%d, hardVersion:%s", ret, g_szOtherServerNO);
	}
	return ret;
}
//
int main()
{
	int errorNumber = 0;
	int ret = 0;
	char timeBuf[20] = {0};
	Init_LogOut(LOGSIZE,LOGUPDATEDIR,FALSE,TEMPDIR);
	GetVersionCfg();
	ret = InitUpdate();
	if(ret != 0)
	{
		LOGOUT("InitUpdate failed ret:%d", ret);
	}
	else
	{
		LOGOUT("InitUpdate success ret:%d %s", ret,CMDREBOOT);
	}
	/*memset(timeBuf, 0, sizeof(timeBuf));
	getStrTime(timeBuf);
	ipcRestartReportImageUpdateResult(timeBuf);*/
	while(1)
	{
		sleep(10);
		GetVersionCfg();
		pid_t pid = GetPIDbyName_Wrapper(SDK_PROCESS_NAME); // If -1 = not found, if -2 = proc fs access error
		if(pid != -1)
		{
			errorNumber = 0;
		}
		else
		{
			errorNumber ++;
		}
		if(errorNumber >10)
		{
			errorNumber = 0;
			updateVersion();
			system(CMDREBOOT);
		}
	}
	return 0;
}

