#ifndef	__ALIYUNOSS_TASK_H_
#define	__ALIYUNOSS_TASK_H_

#ifndef BUG_CPU_X86

#include <stdio.h>
#include <unistd.h>
#include "test_oss_multipart.h"



#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int initAliyunOssTask();
	int ReleaseAliyunOssTask();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
#endif

