#include "Queue.h"

Queue *g_Queue = NULL;

// C���캯��
Queue *QueueListConstruction()
{
	g_Queue = (Queue*)malloc(sizeof(Queue));
	g_Queue->initQueue = InitQueue;
	g_Queue->pushData = DataPush;
	g_Queue->popData = DataPop;
	g_Queue->delData = DataDel;
	g_Queue->clearData= DataClear;
	g_Queue->getAvliableSize = GetAvliableSize;
	g_Queue->release = ReleaseQueue;
	g_Queue->_this = g_Queue;
	return (Queue*)g_Queue;
}

/************************************************************************
**������InitQueue
**���ܣ���ʼ���������
**������
        [in] - v_pQueue�����е�thisָ��
		   v_kShm����������еı�ʶ
		   v_ulQueueSize:������д�С
		   v_iMsgId����Դ���ݵ���Ϣ����ID
		   v_iSocketSymb����Դ���ݵ�SoceketID
**���أ�0���ɹ���������ʧ�� 
************************************************************************/
int InitQueue(void *v_pQueue, unsigned long v_ulQueueSize )
{
	if(v_pQueue == NULL)
	{
		return ERR_PAR;
	}
	if(0 == v_ulQueueSize )
	{
		return ERR_PAR;
	}
	Queue *_this = (Queue *)v_pQueue;
	_this->m_ulMemSize = 0;
	_this->m_iShmID = -1;
	_this->m_pShareMem = NULL;

	int iRet = 0;
	void* pShare = NULL;
	BOOL bFirstCreate = FALSE;
	pShare = malloc(v_ulQueueSize);
	if( NULL == pShare )
	{
		iRet = ERR_SHMAT;
		_this->m_iShmID = -1;
		LOGOUT( "shmat fail" );
		goto INIT_END;
	}
	else
	{
		bFirstCreate = TRUE;
		_this->m_iShmID = 1;
		_this->m_pShareMem = (char*)pShare;
		LOGOUT( "shmat succ" ); 
	}

	if( bFirstCreate )
	{
		memset( _this->m_pShareMem, 0, v_ulQueueSize );
	}

	_this->m_ulMemSize = v_ulQueueSize;
	pthread_mutex_init(&_this->m_DataMutex, NULL);

INIT_END:
	return iRet;
}

/************************************************************************
**������DataPop
**���ܣ�������������
**������
        [in] - v_pQueue�����е�thisָ��
			   v_pPopData���������ݵĻ�����ָ��
			   v_dwPopSize��������ݵĻ�������С
        [out] - v_p_dwPopLen���������ݵĳ���
		        v_p_dwPushSymb���������ݵı�ʶ
				v_p_iMsgId����Դ���ݵ���Ϣ����ID
				v_p_iSocketSymb����Դ���ݵ�SoceketID

**���أ�0���ɹ���������ʧ�� 
**��ע:
	1).ÿ������ֻ��һ������DataPop
************************************************************************/
int DataPop(void *v_pQueue, void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb, int *v_p_iSocketSymb) 
{
	if(v_pQueue == NULL)
	{
		return ERR_PAR;
	}

	Queue *_this = (Queue *)v_pQueue;
	if( !_this->m_pShareMem || 0 == _this->m_ulMemSize || -1 == _this->m_iShmID ) return ERR_MEMLACK;

	if( NULL == v_pPopData || NULL == v_p_dwPopLen || NULL == v_p_dwPushSymb  || NULL == v_p_iSocketSymb)	return ERR_PAR;

	int iRet = ERR_QUEUE_EMPTY;
	INT32U sizeofAddr=sizeof(int *);
	pthread_mutex_lock( &_this->m_DataMutex );

	PQUEUEHEAD pHead = * (PQUEUEHEAD*) (_this->m_pShareMem); // ��Ե�ַ
	if( pHead ) pHead = (PQUEUEHEAD)( (DWORD)pHead + _this->m_pShareMem ); // ���Ե�ַ

	DWORD dwCur = getTickCountMs();
	BOOL bEnd = FALSE;
	do 
	{
		// ���Ե�ַ
		if( !pHead )
		{
			break;
		}

		if( dwCur - pHead->m_dwPushTime <= QUEUEVALID_PERIOD && pHead->m_bValid ) // ������δ��ʱ����Ч
		{
			if( pHead->m_dwDataLen > v_dwPopSize )
			{
				iRet = ERR_SPACELACK;
				bEnd = TRUE;
				goto _POP_ONELOOP_END;
			}

			memcpy( v_pPopData, (char*)pHead + sizeof(QUEUEHEAD), pHead->m_dwDataLen );
			*v_p_dwPopLen = pHead->m_dwDataLen;
			*v_p_dwPushSymb = pHead->m_dwPushSymb;
			*v_p_iSocketSymb = pHead->m_iSocketSymb;

			bEnd = TRUE;
			iRet = 0;
			goto _POP_ONELOOP_END;
		}
		else
		{
			LOGOUT("DataPop: Release overdue or invalid data" );
		}

_POP_ONELOOP_END:
		pHead = pHead->m_pNext; // �޸�ͷָ�� (�õ�������Ե�ַ)
		if( pHead )
		{
			* (PQUEUEHEAD*) (_this->m_pShareMem) = (PQUEUEHEAD)((char*)pHead) ; // ������ָ��, ���������Ե�ַ
			pHead = (PQUEUEHEAD)((DWORD)pHead + _this->m_pShareMem); // ת��Ϊ���Ե�ַ������ʹ��
		}
		else
		{
			// ����ס�βָ��
			* (PQUEUEHEAD*) (_this->m_pShareMem) = NULL;
			* (PQUEUEHEAD*) (_this->m_pShareMem + sizeofAddr) = NULL;
		}
	} while( !bEnd );

	pthread_mutex_unlock( &_this->m_DataMutex );

	return iRet;
}

/************************************************************************
**������DataPush
**���ܣ�����в�������
**������
        [in] - v_pQueue�����е�thisָ��
			   v_pPushData���������ݵ����ݻ�����
			   v_dwPushLen���������ݵĳ���
			   v_dwPushSymb���������ݵı�ʶ
			   v_iMsgId:��ϢID
			   v_iSocketSymb��SocketID
        [out] - null
**���أ�0���ɹ���������ʧ��  
************************************************************************/
int DataPush(void *v_pQueue, void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, int v_iSocketSymb) 
{
	if(v_pQueue == NULL)
	{
		return ERR_PAR;
	}
	Queue *_this = (Queue *)v_pQueue;
	int iRet = 0;

	if( !_this->m_pShareMem || 0 == _this->m_ulMemSize || -1 == _this-> m_iShmID ) return ERR_MEMLACK;
	if( 0 == v_dwPushLen ) return ERR_PAR;

	INT32U sizeofAddr=sizeof(int *);
	pthread_mutex_lock( &_this->m_DataMutex );

	PQUEUEHEAD pHead = NULL, pTail = NULL;
	PQUEUEHEAD pNew = NULL, pNext = NULL;
	
	// ��ʱ�õ��Ľ�����Ե�ַ
	pHead = * (PQUEUEHEAD*) (_this->m_pShareMem);
	pTail = * (PQUEUEHEAD*) (_this->m_pShareMem + sizeofAddr);

	// ��ʱ�õ��Ĳ��Ǿ��Ե�ַ
	if( pHead ) pHead = (PQUEUEHEAD)( (DWORD)pHead + _this->m_pShareMem );
	if( pTail ) pTail = (PQUEUEHEAD)( (DWORD)pTail + _this->m_pShareMem );

	DWORD dwCirFreeTotal = 0; // ���η���ʱ�ͷſռ���ۻ�
	DWORD dwCur = getTickCountMs();

	if( pHead ) // ����ʱΪ�ǿն���
	{
		if( !pTail )
		{
			LOGOUT("DataPush: Head Ptr Vaild but Tail Ptr NULL!" );
			return ERR_QUEUE;
		}

		// ���������������ʱӦ��ʹ�õ���ʼ��ַ
		pNew = (PQUEUEHEAD)( (char*)pTail + sizeof(QUEUEHEAD) + pTail->m_dwDataLen ); // �·����ַ,���Ե�ַ
		pNew = (PQUEUEHEAD)( (char*)pNew + ( (DWORD)pNew % sizeofAddr ? sizeofAddr - (DWORD)pNew % sizeofAddr : 0 ) ); // ȷ���ڴ����

		if( pTail >= pHead ) // ��������˳����� (�����˶��д�ʱֻ��һ��Ԫ�ص������)
		{
			goto _QUESUN_DEAL;
		}
		else
		{
			goto _QUECIR_DEAL;
		}
	}
	else // ˵����ʱ����Ϊ��
	{
		if( pTail )
		{
			LOGOUT("DataPush: Head Ptr NULL but Tail Ptr Invalid!" );
			return ERR_QUEUE;
		}

		goto _QUEEMPTY_DEAL;
	}

_QUESUN_DEAL: // ˳�����
	if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= _this->m_pShareMem + _this->m_ulMemSize ) // ����������󲻻ᳬ���ڴ���Top�߽�
	{
		goto _DATAPUSH_END;
	}
	else
	{
		pNew = (PQUEUEHEAD)( _this->m_pShareMem + sizeofAddr*2 ); 
		//PRTMSG( MSG_DBG, "4\n");
		goto _QUECIR_DEAL;
	}

_QUECIR_DEAL: // ���η���
	dwCirFreeTotal = 0;
	while( TRUE )
	{
		// �����ж�Headָ��
		if( (char*)pHead > _this->m_pShareMem + _this->m_ulMemSize )
		{
			iRet = ERR_QUEUE;
			LOGOUT("DataPush: Head Ptr Over Top Side" );
			goto _DATAPUSH_END;
		}

		// �����ж��ۻ��ͷſռ��С�Ƿ񳬹����������ڴ��С,��ֹ����ѭ��
		if( dwCirFreeTotal > _this->m_ulMemSize )
		{
			iRet = ERR_QUEUE;
			LOGOUT("DataPush: Cir Alloc Total Free Space Over Space Size" );
			goto _DATAPUSH_END;
		}

		// �ж������Ƿ���Ի��η���
		if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= (char*)pHead ) // ������󲻻ᳬ��Headλ��
		{
			goto _DATAPUSH_END; // �ѷ���,�ɹ�����ѭ���ж�
		}


		// �ж�ͷ�����Ƿ����ɾ�����ڳ��ռ�
		if( dwCur <= pHead->m_dwPushTime + QUEUEVALID_PERIOD ) // ��ͷ����δ��ʱ����
		{
			iRet = ERR_QUEUE_FULL; // ����ɾ��,�������޷�������Ԫ��
			goto _DATAPUSH_END; // ����ʧ��,����ѭ���ж�
		}
		else
		{
			LOGOUT( "DataPush: Release overdue data" );
		}

		// ���򼴴�ͷ���ݿ����ͷ�
		// �ͷ�ͷ���ݿռ�
		pNext = pHead->m_pNext; // ��Ե�ַ
		if( pNext ) pNext = (PQUEUEHEAD)( (DWORD)pNext + _this->m_pShareMem ); // ���Ե�ַ
		if( !pNext ) // �����ͷ����һ��Ԫ��,���б�Ϊ�ն���
		{
			if( pTail != pHead )
			{
				iRet = ERR_QUEUE;
				LOGOUT( "DataPush: For Last Element, Head Ptr != Tail Ptr" );
				goto _DATAPUSH_END;
			}

			pHead = pTail = NULL; // �����ͷβָ��
			goto _QUEEMPTY_DEAL; // תΪ�ն���״̬���� 
		}

		// ���ͷŵĲ������һ��Ԫ��,���ͷź������Ȼ�ǿ�

		// �ж��Ƿ��ͷŵ�����Top�߽������Ԫ��
		if( pNext < pHead )
		{
			// �ȼ����ۻ��ͷſռ�
			dwCirFreeTotal += (DWORD)( _this->m_pShareMem + _this->m_ulMemSize - (char*)pHead );

			// �ٸ���ͷָ��,���Ե�ַ
			pHead = pNext;

			// Ȼ���жϼ��������Ƿ񳬹�Top�߽�
			if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= _this->m_pShareMem + _this->m_ulMemSize ) // ��������Top�߽�
			{
				//PRTMSG( MSG_DBG, "6\n");
				goto _DATAPUSH_END;
			}
			else
			{
				pNew = (PQUEUEHEAD)( _this->m_pShareMem + sizeofAddr*2 ); // �´δ�Bottom�߽翪ʼ�жϿɷ����
			}
		}
		else
		{
			pHead = pNext; // ����ͷָ��,���Ե�ַ
			dwCirFreeTotal += (DWORD)( (char*)pNext - (char*)pHead ); // �����ۻ��ͷſռ�
		}
	}

