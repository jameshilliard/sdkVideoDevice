#ifndef __HI_SDK_H_
#define  __HI_SDK_H_

#include <stdio.h>
#include <unistd.h>
#include "../Common/Typedef.h"

//发送视频函数和相关结构体
typedef struct
{
	int 			freamFlag;
	int 			nChannel;             	//通道号,从0开始计数
	int 			freamType;
	unsigned long	ts;						//时间戳，单位：毫秒，递增
	int 			w;						//视频宽
	int 			h;						//视频高
	int 			fr;						//帧率
	int 			bKey;					//是否关键帧
	int 			len;					//数据长度
}VIDEO_HEADER;
//发送音频函数和相关结构体
typedef struct
{
	int 			nChannel;             		//通道号,从0开始计数
	unsigned long	ts;							//时间戳，单位：毫秒，递增
	unsigned int	nSamplePerSec;        		//采样率
	int				wBitPerSample;        		//位数
	int				wFormat;					//压缩格式，目前只支持AAC格式音频输入，此参数暂忽略
	int 			len;						//数据长度
}AUDIO_HEADER;

typedef struct videoParam{
	int 		 nChannel;             		//通道号,从0开始计数
	unsigned int gBitRate;
	unsigned int gFrate;
	unsigned int gQuality;
	unsigned int gKeyFrameRate; 
	unsigned int gWeight; 
	unsigned int gHight;
	BOOL		 insertKeyFreme;
	BOOL  		 videoSwitch;
	BOOL  		 audioSwitch;
}DeviceVideoParam;


#define     VIDEOMAXSIZE    200 *1024
#define     AUDIOMAXSIZE    30  *1024
#define     PICMAXSIZE      200 *1024
#define  	YRVIDEO			'VDYR'
#define  	YRAUIDO			'AUYR'
#define 	MAXREPLAYNUM 	1	


#define ALARMTIME    30
#define MAXCHANNEL   4
#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	BOOL setDeviceParamSwitch(int channelId,BOOL videoSwitch);
	int InitSourceSDKServer();
	int ReleaseSourceSDKServer();

#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif