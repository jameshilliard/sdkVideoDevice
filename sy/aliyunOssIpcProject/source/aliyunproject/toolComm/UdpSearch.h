#ifndef _SY_UDPSEARCH_H_
#define _SY_UDPSEARCH_H_

#include "../Common/Typedef.h"
#include "../Common/Configdef.h"
#include "../Common/GlobFunc.h"
#include "../Common/NetFunc.h"
#include "../mxml/mxmlparser.h"


#define MULTICAST_IP 			"235.255.255.250"	//多播组地址
// 网络参数
#define PC_TOOL_PORT 			60011
#define	SCAN_SERVER_PORT		60012
// TCP连接端口
#define SERVER_PORT 			60015

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	BOOL InitUdpSearch();
	void RealseUdpSearch();
#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif