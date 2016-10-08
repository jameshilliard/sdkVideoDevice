#ifndef _RTPMANAGER_H_
#define _RTPMANAGER_H_

#include "../common/Typedef.h"
#include "../common/Configdef.h"
#include "../common/GlobFunc.h"
#include "../common/ClientSocket.h"
#include "../LogOut/LogOut.h"


enum SOCK_WORK_STA
{
	T_Idle = 1,
	T_Conn,
	T_WaitConn,
	T_Working,
	T_DisConn
};

#define			SENDSWITCHOFF					0
#define			SENDSWITCHON					1
#define			CMDSESSION						'TCMD'
#define			REMOTESERSESSION				'TRSV'//转发服务器发来的连接标志
#define  		YRVIDEO					  		'VDYR'
#define  		YRAUIDO				  			'AUYR'
#define  		SZYAUDIO				  		'AUSZ'
#define         SZYAUDIO_EX			            'AUEX'
#define  		SZYFILE					  		'FILE'

#define			CMDTYPE							1
#define			DATATYPE						2
#define 		DATA_TOKEN						"<!ROW>"

//音频
#define SIZE_AUDIO_PACKED  60*30
#define SIZE_AUDIO_FRAME_AAC 2048
#define SIZE_AUDIO_PACKED_AAC 2048*2*2
#define SIZE_AUDIO_FRAME_MP3 2048
#define SIZE_AUDIO_PACKED_MP3 1500
#define SIZE_VIDEO_PACKED		300*1024

//28个字节
typedef struct RTP_HDR_T_{
	//16个字节
	UINT	session;		//数据类型
	DWORD	ts;			    /*timestamp*/
	DWORD	dataLen;		//数据长度

	DWORD	w:16;			//宽度
	DWORD	h:16;			//高度
	//4字节
	DWORD	nframeCount:18;	//帧号
	DWORD	NowDownRate:14;	//当前使用的下载流量K
	//4字节
	DWORD	keyCount:16;	//关键帧计数
	DWORD	NowUpRate:15;	//当前使用的上传流量K
	DWORD	k:1;			/*keyframe or not,0,no,1,yes*/
	//4字节
	DWORD	cpuUse:7;		//cpu使用率
	DWORD	vs:4;			/*video size*/
	DWORD	fr:5;			/*frame rate*/
	unsigned int bFilpH:1;		/*上下翻转处理*/
	unsigned int bFlipUV:1;		//UV翻转处理

	DWORD 	rotateFlag:2;   //顺时针旋转标记 0:0°,1:180°,2:90°,3:270°
	DWORD	bNewSizeFlag:1;			//是否明确支持读取宽跟高 的协议包头,1:支持读取宽跟搞，0为不支持
	DWORD	reserves:11;		//保留
}RTP_HDR_T;

//设置函数返回值
typedef enum 
{
	SetSZYSdkRet_OK  = 0, 
	SetSZYSdkRet_PRMErr                //参数错误
}SetSZYSdkRet;

//音频数据包头
typedef struct AUD_HDR_T_
{
	//4字节
	int	nSamplePerSec:16;        //采样率
	int	wBitPerSample:8;         //位数 
	int nChannel:4;              //通道数
	int	wFormat:4;               //压缩格式
}AUD_HDR_T;

//新的音频数据包
typedef struct PACK_AUDIO2_
{
	int session;
	unsigned int ts;
	AUD_HDR_T header;
	int datalen;
	unsigned int nPacketFlag:1; //为0
	unsigned int nFirstPacketFlag:1; //为0
	int reserves:30;		//保留字段
	unsigned char pdata[SIZE_AUDIO_PACKED_MP3];
}PACK_AUDIO2;

//老的音频包
typedef struct PACK_AUDIO1_
{
	int session;
	unsigned int ts;
	AUD_HDR_T header;
	unsigned char data[SIZE_AUDIO_PACKED_MP3];
}PACK_AUDIO1;


