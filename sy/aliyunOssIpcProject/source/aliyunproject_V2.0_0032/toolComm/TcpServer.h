#ifndef _SY_TCPSERVER_H_
#define _SY_TCPSERVER_H_

#include "../Common/Typedef.h"
#include "../Common/InnerDataMng.h"
#include "../mxml/mxmlparser.h"
#include "../Common/ConfigMng.h"
#include "../Common/GlobFunc.h"
#include "../Common/DeviceConfig.h"
#include "../Common/GlobVariable.h"


#define SERVER_PORT 		60025
#define BUFFLEN 			64*1024
#define BACKLOG 			5
#define CLIENTNUM 			32
#define GETFILELEN 			192*1024 // ����͵��ļ����ݳ��� 192*1024
#define UPLOADLOGTIMEOUT	30000
#define TCPXMLDATALEN 		65*1024

enum TCPSERVER_WORK_STA
{
	T_TcpIdle = 1,
	T_TcpConn,
	T_TcpAccepting,
	T_TcpDisConn
};

typedef struct TcpXmlData_
{
	int m_iXmlLen;						// ���ݳ��ȣ�xml����+xml�������ݳ���+���������ȣ�
	byte m_bytXmlType;					// xml����
	char szXmlDataBuf[TCPXMLDATALEN];	// xml��������
}TcpXmlData;

// ��������Ϣ�ṹ��
typedef struct TcpRecvPacketBuf_
{
	BOOL m_bFirstPacket;//TRUE,��һ����FALSE,����
	int m_iTotalLen;
	int m_iCurLen;
	char *m_pRecvBuf;
}TcpRecvPacketBuf;

typedef struct stDownFileInfo_
{
	int m_iDownLoadType;		// 0���ںˣ�1��Ӧ��
	int m_iFileSize;
	int m_iTotalPacket;
	BOOL m_bUpdateFile;
	FILE *m_FpOpen;
}stDownFileInfo;

typedef struct UploadLogInfo_
{
	int m_iSocket;
	int m_iType;	// ���ز�ͬ���͵��ļ�,0x02����־�ļ���0x03������
	char m_szTarLogPath[256];
}UploadLogInfo;

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int InitTcpServer();
	void TcpFreeMemory();
	void ReleaseTcpServer();
	//void TcpSendCmdData(int v_iSocket, S_Data *v_stData);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif