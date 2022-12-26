#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#include "quickfix/config.h"
#include "quickfix/Session.h"
#include <iostream>

#include "ClientApplication.h"

#define FIX_VERSION           42
#define GATEWAY_SPDR          "19"
#define GATEWAY_IBFIX         "20"

void ClientApplication::onLogon(const FIX::SessionID& sessionID)
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "Logon - " << sessionID << std::endl;
    // 还原颜色
    resetConsoleTextColor();
}

void ClientApplication::onLogout(const FIX::SessionID& sessionID)
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "Logout - " << sessionID << std::endl;
    // 还原颜色
    resetConsoleTextColor();
}

void ClientApplication::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    crack(message, sessionID);

    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "IN: " << message << std::endl;
    // 还原颜色
    resetConsoleTextColor();
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

    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "OUT: " << message << std::endl;
    // 还原颜色
    resetConsoleTextColor();
}

void ClientApplication::onMessage(const FIX42::ExecutionReport&, const FIX::SessionID&) {}
void ClientApplication::onMessage(const FIX42::OrderCancelReject&, const FIX::SessionID&) {}

void ClientApplication::run()
{
    while (true)
    {
        try
        {
            // 设置绿色
            setConsoleTextColor();
            std::cout << std::endl
                << "1) 手动下单" << std::endl
                << "2) 测试期权" << std::endl
                << "3) 执行测试用例" << std::endl
                << "4) 退出" << std::endl
                << "请选择: ";
            // 还原颜色
            resetConsoleTextColor();

            char action;
            std::cin >> action;
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
            // 设置绿色
            setConsoleTextColor();
            std::cout << std::endl
                << "Message Not Sent: " << e.what();
            // 还原颜色
            resetConsoleTextColor();
        }
    }
}

// 手动输入Fix版本
int ClientApplication::queryVersion()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "1) FIX.4.0" << std::endl
        << "2) FIX.4.1" << std::endl
        << "3) FIX.4.2" << std::endl
        << "4) FIX.4.3" << std::endl
        << "5) FIX.4.4" << std::endl
        << "6) FIXT.1.1 (FIX.5.0)" << std::endl
        << "BeginString: ";
    // 还原颜色
    resetConsoleTextColor();

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

// 修改控制台文字颜色
void ClientApplication::setConsoleTextColor()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
}

// 还原控制台文字颜色
void ClientApplication::resetConsoleTextColor()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

bool ClientApplication::queryConfirm(const std::string& query)
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << query << "? (Y/N) ";
    // 还原颜色
    resetConsoleTextColor();

    std::string value;
    std::cin >> value;
    return toupper(*value.c_str()) == 'Y';
}

FIX::SenderCompID ClientApplication::querySenderCompID()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "SenderCompID: ";
    // 还原颜色
    resetConsoleTextColor();

    std::string value;
    std::cin >> value;
    return FIX::SenderCompID(value);
}

FIX::TargetCompID ClientApplication::queryTargetCompID()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "TargetCompID: ";
    // 还原颜色
    resetConsoleTextColor();

    std::string value;
    std::cin >> value;
    return FIX::TargetCompID(value);
}

FIX::TargetSubID ClientApplication::queryTargetSubID()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "TargetSubID: ";
    // 还原颜色
    resetConsoleTextColor();

    std::string value;
    std::cin >> value;
    return FIX::TargetSubID(value);
}

// 机构分配的订单的唯一标识符。
FIX::ClOrdID ClientApplication::queryClOrdID()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "ClOrdID: ";
    // 还原颜色
    resetConsoleTextColor();

    std::string value;
    std::cin >> value;
    return FIX::ClOrdID(value);
}

// 取消或替换订单时，需要之前的订单的ClOrdID <11>
FIX::OrigClOrdID ClientApplication::queryOrigClOrdID()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "OrigClOrdID: ";
    // 还原颜色
    resetConsoleTextColor();

    std::string value;
    std::cin >> value;
    return FIX::OrigClOrdID(value);
}

FIX::Symbol ClientApplication::querySymbol()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "Symbol: ";
    // 还原颜色
    resetConsoleTextColor();

    std::string value;
    std::cin >> value;
    return FIX::Symbol(value);
}

FIX::Side ClientApplication::querySide()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "1) Buy" << std::endl
        << "2) Sell" << std::endl
        << "3) Sell Short" << std::endl
        << "4) Sell Short Exempt" << std::endl
        << "5) Cross" << std::endl
        << "6) Cross Short" << std::endl
        << "7) Cross Short Exempt" << std::endl
        << "Side: ";
    // 还原颜色
    resetConsoleTextColor();

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
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "OrderQty: ";
    // 还原颜色
    resetConsoleTextColor();

    long value;
    std::cin >> value;
    return FIX::OrderQty(value);
}

FIX::OrdType ClientApplication::queryOrdType()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "1) Market" << std::endl
        << "2) Limit" << std::endl
        << "3) Stop" << std::endl
        << "4) Stop Limit" << std::endl
        << "OrdType: ";
    // 还原颜色
    resetConsoleTextColor();

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
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "Price: ";
    // 还原颜色
    resetConsoleTextColor();

    double value;
    std::cin >> value;
    return FIX::Price(value);
}

FIX::StopPx ClientApplication::queryStopPx()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "StopPx: ";
    // 还原颜色
    resetConsoleTextColor();

    double value;
    std::cin >> value;
    return FIX::StopPx(value);
}

FIX::TimeInForce ClientApplication::queryTimeInForce()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "1) Day" << std::endl
        << "2) IOC" << std::endl
        << "3) OPG" << std::endl
        << "4) GTC" << std::endl
        << "5) GTX" << std::endl
        << "TimeInForce: ";
    // 还原颜色
    resetConsoleTextColor();

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

void ClientApplication::setupHeader(FIX::Header& header)
{
    header.setField(49, "client1"); // SenderCompID <49> field，
    header.setField(56, "gateway"); // TargetCompID <56> field，
}

void ClientApplication::setupMessage(FIX::Message& message)
{
    // IBFIX网关测试
    message.setField(20005, GATEWAY_IBFIX); // IB Gateway

    message.setField(1, "11111111"); // Account <1> field，
    message.setField(55, "LMT"); // Symbol <55> field, 股票代码
    message.setField(100, "0");  // ExDestination <100> field, 固定传"0"，同时还必须设置15或207
    message.setField(15, "USD"); // Currency <15> field，
    message.setField(207, "US"); // SecurityExchange <207> field, SSE | ZSSE | HKEX | SGX | US | USOTC
}

void ClientApplication::startTestAction()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "1) New Order" << std::endl
        << "2) Cancel Order" << std::endl
        << "3) Replace Order" << std::endl
        << "4) Market data test" << std::endl
        << "5) Quit" << std::endl
        << "Action: ";
    // 还原颜色
    resetConsoleTextColor();

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
        testReplaceOrder();
    else if (action == '4')
        // 注意：查询订单，业务上用不到
        testMarketDataRequest();
    else
        return;
}

void ClientApplication::testNewOrder()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "\nNewOrderSingle\n";
    // 还原颜色
    resetConsoleTextColor();

    // 构造创建消息：
    // BeginString <8>, BodyLength <9>, MsgType <35>, MsgSeqNum <34>, SenderCompID <49>,
    // SendingTime <52>, TargetCompID <56>, ClOrdID <11>, HandlInst <21>, OrderQty <38>,
    // OrdType <40>, Side <54>, Symbol <55>, TimeInForce <59>, TransactTime <60>, CheckSum <10>
    FIX::OrdType ordType;
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    message.set(queryOrderQty());
    message.set(queryTimeInForce());
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
    {
        message.set(queryPrice());
    }
    else if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
    {
        message.set(queryStopPx());
    }
    else
    {
        // do nothing
    }

    // 设置消息公共的field
    setupMessage(message);

    // 设置消息header
    setupHeader(message.getHeader());

    if (queryConfirm("Send enter order"))
    {
        // 将订单发送
        FIX::Session::sendToTarget(message);
    }
}

void ClientApplication::testCancelOrder()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "\nOrderCancelRequest\n";
    // 还原颜色
    resetConsoleTextColor();

    // 构造创建消息：
    // BeginString <8>, BodyLength <9>, MsgType <35>, MsgSeqNum <34>, SenderCompID <49>,
    // SendingTime <52>, TargetCompID <56>, ClOrdID <11>, OrderQty <38>,
    // OrigClOrdID <41>, Side <54>, Symbol <55>, TransactTime <60>, CheckSum <10>
    FIX50::OrderCancelRequest message(
        queryOrigClOrdID(),
        queryClOrdID(),
        querySide(),
        FIX::TransactTime()
    );

    message.set(queryOrderQty());

    // 设置消息公共的field
    setupMessage(message);

    // 设置消息header
    setupHeader(message.getHeader());

    if (queryConfirm("Send cancel order"))
    {
        // 将订单发送
        FIX::Session::sendToTarget(message);
    }
}

void ClientApplication::testReplaceOrder()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "\nCancelReplaceRequest\n";
    // 还原颜色

    resetConsoleTextColor();
    FIX50::OrderCancelReplaceRequest message(
        queryOrigClOrdID(),
        queryClOrdID(),
        querySide(),
        FIX::TransactTime(),
        queryOrdType()
    );

    if (queryConfirm("New price"))
    {
        message.set(queryPrice());
    }

    message.set(queryOrderQty());

    // 设置消息公共的field
    setupMessage(message);

    // 设置消息header
    setupHeader(message.getHeader());

    if (queryConfirm("Send replace order"))
    {
        // 将订单发送
        FIX::Session::sendToTarget(message);
    }
}

void ClientApplication::testMarketDataRequest()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "\nMarketDataRequest\n";
    // 还原颜色
    resetConsoleTextColor();

    // 手动输入Fix版本，现在用不到了，直接用FIX50
    int version = queryVersion();

    FIX::Message md;

    switch (version) {
    case 42:
        // 注意：查询订单，业务上用不到，所以Fix42也没有示例
        std::cerr << "No test for version " << version << std::endl;
    default:
        std::cerr << "No test for version " << version << std::endl;
        break;
    }

    FIX::Session::sendToTarget(md);
}

void ClientApplication::startOptionAction()
{
    // 设置绿色
    setConsoleTextColor();
    std::cout << std::endl
        << "1) 联交所港股期权" << std::endl
        << "2) 美股期权" << std::endl
        << "3) 港期所指数期权" << std::endl
        << "4) 港期所期货" << std::endl
        << "5) 退出" << std::endl
        << "请选择: ";
    // 还原颜色
    resetConsoleTextColor();

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
        return;
}

// 测试期权1
void ClientApplication::testOption1()
{
    FIX::OrdType ordType;

    // 构造创建消息：
    // BeginString <8>, BodyLength <9>, MsgType <35>, MsgSeqNum <34>, SenderCompID <49>,
    // SendingTime <52>, TargetCompID <56>, ClOrdID <11>, HandlInst <21>, OrderQty <38>,
    // OrdType <40>, Side <54>, Symbol <55>, TimeInForce <59>, TransactTime <60>, CheckSum <10>
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    message.set(queryOrderQty());
    message.set(queryTimeInForce());
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.set(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.set(queryStopPx());

    // 设置消息公共的field
    setupMessage(message);

    // 设置消息header
    setupHeader(message.getHeader());

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

    // 构造创建消息：
    // BeginString <8>, BodyLength <9>, MsgType <35>, MsgSeqNum <34>, SenderCompID <49>,
    // SendingTime <52>, TargetCompID <56>, ClOrdID <11>, HandlInst <21>, OrderQty <38>,
    // OrdType <40>, Side <54>, Symbol <55>, TimeInForce <59>, TransactTime <60>, CheckSum <10>
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    message.set(queryOrderQty());
    message.set(queryTimeInForce());
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.set(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.set(queryStopPx());

    // 设置消息公共的field
    setupMessage(message);

    // 设置消息header
    setupHeader(message.getHeader());

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

    // 构造创建消息：
    // BeginString <8>, BodyLength <9>, MsgType <35>, MsgSeqNum <34>, SenderCompID <49>,
    // SendingTime <52>, TargetCompID <56>, ClOrdID <11>, HandlInst <21>, OrderQty <38>,
    // OrdType <40>, Side <54>, Symbol <55>, TimeInForce <59>, TransactTime <60>, CheckSum <10>
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    message.set(queryOrderQty());
    message.set(queryTimeInForce());
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.set(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.set(queryStopPx());

    // 设置消息公共的field
    setupMessage(message);

    // 设置消息header
    setupHeader(message.getHeader());

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

    // 构造创建消息：
    // BeginString <8>, BodyLength <9>, MsgType <35>, MsgSeqNum <34>, SenderCompID <49>,
    // SendingTime <52>, TargetCompID <56>, ClOrdID <11>, HandlInst <21>, OrderQty <38>,
    // OrdType <40>, Side <54>, Symbol <55>, TimeInForce <59>, TransactTime <60>, CheckSum <10>
    FIX50::NewOrderSingle message(
        queryClOrdID(),
        FIX::Side_BUY,
        FIX::TransactTime(),
        ordType = queryOrdType()
    );

    message.set(queryOrderQty());
    message.set(queryTimeInForce());
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.set(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.set(queryStopPx());

    // 设置消息公共的field
    setupMessage(message);

    // 设置消息header
    setupHeader(message.getHeader());

    message.setField(167, "FUT"); // SecurityTyp
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

}