#include "source_sdk.h"
#include "../LogOut/LogOut.h"
#include "../rtmp/RtmpDLL.h"

extern PRTMPUnit pRtmpServer;

DeviceVideoParam deviceVideoParam[MAXCHANNEL];

int	 GetVideoData(const char *filePath,VIDEO_HEADER *pvHeader,char *pData,long *filePos)
{
    if(NULL==filePath || NULL==pvHeader || NULL==pData || NULL==filePos)
    {
        return -1;
    }
	FILE *fp = fopen(filePath,"r");
	int iRet = -1;
    int header=0;
    long dump= 0;
	
	if(NULL == fp)
	{
		LOGOUT("fopen failed, i_filename = %s, error(%s)", filePath, strerror(errno));
		return -2;
	}
	iRet = fseek(fp, *filePos, SEEK_SET);
    if(iRet < 0)
    {
        *filePos = 0;
        fclose(fp);
		fp = NULL;
        return -3;
    }

    while(1)
    {
	    iRet = fread(&header,sizeof(header),1,fp);
        if(iRet == 1)
        {
            if(YRVIDEO == header)
                break;
        }
        else
        {
            LOGOUT("fread failed, i_filename = %s, error(%s) iRet = %d", filePath, strerror(errno),iRet);
            *filePos = 0;
            fclose(fp);
    		fp = NULL;
            return -4; 
        }
	}
	
	iRet = fread(pvHeader,sizeof(VIDEO_HEADER),1,fp);
    if(iRet != 1)
    {
        LOGOUT("fread failed, i_filename = %s, error(%s) iRet = %d", filePath, strerror(errno),iRet);
        *filePos = 0;
        fclose(fp);
		fp = NULL;
        return -5;  
    }

    if(pvHeader->len <= VIDEOMAXSIZE)
    {
        iRet = fread(pData, sizeof(char),pvHeader->len,fp);
        if(iRet != pvHeader->len)
        {
            LOGOUT("fread failed, i_filename = %s, error(%s) iRet = %d", filePath, strerror(errno),iRet);
            *filePos = 0;
            fclose(fp);
    		fp = NULL;
            return -6;  
        }
        dump = 0;
    }
    else
    {
        dump = pvHeader->len;
    }
    
    *filePos = ftell(fp);
    if(*filePos == -1)
    {
        LOGOUT("ftell failed, i_filename = %s, error(%s) iRet = %d", filePath, strerror(errno),*filePos);
        *filePos = 0;
        fclose(fp);
		fp = NULL;
        return -7; 
    }
    *filePos = *filePos+dump;
    fclose(fp);
    fp = NULL;
    return 0;     
}
/***********************************************************************************
实现函数: SendPlayData; 
功能描述：发送视频流
备注：    该函数主要调用SZYIPCSdk_SendH264主调函数发送实时视频流数据
          使用方法参见SZYIPCSdk_SendH264的API调用说明
***********************************************************************************/
void SendPlayData(DeviceVideoParam mDeviceVideoParam,VIDEO_HEADER vHeader,char *pvData,AUDIO_HEADER aheader,char *paData)
{
	if(pRtmpServer!=NULL)
	{
		SendVideoUnit(pRtmpServer,pvData,vHeader.len,vHeader.ts);
		printf("Video %u PTS: %u %d %d \n", vHeader.fr, vHeader.ts,vHeader.w,vHeader.h);
	}
}

BOOL getDeviceVideoParam(DeviceVideoParam *pDeviceVideoParam,unsigned int size)
{
	if(size>MAXCHANNEL)
		return FALSE;
	memcpy(pDeviceVideoParam,deviceVideoParam,sizeof(DeviceVideoParam)*size);
	int i=0;
	for(i=0;i<size;i++)
	{
		//LOGOUT("getDeviceVideoParam %d is %d",i,pDeviceVideoParam[i].videoSwitch);
	}
	return TRUE;
}

BOOL setDeviceVideoParam(int channelId,DeviceVideoParam mDeviceVideoParam)
{
	if(channelId>MAXCHANNEL)
		return FALSE;
	deviceVideoParam[channelId]=mDeviceVideoParam;
	return TRUE;
}

