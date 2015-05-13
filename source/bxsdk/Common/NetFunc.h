#ifndef _GLOBAFUNC_H_
#define _GLOBAFUNC_H_
#include "Typedef.h"
#include "../LogOut/LogOut.h"
#include "Configdef.h"

// DNS–≈œ¢
#define SYSCLASSNET		"/sys/class/net/"
#define RESOLV_CONF_PATH  "/etc/resolv.conf"  
#define MAX_DNS_SERVERS   2  
#define MAX_NET_STR_LEN   256  
#define RTF_UP 				0003
typedef struct dns_info_s 
{  
	char dns_svr[MAX_DNS_SERVERS][MAX_NET_STR_LEN];  
	char domain[MAX_NET_STR_LEN];  
} dns_info_t;  


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int  	GetDeviceNetInfo(char *v_szDeviceIp, char *v_szDevSubnet, char *v_szDevHwMask, char *v_szDevGw, char *v_szBcastAddr);
	int 	GetDnsInfo(dns_info_t *dns_info);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif

