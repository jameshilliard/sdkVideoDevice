#include "ConfigMng.h"


pthread_mutex_t m_CfgMutex;
char g_szCfgFilePath[256] = {0};


/************************************************************************
**函数：InitConfigFile
**功能：初始化配置文件
**参数：
        [in] - v_szFlashPath：存放配置信息文件的位置
        [out] - 
**返回：FALSE:失败，TRUE:成功
**备注：
       1). 
************************************************************************/
BOOL InitCfgMng(const char *v_szFlashPath,tagConfigCfg *v_pstConfigCfg)
{
	BOOL bRet = TRUE;
	if(NULL == v_szFlashPath || NULL==v_pstConfigCfg || 0==strlen(v_szFlashPath))
	{
		LOGOUT("v_szFlashPath or v_pstConfigCfg is NULL!");
		bRet = FALSE;
		goto _INIT_END;
	}

	pthread_mutex_init(&m_CfgMutex, NULL);
	
	FILE* _RdFile = NULL;

	memset(g_szCfgFilePath, 0, sizeof(g_szCfgFilePath));
	sprintf(g_szCfgFilePath, "%s/%s", v_szFlashPath, CONFIGFILENAME);
	_RdFile = fopen(g_szCfgFilePath, "rb+");
	if(!_RdFile)
	{
		if(!InitConfigFile(v_pstConfigCfg))
		{
			bRet = FALSE;
			goto _INIT_END;
		}
		else
		{
			_RdFile = fopen(g_szCfgFilePath, "rb+");
			if (!_RdFile)
			{
				bRet = FALSE;
				goto _INIT_END;
			}
		}
	}
	
	fread((char*)v_pstConfigCfg, 1, sizeof(tagConfigCfg), _RdFile);
	//printf("---%s-----\n",v_pstConfigCfg->m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP);
	if(strncmp(CFGCHECKMSG, v_pstConfigCfg->m_szSpecCode, MIN(sizeof(CFGCHECKMSG) - 1 , sizeof(v_pstConfigCfg->m_szSpecCode) - 1)))
	{
		if(!InitConfigFile(v_pstConfigCfg))
		{
			bRet = FALSE;
			goto _INIT_END;
		}
	}

_INIT_END:

	if(_RdFile != NULL)
	{
		fclose(_RdFile);
	}

	return bRet;
}

/************************************************************************
**函数：InitConfigFile
**功能：恢复默认参数
**参数：
        [in] - v_szFlashPath:参数配置文件路径
        [out] - null
**返回：FALSE:失败，TRUE:成功
**备注：
       1). 
************************************************************************/
BOOL InitConfigFile(tagConfigCfg *v_pstConfigCfg)
{
	int i = 0;
	FILE* _RdFile = NULL;
	_RdFile = fopen(g_szCfgFilePath, "wb+");
	if (!_RdFile)
	{
		LOGOUT("can not cerate %s config file for write",g_szCfgFilePath);
		return FALSE;
	}

	tagConfigCfg objConfigCfg;
	memset(&objConfigCfg, 0, sizeof(objConfigCfg));
	strncpy(objConfigCfg.m_szSpecCode, CFGCHECKMSG,sizeof(objConfigCfg.m_szSpecCode));

	// tagMasterServerCfg主控参数默认值
	strncpy(objConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP, DE_ROUTESERVER,sizeof(objConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP));
	//objConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort = DE_ROUTESERVERPORT;

	// tagCapParamCfg摄像头信息默认参数
	for (i = 0; i< MAXCAMERANUM; i++)
	{
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wBitRate = DE_BITRATE;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wFrameRate = DE_FRAMERATE;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wHeight = DE_HEIGHT;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wWidth = DE_WIDTH;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wQpConstant = DE_QPCONSTANT;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wKeyFrameRate = DE_KEYFRAMERATE;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_CodeType= DE_CODETYPE;
	}

	//aliyun 配置
	strncpy(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szBuctetName, DE_ALBUCKETNAME,sizeof(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szBuctetName));
	strncpy(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szOssEndPoint, DE_ALENDPOINT,sizeof(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szOssEndPoint));
	strncpy(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeyId, DE_ALACCESSKEYID,sizeof(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeyId));
	strncpy(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeySecret, DE_ALACCESSSECRET,sizeof(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeySecret));
	strncpy(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szJPGPath, DE_JPGPATH,sizeof(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szJPGPath));
	strncpy(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szVideoPath, DE_VIDEOPATH,sizeof(objConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szVideoPath));
	
	objConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iBefRecLastTime = DE_BEFORE_RECORD_MOTION_LASTTIME;
	objConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iBefRecTimes = DE_BEFORE_RECORD_MOTION_TIMES;
	objConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iConRecLastTime = DE_CONTINUES_RECORD_MOTION_LASTTIME;
	objConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iConRecTimes = DE_CONTINUES_RECORD_MOTION_LASTTIME;
	objConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iEndRecTime = DE_END_RECORD_MOTION_TIME;
	objConfigCfg.m_unMotionCfg.m_objMotionCfg.m_bEnable = DE_ENABLE;

	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iStartPeriod=DE_START_PREIOD;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iStartSumDetect=DE_START_SUM_DETECT;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iStartSumArea=DE_START_SUM_AREA;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iStartSoundSize=DE_START_SOUND_SIZE;
	
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iOverPeriod=DE_OVER_PREIOD;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iOverSumDetect=DE_OVER_SUM_DETECT;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iOverSumArea=DE_OVER_SUM_AREA;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iOverSoundSize=DE_OVER_SOUND_SIZE;
	
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iEndRecTime=DE_URGENCY_REC_TIME;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iCommServerTime=DE_COMMSERVERTIME;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_bEnable=DE_URGENCY_ENABLE;
	objConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_bInvalid=DE_ENABLE;
	
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bStartUpEnable=DE_ENABLE;
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bServerSuccessEnable=DE_DISABLE;
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bServerFailureEnable=DE_DISABLE;
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bUpgradeEnable=DE_DISABLE;
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bLoginInEnable=DE_DISABLE;
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bUrgencyStartEnable=DE_DISABLE;
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bUrgencyOverEnable=DE_DISABLE;
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bEnable=DE_SOUND_ENABLE;
	objConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bInvalid=DE_ENABLE;

	strncpy(objConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,DE_SECRET,sizeof(objConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword));
	
	fwrite((char*)&objConfigCfg, sizeof(objConfigCfg), 1, _RdFile);
	if(NULL!=v_pstConfigCfg)
	{
		memcpy((char*)v_pstConfigCfg,(char*)&objConfigCfg, sizeof(objConfigCfg));
	}
	fflush(_RdFile);
	fclose(_RdFile);

	return TRUE;
}
/************************************************************************
**函数：SetCfgFile
**功能：保存配置文件
**参数：
        [in] - v_pSrc:保存数据的数据结构体指针地址
			   v_uiSaveBegin：保存的数据在tagConfigCfg结构体中的偏移位置
			   v_uiSaveSize：保存的数据的结构体大小
**返回：0：成功，其他：失败
**备注：
       1). 
************************************************************************/
int SetCfgFile(void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize)
{
	int iRet = 0;
	if( !v_pSrc ) 
	{
		LOGOUT("Param v_pSrc is null");
		return ERR_PAR;
	}
	if( v_uiSaveBegin + v_uiSaveSize > sizeof(tagConfigCfg) )
	{
		return ERR_PAR;
	}
	
	pthread_mutex_lock( &m_CfgMutex );
	FILE* _RdFile = NULL;
	_RdFile = fopen(g_szCfgFilePath, "rb+");
	if (!_RdFile)
	{
		if (!InitConfigFile(NULL))
		{
			iRet = ERR_CFG;
			goto _SETCFGEND;
		}
		_RdFile = fopen(g_szCfgFilePath, "rb+");
		if (!_RdFile)
		{
			iRet = ERR_PAR;
			goto _SETCFGEND;
		}
	}


	if (fseek(_RdFile, v_uiSaveBegin, SEEK_SET) != 0)
	{
		return ERR_PAR;
	}

	fwrite((char*)v_pSrc, v_uiSaveSize, 1, _RdFile);
	fflush(_RdFile);
	
_SETCFGEND:
	if(_RdFile)
	{
		fclose(_RdFile);
	}
	pthread_mutex_unlock( &m_CfgMutex );
	return iRet;
}

/************************************************************************
**函数：GetCfgFile
**功能：获取配置文件中的数据
**参数：
        [in] - v_pDes：获取的数据的数据结构指针
			   v_uiDesSize：获取的数据的数据结构大小
			   v_uiGetBegin：获取的数据在tagConfigCfg结构体中的偏移位置
			   v_uiGetSize：获取的数据的结构体大小
**返回：0：成功，其他：失败
**备注：
       1). 
************************************************************************/
int GetCfgFile( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize )
{
	int iRet = 0;
	if( !v_pDes ) 
	{
		return ERR_PAR;
	}

	if( v_uiDesSize < v_uiGetSize ) 
	{
		return ERR_SPACELACK;
	}
	if( v_uiGetBegin + v_uiGetSize > sizeof(tagConfigCfg) ) 
	{
		return ERR_PAR;
	}

	pthread_mutex_lock( &m_CfgMutex );
	FILE* _RdFile = NULL;
	_RdFile = fopen(g_szCfgFilePath, "rb+");
	if (!_RdFile)
	{
		if (!InitConfigFile(NULL))
		{
			iRet = ERR_CFG;
			goto _GETCFGEND;
		}

		_RdFile = fopen(g_szCfgFilePath, "rb+");
		if (!_RdFile)
		{
			iRet = ERR_PAR;
			goto _GETCFGEND;
		}
	}

	if (fseek(_RdFile, v_uiGetBegin, SEEK_SET) != 0)
	{
		iRet = ERR_PAR;
		goto _GETCFGEND;
	}

	if (v_uiGetSize != fread((char*)v_pDes, 1, v_uiGetSize, _RdFile))
	{
		iRet = ERR_PAR;
		goto _GETCFGEND;
	}

_GETCFGEND:
	if(_RdFile)
	{
		fclose(_RdFile);
	}
	pthread_mutex_unlock( &m_CfgMutex );
	return iRet;
}

int  ResetCfgFile(const char *v_szFlashPath)
{
	printf("remove g_szCfgFilePath %s---\n",g_szCfgFilePath);
	unlink(g_szCfgFilePath);
	return 0;
}

// 释放参数配置相关参数
void ReleaseCfgFile()
{
	pthread_mutex_destroy( &m_CfgMutex );
}	
