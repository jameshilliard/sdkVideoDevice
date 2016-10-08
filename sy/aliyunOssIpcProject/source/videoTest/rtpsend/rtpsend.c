#include "rtpsend.h"
#include <stdlib.h>



#define MSG_SENDSIZE	2048 
#define MSG_RECVSIZE	2048
#define MSG_SIGNALSIZE  2048
 
#define PACKET_CMD      1 
#define PACKET_ERROR   -1
#define PACKET_AUDIO    2

const int VSize[][2] = {
	{160,120},			    //DQVGA = 0,
	{176,144},			    //QCIF = 1,
	{320,240},              //QVGA = 2,
	{352,288},			    //CIF	= 3,
	{640,480},			    //VGA  = 4,
	{720,288},			    //HD1  = 5,
	{720,576},			    //D1 = 6,
	{704,576},			    //FOURCIF = 7,
	{704,288},				//BCIF = 8,
	{352,576},				//HALFD = 9,
	{800,600},				//SVGA = 10,
	{1024,768},				//XGA = 11 ,
	{1280,720},				//720P = 12,
	{1600,1200},			//UXGA = 13,
	{1920,1080},			//1080P = 14
	{432,240},				//p432 = 15
};

static BOOL    bExitRtpSendThread = FALSE;

INT32S isValidPacket(byte *ptr,INT32U size,INT32U *leftSize)
{ 
    struct  TMSG_HEADER *pMsgHeader = (struct TMSG_HEADER *)ptr; 
    if(pMsgHeader->m_iXmlLen > size || (char)(*((char *)(ptr+pMsgHeader->m_iXmlLen+4-1))) != 0){
        return PACKET_ERROR;
    }
    if((pMsgHeader->session != CMDSESSION) && (pMsgHeader->session != SZYAUDIO_EX)){
    
		return PACKET_ERROR;
	}

	if(pMsgHeader->session == CMDSESSION){
    	if(pMsgHeader->cMsgID < MSG_CAPLOGIN || pMsgHeader->cMsgID >MSG_CMDMAX){
        	return PACKET_ERROR;
        }
    }
    *leftSize = size-pMsgHeader->m_iXmlLen-4;
    if(pMsgHeader->session == CMDSESSION)
        return PACKET_CMD;
    else if(pMsgHeader->session == SZYAUDIO_EX)
        return PACKET_AUDIO;
}

INT32S RtpSendSendLoginData(INT32S iSocketID,const char *m_szCameraID)
{ 
    char packet[256] = {0};
    static int counter = 0;
    counter++;
    if(counter > 50)
        counter = 0;
    struct TMSG_CAPLOGIN msgLogin;
    int iRet;
    if(iSocketID < 0 || m_szCameraID == NULL)
        return -1;
    memset(&msgLogin,0,sizeof(struct TMSG_CAPLOGIN));
    msgLogin.header.m_iXmlLen = sizeof(struct TMSG_CAPLOGIN)-4+counter;
    if((msgLogin.header.m_iXmlLen+4) >= sizeof(packet))
    {
        counter=0;
        msgLogin.header.m_iXmlLen = sizeof(struct TMSG_CAPLOGIN)-4+counter;
    }
    msgLogin.header.m_bytXmlType = 0;
    msgLogin.header.session = CMDSESSION;
    msgLogin.header.cMsgID = MSG_CAPLOGIN;
    strcpy(msgLogin.password, "9999jecky");
    strcpy(msgLogin.SxtID, m_szCameraID);
    msgLogin.endChar = 0;
    memcpy(packet,(char*)&msgLogin,sizeof(struct TMSG_CAPLOGIN));
     /* 发消息给服务器 */
    iRet = SzySdkSendData(iSocketID, (char*)packet, msgLogin.header.m_iXmlLen+4);
    
    if (iRet  < 0) {
        LOGOUT("msg '%s' fail %s! error code %d，error msg'%s'",
             (char*)&msgLogin,m_szCameraID,errno, strerror(errno));
        return -1;
    } 
    else
    {
       //LOGOUT("msg: %s send success %s，send %d bytes \n",
          //   (char*)&msgLogin,m_szCameraID, iRet);
        return 0;
    } 

}

INT32S RtpSendSendKeepAlive(INT32S iSocketID,const char *m_szCameraID)
{ 
    struct TMSG_KEEPALIVE msKeepAlive;
    int iRet;
    if(iSocketID < 0 || m_szCameraID == NULL)
        return -1;
    memset(&msKeepAlive,0,sizeof(struct TMSG_KEEPALIVE));
    msKeepAlive.header.m_iXmlLen = sizeof(struct TMSG_KEEPALIVE)-4;
    msKeepAlive.header.m_bytXmlType = 0;
    msKeepAlive.header.session = CMDSESSION;
    msKeepAlive.header.cMsgID = MSG_KEEPALIVE;
    msKeepAlive.endChar = 0;
     /* 发消息给服务器 */
    iRet = SzySdkSendData(iSocketID, (char*)&msKeepAlive, sizeof(struct TMSG_KEEPALIVE));
    if (iRet  < 0) {
        LOGOUT("msg '%s' fail %d! error code %d，error msg'%s'\n",
             (char*)&msKeepAlive, m_szCameraID,errno, strerror(errno));
        return -1;
    } 
    else
    {
        //LOGOUT("msg: %s send success %s，send %d bytes \n",
        //(char*)&msKeepAlive,m_szCameraID, iRet);
        return 0;
    }      
}

INT32S ProcLoginRetMSG(struct TMSG_HEADER *pMsgHeader,INT32S iSocketID,const char *m_szCameraID)
{
    if(iSocketID < 0 || m_szCameraID == NULL)
        return INVALID_RETMSG;
	struct TMSG_LOGINRET *pMsgRet = (struct TMSG_LOGINRET*)pMsgHeader;
	if(pMsgRet->m_nStatus == MSG_CAPLOGINSUCCESS)
	{
		//LOGOUT("%s rtpSend::MSG_CAPLOGINSUCCESS",m_szCameraID);
		return MSG_CAPLOGINSUCCESS;
	}
	else if(pMsgRet->m_nStatus == MSG_CAPLOGINPSWRONG)
	{
        RtpSendSendLoginData(iSocketID,m_szCameraID);
		LOGOUT("%s rtpSend::MSG_CAPLOGINPSWRONG",m_szCameraID);
		return MSG_CAPLOGINPSWRONG;
	}
	else if(pMsgRet->m_nStatus == MSG_CAPLOGINHASONE)
	{
		LOGOUT("%s rtpSend::MSG_CAPLOGINHASONE",m_szCameraID);
		return MSG_CAPLOGINHASONE;
	}
}

static inline void ProcPTZCommand(struct TMSG_HEADER *pMsgHeader,RtpSendCameraInfo *param,BOOL *bNoAckStop)
{
	struct TMSG_PTZCOMMAND *ptz = (struct TMSG_PTZCOMMAND*)pMsgHeader;
	if (ptz->m_nFunc >= 4 && ptz->m_nFunc <= 8)
	{
		if(*bNoAckStop)   //对于设置云台设置预置点的功能不响应停止命令。
		{
			*bNoAckStop = FALSE;
			return;
		}
		if(ptz->m_nFunc == 8)
			*bNoAckStop = TRUE;
		INT8U channel = (unsigned char)(param->m_iChannel);
		INT8U szFunc = (unsigned char)(ptz->m_nFunc);
		INT8U szControl = (unsigned char)(ptz->m_nCtl);
		INT8U szPara = (unsigned char)(ptz->m_nSpeed);
	
		//zss++if(NULL != g_ControlPZT)
		{
			LOGOUT("g_ControlPZT channel:%d,m_nFunc:%d,m_nCtl:%d,m_nSpeed:%d",channel,szFunc, szControl, szPara);
			//zss++g_ControlPZT(channel,szFunc, szControl, szPara);
		}
	}
}


static inline void AnalyzeAudioData(unsigned char *v_szAudioData, int v_iLen)
{
	if(sizeof(AUD_HDR_NEW) > v_iLen)
	{
		return ;
	}
	AUD_HDR_NEW sAudHdrNew;
	memcpy(&sAudHdrNew, v_szAudioData, sizeof(AUD_HDR_NEW));
    //zss++if(NULL != g_RecvAudioDataCallBack)
    {
	    //zss++g_RecvAudioDataCallBack(sAudHdrNew.header.nChannel, sAudHdrNew.header.nSamplePerSec, sAudHdrNew.header.wBitPerSample, sAudHdrNew.header.wFormat
		//zss++,v_szAudioData + sizeof(AUD_HDR_NEW), sAudHdrNew.datalen);
        //LOGOUT("rtpSend AnalyzeAudioData %d  %d  %d %d %d\n",
        //sAudHdrNew.header.nChannel, sAudHdrNew.header.nSamplePerSec, 
        //sAudHdrNew.header.wBitPerSample, sAudHdrNew.header.wFormat,
    	// sAudHdrNew.datalen);
    }

}


static INT32S ProcGetParamConfig(RtpSendCameraInfo *param)
{
	struct TMSG_PARAMCONFIG tMsgParamConfig;
	unsigned int m_brate = 0, m_frate = 0, m_width = 0;
	unsigned int m_high = 0, m_qp = 0, m_keyFrameRate = 0;
	INT32S iRet = -1;
	SetSZYSdkRet szySetRet = SetSZYSdkRet_PRMErr;
	//zss++if(NULL != g_GetVidioParamFun)
	{
	    //zss++SetSZYSdkRet szySetRet = g_GetVidioParamFun(param->m_iChannel, &m_brate, &m_frate, 
	    //zss++                    &m_qp, &m_keyFrameRate, &m_width, &m_high);             
	}                       
	LOGOUT("%s g_GetVidioParamFun channel:%d,uBitRate:%d,frameRate:%d,qpConstant:%d,keyFrameRate:%d,width:%d,high:%d", 
		param->m_szCameraID,param->m_iChannel,m_brate, m_frate, m_qp, m_keyFrameRate, m_width, m_high);
	if (SetSZYSdkRet_OK != szySetRet)
	{
		LOGOUT("%s g_GetVidioParamFun faild !!",param->m_szCameraID);	
		return -1;
	}

    memset(&tMsgParamConfig,0,sizeof(struct TMSG_PARAMCONFIG));
    tMsgParamConfig.header.m_iXmlLen = sizeof(struct TMSG_PARAMCONFIG)-4;
    tMsgParamConfig.header.m_bytXmlType = 0;
    tMsgParamConfig.header.session = CMDSESSION;
    tMsgParamConfig.header.cMsgID = MSG_PARAMCONFIG;
    tMsgParamConfig.endChar = 0;
	tMsgParamConfig.param.quality = m_qp;
	tMsgParamConfig.param.nBitRate = m_brate;
	tMsgParamConfig.param.bAudio = param->m_bOpenAudio;
	tMsgParamConfig.param.bNetAdaptive = param->m_bNetAdaptive;
	tMsgParamConfig.param.dbQuality = m_qp;
	tMsgParamConfig.param.sxtType = 5;
	tMsgParamConfig.param.comPort = 0;
	//tMsgParamConfig.param.vs = switchVideoSize(m_width, m_high);
	tMsgParamConfig.param.fr = m_frate;
	//
	tMsgParamConfig.param.version = 2013;
	/* 发消息给服务器 */
    iRet = SzySdkSendData(param->m_iSocketID, (char*)&tMsgParamConfig, sizeof(struct TMSG_PARAMCONFIG));
    if (iRet  < 0) {
        LOGOUT("msg '%s' fail %s! error code %d，error msg'%s'\n",
             (char*)&tMsgParamConfig, param->m_szCameraID,errno, strerror(errno));
        return -1;
    } 
    else
    {
       //LOGOUT("msg: %s send success %s，send %d bytes \n",
        //     (char*)&tMsgParamConfig,param->m_szCameraID, iRet);
        return 0;
    }
}


static INT32S ProcParamConfig(struct TMSG_HEADER *pMsgHeader,RtpSendCameraInfo *setParam)
{
	char *buf  = (char *)pMsgHeader;
	char str[20] = {0};
	sParamConfig param;
	memset((char *)&param,0,sizeof(sParamConfig));
	memcpy((char *)&param,buf+12,8);
	//zss++if (g_SwitchAudioSendCallBack != NULL)
	{
		LOGOUT("%s g_SwitchAudioSendCallBack channel:%d, m_openAudio:%d",setParam->m_szCameraID,setParam->m_iChannel, setParam->m_bOpenAudio);
		//zss++g_SwitchAudioSendCallBack(setParam->m_iChannel,  (setParam->m_bOpenAudio)? szyTrue:szyFalse);
	}
	if((param.fr < 1) || (param.fr  > 25))
	{
		LOGOUT("%s bitRate:%d,frameRate:%d",setParam->m_szCameraID,param.nBitRate, param.fr);
		param.fr = 8;	
	}
    ModifyVideoParam(setParam->m_iChannel,param.nBitRate, param.fr, param.quality,128,VSize[param.vs][0], VSize[param.vs][1]);
	if(setParam->m_bOpenAudio != param.bAudio|| setParam->m_bNetAdaptive != param.bNetAdaptive)
    {
        setParam->m_bNetAdaptive =param.bNetAdaptive;
	    setParam->m_bOpenAudio = param.bAudio;
        tagCapParamCfg objCapParamCfg;
        memset(&objCapParamCfg, 0, sizeof(tagCapParamCfg));
        GetCfgFile(&objCapParamCfg, sizeof(tagCapParamCfg), offsetof(tagConfigCfg, m_unCapParamCfg.m_objCapParamCfg[setParam->m_iChannel]), sizeof(tagCapParamCfg));
        objCapParamCfg.m_bAdaptStream =  setParam->m_bNetAdaptive;
        objCapParamCfg.m_bOpenAudio = setParam->m_bOpenAudio;
        SetCfgFile(&objCapParamCfg, offsetof(tagConfigCfg, m_unCapParamCfg.m_objCapParamCfg[setParam->m_iChannel]), sizeof(tagCapParamCfg));
    }
	return 0;
}

