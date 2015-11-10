#include "DeviceConfig.h"


tagConfigCfg g_stConfigCfg;
char 		 g_szServerNO[80] = {0};
char 		 g_szProductId[80] = {0};

//硬件版本号
INT32S SetHardVersion(const char *v_szFlashPath,char *v_szHdVersion,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szHdVersion)
	{
		LOGOUT("v_szFlashPath or v_szVersionNo is NULL or strlen = 0");
		return -1;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "%s/%s",v_szFlashPath, HDVERSIONFILENAME);
	INT32S iRet=CreateConfigFile(szPath, v_szHdVersion, length);
	LOGOUT("set serverNo %s,iRet=%d",v_szHdVersion,iRet);
	return iRet;
}
//获取硬件版本号
INT32S GetHardVersion(const char *v_szFlashPath,char *v_szHdVersion,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szHdVersion)
	{
		LOGOUT("v_szFlashPath or v_szServerNo is NULL or strlen = 0");
		return -1;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	memset(v_szHdVersion, 0, length);
	sprintf(szPath, "%s/%s",v_szFlashPath, HDVERSIONFILENAME);
	INT32S iRet=ReadConfigFile(szPath, v_szHdVersion, length);
	return iRet;
}
//获取软件版本号
INT32S GetSoftVersion(const char *v_szFlashPath,char *v_szSfServer,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szSfServer)
	{
		LOGOUT("v_szFlashPath or v_szServerNo is NULL or strlen = 0");
		return -1;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	memset(v_szSfServer, 0, length);
	sprintf(szPath, "%s/%s",v_szFlashPath, SFVERSIONFILENAME);
	INT32S iRet=ReadConfigFile(szPath, v_szSfServer, length);
	return iRet;
}

//设置版本号
INT32S SetSoftVersion(const char *v_szFlashPath,char *v_szSfServer,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szSfServer)
	{
		LOGOUT("v_szFlashPath or v_szServerNo is NULL or strlen = 0");
		return -1;
	}
	char softVersion[256]={0};
	int ret = GetSoftVersion(DEVICECONFIGDIR, softVersion, sizeof(softVersion));
	if(ret == 0)
	{
		LOGOUT("GetSoftVersion success over iRet=%d, softVersion:%s", ret, softVersion);
	}
	else
	{
		LOGOUT("GetSoftVersion failed over iRet=%d, softVersion:%s", ret, softVersion);
	}
	if(strcmp(softVersion,v_szSfServer)==0)
	{
		return 0;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "%s/%s",v_szFlashPath, SFVERSIONFILENAME);
	INT32S iRet=CreateConfigFile(szPath, v_szSfServer, length);
	LOGOUT("set serverNo %s,iRet=%d",v_szSfServer,iRet);
	return iRet;
}


//
INT32S SetServerNo(const char *v_szFlashPath,char *v_szServerNo,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szServerNo)
	{
		LOGOUT("v_szFlashPath or v_szServerNo is NULL or strlen = 0");
		return -1;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "%s/%s",v_szFlashPath, SERVERNOFILENAME);
	INT32S iRet=CreateConfigFile(szPath, v_szServerNo, length);
	LOGOUT("set serverNo %s,iRet=%d",v_szServerNo,iRet);
	return iRet;
}

INT32S GetServerNo(const char *v_szFlashPath,char *v_szServerNo,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szServerNo)
	{
		LOGOUT("v_szFlashPath or v_szServerNo is NULL or strlen = 0");
		return -1;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	memset(v_szServerNo, 0, length);
	sprintf(szPath, "%s/%s",v_szFlashPath, SERVERNOFILENAME);
	INT32S iRet=ReadConfigFile(szPath, v_szServerNo, length);
	return iRet;
}

INT32S SetProductId(const char *v_szFlashPath,char *v_szProductId,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szProductId)
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "%s/%s",v_szFlashPath, PRODUCTFILENAME);
	INT32S iRet=CreateConfigFile(szPath, v_szProductId, length);
	return iRet;
}

INT32S GetProductId(const char *v_szFlashPath,char *v_szProductId,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szProductId)
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	memset(v_szProductId, 0, length);
	sprintf(szPath, "%s/%s",v_szFlashPath, PRODUCTFILENAME);
	INT32S iRet=ReadConfigFile(szPath, v_szProductId, length);
	return iRet;
}

INT32S SetServerIpAndPort(const char *v_szFlashPath,char *v_szServerIp,INT32U length,INT32U v_iPort)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szServerIp)
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	int iRet=-1;
	tagMasterServerCfg objSetMasterInfo = {0};
	memset(&objSetMasterInfo,0,sizeof(tagMasterServerCfg));
	strncpy(objSetMasterInfo.m_szMasterIP,v_szServerIp,sizeof(objSetMasterInfo.m_szMasterIP));
	//objSetMasterInfo.m_iMasterPort = v_iPort;
	iRet = SetCfgFile(&objSetMasterInfo, offsetof(tagConfigCfg, m_unMasterServerCfg.m_objMasterServerCfg), sizeof(tagMasterServerCfg));
	LOGOUT("set serverIP=%s,port=%d,iRet=%d",v_szServerIp,v_iPort,iRet);
	return iRet;
}

INT32S SetMotionCfg(const char *v_szFlashPath,tagMotionCfg v_stMotionCfg)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath))
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	int iRet=-1;
	iRet = SetCfgFile(&v_stMotionCfg, offsetof(tagConfigCfg, m_unMotionCfg.m_objMotionCfg), sizeof(tagMotionCfg));
	LOGOUT("SetAliyunOssCfg iRet=%d",iRet);
	return iRet;
}

INT32S SetAliyunOssCfg(const char *v_szFlashPath,tagAliyunOssCfg v_stAliyunOssCfg)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath))
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	int iRet=-1;
	iRet = SetCfgFile(&v_stAliyunOssCfg, offsetof(tagConfigCfg, m_unAliyunOssCfg.m_objAliyunOssCfg), sizeof(tagAliyunOssCfg));
	LOGOUT("SetAliyunOssCfg iRet=%d",iRet);
	return iRet;
}
//tagVersionCfg
/*INT32S SetVersionCfg(const char *v_szFlashPath,tagVersionCfg v_stVersionCfg)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath))
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	int iRet=-1;
	iRet = SetCfgFile(&v_stVersionCfg, offsetof(tagConfigCfg, m_unVersionCfg.m_objVersionCfg), sizeof(tagVersionCfg));
	LOGOUT("SetAliyunOssCfg iRet=%d",iRet);
	return iRet;
}*/
//
/*INT32S GetProductId(const char *v_szFlashPath,char *v_szVersion,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==v_szVersion)
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	char szPath[256] = {0};
	memset(szPath, 0, sizeof(szPath));
	memset(v_szVersion, 0, length);
	sprintf(szPath, "%s/%s",v_szFlashPath, PRODUCTFILENAME);
	INT32S iRet=ReadConfigFile(szPath, v_szVersion, length);
	return iRet;
}
*/
//

INT32S SetCapParamCfg(const char *v_szFlashPath,tagCapParamCfg v_stCapParamCfg)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath))
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	int iRet=-1;
	iRet = SetCfgFile(&v_stCapParamCfg, offsetof(tagConfigCfg, m_unCapParamCfg.m_objCapParamCfg), sizeof(tagCapParamCfg));
	
	LOGOUT("SetAliyunOssCfg iRet=%d",iRet);
	return iRet;
}


INT32S SetSecret(const char *v_szFlashPath,char *v_szSecret,INT32U length)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szSecret) || NULL==v_szSecret)
	{
		LOGOUT("v_szFlashPath or v_szProductId is NULL or strlen = 0");
		return -1;
	}
	int iRet=-1;

	tagDevInfoCfg objDevInfoCfg;
	memset(&objDevInfoCfg,0,sizeof(objDevInfoCfg));
	strncpy(objDevInfoCfg.m_szPassword,v_szSecret,sizeof(objDevInfoCfg.m_szPassword));
	iRet = SetCfgFile(&objDevInfoCfg, offsetof(tagConfigCfg, m_unDevInfoCfg.m_objDevInfoCfg), sizeof(tagDevInfoCfg));
	LOGOUT("set secret=%s,iRet=%d",v_szSecret,iRet);
	return iRet;	
}

INT32S InitDeviceConfig(const char *v_szFlashPath,tagConfigCfg *g_pstConfigCfg)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==g_pstConfigCfg)
	{
		LOGOUT("v_szFlashPath or g_pstConfigCfg is NULL or strlen = 0");
		return -1;
	}
	if(0!=isDeviceAccess(v_szFlashPath))
	{
		mkdir(v_szFlashPath,0777);
		LOGOUT("mkidr %s",v_szFlashPath);
	}
	BOOL bRet=TRUE;
	INT32S iRet=0;
	memset(g_pstConfigCfg,0,sizeof(tagConfigCfg));
	bRet=InitCfgMng(v_szFlashPath,g_pstConfigCfg);
	if(FALSE==bRet)
	{
		LOGOUT("InitCfgMng is failure");
		iRet=1<<1;
	}
	return -iRet;
}

INT32S ResetDeviceConfig(const char *v_szFlashPath)
{
	return ResetCfgFile(v_szFlashPath);
}

void ReleaseDeviceConfig()
{
	ReleaseCfgFile();	
}





