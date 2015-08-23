#pragma once
//#include "rtmpWrapper/DataBuffer.h"
#include "rtmpWrapper/LibRtmp.h"
#include "rtmpWrapper/AmfByteStream.h"
//#include <pthread.h>

#include <deque>
#include <string>
#include <vector>
using namespace std;
//#define ENCODE_AUDIO_USE_MP3

enum{
	RateTypeOne = 0,		//第一种5.5KHZ
	RateTypeTwo = 1,		//第二种11KHZ
	RateTypeThree = 2,		//第三种22KHZ
	RateTypeFour = 3,		//第四种44KHZ	
};

class  RtmpLiveEncoder
{
public:
	static RtmpLiveEncoder*
		RtmpLiveEncoder::createNew(int width, int height,const char* rtmpUrl, bool isNeedLog) 
	{
		RtmpLiveEncoder* newSource = NULL;
		newSource	= new RtmpLiveEncoder(width,height,rtmpUrl,isNeedLog);

		return newSource;
	}

	~RtmpLiveEncoder(void);
	

	bool init();

	int SendVideo(char *h264Nal,int len,__int64 nTimeStamp);
	int SendAudio(char *AudioData,int len,__int64 nTimeStamp,int nEncoderType,int nSoundRate, int nSoundSize ,int nSoundType);
	void close();
	bool isStart;
private:
	int ParseH264FrameForSPSAndPPS(char* frBuf,int frLen,__int64 nTimeStamp);

	void OnSPSAndPPS(char* spsBuf, int spsSize, char* ppsBuf, int ppsSize,__int64 nTimeStamp);

	int SendVideoDataPacket(char *x264buffer,int buflen, unsigned int timestamp, bool isKeyframe);

	int SendAudioDataPacket(char *audiobuffer,int buflen, unsigned int timestamp,int nEncoderType,
		int nSoundRate, int nSoundSize ,int nSoundType);

	void SendMetadataPacket();

	void SendAVCSequenceHeaderPacket();

	void SendAACSequenceHeaderPacket(int nSoundRate, int nSoundSize ,int nSoundType);

	char* WriteMetadata(char* buf);

	char* WriteAVCSequenceHeader(char* buf);

	char* WriteAACSequenceHeader(char* buf,int nSoundRate, int nSoundSize ,int nSoundType);

	unsigned int GetTimestamp(__int64 nTimeStamp);

	RtmpLiveEncoder(int width, int height,const char* rtmpUrl, bool isNeedLog);
	int SendHeart();
	bool RtmpRead();
private:
	LibRtmp* librtmp_;
	std::string rtmp_url_;

	// metadata
	int width_;
	int height_;

	char* sps_;        // sequence parameter set
	int sps_size_;
	char* pps_;        // picture parameter set
	int pps_size_;

	__int64 time_videoBegin_;
	__int64 last_timestamp_;
	bool is_firstVideo;

	char x264buf[200*1024];
	int x264buf_len;

	char sps_buf[1024];
	char pps_buf[1024];
	int sps_size;
	int pps_size;

	unsigned int timestamp;
	bool m_bVideoBegin;
	bool m_bAudioBegin;

	//boost::asio::detail::mutex mutex_Send;
};
