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
* Description:  ?Description
*
*/


#ifndef FUNCTIONSERVER_H
#define FUNCTIONSERVER_H

#include <jni.h>
#include <memory>
#include "javaosheaders.h"
#include "os_functionclient.h"
#include "os_functionserver.h"
#include "exceptionbase.h"
#include "logger.h"
#include "scopedlocks.h"

class MethodCaller;

namespace java
{
namespace util
{

class Monitor;
class OsFunctionServer;
class OsFunctionClient;

/**
 * This class provides functionality to execute Symbian's active objects from platform
 * agnostic code. This based on client/server architecture.
 * See usage example from \javaextensions\jsr\midp\push\pushregistryplugin\src.s60
 * directory.
 */
class OS_IMPORT FunctionServer
{

public:
    //Constructor(s) & destructor
    OS_IMPORT virtual ~FunctionServer();

    OS_IMPORT FunctionServer(const std::string& serverName);
    OS_IMPORT void createServerToNewThread();
    OS_IMPORT void createServerToCurrentThread();
    OS_IMPORT void stopServer();

    OS_IMPORT void stopServerInsideServerThread(bool aDeleteThisObj);
    OS_IMPORT void attachToVm(JNIEnv& env, jobject peer);
    OS_IMPORT void detachFromVm();

protected:
    OS_IMPORT virtual void vmAttached();
    OS_IMPORT virtual void vmDetached();
    OS_IMPORT virtual void doServerSideInit();
    OS_IMPORT virtual void doServerSideClean();
    OS_IMPORT virtual void startServer();

protected:
    JNIEnv*         mJniEnv;
    jobject         mJavaPeerObject;
    jclass          mJavaPeerClass;
    std::string     mServerName;


private: //Methods
    friend class ::MethodCaller;
    friend class ServerShutdown;

    /**
     * Executes a wrapped function in the UI's thread
     */
    int executeInServerThread(const Functor& functor);
    static void* serverThreadEntryPoint(void* ptr);

    int doDetachFromVm();

    int doAttachToVm(JavaVM* javaVm, jobject peer);

    FunctionServer();
    FunctionServer(const FunctionServer&);              //No copy constructor allowed
    FunctionServer& operator= (const FunctionServer&);  //No Assignment operator allowed

private: //Members

    OsFunctionClient        mClient;
    std::auto_ptr<Monitor>  mMonitor;
    ScopedMutex             mMutex;
    // Pointer to the Java VM used to attach the native UI thread to the VM.
    JavaVM*                 mJavaVm;
    bool                    mConnectedToClient;
    OsFunctionServer*       mOsServer;
    bool                    mServerTerminating;
    bool                    mServerRunningInSameThread;
    bool                    mDeleteThisObj;

    //Internal operations.
    void init();
    void createAndStartSrv();

};

} //end namespace util
} //end namespace java
#endif // FUNCTIONSERVER_H

