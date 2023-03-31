#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#include "quickfix/config.h"
#include "quickfix/Session.h"
#include <iostream>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include "ClientApplication.h"
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <vector>

// 要测试的路由ID，20：IBFIX，5：IBTWS
#define TEST_ROUTE_ID "101"

// 1: 执行测试用例，2：期权测试
// #define STSRT_ACTION '3'

namespace
{
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
        m_ordType = message.getField(FIX::FIELD::OrdType);
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
        m_side = message.getField(FIX::FIELD::Side);
        msg.setField(message.getFieldRef(FIX::FIELD::Side));
    }

    if (message.isSetField(FIX::FIELD::Symbol))
    {
        m_symbol = message.getField(FIX::FIELD::Symbol);
        msg.setField(message.getFieldRef(FIX::FIELD::Symbol));
    }

    if (message.isSetField(FIX::FIELD::SecurityExchange))
    {
        m_securityExchange = message.getField(FIX::FIELD::SecurityExchange);
        msg.setField(message.getFieldRef(FIX::FIELD::SecurityExchange));
    }

    if (message.isSetField(FIX::FIELD::Currency))
    {
        m_currency = message.getField(FIX::FIELD::Currency);
        msg.setField(message.getFieldRef(FIX::FIELD::Currency));
    }

    if (message.isSetField(FIX::FIELD::OrderQty))
    {
        m_orderQty = message.getField(FIX::FIELD::OrderQty);
        msg.setField(message.getFieldRef(FIX::FIELD::OrderQty));
    }

    if (message.isSetField(FIX::FIELD::TransactTime))
    {
        msg.setField(message.getFieldRef(FIX::FIELD::TransactTime));
    }

    if (msg.isSetField(FIX::FIELD::Price))
    {
        m_price = message.getField(FIX::FIELD::Price);
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

    if (msg.isSetField(FIX::FIELD::SecurityType))
    {
        m_securityType = message.getField(FIX::FIELD::SecurityType);
    }

    m_maturityMonthYear = "";
    m_maturityDay = "";
    m_strikePrice = "";
    m_putOrCall = "";
    if (m_securityType == FIX::SecurityType_FUTURE)
    {
        m_maturityMonthYear = message.getField(FIX::FIELD::MaturityMonthYear);
        if (message.isSetField(FIX::FIELD::MaturityDay))
        {
            m_maturityDay = message.getField(FIX::FIELD::MaturityDay);
        }
    }
    else if (m_securityType == FIX::SecurityType_OPTION || m_securityType == "FOP")
    {
        m_maturityMonthYear = message.getField(FIX::FIELD::MaturityMonthYear);
        if (message.isSetField(FIX::FIELD::MaturityDay))
        {
            m_maturityDay = message.getField(FIX::FIELD::MaturityDay);
        }
        m_strikePrice = message.getField(FIX::FIELD::StrikePrice);
        m_putOrCall = message.getField(FIX::FIELD::PutOrCall);
    }

    return true;
}

void ClientApplication::implOrderCancelReject(const FIX::Message& message) const
{
}

void ClientApplication::implBusinessMessageReject(const FIX::Message& message) const
{
}

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
        char action = m_action;
        if (!action)
        {
#ifdef STSRT_ACTION
            // 指定运行某项，不选择
            action = STSRT_ACTION;
#else
            std::cout << std::endl
                << "请选择: " << std::endl
                << "1) 执行测试用例" << std::endl
                << "2) 测试期权" << std::endl
                << "3) 退出" << std::endl;

            std::cin >> action;
#endif // STSRT_ACTION
        }

        switch (action)
        {
        case '1': case '2': case '3': break;
        default: action = '1';
        }
        m_action = action;

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
        << "cmd(i、m、c、0)=; 1(side)=; 2(ordType)=; 3(timeInForce)=; 4(symbol)=; 5(currency)=; 6(securityExchang)=; 7(price)=; 8(orderQty)=;" << std::endl;

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
    // SenderCompID、TargetCompID应该放在header里，否则发送消息时本地会报错“Session Not Found”
    FIX::Header& header = message.getHeader();
    header.setField(FIX::FIELD::SenderCompID, "client1"); // SenderCompID <49> field，
    header.setField(FIX::FIELD::TargetCompID, "gateway"); // TargetCompID <56> field，

    message.setField(20005, TEST_ROUTE_ID);               // 路由ID
    message.setField(FIX::FIELD::Account, m_account);     // 终端账号，CLientDemo和OrderService通信可以随意取
    // message.setField(FIX::FIELD::HandlInst, "1");
    message.setField(FIX::FIELD::SecurityExchange, "US"); // 必带参数
    message.setField(FIX::FIELD::Currency, "USD");        // 必带参数

    // IBTWS
    message.setField(FIX::FIELD::Country, m_country);
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

    // IBTWS
    std::string country = m_country;
    if (!country.empty())
        message.setField(FIX::FIELD::Country, country);

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

    // 设置公共的静态字段
    setupStaticFields(message);
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
    //static int times = 0;
    //if (++times == 1)
    //{
    //    // 第一个循环，提示命令格式，后续不再提示
    //    std::cout << std::endl
    //        << "命令格式如下：" << std::endl
    //        << "cmd(c、m、d)=; 1(side)=; 2(ordType)=; 3(timeInForce)=; sym(symbol)=; cur(currency)=; exg(securityExchang)=; 7(price)=; 8(orderQty)=; sty(securityType)" << std::endl
    //        << std::endl;
    //}

    std::cout << std::endl
        << "请输入测试用例： " << std::endl;

#if 1
    const int MAX = 150;
    char chars[MAX];
    std::cin.getline(chars, MAX);
#else
    std::string value;
    std::cin >> value;
    std::string str = value;
    if (startsWith(str, "\"") && endsWith(str, "\""))
    {
        str = str.substr(1, str.length() - 2);
    }
#endif

    std::string str = chars;
    // 把字符产全部转换成小写字母。transform()有4个参数，转换字符串起始地址、终止地址、输出到的位置、转换操作
    // transform(str.begin(), str.end(), str.begin(), ::tolower); 由于参数value可能需要大写，因此不能一律转成小写

    // 解析命令，并修改成员变量，用作请求消息参数
    std::map<std::string, std::string> params = parseString(str);

    // map判断有某个key
    // std::map<std::string, std::string>::iterator iter = params.find("cmd");
    auto iter = params.find("cmd");
    if (iter != params.end())
    {
        std::string cmd = iter->second;
        if (boost::iequals(cmd, "i"))
        {
            // 创建订单
            sendCreateOrder(&params);
        }
        else if (boost::iequals(cmd, "m"))
        {
            // 修改订单
            sendModifyOrder(&params);
        }
        else if (boost::iequals(cmd, "c"))
        {
            // 取消订单
            sendCancelOrder();
        }
        else  if (boost::iequals(cmd, "mi"))
        {
            // 创建订单
            sendCreateMultiOrder(&params);
        }
        else if (boost::iequals(cmd, "mm"))
        {
            // 修改订单
            sendModifyMultiOrder(&params);
        }
        else if (boost::iequals(cmd, "ix"))
        {
            // 创建订单
            sendCreateOrderX(&params);
        }
        else if (boost::iequals(cmd, "e"))
        {
            // 发送错误码
            sendErrorCode(&params);
        }
        else
        {
            // 未知
            return;
        }
    }
}

void ClientApplication::sendCreateOrder(std::map<std::string, std::string>* params)
{
    std::string flag{};
    std::string idSource{};
    std::string securityID{};
    std::string symbol{};
    std::string securityType{};
    std::string securityExchange{};
    std::string maturityMonthYear{};
    std::string maturityDay{};
    std::string currency{};
    std::string timeInForce{};
    std::string side{ FIX::Side_BUY };
    std::string ordType{ FIX::OrdType_MARKET };
    std::string strikePrice{};
    std::string putOrCall{};
    std::string forceOnlyRTH{};
    std::string orderQty{ "1000" };
    std::string price{};
    std::string exDestination{ "0" };
    std::string cashOrderQty{};
    std::string expireDate{};
    std::string expireTime{};
    std::string stopPx{};
    std::string execInst{};
    std::string pegDifference{};
    std::string trailingAmtUnit{};
    std::string maxFloor{};

    auto values = *params;
    if (values.find("f") != values.cend())
    {
        flag = values.at("f");
        if (flag == "0") // 联交所港股
        {
            symbol = "700"; // 不能有前缀0, 如00700, 否则返回错误:Unknown contract
            securityExchange = "SEHK";
        }
        else if (flag == "1") // 股票
        {
            symbol = "IBM";
            //securityExchange = "NYSE"; 不需要指明交易所.
            currency = "USD";
        }
        else if (flag == "2") // 股票期权
        {
            securityType = FIX::SecurityType_OPTION;
            symbol = "IBM";
            //securityExchange = "CBOE";
            currency = "USD";
            maturityMonthYear = "202301";
            // 港股期权目前是每个月最多一个, 所以是否带该值都可以.
            // 如果携带, 则必须匹配个股期权正确的到期日, 否则返回错误:Unknown contract
            // maturityDay有效值1~31.
            // maturityDay = "06";是有效代码
            maturityDay = "6";
            strikePrice = "310";
            putOrCall = std::to_string(FIX::PutOrCall_CALL);
        }
        else if (flag == "3") // 期货
        {
            securityType = FIX::SecurityType_FUTURE;
            symbol = "ES";  // ES: 迷你标普500指数期货  YM: 迷你道琼斯工业平均指数期货
            // 根据IB文档注释, 交易期货的时候, 必须将交易所参数设置到exDestination(100).
            // 但是在实际测试中, tag100=SMART, 而使用SecurityExchange(207)设置真实交易所, 效果等同.
            // 无论那种方式, 期货必须使用交易所, 不能依赖Currency指定.
            exDestination = "CME";
            // securityExchange = "CME";  // ES是CME, YM是CBOT
            maturityMonthYear = "202303";
        }
        else if (flag == "4") // 期货期权
        {
            // 根据IB文档, 期货期权需要ClearingAccount(440), 但是根据附录中的注释, 只有非清算的情况下才需要指定.
            // 可能期货期权必然会出现交易和清算分离, 才会出现这种问题, 例如IB没有期货清算资格.
            securityType = "FOP"; // 没有找到FOP类型
            symbol = "ES";  // ES: 迷你标普500指数期货  YM: 迷你道琼斯工业平均指数期货
            securityExchange = "CME";  // ES是CME, YM是CBOT
            // currency = "USD"; // 仅使用货币无法下单成功, 必须携带交易所.
            maturityMonthYear = "202303";
            // 港股期权目前是每个月最多一个, 所以是否带该值都可以.
            // 如果携带, 则必须匹配个股期权正确的到期日, 否则返回错误:Unknown contract
            // maturityDay有效值1~31.
            maturityDay = "29";
            strikePrice = "3720";
            putOrCall = std::to_string(FIX::PutOrCall_CALL);
        }
        else if (flag == "5") // 差价合约 (以投机为目的的交易产品, 如纸黄金)
        {
            securityType = "CFD"; // 没有找到CFD类型
            symbol = "VOD";
            // IB文档没有说明需要指定交易所或货币. 但是实际测试时发现, 如果不携带货币, 会
            // 返回错误: Ambiguous Contract
            currency = "USD";
        }
        else if (flag == "6") // 债券
        {
            securityType = "BOND"; // 没有找到BOND类型
            securityID = "744567CN6";
            idSource = "1"; // 1: CUSIP 4: ISIN
        }
        else if (flag == "7") // 外汇
        {
            securityType = FIX::SecurityType_CASH;
            // 交易ERU/USD
            symbol = "EUR";  // 外汇的Symbol是Counter Currency;
            currency = "USD"; // 外汇的Currency是Base Currency.
            exDestination = "IDEALPRO"; // 外汇必须使用该值,不能使用SMART.
            // IB文档存在6707的字段, 但是附录中没有. 猜测该模式已经无效.
        }
        else if (flag == "8") // 多腿
        {
            securityType = FIX::SecurityType_BRADY_BOND;
            symbol = "CUSIP - 744567CN6";
            currency = "USD";
        }
        else if (flag == "9") // 现货
        {
            securityType = "CMDTY";
            symbol = "XAUUSD";
        }
        else if (flag == "10") // 基金(猜测是场外)
        {
            securityType = "FUND";
            symbol = "BTIDX";
            exDestination = "FUNDSERV";
            // 根据文档必须携带, 否则返回错误: cashOrderQty is invalid
            // 猜测是买入(申购)时必须指定CashOrderQty(152), 此时不需要携带OrderQty(38)
            // 卖出(赎回)的时候, 此时使用38或152,可能取决于基金本身的规定.
            // cashOrderQty; 卖出时可以不携带
        }
        else if (flag == "11") // 纳斯达克
        {
            symbol = "MSFT";
            //securityExchange = "NASDAQ";
            currency = "USD";
        }
        else if (flag == "12") // 港期所指数期权
        {
            // 根据IB文档, 期货期权需要ClearingAccount(440), 但是根据附录中的注释, 只有非清算的情况下才需要指定.
            // 可能期货期权必然会出现交易和清算分离, 才会出现这种问题, 例如IB没有期货清算资格.
            securityType = "FOP";
            // 恒生指数
            symbol = "HSI";
            securityExchange = "HKFE"; // 指数期货只能是港期所
            //currency = "HKD"; // 仅使用货币无法下单成功, 必须携带交易所.
            maturityMonthYear = "202212";
            // 港股期权目前是每个月最多一个, 所以是否带该值都可以.
            // 如果携带, 则必须匹配个股期权正确的到期日, 否则返回错误:Unknown contract
            // maturityDay有效值1~31.
            maturityDay = "29";
            strikePrice = "19000";
            putOrCall = std::to_string(FIX::PutOrCall_CALL);
        }
        else if (flag == "13") // 港期所期货
        {
            securityType = FIX::SecurityType_FUTURE;
            // 恒生指数
            symbol = "HSI";
            // 根据IB文档注释, 交易期货的时候, 必须将交易所参数设置到exDestination(100).
            // 但是在实际测试中, tag100=SMART, 而使用SecurityExchange(207)设置真实交易所, 效果等同.
            // 无论那种方式, 期货必须使用交易所, 不能依赖Currency指定.
            exDestination = "HKFE";
            // securityExchange = "HKFE";
            maturityMonthYear = "202212";
        }
        else if (flag == "14") // 联交所港股期权
        {
            securityType = FIX::SecurityType_OPTION;
            // 不能赋值在700, 只能使用对应HKATS代码700.
            symbol = "TCH"; // 腾讯代码
            securityExchange = "SEHK";
            //currency = "HKD"; // 仅使用货币无法下单成功, 必须携带交易所.
            maturityMonthYear = "202212";
            // 港股期权目前是每个月最多一个, 所以是否带该值都可以.
            // 如果携带, 则必须匹配个股期权正确的到期日, 否则返回错误:Unknown contract
            // maturityDay有效值1~31.
            maturityDay = "29";
            strikePrice = "140";
            putOrCall = std::to_string(FIX::PutOrCall_CALL);
        }
    }
    else
    {
        symbol = "700"; // 不能有前缀0, 如00700, 否则返回错误:Unknown contract
        securityExchange = "SEHK";
    }

    for (const auto& it : values)
    {
        if (it.first == "sty")
        {
            securityType = it.second;
        }
        else if (it.first == "sym")
        {
            symbol = it.second;
        }
        else if (it.first == "exg")
        {
            securityExchange = it.second;
        }
        else if (it.first == "cur")
        {
            currency = it.second;
        }
        else if (it.first == "may")
        {
            maturityMonthYear = it.second;
        }
        else if (it.first == "mad")
        {
            maturityDay = it.second;
        }
        else if (it.first == "stp")
        {
            strikePrice = it.second;
        }
        else if (it.first == "poc")
        {
            putOrCall = it.second;
        }
        else if (it.first == "rth")
        {
            forceOnlyRTH = "1";
        }
        else if (it.first == "tif")
        {
            timeInForce = it.second;
        }
        else if (it.first == "sde")
        {
            side = it.second;
        }
        else if (it.first == "oty")
        {
            ordType = it.second;
        }
        else if (it.first == "odq")
        {
            orderQty = it.second;
        }
        else if (it.first == "pri")
        {
            price = it.second;
        }
        else if (it.first == "coq")
        {
            cashOrderQty = it.second;
        }
        else if (it.first == "exd")
        {
            expireDate = it.second;
        }
        else if (it.first == "ext")
        {
            expireTime = it.second;
        }
        else if (it.first == "stx")
        {
            stopPx = it.second;
        }
        else if (it.first == "eit")
        {
            execInst = it.second;
        }
        else if (it.first == "pdf")
        {
            pegDifference = it.second;
        }
        else if (it.first == "tau")
        {
            trailingAmtUnit = it.second;
        }
        else if (it.first == "edt")
        {
            exDestination = it.second;
        }
        else if (it.first == "mfl")
        {
            maxFloor = it.second;
        }
    }

    FIX50::NewOrderSingle msg{};

    // 设置公共的静态字段
    setupStaticFields(msg);

    if (!securityType.empty()) msg.setField(FIX::FIELD::SecurityType, securityType);
    // 按照IBFIX协议,Account必须携带.
    // 如果不携带,那么返回错误:Invalid or missing IBCustAcctNo
    // 如果使用master accont账户交易股票/期货/期权,那么返回错误:
    // Submit of non-allocation order to master account I901238 for sectype STK/OPT is not allowed.
    // 从IB的邮件中, 要求所有的订单都必须使用SubAccountID.
    // IB规范中有使用Master Account ID的情况,暂时不需要关注.
    // msg.setField(FIX::Account{ m_ctx.getCfg().getSubAccountIDMap().at("long") });
    // 根据IB的测试邮件通知,股票(不确认是否其它品种,猜测是)最好全部使用SMART.
    // IB文档中,则说明:IBKR does not support SMART routing for futures. The destination exchange must be 
    // specified in tag 100.
    // 目前倾向于通过207区分具体的交易所地址.(尽量避免使用货币, 因为很多交易所可以使用多种货币区分).
    msg.setField(FIX::ExDestination{ exDestination });
    // 如果不携带该值,
    // 那么返回拒绝, 错误原因:Invalid BD flag or Origin
    // 从IB的测试邮件中, 要求必须为0.
    msg.setField(FIX::CustomerOrFirm{ FIX::CustomerOrFirm_CUSTOMER });
    // 按照IBFIX协议,下美股IBM,且100=SMART的时候,如果ExDestination是SMART时,
    // 1. SecurityExchange和Currency都不携带,
    // 那么返回拒绝,错误原因是:Ambiguous Contract.
    // 2. 仅携带SecurityExchange时,如果值错误,
    // 那么返回拒绝,错误原因是:Invalid destination exchange specified
    // 文档说参考FIX 4.2规范的Appendix C. 如链接:
    // https://www.onixs.biz/fix-dictionary/4.2/app_c.html
    // 实际上不是参考该规范.
    // 例如纽交所代码, 按照FIX 4.2附录C的规范,应该填N,但实际上应该填写NYSE.
    // 从实际看, 该参数值实际需要参考TWS上Primary Exchange代码.
    // 目前已知NYSE(纽交所)/NASDAQ/SEHK(联交所)/HKFE(港期所)
    // 
    // 下美股MSFT的个股期权,且100=SMART的时候,不使用207就能下单成功.
    // 如果填写207的值, 必须填写正确的交易所CBOE, 否则报错误:Unknown contract
    //
    // 如果是美股, 部分股票可以填写NYSE或者NASDAQ都可以.
    // 大部分必须使用正确的上市交易所, 否则返回错误:Invalid destination exchange specified
    //
    // 综上所述, 当使用SMART的时候, 美国交易所的代码, 优先使用Currency,即USD.
    // 非美国交易所, 仅使用货币无效, 基本必须使用SecurityExchange区分.
    if (!securityExchange.empty()) msg.setField(FIX::FIELD::SecurityExchange, securityExchange);
    // 货币本来应该是指定价格的货币.但是没有在FIX标准中找到Currency具体对应下单Price的货币,还是行情Price的货币.
    // 目前IB实际上用于指定行情Price的货币.
    // securityExchange和currency是"或"关系.可以两者都存在.如果与都存在,那么必须都正确,
    // 否则返回:Unknown contract
    if (!currency.empty()) msg.setField(FIX::FIELD::Currency, currency);
    // 如果Symbol填写错误, 将返回错误:Unknown contract.
    // 港股代码必须与交易所等同,不能包含前缀0.
    // 根据文档理解, 衍生品代码有两种下单方式,我们统一使用标准FIX的下单方式.
    if (!symbol.empty()) msg.setField(FIX::Symbol{ symbol });
    if (!idSource.empty()) msg.setField(FIX::IDSource{ idSource });
    if (!securityID.empty()) msg.setField(FIX::SecurityID{ securityID });
    if (securityType == FIX::SecurityType_OPTION || securityType == "FOP")
    {
        // 实际测试时发现, 当MaturityMonthYear格式不是YYYYMM时, 将返回:Invalid value in field # 205(MaturityDay).
        msg.setField(FIX::MaturityMonthYear{ maturityMonthYear });
        if (!maturityDay.empty()) msg.setField(FIX::MaturityDay{ maturityDay });
        msg.setField(FIX::FIELD::PutOrCall, putOrCall);
        msg.setField(FIX::FIELD::StrikePrice, strikePrice);
    }
    else if (securityType == FIX::SecurityType_FUTURE)
    {
        msg.setField(FIX::MaturityMonthYear{ maturityMonthYear });
        if (!maturityDay.empty()) msg.setField(FIX::MaturityDay{ maturityDay });
    }
    msg.setField(FIX::FIELD::OrdType, ordType);
    if (!timeInForce.empty()) msg.setField(FIX::FIELD::TimeInForce, timeInForce);
    // 按照规范, 支持做空. 但是UAT环境一直返回错误:Sell short variant is not supported
    msg.setField(FIX::FIELD::Side, side);
    // 市价单的时候,一定不能携带该值. 否则将返回错误的消息:Invalid value in field # 44
    if (!price.empty()) msg.setField(FIX::FIELD::Price, price);
    if (!stopPx.empty()) msg.setField(FIX::FIELD::StopPx, stopPx);
    // 如果OrderQty不符合交易所交易单位要求, 返回错误消息:order size must be a multiple of lot size
    msg.setField(FIX::FIELD::OrderQty, orderQty);
    msg.setField(FIX::ClOrdID{ queryClOrdID() });
    // IB规范中说明,FIX默认只允许常规交易.
    // 如果用户自己要求放开或者自己决定是否仅用于常规交易,那么将放开.
    if (!forceOnlyRTH.empty()) msg.setField(6205, "1"); // ForceOnlyRTH 仅支持1
    // 根据IB邮件说明, 最好携带该值, 表明综合账户的子账户.
    msg.setField(6207, "11111111");
    if (!cashOrderQty.empty()) msg.setField(FIX::FIELD::CashOrderQty, cashOrderQty);
    if (!expireDate.empty()) msg.setField(FIX::FIELD::ExpireDate, expireDate);
    if (!expireTime.empty()) msg.setField(FIX::FIELD::ExpireTime, expireTime);
    if (!execInst.empty()) msg.setField(FIX::FIELD::ExecInst, execInst);
    if (!pegDifference.empty()) msg.setField(FIX::FIELD::PegDifference, pegDifference);
    if (!trailingAmtUnit.empty()) msg.setField(6268, trailingAmtUnit);
    if (!maxFloor.empty()) msg.setField(FIX::FIELD::MaxFloor, maxFloor);

    // 将订单发送
    FIX::Session::sendToTarget(msg);
}

void ClientApplication::sendCreateOrderX(std::map<std::string, std::string>* params)
{
    std::string flag{};
    
    auto values = *params;
    if (values.find("f") != values.cend())
    {
        flag = values.at("f");
        if (flag == "0") // 联交所港股
        {
            m_symbol = "700"; // 不能有前缀0, 如00700, 否则返回错误:Unknown contract
            m_securityExchange = "SEHK";
        }
        else if (flag == "1") // 股票
        {
            m_symbol = "IBM";
            // m_securityExchange = "NYSE"; 不需要指明交易所.
            m_currency = "USD";
        }
        else if (flag == "2") // 股票期权
        {
            m_securityType = FIX::SecurityType_OPTION;
            m_symbol = "IBM";
            // m_securityExchange = "CBOE";
            m_currency = "USD";
            m_maturityMonthYear = "202301";
            // 港股期权目前是每个月最多一个, 所以是否带该值都可以.
            // 如果携带, 则必须匹配个股期权正确的到期日, 否则返回错误:Unknown contract
            // maturityDay有效值1~31.
            // maturityDay = "06";是有效代码
            m_maturityDay = "6";
            m_strikePrice = "310";
            m_putOrCall = std::to_string(FIX::PutOrCall_CALL);
        }
        else if (flag == "3") // 期货
        {
            m_securityType = FIX::SecurityType_FUTURE;
            m_symbol = "ES";  // ES: 迷你标普500指数期货  YM: 迷你道琼斯工业平均指数期货
            // 根据IB文档注释, 交易期货的时候, 必须将交易所参数设置到exDestination(100).
            // 但是在实际测试中, tag100=SMART, 而使用SecurityExchange(207)设置真实交易所, 效果等同.
            // 无论那种方式, 期货必须使用交易所, 不能依赖Currency指定.
            m_exDestination = "CME";
            // m_securityExchange = "CME";  // ES是CME, YM是CBOT
            m_maturityMonthYear = "202303";
        }
        else if (flag == "4") // 期货期权
        {
            // 根据IB文档, 期货期权需要ClearingAccount(440), 但是根据附录中的注释, 只有非清算的情况下才需要指定.
            // 可能期货期权必然会出现交易和清算分离, 才会出现这种问题, 例如IB没有期货清算资格.
            m_securityType = "FOP"; // 没有找到FOP类型
            m_symbol = "ES";  // ES: 迷你标普500指数期货  YM: 迷你道琼斯工业平均指数期货
            m_securityExchange = "CME";  // ES是CME, YM是CBOT
            // m_currency = "USD"; // 仅使用货币无法下单成功, 必须携带交易所.
            m_maturityMonthYear = "202303";
            // 港股期权目前是每个月最多一个, 所以是否带该值都可以.
            // 如果携带, 则必须匹配个股期权正确的到期日, 否则返回错误:Unknown contract
            // maturityDay有效值1~31.
            m_maturityDay = "29";
            m_strikePrice = "3720";
            m_putOrCall = std::to_string(FIX::PutOrCall_CALL);
        }
        else if (flag == "5") // 差价合约 (以投机为目的的交易产品, 如纸黄金)
        {
            m_securityType = "CFD"; // 没有找到CFD类型
            m_symbol = "VOD";
            // IB文档没有说明需要指定交易所或货币. 但是实际测试时发现, 如果不携带货币, 会
            // 返回错误: Ambiguous Contract
            m_currency = "USD";
        }
        //else if (flag == "6") // 债券
        //{
        //    m_securityType = "BOND"; // 没有找到BOND类型
        //    m_securityID = "744567CN6";
        //    m_idSource = "1"; // 1: CUSIP 4: ISIN
        //}
        else if (flag == "7") // 外汇
        {
            m_securityType = FIX::SecurityType_CASH;
            // 交易ERU/USD
            m_symbol = "EUR";  // 外汇的Symbol是Counter Currency;
            m_currency = "USD"; // 外汇的Currency是Base Currency.
            m_exDestination = "IDEALPRO"; // 外汇必须使用该值,不能使用SMART.
            // IB文档存在6707的字段, 但是附录中没有. 猜测该模式已经无效.
        }
        else if (flag == "8") // 多腿
        {
            m_securityType = FIX::SecurityType_BRADY_BOND;
            m_symbol = "CUSIP - 744567CN6";
            m_currency = "USD";
        }
        else if (flag == "9") // 现货
        {
            m_securityType = "CMDTY";
            m_symbol = "XAUUSD";
        }
        else if (flag == "10") // 基金(猜测是场外)
        {
            m_securityType = "FUND";
            m_symbol = "BTIDX";
            m_exDestination = "FUNDSERV";
            // 根据文档必须携带, 否则返回错误: cashOrderQty is invalid
            // 猜测是买入(申购)时必须指定CashOrderQty(152), 此时不需要携带OrderQty(38)
            // 卖出(赎回)的时候, 此时使用38或152,可能取决于基金本身的规定.
            // cashOrderQty; 卖出时可以不携带
        }
        else if (flag == "11") // 纳斯达克
        {
            m_symbol = "MSFT";
            // m_securityExchange = "NASDAQ";
            m_currency = "USD";
        }
        else if (flag == "12") // 港期所指数期权
        {
            // 根据IB文档, 期货期权需要ClearingAccount(440), 但是根据附录中的注释, 只有非清算的情况下才需要指定.
            // 可能期货期权必然会出现交易和清算分离, 才会出现这种问题, 例如IB没有期货清算资格.
            m_securityType = "FOP";
            // 恒生指数
            m_symbol = "HSI";
            m_securityExchange = "HKFE"; // 指数期货只能是港期所
            // m_currency = "HKD"; // 仅使用货币无法下单成功, 必须携带交易所.
            m_maturityMonthYear = "202212";
            // 港股期权目前是每个月最多一个, 所以是否带该值都可以.
            // 如果携带, 则必须匹配个股期权正确的到期日, 否则返回错误:Unknown contract
            // maturityDay有效值1~31.
            m_maturityDay = "29";
            m_strikePrice = "19000";
            m_putOrCall = std::to_string(FIX::PutOrCall_CALL);
        }
        else if (flag == "13") // 港期所期货
        {
            m_securityType = FIX::SecurityType_FUTURE;
            // 恒生指数
            m_symbol = "HSI";
            // 根据IB文档注释, 交易期货的时候, 必须将交易所参数设置到exDestination(100).
            // 但是在实际测试中, tag100=SMART, 而使用SecurityExchange(207)设置真实交易所, 效果等同.
            // 无论那种方式, 期货必须使用交易所, 不能依赖Currency指定.
            m_exDestination = "HKFE";
            // securityExchange = "HKFE";
            m_maturityMonthYear = "202212";
        }
        else if (flag == "14") // 联交所港股期权
        {
            m_securityType = FIX::SecurityType_OPTION;
            // 不能赋值在700, 只能使用对应HKATS代码700.
            m_symbol = "TCH"; // 腾讯代码
            m_securityExchange = "SEHK";
            // m_currency = "HKD"; // 仅使用货币无法下单成功, 必须携带交易所.
            m_maturityMonthYear = "202212";
            // 港股期权目前是每个月最多一个, 所以是否带该值都可以.
            // 如果携带, 则必须匹配个股期权正确的到期日, 否则返回错误:Unknown contract
            // maturityDay有效值1~31.
            m_maturityDay = "29";
            m_strikePrice = "140";
            m_putOrCall = std::to_string(FIX::PutOrCall_CALL);
        }
    }
    else
    {
        m_symbol = "700"; // 不能有前缀0, 如00700, 否则返回错误:Unknown contract
        m_securityExchange = "SEHK";
    }

    for (const auto& it : values)
    {
        if (it.first == "sty")
        {
            m_securityType = it.second;
        }
        else if (it.first == "sym")
        {
            m_symbol = it.second;
        }
        else if (it.first == "exg")
        {
            m_securityExchange = it.second;
        }
        else if (it.first == "cur")
        {
            m_currency = it.second;
        }
        else if (it.first == "may")
        {
            m_maturityMonthYear = it.second;
        }
        else if (it.first == "mad")
        {
            m_maturityDay = it.second;
        }
        else if (it.first == "stp")
        {
            m_strikePrice = it.second;
        }
        else if (it.first == "poc")
        {
            m_putOrCall = it.second;
        }
        else if (it.first == "rth")
        {
            // m_forceOnlyRTH = "1";
        }
        else if (it.first == "tif")
        {
            m_timeInForce = it.second;
        }
        else if (it.first == "sde")
        {
            m_side = it.second;
        }
        else if (it.first == "oty")
        {
            m_ordType = it.second;
        }
        else if (it.first == "odq")
        {
            m_orderQty = it.second;
        }
        else if (it.first == "pri")
        {
            m_price = it.second;
        }
        else if (it.first == "coq")
        {
            // m_cashOrderQty = it.second;
        }
        else if (it.first == "exd")
        {
            // m_expireDate = it.second;
        }
        else if (it.first == "ext")
        {
            // m_expireTime = it.second;
        }
        else if (it.first == "stx")
        {
            // m_stopPx = it.second;
        }
        else if (it.first == "eit")
        {
            // m_execInst = it.second;
        }
        else if (it.first == "pdf")
        {
            // m_pegDifference = it.second;
        }
        else if (it.first == "tau")
        {
            // m_trailingAmtUnit = it.second;
        }
        else if (it.first == "edt")
        {
            m_exDestination = it.second;
        }
        else if (it.first == "mfl")
        {
            // m_maxFloor = it.second;
        }
        else
        {
            // do nothing
        }
    }

    sendCreateOrder();
}

void ClientApplication::sendModifyOrder(std::map<std::string, std::string>* params)
{
    std::string timeInForce{};
    std::string orderQty{ m_orderQty };
    std::string price{ m_price };
    std::string stopPx{};
    std::string expireDate{};
    std::string expireTime{};

    auto values = *params;
    for (const auto& it : values)
    {
        if (it.first == "tif")
        {
            timeInForce = it.second;
        }
        else if (it.first == "odq")
        {
            orderQty = it.second;
        }
        else if (it.first == "pri")
        {
            price = it.second;
        }
        else if (it.first == "exd")
        {
            expireDate = it.second;
        }
        else if (it.first == "ext")
        {
            expireTime = it.second;
        }
        else if (it.first == "stx")
        {
            stopPx = it.second;
        }
    }

    FIX50::OrderCancelReplaceRequest msg{};

    // 设置公共的静态字段
    setupStaticFields(msg);

    // 按照IBFIX协议,Account必须携带.
    // 如果不携带,那么返回错误:Invalid or missing IBCustAcctNo
    // 如果携带的账号与下单时的账号不相同(即使是当前账户的主账户),那么返回错误:No such order
    // msg.setField(FIX::Account{ m_accountID });
    msg.setField(FIX::ClOrdID{ queryClOrdID() });
    msg.setField(FIX::OrigClOrdID{ m_clOrdID });
    // 实际测试中,发现该值改单中和改单后OrderID将会改变.
    // 如果携带错误值, 上手并不关心,依旧正常改单,且OrderID与下单的OrderID存在关联.
    // 所以从实际角度看, 最好不要携带, 减少上下消息中,对于OrderID的误解.
    // msg.setField(fix::OrderID{ "1"});
    // 根据IB文档, OrderQty必须携带.
    // 如果不携带, 那么返回错误:Message must contain field # 38
    if (!orderQty.empty()) msg.setField(FIX::FIELD::OrderQty, orderQty);
    // 根据IB文档, OrderType必须携带.
    // 如果不携带, 那么返回错误:Message must contain field # 40
    msg.setField(FIX::FIELD::OrdType, m_ordType);
    // 根据IB文档, Price可以不携带.但是实际测试中,
    // 如果不携带, 那么返回错误:Message must contain field # 44
    // 目前看, 如果原订单使用了Price, 那么改单时即使不修改, 也必须携带原值.
    if (!price.empty()) msg.setField(FIX::FIELD::Price, price);
    // 当原始订单是Stop或StopLimit的时候,与Price一样,即使不改也必须携带.
    // 如果不携带, 那么返回错误:Message must contain field # 99
    if (!stopPx.empty()) msg.setField(FIX::FIELD::StopPx, stopPx);
    // 根据IB文档, Side必须携带.
    // 如果不携带, 那么返回错误:Message must contain field # 54
    msg.setField(FIX::FIELD::Side, m_side);
    // 根据IB文档, Symbol必须携带.但是实际测试中, 可以不携带.
    // 如果不携带, 返回的35=8,39=E/39=5的消息中, field # 55也不存在.
    // 如果携带错误, 返回的35=8,39=E/39=5的消息中, field # 55也会跟着错误, 且后续的35=8(
    // 可能也包括该订单其它消息, 暂时未测试)将存储该值, 并在后续中返回错误的值.
    // 所以从实际角度看,最好按照IB FIX文档,携带与原值完全匹配的值,避免造成对消息错误理解.
    if (!m_symbol.empty()) msg.setField(FIX::Symbol{ m_symbol });
    // 根据IB文档, 支持从DAY到GTC的转换,但是不支持GTD的转换.
    // 根据FIX规范, 下单时该值可以不携带, 不携带时默认表示DAY;改单中如果该值未携带, 未明确意义.
    // 按照实际测试, 如果下单使用GTC类型, 改单不携带, 那么订单中的TimeInforce值不会改变,继续保持GTC类型.
    // 从实际角度看,改单时如果不改变该值,可以不携带.
    if (!timeInForce.empty())
    {
        msg.setField(FIX::FIELD::TimeInForce, timeInForce);
    }
    if (!expireDate.empty()) msg.setField(FIX::FIELD::ExpireDate, expireDate);
    if (!expireTime.empty()) msg.setField(FIX::FIELD::ExpireTime, expireTime);
    // IB规范中,改单不支持该字段.见附录B,仅用于NewOrderSingle.
    // 实际中即使携带也不会拒绝.
    // msg.setField(6205, "1");

    // 对于期货和期权, 如果不携带期货或期权参数(指MaturityMonthYear/PutOrCall等)和SecurityType, 
    // 那么返回的ExecutionReport消息将自动更新为改单请求中的Symbol相关参数, 所有原订单的参数
    // 全部被清空, 从而变更为股票. 但是实际上依旧是期货或期权订单.
    // 如果携带, 最好与原订单保持一致.
    // 所以是否携带完整期货/期权参数和SecurityType, 取决于客户端情况.
    if (!m_securityType.empty()) msg.setField(FIX::FIELD::SecurityType, m_securityType);
    if (!m_maturityMonthYear.empty()) msg.setField(FIX::FIELD::MaturityMonthYear, m_maturityMonthYear);
    if (!m_maturityDay.empty()) msg.setField(FIX::FIELD::MaturityDay, m_maturityDay);
    if (!m_strikePrice.empty()) msg.setField(FIX::FIELD::StrikePrice, m_strikePrice);
    if (!m_putOrCall.empty()) msg.setField(FIX::FIELD::PutOrCall, m_putOrCall);

    // 根据IB邮件说明, 所有订单最好携带该值, 表明综合账户的子账户.
    // IB规范中,改单请求不需要填写该字段(消息格式),但是该字段可以用于改单请求(附录B备注).
    // 测试时发现:
    //     如果下单请求中携带而改单请求不携带,那么ExecutionReport中的6207将保持不变;OrderCancelReject不携带6207.
    //     如果下单和改单都携带,OrderCancelReject携带6207;如果与原来订单的值不匹配,那么ExecutionReport将返回改单请求中的新值;
    //        但是目前无法确认是否属于成功更改该参数.
    // 从实际角度,如果不更改,那么改单最好不携带.
    // msg.setField(6207, "2222222222");

    // 将订单发送
    FIX::Session::sendToTarget(msg);
}

void ClientApplication::sendCreateMultiOrder(std::map<std::string, std::string>* params)
{
    // 测试多腿
    // f=0 sym=AAPL exg=ISE cur=USD tif=0 sde=1 oty=1 odq=1000 pri=85 rth may=202212 mad=29 stp=310 poc=0 sid=122 ids=0 coq=100 exd=20230115 stx=12.5 eit=R pdf=2 tau=100 edt=SMART mfl=50
    // f (flag): 0-港股 1-股票 2-股票期权 3-期货 4-期货期权 5-差价合约 6-债券 7-外汇 8-多腿 11-美股纳斯达克 12-港指期权 13-港股期货 14-港股期权 
    // sym (symbol):
    // exg (securityExchange): SEHK-联交所 NYSE NASDAQ HKFE-港期所
    // cur (currency): USD HKD
    // tif (timeInForce): 0-DAY 1-GTC 2-ATO 3-IOC(FAK) 4-FOK 6-GTD 7-ATC 8-Auction
    // sde (side): 1-Buy(默认) 2-Sell 5-SellShort
    // oty (ordType): 1-Market(默认) 2-Limit 3-Stop 4-StopLimit
    // odq (ordQty): 1000(默认)
    // pri (price): 85(默认)
    // edt (exDestination)
    // 
    // lsb1 (LegSymbol-1)：腿1-代码，默认和symbol一致，不一致时单独设置
    // lcc1 (LegCFICode-1)：腿1-CFICode，OC – call option, OP – put option, ES – stock
    // lmd1 (LegMaturityDate-1)：腿1-到期日
    // lsp1 (LegStrikePrice-1)：腿1-行权价
    // lpe1 (LegPositionEffect-1)：腿1-仓位，开仓O（Open）、平仓C（Close）
    // lse1 (LegSecurityExchange-1)
    // lrq1 (LegRatioQty-1)：腿1-配比数量
    // lsd1 (LegSide-1)：腿1-方向，1=Buy, 2=Sell
    // lcm1 (LegContractMultiplier-1)
    // ssr1 (ShortSaleRule-1)
    // llr1 (LegLocateReqd-1)
    // llb1 (LegLocateBroke-1)
    // 
    // lsb2 (LegSymbol-2)：腿2-代码，默认和symbol一致，不一致时单独设置
    // lcc2 (LegCFICode-2)：腿2-CFICode，OC – call option, OP – put option, ES – stock
    // lmd2 (LegMaturityDate-2)：腿2-到期日
    // lsp2 (LegStrikePrice-2)：腿2-行权价
    // lpe2 (LegPositionEffect-2)：腿2-仓位，开仓O（Open）、平仓C（Close）
    // lse2 (LegSecurityExchange-2)
    // lrq2 (LegRatioQty-2)：腿2-配比数量
    // lsd2 (LegSide-2)：腿2-方向，1=Buy, 2=Sell
    // lcm2 (LegContractMultiplier-2)
    // ssr2 (ShortSaleRule-2)
    // llr2 (LegLocateReqd-2)
    // llb2 (LegLocateBroke-2)
    // 股票
    std::string symbol{};
    std::string securityExchange{};
    std::string currency{};
    std::string timeInForce{};
    std::string side{ FIX::Side_BUY };
    std::string ordType{ FIX::OrdType_MARKET };
    std::string orderQty{ "1000" };
    std::string price{ "85" };
    std::string exDestination{};
    // 腿1
    std::string legSymbol1{};
    std::string legCFICode1{};
    std::string legMaturityDate1{};
    std::string legStrikePrice1{};
    std::string legPositionEffect1{};
    std::string legSecurityExchange1{};
    std::string legRatioQty1{};
    std::string legSide1{};
    std::string legContractMultiplier1{};
    std::string shortSaleRule1{};
    std::string legLocateReqd1{};
    std::string legLocateBroke1{};
    // 腿2
    std::string legSymbol2{};
    std::string legCFICode2{};
    std::string legMaturityDate2{};
    std::string legStrikePrice2{};
    std::string legPositionEffect2{};
    std::string legSecurityExchange2{};
    std::string legRatioQty2{};
    std::string legSide2{};
    std::string legContractMultiplier2{};
    std::string shortSaleRule2{};
    std::string legLocateReqd2{};
    std::string legLocateBroke2{};

    auto values = *params;
    std::string flag{ "0" };  // 默认执行“0”
    if (values.cend() != values.find("f"))
        flag = values.at("f");

    if (flag == "0") // 期权+期权多腿
    {
        // IB测试的总结，SecurityExchange、Currency都必须携带
        securityExchange = "SMART";
        currency = "USD";
        // symbol = "IBM";
        // 腿1
        legSymbol1 = "IBM"; // 期权+期权多腿，当legSymbol1与symbol一致时，1）只传symbol、2）只传legSymbol1、3）两者都传，都是可以的
        legCFICode1 = "OCXXXX";
        legMaturityDate1 = "20230120";
        legStrikePrice1 = "128";
        legPositionEffect1 = "O";
        legRatioQty1 = "1";
        legSide1 = "1";
        // 腿2
        legSymbol2 = "AAPL"; // 期权+期权多腿，当legSymbol1与symbol一致时，1）只传symbol、2）只传legSymbol1、3）两者都传，都是可以的
        legCFICode2 = "OPXXXX";
        legMaturityDate2 = "20230203";
        legStrikePrice2 = "137";
        legPositionEffect2 = "O";
        legRatioQty2 = "1";
        legSide2 = "1";
    }
    else if (flag == "1") // 期权+股票多腿
    {
        // IB does not support combination orders with  stkopt or optstk ratio above
        // IB测试的总结，SecurityExchange、Currency都必须携带
        securityExchange = "SMART";
        currency = "USD";
        // symbol = "AAPL"; // 期权+期权多腿，，当legSymbol1与symbol一致时，legSymbol1必须传，symbol传与不传都可以
        // 腿1
        legSymbol1 = "IBM";
        legCFICode1 = "OCXXXX";
        legMaturityDate1 = "20230120";
        legStrikePrice1 = "128";
        legPositionEffect1 = "O";
        legRatioQty1 = "1";
        legSide1 = "1";
        // 腿2
        legSymbol2 = "IBM";
        legCFICode2 = "ESXXXX";
        legPositionEffect2 = "O";
        // legSecurityExchange2 = "SMART";
        legRatioQty2 = "1";
        legSide2 = "1";
        legContractMultiplier2 = "100";
        shortSaleRule2 = "2";
        legLocateReqd2 = "N";
        legLocateBroke2 = "N";
    }
    else if (flag == "2") // 股票+股票多腿
    {
        // IB does not support combination orders with  stkopt or optstk ratio above
        // IB测试的总结，SecurityExchange、Currency都必须携带
        securityExchange = "SMART";
        currency = "USD";
        // symbol = "AAPL"; // 期权+期权多腿，，当legSymbol1与symbol一致时，legSymbol1必须传，symbol传与不传都可以
        // 腿1
        legSymbol1 = "IBM";
        legCFICode1 = "ES";
        legPositionEffect1 = "O";
        legRatioQty1 = "1";
        legSide1 = "1";
        // 腿2
        legSymbol2 = "IMB";
        legCFICode2 = "ES";
        legPositionEffect2 = "O";
        legRatioQty2 = "1";
        legSide2 = "2";
    }

    for (const auto& it : values)
    {
        if (it.first == "sym")
        {
            symbol = it.second;
        }
        else if (it.first == "exg")
        {
            securityExchange = it.second;
        }
        else if (it.first == "cur")
        {
            currency = it.second;
        }
        else if (it.first == "tif")
        {
            timeInForce = it.second;
        }
        else if (it.first == "sde")
        {
            side = it.second;
        }
        else if (it.first == "oty")
        {
            ordType = it.second;
        }
        else if (it.first == "odq")
        {
            orderQty = it.second;
        }
        else if (it.first == "pri")
        {
            price = it.second;
        }
        else if (it.first == "edt")
        {
            exDestination = it.second;
        }
        // 腿1
        else if (it.first == "lsb1")
        {
            legSymbol1 = it.second;
        }
        else if (it.first == "lcc1")
        {
            legCFICode1 = it.second;
        }
        else if (it.first == "lmd1")
        {
            legMaturityDate1 = it.second;
        }
        else if (it.first == "lsp1")
        {
            legStrikePrice1 = it.second;
        }
        else if (it.first == "lpe1")
        {
            legPositionEffect1 = it.second;
        }
        else if (it.first == "lse1")
        {
            legSecurityExchange1 = it.second;
        }
        else if (it.first == "lrq1")
        {
            legRatioQty1 = it.second;
        }
        else if (it.first == "lsd1")
        {
            legSide1 = it.second;
        }
        else if (it.first == "lcm1")
        {
            legContractMultiplier1 = it.second;
        }
        else if (it.first == "ssr1")
        {
            shortSaleRule1 = it.second;
        }
        else if (it.first == "llr1")
        {
            legLocateReqd1 = it.second;
        }
        else if (it.first == "llb1")
        {
            legLocateBroke1 = it.second;
        }
        // 腿2
        else if (it.first == "lsb2")
        {
            legSymbol2 = it.second;
        }
        else if (it.first == "lcc2")
        {
            legCFICode2 = it.second;
        }
        else if (it.first == "lmd2")
        {
            legMaturityDate2 = it.second;
        }
        else if (it.first == "lsp2")
        {
            legStrikePrice2 = it.second;
        }
        else if (it.first == "lpe2")
        {
            legPositionEffect2 = it.second;
        }
        else if (it.first == "lse2")
        {
            legSecurityExchange2 = it.second;
        }
        else if (it.first == "lrq2")
        {
            legRatioQty2 = it.second;
        }
        else if (it.first == "lsd2")
        {
            legSide2 = it.second;
        }
        else if (it.first == "lcm2")
        {
            legContractMultiplier2 = it.second;
        }
        else if (it.first == "ssr2")
        {
            shortSaleRule2 = it.second;
        }
        else if (it.first == "llr2")
        {
            legLocateReqd2 = it.second;
        }
        else if (it.first == "llb2")
        {
            legLocateBroke2 = it.second;
        }
    }

    FIX50::NewOrderMultileg msg{};
    // 设置公共的静态字段
    setupStaticFields(msg);

    msg.setField(FIX::ClOrdID{ queryClOrdID() });
    msg.setField(FIX::FIELD::SecurityType, "MULTILEG");
    if (!symbol.empty()) msg.setField(FIX::FIELD::Symbol, symbol);
    msg.setField(FIX::FIELD::OrderQty, orderQty);
    msg.setField(FIX::FIELD::Side, side);
    msg.setField(FIX::FIELD::OrdType, ordType);
    if (!exDestination.empty()) msg.setField(FIX::ExDestination{ exDestination });
    // msg.setField(FIX::FIELD::CustomerOrFirm, "0");
    if (!price.empty()) msg.setField(FIX::FIELD::Price, price);
    if (!timeInForce.empty()) msg.setField(FIX::FIELD::TimeInForce, timeInForce);
    // 仅使用货币无法下单成功, 必须携带交易所.
    if (!currency.empty()) msg.setField(FIX::FIELD::Currency, currency);
    if (!securityExchange.empty()) msg.setField(FIX::FIELD::SecurityExchange, securityExchange);

    FIX50::NewOrderMultileg::NoLegs leg{};
    if (!legSymbol1.empty()) leg.setField(FIX::FIELD::LegSymbol, legSymbol1);
    if (!legCFICode1.empty()) leg.setField(FIX::FIELD::LegCFICode, legCFICode1);
    if (!legMaturityDate1.empty()) leg.setField(FIX::FIELD::LegMaturityDate, legMaturityDate1);
    if (!legStrikePrice1.empty()) leg.setField(FIX::FIELD::LegStrikePrice, legStrikePrice1);
    if (!legPositionEffect1.empty()) leg.setField(FIX::FIELD::LegPositionEffect, legPositionEffect1);
    if (!legSecurityExchange1.empty()) leg.setField(FIX::FIELD::LegSecurityExchange, legSecurityExchange1);
    if (!legRatioQty1.empty()) leg.setField(FIX::FIELD::LegRatioQty, legRatioQty1);
    if (!legSide1.empty()) leg.setField(FIX::FIELD::LegSide, legSide1);
    if (!legContractMultiplier1.empty()) leg.setField(FIX::FIELD::LegContractMultiplier, legContractMultiplier1);
    if (!shortSaleRule1.empty()) leg.setField(6086, shortSaleRule1);
    if (!legLocateReqd1.empty()) leg.setField(6215, legLocateReqd1);
    if (!legLocateBroke1.empty()) leg.setField(6216, legLocateBroke1);
    msg.addGroup(leg);
    leg.clear();

    if (!legSymbol2.empty()) leg.setField(FIX::FIELD::LegSymbol, legSymbol2);
    if (!legCFICode2.empty()) leg.setField(FIX::FIELD::LegCFICode, legCFICode2);
    if (!legMaturityDate2.empty()) leg.setField(FIX::FIELD::LegMaturityDate, legMaturityDate2);
    if (!legStrikePrice2.empty()) leg.setField(FIX::FIELD::LegStrikePrice, legStrikePrice2);
    if (!legPositionEffect2.empty()) leg.setField(FIX::FIELD::LegPositionEffect, legPositionEffect2);
    if (!legSecurityExchange2.empty()) leg.setField(FIX::FIELD::LegSecurityExchange, legSecurityExchange2);
    if (!legRatioQty2.empty()) leg.setField(FIX::FIELD::LegRatioQty, legRatioQty2);
    if (!legSide2.empty()) leg.setField(FIX::FIELD::LegSide, legSide2);
    if (!legContractMultiplier2.empty()) leg.setField(FIX::FIELD::LegContractMultiplier, legContractMultiplier2);
    if (!shortSaleRule2.empty()) leg.setField(6086, shortSaleRule2);
    if (!legLocateReqd2.empty()) leg.setField(6215, legLocateReqd2);
    if (!legLocateBroke2.empty()) leg.setField(6216, legLocateBroke2);
    msg.addGroup(leg);
    leg.clear();

    FIX::Session::sendToTarget(msg);
}

void ClientApplication::sendModifyMultiOrder(std::map<std::string, std::string>* params)
{
    FIX43::MultilegOrderCancelReplaceRequest msg{};

    // 设置公共的静态字段
    setupStaticFields(msg);

    FIX::Session::sendToTarget(msg);
}

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

void ClientApplication::sendErrorCode(std::map<std::string, std::string>* params)
{
    std::string flag{};
    std::string idSource{};
    std::string symbol{};
    std::string currency{};
    std::string securityType{};
    std::string securityExchange{};
    std::string timeInForce{};
    std::string side{ FIX::Side_BUY };
    std::string ordType{ FIX::OrdType_MARKET };
    std::string putOrCall{};
    std::string orderQty{ "1000" };
    std::string price{};
    std::string exDestination{ "0" };
    std::string stopPx{};

    std::string ordStatus{ "0" };
    std::string execType{ "0" };
    std::string text{ "test" };

    auto values = *params;
    if (values.find("f") != values.cend())
    {
        flag = values.at("f");
        if (flag == "0")
        {
            ordStatus = "8";
            execType = "8";
            text = "YOUR ORDER IS NOT ACCEPTED. IN ORDER TO OBTAIN THE DESIRED POSITION YOUR EQUITY WITH LOAN VALUE [369872.44 USD] MUST EXCEED THE INITIAL MARGIN [378741.59 USD]";
        }
        else if (flag == "1")
        {
            ordStatus = "8";
            execType = "8";
            text = "Only closing orders are permitted in this instrument.";
        }
        else if (flag == "2")
        {
            ordStatus = "4";
            execType = "4";
            text = "does not accept odd lot order";
        }
        else if (flag == "3")
        {
            ordStatus = "4";
            execType = "4";
            text = "Non-marketable order does not meet the minimum tier size of";
        }

        symbol = "AAPL";
        // securityExchange = "NYSE"; 不需要指明交易所.
        currency = "USD";
    }
    else
    {
        symbol = "700";
        securityExchange = "SEHK";
    }

    for (const auto& it : values)
    {
        if (it.first == "sty")
        {
            securityType = it.second;
        }
        else if (it.first == "sym")
        {
            symbol = it.second;
        }
        else if (it.first == "exg")
        {
            securityExchange = it.second;
        }
        else if (it.first == "cur")
        {
            currency = it.second;
        }
        else if (it.first == "poc")
        {
            putOrCall = it.second;
        }
        else if (it.first == "tif")
        {
            timeInForce = it.second;
        }
        else if (it.first == "sde")
        {
            side = it.second;
        }
        else if (it.first == "oty")
        {
            ordType = it.second;
        }
        else if (it.first == "odq")
        {
            orderQty = it.second;
        }
        else if (it.first == "pri")
        {
            price = it.second;
        }
        else if (it.first == "stx")
        {
            stopPx = it.second;
        }
        else if (it.first == "edt")
        {
            exDestination = it.second;
        }
    }

    FIX50::NewOrderSingle msg{};

    // 设置公共的静态字段
    setupStaticFields(msg);

    if (!securityType.empty()) msg.setField(FIX::FIELD::SecurityType, securityType);
    // 按照IBFIX协议,Account必须携带.
    // 如果不携带,那么返回错误:Invalid or missing IBCustAcctNo
    // 如果使用master accont账户交易股票/期货/期权,那么返回错误:
    // Submit of non-allocation order to master account I901238 for sectype STK/OPT is not allowed.
    // 从IB的邮件中, 要求所有的订单都必须使用SubAccountID.
    // IB规范中有使用Master Account ID的情况,暂时不需要关注.
    // msg.setField(FIX::Account{ m_ctx.getCfg().getSubAccountIDMap().at("long") });
    // 根据IB的测试邮件通知,股票(不确认是否其它品种,猜测是)最好全部使用SMART.
    // IB文档中,则说明:IBKR does not support SMART routing for futures. The destination exchange must be 
    // specified in tag 100.
    // 目前倾向于通过207区分具体的交易所地址.(尽量避免使用货币, 因为很多交易所可以使用多种货币区分).
    msg.setField(FIX::ExDestination{ exDestination });
    // 如果不携带该值,
    // 那么返回拒绝, 错误原因:Invalid BD flag or Origin
    // 从IB的测试邮件中, 要求必须为0.
    msg.setField(FIX::CustomerOrFirm{ FIX::CustomerOrFirm_CUSTOMER });
    // 按照IBFIX协议,下美股IBM,且100=SMART的时候,如果ExDestination是SMART时,
    // 1. SecurityExchange和Currency都不携带,
    // 那么返回拒绝,错误原因是:Ambiguous Contract.
    // 2. 仅携带SecurityExchange时,如果值错误,
    // 那么返回拒绝,错误原因是:Invalid destination exchange specified
    // 文档说参考FIX 4.2规范的Appendix C. 如链接:
    // https://www.onixs.biz/fix-dictionary/4.2/app_c.html
    // 实际上不是参考该规范.
    // 例如纽交所代码, 按照FIX 4.2附录C的规范,应该填N,但实际上应该填写NYSE.
    // 从实际看, 该参数值实际需要参考TWS上Primary Exchange代码.
    // 目前已知NYSE(纽交所)/NASDAQ/SEHK(联交所)/HKFE(港期所)
    // 
    // 下美股MSFT的个股期权,且100=SMART的时候,不使用207就能下单成功.
    // 如果填写207的值, 必须填写正确的交易所CBOE, 否则报错误:Unknown contract
    //
    // 如果是美股, 部分股票可以填写NYSE或者NASDAQ都可以.
    // 大部分必须使用正确的上市交易所, 否则返回错误:Invalid destination exchange specified
    //
    // 综上所述, 当使用SMART的时候, 美国交易所的代码, 优先使用Currency,即USD.
    // 非美国交易所, 仅使用货币无效, 基本必须使用SecurityExchange区分.
    if (!securityExchange.empty()) msg.setField(FIX::FIELD::SecurityExchange, securityExchange);
    // 货币本来应该是指定价格的货币.但是没有在FIX标准中找到Currency具体对应下单Price的货币,还是行情Price的货币.
    // 目前IB实际上用于指定行情Price的货币.
    // securityExchange和currency是"或"关系.可以两者都存在.如果与都存在,那么必须都正确,
    // 否则返回:Unknown contract
    if (!currency.empty()) msg.setField(FIX::FIELD::Currency, currency);
    // 如果Symbol填写错误, 将返回错误:Unknown contract.
    // 港股代码必须与交易所等同,不能包含前缀0.
    // 根据文档理解, 衍生品代码有两种下单方式,我们统一使用标准FIX的下单方式.
    if (!symbol.empty()) msg.setField(FIX::Symbol{ symbol });
    if (!idSource.empty()) msg.setField(FIX::IDSource{ idSource });
    
    msg.setField(FIX::FIELD::OrdType, ordType);
    if (!timeInForce.empty()) msg.setField(FIX::FIELD::TimeInForce, timeInForce);
    // 按照规范, 支持做空. 但是UAT环境一直返回错误:Sell short variant is not supported
    msg.setField(FIX::FIELD::Side, side);
    // 市价单的时候,一定不能携带该值. 否则将返回错误的消息:Invalid value in field # 44
    if (!price.empty()) msg.setField(FIX::FIELD::Price, price);
    if (!stopPx.empty()) msg.setField(FIX::FIELD::StopPx, stopPx);
    // 如果OrderQty不符合交易所交易单位要求, 返回错误消息:order size must be a multiple of lot size
    msg.setField(FIX::FIELD::OrderQty, orderQty);
    msg.setField(FIX::ClOrdID{ queryClOrdID() });
    // 根据IB邮件说明, 最好携带该值, 表明综合账户的子账户.
    msg.setField(6207, "11111111");

    msg.setField(FIX::FIELD::OrderID, "11111111");
    msg.setField(FIX::FIELD::ExecID, "11111111");
    msg.setField(FIX::FIELD::OrdStatus, ordStatus);
    msg.setField(FIX::FIELD::ExecType, execType);
    msg.setField(FIX::FIELD::Text, text);

    // 将订单发送
    FIX::Session::sendToTarget(msg);
}