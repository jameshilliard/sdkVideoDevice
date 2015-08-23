#include "../stdafx.h"
#include "LibRtmp.h"

#include "AmfByteStream.h"

LibRtmp::LibRtmp(bool isNeedLog)
{
    if (isNeedLog)
    {
        flog_ = fopen("librtmp.log", "w");
        RTMP_LogSetLevel(RTMP_LOGDEBUG2);
        RTMP_LogSetOutput(flog_);
    }
    else
    {
        flog_ = NULL;
    }

    rtmp_ = RTMP_Alloc();
    RTMP_Init(rtmp_);
    RTMP_SetBufferMS(rtmp_, 300);

    streming_url_ = NULL;
}

LibRtmp::~LibRtmp()
{
    Close();
    RTMP_Free(rtmp_);

    if (streming_url_)
    {
        free(streming_url_);
        streming_url_ = NULL;
    }

    if (flog_) fclose(flog_);
}
//ÅÐ¶ÏÐÄÌøÊÇ·ñ³¬Ê±
int LibRtmp::ReadData()
{
	if (!isConnect())
	{
		return 0;	
	}

	RTMPPacket rtmppacket = {0};
	int nRet = RTMP_ReadPacket(rtmp_, &rtmppacket);
	printf("nRet--------%d----0\n", nRet);
	if (nRet == 0)
	{
		return 0;
	}
	
	if (RTMPPacket_IsReady(&rtmppacket))
	{
		printf("nRet--------%d----1\n", nRet);
		RTMPPacket_Free(&rtmppacket);

		return 1;
	}
	
	//printf("nRet--------%d----2\n", nRet);
	return 2;

}
//
bool LibRtmp::Open(const char* url)
{
    streming_url_ = (char*)calloc(strlen(url)+1, sizeof(char));
    strcpy(streming_url_, url);

    //AVal flashver = AVC("flashver");
    //AVal flashver_arg = AVC("WIN 9,0,115,0");
    AVal swfUrl = AVC("swfUrl");
    AVal swfUrl_arg = AVC("http://localhost/librtmp.swf");
    AVal pageUrl = AVC("pageUrl");
    AVal pageUrl_arg = AVC("http://localhost/librtmp.html");
    //RTMP_SetOpt(rtmp_, &flashver, &flashver_arg);
    RTMP_SetOpt(rtmp_, &swfUrl, &swfUrl_arg);
    RTMP_SetOpt(rtmp_, &pageUrl, &pageUrl_arg);
    AVal record = AVC("record");
    AVal record_arg = AVC("on");
    RTMP_SetOpt(rtmp_, &record, &record_arg);

    int err = RTMP_SetupURL(rtmp_, streming_url_);
    if (err <= 0) return false;

    RTMP_EnableWrite(rtmp_);
    
    err = RTMP_Connect(rtmp_, NULL);
    if (err <= 0) return false;

    err = RTMP_ConnectStream(rtmp_, 0);
    if (err <= 0) return false;

    return true;
}

void LibRtmp::Close()
{
    RTMP_Close(rtmp_);
}

int LibRtmp::Send(const char* buf, int bufLen, int type, unsigned int timestamp)
{
    RTMPPacket rtmp_pakt;
    RTMPPacket_Reset(&rtmp_pakt);
    RTMPPacket_Alloc(&rtmp_pakt, bufLen);

    rtmp_pakt.m_packetType = type;
    rtmp_pakt.m_nBodySize = bufLen;
    rtmp_pakt.m_nTimeStamp = timestamp;
    rtmp_pakt.m_nChannel = 4;
    rtmp_pakt.m_headerType = RTMP_PACKET_SIZE_LARGE;
    rtmp_pakt.m_nInfoField2 = rtmp_->m_stream_id;
    memcpy(rtmp_pakt.m_body, buf, bufLen);
    int ret = RTMP_SendPacket(rtmp_, &rtmp_pakt, 0);
    RTMPPacket_Free(&rtmp_pakt);
	return ret;
}

int LibRtmp::Send(RTMPPacket *pRTMPPacket)
{
	pRTMPPacket->m_headerType = RTMP_PACKET_SIZE_LARGE;
	pRTMPPacket->m_nInfoField2 = rtmp_->m_stream_id;
	int ret = RTMP_SendPacket(rtmp_, pRTMPPacket, 0);
	//printf("ret-----%d-------------0\n", ret);
	return ret;
}


void LibRtmp::SendSetChunkSize(unsigned int chunkSize)
{
    RTMPPacket rtmp_pakt;
    RTMPPacket_Reset(&rtmp_pakt);
    RTMPPacket_Alloc(&rtmp_pakt, 4);

    rtmp_pakt.m_packetType = 0x01;
    rtmp_pakt.m_nChannel = 0x02;    // control channel
    rtmp_pakt.m_headerType = RTMP_PACKET_SIZE_LARGE;
    rtmp_pakt.m_nInfoField2 = 0;


    rtmp_pakt.m_nBodySize = 4;
    UI32ToBytes(rtmp_pakt.m_body, chunkSize);

    RTMP_SendPacket(rtmp_, &rtmp_pakt, 0);
    RTMPPacket_Free(&rtmp_pakt);
}