// 控制连接线程
static void *P_RtpsendThread(void *param)
{

	BOOL    bRecAudio = FALSE;
	BOOL    bNextCmd = FALSE;
    BOOL    bNoAckStop = FALSE;
	byte    bytSta = T_Idle;
	byte    byLastSta = T_Idle;
	//byte  szSendBuf[MSG_SENDSIZE]={0};
	byte    szRecvbuf[MSG_SENDSIZE]={0};
	byte    szResolvebuf[MSG_SENDSIZE]={0};
	//byte *szRecvBufbakptr = szRecvbuf;
	byte    *szResolveptr;
	INT32U  leftSize=0;
	INT32U  ikeepAliveCount=0;
	INT32U 	iFailConnectExepLogOuct=0;
	INT32U  dwSleep = 100;
	INT32U  dwLastSendActiveTime = 0;
	INT32U  dwLastReciceActiveTime = 0;
	INT32S  iConnFailCnt = 0;
	INT32S  iSendLen = 0;
	INT32S  iRecvLen = 0;
	INT32S  i;
	INT32S  bPacketValid = -1;
	INT32S  sendErrorTimes=0;
    struct  TMSG_HEADER     *pMsgHeader;
	struct  TMSG_SENDSWITCH *pMsgSwitch;
    RtpSendCameraInfo *ptrRtpSendParam = (RtpSendCameraInfo *)param;

	int 	iRet=-1;
	DWORD 	v_dwSymb=-1;
	char  	recBuffer[SOCKET_PARKET_SIZE]={0,};
	DWORD 	length=0;
    
	while (!bExitRtpSendThread)
	{
        if(!bNextCmd)
        {
            memset(szRecvbuf,0,sizeof(szRecvbuf));
			LOGOUT("%d test",ptrRtpSendParam->m_iSocketID);
			iRet=GetSocketData(ptrRtpSendParam->m_iSocketID,&v_dwSymb,sizeof(recBuffer),recBuffer,&length);
			if(iRet==0)
			{
                switch (v_dwSymb)
                {    
                    case CORRECT: 
                        LOGOUT("%d CORRECT",ptrRtpSendParam->m_iSocketID);
                        memset(szResolvebuf,0,sizeof(szResolvebuf));
                        memcpy(szResolvebuf,recBuffer,length);
                        bNextCmd = TRUE;
                        iRecvLen = length;
                        leftSize = iRecvLen;
                        break;
                    case RECONNECT:
                        LOGOUT("%d RECONNECT",ptrRtpSendParam->m_iSocketID);
                        iRecvLen = -1;
                        leftSize = -1;
                        bytSta = T_DisConn;
                        break;
                    case CTRL_MSG_SENDCAMINFO:
                        LOGOUT("%d CTRL_MSG_SENDCAMINFO",ptrRtpSendParam->m_iSocketID);
                        iRecvLen = -1;
                        leftSize = -1;
                        bytSta = T_DisConn;
                        break;
                    case CTRL_MSG_DELETECAMINFO:
                        LOGOUT("%d CTRL_MSG_DELETECAMINFO",ptrRtpSendParam->m_iSocketID);
                        iRecvLen = -1;
                        leftSize = -1;
                        bytSta = T_DisConn;
                        break;
                    case CTRL_MSG_MODIFYCAMINFO:
                        LOGOUT("%d CTRL_MSG_MODIFYCAMINFO",ptrRtpSendParam->m_iSocketID);
                        iRecvLen = -1;
                        leftSize = -1;
                        bytSta = T_DisConn;
                        break; 
                    default:
                        LOGOUT("%d default",ptrRtpSendParam->m_iSocketID);
                        iRecvLen = -1;
                        leftSize = -1;
                        break;
                } 

            }
        }
		if(iRecvLen > 0 && leftSize > 0)
		{
            szResolveptr = szResolvebuf+(iRecvLen-leftSize);
            bPacketValid  = isValidPacket(szResolveptr,leftSize,&leftSize);
            if(PACKET_ERROR == bPacketValid)
            {
                iRecvLen = -1;
                leftSize = -1;
                bNextCmd = FALSE;
                LOGOUT("CPACKET_ERROR");
            }
            else
            {
                LOGOUT(" PACKET_CMD %d %d ",iRecvLen,leftSize);
            } 
            
	    }
	    else
	    {
            iRecvLen = -1;
            leftSize = -1;
            bNextCmd = FALSE;

            bPacketValid = PACKET_ERROR;
            LOGOUT("CPACKET_ERROR");
	    }   
		switch(bytSta)
		{
            case T_Idle:
            	{   
            		dwSleep = 100;
            		if((0 == strlen(ptrRtpSendParam->m_szRemoteIP)) || (0 == ptrRtpSendParam->m_iRemotePort))
            		{
            			break;
            		}
            		byLastSta = T_Idle;
            		bytSta = T_Conn;
            	}
                break;
            case T_Conn:
            	{
					sendErrorTimes=0;
					ptrRtpSendParam->m_iSocketID = CreateConnect(ptrRtpSendParam->m_szRemoteIP, 
                                                   ptrRtpSendParam->m_iRemotePort,32*1024,1000000000); 
            		if(-1 == ptrRtpSendParam->m_iSocketID)
            		{
            			iConnFailCnt ++;
						iFailConnectExepLogOuct++;	
						LOGOUT("%s	%d %s T_Working error",ptrRtpSendParam->m_szRemoteIP,ptrRtpSendParam->m_iRemotePort,ptrRtpSendParam->m_szCameraID);
            			sleep(1);
            		}
            		else
            		{
						iFailConnectExepLogOuct=0;
                        LOGOUT("%s  %d %s T_Working",ptrRtpSendParam->m_szRemoteIP,ptrRtpSendParam->m_iRemotePort,ptrRtpSendParam->m_szCameraID);
                        RtpSendSendLoginData(ptrRtpSendParam->m_iSocketID,(const char *)ptrRtpSendParam->m_szCameraID);
                        dwLastReciceActiveTime = getTickCountMs();
                        byLastSta = T_Conn;
                		bytSta = T_Working;
            			break;
            		}
            		if(iConnFailCnt > 2) // 连续三次连接不上就断开重新连接
            		{
                        iConnFailCnt = 0;
                        byLastSta = T_Conn;
            			bytSta = T_DisConn;
            		}
					if(iFailConnectExepLogOuct > 10) // 连续三次连接不上就断开重新连接
            		{
                        iFailConnectExepLogOuct = 0; 
            		}
            	}
            	break;


            case T_Working:
            	{
 
            		if( PACKET_CMD == bPacketValid)
            		{
                        pMsgHeader = (struct TMSG_HEADER *)szResolveptr;
                        switch (pMsgHeader->cMsgID)
                    	{    
                    	case MSG_SENDSWITCH:
                    		{
                    			pMsgSwitch = (struct TMSG_SENDSWITCH*)pMsgHeader;
                            	if(pMsgSwitch->m_nSwitch)
                            	{
									LOGOUT("%s rtpSend MSG_SENDSWITCH TRUE:%d",ptrRtpSendParam->m_szCameraID,ptrRtpSendParam->m_iChannel);
                            	    ptrRtpSendParam->m_bOpenVideo = TRUE; 
                            	    //zss++g_SwitchVideoSendFun(ptrRtpSendParam->m_iChannel, szyTrue);
                            	}
                            	else
                            	{
									LOGOUT("%s rtpSend MSG_SENDSWITCH FALSE:%d",ptrRtpSendParam->m_szCameraID,ptrRtpSendParam->m_iChannel);
                            	    ptrRtpSendParam->m_bOpenVideo = FALSE; 
									//zss++if(GetAvliableChannel(ptrRtpSendParam->m_iChannel) == FALSE)
									{
										//zss++g_SwitchVideoSendFun(ptrRtpSendParam->m_iChannel, szyFalse);
									}
                            	}
                    		}
                    		break;
                    	case MSG_LOGINRET:
                    		{
                                //LOGOUT("%s rtpSend MSG_LOGINRET",ptrRtpSendParam->m_szCameraID);
                    			ProcLoginRetMSG(pMsgHeader,ptrRtpSendParam->m_iSocketID,(const char *)ptrRtpSendParam->m_szCameraID);      
                                
                    		}
                    		break;
                    	case MSG_KEEPALIVE:
                    		{
                                ikeepAliveCount++;
                                if(ikeepAliveCount>20)
                                {   
                                    ikeepAliveCount = 0;
                                    LOGOUT("IP:%s PORT:%d %s MSG_KEEPALIVE",ptrRtpSendParam->m_szRemoteIP,ptrRtpSendParam->m_iRemotePort,ptrRtpSendParam->m_szCameraID);
								}
								sendErrorTimes=0;
                    			dwLastReciceActiveTime = getTickCountMs();
                    		}
                    		break;
                    	case MSG_PTZCOMMAND:
                    		{
                                 LOGOUT("%s rtpSend MSG_PTZCOMMAND",ptrRtpSendParam->m_szCameraID);
                    			 ProcPTZCommand(pMsgHeader,ptrRtpSendParam,&bNoAckStop);
                    		}
                    		break;
                    	case MSG_PARAMCONFIG:
                    		{
                                LOGOUT("%s MSG_PARAMCONFIG",ptrRtpSendParam->m_szCameraID);
                    			ProcParamConfig(pMsgHeader,ptrRtpSendParam);
                    		}
                    		break;
                    	case MSG_GETPARAMCONFIG:
                    		{
                                LOGOUT("%s rtpSend MSG_GETPARAMCONFIG",ptrRtpSendParam->m_szCameraID);
                    			ProcGetParamConfig(ptrRtpSendParam);
                    		}
                    		break;
                    	case MSG_AUDIODATAGSM:
                    	case MSG_AUDIODATA:
                    		{
                                LOGOUT("%s rtpSend MSG_AUDIODATAGSM",ptrRtpSendParam->m_szCameraID);
                    		}
                    		break;
                    	case MSG_ViewOutTalk:
                    		{
                                LOGOUT("%s rtpSend MSG_ViewOutTalk",ptrRtpSendParam->m_szCameraID); 
                                dwSleep = 100;
                    			bRecAudio = FALSE;
                    		}
                    		break;
                    	case MSG_ViewTalk:
                    		{
                                LOGOUT("%s rtpSend MSG_ViewTalk",ptrRtpSendParam->m_szCameraID); 
                                dwSleep = 25;
                    			bRecAudio = TRUE;
                    			
                    		}
                    		break;
                        default:
                            break;
                    	}
            		}
            		else if(PACKET_AUDIO == bPacketValid)
            		{
                        if(bRecAudio)
                            AnalyzeAudioData(szResolveptr+5,pMsgHeader->m_iXmlLen-5);
                        
                    }
            		// 等待WAITACTIVETIME秒时间后没有收到心跳或者登陆信息
            		if(getTickCountMs() - dwLastReciceActiveTime > RTPSENDWAITACTIVETIME*1000)
            		{	
                        LOGOUT("%s RTPSENDWAITACTIVETIME",ptrRtpSendParam->m_szCameraID); 
						sendErrorTimes++;
						dwLastSendActiveTime = getTickCountMs();
						dwLastReciceActiveTime = getTickCountMs();
						if(sendErrorTimes>=2)
						{
							byLastSta = T_Working;
	            			bytSta = T_DisConn;
						}
            			break;
            		}
            		if(getTickCountMs() - dwLastSendActiveTime > ((RTPSENDWAITACTIVETIME-5)*1000))
            		{
                        //LOGOUT("RTPSENDWAITACTIVETIME-5 "); 
                        dwLastSendActiveTime = getTickCountMs();
                        RtpSendSendKeepAlive(ptrRtpSendParam->m_iSocketID,ptrRtpSendParam->m_szCameraID);
            		}
            		
            	}
            	break;
            case T_DisConn:
                {
    				byLastSta = T_DisConn;
    				bytSta = T_Idle;
    				if(ptrRtpSendParam->m_iSocketID != -1)
    				{
    					DelSocketId(ptrRtpSendParam->m_iSocketID);
    					ptrRtpSendParam->m_iSocketID = -1;
    				}
    		    }
        	break;
            default:
            	break;
        }
        if ((!(byLastSta == T_DisConn && bytSta == T_Idle)) 
        ||  (!(byLastSta == T_Conn && bytSta == T_Working)) 
        || (!(bNextCmd == TRUE)))
            usleep(dwSleep*1000);
        ptrRtpSendParam->m_workThread=WORKING;
	}
	LOGOUT("Exit RtpSendWork");
	return NULL;
}

