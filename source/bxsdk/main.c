#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "LogOut/LogOut.h"
#include "Common/Typedef.h"
#include "Common/ClientSocket.h"
#include "Server/RouteServer.h"
#include "Server/ControlServer.h"
#include "hisdk/hi_sdk.h"
#include "hisdk/source_sdk.h"


HI_U32 u32Handle=0;

#define 	LOGSIZE		256*1024
#define 	CMDBUFFER	256*1024
#define		LOGDIR		"./flash"
#define 	TEMPDIR 	"./temp"
	
int main()
{
	int iRet = 0;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	sigprocmask(SIG_BLOCK, &set, NULL); 
	Init_LogOut(LOGSIZE,LOGDIR,FALSE,TEMPDIR);
	LOGOUT("Init_LogOut over %d",sizeof(long));
	
	char uname[64]={0,};
	char password[64]={0,};
	memset(uname,0,sizeof(uname));
	memset(password,0,sizeof(password));
	iRet=getUnameAndPassWord(UNAMEFILE,uname,password);
	LOGOUT("iRet=%d uname %s password %s",iRet,uname,password);
	if(iRet!=0)
	{
		memcpy(uname,UNAME,MIN(sizeof(uname),strlen(UNAME)));
		memcpy(password,PWORD,MIN(sizeof(password),strlen(PWORD)));
	}
	if(strlen(uname)==0)
		memcpy(uname,UNAME,MIN(sizeof(uname),strlen(UNAME)));

	char ip[64]={0,};
	char port[64]={0,};
	memset(ip,0,sizeof(ip));
	memset(port,0,sizeof(port));
	iRet=getIPAndPort(NETFILE,ip,port);
	LOGOUT("iRet=%d ip %s port %s",iRet,ip,port);
	if(iRet!=0)
	{
		memcpy(ip,IPHOST,MIN(sizeof(ip),strlen(IPHOST)));
		memcpy(port,IPPORT,MIN(sizeof(port),strlen(IPPORT)));
	}
	if(strlen(ip)==0)
		memcpy(ip,IPHOST,MIN(sizeof(ip),strlen(IPHOST)));
	if(strlen(port)==0)
		memcpy(port,IPPORT,MIN(sizeof(port),strlen(IPPORT)));
	
	iRet=InitNetwork(CMDBUFFER);
	LOGOUT("InitNetwork %d over",iRet);
	iRet=InitControlServer();
	LOGOUT("InitControlServer %d over",iRet);
	iRet=InitHiSDKServer(&u32Handle);
	LOGOUT("InitHiSDKServer %d over",iRet);
	//iRet=InitSourceSDKServer();
	LOGOUT("InitSourceSDKServer %d over",iRet);
	HI_S_VideoEx sVideo;
	sVideo.u32Channel=1;      /* 通道 */
	sVideo.blFlag=HI_TRUE;          /* 主、次码流  */
	sVideo.u32Bitrate=400;	/* 码流 */
	sVideo.u32Frame=6;    /* 帧率 */
	sVideo.u32Iframe=128;	/* 主帧间隔1-300 */
	sVideo.blCbr=HI_TRUE;	    /* 码流模式、HI_TRUE为固定码流，HI_FALSE为可变码流 */		
	sVideo.u32ImgQuality=1;	/* 编码质量1-6 */
	sVideo.u32Width=640;
	sVideo.u32Height=480;
	//iRet=setVideoParam(&u32Handle,sVideo);
	while(1)
	{
		sleep(10);
		//LOGOUT("client sleep");
	}
	iRet=ReleaseHiSDKServer(&u32Handle);
	//iRet=ReleaseSourceSDKServer();
	return 0;
}

