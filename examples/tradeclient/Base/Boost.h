#ifndef HK_COUNTER_BOOST_H_
#define HK_COUNTER_BOOST_H_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/locale.hpp>
#include <boost/dll.hpp> // 目前之所以需要，是因为AdapterFactory中存在没有释放的dll句柄的行为
#include <boost/utility/string_view.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/any.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/lexical_cast.hpp>
// 不要使用
// 缺陷：只能接受十进制转换，如果是0x开头，将无法支持
//#include <boost/lexical_cast.hpp>
//

namespace base
{
    // 保留以下注释代码
    // 说明：
    //     1.boost库作为std候选库存在，在最后入选标准库时，会进行细节更改，修正错误，优化使用。如果按照std库文档说明使用boost相似库，会出现错误。所以通过
    //       以下方式，对使用的boost库进行细节说明。
    //       例如：boost库（1.58及之前存在）中的“nullptr == shared_ptr的实例”会出现运行错误。因为boost库为兼容早期的编译器版本，不支持nullptr比较，但支持NULL。这两者
    //       在C++中不是同一种类型，导致表达式判断永远为false。
    //     2.随着boost库版本的提高，部分boost库代码在特定编译器版本下，直接使用标准库；仅仅将std转为boost域，保持兼容。
    //     3.部分boost库存在缺陷，其使用方式和场景需要注意。
    //     4.汇总boost库使用，方便以后使用预编译
    //     5.保留boost库具体类型使用限制，方便通过编译器发现特定代码使用，防止头文件包含的扩散。同时方便以后使用标准库替代。
    // 综合以上说明，同时保留两种方式编译。详细限制方式仅用于编译检查。

    //*/
    using namespace boost;
    /*/
	using ::boost::bind;
    using ::boost::shared_ptr;

	namespace asio
    {
		using namespace ::boost::asio;
	}
	
    namespace system
    {
		using ::boost::system::error_code;
	}
	
    namespace posix_time
    {
		using ::boost::posix_time::seconds;
	}

    namespace algorithm
    {
        using namespace ::boost::algorithm;
    }
    //*/
}

#endif