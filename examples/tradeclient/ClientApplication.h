#ifndef TRADECLIENT_CLIENT_APPLICATION_H
#define TRADECLIENT_CLIENT_APPLICATION_H

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Mutex.h"

#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/OrderCancelRequest.h"
#include "quickfix/fix42/OrderCancelReject.h"
#include "quickfix/fix42/OrderCancelReplaceRequest.h"

#include "quickfix/fix50/NewOrderSingle.h"
#include "quickfix/fix50/ExecutionReport.h"
#include "quickfix/fix50/OrderCancelRequest.h"
#include "quickfix/fix50/OrderCancelReject.h"
#include "quickfix/fix50/OrderCancelReplaceRequest.h"
#include "quickfix/fix50/MarketDataRequest.h"

#include <queue>

class ClientApplication :
    public FIX::Application,
    public FIX::MessageCracker
{
public:
    void run();

private:
    void onCreate(const FIX::SessionID&) {}
    void onLogon(const FIX::SessionID& sessionID);
    void onLogout(const FIX::SessionID& sessionID);
    void toAdmin(FIX::Message&, const FIX::SessionID&) {}
    void toApp(FIX::Message&, const FIX::SessionID&)
        EXCEPT(FIX::DoNotSend);
    void fromAdmin(const FIX::Message&, const FIX::SessionID&)
        EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {}
    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
        EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

    void onMessage(const FIX42::ExecutionReport&, const FIX::SessionID&);
    void onMessage(const FIX42::OrderCancelReject&, const FIX::SessionID&);

    // 输入信息的一些方法
    int queryVersion();
    void setConsoleTextColor();
    void resetConsoleTextColor();
    bool queryConfirm(const std::string& query);
    FIX::SenderCompID querySenderCompID();
    FIX::TargetCompID queryTargetCompID();
    FIX::TargetSubID queryTargetSubID();
    FIX::ClOrdID queryClOrdID();
    FIX::OrigClOrdID queryOrigClOrdID();
    FIX::Symbol querySymbol();
    FIX::Side querySide();
    FIX::OrderQty queryOrderQty();
    FIX::OrdType queryOrdType();
    FIX::Price queryPrice();
    FIX::StopPx queryStopPx();
    FIX::TimeInForce queryTimeInForce();

    // 测试下单，FIX50
    void startTestAction();
    void testNewOrder();
    void testCancelOrder();
    void testReplaceOrder();
    void testMarketDataRequest();

    void setupHeader(FIX::Header& header);
    void setupMessage(FIX::Message& message);

    // 测试期权，FIX50
    void startOptionAction();
    void testOption1();
    void testOption2();
    void testOption3();
    void testFuture();

    // 执行测试用例，FIX50
    void startTestCaseAction();


};

#endif
