#pragma once

class _declspec(dllimport) RtmpLiveEncoder
{
public:
	RtmpLiveEncoder(int width, int height,const std::string& rtmpUrl, bool isNeedLog);
	~RtmpLiveEncoder(void);
	void init();

	void SendVideo(char *h264Nal,int len);

	void close();
}