#ifndef __G711A_H264_TO_MP4_H__
#define __G711A_H264_TO_MP4_H__
#include <stdio.h>
#include <unistd.h>
#include "faac.h"
#include "mp4v2/mp4v2.h"  
#include "voAAC.h"
#include "cmnMemory.h"


#define  JOSEPH_G711A_LOCATION  "./av_file/test1.g711a"
#define  JOSEPH_H264_LOCALTION "./av_file/"
#define  JOSEPH_MP4_FILE   "test.mp4"

#define  MP4_DETAILS_ALL	 0xFFFFFFFF
#define  NALU_SPS  0
#define  NALU_PPS  1
#define  NALU_I    2
#define  NALU_P    3
#define  NALU_SET  4

#define 	RECORDIDLE		0x1000
#define 	RECORDSTART		0x1001
#define 	RECORDVIDEO		0x1002
#define 	RECORDAUDIO		0x1005
#define 	RECORDDELETE	0x1003
#define 	RECORDSTOP      0x1004


#define VIDEO_TIME_SCALE 90000
#define AUDIO_TIME_SCALE 8000
#define MOVIE_TIME_SCALE VIDEO_TIME_SCALE
#define PTS2TIME_SCALE(CurPTS, PrevPTS, timeScale) \
	((MP4Duration)((CurPTS - PrevPTS) * 1.0 / (double)(1e+3) * timeScale))
	

typedef unsigned int  	uint32_t;
typedef unsigned char   uint8_t;

typedef struct Joseph_Acc_Config
{
	FILE * 			fpIn;                    //打开的音频文件
	faacEncHandle 	hEncoder;        //音频文件描述符
	unsigned long 	nSampleRate;     //音频采样数
	unsigned int 	nChannels;  	      //音频声道数
	unsigned int 	nPCMBufferSize;
	unsigned int 	nPCMBitSize;        //音频采样精度
	unsigned long 	nInputSamples;      //每次调用编码时所应接收的原始数据长度
	unsigned long 	nMaxOutputBytes;    //每次调用编码时生成的AAC数据的最大长度
	unsigned char* 	pbPCMBuffer;       //pcm数据
	unsigned int 	nMaxInputPcmBuffer;
	unsigned int    nPcmBufferLelf;
	unsigned char* 	pbAACBuffer;       //aac数据
	unsigned int   	valid;

	VO_AUDIO_CODECAPI codec_api;
	VO_HANDLE handle;
	VO_MEM_OPERATOR mem_operator;
	VO_CODEC_INIT_USERDATA user_data;
	AACENC_PARAM params;

	VO_CODECBUFFER input;
	VO_CODECBUFFER output;
	VO_AUDIO_OUTPUTINFO output_info;

}JOSEPH_ACC_CONFIG;

typedef struct Joseph_Mp4_Config
{
	FILE* fpInVideo;               //打开的视频文件
	MP4FileHandle hFile;           //mp4文件描述符
	MP4TrackId video;              //视频轨道标志符
	MP4TrackId audio;              //音频轨道标志符
	int m_vFrameDur;               //帧间隔时间
	unsigned int timeScale;        //视频每秒的ticks数,如90000
	unsigned int fps;              //视频帧率
	unsigned short width;          //视频宽
	unsigned short height;         //视频高
	unsigned int  valid;
	time_t 	m_startTime;
	time_t 	m_overTime;
	char  	nFifoName[256];
	char 	nPictureName[256];
	char 	nFifoEndName[256];
	char 	nPictureEndName[256];
	char 	nDataEndName[256];
	char 	nMotionEndName[256];
	char 	nSoundEndName[256];
	char 	nOssVideoName[256];
	char 	nOssJpgName[256];
}JOSEPH_MP4_CONFIG;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

	int InitMp4Module(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config);
	int CloseMp4Module(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config);
	int Mp4FileVideoEncode(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config,unsigned char* nBuffer,unsigned int length,unsigned int pts);
	int Mp4FileAudioEncode(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config,unsigned char* nBuffer,unsigned int length,unsigned int *power,unsigned int pts);
	int g711a_encode(void *pout_buf, int *pout_len, const void *pin_buf, const int in_len);
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#endif

