#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#include "quickfix/config.h"
#include "quickfix/Session.h"
#include <iostream>
#include <chrono>
#include "ClientApplication.h"

// 1: 下单测试，2：期权测试，3：执行测试用例
#define STSRT_ACTION '3'

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

ClientApplication::ClientApplication()
{
    auto nowTime = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    auto now = std::localtime(&nowTime);
    char buf[32]{};
    std::sprintf(buf, "%02d%02d%02d%02d%02d", now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    m_prefix = buf;
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
                    << "1) 手动下单" << std::endl
                    << "2) 测试期权" << std::endl
                    << "3) 执行测试用例" << std::endl
                    << "4) 退出" << std::endl
                    << std::endl;

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
                // 开始测试
                startTestAction();
            else if (action == '2')
                // 开始期权测试
                startOptionAction();
            else if (action == '3')
                // 开始执行测试用例
                startTestCaseAction();
            else if (action == '4')
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
        << "BeginString: " << std::endl
        << std::endl;

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

bool ClientApplication::queryConfirm(const std::string& query)
{
    std::cout << std::endl
        << query << "? (Y/N) " << std::endl
        << std::endl;

    std::string value;
    std::cin >> value;
    return toupper(*value.c_str()) == 'Y';
}

FIX::SenderCompID ClientApplication::querySenderCompID()
{
    std::cout << std::endl
        << "SenderCompID: " << std::endl
        << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::SenderCompID(value);
}

FIX::TargetCompID ClientApplication::queryTargetCompID()
{
    std::cout << std::endl
        << "TargetCompID: " << std::endl
        << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::TargetCompID(value);
}

FIX::TargetSubID ClientApplication::queryTargetSubID()
{
    std::cout << std::endl
        << "TargetSubID: " << std::endl
        << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::TargetSubID(value);
}

// 终端分配的订单的唯一标识符。
FIX::ClOrdID ClientApplication::queryClOrdID()
{
    std::cout << std::endl
        << "ClOrdID: " << std::endl
        << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::ClOrdID(value);
}

// 取消或替换订单时，需要之前的订单的ClOrdID <11>
FIX::OrigClOrdID ClientApplication::queryOrigClOrdID()
{
    std::cout << std::endl
        << "OrigClOrdID: " << std::endl
        << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::OrigClOrdID(value);
}

FIX::Symbol ClientApplication::querySymbol()
{
    std::cout << std::endl
        << "Symbol: " << std::endl
        << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::Symbol(value);
}

FIX::Side ClientApplication::querySide()
{
    std::cout << std::endl
        << "1) Buy" << std::endl
        << "2) Sell" << std::endl
        << "3) Sell Short" << std::endl
        << "4) Sell Short Exempt" << std::endl
        << "5) Cross" << std::endl
        << "6) Cross Short" << std::endl
        << "7) Cross Short Exempt" << std::endl
        << "Side: " << std::endl
        << std::endl;

    char value;
    std::cin >> value;
    switch (value)
    {
        case '1': return FIX::Side(FIX::Side_BUY);
        case '2': return FIX::Side(FIX::Side_SELL);
        case '3': return FIX::Side(FIX::Side_SELL_SHORT);
        case '4': return FIX::Side(FIX::Side_SELL_SHORT_EXEMPT);
        case '5': return FIX::Side(FIX::Side_CROSS);
        case '6': return FIX::Side(FIX::Side_CROSS_SHORT);
        case '7': return FIX::Side('A');
        default: throw std::exception();
    }
}

FIX::OrderQty ClientApplication::queryOrderQty()
{
    std::cout << std::endl
        << "OrderQty: " << std::endl
        << std::endl;

    long value;
    std::cin >> value;
    return FIX::OrderQty(value);
}

FIX::OrdType ClientApplication::queryOrdType()
{
    std::cout << std::endl
        << "1) Market" << std::endl
        << "2) Limit" << std::endl
        << "3) Stop" << std::endl
        << "4) Stop Limit" << std::endl
        << "OrdType: " << std::endl
        << std::endl;

    char value;
    std::cin >> value;
    switch (value)
    {
        case '1': return FIX::OrdType(FIX::OrdType_MARKET);
        case '2': return FIX::OrdType(FIX::OrdType_LIMIT);
        case '3': return FIX::OrdType(FIX::OrdType_STOP);
        case '4': return FIX::OrdType(FIX::OrdType_STOP_LIMIT);
        default: throw std::exception();
    }
}

FIX::Price ClientApplication::queryPrice()
{
    std::cout << std::endl
        << "Price: " << std::endl
        << std::endl;

    double value;
    std::cin >> value;
    return FIX::Price(value);
}

FIX::StopPx ClientApplication::queryStopPx()
{
    std::cout << std::endl
        << "StopPx: " << std::endl
        << std::endl;

    double value;
    std::cin >> value;
    return FIX::StopPx(value);
}

FIX::TimeInForce ClientApplication::queryTimeInForce()
{
    std::cout << std::endl
        << "1) Day" << std::endl
        << "2) IOC" << std::endl
        << "3) OPG" << std::endl
        << "4) GTC" << std::endl
        << "5) GTX" << std::endl
        << "TimeInForce: " << std::endl
        << std::endl;

    char value;
    std::cin >> value;
    switch (value)
    {
        case '1': return FIX::TimeInForce(FIX::TimeInForce_DAY);
        case '2': return FIX::TimeInForce(FIX::TimeInForce_IMMEDIATE_OR_CANCEL);
        case '3': return FIX::TimeInForce(FIX::TimeInForce_AT_THE_OPENING);
        case '4': return FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL);
        case '5': return FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CROSSING);
        default: throw std::exception();
    }
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

std::string ClientApplication::getNewClOrdID()
{
    return m_prefix + std::to_string(++m_count);
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
// SenderCompID <49> field
// SendingTime <52> field
// Side <54> field
// Symbol <55> field
// TargetCompID <56> field
// TimeInForce <59> field
// TransactTime <60> field
// ExDestination <100> field
// SecurityExchange <207> field
// CheckSum <10> field
//
void ClientApplication::startTestAction()
{
    std::cout << std::endl
        << "请选择: " << std::endl
        << "1) New Order" << std::endl
        << "2) Modify Order" << std::endl
        << "3) Cancel Order" << std::endl
        << "4) Market data test" << std::endl
        << std::endl;

    char action;
    std::cin >> action;
    switch (action)
    {
        case '1': case '2': case '3': case '4': case '5': break;
        default: throw std::exception();
    }

    if (action == '1')
        // 创建新订单
        testNewOrder();
    else if (action == '2')
        // 取消已有订单
        testCancelOrder();
    else if (action == '3')
        // 修改已有订单
        testModifyOrder();
    else if (action == '4')
        // 注意：查询订单，业务上用不到
        testMarketDataRequest();
    else
        throw std::exception();
}

void ClientApplication::setupTestMessage(FIX::Message& message)
{
    FIX::Header& header = message.getHeader();
    header.setField(49, "client1"); // SenderCompID <49> field，
    header.setField(56, "gateway"); // TargetCompID <56> field，

    message.setField(20005, "20"); // IB Gateway
}

void ClientApplication::testNewOrder()
{
    FIX::OrdType ordType;
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    // 设置message公共字段
    setupTestMessage(message);

    message.setField(1, "1111");   // Account <1> field，
    message.setField(55, "LMT");   // Symbol <55> field, 股票代码
    message.setField(100, "0");    // ExDestination <100> field, 固定传"0"，同时还必须设置15或207
    message.setField(15, "USD");   // Currency <15> field，
    message.setField(207, "US");   // SecurityExchange <207> field, SSE | ZSSE | HKEX | SGX | US | USOTC

    message.setField(queryOrderQty());
    message.setField(queryTimeInForce());
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(queryPrice());
    
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(queryStopPx());

    

    if (queryConfirm("Send enter order"))
        // 将订单发送
        FIX::Session::sendToTarget(message);
}

void ClientApplication::testModifyOrder()
{
    FIX50::OrderCancelReplaceRequest message(
        queryOrigClOrdID(),
        queryClOrdID(),
        querySide(),
        FIX::TransactTime(),
        queryOrdType()
    );

    // 设置message公共字段
    setupTestMessage(message);

    message.setField(1, "1111");   // Account <1> field，
    message.setField(55, "LMT");   // Symbol <55> field, 股票代码
    message.setField(100, "0");    // ExDestination <100> field, 固定传"0"，同时还必须设置15或207
    message.setField(15, "USD");   // Currency <15> field，
    message.setField(207, "US");   // SecurityExchange <207> field, SSE | ZSSE | HKEX | SGX | US | USOTC

    if (queryConfirm("New price"))
        message.setField(queryPrice());

    message.setField(queryOrderQty());

    if (queryConfirm("Send replace order"))
        // 将订单发送
        FIX::Session::sendToTarget(message);
}

void ClientApplication::testCancelOrder()
{
    FIX50::OrderCancelRequest message(
        queryOrigClOrdID(),
        queryClOrdID(),
        querySide(),
        FIX::TransactTime()
    );

    // 设置message公共字段
    setupTestMessage(message);

    if (queryConfirm("Send cancel order"))
        // 将订单发送
        FIX::Session::sendToTarget(message);
}

void ClientApplication::testMarketDataRequest()
{
    FIX::Message message;

    // 手动输入Fix版本，现在用不到了，直接用FIX50
    int version = queryVersion();
    switch (version)
    {
        case 42:
            // 注意：查询订单，业务上用不到，所以Fix42也没有示例
            std::cerr << "No test for version " << version << std::endl;
        default:
            std::cerr << "No test for version " << version << std::endl;
            break;
    }

    FIX::Session::sendToTarget(message);
}

void ClientApplication::startOptionAction()
{
    std::cout << std::endl
        << "请选择: " << std::endl
        << "1) 联交所港股期权" << std::endl
        << "2) 美股期权" << std::endl
        << "3) 港期所指数期权" << std::endl
        << "4) 港期所期货" << std::endl
        << std::endl;

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

void ClientApplication::setupOptionMessage(FIX::Message& message)
{
    FIX::Header& header = message.getHeader();
    header.setField(49, "client1"); // SenderCompID <49> field，
    header.setField(56, "gateway"); // TargetCompID <56> field，

    message.setField(20005, "20"); // IB Gateway
    message.setField(1, "1111");   // Account <1> field，
    message.setField(55, "LMT");   // Symbol <55> field, 股票代码
    message.setField(100, "0");    // ExDestination <100> field, 固定传"0"，同时还必须设置15或207
    message.setField(15, "USD");   // Currency <15> field，
    message.setField(207, "US");   // SecurityExchange <207> field, SSE | ZSSE | HKEX | SGX | US | USOTC
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
// SenderCompID <49> field
// SendingTime <52> field
// Side <54> field
// Symbol <55> field
// TargetCompID <56> field
// TimeInForce <59> field
// TransactTime <60> field
// ExDestination <100> field
// SecurityExchange <207> field
// CheckSum <10> field
//
void ClientApplication::testOption1()
{
    FIX::OrdType ordType;
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    // 设置message公共字段
    setupOptionMessage(message);
    message.setField(FIX::OrderQty(1000));
    message.setField(FIX::TimeInForce(FIX::TimeInForce_DAY));

    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(queryStopPx());

    message.setField(167, "OPT"); // SecurityTyp
    message.setField(55, "00700"); // Symbol
    message.setField(207, "HKEX"); // SecurityExchange
    message.setField(200, "202212"); // MaturityMonthYear
    message.setField(205, "29"); // MaturityDay
    message.setField(202, "140"); // strikePrice
    message.setField(201, "1"); // PutOrCall

    message.setField(15, "HKD");

    
    if (queryConfirm("创建订单，发送"))
        // 将订单发送
        FIX::Session::sendToTarget(message);
}

// 测试期权2
void ClientApplication::testOption2()
{
    FIX::OrdType ordType;
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    // 设置message公共字段
    setupOptionMessage(message);
    message.setField(FIX::OrderQty(1000));
    message.setField(FIX::TimeInForce(FIX::TimeInForce_DAY));

    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(queryStopPx());

    message.setField(167, "OPT"); // SecurityTyp
    message.setField(55, "MSFT"); // Symbol
    // message.setField(207, "SEHK"); // SecurityExchange
    message.setField(15, "USD"); // Currency
    message.setField(200, "202301"); // MaturityMonthYear
    message.setField(205, "6"); // MaturityDay
    message.setField(202, "310"); // strikePrice
    message.setField(201, "1"); // PutOrCall


    if (queryConfirm("创建订单，发送"))
        // 将订单发送
        FIX::Session::sendToTarget(message);
}

// 测试期权3
void ClientApplication::testOption3()
{
    FIX::OrdType ordType;
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    // 设置message公共字段
    setupOptionMessage(message);
    message.setField(FIX::OrderQty(1000));
    message.setField(FIX::TimeInForce(FIX::TimeInForce_DAY));

    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(queryStopPx());

    message.setField(167, "OPT"); // SecurityTyp
    message.setField(55, "HSI"); // Symbol
    message.setField(207, "HKEX"); // SecurityExchange
    message.setField(200, "202212"); // MaturityMonthYear
    message.setField(205, "29"); // MaturityDay
    message.setField(202, "19000"); // strikePrice
    message.setField(201, "1"); // PutOrCall


    if (queryConfirm("创建订单，发送"))
        // 将订单发送
        FIX::Session::sendToTarget(message);
}

// 测试期货
void ClientApplication::testFuture()
{
    FIX::OrdType ordType;
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    // 设置message公共字段
    setupOptionMessage(message);
    message.setField(FIX::OrderQty(1000));
    message.setField(FIX::TimeInForce(FIX::TimeInForce_DAY));

    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(queryStopPx());

    message.setField(167, "FUT"); // SecurityType
    message.setField(55, "HSI"); // Symbol
    message.setField(207, "HKEX"); // SecurityExchange
    message.setField(200, "202212"); // MaturityMonthYear
    // message.setField(205, ""); // MaturityDay
    // message.setField(202, ""); // strikePrice
    // message.setField(201, ""); // PutOrCall


    if (queryConfirm("创建订单，发送"))
        // 将订单发送
        FIX::Session::sendToTarget(message);
}

void ClientApplication::startTestCaseAction()
{
    std::cout << std::endl
        << "请选择: " << std::endl
        << "1) Buy LMT 1000 @ SMART" << std::endl
        << "2) Modify LMT Order" << std::endl
        << "3) Cancel LMT Order" << std::endl
        << std::endl;

    char action;
    std::cin >> action;
    switch (action)
    { 
        case '1': case '2': case '3': case '4': case '5': break;
        default: throw std::exception();
    }

    if (action == '1')
        testCaseAction1();
    else if (action == '2')
        testCaseAction2();
    else if (action == '3')
        testCaseAction3();
    else
        throw std::exception();
}

// 设置公共的静态字段
void ClientApplication::setupTestCaseMessage(FIX::Message& message)
{
    FIX::Header& header = message.getHeader();
    header.setField(FIX::FIELD::SenderCompID, "client1"); // SenderCompID <49> field，
    header.setField(FIX::FIELD::TargetCompID, "gateway"); // TargetCompID <56> field，

    message.setField(20005, "20"); // IB Gateway
    message.setField(FIX::FIELD::ExDestination, "0"); // 智能路由，固定传"0"，同时还必须设置Currency(15) 或者 SecurityExchange(207)
    message.setField(FIX::FIELD::Currency, "USD");
    message.setField(FIX::FIELD::SecurityExchange, "US"); // 取值范围：SSE | ZSSE | HKEX | SGX | US | USOTC
}

// Repeat above steps for the different Time In Force (DAY,GTC,GTD,GTT,GAT,FOK,AON,IOC,MOO,MOC,LOC,LOO)
// TimeInForce: "0"~"9"
//
// Repeat above steps for the different Order Types such as (STP + STP LMT)
// OrderType: "1"~"4"
// case '1': return FIX::OrdType(FIX::OrdType_MARKET); 
// case '2': return FIX::OrdType(FIX::OrdType_LIMIT);
// case '3': return FIX::OrdType(FIX::OrdType_STOP);
// case '4': return FIX::OrdType(FIX::OrdType_STOP_LIMIT);

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
void ClientApplication::testCaseAction1()
{
    // 动态字段
    FIX::ClOrdID clOrdID = getNewClOrdID();
    m_side = FIX::Side_BUY;
    m_ordType = FIX::OrdType_MARKET;
    m_timeInForce = FIX::TimeInForce_DAY;
    m_symbol = "LMT";
    m_account = "12345678";

    FIX::OrderQty orderQty = 1000;
    FIX::Price price = 1000;
    FIX::StopPx stopPx = 499;
    
    FIX50::NewOrderSingle message{};

    setupTestCaseMessage(message); // 设置公共的静态字段
    message.setField(FIX::FIELD::ClOrdID, clOrdID);
    message.setField(FIX::FIELD::Account, m_account);
    message.setField(FIX::FIELD::Side, m_side);
    message.setField(FIX::FIELD::Symbol, m_symbol);
    message.setField(FIX::TransactTime());

    // 测试内容
    message.setField(FIX::TimeInForce(m_timeInForce));
    message.setField(FIX::OrdType(m_ordType));

    message.setField(FIX::OrderQty(orderQty));
    if (m_ordType == FIX::OrdType_LIMIT || m_ordType == FIX::OrdType_STOP_LIMIT)
        message.setField(FIX::Price(price));  // 限价类型要传Price
    if (m_ordType == FIX::OrdType_STOP || m_ordType == FIX::OrdType_STOP_LIMIT)
        message.setField(FIX::StopPx(stopPx)); // 止损类型要传StopPx

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

void ClientApplication::testCaseAction2()
{
    // 动态字段
    FIX::ClOrdID clOrdID = getNewClOrdID();
    FIX::OrigClOrdID origClOrdID = m_clOrdID;

    FIX::OrderQty orderQty = 900;
    FIX::Price price = 90;
    FIX::StopPx stopPx = 49;

    FIX50::OrderCancelReplaceRequest message{};

    setupTestCaseMessage(message); // 设置公共的静态字段
    message.setField(FIX::FIELD::OrigClOrdID, origClOrdID);
    message.setField(FIX::FIELD::ClOrdID, clOrdID);
    message.setField(FIX::FIELD::Account, m_account);
    message.setField(FIX::FIELD::Side, m_side);
    message.setField(FIX::FIELD::Symbol, m_symbol);
    message.setField(FIX::TransactTime());
    
    // 测试内容
    message.setField(FIX::TimeInForce(m_timeInForce)); // 改单时如果不带，IB会改为默认：当天订单
    message.setField(FIX::OrdType(m_ordType));

    message.setField(FIX::OrderQty(orderQty));
    if (m_ordType == FIX::OrdType_LIMIT || m_ordType == FIX::OrdType_STOP_LIMIT)
        message.setField(FIX::Price(price));  // 限价类型要传Price
    if (m_ordType == FIX::OrdType_STOP || m_ordType == FIX::OrdType_STOP_LIMIT)
        message.setField(FIX::StopPx(stopPx)); // 止损类型要传StopPx

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

void ClientApplication::testCaseAction3()
{
    // 动态字段
    FIX::ClOrdID clOrdID = getNewClOrdID();
    FIX::OrigClOrdID origClOrdID = m_clOrdID;

    FIX50::OrderCancelRequest message{};

    setupTestCaseMessage(message); // 设置公共的静态字段
    message.setField(FIX::FIELD::OrigClOrdID, origClOrdID);
    message.setField(FIX::FIELD::ClOrdID, clOrdID);
    message.setField(FIX::FIELD::Account, m_account);
    message.setField(FIX::FIELD::Side, m_side);
    message.setField(FIX::TransactTime());

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

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

    // 根据IB文档, 如果下单时没有填写账户, 那么拒绝(35=8,39=8)的消息中, 不会携带账户.
    if (message.isSetField(FIX::FIELD::Account))
    {
        m_account = message.getField(FIX::FIELD::Account);
    }

    // 根据IB文档, 当field 207填写错误的时候, 返回错误消息:Invalid destination exchange specified
    // 此时35=8,39=8的消息不携带54. 故需要判断.
    if (message.isSetField(FIX::FIELD::OrdType))
    {
        std::string ordType = message.getField(FIX::FIELD::OrdType);
        m_ordType = ordType.empty() ? NULL : ordType[0];
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

    // 根据IB文档, 当field 207填写错误的时候, 返回错误消息:Invalid destination exchange specified
    // 此时35=8,39=8的消息不携带54. 故需要判断, 且最好修改IBFIX42.xml.
    if (message.isSetField(FIX::FIELD::Side))
    {
        m_side = message.getField(FIX::FIELD::Side);
        msg.setField(FIX::FIELD::Side, m_side);
    }

    // 实际测试时发现, 当MaturityMonthYear格式不是YYYYMM时, 将返回:Invalid value in field # 205(MaturityDay).
    // 此时35=8,39=8的消息不携带55.
    if (message.isSetField(FIX::FIELD::Symbol))
    {
        m_symbol = message.getField(FIX::FIELD::Symbol);
        msg.setField(FIX::FIELD::Symbol, m_symbol);
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