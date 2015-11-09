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
	char file2Path[MAX_PATH];
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
	char *motionString=NULL;
	motionString=malloc(MAX_MOTION_STRING);
	if(!motionString)
	{
		LOGOUT("malloc g_motionString %d",MAX_MOTION_STRING);
		return -4;
	}
	char *soundString=NULL;
	soundString=malloc(MAX_SOUND_STRING);
	if(!soundString)
	{
		LOGOUT("malloc g_motionString %d",MAX_SOUND_STRING);
		return -5;
	}
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
		if(!(g_iServerStatus==1 || g_iServerStatus==4))
			continue;
		memset(fileName,0,sizeof(fileName));
		memset(filePath,0,sizeof(filePath));
		memset(file2Path,0,sizeof(file2Path));
		memset(motionString,0,MAX_MOTION_STRING);
		memset(soundString,0,MAX_SOUND_STRING);
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
					if(fileType==2)
					{
						iRet=isMp4File(filePath);
						if(iRet!=0)
						{
							unlink(filePath);
							LOGOUT("upLoadFile %s %s is no mp4 file and unlink",filePath,aliyunFilePath);
							memset(fileDataName,0,sizeof(fileDataName));
							sscanf(fileName,"%[^.]",fileDataName);
							sprintf(fileDataName,"%s.mot",fileDataName);
							sprintf(filePath,"%s/%s",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
							unlink(filePath);
							memset(fileDataName,0,sizeof(fileDataName));
							sprintf(fileDataName,"%s.sou",fileDataName);
							sprintf(file2Path,"%s/%s",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
							unlink(file2Path);
							LOGOUT("error mp4 file unlink %s and %s",filePath,file2Path);
							continue;
						}
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
					else
					{
						LOGOUT("upload file %d is error iRet=%d",filePath,iRet);
					}
					if(fileType==2)
					{
						memset(fileDataName,0,sizeof(fileDataName));
						sscanf(fileName,"%[^.]",fileDataName);
						sprintf(fileDataName,"%s.mot",fileDataName);
						LOGOUT("send %s",fileDataName);
						sprintf(filePath,"%s/%s",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
						memset(&mRecordData,0,sizeof(mRecordData));
						memset(fileDataName,0,sizeof(fileDataName));
						sscanf(fileName,"%[^.]",fileDataName);
						sprintf(fileDataName,"%s.sou",fileDataName);
						LOGOUT("send %s",fileDataName);
						sprintf(file2Path,"%s/%s",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
						fileSize=0;
						iRet=readFile(filePath,(LPCTSTR)(motionString),MAX_MOTION_STRING,&fileSize);
						if(iRet==0 && fileSize!=0)
						{
							fileSize=0;
							iRet=readFile(file2Path,(LPCTSTR)(soundString),MAX_SOUND_STRING,&fileSize);
							if(iRet==0 && fileSize!=0)
							{
								mRecordData.m_mMotionData.motionDetectInfo=motionString;
								mRecordData.m_mMotionData.soundVolumeInfo=soundString;
								iRet=dataRecord(mRecordData.m_server,mRecordData.m_id,mRecordData.m_videoPath,
													mRecordData.m_creatTimeInMilSecond,mRecordData.m_videoFileSize,
													mRecordData.m_jpgPath,mRecordData.m_videoTimeLength,mRecordData.m_mMotionData);
								if(iRet==1)
								{
									 unlink(filePath);
									 unlink(file2Path);
									 LOGOUT("upLoadFile %s and %s success and unlink",filePath,file2Path);
								}
							}
						}
						else
						{
							unlink(filePath);
							unlink(file2Path);
							LOGOUT("filePath %s and %s was error and unlink iRet=%d",filePath,file2Path,iRet);	
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
