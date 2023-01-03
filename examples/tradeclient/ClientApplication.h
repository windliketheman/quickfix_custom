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

// For Command
// #include "Base/Macros.h"
// #include "Base/Command.h"
// #include "Base/Console.h"

// class Context;

class ClientApplication :
    public FIX::Application,
    public FIX::MessageCracker
{
public:
    // using CommandMap = std::unordered_map < std::string, base::Command<ClientApplication> >;

    // explicit ClientApplication(const Context& ctx);
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
    std::map<std::string, std::string> parseCommands(std::string str);
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
    void sendCreateOrder();
    void sendModifyOrder();
    void sendCancelOrder();

private:
    // using Args = base::Arguments;
    // const Context& m_ctx;
    // const static CommandMap m_commands;
    // base::Console m_console{};
    
    char m_action{ NULL };

    // 数据变量
    std::string m_exDestination{ "0" }; // 默认：智能路由，传"0"
    std::string m_side{ "" };
    std::string m_ordType{ "" };
    std::string m_timeInForce{ "" };
    std::string m_symbol{ "" };
    std::string m_currency{ "" };
    std::string m_securityExchange{ "" };
    std::string m_price{ "" };
    std::string m_orderQty{ "" };

    // 账号相关
    std::string m_account{ "" };
    std::string m_prefix{ "" };
    int m_count{ 0 };
    std::string m_clOrdID{ "" };
    
};

#endif
