/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  locationfunctionserver
 *
*/


#ifndef LOCATIONFUNCTIONSERVER_H
#define LOCATIONFUNCTIONSERVER_H

#include "monitor.h"
#include "functionserver.h"
#include "legacyeventserverwrapper.h"

namespace java
{
namespace location
{

/**
 * If we have to execute some operation that essentially
 * needs ActiveScheduler, then we use this LocationFunctionServer.
 *
 * We must however be careful not to make callbacks through FunctionServer.
 * Reason: If in callback, app calls another method that needs FunctionServer
 * to execute, we enter a deadlock.
 */

class LocationFunctionServer: public java::util::FunctionServer,
        public LegacyEventServerWrapper
{
public:

    //Making these inline as it has nothing more to implement.
    //If implementation grows, then we write separate cpp

    LocationFunctionServer(JNIEnv& env, jobject peer) :
            java::util::FunctionServer("JavaLocationFunctionServer"), mVmAttached(
                false)
    {
        JELOG2(EJavaLocation);

        //Creates a new thread and starts active scheduler
        //This results in call to doServerSideInit()
        createServerToNewThread();
        attachToVm(env, peer);
        mVmAttached = true;
        iServer = reinterpret_cast<java::util::FunctionServer*>(this);
    }

    LocationFunctionServer() :
            java::util::FunctionServer("JavaLocationFunctionServer"), mVmAttached(
                false)
    {
        JELOG2(EJavaLocation);

        //Creates a new thread and starts active scheduler
        //This results in call to doServerSideInit()
        createServerToNewThread();
        iServer = reinterpret_cast<java::util::FunctionServer*>(this);
    }

    /**
     * If we are making any callback in FunctionServer thread context, then we
     * must query for the Thread specific JNI Environment variable.
     * This function that gives JNI environment variable specific to
     * Location Function Server thread.
     */
    JNIEnv* getValidJniEnv()
    {
        JELOG2(EJavaLocation);
        return mJniEnv;
    }

    jobject getPeer()
    {
        return mJavaPeerObject;
    }

    ~LocationFunctionServer()
    {
        JELOG2(EJavaLocation);
        if (mVmAttached)
        {
            detachFromVm();
        }
        stopServer();
    }

    java::util::FunctionServer* getFunctionServer() const
    {
        JELOG2(EJavaLocation);
        return iServer;
    }

private:
    void doServerSideInit()
    {
        JELOG2(EJavaLocation);
        FunctionServer::doServerSideInit();
    }

    void vmDetached()
    {
        JELOG2(EJavaLocation);
        FunctionServer::vmDetached();
    }
private:
    bool mVmAttached;
    java::util::FunctionServer* iServer;
};

} //end namespace location
} //end namespace java

#endif // LOCATIONFUNCTIONSERVER_H
