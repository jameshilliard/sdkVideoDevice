#include "stdafx.h"
#include "SyTcpClient.h"

CSyTcpClient::CSyTcpClient(void)
{

}

CSyTcpClient::~CSyTcpClient(void)
{
	closesocket(m_socketClient);
}

void CSyTcpClient::InitTcpSocket(CString strServer,int port)
{
	m_strSerIp = strServer;
	m_iPort = port;
}

// 编码成xml数据
void CSyTcpClient::XmlEncode(CString &v_strXmlData, map<CString,CString>& mapofParam, int v_iFunName)
{
	std::string strXmlData;
	S_Data stData;
	stData.Clear();

	stData.commandId = v_iFunName;
	char szComName[20];
	sprintf(szComName, "%d",v_iFunName);
	stData.commandName = szComName;
	stData.type =  CONNETTYPE;
	char szFirstBuf[128] = {0};
	char szSecondBuf[2048] = {0};
	map<CString,CString>::const_iterator it;
	for (it = mapofParam.begin();it != mapofParam.end();it++)
	{
		memset(szFirstBuf, 0, sizeof(szFirstBuf));
		memset(szSecondBuf, 0, sizeof(szSecondBuf));
		memcpy(szFirstBuf, it->first, it->first.GetLength());
		memcpy(szSecondBuf, it->second, it->second.GetLength());
		stData.params[szFirstBuf] = szSecondBuf;
	}

	xmlparser::Encode(&stData, strXmlData);
	v_strXmlData.Format("%s", strXmlData.c_str());
}

// 解码xml数据
bool CSyTcpClient::XmlDecode(map<CString,CString>& mapofParam, char *v_szXmlData)
{
	S_Data stData;
	bool bRet = xmlparser::Decode(v_szXmlData, &stData);
	if(!bRet)
	{
		return bRet;
	}
	map<std::string, std::string>::const_iterator it;
	for (it = stData.params.begin();it != stData.params.end();it++)
	{
		mapofParam[ it->first.c_str()] =  it->second.c_str();
	}
	return true;
}

// 发送xml数据，格式:数据长度（xml类型+xml数据内容长度+结束符长度）+xml类型+xml数据内容+结束符
bool CSyTcpClient::TcpPost(map<CString,CString>& mapofParam, map<CString,CString>& mapofResult, int v_iFunName)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int iErr;
	int iConnectCount = 0;

	do 
	{
		m_socketClient = socket(AF_INET,SOCK_STREAM,0);

		SOCKADDR_IN addrSvr;
		addrSvr.sin_addr.S_un.S_addr = inet_addr(m_strSerIp);
		addrSvr.sin_family = AF_INET;
		addrSvr.sin_port = htons((short)m_iPort);
		iErr = connect(m_socketClient,(SOCKADDR*)&addrSvr,sizeof(SOCKADDR));
		if(iErr == -1)
		{
			//AfxMessageBox("连接服务器失败");
			iConnectCount++;
			closesocket(m_socketClient);
			continue;
		}

		break;

	} while (iConnectCount < 3);

	if(iConnectCount == 3)
	{
		closesocket(m_socketClient);
		AfxMessageBox("Connect Server Fail");
		return false;
	}

	linger m_sLinger; 
	m_sLinger.l_onoff = 1; //在调用closesocket()时还有数据未发送完，允许等待， // 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭		   
	m_sLinger.l_linger = 5; //设置等待时间为5秒 
	setsockopt(m_socketClient, SOL_SOCKET, SO_LINGER, ( const char* )&m_sLinger, sizeof(linger )); 

	char szRecvbuf[2048] = {0};
	int iRecvLen = 0;
	char szSendBuf[2048] = {0};
	int iSendLen = 0;
	char szXmlBuf[2048] = {0};
	int iRecvXmlLen = 0;

	int iRet;
	bool bRet  = true;
	CString strXmlData;
	XmlEncode(strXmlData, mapofParam, v_iFunName);
	int iXmlLen = strXmlData.GetLength() + 1 + 1;// 包体长度+序列号类型+结束符
	memcpy(szSendBuf + iSendLen, &iXmlLen, 4);
	iSendLen += 4;

	*(szSendBuf + iSendLen) = 0x00;// 序列号类型
	iSendLen ++;

	memcpy(szSendBuf + iSendLen, strXmlData, strXmlData.GetLength());
	iSendLen += strXmlData.GetLength();

	*(szSendBuf + iSendLen) = 0x00; // 结束符
	iSendLen ++;

	int iSendCount = 0;
	do
	{
		iRet = send(m_socketClient, szSendBuf, iSendLen, 0);
		if(SOCKET_ERROR == iRet)
		{
			AfxMessageBox("Send Fail");
			bRet = false;
			goto _TCP_END;
		}

		fd_set rset, eset;
		struct timeval tval;

		FD_ZERO(&rset);
		FD_ZERO(&eset);

		FD_SET(m_socketClient,&rset);
		FD_SET(m_socketClient,&eset);
		tval.tv_sec = 10;
		tval.tv_usec = 0;

		iRet = select(m_socketClient, &rset, NULL, &eset, &tval);
		if(iRet > 0)
		{
			if( FD_ISSET(m_socketClient, &eset) )
			{
				AfxMessageBox("Read Data Fail");
				bRet = false;
				goto _TCP_END;
			}
			else if( FD_ISSET(m_socketClient, &rset) )
			{
				iRecvLen = recv( m_socketClient, szRecvbuf, sizeof(szRecvbuf), 0 );
				if(iRecvLen > 0)
				{
					memcpy(&iRecvXmlLen, szRecvbuf, 4);
					iRecvXmlLen -= 2;// 减去xml类型和结束符长度，剩下的就是xml数据长度
					memcpy(szXmlBuf, szRecvbuf + 5, min(iRecvXmlLen,sizeof(szXmlBuf)));
					//AfxMessageBox(szXmlBuf);
					bRet = XmlDecode(mapofResult, szXmlBuf);
					if(!iRet)
					{
						AfxMessageBox("xml data is Err");
						bRet = false;
						goto _TCP_END;
					}
				}	
				else
				{
					AfxMessageBox("Recv Data Err");
					bRet = false;
					goto _TCP_END;
				}
			}
		}
		else if(iRet < 0)
		{
			AfxMessageBox("select Fail！");
			bRet = false;
			goto _TCP_END;
		}
		else// 接收超时
		{
			iSendCount ++;
			continue;
		}

		break;

	}while (iSendCount < 3);
	
	if(iSendCount == 3)
	{
		AfxMessageBox("Recv Data Timeout！");
		bRet = false;
	}

_TCP_END:
	closesocket(m_socketClient);
	return bRet;
}

// 上传文件信息
int CSyTcpClient::TcpUploadFileInfo(CString v_strUploadFileName, CString v_strUpType)
{
	SOCKET sSockClient;
	bool bRet = true;
	int iRet = TCP_SUCC;
	sSockClient = ConnectToServer();
	if(SOCKET_ERROR == sSockClient)
	{
		return TCP_CONNECTSERFAIL;
	}

	map<CString,CString> mapofParam;
	map<CString,CString> mapofResult;
	CFile file;
	DWORD dwFileLength = 0;
	DWORD dwTotalPacket = 0;
	if(!file.Open(v_strUploadFileName, CFile::shareDenyNone | CFile::modeRead))
	{
		iRet = TCP_FILEERR;
		goto _UPLOAD_END;
	}
	dwFileLength = (DWORD)file.GetLength();

	if(dwFileLength <= 0)
	{
		iRet = TCP_FILEERR;
		goto _UPLOAD_END;
	}

	dwTotalPacket = dwFileLength/GETFILELEN;
	dwTotalPacket += (dwTotalPacket%GETFILELEN == 0)?0:1;

	mapofParam["updatetype"] = v_strUpType;
	mapofParam["filesize"].Format("%d",dwFileLength);
	mapofParam["totalpacket"].Format("%d",dwTotalPacket);

	bRet = SendDataToServer(sSockClient, mapofParam, 5012);
	if(!bRet)
	{
		iRet = TCP_SENDFAIL;
		goto _UPLOAD_END;
	}

	bRet = RecvXmlDataFromServer(sSockClient, mapofResult);
	if(!bRet)
	{
		iRet = TCP_RECVFAIL;
		goto _UPLOAD_END;
	}

	if(mapofResult["result"] != "10000")
	{
		iRet = TCP_UPLOADFILEINFOFAIL;// 上传文件信息失败
		goto _UPLOAD_END;
	}

_UPLOAD_END:
	CloseSocket(sSockClient);
	file.Close();
	return iRet;
}

