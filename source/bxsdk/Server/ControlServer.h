#ifndef _CONTROLSERVER_H_
#define _CONTROLSERVER_H_
#include "../Common/Typedef.h"
#include "../hisdk/hi_sdk.h"
#include "ServerDefine.h"

#define 	BEATTIME				30*1000
#define		BEATSERVERTIMEOUT		60
#define		BEATTIMEOUT				(BEATSERVERTIMEOUT*1000*2+5*1000)

#define 	PARAMNUM				32
#define 	PARAMSIZE				64
#define 	ARRAYNUM				32
#define 	PARAMSTARTSYMBOL		'['
#define 	PARAMOVERSYMBOL			']'
#define 	PARAMMIDSTARTSYMBOL		'{'
#define	 	PARAMMIDOVERSYMBOL    	'}'
#define 	PARAMEQUALSYMBOL		':'

typedef struct String_Param_
{
	unsigned int 	m_validParamNum;
	LPCTSTR 		m_StartAddr;
	LPCTSTR 		m_OverAddr;
	LPCTSTR 		m_paramStartAddr[PARAMNUM];
	LPCTSTR 		m_paramOverAddr[PARAMNUM];
}String_Param;

typedef struct String_Array_Param_
{
	unsigned int 		m_validArrayParamNum;
	String_Param		m_stringArrayParam[ARRAYNUM];
}String_Array_Param;

typedef struct MsgBody_DEVICE_IDENTITY_VERIFY_
{
	unsigned int 	m_uMsgType;//��Ϣ����
	LPCTSTR 		m_user;
	LPCTSTR 		m_secret;
	LPCTSTR 		m_serverNo;
}MsgBody_DEVICE_IDENTITY_VERIFY;

//[�豸Ʒ��:�豸�ͺ�:�豸���к�:�豸����:
//�豸MAC:�豸ͨ����:ͨ������ʼλ]
//[����:9100:SN111111:IPC:90-2B-34-4E-FC-FE:1:1]
typedef struct MsgBody_DEVICE_REGISTER_
{
	unsigned int 	m_uMsgType;//��Ϣ����
	LPCTSTR 		m_deviceProduct;
	LPCTSTR 		m_deviceModel;
	LPCTSTR 		m_deviceServerNo;
	LPCTSTR 		m_deviceType;
	LPCTSTR 		m_deviceMacAddr;
	unsigned int 	m_deviceChannelNum;
	unsigned int	m_deviceChannelStartNum;
}MsgBody_DEVICE_REGISTER;


//״̬��0 ע��ʧ��
//	   1 ע��ɹ�
//	  -1 �豸��֧��
typedef struct MsgBody_DEVICE_REGISTER_ACK_
{
	int 			m_registerStatus; 
	unsigned int 	m_channelId;//��Ϣ����
	unsigned int 	m_channelNum;
	unsigned int 	m_isValid;
}MsgBody_DEVICE_REGISTER_ACK;

//���뷽ʽ��0�����ʣ� 1������
//���ʵȼ�: 0������1��׼��2����
//����ȼ���0�ͣ�1�У�2��
//�Ƿ񲼷���0������1����

//[IP:�˿�:·��:ͨ��ID:UUID:��:��] 
//[112.124.113.127:1935:live:100:benxun123456:1080:720]
typedef struct MsgBody_OPEN_CHANNEL_PREVIEW_
{
	unsigned int 	m_channelId;
	char  			m_rtmpIp[PARAMSIZE];//��Ϣ����
	unsigned int 	m_rtmpPort;	
	char 			m_rtmpPath[PARAMSIZE];	
	char 			m_rtmpUUID[PARAMSIZE];	
	HI_S_Video_Ext    m_video;
	char 			m_serverStamp[PARAMSIZE];
	unsigned int 	m_isValid;
}MsgBody_OPEN_CHANNEL_PREVIEW_ACK;


typedef struct MsgBody_PREVIEW_FAILURE_NOTIFY_
{
	unsigned int 	m_uMsgType;//��Ϣ����
	unsigned int 	m_channelId;
	int 			m_failureType;//-1 ʧ��-2 ��������ʧ�ܣ�����֧�֣�
	char 			m_serverStamp[PARAMSIZE];		
}MsgBody_PREVIEW_FAILURE_NOTIFY;


typedef struct MsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG_
{
	unsigned int 	m_channelId;
	HI_S_Video_Ext  m_video;
	unsigned int 	m_isValid;
}MsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG;


typedef struct MsgBody_SETTIMEOUTDURATION_
{
	unsigned int 	m_uMsgType;//��Ϣ����
	unsigned int 	m_durationSecond;
}MsgBody_SETTIMEOUTDURATION;

typedef struct MsgBody_GET_ENCODE_CONFIG_TABLE_
{
	unsigned int 	m_uMsgType;//��Ϣ����
	long long 		m_timeStamp;
}MsgBody_GET_ENCODE_CONFIG_TABLE;

typedef struct MsgBody_PUBLISH_CHANNEL_STATE_
{
	unsigned int 	m_uMsgType;//��Ϣ����
	unsigned int 	m_channelId;
	unsigned int 	m_liveStatus;  //0���ߣ�1����
}MsgBody_PUBLISH_CHANNEL_STATE;


typedef struct MsgBody_UPDATE_CHANNEL_CONFIG_
{
	unsigned int   	m_uMsgType;		//��Ϣ����
	unsigned int   	m_channelId;
	unsigned int 	m_channelNumber;
	unsigned int 	m_imageLevel;   	//���ʵȼ�: 0������1��׼��2����
	unsigned int 	m_armingSwitch;	//�������أ�0������1����
	unsigned int 	m_isValid;
}MsgBody_UPDATE_CHANNEL_CONFIG;


typedef struct Device_Net_Info_t_
{
	char	m_MACAddress[32];	//MAC��ַ
	char	m_IPAddress[32];	//��������IP��ַ
	char	m_Subnet[32];		//��������Mask
	char	m_GateWay[32];	//Ĭ�����ػ�·������ַ
	char 	m_BcastAddr[32];
}Device_Net_Info;

typedef struct MsgGlobal_Device_
{
	MsgBody_DEVICE_REGISTER_ACK 		m_deviceRegisterAck;
	MsgBody_OPEN_CHANNEL_PREVIEW_ACK 	m_openChannelPreviewAck;
}MsgGlobal_Device;


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int InitControlServer();
	int ReleaseControlServer();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif

