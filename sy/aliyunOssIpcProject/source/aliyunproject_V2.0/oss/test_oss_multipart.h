#ifndef MUTIPART_H
#define MUTIPART_H
#include <stdio.h>


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int		upLoadFile(char *filePath,char *fileName);
	int 	delete_object(char *object_name);
	void	test_append_object_from_buffer_zmt();
	int 	InitOSSConfig(const char *v_szBucketName,const char *v_szOssEndPoint,
						  const char *v_szAccessKeyId,const char *v_szAccessKeySecret);
	int 	ReleaseOSSConfig();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
