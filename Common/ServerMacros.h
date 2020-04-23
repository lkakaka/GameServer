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
#define MKDIR(dirName, mode) _mkdir(dirName)
#else
#define MKDIR(dirName, mode) mkdir(dirName, mode)
#endif //(WIN32)
