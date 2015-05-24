#include "hi_param.h"
//[tutkcfg]                                        
//tenable                        = "1              "
//tfullfunc                      = "1              "
//uid                            = "hk000001       "
//svraddr1                       = "121.43.234.112 "
//svraddr2                       = "122.248.234.207"
//svraddr3                       = "m2.iotcplatform.com"
//svraddr4                       = "m5.iotcplatform.com"
//pushserver                     = "               "
//floder                         = "               "
//pushserport                    = "               "
//[scccfg]                                         
//sccenable                      = "1              "
//sccsvraddr                     = "xxx            "
//sccsvrport                     = "80             "
//sccname                        = "               "
//sccpasswd                      = "               "
//devname                        = "ipcam          "
//devpasswd                      = "               "
//devlanpwd                      = "               "

int getStringParams(char * path,char *name,char *serverNo,unsigned int size)
{
	FILE * fp;  
	char * line = NULL;  
	size_t len = 0;  
	ssize_t read;  
	int count=0;
	char *findPtr=NULL;
	int iRet=-1;
	fp = fopen(path, "r");  
	if(fp == NULL)  
		return -1;
	if(path==NULL || name==NULL)
		return -2;
	if(strlen(path)==0 || strlen(name)==0)
		return -3;
	memset(serverNo,0,size);
	while((read = getline(&line, &len, fp)) != -1) 
	{    
		 if(strstr(line,name)==line) //uid
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",serverNo);
				if(iRet==1)
				{
					if(serverNo[0]!='"')
					{
						printf("%s=%s\n",name,serverNo);
						iRet=0;
						break;
					}
				}
				
			}
		 }
	}
	if(iRet!=0)
		memset(serverNo,0,size);
	if (line)  
	 	free(line);  
	if(fp != NULL)
		fclose(fp);
	return iRet;
}


int getIPAndPort(char * path,char ip[64],char port[64])
{
	FILE * fp;  
	char * line = NULL;  
	size_t len = 0;  
	ssize_t read;  
	int count=0;
	char *findPtr=NULL;
	int iRet=-1;
	fp = fopen(path, "r");  
	if(fp == NULL)  
		return -1;
	while((read = getline(&line, &len, fp)) != -1) 
	{    
		 if(strstr(line,"ipaddr")==line)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",ip);
				//printf("uname=%s--\n",ip);
			}
		 }
		 
		 if(strstr(line,"httpport")==line)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",port);
				if(iRet==1)
				{
					iRet=0;
					break;
				}
				//printf("password=%s--\n",port);
			}
		 }
	}  
	if (line)  
	 	free(line);  
	if(fp != NULL)
		fclose(fp);
	return iRet;
}

int getUnameAndPassWord(char * path,char uname[64],char password[64])
{
	FILE * fp;  
	char * line = NULL;  
	size_t len = 0;  
	ssize_t read;  
	int count=0;
	int userCount=0;
	char *findPtr=NULL;
	int iRet=-1;
	fp = fopen(path, "r");  
	if(fp == NULL)  
		return -1;
	while((read = getline(&line, &len, fp)) != -1) 
	{    
		 if(strstr(line,"username")==line)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",uname);
				//printf("uname=%s--\n",uname);
				if(iRet==1)
					userCount=count;
			}
		 }
		 
		 if(strstr(line,"password")==line)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",password);
				//printf("password=%s--\n",password);
				if(count==(userCount+1))
				{
					iRet=0;
					break;
				}
			}
		 }
		 count++;
	}  
	if (line)  
	 	free(line);  
	if(fp != NULL)
		fclose(fp);
	return iRet;
}

