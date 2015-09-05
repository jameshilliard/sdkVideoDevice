/***********************************************************************************
Copyright (C), 起始年-结束年,  Tech. Co., Ltd.
文件名: 
作  者: 版本: 完成日期: 
描  述: //用于详细说明此文件完成的主要功能，与其他模块关系，及使用注意事项
其  它: 
接口函数列表: 
修改记录列表: 
	修改日期: 修改者: 修改内容简述: 
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