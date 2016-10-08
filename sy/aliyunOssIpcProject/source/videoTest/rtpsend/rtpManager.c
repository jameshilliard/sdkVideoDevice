#include "rtpManager.h"

// �������ָ��
Queue *g_queueCache = NULL;

// ת���Ϳ��ƹ��ýṹ��ָ��
RtpSendInfo *g_p_objRtpSendInfo;

// ���͵���Ƶ���� 
BYTE g_bytSendVideoBuf[SIZE_VIDEO_PACKED] = {0};
pthread_mutex_t m_RtpSendMutex;

// ���͵���Ƶ����
PACK_AUDIO2 g_objPackAudio;

// ���͵Ļط���Ƶ���� 
BYTE g_bytSendReplayVideoBuf[SIZE_VIDEO_PACKED] = {0};
pthread_mutex_t m_ReplaySendMutex;

// �طŹ�������ָ��
ReplayMsgInfo *g_p_objReplayMsgIfo = NULL;


// �˳��̱߳�ʶ
BOOL g_bExitRtpManager = FALSE;

// CPUʹ����
int g_iCpuUtilization = 0;

int g_bytCtrlThreadSta = WORKING;	// �����߳�״̬,0��������1����


int switchVideoSize(int v_with, int v_heigth)
{
	int i = 0;
	int switchVSize[][3] = {
		{160,120,0},			    //DQVGA = 0,
		{176,144,1},			    //QCIF = 1,
		{320,240,2},			    //QVGA = 2,
		{352,288,3},			    //CIF	= 3,
		{640,480,4},			    //VGA  = 4,
		{720,288,5},			    //HD1  = 5,
		{720,576,6},			    //D1 = 6,
		{704,576,7},			    //FOURCIF = 7,
		{704,288,8},				//BCIF = 8,
		{352,576,9},				//HALFD = 9,
		{800,600,10},				//SVGA = 10,
		{1024,768,11},				//XGA = 11 ,
		{1280,720,12},				//720P = 12,
		{1600,1200,13},				//UXGA = 13,
		{1920,1080,14},				//1080P = 14
		{432,240,15},				//p432 = 15
	};

	for(i = 0; i< 16; i++)
	{
		if( (switchVSize[i][0] == v_with) && (switchVSize[i][1] == v_heigth))
		{
			return switchVSize[i][2];
		}
	}
	//���û���ҵ���Ӧ���Ĭ�Ϸ���4
	return 4;
}

// RtpManager�����߳�
void *P_RtpManagerThread()
{
	DWORD dwCpuGetTickCount = getTickCountMs();
	DWORD dwRateGetTickCount = getTickCountMs();
	DWORD dwCpuTime = 0;
	DWORD dwRateTime = 0;
	int i = 0;
	int iCtrlCheckCount = 0;
	int iReplayCheckCount[MAXREPLAYNUM] = {0};
	char errorLog[256]={0};
	while(!g_bExitRtpManager)
	{
		sleep(1);
		dwCpuTime = getTickCountMs() - dwCpuGetTickCount;
		if( dwCpuTime > 4*1000)
		{
			g_iCpuUtilization = get_CPU_Utilization();
			dwCpuGetTickCount = getTickCountMs();
		}
		dwRateTime = getTickCountMs() - dwRateGetTickCount;
		if(dwRateTime > 1000)
		{
			for (i = 0; i< g_p_objRtpSendInfo->m_iTotalChn; i++)
			{
				g_p_objRtpSendInfo->m_stRtpSendCameraInfo[i].m_iNowRate = (g_p_objRtpSendInfo->m_stRtpSendCameraInfo[i].m_iTotalSendDataLen*1000)/(dwRateTime*1024);
				g_p_objRtpSendInfo->m_stRtpSendCameraInfo[i].m_iTotalSendDataLen = 0;
			}
			
			dwRateGetTickCount = getTickCountMs();
		}

		if(g_bytCtrlThreadSta)
		{
			g_bytCtrlThreadSta = NO_WORKING;
			iCtrlCheckCount = 0;
		}
		else
		{
			if(iCtrlCheckCount ++ > 60)
			{
				iCtrlCheckCount = 0;
			}
		}
		
		for (i = 0; i< g_p_objRtpSendInfo->m_iTotalChn; i++)
		{
			if(g_p_objRtpSendInfo->m_stRtpSendCameraInfo[i].m_workThread)
			{
				g_p_objRtpSendInfo->m_stRtpSendCameraInfo[i].m_workThread = NO_WORKING;
				g_p_objRtpSendInfo->m_stRtpSendCameraInfo[i].m_iRtpSendCheckCount=0;
			}
			else
			{
				
				if(g_p_objRtpSendInfo->m_stRtpSendCameraInfo[i].m_iRtpSendCheckCount++ > 60)
				{
				}
			}
			
		}
		if(NULL!=g_p_objReplayMsgIfo)
		{
			for (i = 0; i< MAXREPLAYNUM; i++)
			{
				if(g_p_objReplayMsgIfo[i].m_workThread)
				{
					g_p_objReplayMsgIfo[i].m_workThread =NO_WORKING;
					iReplayCheckCount[i]=0;
				}
				else
				{
					if(iReplayCheckCount[i]++ > 60)
					{
						sprintf(errorLog,"iProcessID %d",g_p_objReplayMsgIfo[i].m_iProcessID);
						iReplayCheckCount[i] = 0;
					}
				}

			}
		}
	}
	LOGOUT("Exit P_RtpManagerThread");
	return NULL;
}

/************************************************************************
**������RtpSendH264Data
**���ܣ�����ʵʱ��Ƶ����
**������
        [in] - v_stVideoDate����Ƶ���ݻ�����
        
**���ߣ��򺣲�, 2014-04-25
**��ע��
       1). 
************************************************************************/
void RtpSendH264Data(VIDEO_DATA *v_stVideoData)
{
	pthread_mutex_lock( &m_RtpSendMutex );

	static 	int DVR_ENC_QUEUE_GETCount = 0;

	if (v_stVideoData->len > SIZE_VIDEO_PACKED || (v_stVideoData->nChannel<0) ||  (v_stVideoData->nChannel > g_p_objRtpSendInfo->m_iTotalChn))
	{
		if (DVR_ENC_QUEUE_GETCount++ > 20)
		{
			DVR_ENC_QUEUE_GETCount = 0;
			LOGOUT("SIZE_VIDEO_PACKED Over----------%d---1, v_videoData->nChannel %d", v_stVideoData->len, v_stVideoData->nChannel);
		}
		goto _RTPSEND_END;
	}

	if((!g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_stVideoData->nChannel].m_bOpenVideo))	// ���ز��ź�Զ�̶�û���˹ۿ�����ô����
	{
		goto _RTPSEND_END;
	}

	int iNowRate = g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_stVideoData->nChannel].m_iNowRate;
	int iMsgID = g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_stVideoData->nChannel].m_iMsgID;
	int iSocketID = g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_stVideoData->nChannel].m_iSocketID;

	RTP_HDR_T sHeader;
	memset(&sHeader, 0 , sizeof(sHeader));

	int iSendLen = 0;
	sHeader.session = YRVIDEO;
	sHeader.ts = v_stVideoData->ts;
	sHeader.w = v_stVideoData->w;
	sHeader.h = v_stVideoData->h;
	sHeader.vs = switchVideoSize(v_stVideoData->w, v_stVideoData->h);
	sHeader.fr = v_stVideoData->fr;
	sHeader.k = v_stVideoData->bKey;
	sHeader.cpuUse = g_iCpuUtilization; 
	sHeader.NowUpRate = iNowRate?iNowRate:1;				// ��ǰ�ϴ�������;
	sHeader.bFilpH = 1;
	sHeader.bFlipUV = 1;
	sHeader.dataLen = v_stVideoData->len;

	sHeader.rotateFlag = 0;
	sHeader.bNewSizeFlag = 1;

	sHeader.nframeCount = g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_stVideoData->nChannel].m_iFrameCount++;
	if(v_stVideoData->bKey)
	{
		g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_stVideoData->nChannel].m_iFrameKeyCount++;
	}
	sHeader.keyCount = g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_stVideoData->nChannel].m_iFrameKeyCount;
	
	int iDataLen = sizeof(RTP_HDR_T)+ sHeader.dataLen + 2 + 9;
	memcpy(g_bytSendVideoBuf + iSendLen, &iDataLen, sizeof(iDataLen));// ���ݵĳ��ȣ�����+�������ݳ���+������
	iSendLen += sizeof(iDataLen);
	
	g_bytSendVideoBuf[iSendLen++] = 0x00;// ����

	memcpy(g_bytSendVideoBuf + iSendLen, &sHeader, sizeof(RTP_HDR_T));
	iSendLen += sizeof(RTP_HDR_T);

	memcpy(g_bytSendVideoBuf + iSendLen, v_stVideoData->data, v_stVideoData->len);// ��Ƶ����
	iSendLen += v_stVideoData->len;

	g_bytSendVideoBuf[iSendLen] = sHeader.vs;
	g_bytSendVideoBuf[iSendLen + 1] = sHeader.fr;
	g_bytSendVideoBuf[iSendLen + 2] = sHeader.k;
	iSendLen += 3;

	memcpy(g_bytSendVideoBuf + iSendLen, DATA_TOKEN, 6);
	iSendLen += 6;

	g_bytSendVideoBuf[iSendLen++] = 0x00;

	if((-1 != iSocketID) && (NULL != g_queueCache)
		&& g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_stVideoData->nChannel].m_bOpenVideo)// socket�����Ч����ô����
	{
		g_queueCache->pushData(g_queueCache->_this, (void *)g_bytSendVideoBuf, iSendLen, sHeader.k, iSocketID);
	}
	
_RTPSEND_END:
	pthread_mutex_unlock( &m_RtpSendMutex );
	
}

/************************************************************************
**������RtpSendAudioData
**���ܣ�������Ƶ����
**������
        [in] - v_pAudioData:��Ƶ����
**���ߣ��򺣲�, 2014-04-28
**��ע��
       1). 
************************************************************************/
void RtpSendAudioData(AUDIO_DATA* v_pAudioData)
{
	static 	int DVR_ENC_QUEUE_GETCount = 0;

	if ((v_pAudioData->len > SIZE_AUDIO_PACKED)  || (v_pAudioData->nChannel<0) 
		|| (v_pAudioData->nChannel > g_p_objRtpSendInfo->m_iTotalChn))//��Ƶ���Ĵ�С����
	{
		if (DVR_ENC_QUEUE_GETCount++ > 20)
		{
			DVR_ENC_QUEUE_GETCount = 0;
			LOGOUT("sendAudioData data error Over----------%d---1, v_videoData->nChannel %d", v_pAudioData->len, v_pAudioData->nChannel);
		}
		return;
	}

	if(!g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_pAudioData->nChannel].m_bOpenAudio)
	{
		return ;
	}

	if((!g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_pAudioData->nChannel].m_bOpenVideo))	// ���ز��ź�Զ�̶�û���˹ۿ�����ô����
	{
		return ;
	}

	int iMsgID = g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_pAudioData->nChannel].m_iMsgID;
	int iSocketID = g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_pAudioData->nChannel].m_iSocketID;
	
	BYTE bytSendAudioBuf[SIZE_AUDIO_PACKED_MP3 + 10];
	int iSendAudioLen = 0;

	memset(&g_objPackAudio, 0, sizeof(PACK_AUDIO2));
	g_objPackAudio.session = SZYAUDIO_EX;
	g_objPackAudio.header.nChannel = v_pAudioData->nChannel;
	g_objPackAudio.header.nSamplePerSec = v_pAudioData->nSamplePerSec;
	g_objPackAudio.header.wBitPerSample = v_pAudioData->wBitPerSample;
	g_objPackAudio.header.wFormat = v_pAudioData->wFormat;
	g_objPackAudio.datalen = v_pAudioData->len;
	g_objPackAudio.ts = v_pAudioData->ts;
	g_objPackAudio.nFirstPacketFlag = 0;
	g_objPackAudio.nPacketFlag = 0;
	memcpy(g_objPackAudio.pdata, v_pAudioData->pdata, v_pAudioData->len);

	int iLen = 20 + v_pAudioData->len + 2;// ��ͷ20���ֽ� + ��Ƶ���ݳ��� + ���͵��������� + ������
	memcpy(bytSendAudioBuf + iSendAudioLen, &iLen, sizeof(iLen));
	iSendAudioLen += sizeof(iLen);

	bytSendAudioBuf[iSendAudioLen++] = 0x00;

	memcpy(bytSendAudioBuf + iSendAudioLen, &g_objPackAudio, 20 + v_pAudioData->len);
	iSendAudioLen += (20 + v_pAudioData->len);

	bytSendAudioBuf[iSendAudioLen++] = 0x00;// ������
	
	if(( -1 != iSocketID) && (NULL != g_queueCache)
		&& g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_pAudioData->nChannel].m_bOpenVideo)
	{
		g_queueCache->pushData(g_queueCache->_this, (void *)bytSendAudioBuf, iSendAudioLen, 3, iSocketID);
	}
	
}


