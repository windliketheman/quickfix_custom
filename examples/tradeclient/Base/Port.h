// 声明
#ifndef COUNTER_BASE_PORT_H_
#define COUNTER_BASE_PORT_H_

#include <boost/predef/os.h>

#if defined(BOOST_OS_WINDOWS_AVAILABLE)

#define OS_WIN

#elif defined(BOOST_OS_LINUX_AVAILABLE)

#define OS_LINUX

#else

#error 未支持的操作系统平台

#endif

#endif