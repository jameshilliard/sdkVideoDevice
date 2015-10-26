#include "LogOut.h"

FILE *	g_pSzyFpHandle[MAX_LOGFILE];
INT32S 	g_iMaxFileSize[MAX_LOGFILE] = {0};
INT32S 	g_iNowFileSize[MAX_LOGFILE] = {0};
BOOL 	g_bOutToSerial = FALSE;
// �����־�ļ���·��
SCHAR 	g_szFlushLogPath[256] = {0};
// ��־�������ʱ·��
SCHAR  	g_szLogTempPath[256] = {0};
pthread_mutex_t g_SzyLogFileMutex[MAX_LOGFILE];	// ������

// ��ȡ�ļ���С
static DWORD get_FileSize(LPCTSTR path)    
{    
	DWORD filesize = -1;        
	struct stat statbuff;    
	if(stat(path, &statbuff) < 0){    
		return filesize;    
	}else{    
		filesize = statbuff.st_size;    
	}    
	return filesize;    
}    
/************************************************************************
**������Init_LogOut
**���ܣ���ʼ����־
**������
        [in] - v_iLogSize������־�Ĵ�С
			   v_szFlushPath�������־��·��
			   v_bOutToSerial:�Ƿ����������
**���أ�
        
**���ߣ��򺣲�, 2014-04-30
**��ע��
       1). 
************************************************************************/
void Init_LogOut(INT32U v_iLogSize,LPCTSTR v_szFlushPath, BOOL v_bOutToSerial,LPCTSTR v_szTempPath)
{
#ifdef LOGOUTDEBUG
	return ;
#endif
	if((v_szFlushPath == NULL) /*|| (0 == access(v_szFlushPath, F_OK)) */
		|| (v_iLogSize < 100*1024) || (NULL == v_szTempPath))
	{
		printf("%s file or v_iLogSize %d, is error\n",v_szFlushPath, v_iLogSize);
		return ;
	}
	SCHAR szPath[256] = {0};
	SCHAR szMkdirPath[256] = {0};
	INT32S i = 0;
	for (i = 0; i< MAX_LOGFILE; i++)
	{
		g_iMaxFileSize[i] = 0;
		g_iNowFileSize[i] = 0;
	}
	g_bOutToSerial = FALSE;
	sprintf(szPath, "%s/SDK_LOG/", v_szFlushPath);
	if(0 == access(szPath, F_OK))
	{
		SCHAR szCmd[256] = {0};
		sprintf(szCmd, "rm -fr %s/SDK_LOG/", v_szFlushPath);
		system(szCmd);
	}
	strcpy(g_szFlushLogPath, v_szFlushPath);
	sprintf(szMkdirPath,"mkdir -p %s/%s",g_szFlushLogPath, LOGFILEDIR);
	system(szMkdirPath);
	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "%s/%s", g_szFlushLogPath, LOGFILEDIR);
	if(0 != access(szPath, F_OK))
	{
		printf("mkdir -p %s/%s\n", g_szFlushLogPath, LOGFILEDIR); // ����Ŀ¼ʧ��
		return ;
	}
	for (i = 0; i<MAX_LOGFILE; i++)
	{
		memset(szPath, 0 ,sizeof(szPath));
		g_pSzyFpHandle[i] = NULL;
		sprintf(szPath, "%s/%s/SDKLOG%d_00.log", g_szFlushLogPath, LOGFILEDIR, i);
		g_pSzyFpHandle[i] = fopen(szPath, "a+");
		if (!g_pSzyFpHandle[i])
		{
			printf("log file: %s open fail\n",szPath);
		}
		g_iNowFileSize[i] = get_FileSize(szPath);
		if(g_iNowFileSize[i] == -1)
		{
			g_iNowFileSize[i] = 0;
		}
		switch(i)
		{
		case LOG_LV1:
			{
				if(MAX_LOGFILE > 1)
				{
					g_iMaxFileSize[i] = (v_iLogSize/10*8)/4;
				}
				else
				{
					g_iMaxFileSize[i] = v_iLogSize/4;
				}
			}
			break;
		case LOG_LV2:
			g_iMaxFileSize[i] = (v_iLogSize/10*2)/4;
			break;
		default:
			break;
		}

		pthread_mutex_init(&g_SzyLogFileMutex[i], NULL);
	}
	g_bOutToSerial = v_bOutToSerial;
	strcpy(g_szLogTempPath, v_szTempPath);
}


// �����ļ�
void BackupFile(INT32U v_iLv)
{
	SCHAR szNewPath[256] = {0};
	SCHAR szOldPath[256] = {0};
	INT32S i = 0;
	for(i = MAXBACKNUM - 1; i>=0; i--)
	{
		sprintf(szNewPath, "%s/%s/SDKLOG%d_%02d.log", g_szFlushLogPath, LOGFILEDIR, v_iLv, i+1);
		sprintf(szOldPath, "%s/%s/SDKLOG%d_%02d.log", g_szFlushLogPath, LOGFILEDIR, v_iLv, i);
		rename(szOldPath, szNewPath);
	}
}

// д��־���ļ���
void WriteLog(INT32U v_iLv,LPCTSTR v_szWriteBuf)
{
	if(NULL == v_szWriteBuf)
	{
		return;
	}
	fwrite(v_szWriteBuf, 1, strlen(v_szWriteBuf), g_pSzyFpHandle[v_iLv]);
	fflush(g_pSzyFpHandle[v_iLv]);
	g_iNowFileSize[v_iLv] += strlen(v_szWriteBuf);
	if(g_iNowFileSize[v_iLv] > g_iMaxFileSize[v_iLv])
	{
		fclose(g_pSzyFpHandle[v_iLv]);
		BackupFile(v_iLv);
		SCHAR szNewPath[256] = {0};
		sprintf(szNewPath, "%s/%s/SDKLOG%d_00.log", g_szFlushLogPath, LOGFILEDIR, v_iLv);
		g_pSzyFpHandle[v_iLv] = fopen(szNewPath, "w+");
		g_iNowFileSize[v_iLv] = 0;
	}
}

