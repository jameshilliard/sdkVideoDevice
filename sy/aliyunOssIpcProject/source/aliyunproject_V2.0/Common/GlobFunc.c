#include "GlobFunc.h"

const char b64_alphabet[65] = { 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/=" };

#define KEYVALLEN 100


INT32U getFreeMemory(void)
{
    struct sysinfo s_info;
    int error;
    error = sysinfo(&s_info);
    //printf("getFullMemory %lu kb\n",s_info.freeram);
    return s_info.freeram;
}

/************************************************************************
**函数：CreateConfigFile
**功能：创建配置文件
**参数：
        [in] - v_szFilePath：配置文件的路径（包含配置文件名字）
		       v_szWriteBuf：写入配置文件的数据缓存指针
			   v_iLen：写入配置文件的数据长度
**返回：void
        
**备注：
       1). 目前只用来创建设备编号和产品型号配置文件
************************************************************************/
INT32S CreateConfigFile(char *v_szFilePath, char *v_szWriteBuf, int v_iLen)
{
	if((v_szFilePath == NULL)||(v_szWriteBuf == NULL))
	{
		LOGOUT("v_szFilePath is %d,v_szWriteBuf is %d",(int)(v_szFilePath==NULL),(int)(v_szWriteBuf==NULL));
		return -1;
	}

	FILE *fp = NULL;
	fp = fopen(v_szFilePath, "w+");
	if(fp == NULL)
	{
		LOGOUT("create %s failed , fopen error" ,v_szFilePath);
		return -2;
	}
	unsigned int len = fwrite(v_szWriteBuf, 1, v_iLen, fp);
	if(len != v_iLen)
	{
		LOGOUT("create %s failed , fwrite len error", v_szFilePath);
		fclose(fp);
		return -3;
	}
	fclose(fp);
	return 0;
}

/************************************************************************
**函数：ReadConfigFile
**功能：读取文件的内容
**参数：
[in] - v_szFilePath：配置文件的路径（包含配置文件名字）
	   v_szReadBuf：存放读取配置文件的数据缓存指针
	   v_iBufLen：存放读取配置文件的数据缓冲区大小
**返回：void
**备注：
1). 目前只用来读取设备编号和产品型号
************************************************************************/
INT32S ReadConfigFile(char *v_szFilePath, char *v_szReadBuf, int v_iBufLen)
{
	if((v_szFilePath == NULL) || (v_szReadBuf == NULL))
	{
		LOGOUT("v_szFilePath is %d,v_szReadBuf is %d",(int)(v_szFilePath==NULL),(int)(v_szReadBuf==NULL));
		return -1;
	}
	FILE *fp = NULL;
	
	fp = fopen(v_szFilePath, "r");
	if(fp == NULL)
	{
		LOGOUT("readServerNo failed , fopen serverNo error");
		return -2;
	}
	fread(v_szReadBuf, 1, v_iBufLen, fp);

	//LOGOUT("v_szReadBuf = %s", v_szReadBuf);
	fclose(fp);
	return 0;
}

INT32S isDeviceAccess(const char *filePath)
{
    INT32S iRet = -1;
	if(NULL == filePath)
	{
		printf("filePath is NULL\n");
		return -1;
	}
	if(0 == strlen(filePath))
	{
        printf("filePath size is 0\n");
        return -2;
	}
	iRet = access(filePath,F_OK);
	
	if(iRet < 0)
		return iRet;
	else
		return 0;
}


//获取开机到现在的毫秒数
DWORD getTickCountMs()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

INT32S usSleep(long us) 
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = us;
    return select(0, NULL, NULL, NULL, &tv);
}

// 获取文件大小
DWORD getFileSize(LPCTSTR path)    
{    
	DWORD filesize = 0;        
	struct stat statbuff;    
	if(stat(path, &statbuff) < 0){    
		return filesize;    
	}else{    
		filesize = statbuff.st_size;    
	}    
	return filesize;    
}    

INT32S isMp4File(LPCTSTR path)
{
	if(NULL==path)
		return -1;
	INT32S iRet = -2;
	INT32S numRead=0;
	INT32U endNum=0;
	INT32U endFreeFlag=0x65657266;
	INT32U endFree=0;
	//printf("endNum is 0x%x,endFree is 0x%x\n",endNum,endFree);
	FILE *fp=NULL;
	if ((fp = fopen(path, "rb")) != NULL)
	{
		iRet=fseek(fp,-136L,SEEK_END);//把文件内部指针退回到离文件结尾100字节处。
		if(iRet!=0)
		{
			fclose(fp);
			return iRet;
		}
		numRead = fread((void *)(&endNum), sizeof(char), 4, fp);
		if(numRead!=4)
		{
			fclose(fp);
			return -2;
		}
		numRead = fread((void *)(&endFree), sizeof(char), 4, fp);
		if(numRead!=4)
		{
			fclose(fp);
			return -3;
		}
		printf("endNum is 0x%x,endFree is 0x%x\n",endNum,endFree);
		if(endNum==0x88000000 && endFree==endFreeFlag)
		{
			iRet=0;
		}
		else
		{
			iRet=-4;
		}
		fclose(fp);
	}
	return iRet;
}

INT8U calCRC8(INT8U *pData,DWORD nLength)
{
	if(pData==NULL)
		return -1;
	INT8U crc;
	INT8U i;
	crc = 0;
	while(nLength--)
	{
		crc ^= *pData++;
		for(i = 0; i < 8; i++)
		{
			if(crc & 0x80)
			{
				crc = (crc << 1) ^ 0x31;
			}
			else
			{
				crc <<= 1;
			}
		}
	}
	return crc;
}
void squeeze(char *s,int maxSize,int c)
{
	int	i,j;
	for(i=0,j=0;(s[i]!='\0')&&(i<maxSize);i++)
	{
		if(s[i]!=c)
			s[j++]=s[i];
	}
	if(j!=(maxSize-1))
		s[j]='\0';//这一条语句千万不能忘记，字符串的结束标记
}
INT32S isFileSystemBigger(LPCTSTR sdDir,DWORD size) 
{
	struct statfs disk_statfs;
	DWORD  totalspace = 0;
	if(statfs(sdDir, &disk_statfs) >= 0 )
	{
		totalspace =  4*disk_statfs.f_bavail;
	}
	else
	{
		return -1;
	}
	//printf("total size is %d---\n",totalspace);
	if(totalspace < size)
	{
		
		return -2;
	}
	return 0;
}
INT32S split( char **szArray, char *v_szSplitStr, const char *v_szDdelim, int v_iArayNum)
{
	if((v_szSplitStr == NULL)||(szArray == NULL)||(v_szDdelim == NULL))
	{
	    LOGOUT("v_szSplitStr is %d,szArray is %d,v_szDdelim is %d",(int)(v_szSplitStr == NULL),(int)(szArray == NULL),(int)(v_szDdelim == NULL));
		return -1;
	}
	char *s = NULL; 
	int iArrayCount = 0;
	s = strtok(v_szSplitStr, v_szDdelim);
	while(s != NULL)
	{
		*szArray++ = s;
		iArrayCount ++;
		if(iArrayCount >= v_iArayNum)
			break;
		s = strtok(NULL,v_szDdelim);
	}
	return iArrayCount;
}


/************************************************************************
**函数：SzySdkSendData
**功能：发送函数
**参数：
        [in] - v_iSocket：socket句柄
		     - v_szSendBuf:发送缓冲区
			 - v_iSendLen：发送长度
返回：-1:失败，0：成功 
**备注：
       1). 
************************************************************************/
int SzySdkSendData(int v_iSocket, char *v_szSendBuf, int v_iSendLen)
{
    if((v_iSocket < 0) || (NULL == v_szSendBuf) || v_iSendLen < 0)
    {
        return -1;
    }
	fd_set fdWrite,eset;
	struct timeval tv;
	int iRet = 0;
	int iSendCount = 0;
	int iSendSize = 0;
	int iTotalSendSize = 0;

	do
	{
		tv.tv_sec = 0;
		tv.tv_usec = 50*1000;
		iSendSize = 0;

		FD_ZERO(&fdWrite);
		FD_ZERO(&eset);
		FD_SET(v_iSocket, &eset);
		FD_SET(v_iSocket, &fdWrite);

		iRet = select(v_iSocket+1, NULL, &fdWrite, &eset, &tv);
		if(iRet <= 0 )
		{
			continue;
		}

		if( FD_ISSET(v_iSocket, &eset) )
		{
			continue;
		}
		else if( FD_ISSET(v_iSocket, &fdWrite) )
		{
			iSendSize = send(v_iSocket, v_szSendBuf + iTotalSendSize, v_iSendLen - iTotalSendSize, 0);
			if(iSendSize < 0)
			{
				return -1;
			}
			iTotalSendSize += iSendSize;

			if(iTotalSendSize >= v_iSendLen)
			{
				//SZY_LOG("iTotalSendSize = %d, iSendSize = %d",iTotalSendSize, iSendSize);
				return 0;// 发送完全退出
			}
		}
	}while ((iSendCount++ < 10));

	return -1;
}

INT32S creatDir(LPCTSTR pDir)  
{  
    INT32S i = 0;  
    INT32S iRet;  
    INT32S iLen;  
    LPTSTR pszDir=NULL;  
    if(NULL == pDir)  
		return 0;  
    pszDir = strdup(pDir);  
    iLen = strlen(pszDir);  
    // 创建中间目录  
    for (i = 0;i < iLen;i ++)  
    {  
		if(pszDir[i] == '/' && i!=0)  
        {   
            pszDir[i] = '\0';  
            //如果不存在,创建  
            iRet = access(pszDir,0);  
            if (iRet != 0)  
            {  
				iRet = mkdir(pszDir,0755); 
                if (iRet != 0)  
                    return -1;  
            }  
            //支持linux,将所有\换成/  
            pszDir[i] = '/';  
        }   
    }  
    iRet = mkdir(pszDir,0755);  
    free(pszDir);  
    return iRet;  
}  

INT32S writeFile(LPCTSTR filePath,LPCTSTR fileBuffer,DWORD size)
{
	if(NULL==filePath || NULL==fileBuffer)
		return -1;
	INT32S iRet = -2;
	FILE *fp=NULL;
	if ((fp = fopen(filePath, "w+b")) != NULL)
	{   
		SLONG locWrite = 0;   
		SLONG numLeft = size-locWrite; 
		SLONG numWrite = 0;
		INT32U writeCount=0;
		while(numLeft>0) 
		{   
			numWrite = fwrite(fileBuffer+locWrite, sizeof(char), numLeft, fp);   
			if (numWrite<numLeft || writeCount++>100) 
			{   
				break;   
			}   
			locWrite += numWrite;   
			numLeft -= numWrite;   
		}   
		if(fclose(fp) == 0)
		{
			if(numLeft == 0)
				iRet=0;
			else
				iRet=-3;
		} 
		else
			iRet=-4;
	} 
	else
	{
		LOGOUT("  %s open failure",filePath)		
	}
	return iRet;
}

INT32S rmDirFile(const char *pszDir)
{
	if(pszDir==NULL)
		return -1;
	if(strlen(pszDir)==0)
		return -1;
	if(0!=isDeviceAccess(pszDir))
	{
		return -1;
	}
	DIR    *dir;
    struct dirent *ptr;
	unsigned int count=0;
    dir = opendir(pszDir); ///open the dir
    int iRet=0;
	char filePath[256]={0};
    while((ptr = readdir(dir)) != NULL) ///read the list of this dir
    {
		iRet++;
		memset(filePath,0,sizeof(filePath));
		sprintf(filePath,"%s/%s",pszDir,ptr->d_name);
		unlink(filePath);
		LOGOUT("unlink %s",filePath);
    }
    closedir(dir);
    return iRet;
}

