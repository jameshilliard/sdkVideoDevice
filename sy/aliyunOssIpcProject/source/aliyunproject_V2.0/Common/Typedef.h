#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/statfs.h>
#include <linux/unistd.h>     /* 包含调用 _syscallX 宏等相关信息*/
#include <linux/kernel.h>     /* 包含sysinfo结构体信息*/
#include <sys/sysinfo.h>


typedef unsigned char	uchar;
typedef unsigned char	byte;
typedef unsigned char	BOOLEAN;
typedef unsigned char	BYTE;
typedef unsigned char	INT8U;
typedef signed 	 char	INT8S;
typedef unsigned char	UCHAR;
typedef char			SCHAR;

typedef unsigned short	ushort;
typedef	unsigned short	WORD;
typedef unsigned short	INT16U;
typedef signed 	 short	INT16S;	

typedef unsigned int	uint;
typedef unsigned int	UINT;
typedef unsigned int	INT32U;
typedef signed   int	INT32S;
typedef long long		INT64S;
typedef long long		__int64;

typedef unsigned long	DWORD;
typedef signed 	 long	SLONG;
typedef unsigned long	ULONG;

typedef UINT			WPARAM;
typedef INT32S			LPARAM;
typedef int				HANDLE;
typedef void *			LPVOID;
typedef const 	void*	LPCVOID;
typedef const 	char*	LPCTSTR;
typedef char*			LPTSTR;


#define PACKED(n) __attribute__( ( packed, aligned(n) ) )

typedef enum {
	FALSE	= 0,
	TRUE	= 1,
}BOOL;

enum ERR_CODE
{
	ERR_QUEUE_EMPTY = 1,
	ERR_QUEUE_FULL,
	ERR_QUEUE,
	ERR_QUEUE_TOOSMALL,
	ERR_QUEUE_SYMBNOFIND,
	ERR_MEMLACK,
	ERR_SPACELACK,
	ERR_SHMGET,
	ERR_SHMAT,
	ERR_PAR,
	ERR_CFG,
	ERR_THREAD,
};

#define XMLDATALEN 	4096
#define MAX_PATH	256
typedef struct XmlData_
{
	int m_iXmlLen;					// 数据长度（xml类型+xml数据内容长度+结束符长度）
	byte m_bytXmlType;				// xml类型
	char szXmlDataBuf[XMLDATALEN];	// xml数据内容
}XmlData;
//
typedef struct 
{
	int videoMotionTotal_Dist1; 
	int videoMotionTotal_Dist2;
	int videoMotionTotal_Dist3; 
	int videoMotionTotal_Dist4;
	int voiceAlarmTotal;
	int videoMotionScoreFromIPC;
	int videoSoundScoreFromIPC;
}Motion_Data;
//
/*
typedef struct 
{
	int videoMotionTotal_Dist1; 
	int videoMotionTotal_Dist2;
	int videoMotionTotal_Dist3; 
	int videoMotionTotal_Dist4;
	int voiceAlarmTotal;
}Motion_Data;
*/
#define MIN(a,b) ((a)<(b))?(a):(b)
#define MAX(a,b) ((a)>(b))?(a):(b)


#endif