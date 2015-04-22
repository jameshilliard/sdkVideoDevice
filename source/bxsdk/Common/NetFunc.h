#ifndef _GLOBAFUNC_H_
#define _GLOBAFUNC_H_

#include "Typedef.h"
#include "../LogOut/LogOut.h"

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

int  GetDeviceNetInfo(char *v_szDeviceIp, char *v_szDevSubnet, char *v_szDevHwMask, char *v_szDevGw, char *v_szBcastAddr);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif

