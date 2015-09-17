#include "UdpSearch.h"
#include "../hisdk/hi_param.h"



BOOL 				bQuitUdpSearch = FALSE;
BOOL 				bQuit = FALSE;
int 				g_udpScnSocket = -1;
struct sockaddr_in 	g_updScanPcAddr;
struct sockaddr_in 	LocalCastAddr;// 本地广播地址
int 				g_iServerStatus=0;

static void SendXmlDataInfo(char *v_szBcastAddr,S_Data *v_pstData)
{
	XmlData stXmlData;
	stXmlData.m_iXmlLen = EnCode(stXmlData.szXmlDataBuf, sizeof(stXmlData.szXmlDataBuf), v_pstData);
	socklen_t addrsize = sizeof(g_updScanPcAddr);
	sendto(g_udpScnSocket, (BYTE *)stXmlData.szXmlDataBuf,stXmlData.m_iXmlLen , 0,
		(struct sockaddr *) &g_updScanPcAddr,
		addrsize);
	LocalCastAddr.sin_addr.s_addr = inet_addr(v_szBcastAddr);
	sendto(g_udpScnSocket, (BYTE *)stXmlData.szXmlDataBuf,stXmlData.m_iXmlLen , 0,
		(struct sockaddr *) &LocalCastAddr,
		addrsize);
}

