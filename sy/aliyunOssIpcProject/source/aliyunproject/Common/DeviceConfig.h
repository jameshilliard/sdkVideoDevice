#ifndef _SY_DEVICECONFIG_H_
#define _SY_DEVICECONFIG_H_

#include "Typedef.h"
#include "Configdef.h"
#include "GlobFunc.h"
#include "NetFunc.h"
#include "../mxml/mxmlparser.h"


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	INT32S SetServerNo(const char *v_szFlashPath,char *v_szServerNo,INT32U length);
	INT32S GetServerNo(const char *v_szFlashPath,char *v_szServerNo,INT32U length);
	INT32S GetProductId(const char *v_szFlashPath,char *v_szProductId,INT32U length);
	INT32S SetProductId(const char *v_szFlashPath,char *v_szProductId,INT32U length);
	INT32S SetServerIpAndPort(const char *v_szFlashPath,char *v_szServerIp,INT32U length,INT32U v_iPort);
	INT32S SetSecret(const char *v_szFlashPath,char *v_szSecret,INT32U length);
	INT32S SetAliyunOssCfg(const char *v_szFlashPath,tagAliyunOssCfg v_stAliyunOssCfg);
	INT32S SetMotionCfg(const char *v_szFlashPath,tagMotionCfg v_stMotionCfg);
	
	INT32S SetCapParamCfg(const char *v_szFlashPath,tagCapParamCfg v_stCapParamCfg);
	

	INT32S InitDeviceConfig(const char *v_szFlashPath,tagConfigCfg *g_pstConfigCfg);
	void ReleaseDeviceConfig();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif

