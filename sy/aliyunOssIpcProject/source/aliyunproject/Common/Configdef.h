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

#define CFGCHECKMSG				"sykj-aliyunOssIpc-1"
#define MATSERIPCOUNT 			4


#define	MAXCAMERANUM			1						// 最大通道数目
#define CONFIGFILENAME			"SyCfgFile.ini"			// 配置文件名字

// devtool的xml类型
#define CONNETTYPE 				"View-DeviceTool"

// 设备类型
#define SDK_HARD_FWVERSION		"SYKJIPC-1.0.0"
#define SDK_SYSTEM_FWVERSION	"1.0.0000"	// SDK版本号

// 设备编号和产品型号配置文件名字
#define SERVERNOFILENAME  		"serverNo"
#define PRODUCTFILENAME  		"productNo"
#define PASSWORDFILE	  		"password"

#define LOGSIZE					256*1024
#define CMDBUFFER				256*1024
#define DEVICECONFIGDIR			"/mnt/mtd/ipc/sykj"
#define	LOGDIR					"/mnt/mtd/ipc/tmpfs/syflash"
#define TEMPDIR 				"/mnt/mtd/ipc/tmpfs/sytmp"

#define	DE_ROUTESERVERPORT		5002
#define DE_ROUTESERVER			"www.sy.com"

#define	DE_DEVICEPRODUCT		"SYKJ"
#define DE_DEVICEMODEL 			"MD1.0"
#define DE_DEVICETYPE			"IPC"
#define	DE_DEFAULTDEVICEMACADDR	"00-00-00-00-00-00"
#define	DEVICECHANNELNUM		1
#define	DEVICECHANNELSTARTNUM	1
#define DE_SECRET				"123456"

#define	DE_ALBUCKETNAME			"zs-oss-test-go"	
#define	DE_ALENDPOINT			"oss-cn-shenzhen.aliyuncs.com"
#define DE_ALACCESSKEYID		"YCcIdLNNUAkA4d2K"
#define	DE_ALACCESSSECRET		"jwVtojgl6hsxyj7oj86lz0X3T73x5v"


// tagCapParamCfg摄像头信息默认参数
#define DE_BITRATE				50					// 码率
#define DE_FRAMERATE			8					// 帧率
#define DE_HEIGHT				480					// 视频高
#define DE_WIDTH				640					// 视频高
#define DE_QPCONSTANT			24					// 视频质量
#define DE_KEYFRAMERATE			128					// 关键帧间隔
#define DE_CODETYPE				0					// 关键帧间隔



#define DE_BEFORE_RECORD_MOTION_LASTTIME		5
#define DE_BEFORE_RECORD_MOTION_TIMES			3
#define DE_CONTINUES_RECORD_MOTION_LASTTIME		5
#define DE_CONTINUES_RECORD_MOTION_TIMES		3
#define	DE_END_RECORD_MOTION_TIME				3*60

#define LCM_80(x) ((0==(x)%80) ? (x) : (80*(((x)+80)/80)))   //80的最小整数倍

typedef struct __attribute__((packed, aligned(8))) tagMasterServerCfg_
{
	int 	m_iMasterPort;		// 主控端口
	char	m_szMasterIP[236];	// 主控IP
}tagMasterServerCfg;

typedef struct __attribute__((packed, aligned(8))) tagCapParamCfg_
{
	WORD		m_wBitRate;			// 码率
	BYTE		m_wFrameRate;		// 帧率
	WORD		m_wHeight;			// 视频高
	WORD		m_wWidth;			// 视频宽
	BYTE		m_wQpConstant;		// 视频质量
	WORD		m_wKeyFrameRate;	// 关键帧间隔
	BYTE		m_CodeType;			
	char		m_szReserve[22];	// 预留空间		
}tagCapParamCfg;

typedef struct __attribute__((packed, aligned(8))) tagDevInfoCfg_
{
	char	m_szPassword[80];		// 设备密码	
	char 	m_szReserver[80];
}tagDevInfoCfg;

typedef struct __attribute__((packed, aligned(8))) tagAliyunOssCfg_
{
	char 		m_szBuctetName[80];
	char 		m_szOssEndPoint[80];
	char 		m_szAccessKeyId[80];
	char 		m_szAccessKeySecret[80];
	char 		m_szReserver[80];
}tagAliyunOssCfg;


typedef struct __attribute__((packed, aligned(8))) tagMotionCfg_
{
	INT32U		m_iBefRecLastTime;				
	INT32U		m_iBefRecTimes;		
	INT32U		m_iConRecLastTime;		
	INT32U		m_iConRecTimes;	
	INT32U		m_iEndRecTime;	
	char 		m_szReserver[60];
}tagMotionCfg;

typedef struct __attribute__((packed, aligned(8))) tagConfigCfg_
{
	char	m_szSpecCode[80];

	// 主控IP端口参数
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagMasterServerCfg))];
		tagMasterServerCfg m_objMasterServerCfg;
	} m_unMasterServerCfg;

	// 设备信息
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagDevInfoCfg))];
		tagDevInfoCfg m_objDevInfoCfg;
	} m_unDevInfoCfg;
	
	// 摄像头信息参数
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagCapParamCfg)*MAXCAMERANUM)];
		tagCapParamCfg m_objCapParamCfg[MAXCAMERANUM];
	} m_unCapParamCfg;

	union
	{
		char		m_szReserved[LCM_80(sizeof(tagAliyunOssCfg))];
		tagAliyunOssCfg m_objAliyunOssCfg;
	} m_unAliyunOssCfg;

	union
	{
		char		m_szReserved[LCM_80(sizeof(tagMotionCfg))];
		tagMotionCfg 	m_objMotionCfg;
	} m_unMotionCfg;

}tagConfigCfg;

#endif
