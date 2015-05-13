#include "UdpSearch.h"


BOOL bQuitUdpSearch = FALSE;
BOOL bQuit = FALSE;

int g_udpScnSocket = -1;
struct sockaddr_in g_updScanPcAddr;
struct sockaddr_in LocalCastAddr;// ���ع㲥��ַ

UdpSearchParam *g_p_objUdpSearchPar;
int g_iVideochannel = 1;		// ��Ƶͨ����
int g_iAudiochannel = 1;		// ��Ƶͨ����
int g_iAlarmchannel = 1;		// ����ͨ����

int g_iAppHttpPort = 80;

// ת���Ϳ��ƹ��ýṹ��ָ��,��Ϊ�˵õ����ص�����ͷ��Ϣ
//zss++RtpSendInfo *g_p_objUdpRtpSendInfo = NULL;

// ����IP��ַ����
char g_szUdpMasterIps[200] = {0};



// ��ȡ�豸����Ϣ������devtool��
void GetDevtoolDevInfo(Server_Info_t *v_ServerInfo, char *v_szBcastAddr)
{
	if(NULL == g_p_objUdpSearchPar)
	{
		return;
	}
	char	m_MACAddress[32];	//MAC��ַ
	GetDeviceNetInfo(v_ServerInfo->Network_IPAddress, v_ServerInfo->Network_Subnet
		, m_MACAddress, v_ServerInfo->Network_GateWay,v_szBcastAddr);
	squeeze(m_MACAddress,sizeof(m_MACAddress),'-');
	memcpy(v_ServerInfo->System_MACAddress,m_MACAddress,sizeof(v_ServerInfo->System_MACAddress));
	
	v_ServerInfo->Network_HTTPPort = SERVER_PORT;	

	v_ServerInfo->devType = DEVTYPE;
	
	char szTemp[100] = {0};
	if(NULL != g_p_objUdpSearchPar->szKernelVersion)
	{
		sprintf(szTemp,"%s_%s",g_p_objUdpSearchPar->szKernelVersion, SDK_SYSTEM_FWVERSION);
		strcpy(v_ServerInfo->System_FWVersion, szTemp);
	}
	if(NULL != g_p_objUdpSearchPar->szSeriaNo)
	{
		memcpy(v_ServerInfo->System_SerialNo, g_p_objUdpSearchPar->szSeriaNo, 
			MIN(sizeof(v_ServerInfo->System_SerialNo), strlen(g_p_objUdpSearchPar->szSeriaNo)));
	}
	if(NULL != g_p_objUdpSearchPar->szLoginUser)
	{
		memcpy(v_ServerInfo->LoginUser, g_p_objUdpSearchPar->szLoginUser, 
			MIN(sizeof(v_ServerInfo->LoginUser), strlen(g_p_objUdpSearchPar->szLoginUser)));
	}
	if(NULL != g_p_objUdpSearchPar->szDevMask)
	{
		memcpy(v_ServerInfo->DevMaskMessage, g_p_objUdpSearchPar->szDevMask, 
			MIN(sizeof(v_ServerInfo->DevMaskMessage), strlen(g_p_objUdpSearchPar->szDevMask)));
	}
	if(NULL != g_p_objUdpSearchPar->szProductVer)
	{
		memcpy(v_ServerInfo->ProductVersion, g_p_objUdpSearchPar->szProductVer, 
			MIN(sizeof(v_ServerInfo->ProductVersion), strlen(g_p_objUdpSearchPar->szProductVer)));
	}
	v_ServerInfo->AlarmChannelCount = g_iAlarmchannel;
	v_ServerInfo->AudioChannelCount = g_iAudiochannel;
	v_ServerInfo->VedionChannelCount = g_iVideochannel;

	v_ServerInfo->iAppHttpPort = g_iAppHttpPort;

	strcpy(v_ServerInfo->szHardVersion, SDK_HARD_FWVERSION);
}


