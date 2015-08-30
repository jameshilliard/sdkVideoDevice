#include "DeviceConfig.h"


tagConfigCfg g_stConfigCfg;
char 		 g_szServerNO[80] = {0};
char 		 g_szProductId[80] = {0};


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

INT32S InitDeviceConfig(const char *v_szFlashPath,tagConfigCfg *g_pstConfigCfg)
{
	if(NULL==v_szFlashPath || 0==strlen(v_szFlashPath) || NULL==g_pstConfigCfg)
	{
		LOGOUT("v_szFlashPath or g_pstConfigCfg is NULL or strlen = 0");
		return -1;
	}
	if(0==isDeviceAccess(v_szFlashPath))
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

void ReleaseDeviceConfig()
{
	ReleaseCfgFile();	
}





