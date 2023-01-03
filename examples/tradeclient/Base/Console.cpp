// 实现
#include <cctype>
#include <cstdint>
#include <array>
#include <string>
#include <iostream>
#include <iomanip>
#include "Boost.h"
#include "Assert.h"
#include "Port.h"
#include "Console.h"
// #include "Utils.h"
#include <locale>
#include <codecvt>

#define Utils_Enabled 0

namespace base
{

Console::Console()
{
}

Console::~Console()
{
}

Console & Console::operator<<(bool val)
{
    std::cout << val;
	return *this;
}

Console & Console::operator<<(short val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(unsigned short val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(int val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(unsigned int val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(int64_t val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(uint64_t val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(float val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(double val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(long double val)
{
	std::cout << val;
	return *this;
}

Console & Console::operator<<(const char * val)
{
#if Utils_Enabled && defined(OS_WIN)
	std::cout << convertUTF8ToGBK(val);
#else
	std::cout << val;
#endif
	return *this;
}

Console & Console::operator<<(const std::string & val)
{
#if Utils_Enabled && defined(OS_WIN)
	std::cout << convertUTF8ToGBK(val);
#else
	std::cout << val;
#endif
	return *this;
}

Console & Console::operator<<(char val)
{
	std::cout << val;
	return *this;
}

bool Console::readCommand(Arguments* arguments)
{
	// 从目前所知，windows平台仅支持UCS2，不支持UTF-16。
	// 需要两个char16_t表示的字符，windows操作系统不支持。
	// Linux平台支持ucs4。
	BASE_ASSERT(nullptr != arguments);
	std::array<wchar_t, 256> buf{};
	std::wcin.getline(buf.data(), buf.size());
    std::wstring tmp = buf.data();
	std::vector<std::wstring> args;
	base::algorithm::split(args, tmp, base::algorithm::is_any_of(L" \t"), base::algorithm::token_compress_on);
    for (const auto& v : args)
    {
#if Utils_Enabled
        arguments->emplace_back(convertWideToUTF8(v));
#else
        arguments->emplace_back(v);
#endif
    }
	return true;
}

bool Console::readCommand(CommandArgument* arg)
{
    // 从目前所知，windows平台仅支持UCS2，不支持UTF-16。
    // 需要两个char16_t表示的字符，windows操作系统不支持。
    // Linux平台支持ucs4。
    BASE_ASSERT(nullptr != arg);
    std::array<wchar_t, 256> buf{};
    std::wcin.getline(buf.data(), buf.size());
    //operator<<("输出字符串:<") << (convertWideToUTF8(buf.data())) << ">\n";
    if (!parse(buf.data(), buf.size(), arg)) return false;
    //operator<<("解析成功后\n");
    //operator<<("command:") << arg->m_command << '\n';
    //operator<<("flag:\n");
    //for (const auto& p : arg->m_flags)
    //{
    //    operator<<(p.first) << "=<" << p.second << ">\n";
    //}
    //operator<<("arguments:\n");
    //for (const auto& v : arg->m_arguments)
    //{
    //    operator<<("<") << v << ">\n";
    //}
    return true;
}

int Console::system(const char * cmd)
{
#if Utils_Enabled && defined(OS_WIN)
    // You must explicitly flush—by using fflush or _flushall—or close any stream before you call system.
    std::fflush(nullptr);
	return std::system(convertUTF8ToGBK(cmd).c_str());
#else
	return std::system(cmd);
#endif
}

bool Console::parseString(const wchar_t* str, int max, int* index, std::wstring* value)
{
    m_stringState = STRING_STATE_UNINIT;
    int& i = *index;
    auto& buf = *value;

    while (true)
    {
        if (STRING_STATE_UNINIT == m_stringState)
        {
            if ('"' == str[i])
            {
                i++;
                m_stringState = STRING_STATE_NORMAL;
            }
            else
            {
                return false;
            }
        }
        else if (STRING_STATE_NORMAL == m_stringState)
        {
            if ('"' == str[i])
            {
                i++;
                return true; // 字符串解析成功
            }
            else if ('\\' == str[i])
            {
                i++;
                m_stringState = STRING_STATE_ESCAPE;
            }
            else if ('\0' == str[i])
            {
                return false;
            }
            else
            {
                buf += str[i];
                i++;
            }
        }
        else
        {
            BASE_ASSERT(STRING_STATE_ESCAPE == m_stringState);
            if ('\\' == str[i] || '"' == str[i])
            {
                buf += str[i];
                i++;
                m_stringState = STRING_STATE_NORMAL;
            }
            else
            {
                return false;
            }
        }

        if (i >= max)
        {
            operator<<("字符串解析出现问题，导致出现无限循环");
            return false;
        }
    }
}

bool Console::parse(const wchar_t* str, const int max, CommandArgument* arg)
{
    m_state = STATE_START;
    int i = 0;
    std::wstring key = L"";
    std::wstring value = L"";
    while (true)
    {
        if (STATE_START == m_state)
        {
            if (' ' == str[i] || '\t' == str[i])
            {
                i++;
            }
            else if ('\0' == str[i])
            {
                return true;
            }
            else if (0 == std::isalnum(str[i]))
            {
                return false;
            }
            else
            {
                m_state = STATE_COMMAND_START;
            }
        }
        else if (STATE_COMMAND_START == m_state)
        {
            if (' ' == str[i] || '\t' == str[i])
            {
                m_state = STATE_COMMAND_END;
                BASE_ASSERT(1 <= key.length());
#if Utils_Enabled
                arg->m_command = convertWideToUTF8(key);
#else
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                arg->m_command = converter.to_bytes(key);
#endif
                key.clear();
            }
            else if ('\0' == str[i])
            {
                BASE_ASSERT(1 <= key.length());
#if Utils_Enabled
                arg->m_command = convertWideToUTF8(key);
#else
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                arg->m_command = converter.to_bytes(key);
#endif
                return true;
            }
            else if (0 == std::isalnum(str[i]))
            {
                return false;
            }
            else
            {
                key += str[i++];
            }
        }
        else if (STATE_COMMAND_END == m_state)
        {
            if (' ' == str[i] || '\t' == str[i])
            {
                i++;
            }
            else if ('\0' == str[i])
            {
                return true;
            }
            else if ('-' == str[i])
            {
                m_state = STATE_FLAG_START;
                i++;
            }
            else
            {
                m_state = STATE_ARGUMENT_START;
            }
        }
        else if (STATE_FLAG_START == m_state)
        {
            if (' ' == str[i] || '\t' == str[i])
            {
                m_state = STATE_FLAG_END;
                BASE_ASSERT(1 <= key.length());
#if Utils_Enabled
                arg->m_flags.emplace(convertWideToUTF8(key), convertWideToUTF8(value));
#else
                arg->m_flags.emplace(key, value);
#endif
                key.clear();
                value.clear();
            }
            else if ('\0' == str[i])
            {
                return true;
            }
            else if ('=' == str[i])
            {
                m_state = STATE_FLAG_VALUE;
                i++;
            }
            else if (0 == std::isalnum(str[i]))
            {
                return false;
            }
            else
            {
                key += str[i++];
            }
        }
        else if (STATE_FLAG_VALUE == m_state)
        {
            if (' ' == str[i] || '\t' == str[i])
            {
                m_state = STATE_FLAG_END;
                BASE_ASSERT(1 <= key.length());
#if Utils_Enabled
                arg->m_flags.emplace(convertWideToUTF8(key), convertWideToUTF8(value));
#else
                arg->m_flags.emplace(key, value);
#endif
                key.clear();
                value.clear();
            }
            else if ('\0' == str[i])
            {
                BASE_ASSERT(1 <= key.length());
#if Utils_Enabled
                arg->m_flags.emplace(convertWideToUTF8(key), convertWideToUTF8(value));
#else
                arg->m_flags.emplace(key, value);
#endif
                return true;
            }
            else if ('"' == str[i])
            {
                if (!parseString(str, max, &i, &value))
                {
                    operator<<("\"\"字符串格式错误，解析无法完成");
                    return false;
                }
            }
            else
            {
                value += str[i++];
            }
        }
        else if (STATE_FLAG_END == m_state)
        {
            if (' ' == str[i] || '\t' == str[i])
            {
                i++;
            }
            else if ('\0' == str[i])
            {
                return true;
            }
            else if ('-' == str[i])
            {
                m_state = STATE_FLAG_START;
                i++;
            }
            else
            {
                m_state = STATE_ARGUMENT_START;
            }
        }
        else if (STATE_ARGUMENT_START == m_state)
        {
            if (' ' == str[i] || '\t' == str[i])
            {
                m_state = STATE_ARGUMENT_END;
                BASE_ASSERT(1 <= key.length());
#if Utils_Enabled
                arg->m_arguments.emplace_back(convertWideToUTF8(key));
#else
                arg->m_arguments.emplace_back(key);
#endif
                key.clear();
            }
            else if ('\0' == str[i])
            {
                BASE_ASSERT(1 <= key.length());
#if Utils_Enabled
                arg->m_arguments.emplace_back(convertWideToUTF8(key));
#else
                arg->m_arguments.emplace_back(key);
#endif
                return true;
            }
            else if ('"' == str[i])
            {
                if (!parseString(str, max, &i, &key))
                {
                    operator<<("\"\"字符串格式错误，解析无法完成");
                    return false;
                }
            }
            else
            {
                key += str[i++];
            }
        }
        else if (STATE_ARGUMENT_END == m_state)
        {
            if (' ' == str[i] || '\t' == str[i])
            {
                i++;
            }
            else if ('\0' == str[i])
            {
                return true;
            }
            else
            {
                m_state = STATE_ARGUMENT_START;
            }
        }
        else
        {
            BASE_ASSERT(false);
            return false;
        }

        if (i >= max)
        {
            operator<<("命令行解析出现问题,导致无限循环");
            return false;
        }
    }
}

}