// 上传文件
int CSyTcpClient::TcpUploadFile(CString v_strUploadFileName )
{
	SOCKET sSockClient;
	bool bRet = false;
	int iResult = TCP_SUCC;
	sSockClient = ConnectToServer();
	if(SOCKET_ERROR == sSockClient)
	{
		return TCP_CONNECTSERFAIL;
	}
	
	CFile file;
	DWORD dwCurPacket = 1;
	char szFileBuff[GETFILELEN] = {0};
	char szSendPacketBuf[GETFILELEN+100] = {0};
	char szRecvbuf[1024] = {0};
	int iRecvLen = 0;
	int iPackSendLen = 0;
	int iReadLen = 0;
	if(!file.Open(v_strUploadFileName, CFile::shareDenyNone | CFile::modeRead))
	{
		iResult = TCP_OPENFILEFAIL;
		goto _UPLOAD_END;
	}
	
	while(1)
	{
		iPackSendLen = 0;
		memset(szSendPacketBuf, 0, sizeof(szSendPacketBuf));
		memset(szFileBuff, 0, sizeof(szFileBuff));

		iReadLen = file.Read(szFileBuff, GETFILELEN);
		if(iReadLen == 0)
		{
			break;
		}
		int iLen = (iReadLen + 8 + 2); // 发送的长度：数数据的长度+类型+接收符
		memcpy(szSendPacketBuf, &iLen, sizeof(iLen));
		iPackSendLen += sizeof(iLen);

		szSendPacketBuf[iPackSendLen++] = 0x03;

		memcpy(szSendPacketBuf + iPackSendLen, &dwCurPacket, sizeof(dwCurPacket));
		iPackSendLen += sizeof(dwCurPacket);
		dwCurPacket++;

		memcpy(szSendPacketBuf + iPackSendLen, &iReadLen, sizeof(iReadLen));
		iPackSendLen += sizeof(iReadLen);

		memcpy(szSendPacketBuf + iPackSendLen, szFileBuff, iReadLen);
		iPackSendLen += iReadLen;

		szSendPacketBuf[iPackSendLen++] = 0x00;

		int iRet = -1;
		iRet = send(sSockClient, szSendPacketBuf, iPackSendLen, 0);
		if(SOCKET_ERROR == iRet)
		{
			iResult = TCP_SENDFAIL;
			goto _UPLOAD_END;
		}

		bRet = RecvDataFrmServer(sSockClient, szRecvbuf, sizeof(szRecvbuf), iRecvLen);
		if(!bRet)
		{
			iResult = TCP_RECVFAIL;
			goto _UPLOAD_END;
		}
		
		int iDataLen = 0;
		BYTE bytType = 0;
		BYTE bytResult = 0;
		memcpy(&iDataLen, szRecvbuf, sizeof(iDataLen));
		if(iDataLen != (iRecvLen - 4))
		{
			iResult = TCP_UPDATAFAIL;
			goto _UPLOAD_END;
		}
		bytType = szRecvbuf[4];
		memcpy(&bytResult, szRecvbuf + 5, iDataLen - 2);

		if(bytType != 0x03) // 类型3传的是文件数据
		{
			iResult = TCP_UPDATAFAIL;
			goto _UPLOAD_END;
		}
	
		if(bytResult != 0x01) // 0x00:失败，0x01成功
		{
			iResult = TCP_UPDATAFAIL;
			goto _UPLOAD_END;
		}

		if(iReadLen < GETFILELEN)
		{
			break;
		}
	}

_UPLOAD_END:
	CloseSocket(sSockClient);
	file.Close();
	return iResult;
}

// 连接服务器
SOCKET CSyTcpClient::ConnectToServer()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int iErr;
	int iConnectCount = 0;
	SOCKET sSocketClient = SOCKET_ERROR;

	do 
	{
		sSocketClient = socket(AF_INET,SOCK_STREAM,0);

		int nSendBuf = 64*1024;//设置为64k
		setsockopt(sSocketClient,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

		SOCKADDR_IN addrSvr;
		addrSvr.sin_addr.S_un.S_addr = inet_addr(m_strSerIp);
		addrSvr.sin_family = AF_INET;
		addrSvr.sin_port = htons((short)m_iPort);
		//AfxMessageBox(m_strSerIp);
		iErr = connect(sSocketClient,(SOCKADDR*)&addrSvr,sizeof(SOCKADDR));
		if(iErr == -1)
		{
			//AfxMessageBox("连接服务器失败");
			iConnectCount++;
			closesocket(sSocketClient);
			continue;
		}

		break;

	} while (iConnectCount < 3);

	if(iConnectCount == 3)
	{
		closesocket(sSocketClient);
		return SOCKET_ERROR;
	}

	return sSocketClient;
}

