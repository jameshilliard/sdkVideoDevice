#include "InnerDataMng.h"
#include "../LogOut/LogOut.h"


InnerDataList *g_InnerDataList = NULL;

// C构造函数
InnerDataList *ListConstruction()
{
	g_InnerDataList = (InnerDataList*)malloc(sizeof(InnerDataList));
	g_InnerDataList->initCfg 	= InitCfg;
	g_InnerDataList->release 	= ReleaseInnerData;
	g_InnerDataList->popData 	= PopData;
	g_InnerDataList->pushData 	= PushData;
	g_InnerDataList->isEmpty 	= IsEmpty;
	g_InnerDataList->_this 		= g_InnerDataList;
	return (InnerDataList*)g_InnerDataList;

}

/************************************************************************
**函数：InitCfg
**功能：初始化链表配置
**参数：
        [in] - v_pInnerDataList：当前链表的指针this指针
			   v_dwSpaceSize：链表的最大控件
			   v_dwInvalidPeriod：链表数据的存活时间
        [out] - NULL
**返回：void
************************************************************************/
void InitCfg(void *v_pInnerDataList, DWORD v_dwSpaceSize, DWORD v_dwInvalidPeriod )
{
	if(NULL == v_pInnerDataList)
	{
		LOGOUT("v_pInnerDataList is NULL");
		return ;
	}
	InnerDataList* _this = (InnerDataList*)v_pInnerDataList;
	_this->m_dwSpaceSize = v_dwSpaceSize;
	_this->m_dwSpaceUse = 0;
	_this->m_dwPacketNum = 0;
	_this->m_dwInvalidPeriod = v_dwInvalidPeriod;
	_this->m_pHead = _this->m_pTail = NULL;
	pthread_mutex_init(&_this->m_DataMutex, NULL);
}

/************************************************************************
**函数：Release
**功能：释放链表
**参数：
		[in] - v_pInnerDataList：当前链表的指针this指针
		[out] - NULL
**返回：void
************************************************************************/
void ReleaseInnerData(void *v_pInnerDataList)
{
	if(NULL == v_pInnerDataList)
	{
		LOGOUT("v_pInnerDataList is NULL");
		return ;
	}
	InnerDataList* _this = (InnerDataList*)v_pInnerDataList;
	pthread_mutex_lock( &_this->m_DataMutex );

	InnerDdata* pTemp;
	InnerDdata* p;
	while( _this->m_pHead )
	{
		pTemp = _this->m_pHead->m_pNext;
		p = _this->m_pHead;
		free(p->m_pData);
		free(p);
		_this->m_pHead = pTemp;
	}
	_this->m_pTail = NULL;

	_this->m_dwSpaceUse = 0;
	_this->m_dwPacketNum = 0;

	pthread_mutex_unlock( &_this->m_DataMutex );

	pthread_mutex_destroy( &_this->m_DataMutex );
}
/************************************************************************
**函数：Is_Empty
**功能：判断队列是否为空
**参数：
		[in] - v_pInnerDataList：当前链表的指针this指针
		[out] - NULL
**返回：TRUE:为空，FALSE：不空
************************************************************************/
BOOL IsEmpty(void *v_pInnerDataList)
{
	if(NULL == v_pInnerDataList)
	{
		LOGOUT("v_pInnerDataList is NULL");
		return FALSE;
	}
	InnerDataList* _this = (InnerDataList*)v_pInnerDataList;

	BOOL bRet = TRUE;

	pthread_mutex_lock( &_this->m_DataMutex );

	if( !_this->m_pTail || !_this->m_pHead ) bRet = TRUE;
	else bRet = FALSE;

	pthread_mutex_unlock( &_this->m_DataMutex );

	return bRet;
}

