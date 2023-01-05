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
    ~ClientApplication();

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
    std::string queryClOrdID();
    std::string queryCommands();
    bool queryConfirm(const std::string& query);
    void printLog(std::string log);

    // 设置消息动态字段
    void setupStaticFields(FIX::Message& message);
    void setupCreateMessage(FIX::Message& message);
    void setupModifyMessage(FIX::Message& message);
    void setupCancelMessage(FIX::Message& message);

    // 测试期权，FIX50
    void startOptionAction();
    void testOption1();
    void testOption2();
    void testOption3();
    void testFuture();

    // 执行测试用例，FIX50
    void startTestCaseAction();
    void sendCreateOrder(std::map<std::string, std::string>* params);
    void sendCreateOrderX(std::map<std::string, std::string>* params);
    void sendModifyOrder(std::map<std::string, std::string>* params);
    void sendCreateOrder();
    void sendModifyOrder();
    void sendCancelOrder();

private:
    char m_action{ NULL };

    // 数据变量
    std::string m_exDestination{ "0" }; // 默认：智能路由，传"0"
    std::string m_side{ "" };
    std::string m_ordType{ "" };
    std::string m_timeInForce{ "" };
    std::string m_symbol{ "" };
    std::string m_currency{ "USD" };
    std::string m_securityExchange{ "US" };
    std::string m_price{ "" };
    std::string m_orderQty{ "" };
    std::string m_securityType{ "" };
    std::string m_maturityMonthYear{ "" };
    std::string m_maturityDay{ "" };
    std::string m_strikePrice{ "" };
    std::string m_putOrCall{ "" };

    // 账号相关
    std::string m_account{ "" };
    std::string m_prefix{ "" };
    int m_count{ 0 };
    std::string m_clOrdID{ "" };
    
};

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
// CustomerOrFirm <204> field
// SecurityExchange <207> field
// CheckSum <10> field
//

#endif
