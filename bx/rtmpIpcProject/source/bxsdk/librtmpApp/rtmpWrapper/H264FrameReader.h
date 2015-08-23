#ifndef _H264_FRAME_READER_H_
#define _H264_FRAME_READER_H_

#include <cstdio>
#include <stdio.h>
#include <string.h>
class H264FrameReader
{
public:
    H264FrameReader(const char* filename, bool isRepeat);

    ~H264FrameReader();

    bool ReadFrame(char* outBuf, int& outBufSize);

private:
    char* filebuf_;
    const char* pbuf_;
    int filesize_;
    bool is_repeat_;
};

#endif // _H264_FRAME_READER_H_
