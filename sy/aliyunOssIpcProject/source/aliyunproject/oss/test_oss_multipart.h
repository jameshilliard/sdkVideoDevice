#ifndef MUTIPART_H
#define MUTIPART_H
#include <stdio.h>

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int 	getFilePath(char *fullName, char *fileName);
	int		upLoadFile(char *filePath,char *fileName);
	int 	test_oss_local_file(char *object_name,char *data, int size, int *upLoadSize);
	int 	test_oss_local_from_buf(char *object_name,char *data, int dataSize, int filePos);
	void	test_append_object_from_buffer_zmt();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
