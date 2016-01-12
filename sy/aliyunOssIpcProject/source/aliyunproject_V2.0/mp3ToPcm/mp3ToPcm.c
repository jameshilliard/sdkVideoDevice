#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/stat.h>  
#include <sys/mman.h>  
//#include <sys/soundcard.h>  
#include <sys/ioctl.h>  
#include <sys/fcntl.h>  
#include <sys/types.h>  
#include <mad.h>  
#include <alsa/asoundlib.h>
#include "../LogOut/LogOut.h"
#define SAMPLE_RATE 44100  
#define CHANNELS 2  
#define PCM_DEVICE "plughw:0,0"  
static snd_pcm_hw_params_t *hwparams = NULL;  
static snd_pcm_t *pcm_handle = NULL;  
struct buffer  
{  
    unsigned char const *start;  
   unsigned long length;  
};  
static int decode (unsigned char const *, unsigned long);  
//static int init_alsa ();  
FILE *outFile;
char *outPutBuf;
int g_outPutSize = 0;
//
int getPCM(unsigned char *inBuf, int inSize, unsigned char *outBuf, int *outSize)
{
	outPutBuf = NULL;
	g_outPutSize = 0;
	outPutBuf = (char*)malloc(inSize*8);
	decode(inBuf, inSize);
	if(outPutBuf != NULL)
	{
		*outSize = g_outPutSize;
		memcpy(outBuf, outPutBuf, g_outPutSize);
		free(outPutBuf);
	}
	else
	{
		return -1;
	}
	return g_outPutSize;
}
//
static enum mad_flow input (void *data, struct mad_stream *stream)  
{  
    struct buffer *buffer = data;  
    if (!buffer->length)  
       return MAD_FLOW_STOP;  
    mad_stream_buffer (stream, buffer->start, buffer->length);  
    buffer->length = 0;  
    return MAD_FLOW_CONTINUE;  
}  
/*这一段是处理采样后的pcm音频 */  
static inline signed int scale (mad_fixed_t sample)  
{  
    sample += (1L << (MAD_F_FRACBITS - 16));  
   if (sample >= MAD_F_ONE)  
       sample = MAD_F_ONE - 1;  
   else if (sample < -MAD_F_ONE)  
       sample = -MAD_F_ONE;  
   return sample >> (MAD_F_FRACBITS + 1 - 16);  
}

//
static enum mad_flow outputPCM (void *data, struct mad_header const *header, struct mad_pcm *pcm)  
{  
    unsigned int nchannels, nsamples, n;  
    mad_fixed_t const *left_ch, *right_ch;  
    unsigned char Output[6912], *OutputPtr;  
    int fmt, wrote, speed, exact_rate, err, dir;  
    nchannels = pcm->channels;  
    n = nsamples = pcm->length;  
    left_ch = pcm->samples[0];  
    right_ch = pcm->samples[1];  
//    fmt = AFMT_S16_LE;  
//    speed = pcm->samplerate * 2;        /*播放速度是采样率的两倍 */  
    OutputPtr = Output;//将OutputPtr指向Output  
    while (nsamples--)  
    {  
        signed int sample;  
        sample = scale (*left_ch++);  
        *(OutputPtr++) = sample >> 0;  
        *(OutputPtr++) = sample >> 8;  
        if (nchannels == 2)  
        {  
            sample = scale (*right_ch++);  
            *(OutputPtr++) = sample >> 0;  
            *(OutputPtr++) = sample >> 8;  
        }  
    }  
    OutputPtr = Output;//由于之前的操作将OutputPtr的指针指向了最后，这时需要将其指针移动到最前面  
   
    //fwrite(OutputPtr, 1, n*2*nchannels, outFile); 
    LOGOUT("mad_flow  n*2*nchannels:%d---", n*2*nchannels);
   // memset(outPutBuf+g_outPutSize, 0, n*2*nchannels);
	memcpy(outPutBuf+g_outPutSize, OutputPtr, n*2*nchannels);
	g_outPutSize += n*2*nchannels;
//    snd_pcm_writei (pcm_handle, OutputPtr, n);  
    OutputPtr = Output;//写完文件后，OutputPtr的指针也移动到了最后，这时需要将其指针移动到最前面  
    return MAD_FLOW_CONTINUE;  
}
//

static enum mad_flow output (void *data, struct mad_header const *header, struct mad_pcm *pcm)  
{  
    unsigned int nchannels, nsamples, n;  
    mad_fixed_t const *left_ch, *right_ch;  
    unsigned char Output[6912], *OutputPtr;  
    int fmt, wrote, speed, exact_rate, err, dir;  
    nchannels = pcm->channels;  
    n = nsamples = pcm->length;  
    left_ch = pcm->samples[0];  
    right_ch = pcm->samples[1];  
//    fmt = AFMT_S16_LE;  
//    speed = pcm->samplerate * 2;        /*播放速度是采样率的两倍 */  
    OutputPtr = Output;//将OutputPtr指向Output  
    while (nsamples--)  
    {  
        signed int sample;  
        sample = scale (*left_ch++);  
        *(OutputPtr++) = sample >> 0;  
        *(OutputPtr++) = sample >> 8;  
        if (nchannels == 2)  
        {  
            sample = scale (*right_ch++);  
            *(OutputPtr++) = sample >> 0;  
            *(OutputPtr++) = sample >> 8;  
        }  
    }  
    OutputPtr = Output;//由于之前的操作将OutputPtr的指针指向了最后，这时需要将其指针移动到最前面  
    fwrite(OutputPtr, 1, n*2*nchannels, outFile);  
//    snd_pcm_writei (pcm_handle, OutputPtr, n);  
    OutputPtr = Output;//写完文件后，OutputPtr的指针也移动到了最后，这时需要将其指针移动到最前面  
    return MAD_FLOW_CONTINUE;  
}  
static enum mad_flow error (void *data, struct mad_stream *stream, struct mad_frame *frame)  
{  
    return MAD_FLOW_CONTINUE;  
}  
static int decode (unsigned char const *start, unsigned long length)  
{  
    struct buffer buffer;  
    struct mad_decoder decoder;  
    int result;  
    buffer.start = start;  
    buffer.length = length;  
    mad_decoder_init (&decoder, &buffer, input, 0, 0, outputPCM, error, 0);  
    mad_decoder_options (&decoder, 0);  
    result = mad_decoder_run (&decoder, MAD_DECODER_MODE_SYNC);  
    mad_decoder_finish (&decoder);  
   return result;  
} 

