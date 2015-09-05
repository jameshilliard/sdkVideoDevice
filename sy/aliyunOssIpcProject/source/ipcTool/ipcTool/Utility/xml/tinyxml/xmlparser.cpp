#include "xmlparser.h"
 

xmlparser::xmlparser(void)
{
}

xmlparser::~xmlparser(void)
{
}
void xmlparser::Encode(S_Data *sData,std::string &xml)
{
	//boost::asio::detail::mutex::scoped_lock lock(mutex_enc);
	TiXmlDocument doc; 
	TiXmlDeclaration *decl = new TiXmlDeclaration("1.0","GB2312" /*"utf-8"*/, ""); 
	doc.LinkEndChild( decl );

	TiXmlElement *lmtRoot = new TiXmlElement("devicecmd"); 
	doc.LinkEndChild(lmtRoot);

	TiXmlElement *lmtId = new TiXmlElement("cmdid");
	lmtRoot->LinkEndChild(lmtId);
	char szCommandId[64];
	sprintf(szCommandId, "%d", sData->commandId);
	std::string str_commandId = szCommandId;
	TiXmlText *txtId = new TiXmlText(str_commandId.c_str());
	lmtId->LinkEndChild(txtId);

	TiXmlElement *lmtName = new TiXmlElement("command");
	lmtRoot->LinkEndChild(lmtName);
	TiXmlText *txtName = new TiXmlText(sData->commandName.c_str());
	lmtName->LinkEndChild(txtName);

	TiXmlElement *lmtType = new TiXmlElement("type");
	lmtRoot->LinkEndChild(lmtType); 
	TiXmlText *txtType = new TiXmlText(sData->type.c_str());
	lmtType->LinkEndChild(txtType);

	TiXmlElement *lmtParamRoot = new TiXmlElement("params");
	lmtRoot->LinkEndChild(lmtParamRoot);
	std::map<std::string,std::string>::iterator it;
	for (it = sData->params.begin(); it != sData->params.end(); it ++)
	{
		TiXmlElement *lmtTmp = new TiXmlElement("param");
		lmtParamRoot->LinkEndChild(lmtTmp);

		TiXmlElement *lmtKey = new TiXmlElement("key");
		lmtTmp->LinkEndChild(lmtKey);
		TiXmlText *txtKey = new TiXmlText(it->first.c_str());
		lmtKey->LinkEndChild(txtKey);

		TiXmlElement *lmtValue = new TiXmlElement("value");
		lmtTmp->LinkEndChild(lmtValue);
		TiXmlText *txtValue = new TiXmlText(it->second.c_str());
		lmtValue->LinkEndChild(txtValue);
	}
	TiXmlPrinter printer;
	doc.Accept(&printer);

	xml.assign(printer.CStr());
}

bool xmlparser::Decode(char *xml,S_Data *sData)
{
	TiXmlDocument doc;
	doc.Parse(xml);
	if ( doc.Error() )
	{
		return false;
	}
	TiXmlElement *lmtRoot = doc.RootElement();
	if(!lmtRoot)
		return false;
	TiXmlElement *lmtId = lmtRoot->FirstChildElement("cmdid");
	if (lmtId)
	{
		const char *id = lmtId->GetText();
		sData->commandId = atoi(id);

		TiXmlElement *lmtName = lmtId->NextSiblingElement("command");
		if (lmtName)
		{
			const char *name = lmtName->GetText();
			sData->commandName.assign(name);
		}

		TiXmlElement *lmtType = lmtId->NextSiblingElement("type");
		if (lmtType)
		{
			const char *type = lmtType->GetText();
			sData->type.assign(type);
		}

		TiXmlElement *lmtParamRoot = lmtId->NextSiblingElement("params");
		if (lmtParamRoot)
		{
			std::string sKey;
			std::string sVaule;
			TiXmlElement *lmtTmp = lmtParamRoot->FirstChildElement();
			while (lmtTmp)
			{
				TiXmlElement *lmtKey = lmtTmp->FirstChildElement("key");
				if (lmtKey)
				{
					TiXmlElement *lmtValue = lmtKey->NextSiblingElement("value");
					if (lmtValue)
					{
						sKey.assign(lmtKey->GetText());
						sVaule.assign(lmtValue->GetText());
						sVaule = sVaule=="(null)"?"":sVaule;
						sData->params[sKey] = sVaule;
					}
				}

				lmtTmp = lmtTmp->NextSiblingElement();
			}
		}
	}
	return true;
}
