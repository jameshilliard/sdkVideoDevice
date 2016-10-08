#ifndef _RTPSEND_H_
#define _RTPSEND_H_

#include "../Common/Typedef.h"
#include "../Common/GlobFunc.h"
#include "../mxml/mxmlparser.h"
#include "../Common/ConfigMng.h"
#include "../LogOut/LogOut.h"
#include "../Common/Typedef.h"
#include "rtpManager.h"


#define MSG_SENDSIZE	2048 
#define MSG_RECVSIZE	2048
#define MSG_SIGNALSIZE  2048
 
#define PACKET_CMD      1 
#define PACKET_ERROR   -1



#define RTPSENDWAITACTIVETIME 25	// û���յ�������ʱʱ��
#define RTPSENDSENDACTIVETIME 20	// ��������ʱ��



enum	 TCMDTYPE
{
	INVALID_MSG	= -1,
	MSG_CAPLOGIN = 0,						//�ͻ��˵�½
	MSG_VIEWLOGIN=1,						//�ۿ��˵�½
	MSG_LOGINRET,							//����������
	MSG_SENDSWITCH,							//�ͻ�������������Ϳ���
	MSG_KEEPALIVE,							//����������
	MSG_PTZCOMMAND,							//��̨����
	MSG_SENDLOGFILE,						//������־�ļ�
	MSG_PARAMCONFIG,						//��������
	MSG_GETPARAMCONFIG =8,					//��ȡ����
	MSG_AUDIODATA,							//��Ƶ����
	MSG_AUDIODREQ,							//��Ƶ����
	MSG_PLAYBACKREQ,						//¼��ط�����
	MSG_PLAYBACKASK,						//¼��ط�Ӧ��
	MSG_PLAYBACKCTL,						//¼��طſ���
	MSG_AUDIODATAGSM,						//��Ƶ����GSM
	MSG_AUDIOREJECT,							//��Ƶ����ܾ�
	MSG_MIRRORINC,							//��������++
	MSG_MIRRORDEC,							//��������--
	MSG_MIRRORREQ,							//�����������¼������Ƶ
	MSG_ViewOutTalk = 26,					//������˳�����״̬
	MSG_ViewTalk = 32,						//֪ͨ�ɼ��ˣ���������ڽ���
	MSG_UPLOADCAPSTATUE = 35,                  //�ɼ����쳣״̬�ϴ���35��
	MSG_CMDMAX,	
};

enum	 TMSGRETTYPE						//��������
{
	INVALID_RETMSG	= -1,
	MSG_CAPLOGINSUCCESS = 0,	//�ͻ��˵�½�ɹ�
	MSG_VIEWLOGINSUCCESS = 1,	//�ۿ��˵�½�ɹ�
	MSG_CAPLOGINPSWRONG ,				//�ͻ��˵�½�������
	MSG_CAPLOGINHASONE ,				//�ͻ��˵�½�Ѿ�������
	MSG_VIEWLOGINPSWRONG,				//�ۿ��˵�½ʧ��
	MSG_VIEWLOGINOVER ,					//�ۿ��˳�������
	MSG_AUDIOCONNECTED ,					//��Ƶ����
	MSG_AUDIOREFUSED ,					//��Ƶ�ܾ�
	MSG_AUDIOQUIT ,					//��Ƶ�˳�
	MSG_AUDIOKICK,					//��Ƶ�߳�
	MSG_RETMAX,

};


//ת�����͸�IPC����Ƶ����ͷ
typedef struct
{
	int session;
	unsigned int ts;
	AUD_HDR_T header;
	int datalen;
	int dataType:1;		//Ϊ0
	int reserves:31;	//�����ֶ�
}AUD_HDR_NEW;

typedef struct{
	//4�ֽ�
	DWORD	quality:6;		//����
	DWORD	vs:4;			/*video size*/
	DWORD	dbQuality:4;		//˫����¼������
	DWORD	nBitRate:12;		//����
	DWORD	sxtType:4;		    //����ͷ����
	DWORD	bAudio:1;			/*��Ƶ����,0,no,1,yes*/
	DWORD	bRecode:1;			/*�Ƿ�¼��,0,no,1,yes*/
	//4�ֽ�
	DWORD	version:12;		//�汾��
	DWORD	keepDays:6;		//��������
	DWORD	comPort:4;		//���ں�
	DWORD	fr:5;			/*frame rate*/
	DWORD	bDBRecode:1;			/*�Ƿ�˫����¼��,0,no,1,yes*/
	DWORD	bNetAdaptive:1;	//�Ƿ�����Ӧ����
	DWORD	reserves:3;
}sParamConfig;

// ������Ϣ�Ľṹ��


// #pragma pack(1)										// ʹ�ṹ������ݰ���1�ֽ�������,ʡ�ռ�

struct TMSG_HEADER
{          
 	int     m_iXmlLen;					// ���ݳ��ȣ�xml����+xml�������ݳ���+���������ȣ�
	char    m_bytXmlType;				// xml����   
	UINT 	session;					//Э���ʶ���ڴ�Ϊ��TCMD��
	char    cMsgID;						// ��Ϣ��ʶ
}PACKED(1);

//�˺������½��֤ 
//�ͻ��˵�½
struct TMSG_CAPLOGIN
{
    struct TMSG_HEADER header;
	char password[20];
	char SxtID[30];	
	char endChar;				// xml����  
}PACKED(1);

//�ͻ�������������ط�������
//�������ݷ���
struct TMSG_SENDSWITCH
{
    struct TMSG_HEADER header;
	BOOL		m_nSwitch;	
}PACKED(1);


//����������
struct TMSG_KEEPALIVE
{
     struct TMSG_HEADER header;
     char endChar;// xml����  
}PACKED(1);

//����������
struct TMSG_LOGINRET
{
    struct TMSG_HEADER header;
	int		m_nStatus;	
}PACKED(1);

//��̨��������
struct TMSG_PTZCOMMAND
{
    struct TMSG_HEADER header;
	char	m_nFunc;				//����
	char	m_nCtl;					//����
	char	m_nSpeed;			//ת��	
}PACKED(1);

//#pragma pack(push) //�������״̬
//#pragma pack(4)//�趨Ϊ4�ֽڶ���
struct TMSG_PARAMCONFIG
{
    struct TMSG_HEADER header;
    sParamConfig	param;
    char endChar;				
}PACKED(4);
//#pragma pack(pop)//�ָ�����״̬

//#pragma pack()

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
    //�ⲿʹ�õĽӿ�
	INT32S Init_Rtpsend(RtpSendInfo *param);

	void Release_Rtpsend(RtpSendInfo *param);

    //�ڲ�ʹ�õĽӿڡ����̰߳�ȫ
    INT32S isValidPacket(byte *ptr,INT32U size,INT32U *leftSize);
    INT32S RtpSendSendLoginData(INT32S iSocketID,const char *m_szCameraID);
    INT32S RtpSendSendKeepAlive(INT32S iSocketID,const char *m_szCameraID);
    INT32S ProcLoginRetMSG(struct TMSG_HEADER *pMsgHeader,INT32S iSocketID,const char *m_szCameraID);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif

