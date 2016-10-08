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



#define RTPSENDWAITACTIVETIME 25	// 没有收到心跳超时时间
#define RTPSENDSENDACTIVETIME 20	// 心跳发送时间



enum	 TCMDTYPE
{
	INVALID_MSG	= -1,
	MSG_CAPLOGIN = 0,						//客户端登陆
	MSG_VIEWLOGIN=1,						//观看端登陆
	MSG_LOGINRET,							//服务器返回
	MSG_SENDSWITCH,							//客户端与服务器发送开关
	MSG_KEEPALIVE,							//心跳保活检测
	MSG_PTZCOMMAND,							//云台控制
	MSG_SENDLOGFILE,						//发送日志文件
	MSG_PARAMCONFIG,						//参数配置
	MSG_GETPARAMCONFIG =8,					//获取参数
	MSG_AUDIODATA,							//音频数据
	MSG_AUDIODREQ,							//音频请求
	MSG_PLAYBACKREQ,						//录像回放请求
	MSG_PLAYBACKASK,						//录像回放应答
	MSG_PLAYBACKCTL,						//录像回放控制
	MSG_AUDIODATAGSM,						//音频数据GSM
	MSG_AUDIOREJECT,							//音频请求拒绝
	MSG_MIRRORINC,							//镜像连接++
	MSG_MIRRORDEC,							//镜像连接--
	MSG_MIRRORREQ,							//镜像服务器登录请求视频
	MSG_ViewOutTalk = 26,					//浏览端退出讲话状态
	MSG_ViewTalk = 32,						//通知采集端，浏览端正在讲话
	MSG_UPLOADCAPSTATUE = 35,                  //采集端异常状态上传（35）
	MSG_CMDMAX,	
};

enum	 TMSGRETTYPE						//返回内容
{
	INVALID_RETMSG	= -1,
	MSG_CAPLOGINSUCCESS = 0,	//客户端登陆成功
	MSG_VIEWLOGINSUCCESS = 1,	//观看端登陆成功
	MSG_CAPLOGINPSWRONG ,				//客户端登陆密码错误
	MSG_CAPLOGINHASONE ,				//客户端登陆已经有连接
	MSG_VIEWLOGINPSWRONG,				//观看端登陆失败
	MSG_VIEWLOGINOVER ,					//观看端超过人数
	MSG_AUDIOCONNECTED ,					//音频接入
	MSG_AUDIOREFUSED ,					//音频拒绝
	MSG_AUDIOQUIT ,					//音频退出
	MSG_AUDIOKICK,					//音频踢出
	MSG_RETMAX,

};


//转发发送给IPC的音频数据头
typedef struct
{
	int session;
	unsigned int ts;
	AUD_HDR_T header;
	int datalen;
	int dataType:1;		//为0
	int reserves:31;	//保留字段
}AUD_HDR_NEW;

typedef struct{
	//4字节
	DWORD	quality:6;		//质量
	DWORD	vs:4;			/*video size*/
	DWORD	dbQuality:4;		//双码流录像质量
	DWORD	nBitRate:12;		//码流
	DWORD	sxtType:4;		    //摄像头类型
	DWORD	bAudio:1;			/*音频开关,0,no,1,yes*/
	DWORD	bRecode:1;			/*是否录像,0,no,1,yes*/
	//4字节
	DWORD	version:12;		//版本号
	DWORD	keepDays:6;		//保存天数
	DWORD	comPort:4;		//串口号
	DWORD	fr:5;			/*frame rate*/
	DWORD	bDBRecode:1;			/*是否双码流录像,0,no,1,yes*/
	DWORD	bNetAdaptive:1;	//是否自适应码流
	DWORD	reserves:3;
}sParamConfig;

// 定义消息的结构体


// #pragma pack(1)										// 使结构体的数据按照1字节来对齐,省空间

struct TMSG_HEADER
{          
 	int     m_iXmlLen;					// 数据长度（xml类型+xml数据内容长度+结束符长度）
	char    m_bytXmlType;				// xml类型   
	UINT 	session;					//协议标识，在此为’TCMD’
	char    cMsgID;						// 消息标识
}PACKED(1);

//账号密码登陆认证 
//客户端登陆
struct TMSG_CAPLOGIN
{
    struct TMSG_HEADER header;
	char password[20];
	char SxtID[30];	
	char endChar;				// xml类型  
}PACKED(1);

//客户端与服务器开关发送数据
//开关数据发送
struct TMSG_SENDSWITCH
{
    struct TMSG_HEADER header;
	BOOL		m_nSwitch;	
}PACKED(1);


//心跳保活检测
struct TMSG_KEEPALIVE
{
     struct TMSG_HEADER header;
     char endChar;// xml类型  
}PACKED(1);

//服务器返回
struct TMSG_LOGINRET
{
    struct TMSG_HEADER header;
	int		m_nStatus;	
}PACKED(1);

//云台控制命令
struct TMSG_PTZCOMMAND
{
    struct TMSG_HEADER header;
	char	m_nFunc;				//功能
	char	m_nCtl;					//动作
	char	m_nSpeed;			//转速	
}PACKED(1);

//#pragma pack(push) //保存对齐状态
//#pragma pack(4)//设定为4字节对齐
struct TMSG_PARAMCONFIG
{
    struct TMSG_HEADER header;
    sParamConfig	param;
    char endChar;				
}PACKED(4);
//#pragma pack(pop)//恢复对齐状态

//#pragma pack()

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
    //外部使用的接口
	INT32S Init_Rtpsend(RtpSendInfo *param);

	void Release_Rtpsend(RtpSendInfo *param);

    //内部使用的接口　多线程安全
    INT32S isValidPacket(byte *ptr,INT32U size,INT32U *leftSize);
    INT32S RtpSendSendLoginData(INT32S iSocketID,const char *m_szCameraID);
    INT32S RtpSendSendKeepAlive(INT32S iSocketID,const char *m_szCameraID);
    INT32S ProcLoginRetMSG(struct TMSG_HEADER *pMsgHeader,INT32S iSocketID,const char *m_szCameraID);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif

