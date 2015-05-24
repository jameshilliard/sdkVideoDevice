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

#define CFGCHECKMSG					"sykj-hello"
#define MATSERIPCOUNT 				4
// 视频
#define BAS_BUFFER_DEFAULT_SIZE		330*1024		
#define SIZE_VIDEO_PACKED			(BAS_BUFFER_DEFAULT_SIZE - 3000)	// 视频发送最大缓冲区


#define	MAXCAMERANUM				1	// 最大通道数目
#define CONFIGFILENAME				"CfgFile.ini"			// 配置文件名字


// 设备类型
#define SDK_HARD_FWVERSION			"SYIPC-1.0.0"
#define SDK_SYSTEM_FWVERSION		"1.0.0003"	// SDK版本号

#define LOGSIZE						256*1024
#define CMDBUFFER					256*1024
#define DEVICECONFIGDIR				"/mnt/mtd/ipc"
#define	LOGDIR						"/mnt/mtd/ipc/tmpfs/syflash"
#define TEMPDIR 					"/mnt/mtd/ipc/tmpfs/sytmp"

#define		ROUTESERVERPORT		5002
#define		CONTROLSERVERPORT	5000

#define 	ROUTESERVER			"www.benxunshida.com"
//#define 	ROUTESERVER			"121.43.234.112"
#define 	USER				"pick@testdevice"
//#define 	USER				"pick@sd3"

#define		SECRET				"123456"
#define 	SERVERNO0			"hk000000"

//#define 	SERVERNO0			"hk000000"
//#define 	SERVERNO1			"hk000001"
//#define 	SERVERNO2			"hk000002"

#define		DEVICEPRODUCT					"BXSD"
#define 	DEVICEMODEL 					"9100"
#define 	DEVICETYPE						"IPC"
#define		DEFAULTDEVICEMACADDR			"94-DE-80-53-0A-90"
#define		DEVICECHANNELNUM				1
#define		DEVICECHANNELSTARTNUM			1

#define LCM_80(x) ((0==(x)%80) ? (x) : (80*(((x)+80)/80)))   //80的最小整数倍

typedef struct PACKED_(8) tagDevInfoCfg_
{
	char	m_szDeviceId[20];		// 设备ID
	char	m_szProductMode[20];	// 设备型号
	char	m_szPassword[20];		// 设备密码
	char    m_szDeviceName[20];
}tagDevInfoCfg;

typedef struct PACKED_(8) tagMasterServerCfg_
{
	int 	m_iMasterPort;		// 主控端口
	char	m_szMasterIP[75];	// 主控IP
}tagMasterServerCfg;


typedef struct PACKED_(8) tagConfigCfg_
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


}tagConfigCfg;

#endif