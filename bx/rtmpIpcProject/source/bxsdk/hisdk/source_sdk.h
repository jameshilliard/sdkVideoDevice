#ifndef __HI_SDK_H_
#define  __HI_SDK_H_

#include <stdio.h>
#include <unistd.h>
#include "../Common/Typedef.h"

//������Ƶ��������ؽṹ��
typedef struct
{
	int 			freamFlag;
	int 			nChannel;             	//ͨ����,��0��ʼ����
	int 			freamType;
	unsigned long	ts;						//ʱ�������λ�����룬����
	int 			w;						//��Ƶ��
	int 			h;						//��Ƶ��
	int 			fr;						//֡��
	int 			bKey;					//�Ƿ�ؼ�֡
	int 			len;					//���ݳ���
}VIDEO_HEADER;
//������Ƶ��������ؽṹ��
typedef struct
{
	int 			nChannel;             		//ͨ����,��0��ʼ����
	unsigned long	ts;							//ʱ�������λ�����룬����
	unsigned int	nSamplePerSec;        		//������
	int				wBitPerSample;        		//λ��
	int				wFormat;					//ѹ����ʽ��Ŀǰֻ֧��AAC��ʽ��Ƶ���룬�˲����ݺ���
	int 			len;						//���ݳ���
}AUDIO_HEADER;

typedef struct videoParam{
	int 		 nChannel;             		//ͨ����,��0��ʼ����
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