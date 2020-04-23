#pragma once

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif //(WIN32)

#ifdef WIN32
#define M_ACCESS(fileName, mode) _access(fileName, mode)
#define M_MKDIR(dirName, mode) _mkdir(dirName)
#else
#define M_MKDIR(dirName, mode) mkdir(dirName, mode)
#define M_ACCESS(fileName, mode) access(fileName, mode)
#endif //(WIN32)
