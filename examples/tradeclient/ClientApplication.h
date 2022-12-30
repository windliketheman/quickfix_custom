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
    ClientApplication();
    void run();

private:
    void onCreate(const FIX::SessionID&);
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

    // 处理消息返回
    bool implExecutionReport(const FIX::Message& message);
    void implOrderCancelReject(const FIX::Message& message) const;
    void implBusinessMessageReject(const FIX::Message& message) const;

    // 输入信息的一些方法
    int queryVersion();
    FIX::ClOrdID queryClOrdID();
    FIX::Side querySide();
    FIX::OrdType queryOrdType();
    FIX::TimeInForce queryTimeInForce();
    FIX::Symbol querySymbol();
    FIX::Currency queryCurrency();
    FIX::SecurityExchange querySecurityExchange();
    FIX::Price queryPrice();
    FIX::OrderQty queryOrderQty();
    bool queryConfirm(const std::string& query);
    void printLog(std::string log);

    // 设置消息动态字段
    void setupStaticFields(FIX::Message& message);
    void setupCreateMessage(FIX::Message& message);
    void setupModifyMessage(FIX::Message& message);
    void setupCancelMessage(FIX::Message& message);

    // 测试下单，FIX50
    void startTestAction();
    void testCreateOrder();
    void testModifyOrder();
    void testCancelOrder();
    void testMarketDataRequest();

    // 测试期权，FIX50
    void startOptionAction();
    void setupOptionMessage(FIX::Message& message);
    void testOption1();
    void testOption2();
    void testOption3();
    void testFuture();

    // 执行测试用例，FIX50
    void startTestCaseAction();
    void testCaseActionCreate();
    void testCaseActionModify();
    void testCaseActionCancel();

private:
    char m_action{ NULL };

    // 数据变量
    FIX::Side m_side{ NULL };
    FIX::OrdType m_ordType{ NULL };
    std::string m_symbol{ NULL };
    std::string m_exDestination{ "0" }; // 默认：智能路由，传"0"
    std::string m_currency{ NULL };
    std::string m_securityExchange{ NULL };
    FIX::TimeInForce m_timeInForce{ NULL };
    FIX::Price m_price{ 0.0 };
    FIX::OrderQty m_orderQty{ 0.0 };

    // 账号相关
    std::string m_account{ NULL };
    std::string m_prefix{ NULL };
    int m_count{ 0 };
    std::string m_clOrdID{ NULL };
    
};

#endif
