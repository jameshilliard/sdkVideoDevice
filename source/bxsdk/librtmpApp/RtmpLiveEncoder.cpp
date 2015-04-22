#include "stdafx.h"
#include "RtmpLiveEncoder.h"
#include "rtmpWrapper/BitWritter.h"
#include "rtmpWrapper/H264FrameParser.h"
extern bool g_start;
int GetSoundRateIndex(int nSize)
{
	if (nSize <= 8000)
		return RateTypeOne;
	if (nSize > 8000 && nSize <= 16000)
		return RateTypeTwo;
	if (nSize > 16000 && nSize <= 32000)
		return RateTypeThree;
	else
		return RateTypeFour;
}

int GetSoundSampleIndex(int nSoundRate)
{
	int nSampleIndex = 0x07;
	switch (nSoundRate)
	{
	case 96000:
		nSampleIndex = 0x00;	
		break;
	case 88200:
		nSampleIndex = 0x01;	
		break;
	case 64000:
		nSampleIndex = 0x02;	
		break;
	case 48000:
		nSampleIndex = 0x03;	
		break;
	case 44100:
		nSampleIndex = 0x04;	
		break;
	case 32000:
		nSampleIndex = 0x05;	
		break;
	case 24000:
		nSampleIndex = 0x06;	
		break;
	case 22050:
		nSampleIndex = 0x07;	
		break;
	case 16000:
		nSampleIndex = 0x08;	
		break;
	case 12000:
		nSampleIndex = 0x09;	
		break;
	case 11025:
		nSampleIndex = 0x0A;	
		break;
	case 8000:
		nSampleIndex = 0x0B;	
		break;
	case 7350:
		nSampleIndex = 0x0C;	
		break;
	}
	return nSampleIndex;
}


RtmpLiveEncoder::RtmpLiveEncoder(int width, int height,const char* rtmpUrl, bool isNeedLog)
:rtmp_url_(rtmpUrl)
{
	width_ = width;
	height_ = height;
	sps_ = NULL;
	sps_size_ = 0;
	pps_ = NULL;
	pps_size_ = 0;

	librtmp_ = new LibRtmp(isNeedLog);

	isStart = false;
	is_firstVideo = true;
	m_bVideoBegin = false;
	m_bAudioBegin = false;
	memset(x264buf,0,200*1024);
	x264buf_len =0;

	memset(sps_buf,0,1024);
	memset(pps_buf,0,1024);
	sps_size = 0;
	pps_size = 0;
}

RtmpLiveEncoder::~RtmpLiveEncoder(void)
{
	if (librtmp_)
		delete librtmp_;
	if (sps_)
		free(sps_);
	if (pps_)
		free(pps_);
}
bool RtmpLiveEncoder::init()
{
	printf("init-----------0\n");
	isStart = false;
	g_start = false;
	sps_size = 0;
	pps_size = 0;
	is_firstVideo = true;
	m_bVideoBegin = false;
	m_bAudioBegin = false;
	// 连接rtmp server，完成握手等协议
	bool result = librtmp_->Open(rtmp_url_.c_str());
	if(!result)
	{
		printf("Open url failed-----------0\n");
		return result;
	}
	printf("Open url success-----------0\n");
	
	// 发送metadata包
	SendMetadataPacket();
	usleep(100);
	g_start = true;
	isStart = result;
	return result;
	
}
void RtmpLiveEncoder::close()
{
	printf("RtmpLiveEncoder::close()------------------0\n");
	librtmp_->Close();
}
int RtmpLiveEncoder::SendVideo(char *h264Nal,int len,__int64 nTimeStamp)
{
	//printf("SendVideo---------------------0\n");
	//boost::asio::detail::mutex::scoped_lock lock(mutex_Send);
	
	return ParseH264FrameForSPSAndPPS(h264Nal,len,nTimeStamp);

}
int RtmpLiveEncoder::SendAudio(char *AudioData,int len,__int64 nTimeStamp,
							   int nEncoderType,int nSoundRate, int nSoundSize ,int nSoundType)
{
	
	if (!m_bVideoBegin)
		return 0;
	if (!m_bAudioBegin)
	{
		if(nEncoderType == 10)//AAC编码
		{
			//printf("----------------RTMP AACAudioBegin\n");
			SendAACSequenceHeaderPacket(nSoundRate,nSoundSize,nSoundType);
		}
		else
		{
			//printf("----------------RTMP MP3AudioBegin\n");
			m_bAudioBegin = true;
		}
	}
	return SendAudioDataPacket(AudioData,len,GetTimestamp(nTimeStamp),nEncoderType,nSoundRate,nSoundSize,nSoundType);
}
//给RTMPSERVER发心跳
int RtmpLiveEncoder::SendHeart()
{
	RTMPPacket rtmp_pakt;
	RTMPPacket_Reset(&rtmp_pakt);
	RTMPPacket_Alloc(&rtmp_pakt, 6);
	char *pend = rtmp_pakt.m_body + 6;
	char *buf;
	rtmp_pakt.m_nChannel = 0x02;	/* control channel (ping) */
	rtmp_pakt.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	rtmp_pakt.m_packetType = RTMP_PACKET_TYPE_CONTROL;
	rtmp_pakt.m_nTimeStamp = 0;	/* RTMP_GetTime(); */
	rtmp_pakt.m_nInfoField2 = 0;
	rtmp_pakt.m_hasAbsTimestamp = 0;
	rtmp_pakt.m_nBodySize = 6;
	buf = rtmp_pakt.m_body;
	buf = AMF_EncodeInt16(buf, pend, 0x06);
	buf = AMF_EncodeInt32(buf, pend, 0x00);
	int ret = librtmp_->Send(&rtmp_pakt);
	RTMPPacket_Free(&rtmp_pakt);
}
//
int RtmpLiveEncoder::SendVideoDataPacket(char *x264buffer,int buflen, unsigned int timestamp, bool isKeyframe)
{
	//printf("SendVideoDataPacket-----timestamp %d--------------0\n", timestamp);
	static int sendNumber = 0;
	sendNumber ++;
	if(sendNumber >50)
	{
		//printf("SendHeart---------------0\n");
		SendHeart();
		sendNumber = 0;
	}
	if(librtmp_->isConnect() == false)
	{
		printf("isConnect-------------------0\n");
		//close();
		//init();
		return -1;
	}
	RTMPPacket rtmp_pakt;
	RTMPPacket_Reset(&rtmp_pakt);
	RTMPPacket_Alloc(&rtmp_pakt, buflen +5);
	rtmp_pakt.m_packetType = FLV_TAG_TYPE_VIDEO;
	rtmp_pakt.m_nBodySize = buflen +5;
	rtmp_pakt.m_nTimeStamp = timestamp;
	rtmp_pakt.m_nChannel = 4;

	char* pbuf = rtmp_pakt.m_body;
	unsigned char flag = 0;
	if (isKeyframe)
		flag = 0x17;
	else
		flag = 0x27;

	pbuf = UI08ToBytes(pbuf, flag);

	pbuf = UI08ToBytes(pbuf, 1);    // avc packet type (0, nalu)
	pbuf = UI24ToBytes(pbuf, 0);    // composition time

	memcpy(pbuf, x264buffer, buflen);

	int ret = librtmp_->Send(&rtmp_pakt);
	RTMPPacket_Free(&rtmp_pakt);
	/*if(ret == 0)
	{
		printf("isConnect-------------------1\n");
		//close();
		init();
	}*/
	return ret;
}

int RtmpLiveEncoder::SendAudioDataPacket(char *audiobuffer,int buflen, unsigned int timestamp,int nEncoderType,
										 int nSoundRate, int nSoundSize ,int nSoundType)
{
	RTMPPacket rtmp_pakt;
	RTMPPacket_Reset(&rtmp_pakt);
	if (nEncoderType == 2)
	{
		RTMPPacket_Alloc(&rtmp_pakt, buflen +1);
		rtmp_pakt.m_nBodySize = buflen +1;
	}
	else if (nEncoderType == 10)
	{
		RTMPPacket_Alloc(&rtmp_pakt, buflen +2);
		rtmp_pakt.m_nBodySize = buflen +2;
	}
	else
		return 0;
	rtmp_pakt.m_packetType = FLV_TAG_TYPE_AUDIO;
	rtmp_pakt.m_nTimeStamp = timestamp;
	rtmp_pakt.m_nChannel = 4;

	char* pbuf = rtmp_pakt.m_body;
	int nSoundRateIndex = GetSoundRateIndex(nSoundRate);

	unsigned char flag = 0;
	flag = (nEncoderType << 4) |			// soundformat "10 == AAC"
		(nSoundRateIndex << 2) |      // soundrate   "3  == 44-kHZ",2 == 22-KHZ
		(nSoundSize << 1) |      // soundsize   "1  == 16bit"
		nSoundType;              // soundtype   "1  == Stereo",

	pbuf = UI08ToBytes(pbuf, flag);
	if (nEncoderType == 10)
		pbuf = UI08ToBytes(pbuf, 1);    // aac packet type (1, raw)

	memcpy(pbuf, audiobuffer, buflen);
	int ret = librtmp_->Send(&rtmp_pakt);
	RTMPPacket_Free(&rtmp_pakt);
	return ret;
}
void RtmpLiveEncoder::SendMetadataPacket()
{
	char metadata_buf[4096];
	char* pbuf = WriteMetadata(metadata_buf);

	librtmp_->Send(metadata_buf, (int)(pbuf - metadata_buf), FLV_TAG_TYPE_META, 0);
}

char* RtmpLiveEncoder::WriteMetadata(char* buf)
{
	char* pbuf = buf;

	pbuf = UI08ToBytes(pbuf, AMF_DATA_TYPE_STRING);
	pbuf = AmfStringToBytes(pbuf, "@setDataFrame");

	pbuf = UI08ToBytes(pbuf, AMF_DATA_TYPE_STRING);
	pbuf = AmfStringToBytes(pbuf, "onMetaData");

	//     pbuf = UI08ToBytes(pbuf, AMF_DATA_TYPE_MIXEDARRAY);
	//     pbuf = UI32ToBytes(pbuf, 2);
	pbuf = UI08ToBytes(pbuf, AMF_DATA_TYPE_OBJECT);

	pbuf = AmfStringToBytes(pbuf, "width");
	pbuf = AmfDoubleToBytes(pbuf, width_);

	pbuf = AmfStringToBytes(pbuf, "height");
	pbuf = AmfDoubleToBytes(pbuf, height_);

	pbuf = AmfStringToBytes(pbuf, "framerate");
	pbuf = AmfDoubleToBytes(pbuf, 10);

	pbuf = AmfStringToBytes(pbuf, "videocodecid");
	pbuf = UI08ToBytes(pbuf, AMF_DATA_TYPE_STRING);
	pbuf = AmfStringToBytes(pbuf, "avc1");

	// 0x00 0x00 0x09
	pbuf = AmfStringToBytes(pbuf, "");
	pbuf = UI08ToBytes(pbuf, AMF_DATA_TYPE_OBJECT_END);

	return pbuf;
}
void RtmpLiveEncoder::SendAVCSequenceHeaderPacket()
{
	char avc_seq_buf[4096];
	char* pbuf = WriteAVCSequenceHeader(avc_seq_buf);

	librtmp_->Send(avc_seq_buf, (int)(pbuf - avc_seq_buf), FLV_TAG_TYPE_VIDEO, 0);
}

char* RtmpLiveEncoder::WriteAVCSequenceHeader(char* buf)
{
	char* pbuf = buf;

	unsigned char flag = 0;
	flag = (1 << 4) |   // frametype "1  == keyframe"
		7;              // codecid   "7  == AVC"

	pbuf = UI08ToBytes(pbuf, flag);

	pbuf = UI08ToBytes(pbuf, 0);    // avc packet type (0, header)
	pbuf = UI24ToBytes(pbuf, 0);    // composition time

	// AVCDecoderConfigurationRecord

	pbuf = UI08ToBytes(pbuf, 1);            // configurationVersion
	pbuf = UI08ToBytes(pbuf, sps_[1]);      // AVCProfileIndication
	pbuf = UI08ToBytes(pbuf, sps_[2]);      // profile_compatibility
	pbuf = UI08ToBytes(pbuf, sps_[3]);      // AVCLevelIndication
	pbuf = UI08ToBytes(pbuf, 0xff);         // 6 bits reserved (111111) + 2 bits nal size length - 1
	pbuf = UI08ToBytes(pbuf, 0xe1);         // 3 bits reserved (111) + 5 bits number of sps (00001)
	pbuf = UI16ToBytes(pbuf, sps_size_);    // sps
	memcpy(pbuf, sps_, sps_size_);
	pbuf += sps_size_;
	pbuf = UI08ToBytes(pbuf, 1);            // number of pps
	pbuf = UI16ToBytes(pbuf, pps_size_);    // pps
	memcpy(pbuf, pps_, pps_size_);
	pbuf += pps_size_;

	return pbuf;
}

void RtmpLiveEncoder::SendAACSequenceHeaderPacket(int nSoundRate, int nSoundSize ,int nSoundType)
{
	char aac_seq_buf[4096];
	char* pbuf = WriteAACSequenceHeader(aac_seq_buf,nSoundRate,nSoundSize,nSoundType);

	librtmp_->Send(aac_seq_buf, (int)(pbuf - aac_seq_buf), FLV_TAG_TYPE_AUDIO, 0);
}

char* RtmpLiveEncoder::WriteAACSequenceHeader(char* buf,int nSoundRate, int nSoundSize ,int nSoundType)
{
	char* pbuf = buf;
	int nSoundRateIndex = GetSoundRateIndex(nSoundRate);
	unsigned char flag = 0;
	flag = (10 << 4) |  // soundformat "10 == AAC"
		(nSoundRateIndex << 2) |      // soundrate   "3  == 44-kHZ"
		(nSoundSize << 1) |      // soundsize   "1  == 16bit"
		nSoundType;              // soundtype   "1  == Stereo"

	pbuf = UI08ToBytes(pbuf, flag);

	pbuf = UI08ToBytes(pbuf, 0);    // aac packet type (0, header)

	// AudioSpecificConfig

	PutBitContext pb;
	init_put_bits(&pb, pbuf, 1024);
	put_bits(&pb, 5, 2);    //object type - AAC-LC

	int nSampleIndex = GetSoundSampleIndex(nSoundRate);
	put_bits(&pb, 4, nSampleIndex);
	put_bits(&pb, 4, 2);    // channel configuration
	//GASpecificConfig
	put_bits(&pb, 1, 0);    // frame length - 1024 samples
	put_bits(&pb, 1, 0);    // does not depend on core coder
	put_bits(&pb, 1, 0);    // is not extension

	flush_put_bits(&pb);

	pbuf += 2;

	return pbuf;
}

// 当收到sps和pps信息时，发送AVC和AAC的sequence header
void RtmpLiveEncoder::OnSPSAndPPS(char* spsBuf, int spsSize, char* ppsBuf, int ppsSize,__int64 nTimeStamp)
{
	sps_ = (char*)malloc(spsSize);
	memcpy(sps_, spsBuf, spsSize);
	sps_size_ = spsSize;

	pps_ = (char*)malloc(ppsSize);
	memcpy(pps_, ppsBuf, ppsSize);
	pps_size_ = ppsSize;

	SendAVCSequenceHeaderPacket();

	time_videoBegin_ = nTimeStamp;
	last_timestamp_ = time_videoBegin_;
	m_bVideoBegin = true;
	
}

unsigned int RtmpLiveEncoder::GetTimestamp(__int64 nTimeStamp)
{
	unsigned int timestamp;

	//if (now < last_timestamp_)
	if (nTimeStamp < time_videoBegin_)
	{
		timestamp = 0;
		last_timestamp_ = nTimeStamp;
		time_videoBegin_ = nTimeStamp;
	}
	else
	{
		timestamp = nTimeStamp - time_videoBegin_;
		//timestamp = now - last_timestamp_;
	}
	return timestamp;
}
int RtmpLiveEncoder::ParseH264FrameForSPSAndPPS(char* frBuf,int frLen,__int64 nTimeStamp)
{
	//printf("ParseH264FrameForSPSAndPPS---------------------0\n");
	bool is_keyframe = 0;
	// 接收到h264裸流，开始处理
	if (sps_size == 0)
	{
	//	printf("ParseH264FrameForSPSAndPPS---------------------0.1\n");
		ParseH264Frame(frBuf, frLen, x264buf, x264buf_len, sps_buf, sps_size,
			pps_buf, pps_size, is_keyframe);
	}
	else
	{
	//	printf("ParseH264FrameForSPSAndPPS---------------------0.2\n");
		ParseH264Frame(frBuf, frLen, x264buf, x264buf_len, NULL, sps_size,
			NULL, pps_size, is_keyframe);
	}

	if (is_firstVideo && sps_size && pps_size)
	{
		OnSPSAndPPS(sps_buf, sps_size, pps_buf, pps_size,nTimeStamp);
		is_firstVideo = false;
	}
	if (x264buf_len > 0 && sps_size && pps_size)
	{
		return SendVideoDataPacket(x264buf, x264buf_len, GetTimestamp(nTimeStamp),is_keyframe);
	}
	return FALSE;
}