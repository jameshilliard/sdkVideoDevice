#ifndef __GLOBAFUNC_H_
#define __GLOBAFUNC_H_

#include "Typedef.h"
#include "../LogOut/LogOut.h"

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	DWORD 	getTickCountMs();
	void 	squeeze(char *s,int maxSize,int c);
	DWORD	getFileSize(LPCTSTR path);
	BOOL   	isFileSystemBigger(LPCTSTR sdDir,DWORD size);
	BOOL 	creatDir(LPCTSTR pDir);
	INT32S 	writeFile(LPCTSTR filePath,LPCTSTR fileBuffer,DWORD size);
	INT32S 	readFile(LPCTSTR filePath,LPCTSTR fileBuffer,DWORD bufferSize,DWORD *fileSize);
	char * 	SY_base64Encode(const char *text);
#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif
