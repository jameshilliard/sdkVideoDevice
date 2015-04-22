#ifndef _ROUTESERVER_H_
#define _ROUTESERVER_H_

#include "ServerDefine.h"
#include "../Common/Typedef.h"

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int GetControlServer(LPCTSTR ip,int port,LPCTSTR user,LPTSTR controServerIP,int length);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif