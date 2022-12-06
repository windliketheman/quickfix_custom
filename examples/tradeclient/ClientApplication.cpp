#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#include "quickfix/config.h"
#include "quickfix/Session.h"
#include <iostream>

#include "ClientApplication.h"

#define GATEWAY_SPDR_TESTING  0
#define GATEWAY_SPDR          "19"
#define GATEWAY_IBFIX         "20"

#define IS_OPTION_TESTING     1

void ClientApplication::onLogon(const FIX::SessionID& sessionID)
{
    std::cout << std::endl << "Logon - " << sessionID << std::endl;
}

void ClientApplication::onLogout(const FIX::SessionID& sessionID)
{
    std::cout << std::endl << "Logout - " << sessionID << std::endl;
}

void ClientApplication::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    crack(message, sessionID);
    std::cout << std::endl << "IN: " << message << std::endl;
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

    std::cout << std::endl
        << "OUT: " << message << std::endl;
}

void ClientApplication::onMessage(const FIX42::ExecutionReport&, const FIX::SessionID&) {}
void ClientApplication::onMessage(const FIX42::OrderCancelReject&, const FIX::SessionID&) {}

void ClientApplication::run()
{
    while (true)
    {
        try
        {
#if IS_OPTION_TESTING
            // 手动输入消息类型
            char action = queryAction();

            if (action == '1')
                testOption1();
            else if (action == '2')
                testOption2();
            else if (action == '3')
                testOption3();
            else if (action == '4')
                testFuture();
            else if (action == '5')
                break;
#else 
            // 手动输入消息类型
            char action = queryAction();

            if (action == '1')
                // 创建新订单
                queryNewOrder();
            else if (action == '2')
                // 取消已有订单
                queryCancelOrder();
            else if (action == '3')
                // 修改已有订单
                queryReplaceOrder();
            else if (action == '4')
                // 注意：查询订单，业务上用不到
                queryMarketDataRequest();
            else if (action == '5')
                break;
#endif
        }
        catch (std::exception& e)
        {
            std::cout << "Message Not Sent: " << e.what();
        }
    }
}

void ClientApplication::queryNewOrder()
{
    // 手动输入Fix版本
    int version = queryVersion();
    std::cout << "\nNewOrderSingle\n";
    FIX::Message order;

    switch (version) {
    case 42:
        // 构造创建订单数据
        order = queryNewOrderSingle42();
        break;
    case 50:
        // 构造创建订单数据
        order = queryNewOrderSingle50();
        break;
    default:
        std::cerr << "No test for version " << version << std::endl;
        break;
    }

    if (queryConfirm("Send enter order"))
        // 将订单发送
        FIX::Session::sendToTarget(order);
}

void ClientApplication::queryCancelOrder()
{
    // 手动输入Fix版本
    int version = queryVersion();
    std::cout << "\nOrderCancelRequest\n";
    FIX::Message cancel;

    switch (version) {
    case 42:
        // 构造取消订单数据
        cancel = queryOrderCancelRequest42();
        break;
    case 50:
        // 构造取消订单数据
        cancel = queryOrderCancelRequest50();
        break;
    default:
        std::cerr << "No test for version " << version << std::endl;
        break;
    }

    if (queryConfirm("Send cancel order"))
        // 将订单发送
        FIX::Session::sendToTarget(cancel);
}

void ClientApplication::queryReplaceOrder()
{
    // 手动输入Fix版本
    int version = queryVersion();
    std::cout << "\nCancelReplaceRequest\n";
    FIX::Message replace;

    switch (version) {
    case 42:
        // 构造修改订单数据
        replace = queryCancelReplaceRequest42();
        break;
    case 50:
        // 构造修改订单数据
        replace = queryCancelReplaceRequest50();
        break;
    default:
        std::cerr << "No test for version " << version << std::endl;
        break;
    }

    if (queryConfirm("Send replace order"))
        // 将订单发送
        FIX::Session::sendToTarget(replace);
}

void ClientApplication::queryMarketDataRequest()
{
    // 手动输入Fix版本
    int version = queryVersion();
    std::cout << "\nMarketDataRequest\n";
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

FIX42::NewOrderSingle ClientApplication::queryNewOrderSingle42()
{
    FIX::OrdType ordType;

    // 构造创建消息：
    // BeginString <8>, BodyLength <9>, MsgType <35>, MsgSeqNum <34>, SenderCompID <49>,
    // SendingTime <52>, TargetCompID <56>, ClOrdID <11>, HandlInst <21>, OrderQty <38>,
    // OrdType <40>, Side <54>, Symbol <55>, TimeInForce <59>, TransactTime <60>, CheckSum <10>
    FIX42::NewOrderSingle message(
        queryClOrdID(), FIX::HandlInst('1'), querySymbol(), querySide(),
        FIX::TransactTime(), ordType = queryOrdType());

    message.set(queryOrderQty());
    message.set(queryTimeInForce());
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.set(queryPrice());
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.set(queryStopPx());

    // 设置消息header
    setupHeader(message.getHeader());

    return message;
}

FIX42::OrderCancelRequest ClientApplication::queryOrderCancelRequest42()
{
    // 构造创建消息：
    // BeginString <8>, BodyLength <9>, MsgType <35>, MsgSeqNum <34>, SenderCompID <49>,
    // SendingTime <52>, TargetCompID <56>, ClOrdID <11>, OrderQty <38>,
    // OrigClOrdID <41>, Side <54>, Symbol <55>, TransactTime <60>, CheckSum <10>
    FIX42::OrderCancelRequest message(queryOrigClOrdID(),
        queryClOrdID(), querySymbol(), querySide(), FIX::TransactTime());

    message.set(queryOrderQty());

    // 设置消息header
    setupHeader(message.getHeader());

    return message;
}

FIX42::OrderCancelReplaceRequest ClientApplication::queryCancelReplaceRequest42()
{
    FIX42::OrderCancelReplaceRequest message(
        queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst('1'),
        querySymbol(), querySide(), FIX::TransactTime(), queryOrdType());

    if (queryConfirm("New price"))
        message.set(queryPrice());
    
    message.set(queryOrderQty());

    // 设置消息header
    setupHeader(message.getHeader());

    return message;
}

FIX50::NewOrderSingle ClientApplication::queryNewOrderSingle50()
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

    return message;
}

FIX50::OrderCancelRequest ClientApplication::queryOrderCancelRequest50()
{
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

    return message;
}

FIX50::OrderCancelReplaceRequest ClientApplication::queryCancelReplaceRequest50()
{
    FIX50::OrderCancelReplaceRequest message(
        queryOrigClOrdID(), 
        queryClOrdID(),
        querySide(), 
        FIX::TransactTime(),
        queryOrdType()
    );

    if (queryConfirm("New price"))
        message.set(queryPrice());
    
    message.set(queryOrderQty());
    
    // 设置消息公共的field
    setupMessage(message);

    // 设置消息header
    setupHeader(message.getHeader());

    return message;
}

// 手动输入消息类型
char ClientApplication::queryAction()
{
#if IS_OPTION_TESTING
    char value;
    std::cout << std::endl
        << "1) 联交所港股期权" << std::endl
        << "2) 美股期权" << std::endl
        << "3) 港期所指数期权" << std::endl
        << "4) 港期所期货" << std::endl
        << "5) 退出" << std::endl
        << "请选择: ";
    std::cin >> value;
    switch (value)
    {
    case '1': case '2': case '3': case '4': case '5': break;
    default: throw std::exception();
    }
    return value;
#else 
    char value;
    std::cout << std::endl
        << "1) Enter Order" << std::endl
        << "2) Cancel Order" << std::endl
        << "3) Replace Order" << std::endl
        << "4) Market data test" << std::endl
        << "5) Quit" << std::endl
        << "Action: ";
    std::cin >> value;
    switch (value)
    {
    case '1': case '2': case '3': case '4': case '5': break;
    default: throw std::exception();
    }
    return value;
#endif
}

// 手动输入Fix版本
int ClientApplication::queryVersion()
{
#if 0
    char value;
    std::cout << std::endl
        << "1) FIX.4.0" << std::endl
        << "2) FIX.4.1" << std::endl
        << "3) FIX.4.2" << std::endl
        << "4) FIX.4.3" << std::endl
        << "5) FIX.4.4" << std::endl
        << "6) FIXT.1.1 (FIX.5.0)" << std::endl
        << "BeginString: ";
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
#else
    return 50;
#endif
}

