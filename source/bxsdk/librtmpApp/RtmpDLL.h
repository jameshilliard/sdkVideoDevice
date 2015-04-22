#ifndef __RTMP_DLL_H
#define __RTMP_DLL_H


enum{
	Sound_MP3 = 2,		//MP3����
	Sound_AAC = 10,		//AAC����
};

enum{
	SizeTypeOne = 0,		//��һ��8bit
	SizeTypeTwo = 1,		//�ڶ���16bit
};

enum{
	mono = 0,		//������
	stereo = 1,		//������
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
int    GetRtmpConnectStatus();//���أ�0ʧ�� �������ɹ�

#ifdef __cplusplus
}
#endif
#endif