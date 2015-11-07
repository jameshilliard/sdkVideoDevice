#ifndef _GETPID_H
#define _GETPID_H
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

pid_t GetPIDbyName_Wrapper(const char* cchrptr_ProcessName);





#endif

