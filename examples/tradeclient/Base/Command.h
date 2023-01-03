#ifndef HK_COUNTER_COMMAND_H_
#define HK_COUNTER_COMMAND_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace base
{
using Arguments = std::vector < std::string > ;

struct CommandArgument
{
    std::string m_command;
    std::unordered_map<std::string, std::string> m_flags;
    std::vector<std::string> m_arguments;
};

template<typename T>
struct Command
{
    std::string m_desc;
    void(T::*m_pFun)(const Arguments&);
};

template<typename T>
struct CommandEx
{
    std::string m_desc;
    void(T::*m_pFun)(const CommandArgument&);
};

}

#endif