#include "UdpSearch.h"
#include "../Server/ServerDefine.h"

BOOL bQuitUdpSearch = FALSE;
BOOL bQuit = FALSE;

int g_udpScnSocket = -1;
struct sockaddr_in g_updScanPcAddr;
struct sockaddr_in LocalCastAddr;// ���ع㲥��ַ

int g_iVideochannel = 1;		// ��Ƶͨ����
int g_iAudiochannel = 1;		// ��Ƶͨ����
int g_iAlarmchannel = 1;		// ����ͨ����

int g_iAppHttpPort = 80;

// ����IP��ַ����
char g_szUdpMasterIps[200] = {0};
extern char g_serverNo[64];

// ���ͱ�����Ϣ�������ֻ���PC����˵�
void SendXmlDataInfo(S_Data s_data)
{
	S_Data stData;
	char szTemp[20] = {0};
	char System_MACAddress[16] = {0};	//MAC��ַ
	char Network_IPAddress[16] = {0};	//��������IP��ַ
	char Network_Subnet[16] = {0};		//��������Mask
	char Network_GateWay[16] = {0};		//Ĭ�����ػ�·������ַ
	char szBcastAddr[16] = {0};			//�㲥��ַ
	int i = 0;

	GetDeviceNetInfo(Network_IPAddress, Network_Subnet,System_MACAddress, Network_GateWay,szBcastAddr);

	memset(&stData, 0, sizeof(stData));
	stData.szCommandId = s_data.szCommandId;
	strcpy(stData.szCommandName, "1000");

	SetXmlValue(&stData, "ip", Network_IPAddress);		
	SetXmlValue(&stData, "gateWay", Network_GateWay );
	SetXmlValue(&stData, "mac", System_MACAddress);	
	SetXmlValue(&stData, "ipmask", Network_Subnet);
	//�豸���� �豸���� �豸�ͺ� �豸��
	SetXmlValue(&stData, "devicetype",DEVICETYPE);
	SetXmlValue(&stData, "deviceproduct",DEVICEPRODUCT);
	SetXmlValue(&stData, "devicemode",DEVICEMODEL);
	SetXmlValue(&stData, "deviceNo",g_serverNo);
	
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
}

// �����߳�
void SearchServerThread()
{
	bQuit = FALSE;
	int i;
	struct sockaddr_in udpBindAddr;	//���ذ󶨵�ַ
	struct sockaddr_in BroadcastAddr;	//�㲥��ַ

	socklen_t addrsize = sizeof(g_updScanPcAddr);
	fd_set fds;

	char data[1500] = {0};
	int recvnum;

	udpBindAddr.sin_family = AF_INET;
	udpBindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	udpBindAddr.sin_port = htons(SCAN_SERVER_PORT);//���ؽ��ն˿�

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
	//����һ���ಥ��
	nRet = setsockopt(g_udpScnSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char*)&mreq,sizeof(mreq));
	if( nRet)
	{
		LOGOUT("Join IP_ADD_MEMBERSHIP SUCC");
	}

	//���ÿɷ��͹㲥��
	int iOn = 1;
	nRet = setsockopt(g_udpScnSocket,SOL_SOCKET,SO_BROADCAST,&iOn,sizeof(iOn));	
	if ( nRet!=0 )
	{
		LOGOUT("setsockopt err=%d, %s\n",errno,strerror(errno));
	}
	else
	{
		LOGOUT("setsockopt OK\n");
	}
	
	BroadcastAddr.sin_family = AF_INET;
	BroadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	BroadcastAddr.sin_port = htons(PC_DEVTOOL_PORT);

	LocalCastAddr.sin_family = AF_INET;
	LocalCastAddr.sin_port = htons(PC_DEVTOOL_PORT);


	while (!bQuit) 
	{
		//�ȴ���������
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
				if(commandName == 1000)
				{// ����˷�����
					LOGOUT("center reply 1000 msg");
					int sendInfo = 0;
					while(!bQuit)
					{
						// �޸�PC����˺��ֻ�������������̣���û���յ�������Ϣ��ʱ�򣬽��з��������豸��Ϣ�Ĳ�����
						if (sendInfo > 3)
						{
							close(g_udpScnSocket);
							return;
						}
						SendXmlDataInfo(s_data);
						sendInfo++;
						FD_ZERO(&fds);
						//FD_CLR(udpscan_socket,&fds);
						FD_SET(g_udpScnSocket, &fds);
						tv.tv_sec = 1;
						tv.tv_usec = 0;
						if (select(g_udpScnSocket + 1, &fds, NULL, NULL, &tv))
						{
							if (FD_ISSET(g_udpScnSocket,&fds)) 
							{
								recvnum = recvfrom(g_udpScnSocket, data, 256, 0,
									(struct sockaddr *) &g_updScanPcAddr,
									&addrsize);
								if (strcmp(data, "command=1000,result=1")== 0)
								{
									LOGOUT("center receive serverinfo");
									break;
								}
							}
						}
						else
						{
							LOGOUT("client receive err %d,  %s", errno, strerror(errno));
						}							
					}
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
				LOGOUT("tmUdpScanTask:select err %d,  %s", errno, strerror(errno));
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
**������InitUdpSearch
**���ܣ���ʼ��UDP�����߳�
**������
        [in] - null
        [out] - null
**���أ�TRUE���ɹ���FALSE��ʧ��
        
**���ߣ��򺣲�, 2014-04-03
**��ע��
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

	UdpSearchParam udpSearchParamTest;
	udpSearchParamTest.szKernelVersion="szKernelVersion";
	udpSearchParamTest.szSeriaNo="szSeriaNo";
	udpSearchParamTest.szDevMask="szDevMask";
	udpSearchParamTest.szProductVer="szProductVer";
	udpSearchParamTest.szNodeId="szNodeId";
	pthread_detach(pThreadWork);

	return TRUE;
}

// �˳������߳�
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