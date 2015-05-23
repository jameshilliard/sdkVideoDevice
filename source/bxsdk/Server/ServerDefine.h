#ifndef _SERVERDEFINE_H_
#define _SERVERDEFINE_H_
#include "../Common/Typedef.h"
#include "../Common/Configdef.h"


#define PARAM_MAXUSER 				50
#define PARAM_MAXSECRET				32
#define PARAM_MAXSERVERNO			32 

#define MSG_HEAD_VERSION			0x1
#define MSG_HEAD_PLATFORM			0x7

#define SERVER_STATUS_OUTLINE			0
#define SERVER_STATUS_ROUTESERVER_OK	1
#define SERVER_STATUS_REGISTER_NO		2
#define SERVER_STATUS_REGISTER_OK		3

typedef enum {
	StatusIdle = 1,
	StatusConnRoute,
	StatusConnControl,
	StatusWaitConn,
	StatusWorking,
	StatusDisConn,
}SOCKET_WORK_STATUS;

typedef enum {
	LOGIN_ACK_SUCCESS=1,
	LOGIN_ACK_ERROR_USER=-1,
	LOGIN_ACK_ERROR_SECRET=-2,
	LOGIN_ACK_ERROR_SERVERNO=-3,
}MSG_LOGIN_ACK_STATUS;

typedef enum {
	MSG_ACK_ERROR_STATUS_PACKET=-1000,
}MSG_ACK_ERROR_STATUS;


typedef enum {
	ROUTE_PROTOCAL_GET_ROUTE						=	0x0010,	//��ȡ·������
	ROUTE_PROTOCAL_GET_ROUTE_ACK					=	0x0011,	//��ȡ·��Ӧ
}ROUTE_PROTOCAL_NUMBER;

#define 	MSGVERSION_0		0x0
#define 	MSGVERSION_1		0x1

typedef enum {
	CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY		=	0x3300,	//�����֤����
	CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY_ACK	=	0x3301,	//�����֤Ӧ��
	CONTROL_PROTOCAL_KEEPALIVE 					= 	0x0000,	//����
	CONTROL_PROTOCAL_KEEPALIVE_ACK 				= 	0x0001,	//����Ӧ��
	CONTROL_PROTOCAL_SETTIMEOUTDURATION			= 	0x0003,	//����������ʱʱ������
	CONTROL_PROTOCAL_SETTIMEOUTDURATION_ACK		= 	0x0004,//����������ʱʱ��Ӧ��,
	CONTROL_PROTOCAL_DEVICE_REGISTER           	= 	0x3303,//ע���豸����
	CONTROL_PROTOCAL_DEVICE_REGISTER_ACK        = 	0x3304,//ע���豸Ӧ��
	CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG      = 	0x3306,  //����ͨ����������
	CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG_ACK	= 	0x3307, //����ͨ������Ӧ��
	CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE		= 	0x3050,   //����ͨ����������
	CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE_ACK	= 	0x3051,  //����ͨ������Ӧ��
	CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE    = 	0x301C,   //��ȡ�������ò���������
	CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE_ACK=	0x301D,  //��ȡ�������ò�����Ӧ��,
	CONTROL_PROTOCAL_OPEN_CHANNEL_PREVIEW 		= 	0x3A01,   //����ͨ��Զ��Ԥ��֪ͨ
	CONTROL_PROTOCAL_CLOSE_CHANNEL_PREVIEW 		= 	0x3A03,  //�ر�ͨ��Զ��Ԥ��֪ͨ
	CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY		= 	0x3053,   //Զ��Ԥ��ʧ��֪ͨ
	CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY_ACK	= 	0x3054, //Զ��Ԥ��ʧ��֪ͨӦ��
	CONTROL_PROTOCAL_UPLOAD_CHANNEL_KEYFRAME 	= 	0x3A0C,   //�ϴ��ؼ�֪֡ͨ
	CONTROL_PROTOCAL_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG  	= 	0x3A26,//�л�ͨ����������֪ͨ
	CONTROL_PROTOCAL_OPEN_CHANNEL_TALK          = 	0x3110,   //����ͨ��Զ�̶Խ�֪ͨ
	CONTROL_PROTOCAL_OPEN_CHANNEL_TALK_ACK      = 	0x3111,  //����ͨ��Զ�̶Խ�Ӧ��
	CONTROL_PROTOCAL_CLOSE_CHANNEL_TALK       	= 	0x3A19,    //�ر�ͨ��Զ�̶Խ�֪ͨ
	CONTROL_PROTOCAL_TALK_FAILURE_NOTIFY        = 	0x3113,    //Զ�̶Խ�ʧ��֪ͨ
	CONTROL_PROTOCAL_TALK_FAILURE_NOTIFY_ACK    = 	0x3114,    //Զ�̶Խ�ʧ��֪ͨӦ��
	CONTROL_PROTOCAL_GET_CHANNEL_CONFIG         = 	0x3309,       //��ȡͨ������֪ͨ
	CONTROL_PROTOCAL_GET_CHANNEL_CONFIG_ACK     = 	0x330A,      //��ȡͨ������Ӧ��
	CONTROL_PROTOCAL_SET_CHANNEL_CONFIG       	= 	0x330C,      //�޸�ͨ������֪ͨ
	CONTROL_PROTOCAL_SET_CHANNEL_CONFIG_ACK  	= 	0x330D,     //�޸�ͨ������Ӧ��,
	CONTROL_PROTOCAL_CHANGE_CHANNEL_IMAGE_QUALITY				= 0x3069,  //�л�ͨ������֪ͨ
	CONTROL_PROTOCAL_CHANGE_CHANNEL_IMAGE_QUALITY_ACK			= 0x306A,  //�л�ͨ������֪ͨӦ��,
	CONTROL_PROTOCAL_COLLECT_YUNTAI_CONTROL     				= 0x3060, //��̨����
	CONTROL_PROTOCAL_COLLECT_YUNTAI_CONTROL_ACK 				= 0x3061,
	CONTROL_PROTOCAL_COLLECT_YUNTAI_CONTROL_ERROR_ACK 			= 0x3062,
	CONTROL_PROTOCAL_COLLECT_PRESET_CRUISE_CONTROL				= 0x3063, //Ԥ�õ㣬Ѳ��
	CONTROL_PROTOCAL_COLLECT_PRESET_CRUISE_CONTROL_ACK      	= 0x3064,
	CONTROL_PROTOCAL_COLLECT_PRESET_CRUISE_CONTROL_ERROR_ACK 	= 0x3065,
}CONTROL_PROTOCAL_NUMBER;

//��Ϣͷ
typedef struct MsgHead_
{
	unsigned short uHeadLen;//��Ϣͷ���ȣ�Ŀǰ��Ϣͷ���ȹ̶�Ϊ10���ֽڣ����ֶ�ΪԤ���ֶ�
	unsigned char  cMsgVersion;//��Ϣ�汾
	unsigned char  cPlatform;//ƽ̨���� �豸����"6"
	unsigned short uMsgType;//��Ϣ����
	unsigned short uMsgSeq;//��Ϣ���к�
	unsigned short uBodyLen;//��Ϣ�峤�ȣ��������Ϣ����0
}PACKED(1) MsgHead;





#endif