//  转发通道号以及对应的消息ID
typedef struct RtpSendCameraInfo_
{
	int	 m_iMsgID;				// 消息ID
	int  m_iSocketID;			// SocketID
	int  m_iChannel;			// 通道号 
	BOOL m_bOpenAudio;			// 音频开关，1：开，0：关
	BOOL m_bOpenVideo;			// 是否开启实时视频传输
	BOOL m_bNetAdaptive;		// 是否自适应码率
	BOOL m_bAlarmSwitch;		// 布防/撤防标识
	int  m_iFrameCount;			// 帧号ID
	int  m_iFrameKeyCount;		// 关键帧ID
	int  m_iTotalSendDataLen;	// 发送数据总长度
	int  m_iNowRate;			// 现在的码率
	int  m_iRemotePort;			// 转发端口
	int  m_PicturePort;			// 图片上传端口
	char m_szRemoteIP[32];		// 转发IP
	char m_szCameraID[20];		// 摄像头ID
	char m_szCameraName[30];	// 摄像头名称
	int  m_workThread;			//线程工作正常:1
	int  m_iRtpSendCheckCount;	//线程判断检测次数
}RtpSendCameraInfo;

typedef struct RtpSendInfo_
{
	int m_iTotalChn;			// 当前摄像头总数
	RtpSendCameraInfo *m_stRtpSendCameraInfo;
}RtpSendInfo;


// 存放回放信息的结构体
#define MAXREPLAYNUM 2
typedef struct ReplayMsgInfo_
{
	int m_iMsgID;			// 消息ID
	int m_startFlag;		// 消息ID
	int m_iSocketID;		// socketID
	int m_iProcessID;		// 过程ID
	int m_iFsPort;			// 录像回放的转发端口
	int m_iFsType;			// 新旧转发标识
	int m_iReFrameCnt;		// 回放帧ID
	int m_iReKeyFrameCnt;	// 回放帧ID
	unsigned int m_fileSum;          // 回放的文件总数
	unsigned int m_fileNum;          // 回放的文件总数
	unsigned int m_tsLast;           // 发送进度条跳动的最近一次时间戳
	unsigned int m_tsSum;            // 发送进度条跳动的最近一次时间戳
	char m_szFsIp[32];		// 录像回放的转发IP
	char m_szCameraId[20];	// 摄像头ID
	int  m_workThread;			//线程工作正常:1
	
}ReplayMsgInfo;


//发送音频函数和相关结构体
typedef struct
{
	int 			nChannel;             		//通道号,从0开始计数
	unsigned long	ts;							//时间戳，单位：毫秒，递增
	unsigned int	nSamplePerSec;        		//采样率
	int				wBitPerSample;        		//位数
	int				wFormat;					//压缩格式，目前只支持AAC格式音频输入，此参数暂忽略
	int 			len;						//数据长度
	unsigned char  *pdata;						//音频数据
}AUDIO_DATA;

//发送视频函数和相关结构体
typedef struct
{
	int 			nChannel;             	//通道号,从0开始计数
	unsigned long	ts;						//时间戳，单位：毫秒，递增
	int 			w;						//视频宽
	int 			h;						//视频高
	int 			fr;						//帧率
	int 			bKey;					//是否关键帧
	int 			len;					//数据长度
	unsigned char	*data;					//H264数据
}VIDEO_DATA;

// 存放录像回放操作的结构体(138)
typedef struct ReplayCmd_
{
	int		m_iProcessID;	// 过程ID
	int		m_iType;		// 控制操作类型（0x01-0x04:中心指令，0xFF）
	float	m_fParam;		// 控制参数
}ReplayCmd;

// 工作线程状态
enum THREAD_STA
{
	NO_WORKING = 0, // 不工作
	WORKING = 1		// 工作
};

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

	void Init_RtpManager(RtpSendInfo *v_rtpSendInfo, ReplayMsgInfo *v_stReplayMsgIfo);
	void Release_RtpManager();
	void RtpSendH264Data(VIDEO_DATA *v_stVideoData);
	void RtpSendAudioData(AUDIO_DATA* v_pAudioData);
	void SendReplayVideoData(VIDEO_DATA *v_pVideoData, int v_iProcessId);
	void SendReplayAudioData(AUDIO_DATA *v_pAudioData, int v_iProcessId);
	void ModifyVideoParam(int v_iChannel, int v_iBitRate, int v_iFrameRate, int v_iQpConstant, int v_iKeyFrameRate, int v_iWith, int v_iHeight);

	void SetCtrlThreadStatus(int  v_iSta);
	void Init_RtpSendTask(void);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
