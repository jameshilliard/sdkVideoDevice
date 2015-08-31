#pragma once
#include "tinyxml/tinyxml.h"
#include <iostream>
#include <CString>
#include <Map>

using namespace std;

// 包结构定义
typedef struct S_Data
{
	int commandId;
	CString commandName;
	CString commandType;
	map<CString,CString> params;
	void CopyHead(S_Data *pData)
	{
		if (pData == NULL)
			return;
		commandId = pData->commandId;
		commandName = pData->commandName;
	}

	void Clear()
	{
		commandId = 0;
		commandName.Empty();
		params.clear();
	}
	void RemoveKey(CString sKey)
	{
		map<CString,CString>::iterator it = params.find(sKey);
		if (it != params.end())
			params.erase(it);
	}
}S_Data;

class CXMLMethod
{
public:
	static CXMLMethod *GetInstance()
	{
		if (m_pXMLMethodObject == NULL)
			m_pXMLMethodObject = new CXMLMethod();
		return m_pXMLMethodObject;
	}
	~CXMLMethod(void);

	BOOL CheckXMLStrFlag(CString &str);
	BOOL CheckXMLStr(char * pStr);

	BOOL BufToData(BYTE * bSendBuf,int nlen,S_Data *sData);
	void CleanStr(CString &SqlStr);
	void Encode(S_Data *sData,CString &xml);
	BOOL Decode(char *xml,S_Data *sData);
private:
	static CXMLMethod *m_pXMLMethodObject;
	CXMLMethod(void);

};
