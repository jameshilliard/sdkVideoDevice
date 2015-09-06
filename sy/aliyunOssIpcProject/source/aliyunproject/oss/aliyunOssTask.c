#include "aliyunOssTask.h"
#include "../hisdk/hi_sdk.h"
#include "../LogOut/LogOut.h"


int getFilePath(char *videoPath,char *jpgPath,char *ipcId,char *fullName, char *fileName)
{
	if(fullName==NULL || fileName==NULL)
		return -1;
	time_t m_tNowSeconds = time (NULL);
	struct tm *ptm = localtime(&m_tNowSeconds);
	char *ptr=NULL;
	ptr=strstr(fileName,".mp4");
	if(ptr!=NULL)
	{
		sprintf(fullName, "%s/%s/%04d%02d%02d/%s",videoPath,(strlen(ipcId)==0)?"test":ipcId,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,fileName);	
		return 0;
	}
	else
	{
		ptr=strstr(fileName,".jpg");
		if(ptr!=NULL)
		{
			sprintf(fullName, "%s/%s/%04d%02d%02d/%s",jpgPath,(strlen(ipcId)==0)?"test":ipcId,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,fileName);
			return 0;
		}
			
	}
	printf("file is no mp4 or jpg");
	return -2;
}

//Videos/Ipcid/yymmdd/yyyymmddhhmmss.mp4
void * aliyunOssTask(void* param)
{
	char fileName[MAX_PATH];
	char filePath[MAX_PATH];
	char aliyunFilePath[MAX_PATH];
	DWORD fileSize=0;
	RecordData mRecordData;
	int iRet=-1;
	while(1)
	{
		memset(fileName,0,sizeof(fileName));
		memset(filePath,0,sizeof(filePath));
		memset(aliyunFilePath,0,sizeof(aliyunFilePath));
		iRet=readMediaFile(SYSTEM_MEDIA_SENDFILEPATH,fileName);
		if(iRet==1 || iRet==2)
		{
			iRet=getFilePath(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szVideoPath,
							 g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szJPGPath,
							 g_szServerNO,aliyunFilePath,fileName);
			if(iRet==0)
			{
				sprintf(filePath,"%s/%s",SYSTEM_MEDIA_SENDFILEPATH,fileName);
				if(strlen(filePath)!=0)
				{
					iRet=upLoadFile(filePath,aliyunFilePath);
					if(iRet==0)
					{
						unlink(filePath);
						LOGOUT("upLoadFile %s success and unlink",filePath);
					}
					else if(iRet==409)//file was exist
					{
						unlink(filePath);
						LOGOUT("filePath %s was exist and unlink",filePath);						
					}
						
				}
			}
			sleep(1);
		}
		else if(iRet==3)
		{
			memset(&mRecordData,0,sizeof(mRecordData));
			iRet=readFile(fileName,(LPCTSTR)(&mRecordData),sizeof(mRecordData),&fileSize);
			if(iRet==0)
			{
			   iRet=dataRecord(mRecordData.m_server,mRecordData.m_id,mRecordData.m_videoPath,
			   				   mRecordData.m_creatTimeInMilSecond,mRecordData.m_videoFileSize,
			   				   mRecordData.m_jpgPath,mRecordData.m_videoTimeLength,mRecordData.m_mMotionData);
			   if(iRet==1)
			   {
					unlink(filePath);
					LOGOUT("upLoadFile %s success and unlink",filePath);
			   }
			}
			else
			{
				unlink(filePath);
				LOGOUT("filePath %s was error and unlink",filePath);	
			}
		}
		else
		{

			sleep(2);
		}
	}
}

int initAliyunOssTask()
{
	int iRet;
	pthread_t m_aliyunOssTask;//ʵʱ���ţ����̿����߳�
	iRet = pthread_create(&m_aliyunOssTask, NULL, aliyunOssTask, NULL);
	if(iRet != 0)
	{
		LOGOUT("can't create thread: %s",strerror(iRet));
		return -1;
	}
	return 0;	
}

int ReleaseAliyunOssTask()
{

	return 0;
}