// ���ͱ�����Ϣ�������ֻ���PC����˵�
void SendXmlDataInfo()
{
	S_Data stData;
	char szTemp[20] = {0};
	char System_MACAddress[16] = {0};	//MAC��ַ
	char Network_IPAddress[16] = {0};	//��������IP��ַ
	char Network_Subnet[16] = {0};		//��������Mask
	char Network_GateWay[16] = {0};		//Ĭ�����ػ�·������ַ
	char szBcastAddr[16] = {0};			//�㲥��ַ
	int i = 0;

	GetDeviceNetInfo(Network_IPAddress, Network_Subnet, System_MACAddress, Network_GateWay,szBcastAddr);

	memset(&stData, 0, sizeof(stData));
	stData.szCommandId = 501;
	strcpy(stData.szCommandName, "501");
	strcpy(stData.szType, "Cap-CS");


	SetXmlValue(&stData, "username", "szy");	
	SetXmlValue(&stData, "password", "szyipc");	


	char szVersion[128] = {0};
	sprintf(szVersion, "%s_%s_%s",SDK_HARD_FWVERSION,g_p_objUdpSearchPar->szKernelVersion, SDK_SYSTEM_FWVERSION);
	SetXmlValue(&stData, "version", szVersion);

	sprintf(szTemp, "%d", DEVTYPE);
	SetXmlValue(&stData, "devtype", szTemp);

	SetXmlValue(&stData, "ip", Network_IPAddress);		
	SetXmlValue(&stData, "gateWay", Network_GateWay );
	SetXmlValue(&stData, "mac", System_MACAddress);	

	if(NULL != g_p_objUdpSearchPar->szSeriaNo)
	{
		SetXmlValue(&stData, "deviceid", g_p_objUdpSearchPar->szSeriaNo);	
	}

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "%d", g_iAppHttpPort);
	SetXmlValue(&stData, "webport", szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "%d", LOCALVIDEO_PORT);
	SetXmlValue(&stData, "port", szTemp);

	if(NULL != g_p_objUdpSearchPar->szLoginUser)
	{
		SetXmlValue(&stData, "loginuser", g_p_objUdpSearchPar->szLoginUser);	
	}

	int iLoginStatus = 0;
	if((NULL !=g_p_objUdpSearchPar->bLogin) && (NULL != g_p_objUdpSearchPar->bRegister))
	{
		if((*g_p_objUdpSearchPar->bLogin) && (*g_p_objUdpSearchPar->bRegister))
		{
			iLoginStatus = 1;//��ע��
		}
		else if((*g_p_objUdpSearchPar->bLogin) && !(*g_p_objUdpSearchPar->bRegister))
		{
			iLoginStatus = 2;// δע��
		}
		else if(!(*g_p_objUdpSearchPar->bLogin))
		{
			iLoginStatus = 3;// ����
		}
		else
		{
			iLoginStatus = 3; 
		}
	}

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "%d", iLoginStatus);
	SetXmlValue(&stData, "loginstatus", szTemp);
	

	if(NULL != g_p_objUdpSearchPar->szNodeId)
	{
		SetXmlValue(&stData, "nodeid", g_p_objUdpSearchPar->szNodeId);
	}

	char szChannelList[128];
	char szCameraIdList[1024];
	memset(szCameraIdList, 0, sizeof(szCameraIdList));
	memset(szChannelList, 0, sizeof(szChannelList));
	memset(szTemp, 0, sizeof(szTemp));

	#if 0
	for(i = 0; i< g_p_objUdpRtpSendInfo->m_iTotalChn; i++)
	{
		sprintf(szTemp, "%d", i);
		strcat(szChannelList, szTemp);
		strcat(szChannelList, "&");
		if(strlen(g_p_objUdpRtpSendInfo->m_stRtpSendCameraInfo[i].m_szCameraID) != 0 )
		{
			strcat(szCameraIdList, g_p_objUdpRtpSendInfo->m_stRtpSendCameraInfo[i].m_szCameraID);
			strcat(szCameraIdList, "&");
		}
	}
	#endif

	SetXmlValue(&stData, "winidlist", szChannelList);

	SetXmlValue(&stData, "cameraidlist", szCameraIdList);

	SetXmlValue(&stData, "masterip", g_szUdpMasterIps);

	dns_info_t dns_info;  
	if (GetDnsInfo(&dns_info) >= 0)  
	{  
		SetXmlValue(&stData, "maindns", dns_info.dns_svr[0]);
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

	//������
	/*struct ifreq struIR;
	strncpy(struIR.ifr_name, g_szNetCard, IFNAMSIZ);

	if (setsockopt(g_udpScnSocket, SOL_SOCKET, SO_BINDTODEVICE, (char *)&struIR, sizeof(struIR)) < 0)
	{
		LOGOUT("bing netcard fail %d, %s", errno, strerror(errno));
		close (g_udpScnSocket);
		return;
	}*/


	//printf("setsockopt Oprate\r\n");
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
			//printf("tmUdpScanTask:select ok\n");
			if (FD_ISSET(g_udpScnSocket,&fds))
			{
				memset(data, 0, 1500);
				recvnum = recvfrom(g_udpScnSocket, data, 1500, 0,
					(struct sockaddr *)&g_updScanPcAddr, &addrsize);	
				if (strncmp(data, "pc scan", 7) == 0)// devtool������
				{
					char szBcastAddr[16] = {0};
					Server_Info_t ServerInfo;
					memset(&ServerInfo, 0, sizeof(ServerInfo));
					GetDevtoolDevInfo(&ServerInfo,szBcastAddr);
					//�����豸��Ϣ�㲥��
					sendto(g_udpScnSocket, (char *)&ServerInfo,
						sizeof(Server_Info_t), 0,
						(struct sockaddr *) &BroadcastAddr, //updscan_pcaddr,
						addrsize);

					LocalCastAddr.sin_addr.s_addr = inet_addr(szBcastAddr);
					sendto(g_udpScnSocket, (char *)&ServerInfo,
						sizeof(Server_Info_t), 0,
						(struct sockaddr *) &LocalCastAddr, //updscan_pcaddr,
						addrsize);

				} // pc scan
				else
				{
					// ���ص��ֽ���
					if (recvnum < 2 || recvnum > 1500)
					{
						LOGOUT("pc receive err %d,  %s", errno, strerror(errno));
						close (g_udpScnSocket);
						return;
					}
					S_Data s_data;
					DeCode(data,&s_data);
					int commandName = atoi(s_data.szCommandName);
					char szUserName[32] = {0};
					char szPassWord[32] = {0};
					char szDevType[32]  = {0};
					for(i = 0; i<s_data.iParamCount; i++ )
					{
						//LOGOUT("%s -- %s",s_data.params[i].szKey,s_data.params[i].szValue);
						if(!strcmp(s_data.params[i].szKey, "username"))
						{
							strcpy(szUserName, s_data.params[i].szValue);
						}

						if(!strcmp(s_data.params[i].szKey, "password"))
						{
							strcpy(szPassWord, s_data.params[i].szValue);
						}

						if(!strcmp(s_data.params[i].szKey, "devtype"))
						{
							strcpy(szDevType, s_data.params[i].szValue);
						}
					}

					FreeXmlValue(&s_data);

					if((commandName == 500) && (strcmp(szUserName, "szy") == 0) && (strcmp(szPassWord, "szyipc") == 0) 
						&& (strcmp(szDevType, "0") == 0))
					{// ����˷�����
						LOGOUT("center reply 500 msg");
						int sendInfo = 0;
						while(!bQuit)
						{
							// �޸�PC����˺��ֻ�������������̣���û���յ�������Ϣ��ʱ�򣬽��з��������豸��Ϣ�Ĳ�����
							if (sendInfo > 3)
							{
								close(g_udpScnSocket);
								return;
							}
							SendXmlDataInfo();
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
									if (strcmp(data, "pc receive serverinfo ok")== 0)
									{
										LOGOUT("center receive serverinfo");
										break;
									}
								}
							}
							else
							{
								LOGOUT("pc receive err %d,  %s", errno, strerror(errno));
							}							
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
	g_p_objUdpSearchPar=&udpSearchParamTest;
		
	pthread_detach(pThreadWork);

	g_iVideochannel = 1;		// ��Ƶͨ����
	g_iAudiochannel = 1;		// ��Ƶͨ����
	g_iAlarmchannel = 1;		// ����ͨ����

	return TRUE;
}

/************************************************************************
**������SetSearchParm
**���ܣ�������������
**������
        [in] - v_p_objUdpSearchPar�����������ṹ��ָ��

        
**���ߣ��򺣲�, 2014-04-16
**��ע��
       1). 
************************************************************************/
void SetSearchParm(UdpSearchParam *v_p_objUdpSearchPar)
{
	if(NULL == v_p_objUdpSearchPar)
	{
		return ;
	}
	
	g_p_objUdpSearchPar = v_p_objUdpSearchPar;
}


// ��ȡ����IP
void GetMasterIps(char *v_szMaskIps)
{
	if(v_szMaskIps == NULL)
	{
		return;
	}

	strcpy(g_szUdpMasterIps, v_szMaskIps);
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