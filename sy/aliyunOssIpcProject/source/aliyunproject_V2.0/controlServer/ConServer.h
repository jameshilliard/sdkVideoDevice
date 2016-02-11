#ifndef __CONSERVER_H_
#define  __CONSERVER_H_

#ifndef BUG_CPU_X86

#include <stdio.h>
#include <unistd.h>
#include "../Common/Typedef.h"
#include "../Common/GlobVariable.h"
#include "../Common/DeviceConfig.h"



#define  HTTPVESION 	"HTTP/1.1"
#define  CURLFILENAME  	"/mnt/mtd/ipc/tmpfs/syflash/curlposttest.log"  
//"cgtx.100memory.com"
#define 	STRING_LOGIN_CONSERVER  	"http://%s/ipccmd_1p4.php?act=login"
#define 	STRING_LOGOUT_CONSERVER		"http://%s/ipccmd_1p4.php?act=logout"
#define 	STRING_DATARECORD_CONSERVER	"http://%s/ipccmd_1p4.php?act=dataRecord"
#define 	STRING_GENCYCORD_CONSERVER	"http://%s/ipccmd_1p5.php?act=reportUrgencyRecord"

/*#define 	STRING_RECORD_CONSERVER		"ipc_id=%s&videoPath=%s&creatTimeInMilSecond=%lld&videoFileSize=%d&\
jpgFilePath=%s&videoTimeLength=%d&videoMotionTotal_Dist1=%d&\
videoMotionTotal_Dist2=%d&videoMotionTotal_Dist3=%d&videoMotionTotal_Dist4=%d&\
voiceAlarmTotal=%d"*/
#define 	STRING_RECORD_CONSERVER		"ipc_id=%s&videoPath=%s&creatTimeInMilSecond=%lld&videoFileSize=%d&jpgFilePath=%s&videoTimeLength=%d&motionDetectInfo=%s&soundVolumeInfo=%s"
#define 	STRING_URGENCYRECORD_CONSERVER		"ipc_id=%s&pwd=%s&videoPath=%s&creatTimeInMilSecond=%lld&videoFileSize=%d&jpgFilePath=%s&videoTimeLength=%d&motionDetectInfo=%s&soundVolumeInfo=%s"



typedef struct loginReturnInfo_
{
	int result;//返回结果
	int codeStream;//码流
	int frameRate;//帧率
	int streamType;//码流控制方式
	int mainFrameGap;//最大帧间隔
	int isRequireToReboot;//重启
	int resolutionRatio;
	
	char IPCCmdServerURL[512];
	char OSSbucketName[256];
	char OSSEndpoint[128];
	char accessKeyId[128];
	char accesskeySecret[128];
	
}LOGINRETURNINFO;//服务器返回信息列表

typedef struct recordData_
{
	char 	m_server[80];
	char 	m_id[80];
	char 	m_secret[80];
	char 	m_videoPath[80];
	char 	m_jpgPath[80];
	int 	m_videoFileSize;
	int 	m_jpgFileSize;
	int 	m_videoTimeLength;
	long long m_creatTimeInMilSecond; 
	Motion_Data m_mMotionData;
	int 	m_iUrencyFlag;
	char    m_bUploadVideoFlag;
	char    m_bUploadJpgFlag;
	char    m_bUploadDataFlag;
	char 	m_szReserver[73];
}RecordData;//服务器返回信息列表

#define SERVERID_IGNORE 					0
#define SERVERID_DOWNLOAD_AUDIOFILE 		1
#define SERVERID_START_URGENCYCONDITION 	2
#define SERVERID_END_URGENCYCONDITION 		3
#define SERVERID_MOTION_RECORDCONDITION 	4


typedef struct __attribute__((packed, aligned(8))) ServerCmdInfo_
{
	int 	cmdType;
	union
	{
		char					m_szReserved[LCM_80(sizeof(tagMasterServerCfg))];
		tagUrgencyMotionCfg 	m_objUrgencyMotionCfg;
		tagMotionCfg 			m_objMotionCfg;
	} m_unServerCmdInfo;	
}ServerCmdInfo;

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int InitConServer();
	int loginCtrl(const char *server,const char *v_szId,const char *v_szPwd,LOGINRETURNINFO *returnInfo);
	int exitCtrl(const char *server,const char *v_szId,const char *v_szPwd);
	int dataRecord(const char *server,const char *v_szId, char *videoPath, 
			   long long creatTimeInMilSecond, int videoFileSize,char *jpgFilePath, 
			   int videoTimeLength,Motion_Data mMotionData);
	int checkAndLoadCmdFromServer(ServerCmdInfo *type);
	int reportUrgencyRecord(const char *server,const char *v_szId,const char *v_szPwd, char *videoPath, 
				   long long creatTimeInMilSecond, int videoFileSize,char *jpgFilePath, 
				   int videoTimeLength,Motion_Data mMotionData);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
#endif

