#ifndef __CONFIGMNG_H_
#define __CONFIGMNG_H_

#include "Typedef.h"
#include "Configdef.h"
#include "../LogOut/LogOut.h"

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	BOOL InitCfgMng(char *v_szFlashPath);
	void ReleaseCfgFile();
	BOOL InitConfigFile();
	int  SetCfgFile(void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize);
	int	 GetCfgFile( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize );
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif