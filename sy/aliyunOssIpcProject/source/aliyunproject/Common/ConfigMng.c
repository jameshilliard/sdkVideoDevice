#include "ConfigMng.h"


pthread_mutex_t m_CfgMutex;
char g_szCfgFilePath[256] = {0};


/************************************************************************
**������InitConfigFile
**���ܣ���ʼ�������ļ�
**������
        [in] - v_szFlashPath�����������Ϣ�ļ���λ��
        [out] - 
**���أ�FALSE:ʧ�ܣ�TRUE:�ɹ�
        
**���ߣ��򺣲�, 2014-04-04
**��ע��
       1). 
************************************************************************/
BOOL InitCfgMng(char *v_szFlashPath)
{
	BOOL bRet = TRUE;
	if(NULL == v_szFlashPath)
	{
		LOGOUT("v_szFlashPath is NULL!");
		bRet = FALSE;
		goto _INIT_END;
	}

	pthread_mutex_init(&m_CfgMutex, NULL);
	
	FILE* _RdFile = NULL;
	tagConfigCfg objConfigCfg;

	memset(g_szCfgFilePath, 0, sizeof(g_szCfgFilePath));
	sprintf(g_szCfgFilePath, "%s/%s", v_szFlashPath, CONFIGFILENAME);
	_RdFile = fopen(g_szCfgFilePath, "rb+");
	if(!_RdFile)
	{
		if(!InitConfigFile(v_szFlashPath))
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
	
	fread((char*)&objConfigCfg, 1, sizeof(tagConfigCfg), _RdFile);
	if(strncmp(CFGCHECKMSG, objConfigCfg.m_szSpecCode, MIN(sizeof(CFGCHECKMSG) - 1 , sizeof(objConfigCfg.m_szSpecCode) - 1)))
	{
		if(!InitConfigFile(v_szFlashPath))
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
**������InitConfigFile
**���ܣ��ָ�Ĭ�ϲ���
**������
        [in] - v_szFlashPath:���������ļ�·��
        [out] - null
**���أ�FALSE:ʧ�ܣ�TRUE:�ɹ�
        
**���ߣ��򺣲�, 2014-04-04
**��ע��
       1). 
************************************************************************/
BOOL InitConfigFile()
{
	int i = 0;
	FILE* _RdFile = NULL;

	_RdFile = fopen(g_szCfgFilePath, "wb+");
	if (!_RdFile)
	{
		LOGOUT("can not cerate config file for write");
		return FALSE;
	}

	tagConfigCfg objConfigCfg;
	memset(&objConfigCfg, 0, sizeof(objConfigCfg));
	strcpy(objConfigCfg.m_szSpecCode, CFGCHECKMSG);

	// tagMasterServerCfg���ز���Ĭ��ֵ
	strcpy(objConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP, ROUTESERVER);
	objConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort = ROUTESERVERPORT;

	// tagCapParamCfg����ͷ��ϢĬ�ϲ���
	for (i = 0; i< MAXCAMERANUM; i++)
	{
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wBitRate = DE_BITRATE;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wFrameRate = DE_FRAMERATE;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wHeight = DE_HEIGHT;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wWidth = DE_WIDTH;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wQpConstant = DE_QPCONSTANT;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_wKeyFrameRate = DE_KEYFRAMERATE;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_bOpenAudio = DE_OPENAUDIO;
		objConfigCfg.m_unCapParamCfg.m_objCapParamCfg[i].m_bAlarmSwitch = DE_ALARMSWITCH;
	}

	fwrite((char*)&objConfigCfg, sizeof(objConfigCfg), 1, _RdFile);
	fflush(_RdFile);
	fclose(_RdFile);

	return TRUE;
}
/************************************************************************
**������SetCfgFile
**���ܣ����������ļ�
**������
        [in] - v_pSrc:�������ݵ����ݽṹ��ָ���ַ
			   v_uiSaveBegin�������������tagConfigCfg�ṹ���е�ƫ��λ��
			   v_uiSaveSize����������ݵĽṹ���С
**���أ�0���ɹ���������ʧ��
        
**���ߣ��򺣲�, 2014-04-08
**��ע��
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
		if (!InitConfigFile())
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
**������GetCfgFile
**���ܣ���ȡ�����ļ��е�����
**������
        [in] - v_pDes����ȡ�����ݵ����ݽṹָ��
			   v_uiDesSize����ȡ�����ݵ����ݽṹ��С
			   v_uiGetBegin����ȡ��������tagConfigCfg�ṹ���е�ƫ��λ��
			   v_uiGetSize����ȡ�����ݵĽṹ���С
**���أ�0���ɹ���������ʧ��
        
**���ߣ��򺣲�, 2014-04-08
**��ע��
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
		if (!InitConfigFile())
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



// �ͷŲ���������ز���
void ReleaseCfgFile()
{
	pthread_mutex_destroy( &m_CfgMutex );
}	