/************************************************************************
**函数：PushData
**功能：向链表压入数据
**参数：
        [in] - v_pInnerDataList：当前链表的指针this指针 
			   v_bytLvl：数据的优先级
			   v_dwDataLen：数据长度
			   v_pDataBuf：数据内容指针
			   v_dwSymb:插入数据的标识
        [out] - v_dwPacketNum：当前链表中的数据个数
**返回：0：成功，其他：失败
************************************************************************/
int  PushData(void *v_pInnerDataList, BYTE v_bytLvl, DWORD v_dwDataLen, char *v_pDataBuf, DWORD v_dwSymb, DWORD *v_dwPacketNum)
{
	if(NULL == v_pInnerDataList)
	{
		LOGOUT("v_pInnerDataList is NULL");
		return ERR_PAR;
	}
	InnerDataList* _this = (InnerDataList*)v_pInnerDataList;
	int iRet = 0;
	InnerDdata* pNewData = NULL;
	BOOL bNeedDelOld = FALSE;

	pthread_mutex_lock( &_this->m_DataMutex );

	DWORD dwCur = getTickCountMs();

	if( _this->m_dwSpaceUse + v_dwDataLen + sizeof(InnerDdata) + 200 > _this->m_dwSpaceSize ) // 若缓冲区即将满
	{
		iRet = ERR_QUEUE_FULL;
		bNeedDelOld = TRUE;
		LOGOUT("PushData Full:%d,%d,%d\n", _this->m_dwSpaceUse, v_dwDataLen, _this->m_dwSpaceSize);
		goto PUSHDATA_END;
	}

	pNewData = (InnerDdata*)malloc(sizeof(InnerDdata));
	if( !pNewData )
	{
		LOGOUT( "new InnerDdata fail\n" );
		iRet = ERR_MEMLACK;
		goto PUSHDATA_END;
	}
	pNewData->m_dwPushTm = dwCur;
	if( v_dwDataLen > 0 )
	{
		pNewData->m_pData = malloc( v_dwDataLen ); // 必须用malloc分配
		if( !(pNewData->m_pData) )
		{
			LOGOUT( "malloc fail, v_dwDataLen = %d\n", v_dwDataLen);
			iRet = ERR_MEMLACK;
			goto PUSHDATA_END;
		}
		memcpy( pNewData->m_pData, v_pDataBuf, v_dwDataLen );
	}
	else
	{
		pNewData->m_pData = NULL;
	}
	pNewData->m_dwDataLen = v_dwDataLen;
	pNewData->m_bytLvl = v_bytLvl;
	pNewData->m_dwSymb = v_dwSymb;

	/// 插入到链表
	if( 1 == v_bytLvl) // 普通级别的
	{
		// 从头插入
		pNewData->m_pNext = _this->m_pHead;

		if( _this->m_pHead )
			_this->m_pHead->m_pPre = pNewData;

		_this->m_pHead = pNewData;
		_this->m_pHead ->m_pPre = NULL;

		if( !_this->m_pTail ) 
		{
			_this->m_pTail = pNewData;
		}
		//printf("pNewData%s\n",pNewData->m_pData);
	}
	else
	{
		/// 从尾插入,但是跳过级别一样的或更高的
		if( !_this->m_pTail )
		{
			_this->m_pTail = pNewData;
			_this->m_pHead = pNewData;
		}
		else
		{
			InnerDdata* pTemp = _this->m_pTail;
			while( pTemp )
			{
				if( pTemp->m_bytLvl < v_bytLvl )
				{
					pNewData->m_pNext = pTemp->m_pNext;
					pNewData->m_pPre = pTemp;

					if( pTemp->m_pNext ) pTemp->m_pNext->m_pPre = pNewData;
					pTemp->m_pNext = pNewData;

					if( _this->m_pTail == pTemp ) _this->m_pTail = pNewData;

					break;
				}
				else pTemp = pTemp->m_pPre;
			}

			if( !pTemp )
			{
				pNewData->m_pNext = _this->m_pHead;
				if( _this->m_pHead ) _this->m_pHead->m_pPre = pNewData;
				_this->m_pHead = pNewData;
				_this->m_pHead->m_pPre = NULL;

			}
		}
	}

	_this->m_dwSpaceUse += pNewData->m_dwDataLen + sizeof(*pNewData);
	_this->m_dwPacketNum ++;

	pNewData = NULL;

PUSHDATA_END:
	if( pNewData ) 
	{
		free(pNewData->m_pData);
		free(pNewData);
	}

	pthread_mutex_unlock( &_this->m_DataMutex );

	if( bNeedDelOld )
	{
		DelOldData(_this);
	}

	*v_dwPacketNum = _this->m_dwPacketNum;

	if( iRet ) LOGOUT("PushData Fail %d\n", iRet);
	if( _this->m_dwSpaceUse > _this-> m_dwSpaceSize ) 
		LOGOUT("Pushdata err:%d,%d\n", _this->m_dwSpaceUse, iRet);

	return iRet;
}