INT32S readMediaFile(const char *pszDir,char fileName[MAX_PATH])
{
	if(pszDir==NULL)
		return -1;
	if(strlen(pszDir)==0)
		return -1;
	if(0!=isDeviceAccess(pszDir))
	{
		return -1;
	}
	DIR    *dir;
    struct dirent *ptr;
	unsigned int count=0;
	//int i=0;
	//char otherFileName[MAX_PATH]={0};
    dir = opendir(pszDir); ///open the dir
    int iRet=-1;
    while((ptr = readdir(dir)) != NULL) ///read the list of this dir
    {
		switch(ptr->d_type)
		{
			case DT_REG:
				if(strstr(ptr->d_name,"jpg"))
				{
					iRet=1;
				}
				if(strstr(ptr->d_name,"mp4"))
				{
					//memset(otherFileName,0,sizeof(otherFileName));
					//sprintf(otherFileName,"%s/%s",pszDir,ptr->d_name);
					//printf("otherFileName 1 %s i=%d\n",otherFileName,i);
					//iRet=isMp4File(otherFileName);
					//if(iRet==0)
					//{
					//	i++;
					//	printf("otherFileName 2 %s i=%d\n",otherFileName,i);
					//}
					iRet=2;
				}
				//if(strstr(ptr->d_name,"dat"))
				//{
				//	iRet=3;
				//}
				//if(iRet==1 || iRet==2 || iRet==3)
				if(iRet==1 || iRet==2)
				{
					printf("d_type:%s %d d_name: %s\n",pszDir,ptr->d_type,ptr->d_name);
					strcpy(fileName,ptr->d_name);
					closedir(dir);
					return iRet;
				}
				break;
			default:
				break; 
		}
	
    }
    closedir(dir);
    return iRet;
}

INT32S readFile(LPCTSTR filePath,LPCTSTR fileBuffer,DWORD bufferSize,DWORD *fileSize)
{
	if(NULL==filePath || NULL==fileBuffer)
		return -1;
	INT32S iRet = -2;
	FILE *fp=NULL;
	*fileSize=getFileSize(filePath);
	printf("fileSize is %d------",*fileSize);
	if(bufferSize<*fileSize)
	{
		return -3;
	}
	if ((fp = fopen(filePath, "rb")) != NULL)
	{   
		DWORD size=*fileSize;
		DWORD locRead = 0;   
		DWORD numLeft = size; 
		DWORD numRead = 0;
		INT32U readCount=0;
		while(size>locRead) 
		{   
			numRead = fread((void *)(fileBuffer+locRead), sizeof(char), numLeft, fp);   
			if (numRead<numLeft || readCount++>100) 
			{   
				break;   
			}   
			locRead+= numRead;   
			numLeft-= numRead;   
		}   
		if(fclose(fp) == 0)
		{
			if(numLeft == 0)
				iRet=0;
			else
				iRet=-4;
		} 
		else
			iRet=-5;
			
	} 
	else
	{
		LOGOUT("  %s open failure",filePath)		
	}
	return iRet;
}

INT32S readOrgFile(LPCTSTR filePath,LPCTSTR fileBuffer,DWORD bufferSize,DWORD *fileSize)
{
	if(NULL==filePath || NULL==fileBuffer)
		return -1;
	INT32S iRet = -2;
	FILE *fp=NULL;
	if ((fp = fopen(filePath, "r")) != NULL)
	{   
		DWORD numRead = 0;
		numRead = fread((void *)(fileBuffer), sizeof(char), bufferSize, fp);   
		if (numRead>0) 
		{   
			*fileSize=numRead;
			iRet=0;
		}  
		else
		{
			iRet=-3;
		}
		if(fclose(fp) != 0)
		{
			iRet=-5;
		}	
	} 
	else
	{
		LOGOUT("  %s open failure",filePath)		
	}
	return iRet;
}


char * SY_base64Encode(const char *text) 
{
	  /* The tricky thing about this is doing the padding at the end,
	   * doing the bit manipulation requires a bit of concentration only */
	  char *buffer = NULL;
	  char *point = NULL;
	  int inlen = 0;
	  int outlen = 0;

	  /* check our args */
	  if (text == NULL)
	    return NULL;
	  
	  /* Use 'buffer' to store the output. Work out how big it should be...
	   * This must be a multiple of 4 bytes */
	  
	  inlen = strlen( text );
	  /* check our arg...avoid a pesky FPE */
	  if (inlen == 0)
	    {
	      buffer = malloc(sizeof(char));
	      buffer[0] = '\0';
	      return buffer;
	    }
	  outlen = (inlen*4)/3;
	  if( (inlen % 3) > 0 ) /* got to pad */
	    outlen += 4 - (inlen % 3);
	  
	  buffer = malloc( outlen + 1 ); /* +1 for the \0 */
	  memset(buffer, 0, outlen + 1); /* initialize to zero */
	  /* now do the main stage of conversion, 3 bytes at a time,
	   * leave the trailing bytes (if there are any) for later */
	  
	  for( point=buffer; inlen>=3; inlen-=3, text+=3 ) {
	    *(point++) = b64_alphabet[ *text>>2 ]; 
	    *(point++) = b64_alphabet[ (*text<<4 & 0x30) | *(text+1)>>4 ]; 
	    *(point++) = b64_alphabet[ (*(text+1)<<2 & 0x3c) | *(text+2)>>6 ];
	    *(point++) = b64_alphabet[ *(text+2) & 0x3f ];
	  }
	  
	  /* Now deal with the trailing bytes */
	  if( inlen ) {
	    /* We always have one trailing byte */
	    *(point++) = b64_alphabet[ *text>>2 ];
	    *(point++) = b64_alphabet[ (*text<<4 & 0x30) |
				     (inlen==2?*(text+1)>>4:0) ]; 
	    *(point++) = (inlen==1?'=':b64_alphabet[ *(text+1)<<2 & 0x3c ] );
	    *(point++) = '=';
	  }
	  
	  *point = '\0';
  	  return buffer;
}

//#define 	PATH_MAX	256
int find_pid_by_name( char* ProcName, int* foundpid)
{
    DIR             *dir;
    struct dirent   *d;
    int             pid, i;
    char            *s;
    int pnlen;
	if(ProcName==NULL || foundpid==NULL)
	{
	    printf("param is error\n");
        return -2;	
	}
    i = 0;
    foundpid[0] = 0;
    pnlen = strlen(ProcName);
    /* Open the /proc directory. */
    dir = opendir("/proc");
    if (!dir)
    {
        printf("cannot open /proc");
        return -1;
    }
    /* Walk through the directory. */
    while ((d = readdir(dir)) != NULL) 
	{
        char exe [PATH_MAX+1];
        char path[PATH_MAX+1];
        int len;
        int namelen;
        /* See if this is a process */
        if ((pid = atoi(d->d_name)) == 0)       continue;
        snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
        if ((len = readlink(exe, path, PATH_MAX)) < 0)
                continue;
        path[len] = '\0';
        /* Find ProcName */
        s = strrchr(path, '/');
        if(s == NULL) continue;
        s++;
        /* we don't need small name len */
        namelen = strlen(s);
        if(namelen < pnlen)     continue;
        if(!strncmp(ProcName, s, pnlen)) 
		{
            /* to avoid subname like search proc tao but proc taolinke matched */
            if(s[pnlen] == ' ' || s[pnlen] == '\0') 
			{
                foundpid[i] = pid;
                i++;
            }
        }
    }
    foundpid[i] = 0;
    closedir(dir);
    return  0;
}

int GetSendSocketTraffic(char* ProcName, unsigned long long* socketNums)
{
	int iRet=0;
	unsigned long long socketNum=0;
	int foundPid=0;
	char path[PATH_MAX+1]={0,};
	char data[4096]={0,};
	char buffer[4096]={0,};
	char networkName[32]={0,};
	DWORD fileSize=0;
	*socketNums=0;
	if(ProcName==NULL || socketNums==NULL)
	{
	    printf("param is error\n");
        return -1;	
	}
	iRet=find_pid_by_name(ProcName,&foundPid);
	if(iRet < 0)
	{
		printf("find_pid_by_name is error %d\n",iRet);
        return -2;	
	}
	memset(path,0,sizeof(path));
	sprintf(path,"/proc/%d/net/dev",foundPid);
	iRet=isDeviceAccess(path);
	if(iRet < 0)
	{
		printf("path is no access %d\n",path,iRet);
        return -3;	
	}
	iRet=readOrgFile(path,data,sizeof(data),&fileSize);
	if(iRet < 0)
	{
		printf("read %s File is error %d\n",path,iRet);
        return -3;	
	}
	char *ptrStart=data;
	char *ptrEnd=NULL;
	char *ptrMaoHao=NULL;
	int  loop=0;
	unsigned long long test=0;
	while(loop<10)
	{
		ptrEnd=memchr(ptrStart,0x0A,sizeof(data)-(ptrStart-data));
		if(ptrEnd!=NULL)
		{
			ptrMaoHao=memchr(ptrStart,':',ptrEnd-ptrStart);
			if(ptrMaoHao)
			{
				memset(buffer,0,sizeof(buffer));
				memcpy(buffer,ptrStart,ptrEnd-ptrStart);
				memset(networkName,0,sizeof(networkName));
				printf("buffer is %s---\n",buffer);
				iRet = sscanf(buffer,"%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu"
						,networkName,&test,&test,&test,&test,&test,&test,&test,&test
						,&socketNum,&test,&test,&test,&test,&test,&test,&test);
				if(strstr(buffer,"eth0")!=NULL || strstr(buffer,"ra0")!=NULL)
				{
					printf("iRet=%d network=%s packetNum=%llu--\n",iRet,networkName,socketNum);
					*socketNums+=socketNum;
				}
			}
			ptrStart=ptrEnd+1;
		}
		else
		{
			break;
		}
		loop++;
	}
	return 0;
}

/*   删除左边的空格   */
static char * l_trim(char * szOutput, const char *szInput)
{
	 assert(szInput != NULL);
	 assert(szOutput != NULL);
	 assert(szOutput != szInput);
	 for(NULL; *szInput != '\0' && isspace(*szInput); ++szInput)
	 {
	  	;
	 }
	 return strcpy(szOutput, szInput);
}
 
/*   删除右边的空格   */
static char *r_trim(char *szOutput, const char *szInput)
{
	 char *p = NULL;
	 assert(szInput != NULL);
	 assert(szOutput != NULL);
	 assert(szOutput != szInput);
	 strcpy(szOutput, szInput);
	 for(p = szOutput + strlen(szOutput) - 1; p >= szOutput && isspace(*p); --p)
	 {
	  	;
	 }
	 *(++p) = '\0';
	 return szOutput;
}
 
/*   删除两边的空格   */
static char * a_trim(char * szOutput, const char * szInput)
{
	char *p = NULL;
	assert(szInput != NULL);
	assert(szOutput != NULL);
	l_trim(szOutput, szInput);
	for(p = szOutput + strlen(szOutput) - 1;p >= szOutput && isspace(*p); --p)
	{
		;
	}
	*(++p) = '\0';
	return szOutput;
}
 
//char ip[16];
//GetProfileString("./cls.conf", "cls_server", "ip", ip);
INT32S GetProfileString(char *profile, char *AppName, char *KeyName, char *KeyVal)
{
	char appname[32],keyname[32];
	char *buf,*c;
	char buf_i[KEYVALLEN], buf_o[KEYVALLEN];
	FILE *fp;
	int found=0; /* 1 AppName 2 KeyName */
	if((fp=fopen(profile,"r"))==NULL)
	{
		LOGOUT("openfile [%s] error [%s]",profile,strerror(errno));
		return	(-1);
	}
	fseek( fp, 0, SEEK_SET );
	memset( appname, 0, sizeof(appname) );
	sprintf( appname,"[%s]", AppName );

	while( !feof(fp) && fgets( buf_i, KEYVALLEN, fp )!=NULL )
	{
		l_trim(buf_o, buf_i);
		if(strlen(buf_o) <= 0)
			continue;
		buf = NULL;
		buf = buf_o;
		if( found == 0 )
		{
			if( buf[0] != '[' ) 
			{
				continue;
			} 
			else if ( strncmp(buf,appname,strlen(appname))==0)
			{
				found = 1;
				continue;
			}

		} 
		else if( found == 1 )
		{
			if( buf[0] == '#' )
			{
				continue;
			} 
			else if ( buf[0] == '[' ) 
			{
				break;
			} 
			else 
			{
				if( (c = (char*)strchr(buf, '=')) == NULL )
				 continue;
				memset( keyname, 0, sizeof(keyname) );
				sscanf( buf, "%[^=|^ |^\t]", keyname );
				if( strcmp(keyname, KeyName) == 0 )
				{
					sscanf( ++c, "%[^\n]", KeyVal );
					char *KeyVal_o = (char *)malloc(strlen(KeyVal) + 1);
					if(KeyVal_o != NULL)
					{
						memset(KeyVal_o, 0, sizeof(KeyVal_o));
						a_trim(KeyVal_o, KeyVal);
						if(KeyVal_o && strlen(KeyVal_o) > 0)
							strcpy(KeyVal, KeyVal_o);
						free(KeyVal_o);
						KeyVal_o = NULL;
					}
					found = 2;
					break;
				} 
				else 
				{
				 continue;
				}
			}
		}
	}
	fclose( fp );
	if( found == 2 )
		return(0);
	else
		return(-1);
}



