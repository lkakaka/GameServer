#pragma once

#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define PLATHFORM_WIN
#else
#define PLATHFORM_UNIX
#endif
