/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef CENTREPFUNCTIONSERVER_H
#define CENTREPFUNCTIONSERVER_H

#include "monitor.h"
#include "functionserver.h"
#include "legacyeventserverwrapper.h"

namespace java
{
namespace centrep
{

/**
 * If we have to execute some operation that essentially
 * needs ActiveScheduler, then we use this CentrepFunctionServer.
 *
 * We must however be careful not to make callbacks through FunctionServer.
 * Reason: If in callback, app calls another method that needs FunctionServer
 * to execute, we enter a deadlock.
 */

class CentrepFunctionServer: public java::util::FunctionServer,
        public LegacyEventServerWrapper
{
public:

    CentrepFunctionServer() :
            java::util::FunctionServer("JavaCentrepFunctionServer")
    {
        JELOG2(EJavaCentrep);

        //Creates a new thread and starts active scheduler
        //This results in call to doServerSideInit()
        createServerToNewThread();
        mServer = reinterpret_cast<java::util::FunctionServer*>(this);
    }

    ~CentrepFunctionServer()
    {
        JELOG2(EJavaCentrep);
        stopServer();
    }

    java::util::FunctionServer* getFunctionServer() const
    {
        JELOG2(EJavaCentrep);
        return mServer;
    }

private:

    void doServerSideInit()
    {
        JELOG2(EJavaCentrep);
        FunctionServer::doServerSideInit();
    }

private:

    java::util::FunctionServer* mServer;
};

} //end namespace centrep
} //end namespace java

#endif // CENTREPFUNCTIONSERVER_H
