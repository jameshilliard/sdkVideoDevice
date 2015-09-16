#include "aliyunOssTask.h"
#include "../hisdk/hi_sdk.h"
#include "../LogOut/LogOut.h"


int getFilePath(char *videoPath,char *jpgPath,char *ipcId,char *fullName, char *fileName)
{
	if(fullName==NULL || fileName==NULL)
		return -1;
	char *ptr=NULL;
	char day[10]={0};
	memset(day,0,sizeof(day));
	memcpy(day,fileName,8);
	ptr=strstr(fileName,".mp4");
	if(ptr!=NULL)
	{
		sprintf(fullName, "%s/%s/%s/%s",videoPath,(strlen(ipcId)==0)?"test":ipcId,day,fileName);	
		return 0;
	}
	else
	{
		ptr=strstr(fileName,".jpg");
		if(ptr!=NULL)
		{
			sprintf(fullName, "%s/%s/%s/%s",jpgPath,(strlen(ipcId)==0)?"test":ipcId,day,fileName);
			return 0;
		}
			
	}
	printf("file %s is no mp4 or jpg",fileName);
	return -2;
}

//Videos/Ipcid/yymmdd/yyyymmddhhmmss.mp4
void * aliyunOssTask(void* param)
{
	char fileName[MAX_PATH];
	char fileDataName[MAX_PATH];
	char filePath[MAX_PATH];
	char aliyunFilePath[MAX_PATH];
	DWORD fileSize=0;
	RecordData mRecordData;
	int iRet=-1;
	int fileType=-1;
	LOGINRETURNINFO returnInfo;
	loginCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
		      g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,
		      &returnInfo);
	time_t oldTime;
	time_t newTime;
	time((time_t *)&oldTime);
	newTime=oldTime;
	while(1)
	{
		time((time_t *)&newTime);
		if((newTime-oldTime)>12*60*60 || (!(g_iServerStatus==1 || g_iServerStatus==4)))
		{
			iRet=loginCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
					  g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,
					  &returnInfo);
			if(iRet==0)
			{
				g_iServerStatus=returnInfo.result;
			}
			else
			{
				g_iServerStatus=0;
			}
			oldTime=newTime;
			if(!(g_iServerStatus==1 || g_iServerStatus==4))
				sleep(28);
		}
		memset(fileName,0,sizeof(fileName));
		memset(filePath,0,sizeof(filePath));
		memset(aliyunFilePath,0,sizeof(aliyunFilePath));
		iRet=readMediaFile(SYSTEM_MEDIA_SENDFILEPATH,fileName);
		if(iRet==1 || iRet==2)
		{
			fileType=iRet;
			iRet=getFilePath(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szVideoPath,
							 g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szJPGPath,
							 g_szServerNO,aliyunFilePath,fileName);
			if(iRet==0)
			{
				sprintf(filePath,"%s/%s",SYSTEM_MEDIA_SENDFILEPATH,fileName);
				if(strlen(filePath)!=0)
				{
					DWORD fileSize=getFileSize(filePath);
					if(fileSize<=0)
					{
						unlink(filePath);
						LOGOUT("upLoadFile %s %s is null and unlink",filePath,aliyunFilePath);
						continue;
					}
					iRet=upLoadFile(filePath,aliyunFilePath);
					if(iRet==0)
					{
						unlink(filePath);
						LOGOUT("upLoadFile %s %s success and unlink",filePath,aliyunFilePath);
					}
					else if(iRet==409)//file was exist
					{
						unlink(filePath);
						LOGOUT("filePath %s %s was exist and unlink",filePath,aliyunFilePath);						
					}
					if(fileType==2)
					{
						memset(fileDataName,0,sizeof(fileDataName));
						sscanf(fileName,"%[^.]",fileDataName);
						sprintf(fileDataName,"%s.dat",fileDataName);
						LOGOUT("send %s",fileDataName);
						sprintf(filePath,"%s/%s",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
						memset(&mRecordData,0,sizeof(mRecordData));
						iRet=readFile(filePath,(LPCTSTR)(&mRecordData),sizeof(mRecordData),&fileSize);
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
							LOGOUT("filePath %s was error and unlink iRet=%d",filePath,iRet);	
						}

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
	exitCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
			 g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword);//退出
	
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
