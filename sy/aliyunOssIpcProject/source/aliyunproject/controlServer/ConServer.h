#ifndef __CONSERVER_H_
#define  __CONSERVER_H_

#ifndef BUG_CPU_X86

#include <stdio.h>
#include <unistd.h>
#include "../Common/Typedef.h"
#include "../Common/GlobVariable.h"


#define  HTTPVESION 	"HTTP/1.1"
#define  CURLFILENAME  	"curlposttest.log"  
//"cgtx.100memory.com"
#define 	STRING_LOGIN_CONSERVER  	"http://%s:%d/ipccmd.php?act=login"
#define 	STRING_LOGOUT_CONSERVER		"http://%s:%d/ipccmd.php?act=logout"
#define 	STRING_DATARECORD_CONSERVER	"http://%s:%d/ipccmd.php?act=dataRecord"
#define 	STRING_RECORD_CONSERVER		"ipc_id=%s&videoPath=%s&creatTimeInMilSecond=%ld&videoFileSize=%d&\
jpgFilePath=%s&videoTimeLength=%d&videoMotionTotal_Dist1=%d&\
videoMotionTotal_Dist2=%d&videoMotionTotal_Dist3=%d&videoMotionTotal_Dist4=%d&\
voiceAlarmTotal=%d"

typedef struct loginReturnInfo
{
	int result;//���ؽ��
	int codeStream;//����
	int frameRate;//֡��
	int streamType;//�������Ʒ�ʽ
	int mainFrameGap;//���֡���
	int isRequireToReboot;//����
	int resolutionRatio;
	
	char IPCCmdServerURL[512];
	char OSSbucketName[256];
	char OSSEndpoint[128];
	char accessKeyId[128];
	char accesskeySecret[128];
	
}LOGINRETURNINFO;//������������Ϣ�б�


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int InitConServer();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
#endif

