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


#define	MAXCAMERANUM			1						// ���ͨ����Ŀ
#define CONFIGFILENAME			"SyCfgFile.ini"			// �����ļ�����

// devtool��xml����
#define CONNETTYPE 				"View-DeviceTool"

//#define SDK_HARD_FWVERSION	"ASYKJIPC-1.0.0"
#define SDK_HARD_FWVERSION		"SYKJIPC-1.0.0"

#define SDK_PROCESS_NAME        "aliyunOss.bin"


#define 	COMMVER1 					1
#define 	COMMVER2 					2
#define 	SDKVERSION 					COMMVER1 

#if   SDKVERSION == COMMVER1

#define SDK_SYSTEM_FWVERSION	"1.0.0025"	// SDK�汾��
#define DE_ROUTESERVER			"ipc.100memory.com"
#define	DE_ALBUCKETNAME			"memory100"	
#define	DE_ALENDPOINT			"oss-cn-qingdao.aliyuncs.com"
#define DE_ALACCESSKEYID		"FNbJ4XFhi337eObw"
#define	DE_ALACCESSSECRET		"mwpC5hLaYOIR3obMkqJ4uKoPCvwupf"

#elif SDKVERSION == COMMVER2 

#define SDK_SYSTEM_FWVERSION	"2.0.0025"	// SDK�汾��
#define DE_ROUTESERVER			"ipc.355fun.com"
#define	DE_ALBUCKETNAME			"cyq-fyws"	
#define	DE_ALENDPOINT			"oss-cn-qingdao.aliyuncs.com"
#define DE_ALACCESSKEYID		"QyPpDQ2ZRTaO9R4G"
#define	DE_ALACCESSSECRET		"L3gqCggwtaH3byJipRTTZdXs1bTSt8"

#endif

// �豸��źͲ�Ʒ�ͺ������ļ�����
#define SERVERNOFILENAME  		"serverNo"
#define PRODUCTFILENAME  		"productNo"
#define PASSWORDFILE	  		"password"
//Ӳ���汾������汾�����ļ�����
#define HDVERSIONFILENAME  		"hardVersion"
#define SFVERSIONFILENAME  		"softVersion"

#define LOGSIZE					256*1024
#define CMDBUFFER				256*1024
#define DEVICECONFIGDIR			"/mnt/mtd/ipc/sykj"
#define	LOGDIR					"/mnt/mtd/ipc/tmpfs/syflash"
#define	LOGUPDATEDIR			"/mnt/mtd/ipc/tmpfs/syflash/updateLog"
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

//#define	DE_ALBUCKETNAME			"zs-oss-test-go"	
//#define	DE_ALENDPOINT			"oss-cn-shenzhen.aliyuncs.com"
//#define 	DE_ALACCESSKEYID		"YCcIdLNNUAkA4d2K"
//#define	DE_ALACCESSSECRET		"jwVtojgl6hsxyj7oj86lz0X3T73x5v"
#define	DE_ALBUCKETNAME			"memory100"	
#define	DE_ALENDPOINT			"oss-cn-qingdao.aliyuncs.com"
#define DE_ALACCESSKEYID		"FNbJ4XFhi337eObw"
#define	DE_ALACCESSSECRET		"mwpC5hLaYOIR3obMkqJ4uKoPCvwupf"

#define	DE_VIDEOPATH			"Videos"
#define	DE_JPGPATH				"Photos"



// tagCapParamCfg����ͷ��ϢĬ�ϲ���
#define DE_BITRATE				512					// ����
#define DE_FRAMERATE			12					// ֡��
#define DE_HEIGHT				720					// ��Ƶ��
#define DE_WIDTH				1280				// ��Ƶ��
#define DE_QPCONSTANT			1					// ��Ƶ����
#define DE_KEYFRAMERATE			100					// �ؼ�֡���
#define DE_CODETYPE				0					//



#define DE_BEFORE_RECORD_MOTION_LASTTIME		15
#define DE_BEFORE_RECORD_MOTION_TIMES			15
#define DE_CONTINUES_RECORD_MOTION_LASTTIME		5
#define DE_CONTINUES_RECORD_MOTION_TIMES		5
#define	DE_END_RECORD_MOTION_TIME				100
#define	DE_ENABLE								1

#define LCM_80(x) ((0==(x)%80) ? (x) : (80*(((x)+80)/80)))   //80����С������

typedef struct __attribute__((packed, aligned(8))) tagMasterServerCfg_
{
	//int 	m_iMasterPort;		// ���ض˿�
	char	m_szMasterIP[80];	// ����IP
	char	m_szReserve[160];	// ����IP
}tagMasterServerCfg;

typedef struct __attribute__((packed, aligned(8))) tagCapParamCfg_
{
	WORD		m_wBitRate;			// ����
	BYTE		m_wFrameRate;		// ֡��
	WORD		m_wHeight;			// ��Ƶ��
	WORD		m_wWidth;			// ��Ƶ��
	BYTE		m_wQpConstant;		// ��Ƶ����
	WORD		m_wKeyFrameRate;	// �ؼ�֡���
	BYTE		m_CodeType;			
	char		m_szReserve[22];	// Ԥ���ռ�		
}tagCapParamCfg;

typedef struct __attribute__((packed, aligned(8))) tagDevInfoCfg_
{
	char	m_szPassword[80];		// �豸����	
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
	char 		m_szReserver[160];
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
//
/*typedef struct __attribute__((packed, aligned(8))) tagVersionCfg_
{
	char 		m_szHardVerion[80];
	char 		m_szSoftVerion[80];
}tagVersionCfg;*/
//


typedef struct __attribute__((packed, aligned(8))) tagConfigCfg_
{
	char	m_szSpecCode[80];

	// ����IP�˿ڲ���
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagMasterServerCfg))];
		tagMasterServerCfg m_objMasterServerCfg;
	} m_unMasterServerCfg;

	// �豸��Ϣ
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagDevInfoCfg))];
		tagDevInfoCfg m_objDevInfoCfg;
	} m_unDevInfoCfg;
	
	// ����ͷ��Ϣ����
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

	/*union
	{
		char		m_szReserved[LCM_80(sizeof(tagVersionCfg))];
		tagVersionCfg m_objVerionCfg;
	} m_unVersionCfg;*/

}tagConfigCfg;

#endif
