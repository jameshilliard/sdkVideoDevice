#include "StdAfx.h"
#include "XMLMethod.h"
#include <iostream>

CXMLMethod::CXMLMethod(void)
{
}

CXMLMethod::~CXMLMethod(void)
{
	if (m_pXMLMethodObject == NULL)
	{
		delete m_pXMLMethodObject;
		m_pXMLMethodObject = NULL;
	}
}
CXMLMethod *CXMLMethod::m_pXMLMethodObject = NULL;

void CXMLMethod::Encode(S_Data *sData,CString &xml)
{
	TiXmlDocument doc; 
	TiXmlDeclaration *decl = new TiXmlDeclaration("1.0","GB2312", ""); 
	doc.LinkEndChild( decl );

	TiXmlElement *lmtRoot = new TiXmlElement("devicecmd"); 
	doc.LinkEndChild(lmtRoot);

	TiXmlElement *lmtId = new TiXmlElement("cmdid");
	lmtRoot->LinkEndChild(lmtId);
	CString tmp;
	tmp.Format("%d",sData->commandId);
	TiXmlText *txtId = new TiXmlText(tmp);
	lmtId->LinkEndChild(txtId);

	TiXmlElement *lmtName = new TiXmlElement("command");
	lmtRoot->LinkEndChild(lmtName);
	TiXmlText *txtName = new TiXmlText(sData->commandName);
	lmtName->LinkEndChild(txtName);

	TiXmlElement *lmtParamRoot = new TiXmlElement("params");
	lmtRoot->LinkEndChild(lmtParamRoot);
	map<CString,CString>::iterator it;
	for (it = sData->params.begin(); it != sData->params.end(); it ++)
	{
		TiXmlElement *lmtTmp = new TiXmlElement("param");
		lmtParamRoot->LinkEndChild(lmtTmp);

		TiXmlElement *lmtKey = new TiXmlElement("key");
		lmtTmp->LinkEndChild(lmtKey);
		TiXmlText *txtKey = new TiXmlText(it->first);
		lmtKey->LinkEndChild(txtKey);

		TiXmlElement *lmtValue = new TiXmlElement("value");
		lmtTmp->LinkEndChild(lmtValue);
		TiXmlText *txtValue = new TiXmlText(it->second);
		lmtValue->LinkEndChild(txtValue);
	}
	TiXmlPrinter printer;
	doc.Accept(&printer);

	xml.Format("%s",printer.CStr());
}

BOOL CXMLMethod::Decode(char *xml,S_Data *sData)
{
	TiXmlDocument doc;
	doc.Parse(xml);
	doc.SaveFile("C:\\error.xml");
	if ( doc.Error() )
	{
		doc.SaveFile("C:\\error.xml");
		CString str;
		str.Format("Error in %s: %s\n", doc.Value(), doc.ErrorDesc());
		return FALSE;
	}
	TiXmlElement *lmtRoot = doc.RootElement();
	if(!lmtRoot)
		return FALSE;

	
	TiXmlElement *lmtId = lmtRoot->FirstChildElement("cmdid");
	if (lmtId)
	{
		const char *id = lmtId->GetText();
		sData->commandId = atoi(id);

		TiXmlElement *lmtName = lmtId->NextSiblingElement("command");
		if (lmtName)
		{
			const char *name = lmtName->GetText();
			sData->commandName.Format("%s",name);
		}

		TiXmlElement *lmtParamRoot = lmtRoot->FirstChildElement("params");
		if (lmtParamRoot)
		{
			CString sKey;
			CString sVaule;
			TiXmlElement *lmtTmp = lmtParamRoot->FirstChildElement();
			while (lmtTmp)
			{
				TiXmlElement *lmtKey = lmtTmp->FirstChildElement("key");
				if (lmtKey)
				{
					TiXmlElement *lmtValue = lmtKey->NextSiblingElement("value");
					if (lmtValue)
					{
						sKey.Format("%s",lmtKey->GetText());
						sVaule.Format("%s",lmtValue->GetText());
						sVaule = sVaule=="(null)"?"":sVaule;
						sData->params[sKey] = sVaule;
					}
				}

				lmtTmp = lmtTmp->NextSiblingElement();
			}
		}
	}
	return TRUE;
}


void	CXMLMethod::CleanStr(CString &SqlStr)
{
	SqlStr.Trim();
	SqlStr.Remove('\'');
	SqlStr.Remove('\r');
	SqlStr.Remove('\n');
}

BOOL CXMLMethod::CheckXMLStrFlag(CString &str)
{
	int nLeftIndex = str.Find("<");
	int nRightIndex = str.Find(">");

	if (nLeftIndex == -1 || nRightIndex == -1)
		return FALSE;
	if (nLeftIndex > nRightIndex)
		return FALSE;
	str = str.Right(str.GetLength() - nRightIndex - 1);
	str.Trim();
	return TRUE;
}

BOOL CXMLMethod::CheckXMLStr(char * pStr)
{
	CString sCheckStr(pStr);
	BOOL bResult = TRUE;
	while(1)
	{
		bResult = CheckXMLStrFlag(sCheckStr);
		if (!bResult)
			return FALSE;
		else
			if (sCheckStr.IsEmpty())
				return TRUE;
	}
}

BOOL CXMLMethod::BufToData(BYTE * bSendBuf,int nlen,S_Data *sData)
{
	if (bSendBuf == NULL)
		return FALSE;
	if (!CheckXMLStr((char*) bSendBuf))
	{
		return FALSE;
	}
	return Decode((char *)bSendBuf,sData);
}



