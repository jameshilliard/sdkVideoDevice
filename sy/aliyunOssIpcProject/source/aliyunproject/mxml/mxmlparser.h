#ifndef _SZY_MXMLPARSER_H_
#define _SZY_MXMLPARSER_H_

#include "mxml.h"

#define PARAMSNUM 32		// �������ĸ���
#define MAXVALUELEN 128		// ����ֵ��󳤶�
#define MAXKEYLEN 32		// ������ֵ��󳤶�

typedef struct S_Param_
{
	char szKey[32];
	char *szValue;			// ÿ��ʹ����S_Data�ṹ�壬Ҫ����FreeXmlValue�ͷ�szValue���ٵĿռ�
}S_Param;

typedef struct S_Data_
{
	int  szCommandId;			// ����ID
	char szCommandName[32];		// ��������
	char szType[32];			// ����
	int  iParamCount;			// key�����ĸ��������PARAMSNUM��
	S_Param  params[PARAMSNUM]; // ����
}S_Data;

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

int  EnCode(char *szBuf, int iLen, S_Data *v_sData);
void DeCode(char *szBuf, S_Data *v_sData);
void SetXmlValue(S_Data *v_sData, const char *v_szKey, const char *v_szValue);
void SetXmlIntValue(S_Data *v_sData, const char *v_szKey, int value);
void FreeXmlValue(S_Data *v_sData);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif