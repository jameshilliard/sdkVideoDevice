#include "NetFunc.h"



// 判断网卡是否可用
BOOL  isValidNetworkCard(const char *v_szInterface, char *v_szDeviceIp, char *v_szDevMask, char *v_szDevHwMask,char *v_szBcastAddr)
{
	if((NULL == v_szInterface) || (NULL == v_szDeviceIp) || (NULL == v_szDevMask) 
		|| (NULL == v_szDevHwMask))
	{
		return FALSE;
	}
	int fd;  
	struct ifreq buf;  
	struct ifreq ifrcopy;  
	char mac[32] = {0};  
	char ip[32] = {0};  
	char subnetMask[32] = {0};  
	char szGateWay[32] = {0}; 

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  
	{  
		perror("socket");  
		close(fd);  
		return FALSE;  
	}  
	strcpy(buf.ifr_name , v_szInterface);
	//printf("\ndevice name: %s\n", buf.ifr_name);  
	//ignore the interface that not up or not runing  
	ifrcopy = buf;  
	if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy))  
	{  
		printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);  
		close(fd);  
		return FALSE;  
	}  
	//get the mac of this interface  
	if (!ioctl(fd, SIOCGIFHWADDR, (char *)(&buf)))  
	{  
		if(NULL != v_szDevHwMask)
		{
			memset(mac, 0, sizeof(mac));  
			snprintf(mac, sizeof(mac), "%02X-%02X-%02X-%02X-%02X-%02X",  
				(unsigned char)buf.ifr_hwaddr.sa_data[0],  
				(unsigned char)buf.ifr_hwaddr.sa_data[1],  
				(unsigned char)buf.ifr_hwaddr.sa_data[2],  
				(unsigned char)buf.ifr_hwaddr.sa_data[3],  
				(unsigned char)buf.ifr_hwaddr.sa_data[4],  
				(unsigned char)buf.ifr_hwaddr.sa_data[5]);  
			strcpy(v_szDevHwMask, mac);
			printf("device mac: %s\n", mac);  
		}
		
	}  
	else  
	{  
		printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);  
		close(fd);  
		return FALSE;  
	}  
	//get the IP of this interface  
	if (!ioctl(fd, SIOCGIFADDR, (char *)&buf))  
	{  
		if(NULL != v_szDeviceIp)
		{
			snprintf(ip, sizeof(ip), "%s",  
				(char *)inet_ntoa(((struct sockaddr_in *)&(buf.ifr_addr))->sin_addr));  
			printf("device ip: %s\n", ip);  
			strcpy(v_szDeviceIp, ip);
		}
		
	}  
	else  
	{  
		printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);  
		close(fd);  
		return FALSE;  
	}  
	//get the subnet mask of this interface  
	if (!ioctl(fd, SIOCGIFNETMASK, &buf))  
	{  
		if(NULL != v_szDevMask)
		{
			snprintf(subnetMask, sizeof(subnetMask), "%s",  
				(char *)inet_ntoa(((struct sockaddr_in *)&(buf.ifr_netmask))->sin_addr));  
			printf("device subnetMask: %s\n", subnetMask);  
			strcpy(v_szDevMask, subnetMask);
		}
	}  
	else  
	{  
		printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);  
		close(fd);  
		return FALSE;  
	}  

	if (!ioctl(fd, SIOCGIFBRDADDR, &buf))  
	{  
		if(NULL != v_szBcastAddr)
		{
			char bcastAddr[32] = {0};  
			snprintf(bcastAddr, sizeof(bcastAddr), "%s",  
				(char *)inet_ntoa(((struct sockaddr_in *)&(buf.ifr_broadaddr))->sin_addr));  
			printf("device broadaddr: %s\n", bcastAddr);  
			strcpy(v_szBcastAddr, bcastAddr);
		}
	}  
	close(fd);  

	return TRUE;  

}

/************************************************************************
**函数：ScanNetWorkName
**功能：搜索网卡名字
**参数：
        [in] -  v_szDirectory:网卡路径
        [out] - 
				v_szDeviceIp:返回IP
				v_szDevMask:子网掩码
				v_szDevHwMask：mac地址
**返回：FALSE:失败，TRUE：成功 
************************************************************************/
BOOL ScanNetWorkCardName(const char *v_szDirectory, char *v_szNetName, char *v_szDeviceIp, char *v_szDevMask, char *v_szDevHwMask,char *v_szBcastAddr)
{
	if((NULL == v_szDirectory)||(NULL == v_szNetName)||(NULL == v_szDeviceIp)||(NULL == v_szDevHwMask)||(NULL == v_szDevHwMask))
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	DIR *dp; 
	struct dirent *entry;
	struct stat statbuf;
	char *szNetWorkName = "eth0";
	if((dp = opendir(v_szDirectory)) == NULL)
	{
		perror("opendir");
		bRet = isValidNetworkCard(szNetWorkName, v_szDeviceIp, v_szDevMask, v_szDevHwMask,v_szBcastAddr);
		if(NULL != v_szNetName)
		{
			memcpy(v_szNetName, szNetWorkName, MIN(strlen(szNetWorkName), sizeof(v_szNetName)));
		}
		return bRet;
	}
	while ((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode))
		{
			if ((strcmp(entry->d_name, ".") != 0) &&
				(strcmp(entry->d_name, "..") != 0) &&
				(entry->d_name[0] != '.'))
			{
				printf("opendir  %s \n",entry->d_name);
				if(strcmp(entry->d_name,"eth0")==0)
				{
					bRet = isValidNetworkCard("eth0", v_szDeviceIp, v_szDevMask, v_szDevHwMask,v_szBcastAddr);
					if (bRet)
					{
						if(NULL != v_szNetName)
						{
							memcpy(v_szNetName, szNetWorkName, MIN(strlen(szNetWorkName), sizeof(v_szNetName)));
						}
						break;	
					}		
				}
				else
				{
					if(strcmp(entry->d_name,"lo")!=0)
					{
						bRet = isValidNetworkCard(entry->d_name, v_szDeviceIp, v_szDevMask, v_szDevHwMask,v_szBcastAddr);
						if (bRet)
						{
							if(NULL != v_szNetName)
							{
								memcpy(v_szNetName, entry->d_name, MIN(strlen(entry->d_name), sizeof(v_szNetName)));
							}
						}		
					}
				}
			}
		}
	}
	closedir(dp);

	return bRet;
}

