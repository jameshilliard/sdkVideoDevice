#include "stdafx.h"
#include "RtmpLiveEncoder.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef void* PRTMPUnit;
//CMapBase<long,RtmpLiveEncoder*> m_RtmpUnitList;
RtmpLiveEncoder *encoder = NULL;
char g_rtmpUrl[1024] = {0};
__int64 g_time = 0;
bool g_start = false;
pthread_mutex_t testlock;
 PRTMPUnit  RtmpUnitOpen(int width, int height,const char* rtmpUrl, unsigned char isNeedLog)
{	
		printf("RtmpUnitOpen---------------0\n");
		pthread_mutex_init(&testlock, NULL);
		g_start = false;
		g_time = 0;
		//if(encoder != NULL && strcmp(g_rtmpUrl, rtmpUrl) != 0)
		if(encoder != NULL)
		{
			/*PRTMPUnit unit;
			CloseRTMPUnit(unit);*/
			encoder->close();
			delete encoder;
			encoder = NULL;
		}
		else
		{
			memset(g_rtmpUrl, 0, 1024);
			strcpy(g_rtmpUrl, rtmpUrl);
		}
		if(isNeedLog==0)
			encoder = RtmpLiveEncoder::createNew(width,height,rtmpUrl,false);
		else
			encoder = RtmpLiveEncoder::createNew(width,height,rtmpUrl,true);
		/*if (encoder != NULL)
			m_RtmpUnitList.AddData(long(encoder),encoder);*/
		return encoder;
}
void  InitRTMPUnit(PRTMPUnit unit)
{
		/*RtmpLiveEncoder *encoder = (RtmpLiveEncoder*)unit;
		if (m_RtmpUnitList.FindData(long(encoder)))
		{
			
			return true;
		}
		else
			return false;*/
		encoder->init();
}
int  SendVideoUnit(PRTMPUnit unit,char *h264Nal,int len,__int64 nTimeStamp)
{
	if(!g_start)
	{
		return 1;
	}
	pthread_mutex_lock(&testlock);
	if(g_time != 0)
	{
		g_time = nTimeStamp;
	}
	//int result = encoder->SendVideo(h264Nal,len,nTimeStamp);
	int result = encoder->SendVideo(h264Nal,len,g_time);
	static int countNumber = 0;
	g_time ++;
	if(result == 0)
	{
		countNumber++;
	}
	else
	{
		countNumber = 0;
	}
	if(result == -1 || countNumber>100)
	{

		
		printf("SendVideoUnit------%d---------%d-------reconnect------0\n", result, countNumber);
		countNumber = 0;
		//encoder->close();
		//encoder->init();
		g_start = false;
		if(encoder != NULL)
		{
			encoder->close();
			delete encoder;
			encoder = NULL;
		}
	}
	pthread_mutex_unlock(&testlock);
	return result;
}
int  SendAudioUnit(PRTMPUnit unit,char *AudioData,int len,__int64 nTimeStamp,
												  int nEncoderType,int nSoundRate, int nSoundSize ,int nSoundType)
{
	//printf("SendAudioUnit--------------------11010\n");
	/*RtmpLiveEncoder *encoder = (RtmpLiveEncoder*)unit;
	if (m_RtmpUnitList.FindData(long(encoder)))
		return encoder->SendAudio(AudioData,len,nTimeStamp,nEncoderType,nSoundRate,nSoundSize,nSoundType);
	return -1;*/
	static int sendCount = 0;
	int result =  encoder->SendAudio(AudioData,len,nTimeStamp,nEncoderType,nSoundRate,nSoundSize,nSoundType);
/*	if(result == 0)
	{
		//printf("SendVideoUnit----------------------reconnect------0\n");
		//encoder->close();
		//encoder->init();
		sendCount++ ;
		if(sendCount >10)
		{
			g_start = false;
		}
	}
	else
	{
		sendCount = 0;
	}*/
	return result;
}
void  CloseRTMPUnit(PRTMPUnit unit)
{
	//return false;
	//printf("CloseRTMPUnit--------------------11010\n");
	//sleep(1);
	/*RtmpLiveEncoder *encoder = (RtmpLiveEncoder*)unit;
	if (m_RtmpUnitList.FindData(long(encoder)))
	{
		m_RtmpUnitList.DelData(long(encoder));
		if(encoder != NULL)
		{
			
		}
		//m_RtmpUnitList.DelData(long(encoder));
	}*/
	g_start = false;
	if(encoder != NULL)
	{
		encoder->close();
		delete encoder;
		encoder = NULL;
	}
}
//
int    GetRtmpConnectStatus()
{
	if(g_start)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}
#ifdef __cplusplus
	}
#endif