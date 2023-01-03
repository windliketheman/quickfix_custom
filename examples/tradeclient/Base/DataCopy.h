// 声明
#ifndef COUNTER_BASE_DATA_COPY_H_
#define COUNTER_BASE_DATA_COPY_H_

#include <type_traits>
#include <array>
#include <cstring>
#include "Assert.h"

// 增强型数据拷贝（做了类型一致性检查和字符串长度匹配)
// 主要防止隐式类型转换和字符串数组长度不匹配
// 目前主要应用的场景是服务器中业务字段拷贝
namespace base
{
template<typename S, typename D>
struct DataCopy
/* 暂且注释，兼容linux编译 {
    static void run(const S& src, D* dst)
    {
        static_assert(false, "不支持这两种类型的拷贝");
    }
}*/;

template<typename T>
struct DataCopy<T, T>
{
    static void run(const T& src, T* dst)
    {
        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_enum<T>::value, "仅支持普通的native类型");
        *dst = src;
    }
};

template<size_t N>
struct DataCopy<char[N], char[N]>
{
    static void run(const char(&src)[N], char(*dst)[N])
    {
        BASE_ASSERT(N > std::strlen(src));
        std::strncpy(*dst, src, N - 1);
        (*dst)[N - 1] = '\0';
    }
};

template<size_t M, size_t N>
struct DataCopy<char[M], char[N]>
{
    static void run(const char(&src)[M], char(*dst)[N])
    {
        static_assert(M < N, "源长度必须小于目的长度.如果确实需要,请将源类型强制转换为const char*赋值");
        BASE_ASSERT(M > std::strlen(src));
        std::strncpy(*dst, src, M - 1);
        (*dst)[M - 1] = '\0';
    }
};

template<size_t N>
struct DataCopy<std::array<char, N>, char[N]>
{
    static void run(const std::array<char, N>& src, char(*dst)[N])
    {
        BASE_ASSERT(N > std::strlen(src.data()));
        std::strncpy(*dst, src.data(), N - 1);
        (*dst)[N - 1] = '\0';
    }
};

template<size_t M, size_t N>
struct DataCopy<std::array<char, M>, char[N]>
{
    static void run(const std::array<char, M>& src, char(*dst)[N])
    {
        static_assert(M < N, "源长度必须小于目的长度.如果确实需要,请将源类型强制转换为const char*赋值");
        BASE_ASSERT(M > std::strlen(src.data()));
        std::strncpy(*dst, src.data(), M - 1);
        (*dst)[M - 1] = '\0';
    }
};

template<size_t N>
struct DataCopy<std::string, char[N]>
{
    static void run(const std::string& src, char(*dst)[N])
    {
        if (N > src.length())
        {
            std::strcpy(*dst, src.c_str());
        }
        else
        {
            std::strncpy(*dst, src.c_str(), N - 1);
            (*dst)[N - 1] = '\0';
        }
    }
};

template<size_t N>
struct DataCopy<char const*, char[N]>
{
    static void run(char const* const& src, char(*dst)[N])
    {
        std::strncpy(*dst, src, N - 1);
        (*dst)[N - 1] = '\0';
    }
};

template<size_t N>
struct DataCopy<char*, char[N]>
{
    static void run(char const* const& src, char(*dst)[N])
    {
        std::strncpy(*dst, src, N - 1);
        (*dst)[N - 1] = '\0';
    }
};

template<size_t N>
struct DataCopy<char[N], std::string>
{
    static void run(const char(&src)[N], std::string* dst)
    {
        BASE_ASSERT(N > std::strlen(src));
        *dst = src;
    }
};

template<size_t M>
struct DataCopy<std::array<char, M>, std::string>
{
    static void run(const std::array<char, M>& src, std::string* dst)
    {
        BASE_ASSERT(M > std::strlen(src.data()));
        *dst = src;
    }
};

template<>
struct DataCopy<char*, std::string>
{
    static void run(char const* const& src, std::string* dst)
    {
        *dst = src;
    }
};

template<>
struct DataCopy<char const*, std::string>
{
    static void run(char const* const& src, std::string* dst)
    {
        *dst = src;
    }
};

template<>
struct DataCopy<std::string, std::string>
{
    static void run(const std::string& src, std::string* dst)
    {
        *dst = src;
    }
};

template<typename S, typename D>
void copyTo(const S& s, D* d)
{
    BASE_ASSERT(nullptr != d);
    DataCopy<S, D>::run(s, d);
}
}

#endif
