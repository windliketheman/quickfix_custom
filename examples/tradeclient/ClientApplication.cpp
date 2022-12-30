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
                    << "4) 退出" << std::endl;

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

FIX::ClOrdID ClientApplication::queryClOrdID()
{
    return m_prefix + std::to_string(++m_count);
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
        << "Side: " << std::endl;

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

FIX::OrdType ClientApplication::queryOrdType()
{
    std::cout << std::endl
        << "1) Market" << std::endl
        << "2) Limit" << std::endl
        << "3) Stop" << std::endl
        << "4) Stop Limit" << std::endl
        << "OrdType: " << std::endl;

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

FIX::TimeInForce ClientApplication::queryTimeInForce()
{
    std::cout << std::endl
        << "0) DAY" << std::endl
        << "1) GTC" << std::endl
        << "2) OPG" << std::endl
        << "3) IOC" << std::endl
        << "4) FOK" << std::endl
        << "5) GTX" << std::endl
        << "6) GTD" << std::endl
        << "7) ATC" << std::endl
        << "8) AUC" << std::endl
        << "TimeInForce: " << std::endl;

    char value;
    std::cin >> value;
    switch (value)
    {
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
        return FIX::TimeInForce(value);
    default: throw std::exception();
    }
}

FIX::Symbol ClientApplication::querySymbol()
{
    std::cout << std::endl
        << "Symbol: " << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::Symbol(value);
}

FIX::Currency ClientApplication::queryCurrency()
{
    std::cout << std::endl
        << "Currency: " << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::Currency(value);
}

FIX::SecurityExchange ClientApplication::querySecurityExchange()
{
    std::cout << std::endl
        << "Exchange: " << std::endl;

    std::string value;
    std::cin >> value;
    return FIX::SecurityExchange(value);
}

FIX::Price ClientApplication::queryPrice()
{
    std::cout << std::endl
        << "Price: " << std::endl;

    double value;
    std::cin >> value;
    return FIX::Price(value);
}

FIX::OrderQty ClientApplication::queryOrderQty()
{
    std::cout << std::endl
        << "OrderQty: " << std::endl;

    long value;
    std::cin >> value;
    return FIX::OrderQty(value);
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
    FIX::ClOrdID clOrdID = queryClOrdID();
    FIX::Side side = m_side;
    FIX::OrdType ordType = m_ordType;
    std::string symbol = m_symbol;
    std::string exDestination = m_exDestination;
    std::string currency = m_currency;
    std::string securityExchange = m_securityExchange; // 取值范围：SSE | ZSSE | HKEX | SGX | US | USOTC
    FIX::TimeInForce timeInForce = m_timeInForce;
    FIX::OrderQty orderQty = m_orderQty;
    FIX::Price price = m_price;

    message.setField(FIX::FIELD::ClOrdID, clOrdID);
    message.setField(FIX::Side(side));
    message.setField(FIX::OrdType(ordType));
    message.setField(FIX::FIELD::Symbol, symbol);
    message.setField(FIX::FIELD::ExDestination, exDestination); // 智能路由，固定传"0"，同时还必须设置Currency(15) 或者 SecurityExchange(207)
    message.setField(FIX::FIELD::Currency, currency);
    message.setField(FIX::FIELD::SecurityExchange, securityExchange);
    message.setField(FIX::TimeInForce(timeInForce));
    message.setField(FIX::TransactTime());

    // 改单内容：Price、QTY
    message.setField(FIX::OrderQty(orderQty));
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT)
        message.setField(FIX::Price(price));  // 限价类型要传Price
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)
        message.setField(FIX::Price(price / 2)); // 止损类型要传StopPx

    // 设置公共的静态字段
    setupStaticFields(message);
}

void ClientApplication::setupModifyMessage(FIX::Message& message)
{
    FIX::OrigClOrdID origClOrdID = m_clOrdID;
    FIX::ClOrdID clOrdID = queryClOrdID();
    FIX::Side side = m_side;
    FIX::OrdType ordType = m_ordType;
    std::string symbol = m_symbol;
    std::string currency = m_currency; // 货币: USD
    std::string securityExchange = m_securityExchange; // 取值范围：SSE | ZSSE | HKEX | SGX | US | USOTC
    FIX::TimeInForce timeInForce = m_timeInForce;
    FIX::OrderQty orderQty = m_orderQty / 2;
    FIX::Price price = m_price / 2;

    message.setField(FIX::FIELD::OrigClOrdID, origClOrdID); // 最近修改后的客户订单id，一直在更新，并不是初始的那一个
    message.setField(FIX::FIELD::ClOrdID, clOrdID);
    message.setField(FIX::Side(side));
    message.setField(FIX::OrdType(ordType));
    message.setField(FIX::FIELD::Symbol, symbol);
    message.setField(FIX::FIELD::Currency, currency);
    message.setField(FIX::FIELD::SecurityExchange, securityExchange);
    message.setField(FIX::TimeInForce(timeInForce));
    message.setField(FIX::TransactTime());

    // 改单内容：Price、QTY
    message.setField(FIX::OrderQty(orderQty));
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT)
        message.setField(FIX::Price(price));  // 限价类型要传Price
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)
        message.setField(FIX::Price(price / 2)); // 止损类型要传StopPx

    // 设置公共的静态字段
    setupStaticFields(message);
}

