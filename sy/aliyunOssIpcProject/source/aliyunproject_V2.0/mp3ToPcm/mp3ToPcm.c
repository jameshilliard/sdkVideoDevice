#include "mp3ToPcm.h"
#include "../mp4v2/joseph_g711a_h264_to_mp4.h"

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */
static int mp3DecodeStatus=DECODENULL;
static int mp3DecodeControl=DECODENULL;
static char *outBuffer=NULL;
static char *outBufferG711=NULL;
static char dstPath[MAX_PATH]={0};


static enum mad_flow input(void *data,struct mad_stream *stream)
{
	mp3_file *mp3fp;
	int      ret_code;
	int      unproc_data_size;    /*the unprocessed data's size*/
	int      copy_size;

	mp3fp = (mp3_file *)data;
	if(mp3fp->fpos<mp3fp->flen)
	{
		unproc_data_size = stream->bufend - stream->next_frame;
		memcpy(mp3fp->fbuf, mp3fp->fbuf+mp3fp->fbsize-unproc_data_size, unproc_data_size);
		copy_size = BUFSIZE - unproc_data_size;
		if(mp3fp->fpos + copy_size > mp3fp->flen)
		{
		  copy_size = mp3fp->flen - mp3fp->fpos;
		}
		fread(mp3fp->fbuf+unproc_data_size, 1, copy_size, mp3fp->fp);
		mp3fp->fbsize = unproc_data_size + copy_size;
		mp3fp->fpos  += copy_size;

		/*Hand off the buffer to the mp3 input stream*/
		mad_stream_buffer(stream, mp3fp->fbuf, mp3fp->fbsize);
		ret_code = MAD_FLOW_CONTINUE;
	}
	else
	{
	  	ret_code = MAD_FLOW_STOP;
	}
	if(mp3DecodeControl==DECODESTOP)
	{
		LOGOUT("file %s decode mp3 is stop",mp3fp->filePath);
		return MAD_FLOW_STOP;
	}
	return ret_code;
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */

static inline signed int scale(mad_fixed_t sample)
{
	/* round */  
	sample += (1L << (MAD_F_FRACBITS - 16));  
	
	/* clip */	
	if (sample >= MAD_F_ONE)  
	  sample = MAD_F_ONE - 1;  
	else if (sample < -MAD_F_ONE)  
	  sample = -MAD_F_ONE;	
	
	/* quantize */	
	return sample >> (MAD_F_FRACBITS + 1 - 16);  
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */
static enum mad_flow output(void *data,struct mad_header const *header,struct mad_pcm *pcm)
{
	unsigned int nchannels, nsamples;  
	mad_fixed_t const *left_ch, *right_ch;	

	/* pcm->samplerate contains the sampling frequency */  

	nchannels = pcm->channels;	
	nsamples  = pcm->length;  
	left_ch   = pcm->samples[0];  
	right_ch  = pcm->samples[1];  
	char *ptrOutBuffer=outBuffer;
	int realCount=0;
	//printf("out lensth is %d\n",nsamples);
	while(nsamples--) 
	{  
	  signed int sample; 
	  /* output sample(s) in 16-bit signed little-endian PCM */  
	  sample = scale(*left_ch++); 
	  //if(!(nsamples%11==5 || nsamples%11==10))
	  //	continue;
	  *ptrOutBuffer++=(char)((sample >> 0) & 0xff);
	  *ptrOutBuffer++=(char)((sample >> 8) & 0xff);
	  realCount++;
	  //putchar((sample >> 0) & 0xff);  
	  //putchar((sample >> 8) & 0xff);  
	} 
	int outLength=10*1024;
	outLength=g711a_encode(outBufferG711,&outLength,outBuffer,realCount*2);
	writeFileWithFlag(dstPath,outBufferG711,outLength,"a+");
	if(mp3DecodeControl==DECODESTOP)
	{
		LOGOUT("file output decode mp3 is stop");
		return MAD_FLOW_STOP;
	}
	return MAD_FLOW_CONTINUE; 
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */

static enum mad_flow error(void *data,
		    struct mad_stream *stream,
		    struct mad_frame *frame)
{
	mp3_file *mp3fp = data;
	LOGOUT("decoding error 0x%04x (%s) at byte offset %u\n",stream->error,mad_stream_errorstr(stream),stream->this_frame - mp3fp->fbuf);
	if(mp3DecodeControl==DECODESTOP)
	{ 
		LOGOUT("file %s decode mp3 is stop",mp3fp->filePath);
		return MAD_FLOW_STOP;
	}					 									   
	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
	return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */

static int decode(mp3_file *mp3fp)
{
	struct mad_decoder decoder;
	int result;
	/* configure input, output, and error functions */
	mad_decoder_init(&decoder, mp3fp,
	   input, 0 /* header */, 0 /* filter */, output,
	   error, 0 /* message */);
	/* start decoding */
	result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
	/* release the decoder */
	mad_decoder_finish(&decoder);
	return result;
}

int controlMp3Decode(int type)
{ 
	mp3DecodeControl=type;
}
int getMP3DecodeStatus()
{
	return mp3DecodeStatus;
}
int playMp3File(const char *stMp3Path,const char *stDstPath)
{
	long flen, fsta, fend;
	int  dlen;
	mp3_file mp3fp;
	if(stMp3Path==NULL || stDstPath==NULL)
	{
		LOGOUT("param is error");
		return -3;
	}
	if(strlen(stMp3Path)==0 || strlen(stDstPath)==0)
	{
		LOGOUT("param length is error");
		return -4;
	}
	if(strlen(stDstPath)<sizeof(dstPath))
	{
		memset(dstPath,0,sizeof(dstPath));
		strcpy(dstPath,stDstPath);
	}
	else
	{
		LOGOUT("param is error");
		return -5;
	}
	if((mp3fp.fp = fopen(stMp3Path, "r")) == NULL)
	{
		LOGOUT("can't open source file");
		return -1;
	}
	fsta = ftell(mp3fp.fp);
	fseek(mp3fp.fp, 0, SEEK_END);
	fend = ftell(mp3fp.fp);
	flen = fend - fsta;
	if(flen<=0)
	{
		LOGOUT("file is null");
		return -2;
	}
	fseek(mp3fp.fp, 0, SEEK_SET);
	fread(mp3fp.fbuf, 1, BUFSIZE, mp3fp.fp);
	mp3fp.fbsize = BUFSIZE;
	mp3fp.fpos   = BUFSIZE;
	mp3fp.flen   = flen;
	memset(mp3fp.filePath,0,sizeof(mp3fp.filePath));
	int length=strlen(stMp3Path);
	if(length>sizeof(mp3fp.filePath))
		length=sizeof(mp3fp.filePath);
	memcpy(mp3fp.filePath,stMp3Path,length);
	LOGOUT("decode start %s",mp3fp.filePath);
	mp3DecodeStatus=DECODERUN;
	mp3DecodeControl=DECODERUN;
	decode(&mp3fp);
	mp3DecodeStatus=DECODESTOP;
	mp3DecodeControl=DECODESTOP;
	LOGOUT("decode over %s",mp3fp.filePath);
	fclose(mp3fp.fp);
	return 0;
}

int InitMp3Decode()
{
	mp3DecodeStatus=DECODENULL;
	mp3DecodeControl=DECODENULL;
	outBuffer=malloc(DECODEBUFSIZE);
	if(!outBuffer)
	{
		LOGOUT("malloc mp3 outBuffer %d failure",DECODEBUFSIZE);
		return -1;
	}
	outBufferG711=malloc(DECODEBUFSIZE);
	if(!outBufferG711)
	{
		LOGOUT("malloc mp3 outBufferG711 %d failure",DECODEBUFSIZE);
		return -2;
	}
	LOGOUT("InitMp3Decode success");
	return 0;

}
int ReleaseMp3Decode()
{
	if(outBuffer)
	{
		free(outBuffer);
	}
	if(outBufferG711)
	{
		free(outBufferG711);
	}
	mp3DecodeStatus=DECODENULL;
	mp3DecodeControl=DECODENULL;
	LOGOUT("ReleaseMp3Decode success");
}

