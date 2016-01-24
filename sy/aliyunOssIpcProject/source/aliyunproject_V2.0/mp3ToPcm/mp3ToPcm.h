#ifndef _MP3TOPCM_H_
#define _MP3TOPCM_H_
#include "mad.h" 
#include "../LogOut/LogOut.h"


#define BUFSIZE 8192
enum
{
	DECODENULL=0,
	DECODERUN=1,
	DECODESTOP=2,
};

/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */
struct buffer 
{
	FILE  *fp;                    /*file pointer*/
	char  *filePath[256];
	unsigned int  flen;           /*file length*/
	unsigned int  fpos;           /*current position*/
	unsigned char fbuf[BUFSIZE];  /*buffer*/
	unsigned int  fbsize;         /*indeed size of buffer*/
};
typedef struct buffer mp3_file;

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int controlMp3Decode(int type);
	int getMP3DecodeStatus();
	int playMp3File(const char *stMp3Path);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif

