#include "TcpServer.h"

// 配置文件路径
char g_szFilePath[128] = {0};

// 内核和应用程序下载的路径
char g_szDownLoadPath[256] = {0};

// 下载的文件信息
stDownFileInfo g_stDwFileInfo;

// 内核版本信息
char g_szTcpKernelVer[40] = {0};

// 主控ip地址集合和主控端口
char g_szTcpMasterIps[200] = {0};
int g_iTcpMasterPort = 8006;

char g_szServerNO[32]={0,};

typedef struct SocketStatus_
{
	int m_iSocket;
	int m_iSocketType;
}SocketStatus;

// 网络参数
SocketStatus g_iConnectHost[CLIENTNUM];
int g_iConnectNum = 0;
int iSocket = -1;

BOOL g_bExitTcpServer = FALSE;
InnerDataList *g_DataList = NULL;

// 导出日志接收应答队列
InnerDataList *g_LogDataList = NULL;
// 导出日志标识
BOOL g_bUploadLogFile = FALSE;


// 存放下载的临时包
TcpRecvPacketBuf g_stRecvPacketBuf[CLIENTNUM];

// 初始化接收缓存
void TcpRecvBufInit(int v_iNum)
{
	g_stRecvPacketBuf[v_iNum].m_bFirstPacket = TRUE;
	g_stRecvPacketBuf[v_iNum].m_iCurLen = 0;
	g_stRecvPacketBuf[v_iNum].m_iTotalLen = 0;
	g_stRecvPacketBuf[v_iNum].m_pRecvBuf = NULL;
}

// 释放接收缓存
void TcpRecvBufRelease(int v_iNum)
{
	if(g_stRecvPacketBuf[v_iNum].m_pRecvBuf != NULL)
	{
		free(g_stRecvPacketBuf[v_iNum].m_pRecvBuf);
		g_stRecvPacketBuf[v_iNum].m_pRecvBuf = NULL;
	}
}

// socket线程
void *P_TcpServerSocketThread()
{
	int iWarkSta = T_TcpIdle;
	struct sockaddr_in stSocketLocal;
	int i = 0;
	int iRet = 0;

	while (!g_bExitTcpServer)
	{
		usleep(50000);
		switch(iWarkSta)
		{
		case T_TcpIdle:
			{
				for(i = 0; i< CLIENTNUM;i++)
				{
					g_iConnectHost[i].m_iSocket = -1;
					g_iConnectHost[i].m_iSocketType =-1;
				}
				g_iConnectNum = 0;
				iWarkSta = T_TcpConn;
			}
			break;

		case T_TcpConn:
			{
				iSocket = socket(AF_INET,SOCK_STREAM,0);
				memset(&stSocketLocal, 0, sizeof(stSocketLocal));
				stSocketLocal.sin_family = AF_INET;
				stSocketLocal.sin_addr.s_addr = htonl(INADDR_ANY);
				stSocketLocal.sin_port = htons(SERVER_PORT);
				
				int tmp = 1;
				setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int));

				iRet = bind(iSocket, (struct sockaddr*)&stSocketLocal, sizeof(stSocketLocal));
				if(iRet == -1)
				{
					perror("Tcp Server Bind err");
					iWarkSta = T_TcpDisConn;
					break;
				}
				iRet = listen(iSocket,BACKLOG);
				if(iRet == -1)
				{
					perror("Tcp Server listen err");
					iWarkSta = T_TcpDisConn;
					break;
				}
				iWarkSta = T_TcpAccepting;
			}
			break;

		case T_TcpAccepting:
			{
				int iSocketClient = -1;
				struct sockaddr_in stFromAdd;
				int iLen = sizeof(stFromAdd);
				fd_set fds;
				FD_ZERO(&fds);
				FD_SET(iSocket, &fds);
				struct timeval tv;
				tv.tv_sec = 5;
				tv.tv_usec = 0;

				if (!select(iSocket + 1, &fds, NULL, NULL, &tv)) 
				{
					continue;
				}
				iSocketClient = accept(iSocket,(struct sockaddr *)&stFromAdd, &iLen);
				printf("a client connect%d, from:%s\n",iSocketClient,inet_ntoa(stFromAdd.sin_addr));
				int iSendBuf = 64*1024;//设置为64k
				setsockopt(iSocketClient,SOL_SOCKET,SO_RCVBUF,(const char*)&iSendBuf,sizeof(int));
				for(i = 0; i < CLIENTNUM; i++)
				{
					if(g_iConnectHost[i].m_iSocket == -1)
					{
						g_iConnectHost[i].m_iSocket = iSocketClient;
						g_iConnectHost[i].m_iSocketType =-1;
						g_iConnectNum ++;
						TcpRecvBufInit(i);
						LOGOUT("get socket %d %d\n",i,g_iConnectHost[i].m_iSocket);
						break;
					}
				}
			}
			break;

		case T_TcpDisConn:
			{
				iWarkSta = T_TcpIdle;

				for(i = 0; i < CLIENTNUM; i++)
				{
					if(g_iConnectHost[i].m_iSocket != -1)
					{
						close(g_iConnectHost[i].m_iSocket);
						TcpRecvBufRelease(i);
					}
				}
				if(-1 != iSocket)
				{
					close(iSocket);
				}
			}
			break;
		}
	}
	LOGOUT("Exit TcpServerSocket");
	return NULL;
}

// 接收线程
void *P_TcpServerRecvThread()
{
	char szRecvBuf[BUFFLEN+100];
	int iRecvLen = 0;
	time_t now;

	int iMaxFd = -1;
	fd_set scanfd;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	int i = 0;
	int j=0;
	int iErr = -1;
	while (!g_bExitTcpServer)
	{
		usleep(30*1000);
		iMaxFd = -1;
		FD_ZERO(&scanfd);
		for(i = 0; i<CLIENTNUM; i++)
		{
			if(g_iConnectHost[i].m_iSocket != -1)
			{
				FD_SET(g_iConnectHost[i].m_iSocket, &scanfd);
				if(iMaxFd < g_iConnectHost[i].m_iSocket)
				{
					iMaxFd = g_iConnectHost[i].m_iSocket;
				}
			}
		}

		iErr = select(iMaxFd+1, &scanfd, NULL, NULL, &timeout);
		switch(iErr)
		{
		case 0:
			break;
		case -1:
			break;
		default:
			if(g_iConnectNum <= 0)
			{
				break;
			}
			for(i = 0; i < CLIENTNUM; i++)
			{
				if(g_iConnectHost[i].m_iSocket != -1)
				{
					if(FD_ISSET(g_iConnectHost[i].m_iSocket, &scanfd))
					{
						DWORD dwPacketNum = 0;
						memset(szRecvBuf, 0, sizeof(szRecvBuf));
						iRecvLen = recv(g_iConnectHost[i].m_iSocket, szRecvBuf, sizeof(szRecvBuf), 0);	
						if(iRecvLen > 0 )
						{
							if(g_stRecvPacketBuf[i].m_bFirstPacket)
							{
								memcpy(&g_stRecvPacketBuf[i].m_iTotalLen, szRecvBuf, 4);
								g_stRecvPacketBuf[i].m_iTotalLen += 4;
								if(g_stRecvPacketBuf[i].m_iTotalLen == iRecvLen)
								{
									g_DataList->pushdata(g_DataList->_this, 1, iRecvLen, szRecvBuf, g_iConnectHost[i].m_iSocket, &dwPacketNum);
									g_stRecvPacketBuf[i].m_iTotalLen = 0;
									break;
								}
								else
								{
									g_stRecvPacketBuf[i].m_pRecvBuf = (char *)malloc(TCPXMLDATALEN);
									g_stRecvPacketBuf[i].m_bFirstPacket = FALSE;
								}
							}

							memcpy(g_stRecvPacketBuf[i].m_pRecvBuf + g_stRecvPacketBuf[i].m_iCurLen, szRecvBuf, iRecvLen);
							g_stRecvPacketBuf[i].m_iCurLen += iRecvLen;

							if(g_stRecvPacketBuf[i].m_iTotalLen == g_stRecvPacketBuf[i].m_iCurLen)
							{
								g_DataList->pushdata(g_DataList->_this, 1, g_stRecvPacketBuf[i].m_iTotalLen, g_stRecvPacketBuf[i].m_pRecvBuf, g_iConnectHost[i].m_iSocket, &dwPacketNum); 
								g_stRecvPacketBuf[i].m_bFirstPacket = TRUE;
								g_stRecvPacketBuf[i].m_iCurLen = 0;
								g_stRecvPacketBuf[i].m_iTotalLen = 0;
							}
						}
						else
						{
							close(g_iConnectHost[i].m_iSocket);
							g_iConnectHost[i].m_iSocket = -1;
							g_iConnectHost[i].m_iSocketType =-1;
							g_iConnectNum--;
							TcpRecvBufRelease(i);
						}
					}
				}
			}
			break;
		}
	}

	if(iSocket != -1)
	{
		close(iSocket);
	}
	LOGOUT("Exit TcpServerRecv");
	return NULL;
}

// 发送TCP的xml数据
void TcpSendCmdData(int v_iSocket, S_Data *v_stData)
{
	TcpXmlData stSendXmlData;

	stSendXmlData.m_iXmlLen = EnCode(stSendXmlData.szXmlDataBuf, sizeof(stSendXmlData.szXmlDataBuf), v_stData);
	stSendXmlData.m_iXmlLen += 2;// 数据长度（xml类型+xml数据内容长度+结束符长度）
	stSendXmlData.m_bytXmlType = 0;
	SzySdkSendData(v_iSocket, (char*)&stSendXmlData, stSendXmlData.m_iXmlLen + 4);
	//send(v_iSocket, (char*)&stSendXmlData, stSendXmlData.m_iXmlLen + 4, 0);
}
#if 0
/************************************************************************
**函数：SetDeviceOrProduceId
**功能：设置设备编号或者产品型号函数
**参数：
        [in] - v_stDeCodeData：xml信息结构体
        [out] - v_stXmlData：返回应答结果
**返回：void
        
**作者：沈海波, 2014-04-10
**备注：
       1). devtool协议ID，设备编号：5005，产品型号：5006
************************************************************************/
void SetDeviceOrProduceId(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	char pType[32] = {0};
	char pTypeProduceId[] = "newDeviceNumber";
	int iSetType = 0;
	if(v_stDeCodeData->szCommandId == 5005)
	{
		strcpy(pType, "newDeviceNumber");
		iSetType = 0;
	}
	else
	{
		strcpy(pType, "productver");
		iSetType = 1;
	}
	if(!strcmp(v_stDeCodeData->params[0].szKey, pType))	
	{
		BOOL bRet;
		char szPath[256] = {0};
		if(v_stDeCodeData->szCommandId == 5005)
		{
			sprintf(szPath, "%s/%s",g_szFilePath, SERVERNOFILENAME);
		}
		else
		{
			sprintf(szPath, "%s/%s",g_szFilePath,PRODUCTFILENAME);
		}

		bRet = CreateConfigFile(szPath, v_stDeCodeData->params[0].szValue, strlen(v_stDeCodeData->params[0].szValue));

		if(bRet)
		{
			SetDeviceMask(v_stDeCodeData->params[0].szValue, iSetType);
		}
		memset(&stDataEnCode, 0, sizeof(stDataEnCode));

		stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
		sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
		strcpy(stDataEnCode.szType , CONNETTYPE);
		SetXmlValue(&stDataEnCode, "result", bRet?"10000":"0");

		TcpSendCmdData(v_iSocket, &stDataEnCode);
		FreeXmlValue(&stDataEnCode);
	}
}


// 密码校验
void CheckPassword(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	BOOL bRet = TRUE;
	if(!strcmp(v_stDeCodeData->params[0].szKey, "passwd"))	
	{
		char szPath[256] = {0};
		sprintf(szPath, "%s/%s",g_szFilePath, PASSWORDFILE);
		char szLocalPasswd[100] = {0};
		GetPassword(szPath, szLocalPasswd);
				
		memset(&stDataEnCode, 0, sizeof(stDataEnCode));
		stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
		sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
		strcpy(stDataEnCode.szType , CONNETTYPE);

		if(!strncmp(v_stDeCodeData->params[0].szValue, szLocalPasswd, MIN(MD5LEN+strlen(PASSWDMD5FLAG), strlen(szLocalPasswd))))
		{
			bRet = TRUE;
		}
		else
		{
			bRet = FALSE;
		}
		
		SetXmlValue(&stDataEnCode, "result", bRet?"OK":"NO");

		TcpSendCmdData(v_iSocket, &stDataEnCode);
		FreeXmlValue(&stDataEnCode);
	}
}

