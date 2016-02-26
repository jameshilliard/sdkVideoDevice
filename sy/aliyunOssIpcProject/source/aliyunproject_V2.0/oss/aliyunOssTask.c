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

int sendVideoToOss(const char *filePath,const char *videoPath)
{
	int iRet=0;
	iRet=upLoadFile(filePath,videoPath);
	if(iRet==0)
	{
		LOGOUT("upLoadFile %s %s success and unlink",filePath,videoPath);
	}
	else if(iRet==409)//file was exist
	{
		iRet=0;
		LOGOUT("filePath %s %s was exist and unlink",filePath,videoPath); 					
	}
	else
	{
		LOGOUT("upload file %s is error iRet=%d",filePath,iRet);
	}
	return iRet;
}

void * aliyunOssTask(void* param)
{
	char fileName[MAX_PATH];
	char fileDataName[MAX_PATH];
	char filePath[MAX_PATH];
	char file1Path[MAX_PATH];
	char file2Path[MAX_PATH];
	char file3Path[MAX_PATH];
	char file4Path[MAX_PATH];
	char aliyunFilePath[MAX_PATH];
	DWORD fileSize=0;
	RecordData mRecordData;
	int iRet=-1;
	int fileType=-1;
	LOGINRETURNINFO returnInfo;
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
		memset(file4Path,0,sizeof(file2Path));
		memset(motionString,0,MAX_MOTION_STRING);
		memset(soundString,0,MAX_SOUND_STRING);
		memset(aliyunFilePath,0,sizeof(aliyunFilePath));
		iRet=readMediaFile(SYSTEM_MEDIA_SENDFILEPATH,fileName);
		if(iRet==1)
		{
			fileType=iRet;
			sprintf(filePath,"%s/%s",SYSTEM_MEDIA_SENDFILEPATH,fileName);
			memset(fileDataName,0,sizeof(fileDataName));
			sscanf(fileName,"%[^.]",fileDataName);
			sprintf(file1Path,"%s/%s.dat",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
			sprintf(file2Path,"%s/%s.mot",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
			sprintf(file3Path,"%s/%s.sou",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
			sprintf(file4Path,"%s/%s.jpg",SYSTEM_MEDIA_SENDFILEPATH,fileDataName);
			DWORD fileSize=getFileSize(filePath);
			if(strlen(filePath)!=0 && fileSize>0 && fileType==1)
			{
				iRet=isMp4File(filePath);
				if(iRet==0)
				{
					iRet=readFile(file1Path,(LPCTSTR)(&mRecordData),sizeof(mRecordData),&fileSize);
					if(iRet==0)
					{
						fileSize=0;
						iRet=readFile(file2Path,(LPCTSTR)(motionString),MAX_MOTION_STRING,&fileSize);
						//LOGOUT("send:%s motionString:%s",filePath,motionString);
						if(iRet==0 && fileSize>=0)
						{
							fileSize=0;
							iRet=readFile(file3Path,(LPCTSTR)(soundString),MAX_SOUND_STRING,&fileSize);
							if(iRet==0 && fileSize>=0)
							{
								//LOGOUT("send:%s soundString:%s",file2Path,soundString);
								mRecordData.m_mMotionData.motionDetectInfo=motionString;
								mRecordData.m_mMotionData.soundVolumeInfo=soundString;
								LOGOUT("mRecordData.m_iUrencyFlag=%d",mRecordData.m_iUrencyFlag);
								if(mRecordData.m_bUploadVideoFlag==0)
								{
									iRet=sendVideoToOss(filePath,mRecordData.m_videoPath);
									if(iRet==0)
										mRecordData.m_bUploadVideoFlag=1;	
								}
								if(mRecordData.m_bUploadJpgFlag==0)
								{
									iRet=sendVideoToOss(file4Path,mRecordData.m_jpgPath);
									if(iRet==0)
										mRecordData.m_bUploadJpgFlag=1;
								}
								if(mRecordData.m_iUrencyFlag==1 && mRecordData.m_bUploadDataFlag==0  && mRecordData.m_bUploadJpgFlag==1 && mRecordData.m_bUploadVideoFlag==1)
								{
									iRet=reportUrgencyRecord(mRecordData.m_server,mRecordData.m_id,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,mRecordData.m_videoPath,
																	mRecordData.m_creatTimeInMilSecond,mRecordData.m_videoFileSize,
																	mRecordData.m_jpgPath,mRecordData.m_videoTimeLength,mRecordData.m_mMotionData);
									//if(iRet==1)
									mRecordData.m_bUploadDataFlag=1;

								}
								if(mRecordData.m_iUrencyFlag==0 && mRecordData.m_bUploadDataFlag==0 && mRecordData.m_bUploadJpgFlag==1 && mRecordData.m_bUploadVideoFlag==1)
								{
									iRet=dataRecord(mRecordData.m_server,mRecordData.m_id,mRecordData.m_videoPath,
													mRecordData.m_creatTimeInMilSecond,mRecordData.m_videoFileSize,
													mRecordData.m_jpgPath,mRecordData.m_videoTimeLength,mRecordData.m_mMotionData);
									//if(iRet==1)
									mRecordData.m_bUploadDataFlag=1;
								}
								if(mRecordData.m_bUploadDataFlag!=1 || mRecordData.m_bUploadJpgFlag!=1 || mRecordData.m_bUploadVideoFlag!=1)
								{
									iRet=writeFile(file1Path,(LPCTSTR)(&mRecordData),sizeof(mRecordData));
									if(iRet!=0)
									{
										LOGOUT("rewrite dataFile is error %d",iRet);
									}
									continue;
								}
							}
						}
					}
				}	
				unlink(filePath);unlink(file1Path);unlink(file2Path);unlink(file3Path);unlink(file4Path);					
				LOGOUT("filePath %s and %s and %s and %s and %s was unlinked",filePath,file4Path,file1Path,file2Path,file3Path);
			}
			//sleep(1);
		}
		else
		{
			sleep(2);
		}
	}
	exitCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
			 g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword);//退出
	
}


void resloveServerUrgencyMotion(ServerCmdInfo mServerCmdInfo)
{
	tagUrgencyMotionCfg objUrgencyMotionCfg=g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg;
	if(mServerCmdInfo.cmdType==SERVERID_START_URGENCYCONDITION)
	{
		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartPeriod!=-1)
			objUrgencyMotionCfg.m_iStartPeriod=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartPeriod;

		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSumDetect!=-1)
			objUrgencyMotionCfg.m_iStartSumDetect=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSumDetect;

		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSumArea!=-1)
			objUrgencyMotionCfg.m_iStartSumArea=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSumArea;

		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSoundSize!=-1)
			objUrgencyMotionCfg.m_iStartSoundSize=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSoundSize;
	}
	else
	{
		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverPeriod!=-1)
			objUrgencyMotionCfg.m_iOverPeriod=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverPeriod;

		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSumDetect!=-1)
			objUrgencyMotionCfg.m_iOverSumDetect=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSumDetect;

		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSumArea!=-1)
			objUrgencyMotionCfg.m_iOverSumArea=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSumArea;						

		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSoundSize!=-1)
			objUrgencyMotionCfg.m_iOverSoundSize=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSoundSize;

		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iEndRecTime!=-1)
			objUrgencyMotionCfg.m_iEndRecTime=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iEndRecTime;

		if(mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_bEnable!=-1)
			objUrgencyMotionCfg.m_bEnable=mServerCmdInfo.m_unServerCmdInfo.m_objUrgencyMotionCfg.m_bEnable;
	}
	if(0!=memcmp(&objUrgencyMotionCfg,&g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg,sizeof(objUrgencyMotionCfg)))
	{
		SetUrgencyMotionCfg(DEVICECONFIGDIR,objUrgencyMotionCfg);
		memcpy(&g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg,&objUrgencyMotionCfg,sizeof(objUrgencyMotionCfg));
		LOGOUT("SetUrgencyMotionCfg success");
	}
	else
	{
		LOGOUT("SetUrgencyMotionCfg same");
	}
}

