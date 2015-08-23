#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "Typedef.h"
#include "InnerDataMng.h"
#include "GlobFunc.h"
#include "../LogOut/LogOut.h"


typedef struct tagQueueHead
{
	struct tagQueueHead* m_pNext; 

	DWORD		m_dwPushTime;  // ����ʱ��
	DWORD		m_dwPushSymb;  // ��Դ��־
	DWORD		m_dwDataLen;   // ���볤��
	BOOL		m_bValid;	   // �Ƿ���Ч, ɾ��ʱ���Ա��Ϊ��Ч
	int         m_iMsgId;	   // ��Ϣ���е�ID
	int  		m_iSocketSymb; // ��Դsocket��ID
}QUEUEHEAD, *PQUEUEHEAD;

typedef struct _Queue
{
	struct _Queue	*_this;			// ����thisָ��
	unsigned long	m_ulMemSize;	// ���д�С
	int				m_iShmID;		// ���й����ٵı�ʶ
	char*			m_pShareMem;    // ���ӺõĹ����ڴ��ַ
	pthread_mutex_t m_DataMutex;	// ������

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