// 修改密码
void ModifyPassword(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}

	S_Data stDataEnCode;
	int i= 0;
	BOOL bRet = TRUE; 
	char szPath[256] = {0};
	sprintf(szPath, "%s/%s",g_szFilePath, PASSWORDFILE);

	for (i = 0; i<v_stDeCodeData->iParamCount; i++)
	{
		if(!strcmp(v_stDeCodeData->params[i].szKey, "oldpasswd"))	
		{
			char szLocalPasswd[100] = {0};
			GetPassword(szPath, szLocalPasswd);
			if(strncmp(v_stDeCodeData->params[i].szValue, szLocalPasswd, 
				MIN(MD5LEN+strlen(PASSWDMD5FLAG), strlen(szLocalPasswd))))
			{
				bRet = FALSE;
			}
			break;
		}
	}

	if(bRet)
	{
		for (i = 0; i<v_stDeCodeData->iParamCount; i++)
		{
			if(!strcmp(v_stDeCodeData->params[i].szKey, "newpasswd"))
			{
				FILE *fp;
				fp = fopen(szPath, "w+");
				if(fp == NULL)
				{
					LOGOUT("create %s failed , fopen error" ,szPath);
					bRet = FALSE;
				}

				int v_iLen = MD5LEN + strlen(PASSWDMD5FLAG);
				unsigned int len = fwrite(v_stDeCodeData->params[i].szValue, 1, v_iLen, fp);
				if(len != v_iLen )
				{
					LOGOUT("create %s failed , fwrite len error", szPath);
					bRet = FALSE;
				}
				if(fp != NULL)
				{
					fclose(fp);
				}	
			}
		}
	}
	
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
	strcpy(stDataEnCode.szType , CONNETTYPE);
	SetXmlValue(&stDataEnCode, "result", bRet?"OK":"NO");

	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

// 获取网络参数
void GetNetParam(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	char szPort[20] = {0};

	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
	strcpy(stDataEnCode.szType , CONNETTYPE);

	tagMasterServerCfg objGetMasterInfo;	
	GetCfgFile(&objGetMasterInfo, sizeof(tagMasterServerCfg), offsetof(tagConfigCfg, m_unMasterServerCfg.m_objMasterServerCfg), sizeof(tagMasterServerCfg));
	
	SetXmlValue(&stDataEnCode, "MasterIp", objGetMasterInfo.m_szMasterIP);
	if(!strcmp(objGetMasterInfo.m_szMasterIP, DE_MASTERIP))
	{
		SetXmlValue(&stDataEnCode, "AllMsterIp", g_szTcpMasterIps);
		sprintf(szPort, "%d", g_iTcpMasterPort);
	}
	else
	{
		sprintf(szPort, "%d", objGetMasterInfo.m_iMasterPort);
	}

	SetXmlValue(&stDataEnCode, "MasterPort", szPort);

	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

// 设置网络参数
void SetNetParam(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	int i = 0;
	int iRet = 0;
	BOOL bChangeParm = FALSE;
	tagMasterServerCfg objSetMasterInfo = {0};	
	GetCfgFile(&objSetMasterInfo, sizeof(tagMasterServerCfg), offsetof(tagConfigCfg, m_unMasterServerCfg.m_objMasterServerCfg), sizeof(tagMasterServerCfg));
	for(i = 0; i< v_stDeCodeData->iParamCount;i++)
	{
		if(!strcmp(v_stDeCodeData->params[i].szKey, "MasterIp"))	
		{
			if(strcmp(objSetMasterInfo.m_szMasterIP, v_stDeCodeData->params[i].szValue))
			{
				bChangeParm = TRUE;
				memset(objSetMasterInfo.m_szMasterIP, 0, sizeof(objSetMasterInfo.m_szMasterIP));
				strcpy(objSetMasterInfo.m_szMasterIP, v_stDeCodeData->params[i].szValue);
			}
		}

		if(!strcmp(v_stDeCodeData->params[i].szKey, "MasterPort"))	
		{
			int iPort;
			iPort = atoi(v_stDeCodeData->params[i].szValue);
			if(iPort != objSetMasterInfo.m_iMasterPort)
			{
				bChangeParm = TRUE;
				objSetMasterInfo.m_iMasterPort = iPort;
			}
		}
	}

	if(bChangeParm)
	{
		iRet = SetCfgFile(&objSetMasterInfo, offsetof(tagConfigCfg, m_unMasterServerCfg.m_objMasterServerCfg), sizeof(tagMasterServerCfg));
		ModifyMasterSerInfo(objSetMasterInfo.m_szMasterIP, objSetMasterInfo.m_iMasterPort);
	
		if(!strcmp(objSetMasterInfo.m_szMasterIP, DE_MASTERIP))
		{
			GetMasterIps(g_szTcpMasterIps);
		}
		else
		{
			GetMasterIps(objSetMasterInfo.m_szMasterIP);
		}
	}

	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
	strcpy(stDataEnCode.szType , CONNETTYPE);
	strcpy(stDataEnCode.params[stDataEnCode.iParamCount].szKey, "result");
	
	SetXmlValue(&stDataEnCode, "result", iRet?"0":"10000");

	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

// 重启系统
void SZYRebootSystem(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}

	S_Data stDataEnCode;

	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
	strcpy(stDataEnCode.szType , CONNETTYPE);

	SetXmlValue(&stDataEnCode, "result", "10000");

	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);

	if(SetSZYSdkRet_OK == g_pSystemRebootFun())
	{
		SZYIPCSdk_Release();
	}
}