static INT32S resolveCmd1000(S_Data *v_pstData,char *v_szDeviceIp, char *v_szDevSubnet, char *v_szDevHwMask, char *v_szDevGw)
{
	char szVersion[32] = {0};
	char szPort[32]={0};
	char szServerStatus[32]={0};
	char szServerNo[80]={0};
	char szServerPort[80]={0};
	char szSecret[80]={0};
	char szServer[236]={0};
	char szHttpPort[80]={0};
	int  iRet=-1;
	memset(szVersion,0,sizeof(szVersion));
	sprintf(szVersion,"%s",SDK_SYSTEM_FWVERSION);
	SetXmlValue(v_pstData, "version",szVersion);	
	SetXmlValue(v_pstData, "ip", v_szDeviceIp);		
	SetXmlValue(v_pstData, "gateway", v_szDevGw);
	SetXmlValue(v_pstData, "mac", v_szDevHwMask); 
	SetXmlValue(v_pstData, "ipmask", v_szDevSubnet);
	//设备类型 设备厂商 设备型号 设备码
	SetXmlValue(v_pstData, "devicetype",DE_DEVICETYPE);
	SetXmlValue(v_pstData, "deviceproduct",DE_DEVICEPRODUCT);
	SetXmlValue(v_pstData, "devicemode",DE_DEVICEMODEL);
	
	if(0!=strlen(g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword));
	{
		sprintf(szSecret,"%s",g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword);
		LOGOUT("get secret:%s",szSecret);
		SetXmlValue(v_pstData, "secret",szSecret);
	}
	if(0!=strlen(g_szServerNO))
	{
		strcpy(szServerNo,g_szServerNO);
		LOGOUT("get serverNo:%s",g_szServerNO);
		SetXmlValue(v_pstData, "deviceno",szServerNo);
	}
	if(0!=strlen(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP))
	{
		sprintf(szServer,"%s",g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP);
		LOGOUT("get server:%s",szServer);
		SetXmlValue(v_pstData, "server",szServer);
	}
	//sprintf(szServerPort,"%d",g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort);
	//LOGOUT("get port:%s",szServerPort);
	//SetXmlValue(v_pstData, "serverport",szServerPort);
	iRet=getStringParams(NETFILE,HttpPort,szHttpPort,sizeof(szHttpPort));
	if(iRet!=0)
		LOGOUT("get httpPort string is error iRet=%d",iRet);	
	SetXmlValue(v_pstData, "httpport",szHttpPort);
	
	memset(szServerStatus,0,sizeof(szServerStatus));
	sprintf(szServerStatus,"%d",g_iServerStatus);
	SetXmlValue(v_pstData, "serverstatus",szServerStatus);
	return 0;
}
static INT32S resolveCmd1002(S_Data *v_pSData,S_Data *v_pstData,char *v_szDevHwMask)
{
	int iRet=0;
	int i=0;
	int isValidPacket=0;
	char server[236]={0,};
	char port[80]={0,};
	char deviceId[80]={0,};
	char secret[80]={0,};
	INT32U iPort=0;
	for(i=0;i<v_pSData->iParamCount;i++)
	{
		if(strcmp(v_pSData->params[i].szKey,"mac")==0)
		{
			if(strcmp(v_pSData->params[i].szValue,v_szDevHwMask)==0)
			{
				isValidPacket=1;
			}
		}
		if(strcmp(v_pSData->params[i].szKey,"server")==0)
		{
			strcpy(server,v_pSData->params[i].szValue);
		}
		//if(strcmp(v_pSData->params[i].szKey,"port")==0)
		//{
		//	strcpy(port,v_pSData->params[i].szValue);
		//	iPort=atoi(port);
		//}
		if(strcmp(v_pSData->params[i].szKey,"id")==0)
		{
			strcpy(deviceId,v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"secret")==0)
		{
			strcpy(secret,v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"reset")==0)
		{
			int iRet=atoi(v_pSData->params[i].szValue);
			if(iRet==1)
			{
				ResetDeviceConfig(DEVICECONFIGDIR);
				g_main_start=FALSE;
				return 0;
			}
		}
	}
	if(isValidPacket==1)
	{
		LOGOUT("test");
		strncpy(g_szServerNO,deviceId,sizeof(g_szServerNO));
		SetServerNo(DEVICECONFIGDIR,g_szServerNO,strlen(g_szServerNO));
		//if(0!=strcmp(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,server) ||
		//   g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort!=iPort)
		if(0!=strcmp(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,server))
		{
			strncpy(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,server,sizeof(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP));
			//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort=iPort;
			SetServerIpAndPort(DEVICECONFIGDIR,server,sizeof(server),iPort);
			g_iServerStatus=-1;
		}
		if(0!=strcmp(g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,secret))
		{
			strncpy(g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,secret,sizeof(g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword));
			SetSecret(DEVICECONFIGDIR,secret,sizeof(secret));
			g_iServerStatus=-1;
		}
		SetXmlValue(v_pstData, "mac", v_szDevHwMask); 
		SetXmlValue(v_pstData, "result", "10000");
		iRet=0;
	}
	else
	{
		iRet=-1;
	}
	return iRet;
}
// 发送本地信息，发给手机和PC浏览端的
static void resoloveCmd(S_Data *sv_data)
{
	S_Data stData;
	char szTemp[20] = {0};
	char System_MACAddress[32] = {0};	//MAC地址
	char Network_IPAddress[32] = {0};	//服务器的IP地址
	char Network_Subnet[32] = {0};		//子网掩码Mask
	char Network_GateWay[32] = {0};		//默认网关或路由器地址
	char szBcastAddr[32] = {0};			//广播地址
	int i = 0;
	int isValidPacket=0;
	int iRet=-1;
	BOOL restart=FALSE;

	GetDeviceNetInfo(Network_IPAddress, Network_Subnet,System_MACAddress, Network_GateWay,szBcastAddr);
	memset(&stData, 0, sizeof(stData));
	stData.szCommandId = sv_data->szCommandId;
	strcpy(stData.szCommandName,sv_data->szCommandName);
	strcpy(stData.szType,sv_data->szType);
	if(strcmp(stData.szCommandName,"1000")==0)
	{
		iRet=resolveCmd1000(&stData,Network_IPAddress, Network_Subnet,System_MACAddress, Network_GateWay);
	}
	else if(strcmp(stData.szCommandName,"1002")==0)
	{
		iRet=resolveCmd1002(sv_data,&stData,System_MACAddress);
	}
	if(0==iRet)
	{
		SendXmlDataInfo(szBcastAddr,&stData);
	}
	FreeXmlValue(&stData);
}

// 搜索线程
void SearchServerThread()
{
	bQuit = FALSE;
	int i;
	struct sockaddr_in udpBindAddr;	//本地绑定地址
	struct sockaddr_in BroadcastAddr;	//广播地址

	socklen_t addrsize = sizeof(g_updScanPcAddr);
	fd_set fds;

	char data[1500] = {0};
	int recvnum;

	udpBindAddr.sin_family = AF_INET;
	udpBindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	udpBindAddr.sin_port = htons(SCAN_SERVER_PORT);//本地接收端口

	g_udpScnSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(g_udpScnSocket < 0)
	{
		LOGOUT("create socket err %d,  %s", errno, strerror(errno));
		return;
	}

	int nRet = 0;

	bind(g_udpScnSocket, (struct sockaddr *) &udpBindAddr,sizeof(udpBindAddr));

	struct ip_mreq mreq;
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_interface.s_addr = INADDR_ANY;
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
	//加入一个多播组
	nRet = setsockopt(g_udpScnSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char*)&mreq,sizeof(mreq));
	if( nRet)
	{
		LOGOUT("Join IP_ADD_MEMBERSHIP SUCC");
	}

	//设置可发送广播包
	int iOn = 1;
	nRet = setsockopt(g_udpScnSocket,SOL_SOCKET,SO_BROADCAST,&iOn,sizeof(iOn));	
	if ( nRet!=0 )
	{
		LOGOUT("setsockopt err=%d, %s",errno,strerror(errno));
	}
	else
	{
		LOGOUT("setsockopt OK");
	}
	
	BroadcastAddr.sin_family = AF_INET;
	BroadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	BroadcastAddr.sin_port = htons(PC_TOOL_PORT);

	LocalCastAddr.sin_family = AF_INET;
	LocalCastAddr.sin_port = htons(PC_TOOL_PORT);


	while (!bQuit) 
	{
		//等待接收数据
		FD_ZERO(&fds);
		FD_SET(g_udpScnSocket, &fds);
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		if (select(g_udpScnSocket + 1, &fds, NULL, NULL, &tv)) 
		{
			if (FD_ISSET(g_udpScnSocket,&fds))
			{
				memset(data, 0, 1500);
				recvnum = recvfrom(g_udpScnSocket, data, 1500, 0,(struct sockaddr *)&g_updScanPcAddr, &addrsize);
				//LOGOUT("receive %s",data);
				if (recvnum < 2 || recvnum > 1500)
				{
					LOGOUT("pc receive err %d,  %s", errno, strerror(errno));
					close(g_udpScnSocket);
					return;
				}
				S_Data s_data;
				DeCode(data,&s_data);
				int commandName = atoi(s_data.szCommandName);
				LOGOUT("center reply %d msg",commandName);
				if(commandName == 1000)
				{	//浏览端发来的
					resoloveCmd(&s_data);
				}
				else if(commandName == 1002)
				{
					resoloveCmd(&s_data);
				}
				FreeXmlValue(&s_data);
			} // FD_ISSET(udpscan_socket,&fds)
			else
			{
				if (errno == 98 || errno == 24)
				{
					close (g_udpScnSocket);
					return;
				}

				if(errno != 0 && errno != 2)
				{
					LOGOUT("FD_ISSET err %d,  %s", errno, strerror(errno));
				}
			}
		} // select(udpscan_socket+1,&fds,NULL,NULL,&tv)
		else
		{
			if (errno == 98 || errno == 24)
			{
				close (g_udpScnSocket);
				return;
			}
			if(errno != 0 && errno != 2)
			{
				//LOGOUT("tmUdpScanTask:select err %d,  %s", errno, strerror(errno));
			}
		}
	} // while(1)

	if(g_udpScnSocket != -1)
	{
		close (g_udpScnSocket);
	}
	
}

void *P_SearchServerThread()
{
	while (!bQuitUdpSearch)
	{
		SearchServerThread();
	}
	LOGOUT("Exit SearchServer");
	return NULL;
}

/************************************************************************
**函数：InitUdpSearch
**功能：初始化UDP搜索线程
**参数：
        [in] - null
        [out] - null
**返回：TRUE：成功，FALSE：失败
        
**作者：沈海波, 2014-04-03
**备注：
       1). 
************************************************************************/
int InitUdpSearch()
{
	bQuitUdpSearch = FALSE;
	bQuit = FALSE;
	int iRet;
	pthread_t pThreadWork;
	
	iRet = pthread_create(&pThreadWork, NULL, P_SearchServerThread, NULL);
	if(iRet)
	{
		LOGOUT("Create P_SearchServerThread Fail!!\n");
		return -1;
	}

	pthread_detach(pThreadWork);
	return 0;
}

// 退出搜索线程
void RealseUdpSearch()
{
	bQuitUdpSearch = TRUE;
	bQuit = TRUE;
	if(g_udpScnSocket != -1)
	{
		close(g_udpScnSocket);
		g_udpScnSocket = -1;
	}
	LOGOUT("Quit UdpSearch")
}
