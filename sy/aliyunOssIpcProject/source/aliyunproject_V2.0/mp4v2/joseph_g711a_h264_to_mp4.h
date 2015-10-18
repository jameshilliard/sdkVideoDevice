#ifndef __G711A_H264_TO_MP4_H__
#define __G711A_H264_TO_MP4_H__
#include <stdio.h>
#include <unistd.h>
#include "faac.h"
#include "mp4v2/mp4v2.h"  

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

typedef unsigned int  	uint32_t;
typedef unsigned char   uint8_t;

typedef struct Joseph_Acc_Config
{
	FILE * 			fpIn;                    //�򿪵���Ƶ�ļ�
	faacEncHandle 	hEncoder;        //��Ƶ�ļ�������
	unsigned long 	nSampleRate;     //��Ƶ������
	unsigned int 	nChannels;  	      //��Ƶ������
	unsigned int 	nPCMBufferSize;
	unsigned int 	nPCMBitSize;        //��Ƶ��������
	unsigned long 	nInputSamples;      //ÿ�ε��ñ���ʱ��Ӧ���յ�ԭʼ���ݳ���
	unsigned long 	nMaxOutputBytes;    //ÿ�ε��ñ���ʱ���ɵ�AAC���ݵ���󳤶�
	unsigned char* 	pbPCMBuffer;       //pcm����
	unsigned int 	nMaxInputPcmBuffer;
	unsigned int    nPcmBufferLelf;
	unsigned char* 	pbAACBuffer;       //aac����
	unsigned int   	valid;

}JOSEPH_ACC_CONFIG;

typedef struct Joseph_Mp4_Config
{
	FILE* fpInVideo;               //�򿪵���Ƶ�ļ�
	MP4FileHandle hFile;           //mp4�ļ�������
	MP4TrackId video;              //��Ƶ�����־��
	MP4TrackId audio;              //��Ƶ�����־��
	int m_vFrameDur;               //֡���ʱ��
	unsigned int timeScale;        //��Ƶÿ���ticks��,��90000
	unsigned int fps;              //��Ƶ֡��
	unsigned short width;          //��Ƶ��
	unsigned short height;         //��Ƶ��
	unsigned int  valid;
	time_t 	m_startTime;
	time_t 	m_overTime;
	char  	nFifoName[256];
	char 	nPictureName[256];
	char 	nFifoEndName[256];
	char 	nPictureEndName[256];
	char 	nDataEndName[256];
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
	int Mp4FileVideoEncode(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config,unsigned char* nBuffer,unsigned int length);
	int Mp4FileAudioEncode(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config,unsigned char* nBuffer,unsigned int length);
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#endif