// 发送数据
bool CSyTcpClient::SendDataToServer(SOCKET v_sSocketClient, map<CString,CString>& mapofParam, int v_iFunName)
{
	linger m_sLinger; 
	m_sLinger.l_onoff = 1; //在调用closesocket()时还有数据未发送完，允许等待， // 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭		   
	m_sLinger.l_linger = 5; //设置等待时间为5秒 
	setsockopt(v_sSocketClient, SOL_SOCKET, SO_LINGER, ( const char* )&m_sLinger, sizeof(linger )); 

	char szSendBuf[2048] = {0};
	int iSendLen = 0;
	int iRet = 0;

	CString strXmlData;
	
	XmlEncode(strXmlData, mapofParam, v_iFunName);
	//AfxMessageBox(strXmlData);
	int iXmlLen = strXmlData.GetLength() + 1 + 1;// 包体长度+序列号类型+结束符
	memcpy(szSendBuf + iSendLen, &iXmlLen, 4);
	iSendLen += 4;

	*(szSendBuf + iSendLen) = 0x00;// 序列号类型
	iSendLen ++;

	memcpy(szSendBuf + iSendLen, strXmlData, strXmlData.GetLength());
	iSendLen += strXmlData.GetLength();

	*(szSendBuf + iSendLen) = 0x00; // 结束符
	iSendLen ++;

	iRet = send(v_sSocketClient, szSendBuf, iSendLen, 0);
	if(SOCKET_ERROR == iRet)
	{
		return false;
	}
	return true;
}

// 接收数据主要用来传输文件数据
bool CSyTcpClient::RecvDataFrmServer(SOCKET v_sSocketClient, char *v_szRecvData, int v_iBufLen, int &v_iRecvLen)
{
	int iRecvLen = 0;
	int iRet = 0;
	bool bRet = true;

	fd_set rset, eset;
	struct timeval tval;

	FD_ZERO(&rset);
	FD_ZERO(&eset);

	FD_SET(v_sSocketClient,&rset);
	FD_SET(v_sSocketClient,&eset);
	tval.tv_sec = 60;
	tval.tv_usec = 0;

	iRet = select(v_sSocketClient, &rset, NULL, &eset, &tval);
	if(iRet > 0)
	{
		if( FD_ISSET(v_sSocketClient, &eset) )
		{
			bRet = false;
			goto _TCP_END;
		}
		else if( FD_ISSET(v_sSocketClient, &rset) )
		{
			iRecvLen = recv( v_sSocketClient, v_szRecvData, v_iBufLen, 0 );
			if(iRecvLen > 0)
			{
				v_iRecvLen = iRecvLen;
			}
			else
			{
				bRet = false;
				goto _TCP_END;
			}
		}
	}
	else if(iRet < 0)
	{
		bRet = false;
		goto _TCP_END;
	}
_TCP_END:
	return bRet;
}


// 接收数据并且解析xml数据
bool CSyTcpClient::RecvXmlDataFromServer(SOCKET v_sSocketClient, map<CString,CString>& mapofResult)
{
	char szRecvbuf[2048] = {0};
	int iRecvLen = 0;
	char szXmlBuf[2048] = {0};
	int iRecvXmlLen = 0;
	int iRet = 0;
	bool bRet = false;

	fd_set rset, eset;
	struct timeval tval;

	FD_ZERO(&rset);
	FD_ZERO(&eset);

	FD_SET(v_sSocketClient,&rset);
	FD_SET(v_sSocketClient,&eset);
	tval.tv_sec = 10;
	tval.tv_usec = 0;

	iRet = select(v_sSocketClient, &rset, NULL, &eset, &tval);
	if(iRet > 0)
	{
		if( FD_ISSET(v_sSocketClient, &eset) )
		{
			bRet = false;
			goto _TCP_END;
		}
		else if( FD_ISSET(v_sSocketClient, &rset) )
		{
			iRecvLen = recv( v_sSocketClient, szRecvbuf, sizeof(szRecvbuf), 0 );
			if(iRecvLen > 0)
			{
				memcpy(&iRecvXmlLen, szRecvbuf, 4);
				iRecvXmlLen -= 2;// 减去xml类型和结束符长度，剩下的就是xml数据长度
				memcpy(szXmlBuf, szRecvbuf + 5, min(iRecvXmlLen,sizeof(szXmlBuf)));
				//AfxMessageBox(szXmlBuf);
				bRet = XmlDecode(mapofResult, szXmlBuf);
				if(!iRet)
				{
					bRet = false;
					goto _TCP_END;
				}
			}	
			else
			{
				bRet = false;
				goto _TCP_END;
			}
		}
	}
	else if(iRet < 0)
	{
		bRet = false;
		goto _TCP_END;
	}
_TCP_END:
	return bRet;

}
// 关闭数据
bool CSyTcpClient::CloseSocket(SOCKET v_sSocketClient)
{
	closesocket(v_sSocketClient);
	return TRUE;
}

