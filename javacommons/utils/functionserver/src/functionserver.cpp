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


#include <sstream>
#include "functionserver.h"
#include "fs_methodcall.h"
#include "logger.h"
#include "monitor.h"

using namespace java::util;

/**
 *
 */
OS_EXPORT FunctionServer::FunctionServer(const std::string& serverName) :
        mJniEnv(0),mJavaPeerObject(0),mJavaPeerClass(0),
        mServerName(serverName), mMonitor(0), mJavaVm(0), mConnectedToClient(false),
        mOsServer(0), mServerTerminating(false), mServerRunningInSameThread(true),
        mDeleteThisObj(false)
{
    JELOG2(EUtils);
    int identifier = (int)this;
    std::ostringstream uniqServerName;
    uniqServerName <<serverName << "_" << identifier;
    mServerName = uniqServerName.str();
}

/**
 *
 */
OS_EXPORT FunctionServer::~FunctionServer()
{
    JELOG2(EUtils);
    // delete mMonitor;
    // mMonitor = 0;

    // delete mOsServer;
    // mOsServer = 0;

}

/**
 *
 */
OS_EXPORT void FunctionServer::stopServer()
{
    JELOG2(EUtils);
    ScopedLock lock(mMutex);

    if (mOsServer != 0)
    {
    
        mServerTerminating = true;
        if (!mConnectedToClient)
        {
            mClient.connect(*mOsServer);
            mConnectedToClient = true;
        }
    
        mClient.sendCloseMessage(this);
        mClient.Close();
        mMonitor->wait();
    }
    else
    {
        ELOG(EUtils,"FunctionServer::stopServer() called when there was no server running");
    }
}


int FunctionServer::doAttachToVm(JavaVM* javaVm, jobject peer)
{
    JELOG2(EUtils);
    mJavaVm = javaVm;
    mJavaPeerObject = peer;
    int result = mJavaVm->AttachCurrentThread((void**)&mJniEnv, 0);
    if (result == 0)
    {
        result = -1;
        jclass clz = mJniEnv->GetObjectClass(mJavaPeerObject);
        if (clz != 0)
        {
            result = 0;
            mJavaPeerClass = static_cast<jclass>(mJniEnv->NewGlobalRef(clz));
            // mJavaPeerClass = clz;
            vmAttached();
        }
    }
    LOG1(EUtils, EInfo, "FunctionServer::doAttachToVm(), status: %d", result);
    return result;
}

OS_EXPORT void FunctionServer::vmAttached()
{
    JELOG2(EUtils);
}

OS_EXPORT void FunctionServer::vmDetached()
{
    JELOG2(EUtils);
}
/**
 *
 */
OS_EXPORT void FunctionServer::attachToVm(JNIEnv& env, jobject peer)
{
    JELOG2(EUtils);
    JavaVM* vm;
    int status = 0;
    env.GetJavaVM(&vm); // Get pointer to VM
    if (vm)
    {
        jobject globalPeer = env.NewGlobalRef(peer);
        if (globalPeer == 0)
        {
            ELOG(EJavaRuntime, "JNI attachUiToVm(), NewGlobalRef(() failed");
            status = -1;
        }
        else
        {
            CallMethod(status, this, &FunctionServer::doAttachToVm, vm, globalPeer, this);

            if (status != 0)
            {
                ELOG(EJavaRuntime, "JNI attachUiToVm(), attachToVm(() failed");
                env.DeleteGlobalRef(globalPeer);
            }
        }
    }
}

int FunctionServer::doDetachFromVm()
{
    JELOG2(EUtils);

    int result = -1;

    if (mJavaPeerObject)
    {
        // Causes crash currently
        mJniEnv->DeleteGlobalRef(mJavaPeerObject);
        mJavaPeerObject = 0;
    }

    if (mJavaPeerClass)
    {
        // Causes crash currently
        mJniEnv->DeleteGlobalRef(mJavaPeerClass);
        mJavaPeerClass = 0;
    }

    if (mJavaVm)
    {
        mJavaVm->DetachCurrentThread();
        mJavaVm = 0;
        vmDetached();
        result = 0;
    }
    return result;
}

/**
 *
 */
OS_EXPORT void FunctionServer::detachFromVm()
{
    JELOG2(EUtils);
    int status = -1;
    CallMethod(status, this, &FunctionServer::doDetachFromVm, this);
    LOG1(EJavaUI, EInfo, "JavaUi::doDetachFromVm(), status: %d", status);
}

