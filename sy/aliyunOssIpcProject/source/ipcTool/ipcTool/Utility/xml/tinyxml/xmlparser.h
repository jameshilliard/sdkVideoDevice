/***********************************************************************************
Copyright (C), ��ʼ��-������,  Tech. Co., Ltd.
�ļ���: 
��  ��: �汾: �������: 
��  ��: //������ϸ˵�����ļ���ɵ���Ҫ���ܣ�������ģ���ϵ����ʹ��ע������
��  ��: 
�ӿں����б�: 
�޸ļ�¼�б�: 
	�޸�����: �޸���: �޸����ݼ���: 
***********************************************************************************/
#ifndef XMLPARSER_H
#define XMLPARSER_H


#include <string>
#include <map>
#include "tinyxml.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct S_Data
{
	int commandId;
	std::string commandName;
	std::string type;
	std::map<std::string,std::string> params;
	void Clear()
	{
		commandId = 0;
		commandName.clear();
		type.clear();
		params.clear();
	}
};

class xmlparser
{
public:
	xmlparser(void);
	~xmlparser(void);
	static void	  Encode(S_Data *sData,std::string &xml);
	static bool Decode(char *xml,S_Data *sData);
};


#endif