// 获取错误信息
CString CSyTcpClient::GetErrMessage(int v_iType)
{

	CString strMessage = "";
	switch(v_iType)
	{
	case TCP_CONNECTSERFAIL:
		strMessage = "连接服务器失败，上传失败";
		break;

	case TCP_OPENFILEFAIL:
		strMessage = "打开文件失败，上传失败";
		break;

	case TCP_FILEERR:
		strMessage = "上传文件有误，上传失败";
		break;

	case TCP_SENDFAIL:
		strMessage = "发送数据失败，上传失败";
		break;

	case TCP_RECVFAIL:
		strMessage = "接收数据失败，上传失败";
		break;

	case TCP_UPLOADFILEINFOFAIL:
		strMessage = "上传文件信息失败，上传失败";
		break;

	case TCP_UPDATAFAIL:
		strMessage = "上传失败";
		break;
	default:
		strMessage = "上传失败";
		break;
	}
	return strMessage;
}


// 下载日志
// 数据格式：数据长度（数据内容+类型+结束符）+ 数据类型 + 数据内容 + 结束符
bool CSyTcpClient::DownloadFile(CString strSaveFileName,int v_iType)
{
	SOCKET sSockClient;
	bool bRet = false;
	sSockClient = ConnectToServer();
	if(SOCKET_ERROR == sSockClient)
	{
		AfxMessageBox("Connect Server Fail，Export Fail");
		return false;
	}

	int iRet = -1;
	bool bResult = true;
	char szSendBegainCmd[50] = {0};
	stDataBuf objDataBuf;
	char szRecvbuf[MAXDATALEN] = {0};
	int iRecvLen = 0;
	CString filepath;
	CFile myfile;
	int iOpenFileRet = 0;
	char szdir[512];
	
	int iSendLen = 0;
	BYTE bytType = v_iType;	// 数据类型
	BYTE bytCmd = 0x01;		// 传输日志,0x01,开始导出，0x02,正在导出
	
	int iLen =  sizeof(bytType) + sizeof(bytCmd) + 1;//数据头长度：类型+数据内容+结束符

	memcpy(szSendBegainCmd, &iLen, sizeof(iLen));
	iSendLen += sizeof(iLen);
	szSendBegainCmd[iSendLen++] = bytType;
	szSendBegainCmd[iSendLen++] = bytCmd;
	szSendBegainCmd[iSendLen++] = 0x00;

	iRet = send(sSockClient, szSendBegainCmd, iSendLen, 0);
	if(SOCKET_ERROR == iRet)
	{
		AfxMessageBox("Send Data Fail，Export Fail");
		bResult = false;
		goto _DOWNLOAD_END;
	}

	int iRecvCont = 0;
	memset(&objDataBuf, 0, sizeof(objDataBuf));
	do 
	{
		memset(&objDataBuf, 0, sizeof(objDataBuf));
		bRet = RecvDataFrmServer(sSockClient, (char *)&objDataBuf, sizeof(objDataBuf), iRecvLen);
		if(bRet)
		{
			break;
		}
		iRecvCont ++;
	} while (iRecvCont > 6);// 接收6次

	if(iRecvCont > 6)
	{
		AfxMessageBox("Recv Data Timeout，Export Fail");
		bResult = false;
		goto _DOWNLOAD_END;
	}

	if((objDataBuf.m_iDataLen != (iRecvLen - 4)) || (objDataBuf.m_bytDataType != v_iType))
	{
		AfxMessageBox("Recv Data Err，Export Fail");
		bResult = false;
		goto _DOWNLOAD_END;
	}

	// 数据内容：命令类型（1）（0x01传输日志应答）+ 应答结果 + 文件大小（4） + 总包数（4）
	// 应答结果：0x01：成功， 0x02:正在导出中, 0x03:数据有误
	stLogInfoCmd objLogInfoCmd;
	memcpy(&objLogInfoCmd, objDataBuf.szRecvBuf, objDataBuf.m_iDataLen - 2);
	if(objLogInfoCmd.m_bytType != 0x01)
	{
		AfxMessageBox("Recv Cmd Type Err，Export Fail");
		bResult = false;
		goto _DOWNLOAD_END;
	}

	if(objLogInfoCmd.m_bytAck == 0x02)
	{
		AfxMessageBox("Other User is Exporting, Please waiting...");
		bResult = false;
		goto _DOWNLOAD_END;
	}
	
	if(objLogInfoCmd.m_bytAck == 0x03)
	{
		AfxMessageBox("Begain Export Fail");
		bResult = false;
		goto _DOWNLOAD_END;
	}

	if(objLogInfoCmd.m_iFileSize > 20*1024*1024)// 文件大小大于10M
	{
		AfxMessageBox("The File Export is Err");
		bResult = false;
		goto _DOWNLOAD_END;
	}

	//GetCurrentDirectory(sizeof(szdir),szdir);
	//filepath.Format(_T("%s//%s"), szdir,strSaveFileName);
	iOpenFileRet = myfile.Open( strSaveFileName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	if(iOpenFileRet == 0)
	{
		AfxMessageBox("创建文件失败!");
	}

	stLogContent objLogContent;
	int iGetRecvLen = 0;
	iRecvLen = 0;
	// 数据内容：命令类型（1）（0x02日志内容）+ 应答类型 + 当前包数（4） + 当前包大小（4）+ 文件内容
	while(1)
	{
		memset(&objDataBuf, 0, sizeof(objDataBuf));
		do 
		{
			bRet = RecvDataFrmServer(sSockClient, szRecvbuf + iGetRecvLen, sizeof(szRecvbuf) - iGetRecvLen, iRecvLen);
			iRecvLen += iGetRecvLen;
			if(bRet)
			{
				break;
			}
			iRecvCont ++;
		} while (iRecvCont > 6);// 接收6次

		if(iRecvCont > 6)
		{
			AfxMessageBox("Recv Data Timeout，Export Fail");
			bResult = false;
			goto _DOWNLOAD_END;
		}

		iRecvCont = 0;
		memcpy((char *)&objDataBuf, szRecvbuf, iRecvLen);
		iGetRecvLen = 0;
		if(objDataBuf.m_iDataLen != (iRecvLen - 4)) // 数据包没有接收完整，继续接收
		{
			iGetRecvLen = iRecvLen;
			continue;
		}


		memcpy(&objLogContent, objDataBuf.szRecvBuf, sizeof(objLogContent));
		if(objLogContent.m_bytType != 0x02)
		{
			AfxMessageBox("Recv Cmd Type Err，Export Fail");
			bResult = false;
			goto _DOWNLOAD_END;
		}
		
		myfile.Write(objDataBuf.szRecvBuf + sizeof(objLogContent), objLogContent.m_iPacketSize);

		memset(szSendBegainCmd, 0, sizeof(szSendBegainCmd));
		iSendLen = 0;
		bytType = v_iType;	// 数据类型
		bytCmd = 0x02;	// 传输日志内容
		iLen =  sizeof(bytType) + sizeof(bytCmd) + 2;//数据头长度：类型+数据内容+结束符
		memcpy(szSendBegainCmd, &iLen, sizeof(iLen));
		iSendLen += sizeof(iLen);
		szSendBegainCmd[iSendLen++] = bytType;
		szSendBegainCmd[iSendLen++] = bytCmd;
		if(objLogContent.m_iPacketNum == objLogInfoCmd.m_iTotalPacket)
		{
			szSendBegainCmd[iSendLen++] = 0x01;// 已经接收完整
		}
		else
		{
			szSendBegainCmd[iSendLen++] = 0x00;// 正在接收中
		}
		
		szSendBegainCmd[iSendLen++] = 0x00;
		iRet = send(sSockClient, szSendBegainCmd, iSendLen, 0);
		if(SOCKET_ERROR == iRet)
		{
			AfxMessageBox("Send Data Fail，Export Fail");
			bResult = false;
			goto _DOWNLOAD_END;
		}

		if(objLogContent.m_iPacketNum == objLogInfoCmd.m_iTotalPacket)
		{
			break;
		}
	}

_DOWNLOAD_END:
	CloseSocket(sSockClient);
	
	if((myfile.m_hFile != CFile::hFileNull) && (iOpenFileRet != 0))
	{
		myfile.Close();
	}

	if(!bResult)
	{
		//删除创建的文件
		if(iOpenFileRet)
		{
			DeleteFile(filepath);
		}
	}

	return bResult;
}

