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

//#define SDK_HARD_FWVERSION	"ASYKJIPC-1.0.0"
#define SDK_HARD_FWVERSION		"SYKJIPC-1.0.0"

#define SDK_PROCESS_NAME        "aliyunOss.bin"

//#define DEBUG_FILE_VIDEO		1
//#define DEBUG_OTHER_RUN			1


#define	DE_ENABLE						1
#define	DE_DISABLE						0

#define 	COMMVER1 					1
#define 	COMMVER2 					2
#define 	SDKVERSION 					COMMVER2

#if   SDKVERSION == COMMVER1
#define SDK_SYSTEM_FWVERSION	"1.0.0052"	// SDK版本号

#define DE_ROUTESERVER			"ipc.100memory.com"
#define	DE_ALBUCKETNAME			"memory100"	
#define	DE_ALENDPOINT			"oss-cn-qingdao.aliyuncs.com"
#define DE_ALACCESSKEYID		"FNbJ4XFhi337eObw"
#define	DE_ALACCESSSECRET		"mwpC5hLaYOIR3obMkqJ4uKoPCvwupf"
#define	DE_VIDEOPATH			"Videos"
#define	DE_JPGPATH				"Photos"


#define DE_URGENCY_ENABLE		1
#define DE_SOUND_ENABLE			1
#define DE_COMMSERVERTIME		0

#elif SDKVERSION == COMMVER2 
#define SDK_SYSTEM_FWVERSION	"2.0.0052"	// SDK版本号
#define DE_ROUTESERVER			"ipc.355fun.com"
#define	DE_ALBUCKETNAME			"cyq-fyws"	
#define	DE_ALENDPOINT			"oss-cn-qingdao.aliyuncs.com"
#define DE_ALACCESSKEYID		"QyPpDQ2ZRTaO9R4G"
#define	DE_ALACCESSSECRET		"L3gqCggwtaH3byJipRTTZdXs1bTSt8"
#define	DE_VIDEOPATH			"Videos"
#define	DE_JPGPATH				"Photos"

#define DE_URGENCY_ENABLE		0
#define DE_SOUND_ENABLE			0
#define DE_COMMSERVERTIME		3
#endif

// 设备编号和产品型号配置文件名字
#define SERVERNOFILENAME  		"serverNo"
#define PRODUCTFILENAME  		"productNo"
#define PASSWORDFILE	  		"password"
//硬件版本和软件版本配置文件名字
#define HDVERSIONFILENAME  		"hardVersion"
#define SFVERSIONFILENAME  		"softVersion"

#define LOGSIZE					256*1024
#define CMDBUFFER				256*1024
#define DEVICECONFIGDIR			"/mnt/mtd/ipc/sykj"
#define	LOGDIR					"/mnt/mtd/ipc/tmpfs/syflash"
#define	LOGUPDATEDIR			"/mnt/mtd/ipc/tmpfs/syflash/updateLog"
#define	LOGTEMPDIR				"/mnt/mtd/ipc/tmpfs/sytmp/LogTemp"
#define TEMPDIR 				"/mnt/mtd/ipc/tmpfs/sytmp"
#define ENCODEFILE 				"/mnt/mtd/ipc/conf/config_encode.ini"
#define CMDREBOOT 				"killall uipcam;killall aliyunOss.bin;sleep 3;/mnt/mtd/ipc/sykj/uipcam &"
#define	DE_ROUTESERVERPORT		80



//char sendBuf[] = "id=LB1GB29HYM3M8HJ7111C&pwd=123456";
//char strUrl[] = "http://cgtx.100memory.com/ipccmd.php?act=login";

#define	DE_DEVICEPRODUCT		"SYKJ"
#define DE_DEVICEMODEL 			"MotionDetect1.0"
#define DE_DEVICETYPE			"IPC"
#define	DE_DEFAULTDEVICEMACADDR	"00-00-00-00-00-00"
#define	DEVICECHANNELNUM		1
#define	DEVICECHANNELSTARTNUM	1
#define DE_SECRET				""


// tagCapParamCfg摄像头信息默认参数
#define DE_BITRATE				512					// 码率
#define DE_FRAMERATE			12					// 帧率
#define DE_HEIGHT				720					// 视频高
#define DE_WIDTH				1280				// 视频高
#define DE_QPCONSTANT			1					// 视频质量
#define DE_KEYFRAMERATE			100					// 关键帧间隔
#define DE_CODETYPE				0					//



#define DE_BEFORE_RECORD_MOTION_LASTTIME		15
#define DE_BEFORE_RECORD_MOTION_TIMES			15
#define DE_CONTINUES_RECORD_MOTION_LASTTIME		5
#define DE_CONTINUES_RECORD_MOTION_TIMES		5
#define	DE_END_RECORD_MOTION_TIME				100



#define DE_START_PREIOD							13
#define DE_START_SUM_DETECT						24
#define DE_START_SUM_AREA						8
#define DE_START_SOUND_SIZE						2000

#define DE_OVER_PREIOD							30
#define DE_OVER_SUM_DETECT						5
#define DE_OVER_SUM_AREA						5
#define DE_OVER_SOUND_SIZE						1000

#define DE_URGENCY_REC_TIME						40


#define LCM_80(x) ((0==(x)%80) ? (x) : (80*(((x)+80)/80)))   //80的最小整数倍

typedef struct __attribute__((packed, aligned(8))) tagMasterServerCfg_
{
	//int 	m_iMasterPort;		// 主控端口
	char	m_szMasterIP[80];	// 主控IP
	char	m_szReserve[160];	// 主控IP
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
	char 	m_szReserver[160];
}tagDevInfoCfg;

typedef struct __attribute__((packed, aligned(8))) tagAliyunOssCfg_
{
	char 		m_szBuctetName[80];
	char 		m_szOssEndPoint[80];
	char 		m_szAccessKeyId[80];
	char 		m_szAccessKeySecret[80];
	char 		m_szVideoPath[80];
	char 		m_szJPGPath[80];
	char		m_bLogUploadEnable;
	char 		m_szReserver[159];
}tagAliyunOssCfg;


typedef struct __attribute__((packed, aligned(8))) tagMotionCfg_
{
	INT32U		m_iBefRecLastTime;				
	INT32U		m_iBefRecTimes;		
	INT32U		m_iConRecLastTime;		
	INT32U		m_iConRecTimes;	
	INT32U		m_iEndRecTime;
	char		m_bEnable;
	char 		m_szReserver[59];
}tagMotionCfg;

typedef struct __attribute__((packed, aligned(8))) tagUrgencyMotionCfg_
{
	INT32U		m_iStartPeriod;				
	INT32U		m_iStartSumDetect;		
	INT32U		m_iStartSumArea;		
	INT32U		m_iStartSoundSize;

	INT32U		m_iOverPeriod;				
	INT32U		m_iOverSumDetect;		
	INT32U		m_iOverSumArea;		
	INT32U		m_iOverSoundSize;
	
	INT32U		m_iEndRecTime;
	INT32U 		m_iCommServerTime;
	char		m_bEnable;
	char        m_bInvalid;
	char 		m_szReserver[38];
}tagUrgencyMotionCfg;

typedef struct __attribute__((packed, aligned(8))) tagSoundEableCfg_
{
	char		m_bStartUpEnable;
	char		m_bServerSuccessEnable;
	char		m_bServerFailureEnable;
	char		m_bUpgradeEnable;
	char		m_bLoginInEnable;
	char		m_bUrgencyStartEnable;
	char		m_bUrgencyOverEnable;
	char		m_bEnable;
	char  		m_bInvalid;
	char 		m_szReserver[71];
}tagSoundEableCfg;

/*typedef struct __attribute__((packed, aligned(8))) tagVersionCfg_
{
	char 		m_szHardVerion[80];
	char 		m_szSoftVerion[80];
}tagVersionCfg;*/
//


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

	union
	{
		char		m_szReserved[LCM_80(sizeof(tagUrgencyMotionCfg))];
		tagUrgencyMotionCfg 	m_objUrgencyMotionCfg;
	} m_unUrgencyMotionCfg;

	union
	{
		char		m_szReserved[LCM_80(sizeof(tagSoundEableCfg))];
		tagSoundEableCfg 	m_objSoundEableCfg;
	} m_unSoundEableCfg;

	/*union
	{
		char		m_szReserved[LCM_80(sizeof(tagVersionCfg))];
		tagVersionCfg m_objVerionCfg;
	} m_unVersionCfg;*/

}tagConfigCfg;

#endif