#define SERVER_STACKSIZE 0x2000


/**
 *
 */
OS_EXPORT void FunctionServer::createServerToNewThread()
{
    JELOG2(EUtils);

    // This method initializes all datamembers. This is needed in re-starting
    // server use case.
    init();

    // Start the VM into separate thread
    pthread_t tid;
    pthread_attr_t* threadArgs = 0;
    mMonitor.reset(Monitor::createMonitor());
    // mMonitor = Monitor::createMonitor();
    int result = pthread_create(&tid, threadArgs, serverThreadEntryPoint, this);
    mMonitor->wait();

}

/**
 *
 */
void* FunctionServer::serverThreadEntryPoint(void* ptr)
{
    JELOG2(EUtils);
    FunctionServer* server = reinterpret_cast<FunctionServer*>(ptr);
    server->mServerRunningInSameThread = false;
    server->mOsServer = new OsFunctionServer();
    server->mOsServer->osThreadInit();
    server->createAndStartSrv();
    server->mOsServer->osThreadClean();
    server->doServerSideClean();
    delete server->mOsServer;
    server->mOsServer = 0;
    server->mMonitor->notify();
    pthread_exit(0);
    if (true == server->mDeleteThisObj)
    {
        delete server;
    }
    return 0;
}

/**
 *
 */
OS_EXPORT void FunctionServer::createServerToCurrentThread()
{
    JELOG2(EUtils);

    // This method initializes all datamembers. This is needed in re-starting
    // server use case.
    init();
    createAndStartSrv();
}

/**
 *
 */
OS_EXPORT void FunctionServer::createAndStartSrv()
{
    JELOG2(EUtils);

    doServerSideInit();
    if (mOsServer == 0)
    {
        mOsServer = new OsFunctionServer();
    }
    mOsServer->startOsServer();
    if (mMonitor.get())
    {
        mMonitor->notify();
    }
    startServer();
    if (mServerRunningInSameThread)
    {
        doServerSideClean();
        delete mOsServer;
        mOsServer = 0;
        mMonitor->notify();
    }
}

/**
 *
 */
OS_EXPORT void FunctionServer::startServer()
{
    JELOG2(EUtils);
    CActiveScheduler::Start();
    delete CActiveScheduler::Current();         // will delete all objects
}
/**
 *
 */
OS_EXPORT void FunctionServer::doServerSideInit()
{
    JELOG2(EUtils);
    CActiveScheduler* scheduler = new CActiveScheduler;
    CActiveScheduler::Install(scheduler);
}

/**
 *
 */
OS_EXPORT void FunctionServer::doServerSideClean()
{
    JELOG2(EUtils);
}


/**
 *
 */
OS_EXPORT void FunctionServer::stopServerInsideServerThread(bool aDeleteThisObj)
{
    JELOG2(EUtils);
    // ScopedLock was comment out because deadlock occurs if this
    // operation is called inside user's method passed as argument to CallMethodL() operation.
    // There is a tiny risk that this causes problem when 'mServerTerminating' datamember
    // is read in executeInServerThread() operation.
    // ScopedLock lock(mMutex);
    mServerTerminating = true;
    mDeleteThisObj = aDeleteThisObj;

    if (mOsServer)
    {
        mOsServer->stopServerInsideServerThread();
    }
}


void FunctionServer::init()
{
    JELOG2(EUtils);
    mMonitor.reset(0);
    doDetachFromVm();
    mConnectedToClient = false;
    if (mOsServer)
    {
        delete mOsServer;
        mOsServer = 0;
    }
    mServerTerminating = false;
    mServerRunningInSameThread = true;
    mDeleteThisObj = false;
}

int FunctionServer::executeInServerThread(const Functor& functor)
{
    JELOG2(EUtils);
    // See stopServerInsideServerThread operation.
    // ScopedLock lock(mMutex);
    if (mServerTerminating || mOsServer == 0)
    {
        std::string message = "Trying to execute code in non-existing FS: ";
        message.append(mServerName);
        if (mServerTerminating)
        {
            message.append(" server closing.");
        }
        else
        {
            message.append(" server not created.");
        }
        ELOG1(EUtils,"%s", message.c_str());
        throw ExceptionBase(message, __FILE__, __FUNCTION__, __LINE__);
    }
    if (!mConnectedToClient)
    {
        int err = mClient.connect(*mOsServer);
        if (err != 0)
        {
            return err;
        }
        mConnectedToClient = true;
    }
    int res = mClient.execute(functor);
    return res;
}