void ClientApplication::setupCancelMessage(FIX::Message& message)
{
    FIX::OrigClOrdID origClOrdID = m_clOrdID;
    FIX::ClOrdID clOrdID = queryClOrdID();
    FIX::Side side = m_side;
    std::string currency = m_currency; // 货币: USD
    std::string securityExchange = m_securityExchange; // 取值范围：SSE | ZSSE | HKEX | SGX | US | USOTC

    message.setField(FIX::FIELD::OrigClOrdID, origClOrdID);
    message.setField(FIX::FIELD::ClOrdID, clOrdID);
    message.setField(FIX::Side(side));
    message.setField(FIX::FIELD::Currency, currency);
    message.setField(FIX::FIELD::SecurityExchange, securityExchange);
    message.setField(FIX::TransactTime());
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
void ClientApplication::startTestAction()
{
    std::cout << std::endl
        << "请选择: " << std::endl
        << "1) Create Order" << std::endl
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
        testCreateOrder();
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

void ClientApplication::testCreateOrder()
{
    // 设置创建订单所需字段
    m_side = querySide();
    m_ordType = queryOrdType();
    m_timeInForce = queryTimeInForce();
    m_symbol = querySymbol();
    m_currency = "USD"; // 货币: USD
    m_securityExchange = "US"; // 取值范围：SSE | ZSSE | HKEX | SGX | US | USOTC
    m_price = queryPrice();
    m_orderQty = queryOrderQty();

    if (queryConfirm("Send create order"))
    {
        FIX50::NewOrderSingle message{};

        // 设置创建订单所需字段
        setupCreateMessage(message);

        // 将订单发送
        FIX::Session::sendToTarget(message);
    }
}

void ClientApplication::testModifyOrder()
{
    FIX50::OrderCancelReplaceRequest message{};

    // 设置修改订单所需字段
    setupModifyMessage(message);

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

void ClientApplication::testCancelOrder()
{
    FIX50::OrderCancelRequest message{};

    // 设置取消订单所需字段
    setupCancelMessage(message);

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

    FIX::Price price = queryPrice();
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(price);
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(FIX::Price(price - 1));

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

    FIX::Price price = queryPrice();
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(price);
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(FIX::Price(price - 1));

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

    FIX::Price price = queryPrice();
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(price);
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(FIX::Price(price - 1));

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

    FIX::Price price = queryPrice();
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) // 限价单或止损限价单
        message.setField(price);
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)  // 又止损限价单？
        message.setField(FIX::Price(price - 1));

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
#if 0
    std::cout << std::endl
        << "请选择: " << std::endl
        << "1) Buy LMT 1000 @ SMART" << std::endl
        << "2) Modify LMT Order" << std::endl
        << "3) Cancel LMT Order" << std::endl;

    char action;
    std::cin >> action;
    switch (action)
    { 
        case '1': case '2': case '3': case '4': case '5': break;
        default: throw std::exception();
    }

    if (action == '1')
        testCaseActionCreate();
    else if (action == '2')
        testCaseActionModify();
    else if (action == '3')
        testCaseActionCancel();
    else
        throw std::exception();
#else
    // 获取输入流，截取参数
    // 动态字段
    m_side = querySide();
    m_ordType = queryOrdType();
    m_timeInForce = queryTimeInForce(); // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    m_symbol = querySymbol();
    m_currency = queryCurrency();
    m_securityExchange = querySecurityExchange();
    m_price = queryPrice();
    m_orderQty = queryOrderQty();
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
void ClientApplication::testCaseActionCreate()
{
    FIX50::NewOrderSingle message{};

    // 设置创建订单所需字段
    setupCreateMessage(message);

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

void ClientApplication::testCaseActionModify()
{
    FIX50::OrderCancelReplaceRequest message{};

    // 设置修改订单所需字段
    setupModifyMessage(message);

    // 将订单发送
    FIX::Session::sendToTarget(message);
}

void ClientApplication::testCaseActionCancel()
{
    FIX50::OrderCancelRequest message{};
    
    // 设置取消订单所需字段
    setupCancelMessage(message);

    // 将订单发送
    FIX::Session::sendToTarget(message);
}