void resloveMotionRecordCondition(ServerCmdInfo mServerCmdInfo)
{
	tagMotionCfg objMotionCfg=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg;
	if(mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iBefRecLastTime!=-1)
		objMotionCfg.m_iBefRecLastTime=mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iBefRecLastTime;

	if(mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iBefRecTimes!=-1)
		objMotionCfg.m_iBefRecTimes=mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iBefRecTimes;
	
	if(mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iConRecLastTime!=-1)
		objMotionCfg.m_iConRecLastTime=mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iConRecLastTime;
	
	if(mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iConRecTimes!=-1)
		objMotionCfg.m_iConRecTimes=mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iConRecTimes;
	
	if(mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iEndRecTime!=-1)
		objMotionCfg.m_iEndRecTime=mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_iEndRecTime;

	if(mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_bEnable!=-1)
		objMotionCfg.m_bEnable=mServerCmdInfo.m_unServerCmdInfo.m_objMotionCfg.m_bEnable;
	
	if(0!=memcmp(&objMotionCfg,&g_stConfigCfg.m_unMotionCfg.m_objMotionCfg,sizeof(objMotionCfg)))
	{
		SetMotionCfg(DEVICECONFIGDIR,objMotionCfg);
		memcpy(&g_stConfigCfg.m_unMotionCfg.m_objMotionCfg,&objMotionCfg,sizeof(objMotionCfg));
		LOGOUT("SetMotionCfg success");
	}
	else
	{
		LOGOUT("SetMotionCfg same");
	}

}

void *  commucicateTask(void* param)
{
	ServerCmdInfo mServerCmdInfo;
	int iRet=0;
	while(1)
	{
		if(g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iCommServerTime==0)
		{
			sleep(3);
		}
		else
		{
			memset(&mServerCmdInfo,0,sizeof(mServerCmdInfo));
			iRet=checkAndLoadCmdFromServer(&mServerCmdInfo);	
			if(iRet==0)
			{
				LOGOUT("server Id is %d",mServerCmdInfo.cmdType);
				switch(mServerCmdInfo.cmdType)
				{
				case SERVERID_START_URGENCYCONDITION:
				case SERVERID_END_URGENCYCONDITION:
					{
						resloveServerUrgencyMotion(mServerCmdInfo);
					}
					break;
				case SERVERID_MOTION_RECORDCONDITION:
					{
						resloveMotionRecordCondition(mServerCmdInfo);
					}
					break;
				default:
					break;
				}
			}
			sleep(g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iCommServerTime);
		}
		usleep(100000);
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
	pthread_t m_commucicateTask;//实时播放，过程控制线程
	iRet = pthread_create(&m_commucicateTask, NULL, commucicateTask, NULL);
	if(iRet != 0)
	{
		LOGOUT("can't create thread: %s",strerror(iRet));
		return -1;
	}
	return 0;	
}

int ReleaseAliyunOssTask()
{
	ReleaseOSSConfig();
	return 0;
}
