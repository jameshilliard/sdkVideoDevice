#ifndef _LOGOUT_H_
#define _LOGOUT_H_
#include "../Common/Typedef.h"
#include "../Common/GlobFunc.h"

#define MAX_LOGFILE		1	// 日志等级个数
#define LOGFILEDIR		"SDKLog"
#define MAXBACKNUM 		3	// 日志文件备份个数

// 等级越大优先级越大
enum
{
	LOG_LV1 = 0,
	LOG_LV2 = 1
};

//#define LOGOUTDEBUG

#ifdef LOGOUTDEBUG
#define LOGOUT
#define LOGOUT_CDN
#else
#define LOGOUT(format, ...) \
{ \
	LogOutPut(LOG_LV1, __FILE__,  __LINE__, format, ##__VA_ARGS__); \
}
#define LOGOUT_CDN(log_lv,format, ...)\
{\
	LogOutPut(log_lv, __FILE__,  __LINE__, format, ##__VA_ARGS__); \
}
#endif

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

	void Init_LogOut(INT32U v_iLogSize,LPCTSTR v_szFlushPath, BOOL v_bOutToSerial,LPCTSTR v_szTempPath);
	void Release_LogOut();
	void LogOutPut(INT32U v_iLv,LPCTSTR FileName,INT32U FileLine,LPCTSTR msg, ...);
	void TarLogFile(LPTSTR v_szOutPath);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif