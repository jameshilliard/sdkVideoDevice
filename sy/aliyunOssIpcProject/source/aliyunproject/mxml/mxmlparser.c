#include "mxmlparser.h"

/************************************************************************
**������EnCode
**���ܣ�xml����
**������
        [in] - v_sData��Ҫ���б����v_sData�ṹ��ָ��
		 	   iLen����ű��������������ݵĻ�������С
        [out] - szBuf����ű��������������ݵĻ�����
**���أ�������ɺ�����ݳ���
        
**��ע��
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

	xml = mxmlNewXML("1.0");
	command = mxmlNewElement(xml, "devicecmd");

	if(v_sData->szCommandId!=-1)
	{
		id = mxmlNewElement(command, "cmdid");
		sprintf(szId,"%d",v_sData->szCommandId);
		mxmlNewText(id, 0, szId);
	}
	if(strlen(v_sData->szCommandName)!=0)
	{
		name = mxmlNewElement(command, "command");
		mxmlNewText(name, 0, v_sData->szCommandName);
	}
	if(strlen(v_sData->szType)!=0)
	{
		type = mxmlNewElement(command, "type");
		mxmlNewText(type, 0, v_sData->szType);
	}
	
	if(v_sData->iParamCount>0)
		params = mxmlNewElement(command, "params");
	
	for (i = 0;i < v_sData->iParamCount; i++)
	{
		param = mxmlNewElement(params, "param");
		key = mxmlNewElement(param, "key");
		mxmlNewText(key, 0, v_sData->params[i].szKey);
		values = mxmlNewElement(param, "value");
		mxmlNewText(values, 0, v_sData->params[i].szValue);
	}
	
	iDataLen = mxmlSaveString(xml, szBuf, iLen, MXML_NO_CALLBACK);
	
	if(NULL != values)
	{
		mxmlDelete(values);
	}

	if(NULL != key)
	{
		mxmlDelete(key);
	}

	if(NULL != param)
	{
		mxmlDelete(param);
	}

	if(NULL != params)
	{
		mxmlDelete(params);
	}
	if(NULL != type)
	{
		mxmlDelete(type);
	}

	if(NULL != name)
	{
		mxmlDelete(name);
	}

	if(NULL != id)
	{
		mxmlDelete(id);
	}

	if(NULL != command)
	{
		mxmlDelete(command);
	}

	if(NULL != xml)
	{
		mxmlDelete(xml);
	}

	return iDataLen;
}

/************************************************************************
**������DeCode
**���ܣ�xml����
**������
        [in] - szBuf:xml������ָ��
        [out] - v_sData����ŷ��ؽ���ṹ��ָ��
**���أ�void
        
**��ע��
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
	printf("DeCode :%s",szBuf);

	xml = mxmlLoadString(NULL, szBuf, MXML_TEXT_CALLBACK);

	if(NULL == xml)
	{
		goto _DECODE_END;
	}
	command = mxmlFindElement(xml, xml, "devicecmd",NULL,NULL,MXML_DESCEND);
	if(NULL == command)
	{
		goto _DECODE_END;
	}

	id = mxmlFindElement(command, xml, "cmdid",NULL,NULL,MXML_DESCEND);
	if(NULL != id)
	{
		str = mxmlGetText(id, &white);
		v_sData->szCommandId = atoi(str);
		//printf("id = %p\n",id);
		printf("id = %s\n",str);
	}


	name = mxmlFindElement(command, xml, "command",NULL,NULL,MXML_DESCEND);
	if(NULL != name)
	{
		str = mxmlGetText(name, &white);
		strcpy(v_sData->szCommandName, (str == NULL)?"":str);
		//printf("name = %p\n",name);
		printf("name = %s\n",str);
	}
	type = mxmlFindElement(command, xml, "type",NULL,NULL,MXML_DESCEND);
	if(NULL != type)
	{
		str = mxmlGetText(type, &white);
		strcpy(v_sData->szType, (str == NULL)?"":str);
		//printf("type = %p\n",type);
		//printf("type = %s\n",str);
	}
	

	//zss++params = mxmlFindElement(command, xml, "params",NULL,NULL,MXML_DESCEND);
	//zss++if(NULL != params)
	//zss++{
	//zss++	goto _DECODE_END;
	//zss++}
	
	param = mxmlFindElement(command, xml, "param",NULL,NULL,MXML_DESCEND);
	if(NULL == param)
	{	
		goto _DECODE_END;
	}

	do
	{
		lmtTmp = mxmlFindElement(param, xml, "key",NULL,NULL,MXML_DESCEND);
		if(NULL == lmtTmp)
		{
			break ;
		}
		str = mxmlGetText(lmtTmp, &white); /* <key> */
		strcpy(v_sData->params[v_sData->iParamCount].szKey, (str == NULL)?"":str);
		printf("key = %s\n",str);

		lmtTmp = mxmlFindElement(param, xml, "value",NULL,NULL,MXML_DESCEND);
		if(NULL == lmtTmp)
		{
			break ;
		}
		str = mxmlGetText(lmtTmp, &white); /* <value> */

		
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

		printf("value = %s\n",str);

		param = mxmlGetNextSibling(param);
		if(NULL == param)
		{
			break ;
		}
		param = mxmlFindElement(param, xml, "param",NULL,NULL,MXML_DESCEND);
		if(NULL == param)
		{
			break ;
		}
	}while (param);


_DECODE_END:
	if(NULL != lmtTmp)
	{
		mxmlDelete(lmtTmp);
	}

	if(NULL != param)
	{
		mxmlDelete(param);
	}

	if(NULL != params)
	{
		mxmlDelete(params);
	}

	if(NULL != type)
	{
		mxmlDelete(type);
	}

	if(NULL != name)
	{
		mxmlDelete(name);
	}

	if(NULL != id)
	{
		mxmlDelete(id);
	}

	if(NULL != command)
	{
		mxmlDelete(command);
	}

	if(NULL != xml)
	{
		mxmlDelete(xml);
	}
}

/************************************************************************
**������SetXmlValue
**���ܣ�
**������
        [in] - v_sData:xml�洢���ݽṹ
			   v_szKey��xml��key
			   v_szValue��xml��ֵ
**��ע��
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
**������FreeXmlValue
**���ܣ��ͷ�xml���ݵ�value�Ŀռ�
**������
        [in] - v_sData:xml�洢���ݽṹ
**��ע��
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