bool ClientApplication::queryConfirm(const std::string& query)
{
    std::string value;
    std::cout << std::endl << query << "? (Y/N) ";
    std::cin >> value;
    return toupper(*value.c_str()) == 'Y';
}

FIX::SenderCompID ClientApplication::querySenderCompID()
{
    std::string value;
    std::cout << std::endl << "SenderCompID: ";
    std::cin >> value;
    return FIX::SenderCompID(value);
}

FIX::TargetCompID ClientApplication::queryTargetCompID()
{
    std::string value;
    std::cout << std::endl << "TargetCompID: ";
    std::cin >> value;
    return FIX::TargetCompID(value);
}

FIX::TargetSubID ClientApplication::queryTargetSubID()
{
    std::string value;
    std::cout << std::endl << "TargetSubID: ";
    std::cin >> value;
    return FIX::TargetSubID(value);
}

// 机构分配的订单的唯一标识符。
FIX::ClOrdID ClientApplication::queryClOrdID()
{
    std::string value;
    std::cout << std::endl << "ClOrdID: ";
    std::cin >> value;
    return FIX::ClOrdID(value);
}

// 取消或替换订单时，需要之前的订单的ClOrdID <11>
FIX::OrigClOrdID ClientApplication::queryOrigClOrdID()
{
    std::string value;
    std::cout << std::endl << "OrigClOrdID: ";
    std::cin >> value;
    return FIX::OrigClOrdID(value);
}

FIX::Symbol ClientApplication::querySymbol()
{
    std::string value;
    std::cout << std::endl << "Symbol: ";
    std::cin >> value;
    return FIX::Symbol(value);
}

FIX::Side ClientApplication::querySide()
{
    char value;
    std::cout << std::endl
        << "1) Buy" << std::endl
        << "2) Sell" << std::endl
        << "3) Sell Short" << std::endl
        << "4) Sell Short Exempt" << std::endl
        << "5) Cross" << std::endl
        << "6) Cross Short" << std::endl
        << "7) Cross Short Exempt" << std::endl
        << "Side: ";

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
    long value;
    std::cout << std::endl << "OrderQty: ";
    std::cin >> value;
    return FIX::OrderQty(value);
}

FIX::OrdType ClientApplication::queryOrdType()
{
    char value;
    std::cout << std::endl
        << "1) Market" << std::endl
        << "2) Limit" << std::endl
        << "3) Stop" << std::endl
        << "4) Stop Limit" << std::endl
        << "OrdType: ";

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
    double value;
    std::cout << std::endl << "Price: ";
    std::cin >> value;
    return FIX::Price(value);
}

FIX::StopPx ClientApplication::queryStopPx()
{
    double value;
    std::cout << std::endl << "StopPx: ";
    std::cin >> value;
    return FIX::StopPx(value);
}

FIX::TimeInForce ClientApplication::queryTimeInForce()
{
    char value;
    std::cout << std::endl
        << "1) Day" << std::endl
        << "2) IOC" << std::endl
        << "3) OPG" << std::endl
        << "4) GTC" << std::endl
        << "5) GTX" << std::endl
        << "TimeInForce: ";

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
#if GATEWAY_SPDR_TESTING
    header.setField(querySenderCompID());
    header.setField(queryTargetCompID());

    if (queryConfirm("Use a TargetSubID"))
        header.setField(queryTargetSubID());
#else
    header.setField(FIX::SenderCompID("client1"));
    header.setField(FIX::TargetCompID("gateway"));
#endif
}

void ClientApplication::setupMessage(FIX::Message& message)
{
#if GATEWAY_SPDR_TESTING
    // SPDR网关测试
    message.setField(20005, GATEWAY_SPDR); // gateway
    message.setField(207, "US");   // SecurityExchange <207> field, US | USOTC
    message.setField(100, "0");    // ExDestination <100> field, see: jcoms::EDestinationID
    message.setField(55, "AAPL");  // Symbol <55> field, 
#else
    // IBFIX网关测试
    message.setField(20005, GATEWAY_IBFIX); // gateway
    
    message.setField(55, "IBM");    // Symbol <55> field, 股票代码
    message.setField(100, "0"); // 当指定100=SMART后，还必须指定15或者207
    message.setField(15, "USD");
    message.setField(207, "US");    // SecurityExchange
#endif
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