#ifndef _INNERDATAMNG_H_
#define _INNERDATAMNG_H_
#include "Typedef.h"



typedef struct tagInnerData
{
	struct tagInnerData*	m_pNext;
	struct tagInnerData*	m_pPre;
	void*			m_pData;
	DWORD			m_dwDataLen;
	DWORD			m_dwPushTm;
	BYTE			m_bytLvl;
	DWORD			m_dwSymb;
}InnerDdata;

typedef struct _InnerDataList
{
	struct _InnerDataList *_this;
	DWORD			m_dwInvalidPeriod;	// 单位:毫秒
	DWORD			m_dwSpaceSize;		// 最大存储空间
	DWORD			m_dwSpaceUse;		// 当前使用的空间
	DWORD			m_dwPacketNum;		// 当前保存的数据包个数
	pthread_mutex_t m_DataMutex;	
	InnerDdata*	m_pHead;
	InnerDdata*	m_pTail;

	void (*initCfg)(void *, DWORD v_dwSpaceSize, DWORD v_dwInvalidPeriod );
	void (*release)(void *);
	BOOL (*isEmpty)(void *);
	void (*delOldData)(void *);
	int	 (*pushData)(void *, BYTE , DWORD , char *, DWORD, DWORD *);
	int  (*popData)(void *, BYTE *, DWORD *, DWORD , DWORD *, char *);
}InnerDataList;


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	InnerDataList *ListConstruction();
	void InitCfg(void *v_pInnerDataList, DWORD v_dwSpaceSize, DWORD v_dwInvalidPeriod );
	void ReleaseInnerData(void *v_pInnerDataList);
	BOOL IsEmpty(void *v_pInnerDataList);
	int  DelOldData(void *v_pInnerDataList);
	int  PushData(void *v_pInnerDataList, BYTE v_bytLvl, DWORD v_dwDataLen, char *v_pDataBuf, DWORD v_dwSymb, DWORD *v_dwPacketNum);
	int  PopData(void *v_pInnerDataList, BYTE *v_bytLvl, DWORD *v_dwSymb, DWORD v_dwBufSize, DWORD *v_dwDataLen, char *v_pDataBuf);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif