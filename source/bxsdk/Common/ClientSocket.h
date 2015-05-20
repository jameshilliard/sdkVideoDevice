#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "../Common/Typedef.h"
#include "../Common/InnerDataMng.h"


#define 	MAXSOCK 				120
#define 	SIZE_VIDEO_PACKED		300*1024
#define 	SOCKET_PARKET_SIZE	4096

typedef struct tagSocketData
{
	int 			socketId;
	InnerDataList *	pIInnerDataList;
}SocketData;

typedef enum {
	CMDCORRECT	 =	1,
	CMDRECONNECT  =	2,
}CmdNetStatus;

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int 	InitNetwork(int v_iCacheSize);
	int 	CreateConnect(LPCTSTR ip, int port,DWORD v_dwSpaceSize, DWORD v_dwInvalidPeriod);
	int 	SendSocketData(int socketId,char *buffer, DWORD length);
	int 	GetSocketData(int socketId,DWORD *v_dwSymb,DWORD v_dwBufSize,char *recBuffer,DWORD *length);
	int 	DelSocketId(int socketId);
	void 	ReleaseNetwork();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif
