#include "mxmlparser.h"

/************************************************************************
**函数：EnCode
**功能：xml编码
**参数：
        [in] - v_sData：要进行编码的v_sData结构体指针
		 	   iLen：存放编码完后的数据内容的缓冲区大小
        [out] - szBuf：存放编码完后的数据内容的缓冲区
**返回：编码完成后的数据长度
        
**作者：沈海波, 2014-04-04
**备注：
       1). 
************************************************************************/
int EnCode(char *szBuf, int iLen, S_Data *v_sData)
{
	mxml_node_t *xml = NULL;		/* <?xml ... ?> */
	mxml_node_t *command = NULL;	/* <command> */
	mxml_node_t *id = NULL;			/* <id> */
	mxml_node_t *name = NULL;		/* <name> */
	mxml_node_t *type = NULL;		/* <type> */
	mxml_node_t *params = NULL;		/* <params> */
	mxml_node_t *param = NULL;		/* <param> */
	mxml_node_t *key = NULL;		/* <key> */
	mxml_node_t *values = NULL;		/* <value> */
	int i;
	char szId[32] = {0};
	int iDataLen = 0;

	xml = SZY_mxmlNewXML("1.0");
	command = SZY_mxmlNewElement(xml, "command");
	id = SZY_mxmlNewElement(command, "id");
	
	sprintf(szId,"%d",v_sData->szCommandId);
	SZY_mxmlNewText(id, 0, szId);
	name = SZY_mxmlNewElement(command, "name");
	SZY_mxmlNewText(name, 0, v_sData->szCommandName);
	type = SZY_mxmlNewElement(command, "type");
	SZY_mxmlNewText(type, 0, v_sData->szType);

	params = SZY_mxmlNewElement(command, "params");

	for (i = 0;i < v_sData->iParamCount; i++)
	{
		param = SZY_mxmlNewElement(params, "param");
		key = SZY_mxmlNewElement(param, "key");
		SZY_mxmlNewText(key, 0, v_sData->params[i].szKey);
		values = SZY_mxmlNewElement(param, "value");
		SZY_mxmlNewText(values, 0, v_sData->params[i].szValue);
	}
	
	iDataLen = SZY_mxmlSaveString(xml, szBuf, iLen, MXML_NO_CALLBACK);
	
	if(NULL != values)
	{
		SZY_mxmlDelete(values);
	}

	if(NULL != key)
	{
		SZY_mxmlDelete(key);
	}

	if(NULL != param)
	{
		SZY_mxmlDelete(param);
	}

	if(NULL != params)
	{
		SZY_mxmlDelete(params);
	}
	if(NULL != type)
	{
		SZY_mxmlDelete(type);
	}

	if(NULL != name)
	{
		SZY_mxmlDelete(name);
	}

	if(NULL != id)
	{
		SZY_mxmlDelete(id);
	}

	if(NULL != command)
	{
		SZY_mxmlDelete(command);
	}

	if(NULL != xml)
	{
		SZY_mxmlDelete(xml);
	}

	return iDataLen;
}

/************************************************************************
**函数：DeCode
**功能：xml解码
**参数：
        [in] - szBuf:xml的数据指针
        [out] - v_sData：存放返回结果结构体指针
**返回：void
        
**作者：沈海波, 2014-04-04
**备注：
       1). 
************************************************************************/
void DeCode(char *szBuf, S_Data *v_sData)
{
	mxml_node_t *xml = NULL;		/* <?xml ... ?> */
	mxml_node_t *command = NULL;	/* <command>	*/
	mxml_node_t *id = NULL;			/* <id>			*/
	mxml_node_t *name = NULL;		/* <name>		*/
	mxml_node_t *type = NULL;		/* <type>		*/
	mxml_node_t *params = NULL;		/* <params>		*/
	mxml_node_t *param = NULL;		/* <param>		*/
	mxml_node_t *lmtTmp= NULL;

	const char *str = NULL;
	int white = 1;
	int iValueLen = 0;
	v_sData->iParamCount = 0;
	//printf("DeCode :%s",szBuf);

	xml = SZY_mxmlLoadString(NULL, szBuf, MXML_TEXT_CALLBACK);

	if(NULL == xml)
	{
		goto _DECODE_END;
	}
	command = SZY_mxmlFindElement(xml, xml, "command",NULL,NULL,MXML_DESCEND);
	if(NULL == command)
	{
		goto _DECODE_END;
	}

	id = SZY_mxmlFindElement(command, xml, "id",NULL,NULL,MXML_DESCEND);
	if(NULL == id)
	{
		goto _DECODE_END;
	}
	str = SZY_mxmlGetText(id, &white);
	v_sData->szCommandId = atoi(str);
	//printf("id = %p\n",id);
	//printf("id = %s\n",str);

	name = SZY_mxmlFindElement(command, xml, "name",NULL,NULL,MXML_DESCEND);
	if(NULL == name)
	{
		goto _DECODE_END;
	}
	str = SZY_mxmlGetText(name, &white);
	strcpy(v_sData->szCommandName, (str == NULL)?"":str);
	//printf("name = %p\n",name);
	//printf("name = %s\n",str);

	type = SZY_mxmlFindElement(command, xml, "type",NULL,NULL,MXML_DESCEND);
	if(NULL == type)
	{
		goto _DECODE_END;
	}
	str = SZY_mxmlGetText(type, &white);
	strcpy(v_sData->szType, (str == NULL)?"":str);
	//printf("type = %p\n",type);
	//printf("type = %s\n",str);

	params = SZY_mxmlFindElement(command, xml, "params",NULL,NULL,MXML_DESCEND);
	if(NULL == params)
	{
		goto _DECODE_END;
	}
	
	param = SZY_mxmlFindElement(params, xml, "param",NULL,NULL,MXML_DESCEND);
	if(NULL == param)
	{	
		goto _DECODE_END;
	}

	do
	{
		lmtTmp = SZY_mxmlFindElement(param, xml, "key",NULL,NULL,MXML_DESCEND);
		if(NULL == lmtTmp)
		{
			break ;
		}
		str = SZY_mxmlGetText(lmtTmp, &white); /* <key> */
		strcpy(v_sData->params[v_sData->iParamCount].szKey, (str == NULL)?"":str);
		//printf("key = %s\n",str);

		lmtTmp = SZY_mxmlFindElement(param, xml, "value",NULL,NULL,MXML_DESCEND);
		if(NULL == lmtTmp)
		{
			break ;
		}
		str = SZY_mxmlGetText(lmtTmp, &white); /* <value> */

		
		if(NULL == str)
		{
			iValueLen = 10;
		}
		else
		{
			iValueLen = strlen(str) + 10;
		}
		v_sData->params[v_sData->iParamCount].szValue = (char *)malloc(iValueLen);
		memset(v_sData->params[v_sData->iParamCount].szValue, 0, iValueLen);
		strcpy(v_sData->params[v_sData->iParamCount++].szValue, (str == NULL)?"":str);

		//printf("value = %s\n",str);

		param = SZY_mxmlGetNextSibling(param);
		if(NULL == param)
		{
			break ;
		}
		param = SZY_mxmlFindElement(param, xml, "param",NULL,NULL,MXML_DESCEND);
		if(NULL == param)
		{
			break ;
		}
	}while (param);


_DECODE_END:
	if(NULL != lmtTmp)
	{
		SZY_mxmlDelete(lmtTmp);
	}

	if(NULL != param)
	{
		SZY_mxmlDelete(param);
	}

	if(NULL != params)
	{
		SZY_mxmlDelete(params);
	}

	if(NULL != type)
	{
		SZY_mxmlDelete(type);
	}

	if(NULL != name)
	{
		SZY_mxmlDelete(name);
	}

	if(NULL != id)
	{
		SZY_mxmlDelete(id);
	}

	if(NULL != command)
	{
		SZY_mxmlDelete(command);
	}

	if(NULL != xml)
	{
		SZY_mxmlDelete(xml);
	}
}

/************************************************************************
**函数：SetXmlValue
**功能：
**参数：
        [in] - v_sData:xml存储数据结构
			   v_szKey：xml的key
			   v_szValue：xml的值
**作者：沈海波, 2014-04-15
**备注：
       1). 
************************************************************************/
void SetXmlValue(S_Data *v_sData, const char *v_szKey, const char *v_szValue)
{
	if((NULL == v_sData)||(NULL == v_szKey)||(NULL == v_szValue))
	{
		return ;
	}
	
	int iValueLen = strlen(v_szValue);
	//printf("v_szKey = %s,iValueLen = %d\n", v_szKey, iValueLen);
	strcpy(v_sData->params[v_sData->iParamCount].szKey, v_szKey);
	
	v_sData->params[v_sData->iParamCount].szValue = (char *)malloc(iValueLen + 10);
	memset(v_sData->params[v_sData->iParamCount].szValue, 0, iValueLen + 10);
	strcpy(v_sData->params[v_sData->iParamCount++].szValue, (iValueLen == 0)?"":v_szValue);
}


/************************************************************************
**函数：FreeXmlValue
**功能：释放xml数据的value的空间
**参数：
        [in] - v_sData:xml存储数据结构
**作者：沈海波, 2014-04-15
**备注：
       1). 
************************************************************************/
void FreeXmlValue(S_Data *v_sData)
{
	int i = 0;

	for (i = 0; i< v_sData->iParamCount; i++)
	{
		if(v_sData->params[i].szValue != NULL)
		{
			free(v_sData->params[i].szValue);
		}
	}
}