// 获取网关地址
static void GetGateway(char *v_szGateWay)
{
	if(NULL == v_szGateWay)
	{
		return ;
	}
	char buff[256];
	int nl = 0;
	struct in_addr dest;
	struct in_addr gw;
	int flgs, ref, use, metric;
	unsigned long int d, g, m;
	char sgw[16];
	FILE *fp = fopen("/proc/net/route", "r");
	while (fgets(buff, sizeof (buff), fp) != NULL)
	{
		if (nl)
		{
			int ifl = 0;
			while (buff[ifl] != ' ' && buff[ifl] != '\t' && buff[ifl] != '\0')
				ifl++;
			buff[ifl] = 0; /* interface */
			if (sscanf(buff + ifl + 1, "%lx%lx%X%d%d%d%lx",
				&d, &g, &flgs, &ref, &use, &metric, &m) != 7)
			{
				fclose(fp);
				break;
			}
			if (flgs == RTF_UP)
			{
				dest.s_addr = d;
				gw.s_addr = g;
				memset(sgw, 0,sizeof(sgw));
				strcpy(sgw, (gw.s_addr == 0 ? "" : (char *) inet_ntoa(gw)));
				if (dest.s_addr == 0)
				{
					strcpy(v_szGateWay, sgw);
					//LOGOUT("GetGateway:%s\n", sgw);
					break;
				}
			}
		}
		nl++;
	}
	if(fp)
	{
		fclose(fp);
	}
}


// 从ifconfig中获取IP,物理地址，子网掩码
void GetNetInfo(char *v_szDevInfo, char *v_szBuf, const char *v_szType, int v_iOffset)
{
	if((NULL == v_szDevInfo) || (NULL == v_szBuf) || (NULL == v_szType))
	{
		LOGOUT("GetNetParam param err");
		return ;
	}
	
	char *p = NULL;
	char szBufInfo[20] = {0};
	int i = 0;
	p = strstr(v_szBuf, v_szType);
	if(p != NULL)
	{
		p += v_iOffset;		
		while(*p!=' ' && *p!='\n')// p不为空格和换行
		{
			if(*p == ':')
			{
				*p='-';
				//continue;
			}
			szBufInfo[i] = *p;
			i++;
			p++;				
		}
	}
	else
	{
		LOGOUT("Get %s err",v_szType);
	}

	strcpy(v_szDevInfo, szBufInfo);
}

// 获取网络信息
int GetDeviceNetInfo(char *v_szDeviceIp, char *v_szDevSubnet, char *v_szDevHwMask, char *v_szDevGw, char *v_szBcastAddr)
{
	if((NULL == v_szDeviceIp) || (NULL == v_szDevSubnet) || (NULL == v_szDevHwMask) || (NULL == v_szDevGw))
	{
		LOGOUT("GetLocalInfo param err");
		return -1;
	}

	// 获取网卡名字
	BOOL bRet = FALSE;
	char szNetWorkCardName[20] = {0};
	bRet = ScanNetWorkCardName(SYSCLASSNET, szNetWorkCardName, v_szDeviceIp, v_szDevSubnet, v_szDevHwMask,v_szBcastAddr);
	if(bRet)
	{
		GetGateway(v_szDevGw);
		LOGOUT("ifconfig netCardName:%s ipddr:%s hwMac:%s subnet:%s gw:%s",szNetWorkCardName,v_szDeviceIp,v_szDevHwMask,v_szDevSubnet,v_szDevGw);
	}
	else// 使用ifconfig获取网络信息
	{
		char cmd[128]={0};
		sprintf(cmd,"ifconfig %s > %s/netconfig", szNetWorkCardName, TEMPSAVEPATH);
		system(cmd);
		FILE *pNetFile = NULL;
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "%s/netconfig", TEMPSAVEPATH);
		pNetFile = fopen(cmd,"r");
		if(pNetFile)
		{
			char szBuf[1024] = {0};
			fread(szBuf, 1, 1024, pNetFile);
			GetNetInfo(v_szDeviceIp, szBuf, "inet addr:", 10);
			GetNetInfo(v_szDevHwMask, szBuf, "HWaddr ", 7);
			GetNetInfo(v_szDevSubnet, szBuf, "Mask:", 5);
			GetGateway(v_szDevGw);
			fclose(pNetFile);

			memset(cmd, 0, sizeof(cmd));
			sprintf(cmd, "rm -f %s/netconfig", TEMPSAVEPATH);
			system(cmd);
			LOGOUT("ifconfig netCardName:%s ipddr:%s hwMac:%s subnet:%s gw:%s",szNetWorkCardName,v_szDeviceIp,v_szDevHwMask,v_szDevSubnet,v_szDevGw);
		}
		else
			return -2;
	}
	return 0;
}


