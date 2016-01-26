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
	char file1Path[MAX_PATH];
	char file2Path[MAX_PATH];
	char file3Path[MAX_PATH];
	char aliyunFilePath[MAX_PATH];
	DWORD fileSize=0;
	RecordData mRecordData;
	int iRet=-1;
	int fileType=-1;
	LOGINRETURNINFO returnInfo;
	//loginCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
	//	      g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,
	//	      &returnInfo);
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
		memset(file3Path,0,sizeof(file2Path));
		memset(motionString,0,MAX_MOTION_STRING);
		memset(soundString,0,MAX_SOUND_STRING);
		memset(aliyunFilePath,0,sizeof(aliyunFilePath));
		//iRet=readMediaFile(SYSTEM_MEDIA_SENDFILEPATH,fileName);
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
						memset(fileDataName,0,sizeof(fileDataName));
						sscanf(fileName,"%[^.]",fileDataName);
						sprintf(file1Path,"%s/%s.dat",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
						sprintf(file2Path,"%s/%s.mot",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
						sprintf(file3Path,"%s/%s.sou",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
						iRet=isMp4File(filePath);
						if(iRet!=0)
						{
							unlink(filePath);
							LOGOUT("upLoadFile %s %s is no mp4 file and unlink",filePath,aliyunFilePath);
							unlink(file1Path);
							unlink(file2Path);
							unlink(file3Path);
							LOGOUT("error mp4 file unlink %s and %s and %s",file1Path,file2Path,file3Path);
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
						iRet=readFile(file1Path,(LPCTSTR)(&mRecordData),sizeof(mRecordData),&fileSize);
						if(iRet!=0)
						{
							unlink(file1Path);
							unlink(file2Path);
							unlink(file3Path);
							LOGOUT("filePath %s and %s and %s was error and unlink iRet=%d",file1Path,file2Path,file3Path,iRet);	
							continue;
						}
						fileSize=0;
						iRet=readFile(file2Path,(LPCTSTR)(motionString),MAX_MOTION_STRING,&fileSize);
						//LOGOUT("send:%s motionString:%s",filePath,motionString);
						if(iRet==0 && fileSize!=0)
						{
							fileSize=0;
							iRet=readFile(file3Path,(LPCTSTR)(soundString),MAX_SOUND_STRING,&fileSize);
							if(iRet==0 && fileSize!=0)
							{
								//LOGOUT("send:%s soundString:%s",file2Path,soundString);
								mRecordData.m_mMotionData.motionDetectInfo=motionString;
								mRecordData.m_mMotionData.soundVolumeInfo=soundString;
								if(mRecordData.m_iUrencyFlag==1)
								{
									iRet=reportUrgencyRecord(mRecordData.m_server,mRecordData.m_id,mRecordData.m_videoPath,
																	mRecordData.m_creatTimeInMilSecond,mRecordData.m_videoFileSize,
																	mRecordData.m_jpgPath,mRecordData.m_videoTimeLength,mRecordData.m_mMotionData);
								}
								else
								{
									//LOGOUT("server:%s id:%s",mRecordData.m_server,mRecordData.m_id);
									iRet=dataRecord(mRecordData.m_server,mRecordData.m_id,mRecordData.m_videoPath,
													mRecordData.m_creatTimeInMilSecond,mRecordData.m_videoFileSize,
													mRecordData.m_jpgPath,mRecordData.m_videoTimeLength,mRecordData.m_mMotionData);
								}
								if(iRet==1)
								{
									 unlink(file1Path);
									 unlink(file2Path);
									 unlink(file3Path);
									 LOGOUT("upLoadFile %s and %s and %s success and unlink",file1Path,file2Path,file3Path);
								}
							}
							else
							{
								unlink(file1Path);
								unlink(file2Path);
								unlink(file3Path);
								LOGOUT("filePath %s and %s and %s was error and unlink iRet=%d",file1Path,file2Path,file3Path,iRet);
							}
						}
						else
						{
							unlink(file1Path);
							unlink(file2Path);
							unlink(file3Path);
							LOGOUT("filePath %s and %s and %s was error and unlink iRet=%d",file1Path,file2Path,file3Path,iRet);	
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