/************************************************************************
**������SendReplayVideoData
**���ܣ����ͻط���Ƶ����
**������
        [in] - v_pVideoData���ط���Ƶ����
			   v_iProcessId: �طŵĹ���ID
**���أ�
        
**���ߣ��򺣲�, 2014-05-04
**��ע��
       1). 
************************************************************************/
void SendReplayVideoData(VIDEO_DATA *v_pVideoData, int v_iProcessId)
{
	pthread_mutex_lock( &m_ReplaySendMutex );
	if(NULL == g_queueCache)
	{
		LOGOUT("g_queueCache is NULL!!");
		goto _Replay_SEND_END ;
	}

	if (v_pVideoData->len > SIZE_VIDEO_PACKED || (v_pVideoData->nChannel< 0) 
		||  (v_pVideoData->nChannel > g_p_objRtpSendInfo->m_iTotalChn))
	{
		goto _Replay_SEND_END ;
	}
	
	int i = 0;
	for (i = 0; i< MAXREPLAYNUM; i++)
	{
		if(g_p_objReplayMsgIfo[i].m_iProcessID == v_iProcessId)
		{
			break;
		}

		if( i == (MAXREPLAYNUM - 1))// û���ҵ�
		{
			goto _Replay_SEND_END ;
		}
	}
	int iMsgID = g_p_objReplayMsgIfo[i].m_iMsgID;
	int iSocketID = g_p_objReplayMsgIfo[i].m_iSocketID;

	if(-1 == iSocketID)// socket�����Ч����ô����
	{
		goto _Replay_SEND_END ;
	}


	RTP_HDR_T sHeader;
	memset(&sHeader, 0 , sizeof(sHeader));

	int iSendLen = 0;
	sHeader.session = YRVIDEO;
	sHeader.ts = v_pVideoData->ts;
	sHeader.w = v_pVideoData->w;
	sHeader.h = v_pVideoData->h;
	sHeader.vs = switchVideoSize(v_pVideoData->w, v_pVideoData->h);
	sHeader.fr = v_pVideoData->fr;
	sHeader.k = v_pVideoData->bKey;
	sHeader.cpuUse = g_iCpuUtilization; 
	//sHeader.NowUpRate = iNowRate?iNowRate:1;				// ��ǰ�ϴ�������;
	sHeader.bFilpH = 1;
	sHeader.bFlipUV = 1;
	sHeader.dataLen = v_pVideoData->len;

	sHeader.rotateFlag = 0;
	sHeader.bNewSizeFlag = 1;

	sHeader.nframeCount = g_p_objReplayMsgIfo[i].m_iReFrameCnt++;
	if(v_pVideoData->bKey)
	{
		g_p_objReplayMsgIfo[i].m_iReKeyFrameCnt++;
	}
	sHeader.keyCount = g_p_objReplayMsgIfo[i].m_iReKeyFrameCnt;

	int iDataLen = sizeof(RTP_HDR_T)+ sHeader.dataLen + 2 + 9;
	memcpy(g_bytSendReplayVideoBuf + iSendLen, &iDataLen, sizeof(iDataLen));// ���ݵĳ��ȣ�����+�������ݳ���+������
	iSendLen += sizeof(iDataLen);

	g_bytSendReplayVideoBuf[iSendLen++] = 0x00;// ����

	memcpy(g_bytSendReplayVideoBuf + iSendLen, &sHeader, sizeof(RTP_HDR_T));
	iSendLen += sizeof(RTP_HDR_T);

	memcpy(g_bytSendReplayVideoBuf + iSendLen, v_pVideoData->data, v_pVideoData->len);// ��Ƶ����
	iSendLen += v_pVideoData->len;

	g_bytSendReplayVideoBuf[iSendLen] = sHeader.vs;
	g_bytSendReplayVideoBuf[iSendLen + 1] = sHeader.fr;
	g_bytSendReplayVideoBuf[iSendLen + 2] = sHeader.k;
	iSendLen += 3;

	memcpy(g_bytSendReplayVideoBuf + iSendLen, DATA_TOKEN, 6);
	iSendLen += 6;

	g_bytSendReplayVideoBuf[iSendLen++] = 0x00;

	g_queueCache->pushData(g_queueCache->_this, (void *)g_bytSendReplayVideoBuf, iSendLen, sHeader.k, iSocketID);

    //SendFileNum(&g_p_objReplayMsgIfo[i], v_pVideoData->ts,iSocketID); //���ͽ�����
    
_Replay_SEND_END:
	pthread_mutex_unlock( &m_ReplaySendMutex );
}

/************************************************************************
**������SendReplayAudioData
**���ܣ����ͻط���Ƶ����
**��ע��
       1). 
************************************************************************/
void SendReplayAudioData(AUDIO_DATA *v_pAudioData, int v_iProcessId)
{
	if(NULL == g_queueCache)
	{
		LOGOUT("g_queueCache is NULL!!");
		return ;
	}

	if (v_pAudioData->len > SIZE_AUDIO_PACKED_MP3 || (v_pAudioData->nChannel< 0) 
		|| (v_pAudioData->nChannel > g_p_objRtpSendInfo->m_iTotalChn))
	{
		return;
	}

	int i = 0;
	for (i = 0; i< MAXREPLAYNUM; i++)
	{
		if(g_p_objReplayMsgIfo[i].m_iProcessID == v_iProcessId)
		{
			break;
		}

		if( i == (MAXREPLAYNUM - 1))// û���ҵ�
		{
			return ;
		}
	}
	int iMsgID = g_p_objReplayMsgIfo[i].m_iMsgID;
	int iSocketID = g_p_objReplayMsgIfo[i].m_iSocketID;

	if(-1 == iSocketID)// socket�����Ч����ô����
	{
		return ;
	}

	BYTE bytSendAudioBuf[SIZE_AUDIO_PACKED_MP3 + 10];
	int iSendAudioLen = 0;
	if(g_p_objReplayMsgIfo[i].m_iFsType  == 1)
	{
		PACK_AUDIO2 objPackAudio = {0};
		memset(&objPackAudio, 0, sizeof(PACK_AUDIO2));
		objPackAudio.session = SZYAUDIO_EX;
		objPackAudio.header.nChannel = v_pAudioData->nChannel;
		objPackAudio.header.nSamplePerSec = v_pAudioData->nSamplePerSec;
		objPackAudio.header.wBitPerSample = v_pAudioData->wBitPerSample;
		objPackAudio.header.wFormat = v_pAudioData->wFormat;
		objPackAudio.datalen = v_pAudioData->len;
		objPackAudio.ts = v_pAudioData->ts;
		objPackAudio.nFirstPacketFlag = 0;
		objPackAudio.nPacketFlag = 0;
		memcpy(objPackAudio.pdata, v_pAudioData->pdata, v_pAudioData->len);

		int iLen = 20 + v_pAudioData->len + 2;// ��ͷ20���ֽ� + ����Ƶ���ݳ��� + ���͵��������� + ������
		memcpy(bytSendAudioBuf + iSendAudioLen, &iLen, sizeof(iLen));
		iSendAudioLen += sizeof(iLen);

		bytSendAudioBuf[iSendAudioLen++] = 0x00;

		memcpy(bytSendAudioBuf + iSendAudioLen, &objPackAudio, 20 + v_pAudioData->len);
		iSendAudioLen += (20 + v_pAudioData->len);

		bytSendAudioBuf[iSendAudioLen++] = 0x00;// ������
	}

	if(g_p_objReplayMsgIfo[i].m_iFsType  == 0)
	{
		PACK_AUDIO1 objOldAudioPack = {0};
		memset(&objOldAudioPack, 0, sizeof(PACK_AUDIO1));
		objOldAudioPack.session = SZYAUDIO_EX;
		objOldAudioPack.header.nChannel = v_pAudioData->nChannel;
		objOldAudioPack.header.nSamplePerSec = v_pAudioData->nSamplePerSec;
		objOldAudioPack.header.wBitPerSample = v_pAudioData->wBitPerSample;
		objOldAudioPack.header.wFormat = v_pAudioData->wFormat;
		objOldAudioPack.ts = v_pAudioData->ts;
		memcpy(objOldAudioPack.data, v_pAudioData->pdata, v_pAudioData->len);

		int iLen = 12 + v_pAudioData->len + 2;// ��ͷ12���ֽ� + ����Ƶ���ݳ��� + ���͵��������� + ������
		memcpy(bytSendAudioBuf + iSendAudioLen, &iLen, sizeof(iLen));
		iSendAudioLen += sizeof(iLen);

		bytSendAudioBuf[iSendAudioLen++] = 0x00;

		memcpy(bytSendAudioBuf + iSendAudioLen, &objOldAudioPack, 12 + v_pAudioData->len);
		iSendAudioLen += (12 + v_pAudioData->len);

		bytSendAudioBuf[iSendAudioLen++] = 0x00;// ������
	}


	g_queueCache->pushData(g_queueCache->_this, (void *)bytSendAudioBuf, iSendAudioLen, 3, iSocketID);

}

/************************************************************************
**������Init_RtpManager
**���ܣ���ʼ����Ƶ���͹���ģ��
**������
        [in] - v_rtpSendInfo
**���ߣ��򺣲�, 2014-04-25
**��ע��
       1). 
************************************************************************/
void Init_RtpManager(RtpSendInfo *v_rtpSendInfo, ReplayMsgInfo *v_stReplayMsgIfo)
{
	if((NULL == v_rtpSendInfo) || (NULL == v_stReplayMsgIfo))
	{
		LOGOUT("Init_RtpManager Fail!!");
		return ;
	}
	g_p_objRtpSendInfo = v_rtpSendInfo;

	g_queueCache = GetCache();
	
	g_bExitRtpManager = FALSE;
	g_iCpuUtilization = 0;
	
	int iRet;
	pthread_t pRtpManagerThreadWork;
	iRet = pthread_create(&pRtpManagerThreadWork, NULL, P_RtpManagerThread, NULL);
	if(iRet)
	{
		LOGOUT("Create P_CtrlSocketThread Fail!!\n");
		return ;
	}

	pthread_detach(pRtpManagerThreadWork);

	g_p_objReplayMsgIfo = v_stReplayMsgIfo;
	pthread_mutex_init(&m_RtpSendMutex, NULL);
	pthread_mutex_init(&m_ReplaySendMutex, NULL);
}