_QUEEMPTY_DEAL: // �ն��з���
	pHead = pTail = NULL; // ȷ��ͷβָ��Ϊ��
	pNew = (PQUEUEHEAD)( _this->m_pShareMem + sizeofAddr*2 ); // �ӱ߽翪ʼ����,���Ե�ַ
	if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= _this->m_pShareMem + _this->m_ulMemSize ) // ������󲻻ᳬ���ڴ���Top�߽�
	{
		pHead = pNew; // ����ֻ����ͷָ�� (βָ�����ͳһ����)
		goto _DATAPUSH_END;
	}
	else
	{
		iRet = ERR_QUEUE_TOOSMALL;
		goto _DATAPUSH_END;
	}

_DATAPUSH_END:
	// ��ǰ�����ɹ�,���������ͳһ��Ԫ�ز������
	if( !iRet )
	{
		// ��Ԫ�ظ�ֵ
		memcpy( (char*)pNew + sizeof(QUEUEHEAD), v_pPushData, v_dwPushLen );
		pNew->m_dwPushSymb = v_dwPushSymb;
		pNew->m_iSocketSymb = v_iSocketSymb;
		pNew->m_dwDataLen = v_dwPushLen;
		pNew->m_dwPushTime = dwCur;
		pNew->m_pNext = NULL;
		pNew->m_bValid = TRUE;
		// ԭβԪ�صĺ���ָ��ָ�򱾴ε���βԪ��, ���������Ե�ַ (��ԭβԪ�ش���ʱ)
		if( pTail ) pTail->m_pNext = (PQUEUEHEAD)( (char*)pNew - _this->m_pShareMem );

		// ����βָ��,���Ե�ַ
		pTail = pNew;
	}

	// �ڹ����ڴ��и���ͷβָ�� (��Ե�ַ)
	*(PQUEUEHEAD*) (_this->m_pShareMem) = (PQUEUEHEAD)( pHead ? ((char*)pHead - _this->m_pShareMem) : NULL ); // �޸�ͷָ��
	*(PQUEUEHEAD*) (_this->m_pShareMem + sizeofAddr) = (PQUEUEHEAD)( pTail ? ((char*)pTail - _this->m_pShareMem) : NULL ); // �޸�βָ��

	if( iRet ) LOGOUT("DataPush: Result %d", iRet );
	pthread_mutex_unlock( &_this->m_DataMutex );

	return iRet;
}

