//20091231	bcb	Changed char* to const char* for gcc4.2 warnings
//20100111	mvh	Merged
//20100124	mvh	Renamed to MyGetPrivateProfileString used for windows and linux
//20110331	mvh	Added FlushPrivateProfileStringCache

// MyGetPrivateProfileString: much faster version than GetPrivateProfileString

#ifndef GPPS_HPP
#define GPPS_HPP
#ifdef UNIX
#include <sys/types.h>
#include "wintypes.hpp"
#else
#include "windows.h"
#endif

DWORD
MyGetPrivateProfileString(const char *theSection,   // section name
    const char *theKey,                           // search key name
    const char *theDefault,                       // default value if not found
    char *theReturnBuffer,                  // return valuse stored here
    size_t theBufferLength,                 // byte length of return buffer
    char *theIniFileName);                  // pathname of ini file to search

void FlushPrivateProfileStringCache(void);

#endif
