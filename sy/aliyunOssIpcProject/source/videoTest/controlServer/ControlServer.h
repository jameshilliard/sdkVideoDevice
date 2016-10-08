#ifndef __CONCTROLSERVER_H_
#define  __CONCTROLSERVER_H_

#ifndef BUG_CPU_X86

#include <stdio.h>
#include <unistd.h>
#include "../Common/Typedef.h"
#include "../Common/GlobVariable.h"
#include "../Common/DeviceConfig.h"



#define  HTTPVESION 	"HTTP/1.1"
#define  CURLFILENAME  	"/mnt/mtd/ipc/tmpfs/syflash/curlposttest.log"  

#define  	CONCTROLSERVER 				"112.74.86.237:9145"
#define 	LOGIN_CONSERVER_STRING  	"http://%s/goform/ctrlsvr?cmdname=2010&channel=CAP-CTRLSVR&format=1&encryption=1"
#define 	BEAT_CONSERVER_STRING 		"http://%s/goform/ctrlsvr?cmdname=2007&channel=CAP-CTRLSVR&format=1&encryption=1"
#define 	GETRTPSEND_CONSERVER_STRING "http://%s/goform/ctrlsvr?cmdname=2004&channel=CAP-CTRLSVR&format=1&encryption=1"

typedef struct LoginInfo_
{
	char m_cameraid[64];
}LoginInfo;

typedef struct GetRtpSendInfo_
{
	char m_rtpSendIp[64];
	char m_rtpSendPort[64];
	char m_rtpSendHttpPort[64];
}GetRtpSendInfo;


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int InitControlServer();

#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
#endif


