#ifndef _LIB_RTMP_H_
#define _LIB_RTMP_H_

#include "../librtmp/rtmp.h"
#include "../librtmp/log.h"

class LibRtmp
{
public:
    LibRtmp(bool isNeedLog);

    ~LibRtmp();

    bool Open(const char* url);

    void Close();

    int Send(const char* buf, int bufLen, int type, unsigned int timestamp);
	int Send(RTMPPacket *pRTMPPacket);
    void SendSetChunkSize(unsigned int chunkSize);
	int ReadData();
	bool isConnect()
	{
		//printf("isConnect---------------inin\n");
		if(rtmp_)
			return RTMP_IsConnected(rtmp_);
		else
			return false;
	};
private:
    RTMP* rtmp_;
    char* streming_url_;
    FILE* flog_;
};

#endif // _LIB_RTMP_H_
