///@brief MY::Base::Common库的相关宏定义
///@author lpx
///@date 2015-06-12

#ifndef MY_COMMON_LIB_DEFINES_H
#define MY_COMMON_LIB_DEFINES_H

#ifdef _WIN32
    #pragma warning(disable : 4251)

    #ifdef MY_Base_Common_STATIC
        #define MY_Base_Common_API
    #elif defined MY_Base_Common_DYNAMIC
        #define MY_Base_Common_API __declspec(dllexport)
    #else
        #define MY_Base_Common_API __declspec(dllimport)
    #endif
#else //linux platform
    #define MY_Base_Common_API
#endif


#ifdef _WIN32
    #define MY_likely(x)   x
    #define MY_unlikely(x) x
    #define __MY_FUNC__  __FUNCSIG__
#else
    //减少指令跳转带来的性能影响
    #define MY_likely(x)   __builtin_expect(!!(x), 1)
    #define MY_unlikely(x) __builtin_expect(!!(x), 0)
    #define __MY_FUNC__  __PRETTY_FUNCTION__
#endif

namespace MY { 
namespace Base{
namespace Common{
    void MY_Base_Common_API commonLibInit(const char* config=nullptr);
    void MY_Base_Common_API commonLibUninit();
}  //end namespace Common
}  //end namespace Base
}  //end namespace MY

#endif