// 设置网络参数
void SzySetIpInfo(int  v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}

	int iIsAutoIp = 0;
	char szIpAddr[16] = {0};
	char szSubNetWork[16] = {0};
	char szGateWay[16] = {0};
	char szMainDns[16] = {0};
	char szMacAddr[32] = {0};
	int iRet = 0;

	int i = 0;
	for(i = 0; i< v_stDeCodeData->iParamCount; i++)
	{
		if(!strcmp(v_stDeCodeData->params[i].szKey, "ipAddr"))	
		{
			strcpy(szIpAddr, v_stDeCodeData->params[i].szValue);
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "netmask"))	
		{
			strcpy(szSubNetWork, v_stDeCodeData->params[i].szValue);
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "gateway"))	
		{
			strcpy(szGateWay, v_stDeCodeData->params[i].szValue);
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "macAddr"))	
		{
			strcpy(szMacAddr, v_stDeCodeData->params[i].szValue);
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "mainDns"))	
		{
			strcpy(szMainDns, v_stDeCodeData->params[i].szValue);
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "isAutoIp"))	
		{
			iIsAutoIp = atoi(v_stDeCodeData->params[i].szValue);
		}
	}

	if(g_pGetNetWorkFun != NULL)
	{
		int iIsGetAutoIp = 0;
		char szGetIpAddr[16] = {0};
		char szGetSubNetWork[16] = {0};
		char szGetGateWay[16] = {0};
		char szGetMainDns[16] = {0};
		char szGetMacAddr[32] = {0};

		if(SetSZYSdkRet_OK == g_pGetNetWorkFun(&iIsGetAutoIp,szGetIpAddr,szGetSubNetWork,szGetGateWay,szGetMacAddr,szGetMainDns))
		{
			BOOL bModifyParm = FALSE;
			if(iIsAutoIp != iIsGetAutoIp)
			{
				bModifyParm = TRUE;
			}
			if((strlen(szIpAddr) != 0) && strcmp(szIpAddr,szGetIpAddr))
			{
				bModifyParm = TRUE;
			}
			if((strlen(szSubNetWork) != 0) && strcmp(szSubNetWork,szGetSubNetWork))
			{
				bModifyParm = TRUE;
			}
			if((strlen(szGateWay) != 0) && strcmp(szGateWay,szGetGateWay))
			{
				bModifyParm = TRUE;
			}
			if((strlen(szMacAddr) != 0) && strcmp(szMacAddr,szGetMacAddr))
			{
				bModifyParm = TRUE;
			}
			if((strlen(szMainDns) != 0) && strcmp(szMainDns,szGetMainDns))
			{
				bModifyParm = TRUE;
			}

			if((g_pSetNetWorkFun != NULL) && bModifyParm)
			{
				if(SetSZYSdkRet_OK == g_pSetNetWorkFun(iIsAutoIp,szIpAddr,szSubNetWork,szGateWay,szMacAddr,szMainDns))
				{
					iRet = 1;
				}
			}
		}
	}

	LOGOUT("szIpAddr:%s,iIsAutoIp=%d,iRet=%d",szIpAddr,iIsAutoIp,iRet);
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
	strcpy(stDataEnCode.szType , CONNETTYPE);

	SetXmlValue(&stDataEnCode, "result", iRet?"10000":"0");

	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

// 获取网络参数
void SzyGetIpInfo(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	char szTemp[16] = {0};

	int iIsAutoIp = 0;
	char szIpAddr[16] = {0};
	char szSubNetWork[16] = {0};
	char szGateWay[16] = {0};
	char szMainDns[16] = {0};
	char szMacAddr[32] = {0};
	int iRet = 0;

	if(g_pGetNetWorkFun != NULL)
	{
		if(SetSZYSdkRet_OK == g_pGetNetWorkFun(&iIsAutoIp
			,szIpAddr
			,szSubNetWork
			,szGateWay
			,szMacAddr
			,szMainDns))
		{
			iRet = 1;
		}
	}

	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
	strcpy(stDataEnCode.szType , CONNETTYPE);

	if(1 == iRet)
	{
		sprintf(szTemp, "%d", iIsAutoIp);
		SetXmlValue(&stDataEnCode, "isAutoIp", szTemp);
		SetXmlValue(&stDataEnCode, "ipAddr", szIpAddr);
		SetXmlValue(&stDataEnCode, "netmask", szSubNetWork);
		SetXmlValue(&stDataEnCode, "gateway", szGateWay);
		SetXmlValue(&stDataEnCode, "macAddr", szMacAddr);
		SetXmlValue(&stDataEnCode, "mainDns", szMainDns);
		SetXmlValue(&stDataEnCode, "result", "10000");
	}
	else

	{
		SetXmlValue(&stDataEnCode, "result", "0");
	}
	
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

// 设置码流类型
void SzySetVideoParam(int  v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}

	int iChannel = 0;
	int iStreamType = 0;
	int iStreamCtrlType = 0;
	BOOL bParamError = FALSE;
	unsigned int iWith = 0;
	unsigned int iHeight = 0;
	unsigned int iStream = 0;
	unsigned int iFrate = 0;
	unsigned int iQuality = 0;
	unsigned int iKeyFrame = 0;

	int i = 0;
	for(i = 0; i< v_stDeCodeData->iParamCount; i++)
	{

		if(!strcmp(v_stDeCodeData->params[i].szKey, "channel"))	
		{
			iChannel = atoi(v_stDeCodeData->params[i].szValue);
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "streamType"))	
		{
			iStreamType = atoi(v_stDeCodeData->params[i].szValue);
			if( iStreamType != 0  && iStreamType != 1 )
			{
				bParamError = TRUE;
				break;
			}
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "streamCtrlType"))	
		{
			iStreamCtrlType = atoi(v_stDeCodeData->params[i].szValue);
			if( iStreamCtrlType != 0  && iStreamCtrlType != 1 )
			{
				bParamError = TRUE;
				break;
			}
		}

		if(!strcmp(v_stDeCodeData->params[i].szKey, "resolution"))	
		{
			char *szResolution[2];
			split(szResolution, v_stDeCodeData->params[i].szValue, "X", 2);
			iWith = atoi(szResolution[0]);
			iHeight = atoi(szResolution[1]);
			if((iWith == 0) || (iHeight == 0))
			{
				bParamError = TRUE;
			}
		}

		if(!strcmp(v_stDeCodeData->params[i].szKey, "stream"))	
		{
			iStream = atoi(v_stDeCodeData->params[i].szValue);
			if( iStream < 50 || iStream > 2000 )
			{
				bParamError = TRUE;
				break;
			}
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "frate"))	
		{
			iFrate = atoi(v_stDeCodeData->params[i].szValue);
			if( iFrate < 1 || iFrate > 30 )
			{
				bParamError = TRUE;
				break;
			}
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "quality"))	
		{
			iQuality = atoi(v_stDeCodeData->params[i].szValue);
			if( iQuality < 22 || iQuality > 36 )
			{
				bParamError = TRUE;
				break;
			}
		}
		if(!strcmp(v_stDeCodeData->params[i].szKey, "keyFrame"))	
		{
			iKeyFrame = atoi(v_stDeCodeData->params[i].szValue);
			if( iKeyFrame < 25 || iKeyFrame > 128 )
			{
				bParamError = TRUE;
				break;
			}
		}
		
	}

	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
	strcpy(stDataEnCode.szType , CONNETTYPE);

	if(bParamError)
	{
		SetXmlValue(&stDataEnCode, "result", "0");
		LOGOUT("****set  cameraid error*****");
	}
	else
	{
		SetXmlValue(&stDataEnCode, "result", "10000");
		LOGOUT("****set  cameraid success*****");
	}

	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
	
	if(!bParamError)
	{
		int iGetStreamType = 0;
		int iGetStreamCtrlType = 0;

		unsigned int uGetBiteRate = 0;
		unsigned int uGetiQpConstant = 0;
		unsigned int uGetiFrameRate = 0;
		unsigned int uGetiKeyFrameRate = 0;
		unsigned int uGetiWith = 0;
		unsigned int uGetiHeight = 0;

		if(g_pGetLocalVideoParamFun != NULL)
		{
			g_pGetLocalVideoParamFun(iChannel, &iGetStreamType, &iGetStreamCtrlType, &(uGetBiteRate), 
				&(uGetiFrameRate), &(uGetiQpConstant), &(uGetiKeyFrameRate),&(uGetiWith), &(uGetiHeight));
		}
		if((iGetStreamType != iStreamType) && (iGetStreamCtrlType != iStreamCtrlType) 
			&&(uGetBiteRate != iStream) && (uGetiFrameRate != iFrate) && (iKeyFrame != uGetiKeyFrameRate)
			&&(iWith != uGetiWith) && (uGetiHeight != iHeight))
		{
			if(g_pSetLocalVideoParamFun != NULL)
			{
				g_pSetLocalVideoParamFun(iChannel, iStreamType, iStreamCtrlType,iStream,
					iFrate,iQuality,iKeyFrame,iWith,iHeight);
			}
		}	
	}
	
}

