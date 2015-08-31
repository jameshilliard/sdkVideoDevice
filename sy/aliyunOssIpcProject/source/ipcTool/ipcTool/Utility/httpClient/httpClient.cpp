#include "stdafx.h"
#include "httpclient.h"  
#include "urlParser.hpp"
#include <WinSock2.h>
#include <vector>

using namespace std;
#pragma comment(lib,"ws2_32.lib") 
#include <stdio.h>
#define  HTTPVESION "HTTP/1.1"
#define  POST       "POST"
#define  GET        "GET"
#define  PUT        "PUT"
#define  DELETE        "DELETE"
#define  SENDLEN     1460
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))




CHTTPClient::CHTTPClient(void)  
{  

}  

CHTTPClient::~CHTTPClient(void)  
{  

}  
void OnRead(char* buf,int nlength,int socket);
bool CHTTPClient::HttpPost(const std::string & strUrl,char* buf,int length,std::map<string,string>& _ResonseHead,std::vector<char>& vcResponse)  
{    
	urlparser surlparser(strUrl);
	std::string sIp = surlparser.getdomain();
	unsigned int nPort = atoi(surlparser.getport().c_str());
	std::string spath = surlparser.getPathUrl();
	WSADATA	WsaData;
	WSAStartup (0x0101, &WsaData);
	sockaddr_in       sin;
	int sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		return false;
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons( (unsigned short)nPort);

	struct hostent * host_addr = gethostbyname(sIp.c_str());
	if(host_addr==NULL) {
		return false;
	}
	sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list) ;

	if( connect (sock,(const struct sockaddr *)&sin, sizeof(sockaddr_in) ) == -1 ) {
		return false;
	}

	char sendbuf[2048];
	char buftmp[1024];
	ZeroMemory(sendbuf,2048);
	sprintf(sendbuf,"%s %s %s\r\n",POST,spath.c_str(),HTTPVESION);


	sprintf(buftmp,"%s %d\r\n","Content-Length:",length);
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s:%d\r\n","Host:",sIp.c_str(),nPort);
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s\r\n","User-Agent:","dwk");
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s\r\n","Accept:","*/*");
	strcat(sendbuf,buftmp);

	strcat(sendbuf,"\r\n");
	//发送http 的头
	int sendret = send(sock,sendbuf,strlen(sendbuf),0);
	if (sendbuf <= 0)
	{
		return false;
	}
	int poslen = 0;
	sendret = send(sock,buf,length,0);
	char c1[1];
	int l,line_length;
	bool loop = true;
	bool bHeader = false;
	std::string message;
	while(loop) {
		l = recv(sock, c1, 1, 0);
		if(l<0) loop = false;
		if(c1[0]=='\n') {
			if(line_length == 0) 
			{
				loop = false;
				break;
			}
			line_length = 0;
			if(message.find(HTTPVESION) != string::npos)
			{
				std::string svalue = (char*)memchr(message.c_str(),' ',message.length());
				svalue = svalue.substr(1,3);
				_ResonseHead[HTTPVESION] =  svalue;
			}
			else
			{
				std::string svalue = (char*)memchr(message.c_str(),' ',message.length());
				svalue = svalue.substr(1,svalue.length() -1);
				std::string skey = strtok ((char*)message.c_str(),":");
				_ResonseHead[skey] =  svalue;
			}
			message.clear();
		}
		else if(c1[0]!='\r')
		{
			line_length++;	
			message += c1[0];
		}
	}
	length = atoi(_ResonseHead["Content-Length"].c_str());
	vcResponse.clear();	
	if (length > 0)
	{
		vcResponse.assign(length+1,'\0');
		l = recv(sock, (char*)&vcResponse[0], length, 0);
		if (l != length)
		{
			return false;
		}
		return true;
	}
	return true;
}  
bool CHTTPClient::HttpPost(const std::string & strUrl,char* buf,int length,std::string& strResult)  
{    
	std::vector<char> vcResponse;
	std::map<string,string> _ResonseHead;
	urlparser surlparser(strUrl);
	std::string sIp = surlparser.getdomain();
	unsigned int nPort = atoi(surlparser.getport().c_str());
	std::string spath = surlparser.getPathUrl();
	WSADATA	WsaData;
	WSAStartup (0x0101, &WsaData);
	sockaddr_in       sin;
	int sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		return false;
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons( (unsigned short)nPort);

	struct hostent * host_addr = gethostbyname(sIp.c_str());
	if(host_addr==NULL) {
		return false;
	}
	sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list) ;

	if( connect (sock,(const struct sockaddr *)&sin, sizeof(sockaddr_in) ) == -1 ) {
		return false;
	}

	char sendbuf[2048];
	char buftmp[1024];
	ZeroMemory(sendbuf,2048);
	sprintf(sendbuf,"%s %s %s\r\n",POST,spath.c_str(),HTTPVESION);


	sprintf(buftmp,"%s %d\r\n","Content-Length:",length);
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s:%d\r\n","Host:",sIp.c_str(),nPort);
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s\r\n","User-Agent:","dwk");
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s\r\n","Content-Type:","application/x-www-form-urlencoded");
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s\r\n","Accept:","*/*");
	strcat(sendbuf,buftmp);

	strcat(sendbuf,"\r\n");
	//发送http 的头
	int sendret = send(sock,sendbuf,strlen(sendbuf),0);
	if (sendbuf <= 0)
	{
		return false;
	}
	int poslen = 0;
	if (length > 0)
	{
		sendret = send(sock,buf,length,0);
	}
	char c1[1];
	int l,line_length;
	bool loop = true;
	bool bHeader = false;
	std::string message;
	while(loop) {
		l = recv(sock, c1, 1, 0);
		if(l<0) loop = false;
		if(c1[0]=='\n') {
			if(line_length == 0) 
			{
				loop = false;
				break;
			}
			line_length = 0;
			if(message.find(HTTPVESION) != string::npos)
			{
				std::string svalue = (char*)memchr(message.c_str(),' ',message.length());
				svalue = svalue.substr(1,3);
				_ResonseHead[HTTPVESION] =  svalue;
			}
			else
			{
				std::string svalue = (char*)memchr(message.c_str(),' ',message.length());
				svalue = svalue.substr(1,svalue.length() -1);
				std::string skey = strtok ((char*)message.c_str(),":");
				_ResonseHead[skey] =  svalue;
			}
			message.clear();
		}
		else if(c1[0]!='\r')
		{
			line_length++;	
			message += c1[0];
		}
	}
	length = atoi(_ResonseHead["Content-Length"].c_str());
	vcResponse.clear();	
	if (length > 0)
	{
		vcResponse.assign(length+1,'\0');
		l = recv(sock, (char*)&vcResponse[0], length, 0);
		if (l != length)
		{
			return false;
		}
		strResult = (char*)&vcResponse[0];
		return true;
	}
	return true;
}
bool CHTTPClient::HttpGet(const std::string & strUrl,std::string& strResult,std::string& password)  
{    
	std::map<string,string> _ResonseHead;
	std::vector<char> vcResponse;
	urlparser surlparser(strUrl);
	std::string sIp = surlparser.getdomain();
	UINT nPort = atoi(surlparser.getport().c_str());
	std::string spath = surlparser.getPathUrl();
	WSADATA	WsaData;
	WSAStartup (0x0101, &WsaData);
	sockaddr_in       sin;
	int sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		return false;
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons( (unsigned short)nPort);

	struct hostent * host_addr = gethostbyname(sIp.c_str());
	if(host_addr==NULL) {
		return false;
	}
	sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list) ;

	if( connect (sock,(const struct sockaddr *)&sin, sizeof(sockaddr_in) ) == -1 ) {
		return false;
	}

	char sendbuf[2048];
	char buftmp[1024];
	ZeroMemory(sendbuf,2048);
	sprintf(sendbuf,"%s %s %s\r\n",GET,spath.c_str(),HTTPVESION);

	sprintf(buftmp,"%s %s\r\n","Accept:"," text/html, application/xhtml+xml, */*");
	strcat(sendbuf,buftmp);
	
	sprintf(buftmp,"%s %s\r\n","User-Agent:"," dwk");
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s:%d\r\n","Host:",sIp.c_str(),nPort);
	strcat(sendbuf,buftmp);

	sprintf(buftmp,"%s %s\r\n","Connection:"," Keep-Alive");
	strcat(sendbuf,buftmp);

	if(!password.empty())
	{
		char passwordString[4096]={0,};
		base64_encode(password.c_str(),password.length(),passwordString);
		sprintf(buftmp,"%s %s\r\n","Authorization: Basic",passwordString);
		strcat(sendbuf,buftmp);
	}
	//strcat(sendbuf,m_sHead.c_str());
	strcat(sendbuf,"\r\n");
	//发送http 的头
	int sendret = send(sock,sendbuf,strlen(sendbuf),0);
	if (sendbuf <= 0)
	{
		return false;
	}
	char c1[1];
	int l,line_length;
	bool loop = true;
	bool bHeader = false;
	std::string message;
	while(loop) {
		l = recv(sock, c1, 1, 0);
		if(l<0) loop = false;
		if(c1[0]=='\n') {
			if(line_length == 0) 
			{
				loop = false;
				break;
			}
			line_length = 0;
			if(message.find(HTTPVESION) != string::npos)
			{
				char *ptrChar = (char*)memchr(message.c_str(),' ',message.length());
				if(ptrChar!=NULL)
				{
					std::string svalue(ptrChar);
					if(!svalue.empty())
					{
						svalue = svalue.substr(1,3);
						_ResonseHead[HTTPVESION] =  svalue;
					}	
				}

			}
			else
			{
				char *ptrChar  = (char*)memchr(message.c_str(),':',message.length());
				if(ptrChar!=NULL)
				{
					std::string svalue(ptrChar);
					if(!svalue.empty())
					{
						svalue = svalue.substr(1,svalue.length() -1);
						std::string skey = strtok ((char*)message.c_str(),":");
						_ResonseHead[skey] =  svalue;
					}
				}
			}
			message.clear();
		}
		else if(c1[0]!='\r')
		{
			line_length++;	
			message += c1[0];
		}

	}
	std::string strkeylen = "Content-Length";
	map<std::string,std::string>::iterator it = _ResonseHead.find(strkeylen.c_str());
	std::string strlen = _ResonseHead[strkeylen];
	int length = atoi(strlen.c_str());
	vcResponse.clear();	
	if (length > 0)
	{
		vcResponse.assign(length+1,'\0');
		l = recv(sock, (char*)&vcResponse[0], length, 0);
		if (l != length)
		{
			return false;
		}
	}
	strkeylen = "Transfer-Encoding";
	it = _ResonseHead.find(strkeylen.c_str());
	if (it != _ResonseHead.end())		
	{
		loop = true;
		while(loop) {
			l = recv(sock, c1, 1, 0);
			if(l<0) loop = false;

			if(c1[0]=='\n') {
				int len = 0;
				sscanf(message.c_str(),"%x",&len);
				if (len > 0 )
				{
					vcResponse.assign(len+1,'\0');
					l = recv(sock, (char*)&vcResponse[0], len, 0);
				}
				loop = false;
			}
			else if(c1[0]!='\r')
			{
				line_length++;	
				message += c1[0];
			}
		}

	}
	vcResponse.push_back('\0');
	if (vcResponse.size() > 0 )
	{
		strResult = &vcResponse[0];
	}
	return true;
}  
void OnRead(char* buf,int nlength,int socket)
{

}
int CHTTPClient::GetPostConnectLength(char* pBuf,unsigned int nLen)
{
	if (pBuf != NULL)
	{
		char* pConnectLenth = strstr(pBuf,"Content-Length: ");
		if (pConnectLenth != NULL)
		{
			pConnectLenth =  strtok(pConnectLenth,"\r\n");
			if (pConnectLenth != NULL)
			{
				int nCntentlen = strlen("Content-Length: ");
				if (strlen(pConnectLenth) > nCntentlen)
				{
					return atoi(pConnectLenth + nCntentlen);
				}	
			}
		}
	}
	return 0;
}

const char  *_base64_encode_chars = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const signed char _base64_decode_chars[] = 
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};


/*******************************************************************************
*         Name: base64_encode 
*  Description: BASE64加密算法实现  
* 
*       Mode   Type         Name         Description
* -----------------------------------------------------------------------------
*        in:   char *      in_str      原始输入数据
*              int         in_len      输入数据长度 
*    in-out:   无 
*       out:   char *      out_str     加密结果输出
*    return:   int                     加密结果长度 
* -----------------------------------------------------------------------------
*    Author: xmq
*   Created: 01/02/2008 00 CST
*  Revision: none
******************************************************************************/
int CHTTPClient::base64_encode(const char *in_str, int in_len, char *out_str)
{
	unsigned char c1, c2, c3;
	int     i = 0;
	int     index = 0;

	if((NULL == in_str) || (NULL == out_str))
	{
		return (-1);
	}

	while(i < in_len)
	{
		/* read the first byte */
		c1 = in_str[i++];
		if(i == in_len)         /* pad with "="*/
		{
			out_str[index++] = _base64_encode_chars[ c1>>2 ];
			out_str[index++] = _base64_encode_chars[ (c1&0x3)<<4 ];
			out_str[index++] = '=';
			out_str[index++] = '=';
			break;
		}

		/* read the second byte */
		c2 = in_str[i++];
		if(i == in_len)        /* pad with "=" */
		{
			out_str[index++] = _base64_encode_chars[ c1>>2 ];
			out_str[index++] = _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
			out_str[index++] = _base64_encode_chars[ (c2&0xF)<<2 ];
			out_str[index++] = '=';
			break;
		}

		/* read the third byte */
		c3 = in_str[i++];

		/* convert into four bytes string */
		out_str[index++] = _base64_encode_chars[c1 >> 2];
		out_str[index++] = _base64_encode_chars[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
		out_str[index++] = _base64_encode_chars[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];
		out_str[index++] = _base64_encode_chars[c3 & 0x3F];
	}

	return index;
}


/*******************************************************************************
*         Name: base64_decode 
*  Description: BASE64解码算法
* 
*       Mode   Type         Name         Description
* -----------------------------------------------------------------------------
*        in:   char *      in_str      输入原始数据
*              int         in_len      输入数据长度 
*    in-out:   无 
*       out:   char *      out_str     解密数据输出
*    return:   int                     解密输出数据长度
* -----------------------------------------------------------------------------
*    Author: xmq
*   Created: 01/02/2008 00 CST
*  Revision: none
******************************************************************************/
int CHTTPClient::base64_decode(const char *in_str, int in_len, char *out_str)
{
	signed char c1, c2, c3, c4;
	int     i = 0;
	int     index = 0;

	while(i < in_len)
	{
		/* read the first byte */
		do
		{
			c1 = _base64_decode_chars[(int)in_str[i++]];
		} while((i < in_len) && (c1 == -1));

		if(c1 == -1)
		{
			break;
		}

		/* read the second byte */
		do
		{
			c2 = _base64_decode_chars[(int)in_str[i++]];
		} while((i < in_len) && (c2 == -1));

		if(c2 == -1)
		{
			break;
		}

		/* assamble the first byte */
		out_str[index++] = (char)((c1 << 2) | ((c2 & 0x30) >> 4));

		/* read the third byte */
		do
		{
			c3 = in_str[i++];
			if(c3 == 61)        /* meet with "=", break */
			{
				return index;
			}

			c3 = _base64_decode_chars[(int)c3];
		} while((i < in_len) && (c3 == -1));

		if(c3 == -1)
		{
			break;
		}

		/* assabmel the second byte */
		out_str[index++] = (char)(((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2));

		/* read the fourth byte */
		do
		{
			c4 = in_str[i++];
			if(c4 == 61)        /* meet with "=", break */
			{
				return index;
			}

			c4 = _base64_decode_chars[(int)c4];
		} while((i < in_len) && (c4 == -1));

		if(c4 == -1)
		{
			break;
		}

		/* assamble the third byte */
		out_str[index++] = (char)(((c3 & 0x03) << 6) | c4);
	}

	return index;
}


