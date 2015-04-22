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

#define CFGCHECKMSG		"��Ʒ��������"

// ��Ƶ
#define BAS_BUFFER_DEFAULT_SIZE		330*1024		
#define SIZE_VIDEO_PACKED	(BAS_BUFFER_DEFAULT_SIZE - 3000)	// ��Ƶ������󻺳���

//��Ƶ
#define SIZE_AUDIO_PACKED  			60*30
#define SIZE_AUDIO_FRAME_AAC 		2048
#define SIZE_AUDIO_PACKED_AAC 		2048*2*2
#define SIZE_AUDIO_FRAME_MP3 		2048
#define SIZE_AUDIO_PACKED_MP3 		1500

#define	MAXCAMERANUM						1	// ���ͨ����Ŀ
#define CONFIGFILENAME		"CfgFile.ini"			// �����ļ�����

#define DEFAULT_KERNEL_FWVERSION		"1.0.0.0"	// Ĭ���ں˰汾��
#define DEFAULT_NETWORKCARDNAME         "eth0"		// Ĭ������
#define DEFAULT_LOAD_PATH               "/tmp"		// Ĭ��·��

// �豸����
#define DEVTYPE SDK_DEV_TYPE

// ������Ƶ�˿�
#define LOCALVIDEO_PORT 9886	

// �豸��źͲ�Ʒ�ͺ������ļ�����
#define SERVERNOFILENAME  	"serverNo"
#define PRODUCTFILENAME  	"productNo"
#define PASSWORDFILE	 	"password"

// tagNetWorkCfg�������Ĭ��ֵ
#define DE_IP				"192.168.1.133"		//Ĭ��IP
#define DE_NETMASK			"255.255.0.0"		//Ĭ������
#define DE_GW				"192.168.1.1"		//Ĭ������
#define DE_DNS				"8.8.8.8"			//Ĭ��DNS
#define DE_HTTPPORT			7001			//Ĭ��http�˿�

// tagMasterServerCfg���ز���Ĭ��ֵ
#define DE_MASTERIP		"0.0.0.0"			//Ĭ������IP
#define DE_MASTERPORT		8006				//Ĭ�����ض˿�

// tagWorkTimeCfg����ʱ�����Ĭ��ֵ
#define DE_BEGAINTM		"00:00:00"			// ��ʼʱ��
#define DE_ENDTM			"00:00:00"			// ����ʱ��
#define DE_BRECORD			1					// �ǹ���ʱ�����Ƿ�¼��
#define DE_REBOOTCOUNT		80					// �������ز������ƴ���

// tagCapParamCfg����ͷ��ϢĬ�ϲ���
#define DE_BITRATE			50					// ����
#define DE_FRAMERATE		8					// ֡��
#define DE_HEIGHT			352					// ��Ƶ��
#define DE_WIDTH			640					// ��Ƶ��
#define DE_QPCONSTANT		24					// ��Ƶ����
#define DE_KEYFRAMERATE	128					// �ؼ�֡���
#define DE_OPENAUDIO		0					// ��Ƶ�Ƿ��
#define DE_ALARMSWITCH		1					// ����/����

#define LCM_80(x) ((0==(x)%80) ? (x) : (80*(((x)+80)/80)))   //80����С������

typedef struct PACKED_(8) tagNetWorkCfg_
{
	BOOL	m_bAutoIp;			// �Ƿ��Զ�����IP
	BOOL	m_bOpen3G;			// �Ƿ�ʹ��3Gģʽ
	char	m_szIpAddr[20];	// �豸IP��ַ
	char	m_szSubMask[20];	// �豸��������
	char	m_szGateWay[20];	// �豸����
	char	m_szDnsIp[20];		// �豸DNS
	char	m_szMacAddr[20];	// �豸MAC��ַ
	int		m_iHttpPort;		// �豸http�˿�
	char	m_szReserve[40];	// Ԥ���ռ�
}tagNetWorkCfg;

typedef struct PACKED_(8) tagMasterServerCfg_
{
	int 	m_iMasterPort;		// ���ض˿�
	char	m_szMasterIP[75];	// ����IP
}tagMasterServerCfg;

typedef struct PACKED_(8) tagDevInfoCfg_
{
	char 	m_szUserName[20];	// ��½�˺���
	char	m_szDevMask[32];	// �豸�ı�ʶ
	int     m_iCmdOverTime;	// ���ʱʱ�䣬��
}tagDevInfoCfg;

typedef struct PACKED_(8) tagWorkTimeCfg_
{
	BOOL	m_bRecodeOnWorkTime;		// ����ʱ�����Ƿ����¼��
	WORD	m_wRebootCount;			// ���Ӳ������صĴ���
	char 	m_szBegain[16];			// ����ʱ�俪ʼʱ��
	char	m_szEnd[16];				// ����ʱ�����ʱ��
}tagWorkTimeCfg;

typedef struct PACKED_(8) tagUpdateInfoCfg_
{
	BOOL	m_bUpdateStart;		// �Ƿ�ʼ��������
	BYTE	m_iUpdateResult;		// �������
	BYTE    m_iUpdateType;			// �������ͣ�0���ں�����/1��Ӧ��������
	int  	m_iUpdateCommId;		// ���������·�������ID
	char	m_szVersion[60];		// ����ǰ�İ汾��Ϣ
}tagUpdateInfoCfg;

typedef struct PACKED_(8) tagCapParamCfg_
{
	WORD		m_wBitRate;		// ����
	BYTE		m_wFrameRate;		// ֡��
	WORD		m_wHeight;			// ��Ƶ��
	WORD		m_wWidth;			// ��Ƶ��
	BYTE		m_wQpConstant;		// ��Ƶ����
	WORD		m_wKeyFrameRate;	// �ؼ�֡���
	BOOL		m_bAdaptStream;	// �Ƿ�����Ӧ����
	BOOL		m_bOpenAudio;		// ��Ƶ�Ƿ��
	BOOL		m_bAlarmSwitch;	// ����/����
	char		m_szReserve[20];	// Ԥ���ռ�		
}tagCapParamCfg;


typedef struct PACKED_(8) tagCodeConfigParamCfg_
{
	WORD		m_timeStamp;			// ʱ��		
}tagCodeConfigParamCfg;



typedef struct PACKED_(8) tagConfigCfg_
{
	char	m_szSpecCode[80];

	// ����IP�˿ڲ���
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagMasterServerCfg))];
		tagMasterServerCfg m_objMasterServerCfg;
	} m_unMasterServerCfg;

	// �����������
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagNetWorkCfg))];
		tagNetWorkCfg m_objNetWorkCfg;
	} m_unNetWorkCfg;

	// �豸��Ϣ
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagDevInfoCfg))];
		tagDevInfoCfg m_objDevInfoCfg;
	} m_unDevInfoCfg;

	// ����ʱ�����
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagWorkTimeCfg))];
		tagWorkTimeCfg m_objWorkTimeCfg;
	} m_unWorkTimeCfg;

	// ����������Ϣ
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagUpdateInfoCfg))];
		tagUpdateInfoCfg m_objUpdateInfoCfg;
	} m_unUpdateInfoCfg;

	// ����ͷ��Ϣ����
	union
	{
		char		m_szReserved[LCM_80(sizeof(tagCapParamCfg))];
		tagCapParamCfg m_objCapParamCfg[MAXCAMERANUM];
	} m_unCapParamCfg;
}tagConfigCfg;

#endif