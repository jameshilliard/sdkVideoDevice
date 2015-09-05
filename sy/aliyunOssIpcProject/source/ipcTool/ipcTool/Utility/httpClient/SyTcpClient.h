#pragma once
#include <map>
#include <afxinet.h>
#include <windows.h>
#include "../xml/tinyxml/xmlparser.h"
using namespace std;

// devtool��xml����
#define CONNETTYPE 	"View-DeviceTool"

#define SERVER_PORT 60025

#define GETFILELEN  65536	// 64*1024 �ļ����ͳ��ȣ�64K�����������С����Ӧ���豸�Ĵ�СҲҪ����

enum ERR_CODE
{
	TCP_SUCC = 0,
	TCP_CONNECTSERFAIL,
	TCP_OPENFILEFAIL,
	TCP_FILEERR,
	TCP_SENDFAIL,
	TCP_RECVFAIL,
	TCP_UPLOADFILEINFOFAIL,
	TCP_UPDATAFAIL
};

#define MAXDATALEN 256*1024
#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���
typedef struct DataBuf_
{
	int m_iDataLen;
	BYTE m_bytDataType;
	char szRecvBuf[MAXDATALEN+1024];
}stDataBuf;


typedef struct LogInfoCmd_
{
	BYTE m_bytType;
	BYTE m_bytAck;
	int m_iFileSize;
	int m_iTotalPacket;
}stLogInfoCmd;

typedef struct LogContent_
{
	BYTE m_bytType;
	int m_iPacketNum;
	int m_iPacketSize;
}stLogContent;

#pragma pack(pop)//�ָ�����״̬

class CSyTcpClient
{
public:
	CSyTcpClient(void);
	~CSyTcpClient(void);
	

	/**********************ʵ���ڲ��ӿ�*****************/
public: 
	void InitTcpSocket(CString strServer,int port);
	bool TcpPost(map<CString,CString>& mapofParam, map<CString,CString>& mapofResult, int v_iFunName);
	void XmlEncode(CString &v_strXmlData, map<CString,CString>& mapofParam, int v_iFunName);
	bool XmlDecode(map<CString,CString>& mapofParam, char *v_szXmlData);
	
	SOCKET ConnectToServer();
	bool SendDataToServer(SOCKET v_sSocketClient, map<CString,CString>& mapofParam, int v_iFunName);
	bool RecvXmlDataFromServer(SOCKET v_sSocketClient, map<CString,CString>& mapofResult);
	bool RecvDataFrmServer(SOCKET v_sSocketClient, char *v_szRecvData, int v_iBufLen, int &v_iRecvLen);
	bool CloseSocket(SOCKET v_sSocketClient);
	
	int TcpUploadFileInfo(CString v_strUploadFileName, CString v_strUpType);
	int TcpUploadFile(CString v_strUploadFileName);
	CString GetErrMessage(int v_iType);
	bool DownloadFile(CString strSaveFileName, int v_iType);

private:
	CString m_strSerIp;
	int m_iPort;
	SOCKET m_socketClient;

};