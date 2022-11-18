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

#include "quickfix/FileStore.h"
#include "quickfix/SocketAcceptor.h"
#ifdef HAVE_SSL
#include "quickfix/ThreadedSSLSocketAcceptor.h"
#include "quickfix/SSLSocketAcceptor.h"
#endif
#include "quickfix/Log.h"
#include "quickfix/SessionSettings.h"
#include "Application.h"
#include <string>
#include <iostream>
#include <fstream>

#include "TestApplication.h"

void wait()
{
    std::cout << "Type Ctrl-C to quit" << std::endl;
    while(true)
    {
        FIX::process_sleep(1);
    }
}

void excuteTest(std::string fileName)
{
    TestApplication application;
    FIX::SessionSettings settings(fileName);
    FIX::FileStoreFactory storeFactory(settings);
    FIX::SocketAcceptor acceptor(application, storeFactory, settings);
    acceptor.start();
    // while( condition == true ) { do something; }
    acceptor.stop();
}

int main( int argc, char** argv )
{
    if ( argc < 2 )
    {
        if (argc < 2) return 1;
        std::string fileName = argv[1];

        // test
        excuteTest(fileName);

        return 0;

#if 0
    std::cout << "usage: " << argv[ 0 ]
    << " FILE." << std::endl;
    return 0;
#endif
    }

    std::string file = argv[ 1 ];
#ifdef HAVE_SSL
    std::string isSSL;
    if (argc > 2)
    {
        isSSL.assign(argv[2]);
    }
#endif

    FIX::Acceptor * acceptor = 0;
    try
    {
        FIX::SessionSettings settings( file );

        TestApplication application;
        FIX::FileStoreFactory storeFactory( settings );
        FIX::ScreenLogFactory logFactory( settings );

#ifdef HAVE_SSL
    if (isSSL.compare("SSL") == 0)
        acceptor = new FIX::ThreadedSSLSocketAcceptor ( application, storeFactory, settings, logFactory );
    else if (isSSL.compare("SSL-ST") == 0)
        acceptor = new FIX::SSLSocketAcceptor ( application, storeFactory, settings, logFactory );
    else
#endif
        acceptor = new FIX::SocketAcceptor ( application, storeFactory, settings, logFactory );

        acceptor->start();
        wait();
        acceptor->stop();
        delete acceptor;
        return 0;
    }
    catch ( std::exception & e )
    {
        std::cout << e.what() << std::endl;
        delete acceptor;
        return 1;
    }
}
