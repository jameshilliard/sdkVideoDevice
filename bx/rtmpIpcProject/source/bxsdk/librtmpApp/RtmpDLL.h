#ifndef __RTMP_DLL_H
#define __RTMP_DLL_H


enum{
	Sound_MP3 = 2,		//MP3编码
	Sound_AAC = 10,		//AAC编码
};

enum{
	SizeTypeOne = 0,		//第一种8bit
	SizeTypeTwo = 1,		//第二种16bit
};

enum{
	mono = 0,		//单声道
	stereo = 1,		//立体声
};


#ifdef __cplusplus
extern "C" {
#endif

typedef void* PRTMPUnit;

PRTMPUnit  	RtmpUnitOpen(int width, int height,const char* rtmpUrl, unsigned char isNeedLog);
void  		InitRTMPUnit(PRTMPUnit unit);
int  		SendVideoUnit(PRTMPUnit unit,char *h264Nal,int len,__int64 nTimeStamp);
int  		SendAudioUnit(PRTMPUnit unit,char *AudioData,int len,__int64 nTimeStamp,int nEncoderType,
								int nSoundRate, int nSoundSize ,int nSoundType);
void  		CloseRTMPUnit(PRTMPUnit unit);
int    GetRtmpConnectStatus();//返回：0失败 其它：成功

#ifdef __cplusplus
}
#endif
#endif