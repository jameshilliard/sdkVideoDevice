#include "GlobFunc.h"

const char b64_alphabet[65] = { 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/=" };

	
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
	DWORD filesize = -1;        
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
BOOL isFileSystemBigger(LPCTSTR sdDir,DWORD size) 
{
	struct statfs disk_statfs;
	DWORD  totalspace = 0;
	if(statfs(sdDir, &disk_statfs) >= 0 )
	{
		totalspace = (((INT64S)disk_statfs.f_bsize * (INT64S)disk_statfs.f_blocks) /(INT64S)1024);
	}else
	{
		return FALSE;
	}
	if( totalspace < size)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL creatDir(LPCTSTR pDir)  
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