// 获取视频参数
void SzyGetVideoParam(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	char szTemp[16] = {0};

	int iChannel = 0;
	int i = 0;
	for(i = 0; i< v_stDeCodeData->iParamCount; i++)
	{

		if(!strcmp(v_stDeCodeData->params[i].szKey, "channel"))	
		{
			iChannel = atoi(v_stDeCodeData->params[i].szValue);
		}
	}

	int iStreamType = 0;
	int iStreamCtrlType = 0;

	unsigned int uBiteRate = 0;
	unsigned int uiQpConstant = 0;
	unsigned int uiFrameRate = 0;
	unsigned int uiKeyFrameRate = 0;
	unsigned int uiWith = 0;
	unsigned int uiHeight = 0;
	int iRet = 0;

	if(g_pGetLocalVideoParamFun != NULL)
	{
		if(SetSZYSdkRet_OK == g_pGetLocalVideoParamFun(iChannel, &iStreamType, &iStreamCtrlType, &(uBiteRate), &(uiFrameRate), &(uiQpConstant), &(uiKeyFrameRate),&(uiWith), &(uiHeight)))
		{
			iRet = 1;
		}
	}
	
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	sprintf(stDataEnCode.szCommandName, "%d",stDataEnCode.szCommandId);
	strcpy(stDataEnCode.szType , CONNETTYPE);

	if(1 == iRet)
	{
		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", iChannel);
		SetXmlValue(&stDataEnCode, "channel", szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", iStreamType);
		SetXmlValue(&stDataEnCode, "streamType", szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", iStreamCtrlType);
		SetXmlValue(&stDataEnCode, "streamCtrlType", szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp,"%dX%d", uiWith, uiHeight);
		SetXmlValue(&stDataEnCode, "resolution", szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", uBiteRate);
		SetXmlValue(&stDataEnCode, "stream", szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", uiFrameRate);
		SetXmlValue(&stDataEnCode, "frate", szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", uiQpConstant);
		SetXmlValue(&stDataEnCode, "quality", szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%d", uiKeyFrameRate);
		SetXmlValue(&stDataEnCode, "keyFrame", szTemp);

		SetXmlValue(&stDataEnCode, "result", "10000");
	}
	else

	{
		SetXmlValue(&stDataEnCode, "result", "0");
	}

	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}
#endif
// 处理业务和发送线程
void *P_TcpServerSendThread()
{
	TcpXmlData stXmlData;
	DWORD dwPopLen = 0;
	BYTE bytLyv = 0; 
	DWORD dwPushSym = 0;

	S_Data stDataDe;
	int iCommandId = 0;

	int i = 0;

	while (!g_bExitTcpServer)
	{
		usleep(30000);
		bytLyv = 0; 
		dwPushSym = 0;
		dwPopLen = 0;
		memset(&stXmlData, 0, sizeof(stXmlData));
		if(!g_DataList->popdata(g_DataList->_this, &bytLyv, &dwPushSym, sizeof(stXmlData), &dwPopLen, (char*)&stXmlData))
		{
			stDataDe.szCommandId = 0;
			DeCode(stXmlData.szXmlDataBuf, &stDataDe);
			//printf("stXmlData.szXmlDataBuf = %s",stXmlData.szXmlDataBuf);
			if(0 == stDataDe.szCommandId)
			{
				continue ;
			}
			iCommandId = stDataDe.szCommandId;
			if(strcmp(stDataDe.szType, CONNETTYPE))
			{
				FreeXmlValue(&stDataDe);
				continue;
			}
			//LOGOUT("tcpServer socketid:%d commandId:%d ",dwPushSym,iCommandId);
			switch(iCommandId)
			{
			#if 0
			case 5000:
				{
					GetNetParam(dwPushSym, &stDataDe);
				}
				break;

			case 5001:
				{
					SetNetParam(dwPushSym, &stDataDe);
				}
				break;

			case 5002:
				{
					ModifyDevMaskMessage(dwPushSym, &stDataDe);
				}
				break;

			case 5003:
				{
					CheckPassword(dwPushSym, &stDataDe);
				}
				break;

			case 5004:
				{
					ModifyPassword(dwPushSym, &stDataDe);
				}
				break;

			case 5005:
				{	
					SetDeviceOrProduceId(dwPushSym, &stDataDe);
				}
				break;
				
			case 5006:
				{	
					SetDeviceOrProduceId(dwPushSym, &stDataDe);
				}
				break;

			case 5010:
				{
					SZYRebootSystem(dwPushSym, &stDataDe);
				}
				break;

			case 5013:
				{
					SzySetIpInfo(dwPushSym, &stDataDe);
				}
				break;

			case 5014:
				{
					SzyGetIpInfo(dwPushSym, &stDataDe);
				}
				break;

			case 5015:
				{
					SzySetVideoParam(dwPushSym, &stDataDe);
				}
				break;

			case 5016:
				{
					SzyGetVideoParam(dwPushSym, &stDataDe);
				}
				break;
		
			case 5026:
				{
					SZYRebootSystem(dwPushSym, &stDataDe);
				}
				break;
			#endif
			default:
				break;
			}

			FreeXmlValue(&stDataDe);
			for (i = 0; i<CLIENTNUM;i++)
			{
				if(g_iConnectHost[i].m_iSocket == dwPushSym)
				{
					close(g_iConnectHost[i].m_iSocket);
					g_iConnectHost[i].m_iSocket = -1;
					g_iConnectHost[i].m_iSocketType =-1;
					g_iConnectNum--;
					TcpRecvBufRelease(i);
				}
			}
		}
	}

	LOGOUT("Exit TcpServerSend");
	return NULL;
}


// 初始化devtool TCP服务器
int InitTcpServer(const char v_szCfgFilePath)
{
	g_bExitTcpServer = FALSE;

	g_DataList = (InnerDataList*)ListConstruction();
	g_DataList->initcfg(g_DataList->_this,2*1024*1024,1000000000);
	

	char szPath[256]={0,};
	memset(g_szServerNO, 0, sizeof(g_szServerNO));
	sprintf(szPath, "%s/%s",v_szCfgFilePath, SERVERNOFILENAME);
	ReadConfigFile(szPath, g_szServerNO, sizeof(g_szServerNO));

	int iRet;
	pthread_t pThreadMng;
	pthread_t pThreadSend;
	pthread_t pThreadRcv;
	iRet = pthread_create(&pThreadMng, NULL, P_TcpServerSocketThread, NULL);
	if(iRet)
	{
		LOGOUT("Create P_TcpServerSocketThread Fail!!\n");
		return ERR_THREAD;
	}
	pthread_detach(pThreadMng);

	iRet = pthread_create(&pThreadRcv, NULL, P_TcpServerRecvThread, NULL);
	if(iRet)
	{
		LOGOUT("Create P_TcpServerRecvThread Fail!!\n");
		return ERR_THREAD;
	}
	pthread_detach(pThreadRcv);
	iRet = pthread_create(&pThreadSend, NULL, P_TcpServerSendThread, NULL);
	if(iRet)
	{
		LOGOUT("Create P_TcpServerSendThread Fail!!\n");
		return ERR_THREAD;
	}
	pthread_detach(pThreadSend);

	return 0;
}

// 释放devtool TCP服务器
void ReleaseTcpServer()
{
	int i;
	g_bExitTcpServer = TRUE;
	g_bUploadLogFile = FALSE;
	
	for(i = 0; i < CLIENTNUM; i++)
	{
		if(g_iConnectHost[i].m_iSocket != -1)
		{
			close(g_iConnectHost[i].m_iSocket);
			TcpRecvBufRelease(i);
		}
	}
	if(iSocket == -1)
	{
		close(iSocket);
	}
	
	g_DataList->release(g_DataList->_this);
	LOGOUT("ReleaseTcpServer");
}

// 释放内存
void TcpFreeMemory()
{
	if(g_DataList != NULL)
	{
		free(g_DataList);
		g_DataList = NULL;
	}
}
