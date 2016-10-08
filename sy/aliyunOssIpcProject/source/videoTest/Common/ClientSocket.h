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

#define 	CORRECT 				1  //��������
#define 	RECONNECT 				2  //���ʹ�����������?
//

// ���ſ��Ƶ���Ϣ
#define 	CTRL_MSG_SENDCAMINFO	0x20		// ��������ͷ��Ϣ
#define 	CTRL_MSG_DELETECAMINFO	0x21		// ɾ������ͷ��Ϣ
#define 	CTRL_MSG_MODIFYCAMINFO	0x22		// �޸�����ͷ��Ϣ

//�طſ���ָ��
#define 	REPLAY_MSG_CONNECT		0x31		// �ط�ת����������
#define 	REPLAY_MSG_DISCONNECT	0x32		// �ط�ת���Ͽ�����
#define 	REPLAY_MSG_INFO			0x33		// �ط�ת��������Ϣ

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
