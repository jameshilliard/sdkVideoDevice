#include "RouteServer.h"
#include "../Common/ClientSocket.h"
#include "../LogOut/LogOut.h"

int GetControlServer(LPCTSTR ip,int port,LPCTSTR user,LPTSTR controServerIP,int length)
{
	if(ip == NULL || port<1)
		return -1;
 	if(strlen(ip)==0)
		return -2;
	int socketId=-1;
	int iRet=-1;
	static MsgHead gMsgHead;
	MsgHead recMsgHead;
	socketId=CreateConnect(ip,port,128*1024,1000000000);
	if(socketId<0)
	{
		return socketId;
	}
	char socketBuffer[1024]={0,};
	gMsgHead.uHeadLen=sizeof(MsgHead);
	gMsgHead.cMsgVersion=MSG_HEAD_VERSION;
	gMsgHead.cPlatform=MSG_HEAD_PLATFORM;
	gMsgHead.uMsgType=ROUTE_PROTOCAL_GET_ROUTE;
	gMsgHead.uMsgSeq++;
	gMsgHead.uBodyLen=0;
	memset(socketBuffer,0,sizeof(socketBuffer));
	char *base64User=SY_base64Encode(user);
	if(base64User==NULL)
	{		
		LOGOUT("base64Encode error");
		iRet=-3;
		goto FailOut1;
	}
	sprintf(socketBuffer+sizeof(MsgHead),"[%s:%d]",base64User,MSG_HEAD_PLATFORM);
	gMsgHead.uBodyLen=strlen(socketBuffer+sizeof(MsgHead));
	memcpy(socketBuffer,&gMsgHead,sizeof(MsgHead));
	iRet=SendSocketData(socketId,socketBuffer,gMsgHead.uBodyLen+sizeof(MsgHead));
	if(iRet==0)
	{
		DWORD v_dwSymb;
		DWORD recLength;
		INT32S	succssFlag=0;
		INT32U 	iTemp=0;
		memset(socketBuffer,0,sizeof(socketBuffer));
		memset(&recMsgHead,0,sizeof(MsgHead));
		while(1)
		{
			iRet=GetSocketData(socketId,&v_dwSymb,sizeof(socketBuffer),socketBuffer,&recLength);
			if(iRet==0)
			{
				if(recLength>sizeof(MsgHead))
				{
					memcpy(&recMsgHead,socketBuffer,sizeof(MsgHead));
					if(recMsgHead.uHeadLen==sizeof(MsgHead) && recMsgHead.uMsgType==ROUTE_PROTOCAL_GET_ROUTE_ACK &&
					   recMsgHead.uBodyLen==(recLength-sizeof(MsgHead)))
					{
						if(recMsgHead.uBodyLen>length)
						{
							LOGOUT("GetControServer get ip too big %d",(recLength-sizeof(MsgHead)));
							goto FailOut2;
						}
						else
						{
							memset(controServerIP,0,length);
							iRet=sscanf(socketBuffer+sizeof(MsgHead),"[%d:%s]",&succssFlag,controServerIP);
							iTemp=strlen(controServerIP);
							*(controServerIP+iTemp-1)='\0';
							if(succssFlag==1)
								iRet=0;
							else
								iRet=-5;
							LOGOUT("GetControServer ip is %s:flag:%d",controServerIP,succssFlag);
							break;
						}
					}
					else
					{
						LOGOUT("GetControServer data packet error");
						goto FailOut2;
					}
				}
				else
				{
					LOGOUT("GetControServer get data too short %d",recLength);
					goto FailOut2;
				}
			}
			sleep(1);
		}
	}
FailOut2:
	free(base64User);
FailOut1:
	DelSocketId(socketId);
	return iRet;

}
