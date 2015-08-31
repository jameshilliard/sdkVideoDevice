#ifndef HTTPClient_H_
#define HTTPClient_H_
#include <map>
#include <string>
#include <vector>
using namespace std;

class CHTTPClient
{
public:
    CHTTPClient(void);
    ~CHTTPClient(void);
public:
	bool HttpPost(const std::string & strUrl,char* buf,int length,std::map<string,string>& _ResonseHead,std::vector<char>& vcResponse);
	bool HttpPost(const std::string & strUrl,char* buf,int length,std::string& strResult)  ;
	bool HttpGet(const std::string & strUrl,std::string& strResult,std::string& password);
	int GetPostConnectLength(char* pBuf,unsigned int nLen);
private:
	int base64_encode(const char *in_str, int in_len, char *out_str);
	int base64_decode(const char *in_str, int in_len, char *out_str);
};


#endif