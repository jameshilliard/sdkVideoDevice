#include "TcpServer.h"
#include "../hisdk/hi_sdk.h"


// 配置文件路径
char g_szFilePath[128] = {0};


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
						LOGOUT("get socket %d %d",i,g_iConnectHost[i].m_iSocket);
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
									g_DataList->pushData(g_DataList->_this, 1, iRecvLen, szRecvBuf, g_iConnectHost[i].m_iSocket, &dwPacketNum);
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
								g_DataList->pushData(g_DataList->_this, 1, g_stRecvPacketBuf[i].m_iTotalLen, g_stRecvPacketBuf[i].m_pRecvBuf, g_iConnectHost[i].m_iSocket, &dwPacketNum); 
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
	printf("encode:%s",stSendXmlData.szXmlDataBuf);
	stSendXmlData.m_iXmlLen += 2;// 数据长度（xml类型+xml数据内容长度+结束符长度）
	stSendXmlData.m_bytXmlType = 0;
	SzySdkSendData(v_iSocket, (char*)&stSendXmlData, stSendXmlData.m_iXmlLen + 4);
	//send(v_iSocket, (char*)&stSendXmlData, stSendXmlData.m_iXmlLen + 4, 0);
}

// 获取算法参数
void GetAlgorithmParam(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_stDeCodeData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	SetXmlIntValue(&stDataEnCode, "BefRecLastTime", g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iBefRecLastTime);
	SetXmlIntValue(&stDataEnCode, "BefRecTimes", g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iBefRecTimes);
	SetXmlIntValue(&stDataEnCode, "ConRecLastTime", g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iConRecLastTime);
	SetXmlIntValue(&stDataEnCode, "ConRecTimes", g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iConRecTimes);
	SetXmlIntValue(&stDataEnCode, "EndRecTime", g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iEndRecTime);
	SetXmlIntValue(&stDataEnCode, "Enable", g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_bEnable);
	
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

void SetAlgorithmParam(int v_iSocket, S_Data *v_pSData)
{
	if(-1 == v_iSocket || NULL == v_pSData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_pSData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_pSData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	int i=0;
	tagMotionCfg objMotionCfg=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg;
	memset(&objMotionCfg,0,sizeof(tagMotionCfg));
	for(i=0;i<v_pSData->iParamCount;i++)
	{
		if(strcmp(v_pSData->params[i].szKey,"BefRecLastTime")==0)
		{
			objMotionCfg.m_iBefRecLastTime=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"BefRecTimes")==0)
		{
			objMotionCfg.m_iBefRecTimes=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"ConRecLastTime")==0)
		{
			objMotionCfg.m_iConRecLastTime=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"ConRecTimes")==0)
		{
			objMotionCfg.m_iConRecTimes=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"EndRecTime")==0)
		{
			objMotionCfg.m_iEndRecTime=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"Enable")==0)
		{
			objMotionCfg.m_bEnable =atoi(v_pSData->params[i].szValue);
		}
	}
	if(0!=memcmp(&objMotionCfg,&g_stConfigCfg.m_unMotionCfg.m_objMotionCfg,sizeof(objMotionCfg)))
	{
		SetMotionCfg(DEVICECONFIGDIR,objMotionCfg);
		memcpy(&g_stConfigCfg.m_unMotionCfg.m_objMotionCfg,&objMotionCfg,sizeof(objMotionCfg));
	}
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

// 获取算法参数

void GetOSSConfigmParam(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_stDeCodeData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	SetXmlValue(&stDataEnCode, "BuctetName", g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szBuctetName);
	SetXmlValue(&stDataEnCode, "OssEndPoint", g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szOssEndPoint);
	SetXmlValue(&stDataEnCode, "AccessKeyId", g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeyId);
	SetXmlValue(&stDataEnCode, "AccessKeySecret", g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeySecret);
	SetXmlValue(&stDataEnCode, "VideoPath", g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szVideoPath);
	SetXmlIntValue(&stDataEnCode, "LogUploadEnable", g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_bLogUploadEnable);
	SetXmlValue(&stDataEnCode, "JpgPath", g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szJPGPath);
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}


void SetOSSConfigmParam(int v_iSocket, S_Data *v_pSData)
{
	if(-1 == v_iSocket || NULL == v_pSData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_pSData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_pSData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	int i=0;
	tagAliyunOssCfg objAliyunOssCfg=g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg;
	memset(&objAliyunOssCfg,0,sizeof(tagAliyunOssCfg));
	for(i=0;i<v_pSData->iParamCount;i++)
	{
		if(strcmp(v_pSData->params[i].szKey,"BuctetName")==0)
		{
			strncpy(objAliyunOssCfg.m_szBuctetName,v_pSData->params[i].szValue,sizeof(objAliyunOssCfg.m_szBuctetName));
		}
		if(strcmp(v_pSData->params[i].szKey,"OssEndPoint")==0)
		{
			strncpy(objAliyunOssCfg.m_szOssEndPoint,v_pSData->params[i].szValue,sizeof(objAliyunOssCfg.m_szOssEndPoint));
		}
		if(strcmp(v_pSData->params[i].szKey,"AccessKeyId")==0)
		{
			strncpy(objAliyunOssCfg.m_szAccessKeyId,v_pSData->params[i].szValue,sizeof(objAliyunOssCfg.m_szAccessKeyId));
		}
		if(strcmp(v_pSData->params[i].szKey,"AccessKeySecret")==0)
		{
			strncpy(objAliyunOssCfg.m_szAccessKeySecret,v_pSData->params[i].szValue,sizeof(objAliyunOssCfg.m_szAccessKeySecret));
		}
		if(strcmp(v_pSData->params[i].szKey,"LogUploadEnable")==0)
		{
			objAliyunOssCfg.m_bLogUploadEnable =atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"VideoPath")==0)
		{
			strncpy(objAliyunOssCfg.m_szVideoPath,v_pSData->params[i].szValue,sizeof(objAliyunOssCfg.m_szVideoPath));
		}
		if(strcmp(v_pSData->params[i].szKey,"JpgPath")==0)
		{
			strncpy(objAliyunOssCfg.m_szJPGPath,v_pSData->params[i].szValue,sizeof(objAliyunOssCfg.m_szJPGPath));
		}
	}
	if(0!=memcmp(&objAliyunOssCfg,&g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg,sizeof(objAliyunOssCfg)))
	{
		SetAliyunOssCfg(DEVICECONFIGDIR,objAliyunOssCfg);
		memcpy(&g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg,&objAliyunOssCfg,sizeof(objAliyunOssCfg));
	}
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

void GetVideoParam(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_stDeCodeData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	HI_S_Video_Ext sVideo;
	char buffer[80]={0};
	int iRet=GetMasterVideoStream(&sVideo);
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%dX%d",sVideo.u32Width,sVideo.u32Height);
	SetXmlValue(&stDataEnCode, "Resolution",buffer);
	SetXmlIntValue(&stDataEnCode, "Frame",sVideo.u32Frame);
	SetXmlIntValue(&stDataEnCode, "Quality",sVideo.u32ImgQuality);
	SetXmlIntValue(&stDataEnCode, "CodeType",sVideo.blCbr);
	SetXmlIntValue(&stDataEnCode, "Code",sVideo.u32Bitrate);
	SetXmlIntValue(&stDataEnCode, "KeyFrame",sVideo.u32Iframe);
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

void SetVideoParam(int v_iSocket, S_Data *v_pSData)
{
	if(-1 == v_iSocket || NULL == v_pSData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_pSData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_pSData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	HI_S_Video_Ext sVideo;
	char buffer[80]={0};
	char wBuffer[80]={0};
	char hBuffer[80]={0};
	int i=0;
	tagCapParamCfg objCapParamCfg=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0];
	memset(&objCapParamCfg,0,sizeof(tagCapParamCfg));
	
	for(i=0;i<v_pSData->iParamCount;i++)
	{
		if(strcmp(v_pSData->params[i].szKey,"Resolution")==0)
		{
		
			sscanf(v_pSData->params[i].szValue,"%[0-9]X%[0-9]",wBuffer,hBuffer);
			objCapParamCfg.m_wWidth=atoi(wBuffer);
			objCapParamCfg.m_wHeight=atoi(hBuffer);
			printf("width=%d,height=%d---\n",objCapParamCfg.m_wWidth,objCapParamCfg.m_wHeight);
		}
		if(strcmp(v_pSData->params[i].szKey,"Frame")==0)
		{
			objCapParamCfg.m_wFrameRate=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"Quality")==0)
		{
			objCapParamCfg.m_wQpConstant=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"CodeType")==0)
		{
			objCapParamCfg.m_CodeType=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"KeyFrame")==0)
		{
			objCapParamCfg.m_wKeyFrameRate=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"Code")==0)
		{
			objCapParamCfg.m_wBitRate=atoi(v_pSData->params[i].szValue);
		}
	}
	if(0!=memcmp(&objCapParamCfg,&g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg,sizeof(objCapParamCfg)))
	{
		SetCapParamCfg(DEVICECONFIGDIR,objCapParamCfg);
		memcpy(&g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg,&objCapParamCfg,sizeof(objCapParamCfg));
	}
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}


void GetUrgencyMotionParam(int v_iSocket, S_Data *v_stDeCodeData)
{
	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_stDeCodeData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	
	SetXmlIntValue(&stDataEnCode, "StartPeriod", g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iStartPeriod);
	SetXmlIntValue(&stDataEnCode, "StartSumDetect",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iStartSumDetect);
	SetXmlIntValue(&stDataEnCode, "StartSumArea",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iStartSumArea);
	SetXmlIntValue(&stDataEnCode, "StartSoundSize",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iStartSoundSize);
	SetXmlIntValue(&stDataEnCode, "OverPeriod", g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iOverPeriod);
	SetXmlIntValue(&stDataEnCode, "OverSumDetect",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iOverSumDetect);
	SetXmlIntValue(&stDataEnCode, "OverSumArea",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iOverSumArea);
	SetXmlIntValue(&stDataEnCode, "OverSoundSize",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iOverSoundSize);
	SetXmlIntValue(&stDataEnCode, "EndRecTime",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iEndRecTime);
	SetXmlIntValue(&stDataEnCode, "CommServerTime",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_iCommServerTime);
	SetXmlIntValue(&stDataEnCode, "Enable",g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg.m_bEnable);
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}

void SetUrgencyMotionParam(int v_iSocket, S_Data *v_pSData)
{

	if(-1 == v_iSocket || NULL == v_pSData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_pSData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_pSData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	HI_S_Video_Ext sVideo;
	char buffer[80]={0};
	char wBuffer[80]={0};
	char hBuffer[80]={0};
	int i=0;
	tagUrgencyMotionCfg objUrgencyMotionCfg=g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg;
	memset(&objUrgencyMotionCfg,0,sizeof(tagUrgencyMotionCfg));
	
	for(i=0;i<v_pSData->iParamCount;i++)
	{
		if(strcmp(v_pSData->params[i].szKey,"StartPeriod")==0)
		{
			objUrgencyMotionCfg.m_iStartPeriod=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"StartSumDetect")==0)
		{
			objUrgencyMotionCfg.m_iStartSumDetect=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"StartSumArea")==0)
		{
			objUrgencyMotionCfg.m_iStartSumArea=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"StartSoundSize")==0)
		{
			objUrgencyMotionCfg.m_iStartSoundSize=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"OverPeriod")==0)
		{
			objUrgencyMotionCfg.m_iOverPeriod=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"OverSumDetect")==0)
		{
			objUrgencyMotionCfg.m_iOverSumDetect=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"OverSumArea")==0)
		{
			objUrgencyMotionCfg.m_iOverSumArea=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"OverSoundSize")==0)
		{
			objUrgencyMotionCfg.m_iOverSoundSize=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"EndRecTime")==0)
		{
			objUrgencyMotionCfg.m_iEndRecTime=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"CommServerTime")==0)
		{
			objUrgencyMotionCfg.m_iCommServerTime=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"Enable")==0)
		{
			objUrgencyMotionCfg.m_bEnable=atoi(v_pSData->params[i].szValue);
		}
	}
	if(0!=memcmp(&objUrgencyMotionCfg,&g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg,sizeof(objUrgencyMotionCfg)))
	{
		SetUrgencyMotionCfg(DEVICECONFIGDIR,objUrgencyMotionCfg);
		memcpy(&g_stConfigCfg.m_unUrgencyMotionCfg.m_objUrgencyMotionCfg,&objUrgencyMotionCfg,sizeof(objUrgencyMotionCfg));
	}
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
	
}

void GetSoundEableParam(int v_iSocket, S_Data *v_stDeCodeData)
{

	if(-1 == v_iSocket || NULL == v_stDeCodeData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_stDeCodeData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_stDeCodeData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);

	SetXmlIntValue(&stDataEnCode, "StartUpEnable", g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bStartUpEnable);
	SetXmlIntValue(&stDataEnCode, "ServerSuccessEnable", g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bServerSuccessEnable);
	SetXmlIntValue(&stDataEnCode, "ServerFailureEnable", g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bServerFailureEnable);
	SetXmlIntValue(&stDataEnCode, "UpgradeEnable", g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bUpgradeEnable);
	SetXmlIntValue(&stDataEnCode, "LoginInEnable", g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bLoginInEnable);
	SetXmlIntValue(&stDataEnCode, "UrgencyStartEnable", g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bUrgencyStartEnable);
	SetXmlIntValue(&stDataEnCode, "UrgencyOverEnable", g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bUrgencyOverEnable);
	SetXmlIntValue(&stDataEnCode, "Enable", g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg.m_bEnable);

	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
}
void SetSoundEableParam(int v_iSocket, S_Data *v_pSData)
{

	if(-1 == v_iSocket || NULL == v_pSData)
	{
		return ;
	}
	S_Data stDataEnCode;
	memset(&stDataEnCode, 0, sizeof(stDataEnCode));
	stDataEnCode.szCommandId = v_pSData->szCommandId;
	strncpy(stDataEnCode.szCommandName,v_pSData->szCommandName,sizeof(stDataEnCode.szCommandName));
	strcpy(stDataEnCode.szType , CONNETTYPE);
	HI_S_Video_Ext sVideo;
	char buffer[80]={0};
	char wBuffer[80]={0};
	char hBuffer[80]={0};
	int i=0;
	tagSoundEableCfg objSoundEableCfg=g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg;
	memset(&objSoundEableCfg,0,sizeof(tagSoundEableCfg));
	
	for(i=0;i<v_pSData->iParamCount;i++)
	{
		if(strcmp(v_pSData->params[i].szKey,"StartUpEnable")==0)
		{
			objSoundEableCfg.m_bStartUpEnable=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"ServerSuccessEnable")==0)
		{
			objSoundEableCfg.m_bServerSuccessEnable=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"ServerFailureEnable")==0)
		{
			objSoundEableCfg.m_bServerFailureEnable=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"UpgradeEnable")==0)
		{
			objSoundEableCfg.m_bUpgradeEnable=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"LoginInEnable")==0)
		{
			objSoundEableCfg.m_bLoginInEnable=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"UrgencyStartEnable")==0)
		{
			objSoundEableCfg.m_bUrgencyStartEnable=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"UrgencyOverEnable")==0)
		{
			objSoundEableCfg.m_bUrgencyOverEnable=atoi(v_pSData->params[i].szValue);
		}
		if(strcmp(v_pSData->params[i].szKey,"Enable")==0)
		{
			objSoundEableCfg.m_bEnable=atoi(v_pSData->params[i].szValue);
		}
	}
	objSoundEableCfg.m_bInvalid=DE_ENABLE;
	if(0!=memcmp(&objSoundEableCfg,&g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg,sizeof(objSoundEableCfg)))
	{
		SetSoundEableCfg(DEVICECONFIGDIR,objSoundEableCfg);
		memcpy(&g_stConfigCfg.m_unSoundEableCfg.m_objSoundEableCfg,&objSoundEableCfg,sizeof(objSoundEableCfg));
	}
	SetXmlValue(&stDataEnCode, "result","10000");
	TcpSendCmdData(v_iSocket, &stDataEnCode);
	FreeXmlValue(&stDataEnCode);
	
}

// 重启系统
void RebootSystem(int v_iSocket, S_Data *v_stDeCodeData)
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
	system("reboot");
}


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
		if(!g_DataList->popData(g_DataList->_this, &bytLyv, &dwPushSym, sizeof(stXmlData), &dwPopLen, (char*)&stXmlData))
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
			LOGOUT("tcpServer socketid:%d commandId:%d ",dwPushSym,iCommandId);
			switch(iCommandId)
			{
			case 4000:
				{	
					GetAlgorithmParam(dwPushSym, &stDataDe);
				}
				break;
				
			case 4001:
				{	
					SetAlgorithmParam(dwPushSym, &stDataDe);
				}
				break;
			case 4002:
				{	
					GetOSSConfigmParam(dwPushSym, &stDataDe);
				}
				break;
			case 4003:
				{	
					SetOSSConfigmParam(dwPushSym, &stDataDe);
				}
				break; 
			case 4004:
				{	
					GetVideoParam(dwPushSym, &stDataDe);
				}
				break;
			case 4005:
				{	
					SetVideoParam(dwPushSym, &stDataDe);
				}
				break;
			case 4006:
				{	
					GetUrgencyMotionParam(dwPushSym, &stDataDe);
				}
				break;
			case 4007:
				{	
					SetUrgencyMotionParam(dwPushSym, &stDataDe);
				}
				break;
			case 4008:
				{	
					GetSoundEableParam(dwPushSym, &stDataDe);
				}
				break;
			case 4009:
				{	
					SetSoundEableParam(dwPushSym, &stDataDe);
				}
				break;
			case 4040:
				{	
					RebootSystem(dwPushSym, &stDataDe);
				}
				break; 
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
int InitTcpServer()
{
	g_bExitTcpServer = FALSE;

	g_DataList = (InnerDataList*)ListConstruction();
	g_DataList->initCfg(g_DataList->_this,2*1024*1024,1000000000);
	
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
