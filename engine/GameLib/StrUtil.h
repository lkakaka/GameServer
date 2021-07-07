#pragma once
#include "../Common/ServerExports.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <string.h>

class SERVER_EXPORT_API StrUtil
{
public:
	static std::vector<std::string> split(char* str, const char* delimiters);
};

// string_cast<int>("12345")
// string_cast<double>("12345.78")
template <typename T>
class string_cast
{
public:
    string_cast(const std::string& from) : m_from(from) {}
    operator T() const {
        std::stringstream sstr(m_from);
        T ret;
        try {
            sstr >> ret;
        }
        catch (std::exception& e) {
            return T(0);
        }
        return ret;
    }
private:
    const std::string& m_from;
};

