#include "Queue.h"

Queue *g_Queue = NULL;

// C构造函数
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
**函数：InitQueue
**功能：初始化共享队列
**参数：
        [in] - v_pQueue：队列的this指针
		   v_kShm：共享队列中的标识
		   v_ulQueueSize:共享队列大小
		   v_iMsgId：来源数据的消息队列ID
		   v_iSocketSymb：来源数据的SoceketID
**返回：0：成功，其他：失败 
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
**函数：DataPop
**功能：弹出队列数据
**参数：
        [in] - v_pQueue：队列的this指针
			   v_pPopData：弹出数据的缓冲区指针
			   v_dwPopSize：存放数据的缓冲区大小
        [out] - v_p_dwPopLen：弹出数据的长度
		        v_p_dwPushSymb：弹出数据的标识
				v_p_iMsgId：来源数据的消息队列ID
				v_p_iSocketSymb：来源数据的SoceketID

**返回：0：成功，其他：失败 
**备注:
	1).每个对象只能一处进行DataPop
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

	PQUEUEHEAD pHead = * (PQUEUEHEAD*) (_this->m_pShareMem); // 相对地址
	if( pHead ) pHead = (PQUEUEHEAD)( (DWORD)pHead + _this->m_pShareMem ); // 绝对地址

	DWORD dwCur = getTickCountMs();
	BOOL bEnd = FALSE;
	do 
	{
		// 绝对地址
		if( !pHead )
		{
			break;
		}

		if( dwCur - pHead->m_dwPushTime <= QUEUEVALID_PERIOD && pHead->m_bValid ) // 若数据未超时且有效
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
		pHead = pHead->m_pNext; // 修改头指针 (得到的是相对地址)
		if( pHead )
		{
			* (PQUEUEHEAD*) (_this->m_pShareMem) = (PQUEUEHEAD)((char*)pHead) ; // 更新首指针, 保存的是相对地址
			pHead = (PQUEUEHEAD)((DWORD)pHead + _this->m_pShareMem); // 转化为绝对地址供后续使用
		}
		else
		{
			// 清除首、尾指针
			* (PQUEUEHEAD*) (_this->m_pShareMem) = NULL;
			* (PQUEUEHEAD*) (_this->m_pShareMem + sizeofAddr) = NULL;
		}
	} while( !bEnd );

	pthread_mutex_unlock( &_this->m_DataMutex );

	return iRet;
}

/************************************************************************
**函数：DataPush
**功能：向队列插入数据
**参数：
        [in] - v_pQueue：队列的this指针
			   v_pPushData：插入数据的数据缓冲区
			   v_dwPushLen：插入数据的长度
			   v_dwPushSymb：插入数据的标识
			   v_iMsgId:消息ID
			   v_iSocketSymb：SocketID
        [out] - null
**返回：0：成功，其他：失败  
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
	
	// 此时得到的仅是相对地址
	pHead = * (PQUEUEHEAD*) (_this->m_pShareMem);
	pTail = * (PQUEUEHEAD*) (_this->m_pShareMem + sizeofAddr);

	// 此时得到的才是绝对地址
	if( pHead ) pHead = (PQUEUEHEAD)( (DWORD)pHead + _this->m_pShareMem );
	if( pTail ) pTail = (PQUEUEHEAD)( (DWORD)pTail + _this->m_pShareMem );

	DWORD dwCirFreeTotal = 0; // 环形分配时释放空间的累积
	DWORD dwCur = getTickCountMs();

	if( pHead ) // 若此时为非空队列
	{
		if( !pTail )
		{
			LOGOUT("DataPush: Head Ptr Vaild but Tail Ptr NULL!" );
			return ERR_QUEUE;
		}

		// 计算往后继续分配时应该使用的起始地址
		pNew = (PQUEUEHEAD)( (char*)pTail + sizeof(QUEUEHEAD) + pTail->m_dwDataLen ); // 新分配地址,绝对地址
		pNew = (PQUEUEHEAD)( (char*)pNew + ( (DWORD)pNew % sizeofAddr ? sizeofAddr - (DWORD)pNew % sizeofAddr : 0 ) ); // 确保内存对齐

		if( pTail >= pHead ) // 若现在是顺向分配 (包括了队列此时只有一个元素的情况下)
		{
			goto _QUESUN_DEAL;
		}
		else
		{
			goto _QUECIR_DEAL;
		}
	}
	else // 说明此时队列为空
	{
		if( pTail )
		{
			LOGOUT("DataPush: Head Ptr NULL but Tail Ptr Invalid!" );
			return ERR_QUEUE;
		}

		goto _QUEEMPTY_DEAL;
	}

_QUESUN_DEAL: // 顺向分配
	if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= _this->m_pShareMem + _this->m_ulMemSize ) // 若继续分配后不会超过内存区Top边界
	{
		goto _DATAPUSH_END;
	}
	else
	{
		pNew = (PQUEUEHEAD)( _this->m_pShareMem + sizeofAddr*2 ); 
		//PRTMSG( MSG_DBG, "4\n");
		goto _QUECIR_DEAL;
	}

_QUECIR_DEAL: // 环形分配
	dwCirFreeTotal = 0;
	while( TRUE )
	{
		// 保险判断Head指针
		if( (char*)pHead > _this->m_pShareMem + _this->m_ulMemSize )
		{
			iRet = ERR_QUEUE;
			LOGOUT("DataPush: Head Ptr Over Top Side" );
			goto _DATAPUSH_END;
		}

		// 保险判断累积释放空间大小是否超过整个共享内存大小,防止无限循环
		if( dwCirFreeTotal > _this->m_ulMemSize )
		{
			iRet = ERR_QUEUE;
			LOGOUT("DataPush: Cir Alloc Total Free Space Over Space Size" );
			goto _DATAPUSH_END;
		}

		// 判断现在是否可以环形分配
		if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= (char*)pHead ) // 若分配后不会超过Head位置
		{
			goto _DATAPUSH_END; // 已分配,成功结束循环判断
		}


		// 判断头数据是否可以删除以腾出空间
		if( dwCur <= pHead->m_dwPushTime + QUEUEVALID_PERIOD ) // 若头数据未超时过期
		{
			iRet = ERR_QUEUE_FULL; // 不能删除,队列满无法插入新元素
			goto _DATAPUSH_END; // 分配失败,结束循环判断
		}
		else
		{
			LOGOUT( "DataPush: Release overdue data" );
		}

		// 否则即此头数据可以释放
		// 释放头数据空间
		pNext = pHead->m_pNext; // 相对地址
		if( pNext ) pNext = (PQUEUEHEAD)( (DWORD)pNext + _this->m_pShareMem ); // 绝对地址
		if( !pNext ) // 若已释放最后一个元素,队列变为空队列
		{
			if( pTail != pHead )
			{
				iRet = ERR_QUEUE;
				LOGOUT( "DataPush: For Last Element, Head Ptr != Tail Ptr" );
				goto _DATAPUSH_END;
			}

			pHead = pTail = NULL; // 先清空头尾指针
			goto _QUEEMPTY_DEAL; // 转为空队列状态处理 
		}

		// 若释放的不是最后一个元素,即释放后队列仍然非空

		// 判断是否释放的是离Top边界最近的元素
		if( pNext < pHead )
		{
			// 先计算累积释放空间
			dwCirFreeTotal += (DWORD)( _this->m_pShareMem + _this->m_ulMemSize - (char*)pHead );

			// 再更新头指针,绝对地址
			pHead = pNext;

			// 然后判断继续分配是否超过Top边界
			if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= _this->m_pShareMem + _this->m_ulMemSize ) // 若不超过Top边界
			{
				//PRTMSG( MSG_DBG, "6\n");
				goto _DATAPUSH_END;
			}
			else
			{
				pNew = (PQUEUEHEAD)( _this->m_pShareMem + sizeofAddr*2 ); // 下次从Bottom边界开始判断可否分配
			}
		}
		else
		{
			pHead = pNext; // 更新头指针,绝对地址
			dwCirFreeTotal += (DWORD)( (char*)pNext - (char*)pHead ); // 计算累积释放空间
		}
	}

_QUEEMPTY_DEAL: // 空队列分配
	pHead = pTail = NULL; // 确保头尾指针为空
	pNew = (PQUEUEHEAD)( _this->m_pShareMem + sizeofAddr*2 ); // 从边界开始分配,绝对地址
	if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= _this->m_pShareMem + _this->m_ulMemSize ) // 若分配后不会超过内存区Top边界
	{
		pHead = pNew; // 这里只更新头指针 (尾指针后续统一更新)
		goto _DATAPUSH_END;
	}
	else
	{
		iRet = ERR_QUEUE_TOOSMALL;
		goto _DATAPUSH_END;
	}

_DATAPUSH_END:
	// 若前面分配成功,则这里进行统一的元素插入操作
	if( !iRet )
	{
		// 新元素赋值
		memcpy( (char*)pNew + sizeof(QUEUEHEAD), v_pPushData, v_dwPushLen );
		pNew->m_dwPushSymb = v_dwPushSymb;
		pNew->m_iSocketSymb = v_iSocketSymb;
		pNew->m_dwDataLen = v_dwPushLen;
		pNew->m_dwPushTime = dwCur;
		pNew->m_pNext = NULL;
		pNew->m_bValid = TRUE;
		// 原尾元素的后向指针指向本次的新尾元素, 保存的是相对地址 (当原尾元素存在时)
		if( pTail ) pTail->m_pNext = (PQUEUEHEAD)( (char*)pNew - _this->m_pShareMem );

		// 更新尾指针,绝对地址
		pTail = pNew;
	}

	// 在共享内存中更新头尾指针 (相对地址)
	*(PQUEUEHEAD*) (_this->m_pShareMem) = (PQUEUEHEAD)( pHead ? ((char*)pHead - _this->m_pShareMem) : NULL ); // 修改头指针
	*(PQUEUEHEAD*) (_this->m_pShareMem + sizeofAddr) = (PQUEUEHEAD)( pTail ? ((char*)pTail - _this->m_pShareMem) : NULL ); // 修改尾指针

	if( iRet ) LOGOUT("DataPush: Result %d", iRet );
	pthread_mutex_unlock( &_this->m_DataMutex );

	return iRet;
}

/************************************************************************
**函数：DataDel
**功能：删除数据
**参数：
        [in] - v_pQueue：队列的this指针
		       v_dwPushSymb：删除数据的标识
        [out] - null
**返回：成功：0，失败：其他
************************************************************************/
int DataDel(void *v_pQueue, DWORD v_dwPushSymb )
{
	if(v_pQueue == NULL)
	{
		return ERR_PAR;
	}
	Queue *_this = (Queue *)v_pQueue;
	if( !_this->m_pShareMem || 0 == _this->m_ulMemSize || -1 == _this->m_iShmID ) return ERR_MEMLACK;

	PQUEUEHEAD pHead = * (PQUEUEHEAD*) (_this->m_pShareMem); // 相对地址
	if( pHead ) pHead = (PQUEUEHEAD)( (DWORD)pHead + _this->m_pShareMem ); // 绝对地址

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
**函数：DataClear
**功能：清空队列
**参数：
        [in] - v_pQueue：队列的this指针
        [out] - null
**返回：成功：0，失败：其他 
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
**函数：GetAvliableSize
**功能：获取当前可用队列空间
**参数：
		[in] - v_pQueue：队列的this指针
		[out] - null
**返回：队列可用空间大小 
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
**函数：ReleaseQueue
**功能：释放队列
**参数：
		[in] - v_pQueue：队列的this指针
		[out] - null
**返回：void
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