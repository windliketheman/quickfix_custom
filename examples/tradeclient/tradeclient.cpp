/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#include "quickfix/config.h"
#include "quickfix/FileLog.h"
#include "quickfix/FileStore.h"
#include "quickfix/SocketInitiator.h"
#ifdef HAVE_SSL
#include "quickfix/ThreadedSSLSocketInitiator.h"
#include "quickfix/SSLSocketInitiator.h"
#endif
#include "quickfix/SessionSettings.h"
#include "quickfix/Log.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../../src/getopt-repl.h"

#include "ClientApplication.h"

std::string readFile(const std::string& fileName)
{
    std::ifstream inFile;
    inFile.open(fileName); //open the input file

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    return strStream.str(); //str holds the content of the file
}

int main( int argc, char** argv )
{
    // std::string file = readFile("config_fix42.txt"); // 读取配置文件
    std::string file = "config_fix42.txt"; // 读取配置文件
    
    FIX::Initiator * initiator = 0;
    try
    {
        FIX::SessionSettings settings( file );
        
        ClientApplication application;
        FIX::FileStoreFactory storeFactory( settings ); // 创建基于文件的Messagestore实现。
        // FIX::ScreenLogFactory logFactory( settings ); // 将所有日志事件显示到标准输出
        FIX::FileLogFactory logFactory(settings);

        initiator = new FIX::SocketInitiator(application, storeFactory, settings, logFactory);
        initiator->start();
        application.run(); // 内部是for循环，监听输入
        initiator->stop();
        delete initiator;
        return 0;
    }
    catch ( std::exception & e )
    {
        std::cout << e.what();
        delete initiator;
        return 1;
    }
}
