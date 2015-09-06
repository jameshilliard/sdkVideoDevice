#include "GlobFunc.h"

const char b64_alphabet[65] = { 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/=" };

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

	LOGOUT("v_szReadBuf = %s", v_szReadBuf);
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
		totalspace = (((INT64S)disk_statfs.f_bsize * (INT64S)disk_statfs.f_blocks) /(INT64S)1024);
	}else
	{
		return -1;
	}
	if( totalspace < size)
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

INT32S readDirFileNum(const char *pszDir)
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
    while((ptr = readdir(dir)) != NULL) ///read the list of this dir
    {
		iRet++;
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
					iRet=2;
				}
				if(strstr(ptr->d_name,"dat"))
				{
					iRet=3;
				}
				if(iRet==1 || iRet==2 || iRet==3)
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