/************************************************************************
**函数：Init_rtpSend
**功能：控制连接初始化路口
**备注：
1). 
************************************************************************/
INT32S Init_Rtpsend(RtpSendInfo *param)
{
    int iRet = 0;
	bExitRtpSendThread = FALSE;
	pthread_t *ptrrtpsendThreadWork;
    INT32U i=0;
    INT32S iMsgId;
    LOGOUT("init_Rtpsend----------------start---------!!");
    ptrrtpsendThreadWork = (pthread_t *)malloc(sizeof(pthread_t)*param->m_iTotalChn);

    for(i=0; i<param->m_iTotalChn; i++)
    {
    	iRet = pthread_create((pthread_t *)&ptrrtpsendThreadWork[i], NULL, 
    	P_RtpsendThread, (void *)(&param->m_stRtpSendCameraInfo[i]));
    	if(iRet)
    	{
    		LOGOUT("Create P_CtrlSocketThread Fail!!");
    		return -2;
    	}
    	pthread_detach(ptrrtpsendThreadWork[i]);
	}
	if(ptrrtpsendThreadWork != NULL)
	{
		free(ptrrtpsendThreadWork);
	}
    LOGOUT("init_Rtpsend----------------over---------!!");
	return 0;

}

// 退出转发线程
void Release_Rtpsend(RtpSendInfo *param)
{
    INT32U i=0;
    bExitRtpSendThread = TRUE;
    for(i=0; i<param->m_iTotalChn; i++)
    {
    	if(param->m_stRtpSendCameraInfo[i].m_iSocketID != -1)
    	{
    		DelSocketId(param->m_stRtpSendCameraInfo[i].m_iSocketID);
    	}
	}	
}

