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
#define			REMOTESERSESSION				'TRSV'//ת�����������������ӱ�־
#define  		YRVIDEO					  		'VDYR'
#define  		YRAUIDO				  			'AUYR'
#define  		SZYAUDIO				  		'AUSZ'
#define         SZYAUDIO_EX			            'AUEX'
#define  		SZYFILE					  		'FILE'

#define			CMDTYPE							1
#define			DATATYPE						2
#define 		DATA_TOKEN						"<!ROW>"

//��Ƶ
#define SIZE_AUDIO_PACKED  60*30
#define SIZE_AUDIO_FRAME_AAC 2048
#define SIZE_AUDIO_PACKED_AAC 2048*2*2
#define SIZE_AUDIO_FRAME_MP3 2048
#define SIZE_AUDIO_PACKED_MP3 1500
#define SIZE_VIDEO_PACKED		300*1024

//28���ֽ�
typedef struct RTP_HDR_T_{
	//16���ֽ�
	UINT	session;		//��������
	DWORD	ts;			    /*timestamp*/
	DWORD	dataLen;		//���ݳ���

	DWORD	w:16;			//���
	DWORD	h:16;			//�߶�
	//4�ֽ�
	DWORD	nframeCount:18;	//֡��
	DWORD	NowDownRate:14;	//��ǰʹ�õ���������K
	//4�ֽ�
	DWORD	keyCount:16;	//�ؼ�֡����
	DWORD	NowUpRate:15;	//��ǰʹ�õ��ϴ�����K
	DWORD	k:1;			/*keyframe or not,0,no,1,yes*/
	//4�ֽ�
	DWORD	cpuUse:7;		//cpuʹ����
	DWORD	vs:4;			/*video size*/
	DWORD	fr:5;			/*frame rate*/
	unsigned int bFilpH:1;		/*���·�ת����*/
	unsigned int bFlipUV:1;		//UV��ת����

	DWORD 	rotateFlag:2;   //˳ʱ����ת��� 0:0��,1:180��,2:90��,3:270��
	DWORD	bNewSizeFlag:1;			//�Ƿ���ȷ֧�ֶ�ȡ����� ��Э���ͷ,1:֧�ֶ�ȡ����㣬0Ϊ��֧��
	DWORD	reserves:11;		//����
}RTP_HDR_T;

//���ú�������ֵ
typedef enum 
{
	SetSZYSdkRet_OK  = 0, 
	SetSZYSdkRet_PRMErr                //��������
}SetSZYSdkRet;

//��Ƶ���ݰ�ͷ
typedef struct AUD_HDR_T_
{
	//4�ֽ�
	int	nSamplePerSec:16;        //������
	int	wBitPerSample:8;         //λ�� 
	int nChannel:4;              //ͨ����
	int	wFormat:4;               //ѹ����ʽ
}AUD_HDR_T;

//�µ���Ƶ���ݰ�
typedef struct PACK_AUDIO2_
{
	int session;
	unsigned int ts;
	AUD_HDR_T header;
	int datalen;
	unsigned int nPacketFlag:1; //Ϊ0
	unsigned int nFirstPacketFlag:1; //Ϊ0
	int reserves:30;		//�����ֶ�
	unsigned char pdata[SIZE_AUDIO_PACKED_MP3];
}PACK_AUDIO2;

//�ϵ���Ƶ��
typedef struct PACK_AUDIO1_
{
	int session;
	unsigned int ts;
	AUD_HDR_T header;
	unsigned char data[SIZE_AUDIO_PACKED_MP3];
}PACK_AUDIO1;


//  ת��ͨ�����Լ���Ӧ����ϢID
typedef struct RtpSendCameraInfo_
{
	int	 m_iMsgID;				// ��ϢID
	int  m_iSocketID;			// SocketID
	int  m_iChannel;			// ͨ���� 
	BOOL m_bOpenAudio;			// ��Ƶ���أ�1������0����
	BOOL m_bOpenVideo;			// �Ƿ���ʵʱ��Ƶ����
	BOOL m_bNetAdaptive;		// �Ƿ�����Ӧ����
	BOOL m_bAlarmSwitch;		// ����/������ʶ
	int  m_iFrameCount;			// ֡��ID
	int  m_iFrameKeyCount;		// �ؼ�֡ID
	int  m_iTotalSendDataLen;	// ���������ܳ���
	int  m_iNowRate;			// ���ڵ�����
	int  m_iRemotePort;			// ת���˿�
	int  m_PicturePort;			// ͼƬ�ϴ��˿�
	char m_szRemoteIP[32];		// ת��IP
	char m_szCameraID[20];		// ����ͷID
	char m_szCameraName[30];	// ����ͷ����
	int  m_workThread;			//�̹߳�������:1
	int  m_iRtpSendCheckCount;	//�߳��жϼ�����
}RtpSendCameraInfo;

typedef struct RtpSendInfo_
{
	int m_iTotalChn;			// ��ǰ����ͷ����
	RtpSendCameraInfo *m_stRtpSendCameraInfo;
}RtpSendInfo;


// ��Żط���Ϣ�Ľṹ��
#define MAXREPLAYNUM 2
typedef struct ReplayMsgInfo_
{
	int m_iMsgID;			// ��ϢID
	int m_startFlag;		// ��ϢID
	int m_iSocketID;		// socketID
	int m_iProcessID;		// ����ID
	int m_iFsPort;			// ¼��طŵ�ת���˿�
	int m_iFsType;			// �¾�ת����ʶ
	int m_iReFrameCnt;		// �ط�֡ID
	int m_iReKeyFrameCnt;	// �ط�֡ID
	unsigned int m_fileSum;          // �طŵ��ļ�����
	unsigned int m_fileNum;          // �طŵ��ļ�����
	unsigned int m_tsLast;           // ���ͽ��������������һ��ʱ���
	unsigned int m_tsSum;            // ���ͽ��������������һ��ʱ���
	char m_szFsIp[32];		// ¼��طŵ�ת��IP
	char m_szCameraId[20];	// ����ͷID
	int  m_workThread;			//�̹߳�������:1
	
}ReplayMsgInfo;


//������Ƶ��������ؽṹ��
typedef struct
{
	int 			nChannel;             		//ͨ����,��0��ʼ����
	unsigned long	ts;							//ʱ�������λ�����룬����
	unsigned int	nSamplePerSec;        		//������
	int				wBitPerSample;        		//λ��
	int				wFormat;					//ѹ����ʽ��Ŀǰֻ֧��AAC��ʽ��Ƶ���룬�˲����ݺ���
	int 			len;						//���ݳ���
	unsigned char  *pdata;						//��Ƶ����
}AUDIO_DATA;

//������Ƶ��������ؽṹ��
typedef struct
{
	int 			nChannel;             	//ͨ����,��0��ʼ����
	unsigned long	ts;						//ʱ�������λ�����룬����
	int 			w;						//��Ƶ��
	int 			h;						//��Ƶ��
	int 			fr;						//֡��
	int 			bKey;					//�Ƿ�ؼ�֡
	int 			len;					//���ݳ���
	unsigned char	*data;					//H264����
}VIDEO_DATA;

// ���¼��طŲ����Ľṹ��(138)
typedef struct ReplayCmd_
{
	int		m_iProcessID;	// ����ID
	int		m_iType;		// ���Ʋ������ͣ�0x01-0x04:����ָ�0xFF��
	float	m_fParam;		// ���Ʋ���
}ReplayCmd;

// �����߳�״̬
enum THREAD_STA
{
	NO_WORKING = 0, // ������
	WORKING = 1		// ����
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
