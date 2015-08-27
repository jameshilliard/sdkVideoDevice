#include "UdpSearch.h"
//#include "../Server/ServerDefine.h"
#include "../hisdk/hi_param.h"

BOOL bQuitUdpSearch = FALSE;
BOOL bQuit = FALSE;

int g_udpScnSocket = -1;
struct sockaddr_in g_updScanPcAddr;
struct sockaddr_in LocalCastAddr;// 本地广播地址

char szLastServerNo[64]={0};
int  szLastServerPort[64]={0};
char szLastServer[200]={0};
char szLastHttpPort[64]={0};

int 	g_iServerStatus=0;
extern BOOL g_main_start;

// 发送本地信息，发给手机和PC浏览端的
void SendXmlDataInfo(S_Data sv_data)
{
	S_Data stData;
	char szTemp[20] = {0};
	char System_MACAddress[32] = {0};	//MAC地址
	char Network_IPAddress[32] = {0};	//服务器的IP地址
	char Network_Subnet[32] = {0};		//子网掩码Mask
	char Network_GateWay[32] = {0};		//默认网关或路由器地址
	char szBcastAddr[32] = {0};			//广播地址
	char szVersion[32] = {0};			//广播地址
	char szPort[32]={0};
	char szServerStatus[32]={0};
	char szServerNo[64]={0};
	int  szServerPort[64]={0};
	char szServer[200]={0};
	char szHttpPort[64]={0};
	
	int i = 0;
	int isValidPacket=0;
	char server[128]={0,};
	char port[16]={0,};
	int iRet=-1;
	BOOL restart=FALSE;

	GetDeviceNetInfo(Network_IPAddress, Network_Subnet,System_MACAddress, Network_GateWay,szBcastAddr);

	memset(&stData, 0, sizeof(stData));
	stData.szCommandId = sv_data.szCommandId;
	strcpy(stData.szCommandName,sv_data.szCommandName);
	if(strcmp(stData.szCommandName,"1000")==0)
	{
		memset(szVersion,0,sizeof(szVersion));
		sprintf(szVersion,"%s",SDK_SYSTEM_FWVERSION);
		SetXmlValue(&stData, "version",szVersion);	
		SetXmlValue(&stData, "ip", Network_IPAddress);		
		SetXmlValue(&stData, "gateway", Network_GateWay);
		SetXmlValue(&stData, "mac", System_MACAddress);	
		SetXmlValue(&stData, "ipmask", Network_Subnet);
		//设备类型 设备厂商 设备型号 设备码
		SetXmlValue(&stData, "devicetype",DEVICETYPE);
		SetXmlValue(&stData, "deviceproduct",DEVICEPRODUCT);
		SetXmlValue(&stData, "devicemode",DEVICEMODEL);
		
		iRet=getStringParams(CONFIGPLATFORM,ServerNoString,szServerNo,sizeof(szServerNo));
		if(iRet!=0)
			LOGOUT("getServerNo is error iRet=%d",iRet);
		int iRet=getStringParams(CONFIGPLATFORM,ServerString1,szServer,sizeof(szServer));
		if(iRet!=0)
			LOGOUT("get server1 string is error iRet=%d",iRet);
		iRet=getStringParams(CONFIGPLATFORM,ServerPort,szServerPort,sizeof(szServerPort));
		if(iRet!=0)
			LOGOUT("get port string is error iRet=%d",iRet);
		iRet=getStringParams(NETFILE,HttpPort,szHttpPort,sizeof(szHttpPort));
		if(iRet!=0)
			LOGOUT("get port string is error iRet=%d",iRet);	
		
		SetXmlValue(&stData, "deviceno",szServerNo);
		SetXmlValue(&stData, "server",szServer);
		SetXmlValue(&stData, "httpport",szHttpPort);
		SetXmlValue(&stData, "serverport",szServerPort);
		
		memset(szServerStatus,0,sizeof(szServerStatus));
		sprintf(szServerStatus,"%d",g_iServerStatus);
		SetXmlValue(&stData, "serverstatus",szServerStatus);

		if(strlen(szLastHttpPort)==0)
			strcpy(szLastHttpPort,szHttpPort);
		else
		{
			if(strcmp(szLastHttpPort,szHttpPort)!=0)
				restart=TRUE;
		}

		if(strlen(szLastServer)==0)
			strcpy(szLastServer,szServer);
		else
		{
			if(strcmp(szLastServer,szServer)!=0)
				restart=TRUE;
		}
		
		if(strlen(szLastServerPort)==0)
			strcpy(szLastServerPort,szServerPort);
		else
		{
			if(strcmp(szLastServerPort,szServerPort)!=0)
				restart=TRUE;
		}

		if(strlen(szLastServerNo)==0)
			strcpy(szLastServerNo,szServerNo);
		else
		{
			if(strcmp(szLastServerNo,szServerNo)!=0)
				restart=TRUE;
		}
		
	}
	else if(strcmp(stData.szCommandName,"1002")==0)
	{
		LOGOUT("test %d",sv_data.iParamCount);
		for(i=0;i<sv_data.iParamCount;i++)
		{
			if(strcmp(sv_data.params[i].szKey,"mac")==0)
			{
				if(strcmp(sv_data.params[i].szValue,System_MACAddress)==0)
				{
					LOGOUT("test");
					isValidPacket=1;
				}
			}
			if(strcmp(sv_data.params[i].szKey,"server")==0)
			{
				strcpy(server,sv_data.params[i].szValue);
			}
			if(strcmp(sv_data.params[i].szKey,"port")==0)
			{
				strcpy(port,sv_data.params[i].szValue);
			}
		}
		if(isValidPacket==1)
		{
			LOGOUT("test");
			//setMasterAndPort(server,atoi(port));
		}
		else
			return;
			
		SetXmlValue(&stData, "mac", System_MACAddress);	
		SetXmlValue(&stData, "result", "1");	
	}

	
	XmlData stXmlData;
	stXmlData.m_iXmlLen = EnCode(stXmlData.szXmlDataBuf, sizeof(stXmlData.szXmlDataBuf), &stData);

	socklen_t addrsize = sizeof(g_updScanPcAddr);

	sendto(g_udpScnSocket, (BYTE *)stXmlData.szXmlDataBuf,stXmlData.m_iXmlLen , 0,
		(struct sockaddr *) &g_updScanPcAddr,
		addrsize);

	LocalCastAddr.sin_addr.s_addr = inet_addr(szBcastAddr);
	sendto(g_udpScnSocket, (BYTE *)stXmlData.szXmlDataBuf,stXmlData.m_iXmlLen , 0,
		(struct sockaddr *) &LocalCastAddr,
		addrsize);

	FreeXmlValue(&stData);

	//if(restart)
	//	g_main_start=FALSE;
	
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
		LOGOUT("setsockopt err=%d, %s\n",errno,strerror(errno));
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
				recvnum = recvfrom(g_udpScnSocket, data, 1500, 0,
					(struct sockaddr *)&g_updScanPcAddr, &addrsize);
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
				char szUserName[32] = {0};
				char szPassWord[32] = {0};
				char szDevType[32]  = {0};
				FreeXmlValue(&s_data);
				LOGOUT("center reply %d msg",commandName);
				if(commandName == 1000)//commandName == 1002
				{// 浏览端发来的
					int sendInfo = 0;
					SendXmlDataInfo(s_data);
				}
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
BOOL InitUdpSearch()
{
	bQuitUdpSearch = FALSE;
	bQuit = FALSE;
	int iRet;
	pthread_t pThreadWork;
	
	iRet = pthread_create(&pThreadWork, NULL, P_SearchServerThread, NULL);
	if(iRet)
	{
		LOGOUT("Create P_SearchServerThread Fail!!\n");
		return FALSE;
	}

	pthread_detach(pThreadWork);
	return TRUE;
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
	LOGOUT("Quit UdpSearch");
}
