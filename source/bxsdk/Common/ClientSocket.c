#include "ClientSocket.h"
#include "../Common/Queue.h"
#include "../LogOut/LogOut.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>


static SocketData socketInfo[MAXSOCK];
static Queue *	  g_quene=NULL;
static BOOL 	  g_bExitSocket = FALSE;
static pthread_mutex_t m_SockMutex;

static struct hostent *gethostnameinfo (const char *host)
{
	struct hostent hostbuf, *hp;
	size_t hstbuflen;
	char tmphstbuf[1024];
	int res;
	int herr;
	hstbuflen = 1024;
	res = gethostbyname_r (host, &hostbuf, tmphstbuf, hstbuflen,
		&hp, &herr);
	/*  Check for errors.  */
	if (res || hp == NULL)
		return NULL;
	return hp;
}

static int AddSocketId(int socketId,DWORD v_dwSpaceSize, DWORD v_dwInvalidPeriod)
{
	int isReconnect = -1;
	int i = 0;
	BOOL isfindSocketFree=FALSE;
	if(socketId <= 2)
	{
		LOGOUT("addrdfds param error");
		return -1;
	}
	pthread_mutex_lock(&m_SockMutex);
	for(i=0;i<MAXSOCK;i++)
	{
		if(socketInfo[i].socketId == -1)	
		{
			socketInfo[i].socketId=socketId;
			socketInfo[i].pIInnerDataList=(InnerDataList*)ListConstruction();
			if(socketInfo[i].pIInnerDataList)
				socketInfo[i].pIInnerDataList->initCfg(socketInfo[i].pIInnerDataList->_this,v_dwSpaceSize,v_dwInvalidPeriod);
			else
			{
				LOGOUT("ListConstruction is error");
				isfindSocketFree=FALSE;
				break;
			}
			isfindSocketFree=TRUE;
			LOGOUT("AddRdfds new socket %d", socketInfo[i].socketId);
			break;
		}
	}
	pthread_mutex_unlock(&m_SockMutex);
	if(i>=MAXSOCK)
	{
		LOGOUT("AddRdfds error,MAXSOCK");
		return -2;
	}
	if(!isfindSocketFree)
	{
		return -3;
	}
	return 0;
}

static void *SendProc(void *data)
{
	char *buf = malloc(SIZE_VIDEO_PACKED);
	if(NULL == buf)
	{	
		LOGOUT("malloc failure---no memory");
		return NULL;
	}
	char *errorBuf = "send socket error";
	fd_set fdSocket,eset;        // 所有可用套节字集合
	int i = 0,j = 0;
	int sendSize = 0;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 200*1000;
	DWORD recvLen = 0;
	DWORD sym;
	int iOneLoopSendLen = 0;
	int iOneLoopMaxSendLen = 256*1024;
	DWORD dwSleep = 200*1000;
	BOOL bFindSock = FALSE;
	DWORD dwPacketNum = 0;
	SocketData mSocketData;
	while(!g_bExitSocket)
	{
		usleep(dwSleep);
		iOneLoopSendLen = 0;
		for (i = 0; i< 200; i++)
		{
			memset(buf,0,SIZE_VIDEO_PACKED);
			int ret = g_quene->popData(g_quene->_this, (void*)buf, SIZE_VIDEO_PACKED, &recvLen, &sym, &mSocketData.socketId);
			if(ret == 0)
			{
				if(mSocketData.socketId < 2)
					continue;
				FD_ZERO(&fdSocket);
				FD_ZERO(&eset);
				FD_SET(mSocketData.socketId, &eset);
				FD_SET(mSocketData.socketId, &fdSocket);
				ret = select(mSocketData.socketId+1, NULL, &fdSocket, &eset, &tv);
				if(ret < 0)
					continue;
				pthread_mutex_lock(&m_SockMutex);
				bFindSock = FALSE;
				for(j = 0; j<MAXSOCK; j++)
				{
					if(socketInfo[j].socketId == mSocketData.socketId)
					{
						mSocketData=socketInfo[j];
						bFindSock = TRUE;
						break;
					}
				}
				pthread_mutex_unlock(&m_SockMutex);
				if(!bFindSock)
					continue;
				if(FD_ISSET(mSocketData.socketId, &eset) )
				{
					LOGOUT("Socket select write except!");
					continue;
				}
				else if(FD_ISSET(mSocketData.socketId, &fdSocket) )
				{
					//printf("send client:");
					//for(i=0;i<10;i++)
					//	printf("0x%02x ",(unsigned char)*(buf+i));
					//printf("---send msgbody:%s\n",buf+10);
					sendSize = send(mSocketData.socketId, buf, recvLen, 0);
					if(sendSize == -1)
					{

						if(mSocketData.pIInnerDataList != NULL)
						{
							mSocketData.pIInnerDataList->pushData(mSocketData.pIInnerDataList->_this, 1,strlen(errorBuf),errorBuf,CMDRECONNECT, &dwPacketNum);
						}
						LOGOUT("send size is %d ,mSocketData.socketId = %d, Error code %d，error msg:%s",sendSize, mSocketData.socketId,errno, strerror(errno));
						break;
					}
					//LOGOUT("send size is %d ,mSocketData.socketId = %d ",sendSize, mSocketData.socketId);
					if(sendSize != recvLen)
					{
						LOGOUT("send fail mSocketData.socketId = %d,sendSize = %d,recvLen = %d",mSocketData.socketId, sendSize, recvLen);
						break;
					}
					iOneLoopSendLen += sendSize;
					if(iOneLoopSendLen > iOneLoopMaxSendLen)
					{
						//LOGOUT("iOneLoopSendLen = %d", iOneLoopSendLen);
						break;
					}
				}	
			}
			else if(ret == ERR_QUEUE_EMPTY)
			{
				usleep(dwSleep);
			}
		}
	}
	LOGOUT("Exit SendProc");
	if(buf)
		free(buf);
	return NULL;
}

static void *RecvProc(void *data)
{
	fd_set fdSocket;        // 所有可用套节字集合
	int i=0;
	char *errorBuf = "recv socket error";
	char buf[SOCKET_PARKET_SIZE] = {0};
	int maxSocket = -1;
	int ret = -1;
	SocketData mSocketData;
	DWORD dwPacketNum = 0;
	struct tcp_info info;  
	int len=sizeof(info); 
	struct timeval tv;
	sleep(1);
	while(!g_bExitSocket)
	{
		FD_ZERO(&fdSocket);	
		maxSocket = -1;
		tv.tv_sec = 0;
		tv.tv_usec = 100*1000;
		pthread_mutex_lock(&m_SockMutex);
		for(i=0; i<MAXSOCK; i++)
		{
			if(socketInfo[i].socketId > 2)
			{
				FD_SET(socketInfo[i].socketId, &fdSocket);
				if(maxSocket < socketInfo[i].socketId)
				{
					maxSocket = socketInfo[i].socketId;
				}
			}
		}
		pthread_mutex_unlock(&m_SockMutex);
		ret = select(maxSocket+1, &fdSocket, NULL, NULL, &tv);
		if (ret == 0)  //超时
		{
			continue;
		}		   
		for(i=0; i<MAXSOCK; i++)
		{
			pthread_mutex_lock(&m_SockMutex);
			mSocketData=socketInfo[i];
			pthread_mutex_unlock(&m_SockMutex);
			if(mSocketData.socketId>0)
			{
				memset(&info,0,sizeof(info)); 
				getsockopt(mSocketData.socketId, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);  
				if(info.tcpi_state!=1) 
				{  
					if(mSocketData.pIInnerDataList != NULL)
					{
						LOGOUT("getsockopt client[%d] close, socket = %d, ret = %d,Error code %d，error msg:%s", 
						i,mSocketData.socketId, ret, errno, strerror(errno));
						mSocketData.pIInnerDataList->pushData(mSocketData.pIInnerDataList->_this, 1,strlen(errorBuf),errorBuf,CMDRECONNECT, &dwPacketNum);
					}
					continue;
				} 
			}
			if(mSocketData.socketId < 2)
				continue;
			if(FD_ISSET(mSocketData.socketId, &fdSocket))
			{
				memset(buf,0,sizeof(buf));
				ret = recv(mSocketData.socketId, buf, sizeof(buf), 0);
				if (ret < 0) //接收数据出错
				{       
					if(mSocketData.pIInnerDataList != NULL)
					{
						mSocketData.pIInnerDataList->pushData(mSocketData.pIInnerDataList->_this, 1,strlen(errorBuf),errorBuf,CMDRECONNECT, &dwPacketNum);
					}
					LOGOUT("Recv client[%d] close, socket = %d, ret = %d,Error code %d，error msg:%s", 
						i,mSocketData.socketId, ret, errno, strerror(errno));
				}
				else if(ret == 0)
				{
					continue;
				}
				else  // 数据接收成功
				{  
					//将接收数据的最后一位补0
					if (ret <= SOCKET_PARKET_SIZE)
					{
						if(mSocketData.pIInnerDataList != NULL)
						{
							mSocketData.pIInnerDataList->pushData(mSocketData.pIInnerDataList->_this, 1,ret,buf,CMDCORRECT,&dwPacketNum);
							//printf("Recv client:");
							//for(i=0;i<10;i++)
							//	printf("0x%02x ",(unsigned char)*(buf+i));
							//printf("---recv msgbody:%s\n",buf+10);
						}
					}
				}

			}
		}
		usleep(100*1000);
	}
	LOGOUT("Exit RecvProc");
	return NULL;
}

int CreateConnect(LPCTSTR ip, int port,DWORD v_dwSpaceSize, DWORD v_dwInvalidPeriod)
{
	if(ip == NULL || port <1)
		return -1;
	if(strlen(ip)==0)
		return -2;
	LOGOUT("CreateConnectip:%s, port:%d",ip, port);
	int sockFd;
	struct sockaddr_in server = {0};
	struct hostent *he;
	struct timeval timeo;
	timeo.tv_sec  = 2;
	timeo.tv_usec = 0;
	socklen_t len = sizeof(timeo);
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockFd, SOL_SOCKET, SO_SNDTIMEO, &timeo, len);
	int nSendBuf = 64*1024;//设置为64k
	setsockopt(sockFd,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
	he=gethostnameinfo(ip);
	if (he != NULL) 
	{
		memset(&server, 0, sizeof(server));
		memcpy(&server.sin_addr, he->h_addr_list[0], sizeof(server.sin_addr));
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
	}
	else
	{
		printf("CreateConnect:gethostbyname is null\n");
		close(sockFd);//如果失败 关闭socket
		sockFd = -1;
		goto _CREATE_END;
	}
	int res = connect(sockFd, (struct sockaddr*)&server, sizeof(server));
	if (res < 0)
	{
		if(res == EINPROGRESS)
		{
			perror("CreateConnect:connecting stream socket time out:");
		}
		printf("CreateConnect:connect is failure\n");
		close(sockFd);
		sockFd = -1;
		goto _CREATE_END;
	}
	else
	{
		res = AddSocketId(sockFd,v_dwSpaceSize,v_dwInvalidPeriod);
		if(res!=0)
		{
			LOGOUT("CreateConnect:AddRdfds failed");
			sockFd =  -1;
			goto _CREATE_END;
		}
	}
	
_CREATE_END:
	return sockFd;
}

int DelSocketId(int socketId)
{
	int i = 0;
	if(socketId <= 2)
	{
		LOGOUT("addrdfds param error");
		return -1;
	}
	pthread_mutex_lock( &m_SockMutex );
	for(i=0; i< MAXSOCK; i++)
	{
		if(socketInfo[i].socketId == socketId)	
		{
			socketInfo[i].socketId = -1;
			if(socketInfo[i].pIInnerDataList)
				socketInfo[i].pIInnerDataList->release(socketInfo[i].pIInnerDataList->_this);
			socketInfo[i].pIInnerDataList=NULL;
			LOGOUT("Delete Socket %d",socketId);
			break;
		}
	}
	pthread_mutex_unlock(&m_SockMutex);
	if(socketId>0)
		close(socketId);
	return 0;
	
}

int SendSocketData(int socketId,char *sendbuffer, DWORD length)
{ 	
	if(sendbuffer==NULL)
		return -1;
	int ret=g_quene->pushData(g_quene->_this,(void*)sendbuffer,length,0,socketId);
	return ret;
}

int GetSocketData(int socketId,DWORD *v_dwSymb,DWORD v_dwBufSize,char *recBuffer,DWORD *length)
{
	if(recBuffer==NULL || length==NULL)
		return -1;
	int i;
	int iRet=-1;
	BYTE  v_bytLvl;
	pthread_mutex_lock( &m_SockMutex );
	for(i=0; i< MAXSOCK; i++)
	{
		if(socketInfo[i].socketId == socketId)	
		{
			if(socketInfo[i].pIInnerDataList)
			{
				iRet=socketInfo[i].pIInnerDataList->popData(socketInfo[i].pIInnerDataList->_this,
				&v_bytLvl,v_dwSymb,v_dwBufSize,length,recBuffer);
			}
			break;
		}
	}
	pthread_mutex_unlock(&m_SockMutex);
	return iRet;
}

int InitNetwork(int v_iCacheSize)
{
	int i =0;
	int iRet=0;
	pthread_mutex_init(&m_SockMutex, NULL);
	for(i=0; i< MAXSOCK; i++)
	{
		socketInfo[i].socketId=-1;
		socketInfo[i].pIInnerDataList=NULL;
	}
	g_bExitSocket = FALSE;
	
	g_quene = (Queue*)QueueListConstruction();
	if(g_quene)
	{	
		iRet=g_quene->initQueue(g_quene->_this,v_iCacheSize);
		if(iRet!=0)
		{
			LOGOUT("InitNetwork initqueue error\n");
			goto FAILOUT;
		}
	}
	else
	{
		LOGOUT("InitNetwork QueueListConstruction error\n");
		goto FAILOUT;
	}
	
	pthread_t recvThread, sendThread;
	iRet = pthread_create(&recvThread, NULL, RecvProc, NULL);
	if(iRet!=0)
	{
		LOGOUT("InitNetwork create recvThread error\n");
		goto FAILOUT;
	}
	pthread_detach(recvThread);
	
	iRet = pthread_create(&sendThread, NULL, SendProc, NULL);
	if(iRet!=0)
	{
		LOGOUT("InitNetwork create sendThread error\n");
		goto FAILOUT;
	}
	pthread_detach(sendThread);
	return 0;
FAILOUT:
	ReleaseNetwork();
	return iRet;
}

void ReleaseNetwork()
{
	g_bExitSocket=TRUE;
	g_quene->release(g_quene->_this);
	pthread_mutex_destroy(&m_SockMutex);
	if(g_quene)
	{
		free(g_quene);
		g_quene = NULL;
	}
	LOGOUT("ReleaseNetwork over");
}

