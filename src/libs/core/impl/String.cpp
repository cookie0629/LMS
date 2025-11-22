#include "core/String.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace lms::core::stringUtils
{
    std::vector<std::string_view> splitString(std::string_view str, char separator)
    {
        std::vector<std::string_view> result;
        
        if (str.empty())
            return result;
        
        size_t start = 0;
        size_t pos = 0;
        
        while ((pos = str.find(separator, start)) != std::string_view::npos)
        {
            if (pos > start)
            {
                result.push_back(str.substr(start, pos - start));
            }
            start = pos + 1;
        }
        
        // 添加最后一部分
        if (start < str.length())
        {
            result.push_back(str.substr(start));
        }
        else if (str.back() == separator)
        {
            // 如果以分隔符结尾，添加空字符串
            result.push_back(str.substr(str.length()));
        }
        
        return result;
    }

    std::vector<std::string_view> splitString(std::string_view str, std::string_view separator)
    {
        std::vector<std::string_view> result;
        
        if (str.empty())
            return result;
        
        if (separator.empty())
        {
            // 如果分隔符为空，返回原字符串
            result.push_back(str);
            return result;
        }
        
        size_t start = 0;
        size_t pos = 0;
        
        while ((pos = str.find(separator, start)) != std::string_view::npos)
        {
            if (pos > start)
            {
                result.push_back(str.substr(start, pos - start));
            }
            start = pos + separator.length();
        }
        
        // 添加最后一部分
        if (start < str.length())
        {
            result.push_back(str.substr(start));
        }
        else if (str.length() >= separator.length() && 
                 str.substr(str.length() - separator.length()) == separator)
        {
            // 如果以分隔符结尾，添加空字符串
            result.push_back(str.substr(str.length()));
        }
        
        return result;
    }

    std::string joinStrings(std::span<const std::string> strings, std::string_view delimiter)
    {
        if (strings.empty())
            return "";
        
        std::ostringstream oss;
        bool first = true;
        
        for (const auto& str : strings)
        {
            if (!first)
                oss << delimiter;
            oss << str;
            first = false;
        }
        
        return oss.str();
    }

    std::string joinStrings(std::span<const std::string> strings, char delimiter)
    {
        if (strings.empty())
            return "";
        
        std::ostringstream oss;
        bool first = true;
        
        for (const auto& str : strings)
        {
            if (!first)
                oss << delimiter;
            oss << str;
            first = false;
        }
        
        return oss.str();
    }

    std::string_view stringTrimStart(std::string_view str, std::string_view whitespaces)
    {
        size_t start = 0;
        while (start < str.length() && whitespaces.find(str[start]) != std::string_view::npos)
        {
            ++start;
        }
        return str.substr(start);
    }

    std::string_view stringTrimEnd(std::string_view str, std::string_view whitespaces)
    {
        size_t end = str.length();
        while (end > 0 && whitespaces.find(str[end - 1]) != std::string_view::npos)
        {
            --end;
        }
        return str.substr(0, end);
    }

    std::string_view stringTrim(std::string_view str, std::string_view whitespaces)
    {
        return stringTrimStart(stringTrimEnd(str, whitespaces), whitespaces);
    }

    std::string stringToLower(std::string_view str)
    {
        std::string result;
        result.reserve(str.length());
        
        for (char c : str)
        {
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        
        return result;
    }

    void stringToLower(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    }

    std::string stringToUpper(std::string_view str)
    {
        std::string result;
        result.reserve(str.length());
        
        for (char c : str)
        {
            result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        
        return result;
    }

    void stringToUpper(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    }

    bool stringCaseInsensitiveEqual(std::string_view strA, std::string_view strB)
    {
        if (strA.length() != strB.length())
            return false;
        
        for (size_t i = 0; i < strA.length(); ++i)
        {
            if (std::tolower(static_cast<unsigned char>(strA[i])) != 
                std::tolower(static_cast<unsigned char>(strB[i])))
            {
                return false;
            }
        }
        
        return true;
    }

    bool stringCaseInsensitiveContains(std::string_view str, std::string_view substr)
    {
        if (substr.empty())
            return true;
        
        if (substr.length() > str.length())
            return false;
        
        for (size_t i = 0; i <= str.length() - substr.length(); ++i)
        {
            bool match = true;
            for (size_t j = 0; j < substr.length(); ++j)
            {
                if (std::tolower(static_cast<unsigned char>(str[i + j])) != 
                    std::tolower(static_cast<unsigned char>(substr[j])))
                {
                    match = false;
                    break;
                }
            }
            if (match)
                return true;
        }
        
        return false;
    }

    void capitalize(std::string& str)
    {
        if (str.empty())
            return;
        
        str[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(str[0])));
        
        for (size_t i = 1; i < str.length(); ++i)
        {
            str[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(str[i])));
        }
    }
} // namespace lms::core::stringUtils

