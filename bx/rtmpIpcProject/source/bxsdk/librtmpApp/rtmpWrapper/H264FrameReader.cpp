#include "../stdafx.h"
#include "H264FrameReader.h"

#include "H264FrameParser.h"

H264FrameReader::H264FrameReader(const char* filename, bool isRepeat)
{
    FILE* fp = fopen(filename, "rb");
    is_repeat_ = isRepeat;
    filebuf_ = NULL;
    filesize_ = 0;

    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        filesize_ = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        filebuf_ = new char[filesize_];
        fread(filebuf_, 1, filesize_, fp);

        fclose(fp);
    }
    pbuf_ = filebuf_;
}

H264FrameReader::~H264FrameReader()
{
    delete filebuf_;
}

bool H264FrameReader::ReadFrame(char* outBuf, int& outBufSize)
{
    char startcodebuf[] = {0x00, 0x00, 0x00, 0x01};
    if (pbuf_ >= filebuf_ + filesize_)
    {
        if (is_repeat_)
        {
            pbuf_ = filebuf_;
        }
        else
        {
            return false;
        }
    }

    char* pbufout = outBuf;
    const char *p = pbuf_;
    const char *end = filebuf_ + filesize_;

    const char *nal_start, *nal_end;

    nal_start = AVCFindStartCode(p, end);

    bool has_video_nal = false;

    while (nal_start < end)
    {
        while(!*(nal_start++));

        nal_end = AVCFindStartCode(nal_start, end);

        unsigned int nal_size = nal_end - nal_start;
        unsigned char nal_type = nal_start[0] & 0x1f;

        if (nal_type == 7)        /* SPS */
        {
            has_video_nal = false;
        }
        else if (nal_type == 8)   /* PPS */
        {
            has_video_nal = false;
        }
        else
        {
            has_video_nal = true;
        }

        memcpy(pbufout, startcodebuf, 4);
        pbufout += 4;
        memcpy(pbufout, nal_start, nal_size);
        pbufout += nal_size;

        nal_start = nal_end;

        if (has_video_nal)
        {
            break;
        }
    }
    
    outBufSize = pbufout - outBuf;
    pbuf_ = nal_start;
	return true;
}
