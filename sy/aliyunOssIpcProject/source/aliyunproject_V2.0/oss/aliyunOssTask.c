#include "aliyunOssTask.h"
#include "../hisdk/hi_sdk.h"
#include "../LogOut/LogOut.h"


static int sendVideoToOss(const char *filePath,const char *videoPath)
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
static void getLogName(int timeStamp,char *timeStr,int size)
{
	struct tm * timeinfo;
	struct tm utc_tm;
	srand((int) time(0)); 
	timeinfo = gmtime_r( (time_t *)&timeStamp,&utc_tm );
	strftime(timeStr,size,("%Y-%m-%d_%H-%M-%S.tar.gz"),timeinfo);
	printf("timeStr=%s-----\n",timeStr);
	return;
}

static int sendLogToOSS(char *ipcId)
{
	if(ipcId==NULL || strlen(ipcId)==0)
		return -1;
	char *ptr=NULL;
	char fileName[MAX_PATH]={0,};
	char filePath[MAX_PATH]={0,};
	char localPath[MAX_PATH]={0,};
	char systemCmd[1024]={0,};
	int  iRet=0;
	memset(fileName,0,sizeof(fileName));
	memset(filePath,0,sizeof(filePath));
	memset(localPath,0,sizeof(localPath));
	memset(systemCmd,0,sizeof(systemCmd));
	time_t localTime;
	time((time_t *)&localTime);
	localTime+=CHINATIME;
	getLogName(localTime,fileName,sizeof(fileName));
	#if 0
	iRet=isDeviceAccess(LOGTEMPDIR);
	if(iRet!=0)
	{
		memset(systemCmd,0,sizeof(systemCmd));
		sprintf(systemCmd,"mkdir -p %s",LOGTEMPDIR);
		iRet=system(systemCmd);
		if(iRet<0)
		{
			LOGOUT("%s failure",systemCmd);
			return -2;
		}
	}
	memset(systemCmd,0,sizeof(systemCmd));
	sprintf(systemCmd,"cp -fr %s/%s/* %s",LOGUPDATEDIR,LOGFILEDIR,LOGTEMPDIR);
	iRet=system(systemCmd);
	if(iRet<0)
	{
		LOGOUT("%s failure %d",systemCmd,iRet);
		return -3;
	}
	memset(systemCmd,0,sizeof(systemCmd));
	sprintf(systemCmd,"cp -fr %s/%s/* %s",LOGDIR,LOGFILEDIR,LOGTEMPDIR);
	//iRet=system(systemCmd);
	if(iRet<0)
	{
		LOGOUT("%s failure",systemCmd);
		return -4;
	}
	#endif
	memset(systemCmd,0,sizeof(systemCmd));
	sprintf(systemCmd,"tar zcvf %s/%s %s/%s",TEMPDIR,fileName,LOGDIR,LOGFILEDIR);
	iRet=system(systemCmd);
	if(iRet<0)
	{
		LOGOUT("%s failure",systemCmd);
		return -5;
	}
	sprintf(localPath,"%s/%s",TEMPDIR,fileName);
	sprintf(filePath, "Log/%s/%s",(strlen(ipcId)==0)?"test":ipcId,fileName);
	iRet=sendVideoToOss(localPath,filePath);
	if(iRet!=0)
	{
		LOGOUT("sendVideoToOss %s %s is failure %d",localPath,filePath,iRet);
		return -6;
	}
	else
	{
		memset(systemCmd,0,sizeof(systemCmd));
		sprintf(systemCmd,"rm -fr %s/*",TEMPDIR);
		iRet=system(systemCmd);
		if(iRet<0)
		{
			LOGOUT("%s failure",systemCmd);
		}
		LOGOUT("%s success",systemCmd);
	}
	return iRet;
}

static void * aliyunOssTask(void* param)
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
	DWORD oldTime=0;
	DWORD newTime=0;
	DWORD lastLogTime=0;
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
	newTime=getTickSecond();
	oldTime=newTime;
	lastLogTime=newTime;
	while(1)
	{
		newTime=getTickSecond();
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
		if((newTime-lastLogTime)>24*60*60 && returnInfo.logUploadEnable==1)
		{
			iRet=sendLogToOSS(g_szServerNO);
			if(iRet==0)
			{
				lastLogTime=newTime;
			}
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


static void resloveServerUrgencyMotion(ServerCmdInfo mServerCmdInfo)
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

static void resloveMotionRecordCondition(ServerCmdInfo mServerCmdInfo)
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

static void *  commucicateTask(void* param)
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
			iRet=checkAndLoadCmdFromServer(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,
										   g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,
										   &mServerCmdInfo);	
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