/************************************************************************
**������DataDel
**���ܣ�ɾ������
**������
        [in] - v_pQueue�����е�thisָ��
		       v_dwPushSymb��ɾ�����ݵı�ʶ
        [out] - null
**���أ��ɹ���0��ʧ�ܣ�����
************************************************************************/
int DataDel(void *v_pQueue, DWORD v_dwPushSymb )
{
	if(v_pQueue == NULL)
	{
		return ERR_PAR;
	}
	Queue *_this = (Queue *)v_pQueue;
	if( !_this->m_pShareMem || 0 == _this->m_ulMemSize || -1 == _this->m_iShmID ) return ERR_MEMLACK;

	PQUEUEHEAD pHead = * (PQUEUEHEAD*) (_this->m_pShareMem); // ��Ե�ַ
	if( pHead ) pHead = (PQUEUEHEAD)( (DWORD)pHead + _this->m_pShareMem ); // ���Ե�ַ

	PQUEUEHEAD pCur = pHead;
	pthread_mutex_lock( &_this->m_DataMutex );
	while( pCur )
	{
		if( v_dwPushSymb == pCur->m_dwPushSymb )
		{
			pCur->m_bValid = FALSE;
		}

		pCur = pCur->m_pNext;
		if( pCur )
		{
			pCur = (PQUEUEHEAD)( (DWORD)pCur + _this->m_pShareMem );
		}
	}
	pthread_mutex_unlock( &_this->m_DataMutex );
	return 0;
}
/************************************************************************
**������DataClear
**���ܣ���ն���
**������
        [in] - v_pQueue�����е�thisָ��
        [out] - null
**���أ��ɹ���0��ʧ�ܣ����� 
************************************************************************/
int DataClear(void *v_pQueue)
{
	if(v_pQueue == NULL)
	{
		return ERR_PAR;
	}
	Queue *_this = (Queue *)v_pQueue;
	if( !_this->m_pShareMem || 0 == _this->m_ulMemSize || -1 == _this->m_iShmID )
	{
		return ERR_MEMLACK;
	}
	pthread_mutex_lock( &_this->m_DataMutex );
	memset( _this->m_pShareMem, 0, _this->m_ulMemSize );
	pthread_mutex_unlock( &_this->m_DataMutex );
	return 0;
}

/************************************************************************
**������GetAvliableSize
**���ܣ���ȡ��ǰ���ö��пռ�
**������
		[in] - v_pQueue�����е�thisָ��
		[out] - null
**���أ����п��ÿռ��С 
************************************************************************/
DWORD GetAvliableSize(void *v_pQueue)
{
	if(v_pQueue == NULL)
	{
		return ERR_PAR;
	}
	Queue *_this = (Queue *)v_pQueue;
	int iSize = 0;
	PQUEUEHEAD pHead = NULL, pTail = NULL;
	INT32U sizeofAddr=sizeof(int *);
	pthread_mutex_lock( &_this->m_DataMutex );

	pHead = * (PQUEUEHEAD*) (_this->m_pShareMem);
	pTail = * (PQUEUEHEAD*) (_this->m_pShareMem + sizeofAddr);

	if( pHead ) pHead = (PQUEUEHEAD)( (DWORD)pHead + _this->m_pShareMem );
	if( pTail ) pTail = (PQUEUEHEAD)( (DWORD)pTail + _this->m_pShareMem );

	iSize = (char*)pTail - (char*)pHead;

	if( iSize < 0 )
	{
		iSize += _this->m_ulMemSize;
	}

	iSize += sizeof(QUEUEHEAD);

	if( pTail )
	{
		iSize += sizeof(QUEUEHEAD) + pTail->m_dwDataLen + ( pTail->m_dwDataLen % sizeofAddr ? sizeofAddr - pTail->m_dwDataLen % sizeofAddr : 0 );
	}
	
	pthread_mutex_unlock( &_this->m_DataMutex );
	return (DWORD)(_this->m_ulMemSize - iSize - sizeofAddr*2);
}

/************************************************************************
**������ReleaseQueue
**���ܣ��ͷŶ���
**������
		[in] - v_pQueue�����е�thisָ��
		[out] - null
**���أ�void
************************************************************************/
void ReleaseQueue(void *v_pQueue)
{
	if(v_pQueue == NULL)
	{
		LOGOUT("v_pQueue is NULL");
		return ;
	}
	Queue *_this = (Queue *)v_pQueue;
	if( _this->m_pShareMem  )
	{
		free(_this->m_pShareMem);
		_this->m_pShareMem = NULL;
	}
	if(_this->m_iShmID != -1)
		_this->m_iShmID=-1;
	
	pthread_mutex_destroy( &_this->m_DataMutex );
}