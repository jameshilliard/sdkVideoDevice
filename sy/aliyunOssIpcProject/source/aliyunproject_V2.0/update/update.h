#ifndef UPDATE_H
#define UPDATE_H
#include "../Common/Typedef.h"
#include "../Common/configdef.h"
typedef struct serviceversionInfo
{
	int result;//���ؽ��
	int imageFileBytes;//�̼�����С
	char SwVersion[128];//���°汾��
	char imageFileUrl[256];//��ŵ�URL��ַ
}SERVICEVERSION;//��������IPC���°汾��

int InitUpdate();


#endif