#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "../Common/Typedef.h"
#include "../Common/InnerDataMng.h"
#include "Queue.h"


#define 	MAXSOCK 				120
#define 	SIZE_VIDEO_PACKED		300*1024
#define 	SOCKET_PARKET_SIZE	4096
#define 	MAXSOCK 				120
#define 	SOCKET_PARKET_SIZE		4096

#define 	CORRECT 				1  //接收正常
#define 	RECONNECT 				2  //发送错误请求重连?
//

// 来着控制的消息
#define 	CTRL_MSG_SENDCAMINFO	0x20		// 发送摄像头信息
#define 	CTRL_MSG_DELETECAMINFO	0x21		// 删除摄像头信息
#define 	CTRL_MSG_MODIFYCAMINFO	0x22		// 修改摄像头信息

//回放控制指令
#define 	REPLAY_MSG_CONNECT		0x31		// 回放转发进行连接
#define 	REPLAY_MSG_DISCONNECT	0x32		// 回放转发断开连接
#define 	REPLAY_MSG_INFO			0x33		// 回放转发控制消息

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
	Queue * GetCache();
	int SendBufferData(int socketId,DWORD v_dwSymb,char *v_pDataBuf,DWORD v_dwDataLen);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif
