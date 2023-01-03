#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#include "quickfix/config.h"
#include "quickfix/Session.h"
#include <iostream>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include "ClientApplication.h"
// #include "Assert.h"
// #include "Base/DataCopy.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <vector>

// 1: 执行测试用例，2：期权测试
#define STSRT_ACTION '1'

namespace
{
    //using ArgMap = std::map<std::string, std::string>;
    //bool parserCommandArgs(const base::Arguments& args, ArgMap* v)
    //{
    //    BASE_ASSERT(nullptr != v);
    //    std::string key{};
    //    std::string value{};
    //    for (std::size_t i = 1; i < args.size(); i++)
    //    {
    //        auto pos = args[i].find_first_of('=');
    //        if (pos == args[i].npos)
    //        {
    //            key = args[i];
    //            (*v)[key] = "";
    //        }
    //        else
    //        {
    //            key.assign(args[i].c_str(), pos);
    //            value.assign(args[i].c_str() + pos + 1);
    //            (*v)[key] = value;
    //        }
    //    }
    //    return true;
    //}

    size_t find(const std::string& line, std::vector<std::string> vect, int pos = 0)
    {
        int eol1;
        eol1 = 0;
        for (std::vector<std::string>::iterator iter = vect.begin(); iter != vect.end(); ++iter)
        {
            //std::cout << *iter << std::endl;
            int eol2 = line.find(*iter, pos);
            if (eol1 == 0 && eol2 > 0)
                eol1 = eol2;
            else if (eol2 > 0 && eol2 < eol1)
                eol1 = eol2;
        }
        return eol1;
    }

    std::map<std::string, std::string> parseString(std::string const& str, char delim = '=')
    {
        std::map<std::string, std::string> result;

        std::string::size_type key_pos = 0, i, j;
        std::string::size_type key_end;
        std::string::size_type val_pos;
        std::string::size_type lim_pos;
        std::string::size_type val_end;

        while ((key_end = str.find(delim, key_pos)) != std::string::npos)
        {
            if ((val_pos = str.find_first_not_of(delim, key_end + 1)) == std::string::npos) break;
            while (key_end - 1 > 0 && (str[key_end - 1] <= 32 || str[key_end - 1] == ';'))
                key_end--;
            while (val_pos < str.size() && (str[val_pos] <= 32 || str[val_pos] == ';'))
                val_pos++;
            val_end = str.find('\n', val_pos);
            i = str.find('\"', val_pos);
            if (i != std::string::npos)
                j = str.find('\"', i + 1);
            else
                j = 0;
            lim_pos = find(str.substr(0, i), { " ", ";", "\t" }, val_pos + 1);
            // std::cout << "s.substr(j):" << s.substr(j) << std::endl;
            if (lim_pos == 0 && j != std::string::npos) lim_pos = find(str.substr(j), { " ", ";", "\t" }) + j;
            if (lim_pos < val_pos) lim_pos = val_pos + 1;
            if (j > 0) val_end = j + 1;
            if (val_end > lim_pos) val_end = lim_pos;
            result.emplace(str.substr(key_pos, key_end - key_pos), str.substr(val_pos, val_end - val_pos));
            key_pos = val_end;
            while ((key_pos < str.size() && str[key_pos] <= 32 || str[key_pos] == ';'))
                ++key_pos;
            if (val_end == 0) break;
        }
        return result;
    }

    bool startsWith(const std::string& str, const std::string& prefix)
    {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }

    bool endsWith(std::string const& str, std::string const& suffix)
    {
        if (str.length() < suffix.length())
        {
            return false;
        }
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
}

void ClientApplication::onCreate(const FIX::SessionID& sessionID)
{
    // 设置颜色RED
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
    std::cout << std::endl
        << "Create - " << sessionID << std::endl
        << std::endl;
    // 还原颜色
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ClientApplication::onLogon(const FIX::SessionID& sessionID)
{
    // 设置颜色RED
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
    std::cout << std::endl
        << "Logon - " << sessionID << std::endl
        << std::endl;
    // 还原颜色
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ClientApplication::onLogout(const FIX::SessionID& sessionID)
{
    // 设置颜色RED
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
    std::cout << std::endl
        << "Logout - " << sessionID << std::endl
        << std::endl;
    // 还原颜色
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ClientApplication::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    // crack(message, sessionID);

    // 设置颜色GREEN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    std::cout << std::endl
        << "IN: " << std::endl 
        << message << std::endl
        << std::endl;
    // 还原颜色
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    const auto& header = message.getHeader();
    const auto& msgType = header.getField(FIX::FIELD::MsgType);
    if (msgType == FIX::MsgType_ExecutionReport)
    {
        implExecutionReport(message);
    }
    else if (msgType == FIX::MsgType_OrderCancelReject)
    {
        implOrderCancelReject(message);
    }
    else if (msgType == FIX::MsgType_BusinessMessageReject)
    {
        implBusinessMessageReject(message);
    }
    else
    {
        printLog("未知的消息类型");
    }
}

void ClientApplication::toApp(FIX::Message& message, const FIX::SessionID& sessionID)
EXCEPT(FIX::DoNotSend)
{
    try
    {
        FIX::PossDupFlag possDupFlag;
        message.getHeader().getField(possDupFlag);
        if (possDupFlag) throw FIX::DoNotSend();
    }
    catch (FIX::FieldNotFound&) {}

    // 设置颜色BLUE
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
    std::cout << std::endl
        << "OUT: " << std::endl
        << message << std::endl
        << std::endl;
    // 还原颜色
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ClientApplication::onMessage(const FIX42::ExecutionReport&, const FIX::SessionID&) {}
void ClientApplication::onMessage(const FIX42::OrderCancelReject&, const FIX::SessionID&) {}

bool ClientApplication::implExecutionReport(const FIX::Message& message)
{
    FIX42::ExecutionReport msg{};
    if (message.isSetField(FIX::FIELD::ClOrdID))
    {
        m_clOrdID = message.getField(FIX::FIELD::ClOrdID);
        msg.setField(FIX::FIELD::ClOrdID, m_clOrdID);
    }

    if (message.isSetField(FIX::FIELD::OrigClOrdID))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::OrigClOrdID));
    }

    if (message.isSetField(FIX::FIELD::Account))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::Account));
    }

    if (message.isSetField(FIX::FIELD::OrdType))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::OrdType));
    }

    if (message.isSetField(FIX::FIELD::OrderID))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::OrderID));
    }

    if (message.isSetField(FIX::FIELD::ExecID))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::ExecID));
    }

    if (message.isSetField(FIX::FIELD::CumQty))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::CumQty));
    }

    if (message.isSetField(FIX::FIELD::LeavesQty))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::LeavesQty));
    }

    if (message.isSetField(FIX::FIELD::OrdStatus))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::OrdStatus));
    }

    if (message.isSetField(FIX::FIELD::ExecType))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::ExecType));
    }

    if (message.isSetField(FIX::FIELD::Side))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::Side));
    }

    if (message.isSetField(FIX::FIELD::Symbol))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::Symbol));
    }

    if (message.isSetField(FIX::FIELD::OrderQty))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::OrderQty));
    }

    if (message.isSetField(FIX::FIELD::TransactTime))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::TransactTime));
    }

    if (msg.isSetField(FIX::FIELD::Price))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::Price));
    }

    if (msg.isSetField(FIX::FIELD::ExecRefID))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::ExecRefID));
    }

    if (msg.isSetField(FIX::FIELD::LastPx))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::LastPx));
    }

    if (msg.isSetField(FIX::FIELD::LastQty))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::LastQty));
    }

    if (msg.isSetField(FIX::FIELD::TrdMatchID))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::TrdMatchID));
    }
    return true;
}

void ClientApplication::implOrderCancelReject(const FIX::Message& message) const
{
}

void ClientApplication::implBusinessMessageReject(const FIX::Message& message) const
{
}

// ClientApplication::ClientApplication(const Context& ctx) : m_ctx(ctx)
ClientApplication::ClientApplication()
{
    m_account = "12345678";

    auto nowTime = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    auto now = std::localtime(&nowTime);
    char buf[32]{};
    std::sprintf(buf, "%02d%02d%02d%02d%02d", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    m_prefix = buf;
}

ClientApplication::~ClientApplication()
{
}

void ClientApplication::run()
{
    while (true)
    {
        try
        {
            char action = m_action;
            if (!action)
            {
#ifdef STSRT_ACTION
                // 指定运行某项，不选择
                action = STSRT_ACTION;
#elif
                std::cout << std::endl
                    << "请选择: " << std::endl
                    << "1) 执行测试用例" << std::endl
                    << "2) 测试期权" << std::endl
                    << "3) 退出" << std::endl;

                std::cin >> action;
                m_action = action;
#endif // STSRT_ACTION
            }
            
            switch (action)
            {
                case '1': case '2': case '3': case '4': break;
                default: throw std::exception();
            }

            if (action == '1')
                // 开始执行测试用例
                startTestCaseAction();
            else if (action == '2')
                // 开始期权测试
                startOptionAction();
            else if (action == '3')
                // 退出，结束程序
                break;
        }
        catch (std::exception& e)
        {
            // 设置颜色RED
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            std::cout << std::endl
                << "Message Not Sent: " << e.what() << std::endl
                << std::endl;
            // 还原颜色
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }
}

// 手动输入Fix版本
int ClientApplication::queryVersion()
{
    std::cout << std::endl
        << "1) FIX.4.0" << std::endl
        << "2) FIX.4.1" << std::endl
        << "3) FIX.4.2" << std::endl
        << "4) FIX.4.3" << std::endl
        << "5) FIX.4.4" << std::endl
        << "6) FIXT.1.1 (FIX.5.0)" << std::endl
        << "BeginString: " << std::endl;

    char value;
    std::cin >> value;
    switch (value)
    {
        case '1': return 40;
        case '2': return 41;
        case '3': return 42;
        case '4': return 43;
        case '5': return 44;
        case '6': return 50;
        default: throw std::exception();
    }
}

std::string ClientApplication::queryClOrdID()
{
    return m_prefix + std::to_string(++m_count);
}

//std::string ClientApplication::querySide()
//{
//    std::cout << std::endl
//        << "1) Buy" << std::endl
//        << "2) Sell" << std::endl
//        << "3) Sell Short" << std::endl
//        << "4) Sell Short Exempt" << std::endl
//        << "5) Cross" << std::endl
//        << "6) Cross Short" << std::endl
//        << "7) Cross Short Exempt" << std::endl
//        << "Side: " << std::endl;
//
//    char value;
//    std::cin >> value;
//    switch (value)
//    {
//    case '1': return FIX::Side(FIX::Side_BUY);
//    case '2': return FIX::Side(FIX::Side_SELL);
//    case '3': return FIX::Side(FIX::Side_SELL_SHORT);
//    case '4': return FIX::Side(FIX::Side_SELL_SHORT_EXEMPT);
//    case '5': return FIX::Side(FIX::Side_CROSS);
//    case '6': return FIX::Side(FIX::Side_CROSS_SHORT);
//    case '7': return FIX::Side('A');
//    default: throw std::exception();
//    }
//}
//
//std::string ClientApplication::queryOrdType()
//{
//    std::cout << std::endl
//        << "1) Market" << std::endl
//        << "2) Limit" << std::endl
//        << "3) Stop" << std::endl
//        << "4) Stop Limit" << std::endl
//        << "OrdType: " << std::endl;
//
//    char value;
//    std::cin >> value;
//    switch (value)
//    {
//    case '1': return FIX::OrdType(FIX::OrdType_MARKET);
//    case '2': return FIX::OrdType(FIX::OrdType_LIMIT);
//    case '3': return FIX::OrdType(FIX::OrdType_STOP);
//    case '4': return FIX::OrdType(FIX::OrdType_STOP_LIMIT);
//    default: throw std::exception();
//    }
//}
//
//std::string ClientApplication::queryTimeInForce()
//{
//    std::cout << std::endl
//        << "0) DAY" << std::endl
//        << "1) GTC" << std::endl
//        << "2) OPG" << std::endl
//        << "3) IOC" << std::endl
//        << "4) FOK" << std::endl
//        << "5) GTX" << std::endl
//        << "6) GTD" << std::endl
//        << "7) ATC" << std::endl
//        << "8) AUC" << std::endl
//        << "TimeInForce: " << std::endl;
//
//    char value;
//    std::cin >> value;
//    switch (value)
//    {
//    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
//        return FIX::TimeInForce(value);
//    default: throw std::exception();
//    }
//}
//
//std::string ClientApplication::querySymbol()
//{
//    std::cout << std::endl
//        << "Symbol: " << std::endl;
//
//    std::string value;
//    std::cin >> value;
//    return FIX::Symbol(value);
//}
//
//std::string ClientApplication::queryCurrency()
//{
//    std::cout << std::endl
//        << "Currency: " << std::endl;
//
//    std::string value;
//    std::cin >> value;
//    return FIX::Currency(value);
//}
//
//std::string ClientApplication::querySecurityExchange()
//{
//    std::cout << std::endl
//        << "Exchange: " << std::endl;
//
//    std::string value;
//    std::cin >> value;
//    return FIX::SecurityExchange(value);
//}
//
//std::string ClientApplication::queryPrice()
//{
//    std::cout << std::endl
//        << "Price: " << std::endl;
//
//    std::string value;
//    std::cin >> value;
//    return value;
//}
//
//std::string ClientApplication::queryOrderQty()
//{
//    std::cout << std::endl
//        << "OrderQty: " << std::endl;
//
//    std::string value;
//    std::cin >> value;
//    return value;
//}

std::string ClientApplication::queryCommands()
{
    std::cout << std::endl
        << "cmd(c、m、d)=; 1(side)=; 2(ordType)=; 3(timeInForce)=; 4(symbol)=; 5(currency)=; 6(securityExchang)=; 7(price)=; 8(orderQty)=;" << std::endl;

    std::string value;
    std::cin >> value;

    /*
    "File=\"c:\\dir\\ocean\\\nCCS_test.txt\"\n
    iEcho=10000; iHrShift=0 rho_Co2 = 1.15d0;\n
    Liner=01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
    */
    std::string str = value;
    if (startsWith(str, "\"") && endsWith(str, "\""))
    {
        str = str.substr(1, str.length() - 2);
    }
    return str;
}

std::map<std::string, std::string> ClientApplication::parseCommands(std::string str)
{
    std::map<std::string, std::string> commands = parseString(str);
    for (auto iter = commands.begin(); iter != commands.end(); ++iter) {
        if (iter->first == "1" || boost::iequals(iter->first, "side"))
        {
            if (iter->second == "-")
            {
                m_side = "";
                continue;
            }
            m_side = iter->second;
        }
        else if (iter->first == "2" || boost::iequals(iter->first, "ordType"))
        {
            if (iter->second == "-")
            {
                m_ordType = "";
                continue;
            }
            m_ordType = iter->second;
        }
        else if (iter->first == "3" || boost::iequals(iter->first, "timeInForce"))
        {
            if (iter->second == "-")
            {
                m_timeInForce = "";
                continue;
            }
            m_timeInForce = iter->second;
        }
        else if (iter->first == "4" || boost::iequals(iter->first, "symbol"))
        {
            if (iter->second == "-")
            {
                m_symbol = "";
                continue;
            }
            m_symbol = iter->second;
        }
        else if (iter->first == "5" || boost::iequals(iter->first, "currency"))
        {
            if (iter->second == "-")
            {
                m_currency = "";
                continue;
            }
            m_currency = iter->second;
        }
        else if (iter->first == "6" || boost::iequals(iter->first, "securityExchang"))
        {
            if (iter->second == "-")
            {
                m_securityExchange = "";
                continue;
            }
            m_securityExchange = iter->second;
        }
        else if (iter->first == "7" || boost::iequals(iter->first, "price"))
        {
            if (iter->second == "-")
            {
                m_price = "";
                continue;
            }
            m_price = iter->second;
        }
        else if (iter->first == "8" || boost::iequals(iter->first, "orderQty"))
        {
            if (iter->second == "-")
            {
                m_orderQty = "";
                continue;
            }
            m_orderQty = iter->second;
        }
        else
        {
            continue;
        }
    }
    return commands;
}


bool ClientApplication::queryConfirm(const std::string& query)
{
    std::cout << std::endl
        << query << "? (Y/N) " << std::endl;

    std::string value;
    std::cin >> value;
    return toupper(*value.c_str()) == 'Y';
}

void ClientApplication::printLog(std::string log)
{
    // 设置颜色YELLOW
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    std::cout << std::endl
        << log << std::endl
        << std::endl;
    // 还原颜色
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ClientApplication::setupStaticFields(FIX::Message& message)
{
    FIX::Header& header = message.getHeader();
    header.setField(FIX::FIELD::SenderCompID, "client1"); // SenderCompID <49> field，
    header.setField(FIX::FIELD::TargetCompID, "gateway"); // TargetCompID <56> field，

    message.setField(20005, "20");                        // IB Gateway
    message.setField(FIX::FIELD::Account, m_account);     // 终端账号，CLientDemo和OrderService通信可以随意取
}

void ClientApplication::setupCreateMessage(FIX::Message& message)
{
    std::string clOrdID = queryClOrdID();
    if (!clOrdID.empty())
        message.setField(FIX::FIELD::ClOrdID, clOrdID);

    std::string side = m_side;
    if (!side.empty())
        message.setField(FIX::FIELD::Side, side);

    std::string ordType = m_ordType;
    if (!ordType.empty())
        message.setField(FIX::FIELD::OrdType, ordType);

    std::string symbol = m_symbol;
    if (!symbol.empty())
        message.setField(FIX::FIELD::Symbol, symbol);

    std::string exDestination = m_exDestination;
    if (!exDestination.empty())
        message.setField(FIX::FIELD::ExDestination, exDestination); // 智能路由，固定传"0"，同时还必须设置Currency(15) 或者 SecurityExchange(207)

    std::string currency = m_currency;
    if (!currency.empty())
        message.setField(FIX::FIELD::Currency, currency);

    std::string securityExchange = m_securityExchange; // 取值范围：SSE | ZSSE | HKEX | SGX | US | USOTC
    if (!securityExchange.empty())
        message.setField(FIX::FIELD::SecurityExchange, securityExchange);

    std::string timeInForce = m_timeInForce;
    if (!timeInForce.empty())
        message.setField(FIX::FIELD::TimeInForce, timeInForce);

    FIX::OrderQty orderQty = std::stof(m_orderQty);
    if (orderQty > 0)
        message.setField(FIX::OrderQty(orderQty));

    FIX::Price price = std::stof(m_price);
    if (price > 0)
    {
        if (ordType.compare("2") || ordType.compare("4")) // OrdType_LIMIT、OrdType_STOP_LIMIT
            message.setField(FIX::Price(price));  // 限价类型要传Price
        if (ordType.compare("3") || ordType.compare("4")) // OrdType_STOP、OrdType_STOP_LIMIT
            message.setField(FIX::StopPx(price / 2)); // 止损类型要传StopPx
    }

    message.setField(FIX::TransactTime());

    // 设置公共的静态字段
    setupStaticFields(message);
}

void ClientApplication::setupModifyMessage(FIX::Message& message)
{
    std::string origClOrdID = m_clOrdID;
    if (!origClOrdID.empty())
        message.setField(FIX::FIELD::OrigClOrdID, origClOrdID); // 最近修改后的客户订单id，一直在更新，并不是初始的那一个

    std::string clOrdID = queryClOrdID();
    if (!clOrdID.empty())
        message.setField(FIX::FIELD::ClOrdID, clOrdID);

    std::string side = m_side;
    if (!side.empty())
        message.setField(FIX::FIELD::Side, side);

    std::string ordType = m_ordType;
    if (!ordType.empty())
        message.setField(FIX::FIELD::OrdType, ordType);

    std::string symbol = m_symbol;
    if (!symbol.empty())
        message.setField(FIX::FIELD::Symbol, symbol);

    std::string currency = m_currency; // 货币: USD
    if (!currency.empty())
        message.setField(FIX::FIELD::Currency, currency);

    std::string securityExchange = m_securityExchange; // 取值范围：SSE | ZSSE | HKEX | SGX | US | USOTC
    if (!securityExchange.empty())
        message.setField(FIX::FIELD::SecurityExchange, securityExchange);

    std::string timeInForce = m_timeInForce;
    if (!timeInForce.empty())
        message.setField(FIX::FIELD::TimeInForce, timeInForce);

    FIX::OrderQty orderQty = std::stof(m_orderQty);
    if (orderQty > 0)
        message.setField(FIX::OrderQty(orderQty));

    FIX::Price price = std::stof(m_price);
    if (price > 0)
    {
        if (ordType.compare("2") || ordType.compare("4")) // OrdType_LIMIT、OrdType_STOP_LIMIT
            message.setField(FIX::Price(price));  // 限价类型要传Price
        if (ordType.compare("3") || ordType.compare("4")) // OrdType_STOP、OrdType_STOP_LIMIT
            message.setField(FIX::StopPx(price / 2)); // 止损类型要传StopPx
    }

    message.setField(FIX::TransactTime());

    // 设置公共的静态字段
    setupStaticFields(message);
}

void ClientApplication::setupCancelMessage(FIX::Message& message)
{
    std::string origClOrdID = m_clOrdID;
    if (!origClOrdID.empty())
        message.setField(FIX::FIELD::OrigClOrdID, origClOrdID);

    std::string clOrdID = queryClOrdID();
    if (!clOrdID.empty())
        message.setField(FIX::FIELD::ClOrdID, clOrdID);

    std::string side = m_side;
    if (!side.empty())
        message.setField(FIX::FIELD::Side, side);

    std::string currency = m_currency; // 货币: USD
    if (!currency.empty())
        message.setField(FIX::FIELD::Currency, currency);

    std::string securityExchange = m_securityExchange; // 取值范围：SSE | ZSSE | HKEX | SGX | US | USOTC
    if (!securityExchange.empty())
        message.setField(FIX::FIELD::SecurityExchange, securityExchange);
    
    message.setField(FIX::TransactTime());
}

void ClientApplication::startOptionAction()
{
    std::cout << std::endl
        << "请选择: " << std::endl
        << "1) 联交所港股期权" << std::endl
        << "2) 美股期权" << std::endl
        << "3) 港期所指数期权" << std::endl
        << "4) 港期所期货" << std::endl;

    char action;
    std::cin >> action;
    switch (action)
    {
    case '1': case '2': case '3': case '4': case '5': break;
    default: throw std::exception();
    }

    if (action == '1')
        testOption1();
    else if (action == '2')
        testOption2();
    else if (action == '3')
        testOption3();
    else if (action == '4')
        testFuture();
    else
        throw std::exception();
}

// 1) 联交所港股期权
void ClientApplication::testOption1()
{
    m_side = "1"; // 1~9
    m_ordType = "2"; // 1~5 B J LT P TSL
    m_timeInForce = "0"; // 0~8(0 = Day)
    m_symbol = "00700";
    m_currency = "HKD";
    m_securityExchange = "HKEX";
    m_price = "1000";
    m_orderQty = "1000";

    FIX50::NewOrderSingle message{};

    // 设置创建订单所需字段
    setupCreateMessage(message);

    message.setField(167, "OPT"); // SecurityTyp
    message.setField(200, "202212"); // MaturityMonthYear
    message.setField(205, "29"); // MaturityDay
    message.setField(202, "140"); // strikePrice
    message.setField(201, "1"); // PutOrCall

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

// 2) 美股期权
void ClientApplication::testOption2()
{
    m_side = "1"; // 1~9
    m_ordType = "2"; // 1~5 B J LT P TSL
    m_timeInForce = "0"; // 0~8(0 = Day)
    m_symbol = "MSFT";
    m_currency = "USD";
    m_securityExchange = "";
    m_price = "1000";
    m_orderQty = "1000";

    FIX50::NewOrderSingle message{};

    // 设置创建订单所需字段
    setupCreateMessage(message);

    message.setField(167, "OPT"); // SecurityTyp
    message.setField(200, "202301"); // MaturityMonthYear
    message.setField(205, "6"); // MaturityDay
    message.setField(202, "310"); // strikePrice
    message.setField(201, "1"); // PutOrCall

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

// 3) 港期所指数期权
void ClientApplication::testOption3()
{
    m_side = "1"; // 1~9
    m_ordType = "2"; // 1~5 B J LT P TSL
    m_timeInForce = "0"; // 0~8(0 = Day)
    m_symbol = "HSI";
    m_currency = "HKD";
    m_securityExchange = "HKEX";
    m_price = "1000";
    m_orderQty = "1000";

    FIX50::NewOrderSingle message{};

    // 设置创建订单所需字段
    setupCreateMessage(message);

    message.setField(167, "OPT"); // SecurityTyp
    message.setField(200, "202212"); // MaturityMonthYear
    message.setField(205, "29"); // MaturityDay
    message.setField(202, "19000"); // strikePrice
    message.setField(201, "1"); // PutOrCall

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

// 4) 港期所期货
void ClientApplication::testFuture()
{
    m_side = "1"; // 1~9
    m_ordType = "2"; // 1~5 B J LT P TSL
    m_timeInForce = "0"; // 0~8(0 = Day)
    m_symbol = "HSI";
    m_currency = "HKD";
    m_securityExchange = "HKEX";
    m_price = "1000";
    m_orderQty = "1000";

    FIX50::NewOrderSingle message{};

    // 设置创建订单所需字段
    setupCreateMessage(message);

    message.setField(167, "FUT"); // SecurityTyp
    message.setField(200, "202212"); // MaturityMonthYear

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

void ClientApplication::startTestCaseAction()
{
#if 1
    static int times = 0;
    if (++times == 1)
    {
        // 第一个循环，提示命令格式，后续不再提示
        std::cout << std::endl
            << "命令格式如下：" << std::endl
            << "cmd(c、m、d)=; 1(side)=; 2(ordType)=; 3(timeInForce)=; 4(symbol)=; 5(currency)=; 6(securityExchang)=; 7(price)=; 8(orderQty)=;" << std::endl
            << std::endl;
    }

    std::cout << std::endl
        << "请输入新命令： " << std::endl;

    std::string value;
    std::cin >> value;
    std::string str = value;
    if (startsWith(str, "\"") && endsWith(str, "\""))
    {
        str = str.substr(1, str.length() - 2);
    }

    // 解析命令，并修改成员变量，用作请求消息参数
    std::map<std::string, std::string> commands = parseCommands(str);

    // map判断有某个key
    if (commands.find("cmd") != commands.end())
    {
        std::map<std::string, std::string>::iterator iter = commands.find("cmd");
        std::string cmd = iter->second;
        if (boost::iequals(cmd, "c"))
        {
            // 创建订单
            sendCreateOrder();
        }
        else if (boost::iequals(cmd, "m"))
        {
            // 修改订单
            sendModifyOrder();
        }
        else if (boost::iequals(cmd, "d"))
        {
            // 取消订单
            sendCancelOrder();
        }
        else
        {
            // 未知
            return;
        }
    }

#else
    //m_console << "欢迎使用\n";

    //Args args;
    //while (true)
    //{
    //    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //    m_console << "->";
    //    args.clear();
    //    if (!m_console.readCommand(&args)) continue;
    //    const auto& cmd = args[0];

    //    if ("" == cmd) continue;
    //    else if ("exit" == cmd) break;
    //    else if ("help" == cmd)
    //    {
    //        for (const auto& pair : m_commands)
    //        {
    //            m_console << "命令:" << pair.first << ", 说明:" << pair.second.m_desc << '\n';
    //        }
    //    }
    //    else
    //    {
    //        auto iter = m_commands.find(cmd);
    //        if (iter == m_commands.cend())
    //        {
    //            m_console.system(cmd);
    //        }
    //        else
    //        {
    //            (this->*(iter->second.m_pFun))(args);
    //        }
    //    }
    //}

    //m_console << "程序退出\n";
    //return;
#endif
}

// Account <1> field
// BeginString <8> field
// BodyLength <9> field
// ClOrdID <11> field
// Currency <15> field
// HandlInst <21> field
// MsgSeqNum <34> field
// MsgType <35> field
// OrderQty <38> field
// OrdType <40> field
// OrigClOrdID <41> field
// SenderCompID <49> field
// SendingTime <52> field
// Side <54> field
// Symbol <55> field
// TargetCompID <56> field
// TimeInForce <59> field
// TransactTime <60> field
// StopPx <99> field
// ExDestination <100> field
// SecurityExchange <207> field
// CheckSum <10> field
//
void ClientApplication::sendCreateOrder()
{
    FIX50::NewOrderSingle message{};

    // 设置创建订单所需字段
    setupCreateMessage(message);

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

void ClientApplication::sendModifyOrder()
{
    FIX50::OrderCancelReplaceRequest message{};

    // 设置修改订单所需字段
    setupModifyMessage(message);

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

void ClientApplication::sendCancelOrder()
{
    FIX50::OrderCancelRequest message{};
    
    // 设置取消订单所需字段
    setupCancelMessage(message);

    // 将订单发送
    FIX::Session::sendToTarget(message);
}