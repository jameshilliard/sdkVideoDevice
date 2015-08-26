#ifndef MUTIPART_H
#define MUTIPART_H
#include <stdio.h>
typedef unsigned char bool;
enum {false, true};

enum VIDEOTYPE
{
	VIDEO	= 0,
	PHOTOS = 1,	
};


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int 	getFilePath(char *fullName, char *fileName);
	bool	upLoadFile(char *fileName);
	int 	test_oss_local_file(char *object_name,char *data, int size, int *upLoadSize);
	int 	test_oss_local_from_buf(char *object_name,char *data, int dataSize, int filePos);
	void	test_append_object_from_buffer_zmt();
	int 	initAliyunTask();
	int 	ReleaseAliyunTask();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