/************************************************************************
**函数：PopData
**功能：取出数据
**参数：
        [in] - v_pInnerDataList：当前链表的指针this指针 
			   v_dwBufSize：存放数据的缓冲区大小
        [out] - v_bytLvl：取出的数据的优先级
				v_dwDataLen：取出的数据的长度
				v_pDataBuf：取出的数据的缓冲区指针
				v_dwSymb:取出的数据的标识
**返回：0：成功，其他：失败
**备注：
	1).每个对象只能一处进行PopData操作
************************************************************************/
int  PopData(void *v_pInnerDataList, BYTE *v_bytLvl, DWORD *v_dwSymb, DWORD v_dwBufSize, DWORD *v_dwDataLen, char * v_pDataBuf )
{
	if(NULL == v_pInnerDataList)
	{
		LOGOUT("v_pInnerDataList is NULL");
		return ERR_PAR;
	}
	InnerDataList* _this = (InnerDataList*)v_pInnerDataList;
	int iRet = 0;
	DWORD dwDiscardPackets = 0; // 丢弃太旧的数据包计数

	InnerDdata* pTemp = NULL;

	pthread_mutex_lock( &_this->m_DataMutex );

	DWORD dwCur = getTickCountMs();

	BOOL bContinue = FALSE;
	do
	{
		if( !_this->m_pTail )
		{
			if( !_this->m_pHead )
			{
				iRet = ERR_QUEUE_EMPTY;
				goto POPDATA_END;
			}
			else
			{
				iRet = ERR_QUEUE;
				goto POPDATA_END;
			}
		}
		else if( !_this->m_pHead )
		{
			iRet = ERR_QUEUE;
			goto POPDATA_END;
		}
		pTemp = _this->m_pTail;
		_this->m_pTail = _this->m_pTail->m_pPre;
		if( _this->m_pTail ) 
		{
			_this->m_pTail->m_pNext = NULL;
		}
		else 
		{
			_this->m_pHead = NULL;
		}
		if( dwCur - pTemp->m_dwPushTm >= _this->m_dwInvalidPeriod || pTemp->m_dwDataLen > v_dwBufSize )
		{
			// 丢弃过时数据包
			dwDiscardPackets ++;
			LOGOUT("pTemp->m_dwDataLen = %d,%d",pTemp->m_dwDataLen ,v_dwBufSize );
			bContinue = TRUE;
		}
		else
		{
			memcpy( v_pDataBuf, pTemp->m_pData, pTemp->m_dwDataLen );
			*v_dwDataLen = pTemp->m_dwDataLen;
			*v_bytLvl = pTemp->m_bytLvl;
			*v_dwSymb = pTemp->m_dwSymb;
			bContinue = FALSE;
		}

		//*v_dwPushTm = pTemp->m_dwPushTm;

		if( _this->m_dwSpaceUse > pTemp->m_dwDataLen + sizeof(*pTemp) )
			_this->m_dwSpaceUse -= pTemp->m_dwDataLen + sizeof(*pTemp);
		else
			_this->m_dwSpaceUse = 0;

		if( _this->m_dwPacketNum > 0 ) 
			_this->m_dwPacketNum --;

		if(pTemp)
		{
			free(pTemp->m_pData);
			free(pTemp);
		}

	} while( bContinue );

	pTemp = NULL;
POPDATA_END:
	pthread_mutex_unlock( &_this->m_DataMutex );

	if( dwDiscardPackets > 0 ) LOGOUT( "Discard %d packets when pop\n", dwDiscardPackets );
	if( _this->m_dwSpaceUse > _this->m_dwSpaceSize ) LOGOUT( "Popdata err:%d,%d\n", _this->m_dwSpaceUse, iRet );
	return iRet;
}

// 删除旧数据，暂时未实现
int DelOldData(void *v_pInnerDataList)
{
	if(NULL == v_pInnerDataList)
	{
		LOGOUT("v_pInnerDataList is NULL");
		return ERR_PAR;
	}
	return 0;
}