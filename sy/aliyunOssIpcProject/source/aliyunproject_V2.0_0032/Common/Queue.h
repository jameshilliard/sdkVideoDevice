#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "Typedef.h"
#include "InnerDataMng.h"
#include "GlobFunc.h"
#include "../LogOut/LogOut.h"


typedef struct tagQueueHead
{
	struct tagQueueHead* m_pNext; 

	DWORD		m_dwPushTime;  // 插入时间
	DWORD		m_dwPushSymb;  // 来源标志
	DWORD		m_dwDataLen;   // 插入长度
	BOOL		m_bValid;	   // 是否有效, 删除时可以标记为无效
	int         m_iMsgId;	   // 消息队列的ID
	int  		m_iSocketSymb; // 来源socket的ID
}QUEUEHEAD, *PQUEUEHEAD;

typedef struct _Queue
{
	struct _Queue	*_this;			// 队列this指针
	unsigned long	m_ulMemSize;	// 队列大小
	int				m_iShmID;		// 队列共享开辟的标识
	char*			m_pShareMem;    // 附加好的共享内存地址
	pthread_mutex_t m_DataMutex;	// 互斥锁

	int   (*initQueue)(void *,unsigned long);
	int   (*delData)(void *,DWORD );
	int   (*clearData)(void *);
	DWORD (*getAvliableSize)(void *);
	int	  (*pushData)( void *,void* , DWORD , DWORD , int);
	int   (*popData)(void *, void* , DWORD , DWORD *, DWORD *, int *);
	void  (*release)(void *);
}Queue;

#define QUEUEVALID_PERIOD  1000000

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

	Queue *	QueueListConstruction();
	int 	DataPush(void *v_pQueue, void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, int v_iSocketSymb);
	int 	DataPop(void *v_pQueue, void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb, int *v_p_iSocketSymb);
	int 	DataDel(void *v_pQueue, DWORD v_dwPushSymb );
	int 	InitQueue(void *v_pQueue, unsigned long v_ulQueueSize );
	int 	DataClear(void *v_pQueue);
	DWORD 	GetAvliableSize(void *v_pQueue);
	void 	ReleaseQueue(void *v_pQueue);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif