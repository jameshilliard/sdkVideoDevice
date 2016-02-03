#ifndef __HI_PARAMS_H__
#define __HI_PARAMS_H__
#include <stdio.h>
#include <unistd.h>
#include "hi_type.h"


#define 	CONFIGPLATFORM				"/mnt/mtd/ipc/conf/config_platform.ini"
#define 	UNAMEFILE					"/mnt/mtd/ipc/conf/config_user.ini"
#define 	NETFILE						"/mnt/mtd/ipc/conf/config_net.ini"
#define 	ServerNoString				"devname"
#define 	ServerString1				"sccsvraddr"
#define 	ServerString2				""
#define 	ServerString3				""
#define 	ServerString4				""
#define 	ServerPort					"sccsvrport"
#define     HttpPort 				    "httpport"

#define 	UNAME						"admin"
#define 	PWORD						"admin"
#define 	IPHOST						"127.0.0.1"
#define 	IPPORT						"80"
#define 	RECORD_FILE 				"test.hx"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

	int getIPAndPort(char * path,char ip[64],char port[64]);
	int getStringParams(char * path,char *key,char *value,unsigned int size);
	int getUnameAndPassWord(char * path,char uname[64],char password[64]);
	int getParam(const char *paramString,char paramName[64],char paramValue[64]);
	
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#endif
