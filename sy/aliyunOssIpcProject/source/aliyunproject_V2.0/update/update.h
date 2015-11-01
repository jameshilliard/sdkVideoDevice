#ifndef UPDATE_H
#define UPDATE_H
#include "../Common/Typedef.h"
#include "../Common/configdef.h"
typedef struct serviceversionInfo
{
	int result;//返回结果
	int imageFileBytes;//固件包大小
	char SwVersion[128];//最新版本号
	char imageFileUrl[256];//存放的URL地址
}SERVICEVERSION;//服务器返IPC最新版本号

int InitUpdate();


#endif