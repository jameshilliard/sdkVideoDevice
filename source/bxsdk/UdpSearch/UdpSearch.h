#ifndef _SZY_UDPSEARCH_H_
#define _SZY_UDPSEARCH_H_

#include "../Common/Typedef.h"
#include "../Common/Configdef.h"
#include "../Common/GlobFunc.h"
#include "../Common/NetFunc.h"
#include "../mxml/mxmlparser.h"


#define MULTICAST_IP "245.255.255.250"	//多播组地址
// 网络参数
#define PC_DEVTOOL_PORT 		60011
#define	SCAN_SERVER_PORT		60012
// TCP连接端口
#define SERVER_PORT 			60014

typedef struct UdpSearchParam_
{
	char *szSeriaNo;		// 设备编号
	char *szLoginUser;		// 注册账号
	char *szDevMask;		// 设备名称
	char *szKernelVersion;	// 内核程序版本号
	char *szProductVer;	// 产品型号
	BOOL *bLogin;			// 是否登陆
	BOOL *bRegister;		// 是否注册
	char *szNodeId;			// 节点ID
}UdpSearchParam;

typedef struct Server_Info_t_
{
	char	System_MACAddress[16];	//MAC地址
	char	Network_IPAddress[16];	//服务器的IP地址
	char	Network_Subnet[16];		//子网掩码Mask
	char	Network_GateWay[16];	//默认网关或路由器地址
	char	System_FWVersion[24];		//含设备型号
	char	System_SerialNo[16];		//SerialNum

	char    LoginUser[30];				//登陆用户名
	char    DevMaskMessage[100];		//设备名称

	int		VedionChannelCount;		//视频通道数
	int 	AudioChannelCount;			//音频通道数
	int 	AlarmChannelCount;			//报警通道数

	char	ProductVersion[30];		// 产品型号

	char	TfTotalAndFreeSpace[30];	// TF卡的总空间和剩余空间
	int		iTfFlageSta;				// TF卡的标识状态
	int		iAppHttpPort;				// 应用程序内嵌Web Server的HTTP服务端口号
	char	szHardVersion[50];			// 硬件版本号

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