BOOL setDeviceParamSwitch(int channelId,BOOL videoSwitch)
{
	if(channelId>=MAXCHANNEL)
		return FALSE;
	deviceVideoParam[channelId].videoSwitch=videoSwitch;
	//LOGOUT("deviceVideoParam[%d].videoSwitch=%d",channelId,deviceVideoParam[channelId].videoSwitch);
	return TRUE;
}


/*==================================================================
//函数： playThread
//功能： 实时播放视频
//参数： LPVOID data，将对象传入线程
//返回：无
==================================================================*/
void * playThread(void* param)
{

    VIDEO_HEADER vheader;
    AUDIO_HEADER aheader;
	
	DeviceVideoParam mdeviceVideoParam[MAXCHANNEL];


	char *videoBuffer=NULL;
	char *audioBuffer=NULL;
	videoBuffer=malloc(VIDEOMAXSIZE);
	if(NULL==videoBuffer)
		LOGOUT("videoBuffer is NULL");

	audioBuffer=malloc(VIDEOMAXSIZE);
	if(NULL==audioBuffer)
		LOGOUT("audioBuffer is NULL");
	
	long videoSavePos = 0;
	long audioSavePos = 0;

	BOOL startPalyFlag = FALSE;
	BOOL bRet = FALSE;
    int channel = 0;
	int iRet = -1;
	int i=0;
	
	struct timespec ts_last;
    struct timespec ts_now;
    
	memset(&vheader,0,sizeof(vheader));
	memset(&aheader,0,sizeof(aheader));
    clock_gettime(CLOCK_MONOTONIC, &ts_last);
	
	while(1)
	{
		startPalyFlag=FALSE;
		bRet = getDeviceVideoParam(mdeviceVideoParam,MAXCHANNEL);
		for(i=0;i<MAXBACKNUM;i++)
		{
			if(mdeviceVideoParam[i].videoSwitch==TRUE)
				startPalyFlag=TRUE;
		}
		if(startPalyFlag)
		{
            //获取视频数据
            memset(&vheader,0,sizeof(vheader));
			iRet = GetVideoData("video.h264",&vheader,videoBuffer,&videoSavePos);
			if(iRet < 0)//该文件有损坏，跳到下一个回放文件
			{
				videoSavePos = 0;
                memset(&vheader,0,sizeof(vheader));
				continue;
			}
			
			//获取音频数据
			//memset(&aheader,0,sizeof(aheader));
			//iRet = GetAudioData("audio.aac",&aheader,audioBuffer,&audioSavePos);
			//if(iRet < 0)//该文件有损坏，跳到下一个回放文件
			//{
			//	audioSavePos = 0;
            //    memset(&aheader,0,sizeof(aheader));
			//	continue;
			//}
			//如果要求插入关键帧，则从文件中读取一个关键帧
		
			for(i=0;i<MAXBACKNUM;i++)
			{
				if(mdeviceVideoParam[i].videoSwitch==TRUE)
				{
					if(vheader.bKey == 1 && mdeviceVideoParam[i].insertKeyFreme == TRUE)
					{
					   mdeviceVideoParam[i].insertKeyFreme = FALSE;
					   setDeviceVideoParam(i,mdeviceVideoParam[i]);
					}
					if(FALSE == mdeviceVideoParam[i].insertKeyFreme)
					{ 
						SendPlayData(mdeviceVideoParam[i],vheader,videoBuffer,aheader,audioBuffer); 
					} 
				}
			}	
			usleep(40000);
		}
		else
            sleep(1);	
	}
	free(videoBuffer);
	videoBuffer=NULL;
	free(audioBuffer);
	audioBuffer=NULL;
	return NULL;
}

int InitSourceSDKServer()
{
	int iRet;
	pthread_t m_playThread;//实时播放，过程控制线程
	iRet = pthread_create(&m_playThread, NULL, playThread, NULL);
	if(iRet != 0)
	{
		LOGOUT("can't create thread: %s",strerror(iRet));
		return -1;
	}
	return 0;
}


int ReleaseSourceSDKServer()
{
	
}
    


