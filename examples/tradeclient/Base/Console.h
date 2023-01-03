// 声明
#ifndef HK_COUNTER_BASE_CONSOLE_H_
#define HK_COUNTER_BASE_CONSOLE_H_

#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "Macros.h"
#include "Command.h"

// 特殊字符 '空格0x20' 'Tab\t' '\0'
// 空格或者tab键自动过滤或压缩，并对字符串进行分割
// command [flags] [argumengs]
// command 可以为空；如果非空，则必须是数字或者字母，符合isalnum
// 如果flags表明bool类型，那么形式为-flag；如果flags表明其它类型，那么形式为-flag=xxx。所有flags必须在arguments之前，否则会当作arguments
// argumengs仅表示不需要字符‘-’开头的参数，可以有多个；与flags没有本质区别。

namespace base
{
class Console
{
public:
	Console();
	~Console();

	Console& operator<<(bool val);
	Console& operator<<(short val);
	Console& operator<<(unsigned short val);
	Console& operator<<(int val);
	Console& operator<<(unsigned int val);
	Console& operator<<(int64_t val);
	Console& operator<<(uint64_t val);
	Console& operator<<(float val);
	Console& operator<<(double val);
	Console& operator<<(long double val);
	Console& operator<<(const char* val);
	Console& operator<<(const std::string& val);
	Console& operator<<(char val);
	bool readCommand(Arguments* arguments);
    bool readCommand(CommandArgument* arg);
	static int system(const char* cmd);
	static int system(const std::string& cmd) { return system(cmd.c_str()); }

private:
    struct ArgKeyValue
    {
        std::wstring key;
        std::wstring value;
    };
    
    enum StateType
    {
        STATE_START,
        STATE_COMMAND_START,
        STATE_COMMAND_END,
        STATE_FLAG_START,
        STATE_FLAG_VALUE,
        STATE_FLAG_END,
        STATE_ARGUMENT_START,
        STATE_ARGUMENT_END,
    };

    enum StringStateType
    {
        STRING_STATE_UNINIT,
        STRING_STATE_NORMAL,
        STRING_STATE_ESCAPE,
    };
    using ArgKeyValues = std::vector<ArgKeyValue>;

	// 防止基础类型的隐式转换
	Console& operator<<(wchar_t val) = delete;
	Console& operator<<(const wchar_t* val) = delete;

    bool parse(const wchar_t* str, int max, CommandArgument* arg);
    bool parseString(const wchar_t* str, int max, int* index, std::wstring* value);
    StateType m_state{ STATE_START };
    StringStateType m_stringState{ STRING_STATE_UNINIT };
	DISALLOW_COPY_AND_ASSIGN(Console)
};

}

#endif