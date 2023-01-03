///@brief 日期类
///@author lpx
///@date 2015-06-12

#ifndef MY_UTC_H
#define MY_UTC_H

#include "CommonLibDefines.h"

#include <string>
#include <cstdint>

namespace MY { 
namespace Base{
namespace Common{

class MY_Base_Common_API UTC
{
public:
    //自1970年1月1日零点起至伦敦当前时间的微秒数(us)
    explicit UTC(int64_t v);

    //UTC时间的字符串形式，格式为"YYYY-MM-DD HH:MM:SS.xxxxxx"
    explicit UTC(const std::string& str);

    //取当前UTC时间
	UTC(void);

    //重新获取当前时间
    UTC& now(void);
    
    //UTC时间转字符串,格式为"YYYY-MM-DD HH:MM:SS.xxxxxx"
    std::string toString(void);

    //UTC时间转北京时间的字符串,格式为"YYYY-MM-DD HH:MM:SS.xxxxxx"
    std::string toBeiJing(void);

    //UTC时间转北京时间的字符串,格式为"YYYYMMDDHHMMSS"
    std::string toBeiJing1(void);

    //UTC时间转北京时间的长整数,格式为"YYYYMMDDHHMMSSXXX"
    int64_t toBeiJing2(void);

    //UTC时间转北京时间的字符串,格式为"YYYYMMDD HHMMSS.xxxxxx"
    std::string toBeiJing3(void);

    static int init();
    static double getNanoSecs();

public:
    int64_t m_val;       //自1970年1月1日零点起至伦敦当前时间的微秒数(us)
};

}  //end namespace Common
}  //end namespace Base
}  //end namespace MY

#endif