//�޸�����ͷ��Ϣ
void ModifyVideoParam(int v_iChannel, int v_iBitRate, int v_iFrameRate, int v_iQpConstant, int v_iKeyFrameRate, int v_iWith, int v_iHeight)
{
	g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_iChannel].m_bOpenVideo = FALSE;
	//zss++g_ModfieVidioParam(v_iChannel, v_iBitRate, v_iFrameRate, v_iQpConstant, v_iKeyFrameRate, v_iWith, v_iHeight);
	g_p_objRtpSendInfo->m_stRtpSendCameraInfo[v_iChannel].m_bOpenVideo = TRUE;
}

// �޸Ŀ����̵߳�״̬
void SetCtrlThreadStatus(int v_iSta)
{
	g_bytCtrlThreadSta = v_iSta;
}
// �ͷ�
void Release_RtpManager()
{
	g_bExitRtpManager = TRUE;
	pthread_mutex_destroy(&m_RtpSendMutex);
	pthread_mutex_destroy(&m_ReplaySendMutex);
}


// ת���Ϳ��ƹ��õ�ȫ�ֽṹ��
RtpSendInfo g_stRtpSendInfo;
// ¼��طŲ�����Ϣ�ṹ��
ReplayMsgInfo g_stReplayMsgInfo[MAXREPLAYNUM];


void Init_RtpSendTask(void)
{
	int iCacheSize=-1;
	int i = 0;
	g_stRtpSendInfo.m_iTotalChn = 1;
	g_stRtpSendInfo.m_stRtpSendCameraInfo = (RtpSendCameraInfo *)malloc(sizeof(RtpSendCameraInfo)*g_stRtpSendInfo.m_iTotalChn + 100);
	memset(g_stRtpSendInfo.m_stRtpSendCameraInfo, 0, sizeof(RtpSendCameraInfo)*g_stRtpSendInfo.m_iTotalChn + 100);
	for (i = 0; i< g_stRtpSendInfo.m_iTotalChn;i++)
	{
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_iChannel = i;
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_iMsgID = -1;
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_iSocketID = -1;
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_bOpenVideo = FALSE;
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_iFrameCount = 1;
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_iFrameKeyCount = 1;
		tagCapParamCfg objCapParamCfg;
		memset(&objCapParamCfg, 0, sizeof(tagCapParamCfg));
		GetCfgFile(&objCapParamCfg, sizeof(tagCapParamCfg), offsetof(tagConfigCfg, m_unCapParamCfg.m_objCapParamCfg[i]), sizeof(tagCapParamCfg));
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_bNetAdaptive = objCapParamCfg.m_bAdaptStream;
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_bOpenAudio = objCapParamCfg.m_bOpenAudio;
		g_stRtpSendInfo.m_stRtpSendCameraInfo[i].m_bAlarmSwitch = objCapParamCfg.m_bAlarmSwitch;
	}
	if(g_stRtpSendInfo.m_iTotalChn <= 4)
	{
		iCacheSize = 1;
	}
	else if((g_stRtpSendInfo.m_iTotalChn > 4) && (g_stRtpSendInfo.m_iTotalChn <= 8))
	{
		iCacheSize = 4;
	}
	else if((g_stRtpSendInfo.m_iTotalChn > 8) && (g_stRtpSendInfo.m_iTotalChn <= 16))
	{
		iCacheSize = 6;
	}
	else
	{
		iCacheSize = 8;
	}
	// ��ʼ���ײ�����ģ��
	InitNetwork(iCacheSize*256*4096);
	// ��ʼ��ת������
	Init_RtpManager(&g_stRtpSendInfo, g_stReplayMsgInfo);
	// ��ʼ��ת��
	Init_Rtpsend(&g_stRtpSendInfo);

}


void Release_RtpSendTask(void)
{

	// �ͷ�����ģ��
	ReleaseNetwork();
}

