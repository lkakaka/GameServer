#include "StrUtil.h"

std::vector<std::string> StrUtil::split(char* str, const char* delimiters) {
    std::vector<std::string> vec;
    char* strc = new char[strlen(str) + 1];
    strcpy(strc, str);
    char* pch = strtok(str, delimiters);
    while (pch != NULL)
    {
        //printf("%s\n", pch);
        vec.push_back(std::string(pch));
        pch = strtok(NULL, delimiters);
    }
    delete[] strc;
    return vec;
}