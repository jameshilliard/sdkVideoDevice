#ifndef __GLOBAFUNC_H_
#define __GLOBAFUNC_H_
#include "Typedef.h"
#include "../LogOut/LogOut.h"


#define VMRSS_LINE 		15//VMRSS������
#define PROCESS_ITEM 	14//����CPUʱ�俪ʼ������
typedef struct        //����һ��occupy�Ľṹ��
{
        unsigned int user;  //��ϵͳ������ʼ�ۼƵ���ǰʱ�̣������û�̬������ʱ�䣬������ niceֵΪ�����̡�
        unsigned int nice;  //��ϵͳ������ʼ�ۼƵ���ǰʱ�̣�niceֵΪ���Ľ�����ռ�õ�CPUʱ��
        unsigned int system;//��ϵͳ������ʼ�ۼƵ���ǰʱ�̣����ں���̬������ʱ��
        unsigned int idle;  //��ϵͳ������ʼ�ۼƵ���ǰʱ�̣���IO�ȴ�ʱ������������ȴ�ʱ��iowait (12256) ��ϵͳ������ʼ�ۼƵ���ǰʱ�̣�IO�ȴ�ʱ��(since 2.5.41)
}total_cpu_occupy_t;

typedef struct
{
    pid_t pid;//pid��
    unsigned int utime;  //���������û�̬���е�ʱ�䣬��λΪjiffies
    unsigned int stime;  //�������ں���̬���е�ʱ�䣬��λΪjiffies
    unsigned int cutime;//���������߳����û�̬���е�ʱ�䣬��λΪjiffies
    unsigned int cstime;  //���������ں���̬���е�ʱ�䣬��λΪjiffies
}process_cpu_occupy_t;

typedef size_t curlWriteData(void* buffer,size_t size,size_t nmemb,void *stream);

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	INT32S 	isDeviceAccess(const char *filePath);
	INT32S 	CreateConfigFile(char *v_szFilePath, char *v_szWriteBuf, int v_iLen);
	INT32S  isMp4File(LPCTSTR path);
	INT32S  ReadConfigFile(char *v_szFilePath, char *v_szReadBuf, int v_iBufLen);
	INT32U  getFreeMemory(void);
	DWORD 	getTickCountMs();
	DWORD 	getTickSecond();
	void 	squeeze(char *s,int maxSize,int c);
	DWORD	getFileSize(LPCTSTR path);
	INT32S  isFileSystemBigger(LPCTSTR sdDir,DWORD size);
	INT32S	creatDir(LPCTSTR pDir);
	INT32S 	split( char **szArray, char *v_szSplitStr, const char *v_szDdelim, int v_iArayNum);
	INT32S 	writeFile(LPCTSTR filePath,LPCTSTR fileBuffer,DWORD size);
	INT32S  writeFileWithFlag(LPCTSTR filePath,LPCTSTR fileBuffer,DWORD size,LPCTSTR flag);
	INT32S 	readFile(LPCTSTR filePath,LPCTSTR fileBuffer,DWORD bufferSize,DWORD *fileSize);
	char * 	SY_base64Encode(const char *text);
	INT32S 	readMediaFile(const char *pszDir,char fileName[MAX_PATH]);
	INT32S  rmDirFile(const char *pszDir);
	INT32S  GetProfileString(char *profile, char *AppName, char *KeyName, char *KeyVal);
	INT32S 	usSleep(long us);
	int 	find_pid_by_name( char* ProcName, int* foundpid,int num);
	int 	GetSendSocketTraffic(char* ProcName, unsigned long long* socketNums);
	float 	get_cpu_process_occupy_name(char* ProcName);
	float 	get_cpu_process_occupy_name2(char* ProcName);
	int 	postHttpServer(char *strUrl, char *strPost,char *strResponse,curlWriteData fuction);
	int 	getHttpServer(char *strUrl, char *strPost,char *strResponse,curlWriteData fuction);
	int 	calSecret(char uid[80],char passwd[80]);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif
