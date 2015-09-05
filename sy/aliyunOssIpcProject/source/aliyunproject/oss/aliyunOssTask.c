#include "aliyunOssTask.h"
#include "../hisdk/hi_sdk.h"
#include "../LogOut/LogOut.h"


int getFilePath(char *ipcId,char *fullName, char *fileName)
{
	if(fullName==NULL || fileName==NULL)
		return -1;
	time_t m_tNowSeconds = time (NULL);
	struct tm *ptm = localtime(&m_tNowSeconds);
	char *ptr=NULL;
	ptr=strstr(fileName,".mp4");
	if(ptr!=NULL)
	{
		sprintf(fullName, "Videos/%s/%04d%02d%02d/%s",(strlen(ipcId)==0)?"test":ipcId,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,fileName);	
		return 0;
	}
	else
	{
		ptr=strstr(fileName,".jpg");
		if(ptr!=NULL)
		{
			sprintf(fullName, "Photos/%s/%04d%02d%02d/%s",(strlen(ipcId)==0)?"test":ipcId,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,fileName);
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
	int iRet=-1;
	while(1)
	{
		memset(fileName,0,sizeof(fileName));
		memset(filePath,0,sizeof(filePath));
		memset(aliyunFilePath,0,sizeof(aliyunFilePath));
		iRet=readMediaFile(SYSTEM_MEDIA_SENDFILEPATH,fileName);
		if(iRet==1 || iRet==2)
		{
			iRet=getFilePath(g_szServerNO,aliyunFilePath,fileName);
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
		else
		{

			sleep(2);
		}
	}
}

int initAliyunOssTask()
{
	int iRet;
	pthread_t m_aliyunOssTask;//实时播放，过程控制线程
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
