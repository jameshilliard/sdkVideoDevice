#ifndef _SZY_MXMLPARSER_H_
#define _SZY_MXMLPARSER_H_

#include "mxml.h"

#define PARAMSNUM 32		// 参数最大的个数
#define MAXVALUELEN 128		// 参数值最大长度
#define MAXKEYLEN 32		// 参数键值最大长度

typedef struct S_Param_
{
	char szKey[32];
	char *szValue;			// 每次使用完S_Data结构体，要调用FreeXmlValue释放szValue开辟的空间
}S_Param;

typedef struct S_Data_
{
	int  szCommandId;			// 命令ID
	char szCommandName[32];		// 命令名称
	char szType[32];			// 类型
	int  iParamCount;			// key参数的个数，最大PARAMSNUM个
	S_Param  params[PARAMSNUM]; // 参数
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