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
* Description:
*
*/


#ifndef BLUETOOTHFUNCTIONSERVER_H
#define BLUETOOTHFUNCTIONSERVER_H

#include "monitor.h"
#include "functionserver.h"

namespace java
{
namespace bluetooth
{

/**
 * BluetoothFunctionServer will be created when BluetoothStackS60 is created.
 * Handle of this will be stored in Java side and will be passed to native
 * calls that need this. If we have to execute some operation that essentially
 * needs ActiveScheduler, then we use this.
 *
 * We must however be careful not to make callbacks through FunctionServer.
 * Reason: If in callback, app calls another method that needs FunctionServer
 * to execute, we enter a deadlock.
 */

class BluetoothFunctionServer: public java::util::FunctionServer
{
public:

    //Making these inline as it has nothing more to implement.
    //If implementation grows, then we write separate cpp

    BluetoothFunctionServer(JNIEnv& env, jobject peer) :
            java::util::FunctionServer("JavaBluetoothFunctionServer")
    {
        JELOG2(EJavaBluetooth);

        //Creates a new thread and starts active scheduler
        //This results in call to doServerSideInit()
        createServerToNewThread();
        attachToVm(env, peer);
        mAttachedToVm = true;
    }

    BluetoothFunctionServer() :
            java::util::FunctionServer("JavaBluetoothPushFunctionServer")
    {
        JELOG2(EJavaBluetooth);

        //Creates a new thread and starts active scheduler
        //This results in call to doServerSideInit()
        createServerToNewThread();
        mAttachedToVm = false;
    }

    /**
     * If we are making any callback in FunctionServer thread context, then we
     * must query for the Thread specific JNI Environment variable.
     * This function that gives JNI environment variable specific to
     * Bluetooth Function Server thread.
     */
    JNIEnv* getValidJniEnv()
    {
        return mJniEnv;
    }

    jobject getPeer()
    {
        return mJavaPeerObject;
    }

    void attach(JNIEnv* jni, jobject peer)
    {
        attachToVm(*jni, peer);
        mAttachedToVm = true;
    }

    bool attachedToVm()
    {
        return mAttachedToVm;
    }

    ~BluetoothFunctionServer()
    {
        if (mAttachedToVm)
        {
            detachFromVm();
        }
        stopServer();
    }

private:
    void doServerSideInit()
    {
        JELOG2(EJavaBluetooth);
        FunctionServer::doServerSideInit();
        //Nothing needs to be done here.
    }

    void vmDetached()
    {
        JELOG2(EJavaBluetooth);
        FunctionServer::vmDetached();
    }

    //We can execute methods in Function  server context using
    //CallMethodL() calls.

private:
    bool mAttachedToVm;
};

} //end namespace bluetooth
} //end namespace java

#endif // BLUETOOTHFUNCTIONSERVER_H