// д��־ģ��
void LogOutPut(INT32U v_iLv,LPCTSTR v_szFileName,INT32U v_iFileLine,LPCTSTR v_szMsg, ...)
{
	if(v_iLv > (MAX_LOGFILE - 1))
	{
		return ;
	}
	if(g_pSzyFpHandle[v_iLv] == NULL)
	{
		return ;
	}
	pthread_mutex_lock( &g_SzyLogFileMutex[v_iLv]);
	struct tm nowTime;
	time_t   now;  
	struct timeval mTimeVal;
	time(&now);
	gettimeofday(&mTimeVal,NULL);
	localtime_r(&now, &nowTime);
	va_list argList;
	SCHAR szLogBuf[1024] = {0};
	SCHAR szOutBuf[1024] = {0};
	INT32S size = sizeof(szOutBuf);
	INT32S n = 0;
	LPCTSTR pFile = NULL;
	if((pFile = strrchr(v_szFileName,'/')) != NULL )
	{
		pFile+=1;
	}
	else
	{
		pFile = v_szFileName;
	}
	sprintf(szLogBuf, "%04d-%02d-%02d %02d:%02d:%02d:%03d [%15s at %4d] %s  \n", 
		nowTime.tm_year + 1900, nowTime.tm_mon+1, nowTime.tm_mday, nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec,mTimeVal.tv_usec/1000,pFile, v_iFileLine, v_szMsg);
	va_start(argList, szLogBuf);
	n = vsnprintf(szOutBuf, size, szLogBuf, argList);
	//vsprintf(szOutBuf, szLogBuf, argList);
	va_end(argList);
	if ((n > -1) && (n < size))	
	{
		if (g_bOutToSerial)
		{//���������
			printf(szOutBuf);
		}
		else
		{//�������־�ļ�
			WriteLog(v_iLv, szOutBuf);
		}
	}
	pthread_mutex_unlock( &g_SzyLogFileMutex[v_iLv]);
}

/************************************************************************
**������TarLogFile
**���ܣ�ѹ����־�ļ�
**������
        [out] - v_szOutPath��ѹ����ɺ����־��·��
**���أ�
        
**���ߣ��򺣲�, 2014-05-04
**��ע��
       1). 
************************************************************************/
void TarLogFile(LPTSTR v_szOutPath)
{
	if(v_szOutPath == NULL)
	{
		return ;
	}
	//��ѹ������log�ļ�
	SCHAR szFilePath[256] = {0};
	INT32S i = 0;
	sprintf(szFilePath, "%s/%s/",g_szFlushLogPath, LOGFILEDIR);
	SCHAR szCmd[256] = {0};
	SCHAR szTemp[256] = {0};
	// ������־�ļ���
	sprintf(szTemp, "%s/syslog", g_szLogTempPath);
	if(0 != access(szTemp, F_OK))
	{
		sprintf(szCmd, "mkdir -p %s", szTemp);
		system(szCmd);
	}
	memset(szCmd, 0, sizeof(szCmd));
	sprintf(szCmd, "cp %s/* %s/syslog/", szFilePath, g_szLogTempPath);
	for (i = 0; i< MAX_LOGFILE; i++)
	{
		pthread_mutex_lock( &g_SzyLogFileMutex[i]);
	}
	system(szCmd);
	for (i = 0; i< MAX_LOGFILE; i++)
	{
		pthread_mutex_unlock( &g_SzyLogFileMutex[i]);
	}
	memset(szCmd, 0, sizeof(szCmd));
	sprintf(szCmd, 	"tar -cf %s/syslog.tar %s/syslog", g_szLogTempPath, g_szLogTempPath);
	system(szCmd);
	memset(szCmd, 0, sizeof(szCmd));
	sprintf(szCmd,	"gzip -c %s/syslog.tar > %s/syslog.tar.gz", g_szLogTempPath, g_szLogTempPath);
	system(szCmd);
	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "%s/syslog.tar.gz", g_szLogTempPath);
	if( access(szTemp,0)!=0 )
	{
		LOGOUT("Create %s/syslog.tar.gz fail", g_szLogTempPath);
		return ;
	}
	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "%s/syslog", g_szLogTempPath);
	if(0 == access(szTemp, F_OK))
	{
		memset(szCmd, 0, sizeof(szCmd));
		sprintf(szCmd,"rm -rf  %s/syslog", g_szLogTempPath);
		system(szCmd);
	}
	memset(szCmd, 0, sizeof(szCmd));
	sprintf(szCmd,"rm -rf  %s/syslog.tar", g_szLogTempPath);
	system(szCmd);
	sprintf(v_szOutPath, "%s/syslog.tar.gz", g_szLogTempPath);
	return ;
}

// �ͷ���־ģ��
void Release_LogOut()
{
	INT32S i = 0;
	for (i = 0; i<MAX_LOGFILE; i++)
	{
		if (g_pSzyFpHandle[i])
		{
			fclose(g_pSzyFpHandle[i]);
			g_pSzyFpHandle[i] = NULL;
		}
		pthread_mutex_destroy( &g_SzyLogFileMutex[i]);
	}
}
