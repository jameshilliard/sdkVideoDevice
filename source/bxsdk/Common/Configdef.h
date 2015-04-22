#ifndef _SZY_CONFIGDEF_H_
#define _SZY_CONFIGDEF_H_
#include 	"Typedef.h"

#if !defined(_MSC_VER)
#define PACKED_1 __attribute__( (packed, aligned(1)) )
#define PACKED_(x) __attribute__( (packed, aligned(x)) )
#else
#define PACKED_1 
#define PACKED_(x) 
#endif

#define CFGCHECKMSG		"产品大卖天下"

// 视频
#define BAS_BUFFER_DEFAULT_SIZE		330*1024		
#define SIZE_VIDEO_PACKED	(BAS_BUFFER_DEFAULT_SIZE - 3000)	// 视频发送最大缓冲区

//音频
#define SIZE_AUDIO_PACKED  			60*30
#define SIZE_AUDIO_FRAME_AAC 		2048
#define SIZE_AUDIO_PACKED_AAC 		2048*2*2
#define SIZE_AUDIO_FRAME_MP3 		2048
#define SIZE_AUDIO_PACKED_MP3 		1500

#define	MAXCAMERANUM						1	// 最大通道数目
#define CONFIGFILENAME		"CfgFile.ini"			// 配置文件名字

#define DEFAULT_KERNEL_FWVERSION		"1.0.0.0"	// 默认内核版本号
#define DEFAULT_NETWORKCARDNAME         "eth0"		// 默认网卡
#define DEFAULT_LOAD_PATH               "/tmp"		// 默认路径

// 设备类型
#define DEVTYPE SDK_DEV_TYPE

// 本地视频端口
#define LOCALVIDEO_PORT 9886	

// 设备编号和产品型号配置文件名字
#define SERVERNOFILENAME  	"serverNo"
#define PRODUCTFILENAME  	"productNo"
#define PASSWORDFILE	 	"password"

// tagNetWorkCfg网络参数默认值
#define DE_IP				"192.168.1.133"		//默认IP
#define DE_NETMASK			"255.255.0.0"		//默认掩码
#define DE_GW				"192.168.1.1"		//默认网关
#define DE_DNS				"8.8.8.8"			//默认DNS
#define DE_HTTPPORT			7001			//默认http端口

// tagMasterServerCfg主控参数默认值
#define DE_MASTERIP		"0.0.0.0"			//默认主控IP
#define DE_MASTERPORT		8006				//默认主控端口

// tagWorkTimeCfg工作时间参数默认值
#define DE_BEGAINTM		"00:00:00"			// 开始时间
#define DE_ENDTM			"00:00:00"			// 结束时间
#define DE_BRECORD			1					// 非工作时间外是否录像
#define DE_REBOOTCOUNT		80					// 连接主控不上限制次数

// tagCapParamCfg摄像头信息默认参数
#define DE_BITRATE			50					// 码率
#define DE_FRAMERATE		8					// 帧率
#define DE_HEIGHT			352					// 视频高
#define DE_WIDTH			640					// 视频高
#define DE_QPCONSTANT		24					// 视频质量
#define DE_KEYFRAMERATE	128					// 关键帧间隔
#define DE_OPENAUDIO		0					// 音频是否打开
#define DE_ALARMSWITCH		1					// 布防/撤防

#define LCM_80(x) ((0==(x)%80) ? (x) : (80*(((x)+80)/80)))   //80的最小整数倍

typedef struct PACKED_(8) tagNetWorkCfg_
{
	BOOL	m_bAutoIp;			// 是否自动配置IP
	BOOL	m_bOpen3G;			// 是否使用3G模式
	char	m_szIpAddr[20];	// 设备IP地址
	char	m_szSubMask[20];	// 设备子网掩码
	char	m_szGateWay[20];	// 设备网关
	char	m_szDnsIp[20];		// 设备DNS
	char	m_szMacAddr[20];	// 设备MAC地址
	int		m_iHttpPort;		// 设备http端口
	char	m_szReserve[40];	// 预留空间
}tagNetWorkCfg;

typedef struct PACKED_(8) tagMasterServerCfg_
{
	int 	m_iMasterPort;		// 主控端口
	char	m_szMasterIP[75];	// 主控IP
}tagMasterServerCfg;

typedef struct PACKED_(8) tagDevInfoCfg_
{
	char 	m_szUserName[20];	// 登陆账号名
	char	m_szDevMask[32];	// 设备的标识
	int     m_iCmdOverTime;	// 命令超时时间，秒
}tagDevInfoCfg;

typedef struct PACKED_(8) tagWorkTimeCfg_
{
	BOOL	m_bRecodeOnWorkTime;		// 工作时间内是否进行录像
	WORD	m_wRebootCount;			// 连接不上主控的次数
	char 	m_szBegain[16];			// 工作时间开始时间
	char	m_szEnd[16];				// 工作时间结束时间
}tagWorkTimeCfg;

typedef struct PACKED_(8) tagUpdateInfoCfg_
{
	BOOL	m_bUpdateStart;		// 是否开始进行升级
	BYTE	m_iUpdateResult;		// 升级结果
	BYTE    m_iUpdateType;			// 升级类型（0：内核升级/1：应用升级）
	int  	m_iUpdateCommId;		// 升级中心下发的命令ID
	char	m_szVersion[60];		// 升级前的版本信息
}tagUpdateInfoCfg;

typedef struct PACKED_(8) tagCapParamCfg_
{
	WORD		m_wBitRate;		// 码率
	BYTE		m_wFrameRate;		// 帧率
	WORD		m_wHeight;			// 视频高
	WORD		m_wWidth;			// 视频宽
	BYTE		m_wQpConstant;		// 视频质量
	WORD		m_wKeyFrameRate;	// 关键帧间隔
	BOOL		m_bAdaptStream;	// 是否自适应码率
	BOOL		m_bOpenAudio;		// 音频是否打开
	BOOL		m_bAlarmSwitch;	// 布防/撤防
	char		m_szReserve[20];	// 预留空间		
}tagCapParamCfg;


typedef struct PACKED_(8) tagCodeConfigParamCfg_
{
	WORD		m_timeStamp;			// 时间		
}tagCodeConfigParamCfg;



typedef struct PACKED_(8) tagConfigCfg_
{
	char	m_szSpecCode[80];

	// 主控IP端口参数
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagMasterServerCfg))];
		tagMasterServerCfg m_objMasterServerCfg;
	} m_unMasterServerCfg;

	// 网络参数配置
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagNetWorkCfg))];
		tagNetWorkCfg m_objNetWorkCfg;
	} m_unNetWorkCfg;

	// 设备信息
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagDevInfoCfg))];
		tagDevInfoCfg m_objDevInfoCfg;
	} m_unDevInfoCfg;

	// 工作时间参数
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagWorkTimeCfg))];
		tagWorkTimeCfg m_objWorkTimeCfg;
	} m_unWorkTimeCfg;

	// 升级参数信息
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagUpdateInfoCfg))];
		tagUpdateInfoCfg m_objUpdateInfoCfg;
	} m_unUpdateInfoCfg;

	// 摄像头信息参数
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagCapParamCfg))];
		tagCapParamCfg m_objCapParamCfg[MAXCAMERANUM];
	} m_unCapParamCfg;
}tagConfigCfg;

#endif