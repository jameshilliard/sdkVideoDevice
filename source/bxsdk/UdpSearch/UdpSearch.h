#ifndef _SZY_UDPSEARCH_H_
#define _SZY_UDPSEARCH_H_

#include "../Common/Typedef.h"
#include "../Common/Configdef.h"
#include "../Common/GlobFunc.h"
#include "../Common/NetFunc.h"
#include "../mxml/mxmlparser.h"


#define MULTICAST_IP "245.255.255.250"	//�ಥ���ַ
// �������
#define PC_DEVTOOL_PORT 		60011
#define	SCAN_SERVER_PORT		60012
// TCP���Ӷ˿�
#define SERVER_PORT 			60014

typedef struct UdpSearchParam_
{
	char *szSeriaNo;		// �豸���
	char *szLoginUser;		// ע���˺�
	char *szDevMask;		// �豸����
	char *szKernelVersion;	// �ں˳���汾��
	char *szProductVer;	// ��Ʒ�ͺ�
	BOOL *bLogin;			// �Ƿ��½
	BOOL *bRegister;		// �Ƿ�ע��
	char *szNodeId;			// �ڵ�ID
}UdpSearchParam;

typedef struct Server_Info_t_
{
	char	System_MACAddress[16];	//MAC��ַ
	char	Network_IPAddress[16];	//��������IP��ַ
	char	Network_Subnet[16];		//��������Mask
	char	Network_GateWay[16];	//Ĭ�����ػ�·������ַ
	char	System_FWVersion[24];		//���豸�ͺ�
	char	System_SerialNo[16];		//SerialNum

	char    LoginUser[30];				//��½�û���
	char    DevMaskMessage[100];		//�豸����

	int		VedionChannelCount;		//��Ƶͨ����
	int 	AudioChannelCount;			//��Ƶͨ����
	int 	AlarmChannelCount;			//����ͨ����

	char	ProductVersion[30];		// ��Ʒ�ͺ�

	char	TfTotalAndFreeSpace[30];	// TF�����ܿռ��ʣ��ռ�
	int		iTfFlageSta;				// TF���ı�ʶ״̬
	int		iAppHttpPort;				// Ӧ�ó�����ǶWeb Server��HTTP����˿ں�
	char	szHardVersion[50];			// Ӳ���汾��

}Server_Info_t;


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	BOOL InitUdpSearch();
	void RealseUdpSearch();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif