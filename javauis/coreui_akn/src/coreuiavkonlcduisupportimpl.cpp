/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: A support class implementation for the LCDUI.
*
*/

#include "coreuiavkonlcduisupportimpl.h"
#include "coreuiavkonlcdui.h"
#include "coreuiavkonimpl.h"

#include "mevents.h"

#include "logger.h"
#include "javacommonutils.h"
#include "runtimeexception.h"
#include "exceptionbase.h"

// Stack size for the UI thread. Default size 0x2000 was too small and caused crashes
#ifndef JAVA_STACKSIZE
#define JAVA_STACKSIZE 0x8000
#endif
_LIT(KUiLibName,"lcdui");

using namespace java::ui;
using namespace java::util;

CoreUiAvkonLcduiSupportImpl::CoreUiAvkonLcduiSupportImpl(): mHandle(0),
        mUiHeap(0),
        mProcessHeap(0),
        mCoreUi(0)
{
    JELOG2(EJavaUI);
    mMonitor.reset(Monitor::createMonitor());
#ifdef RD_JAVA_UITHREAD_OWN_HEAP
    // Create a new heap for the native UI thread
    const TInt KAppUiHeapMaxSize = 0x200000; // 2 MB
    mUiHeap = User::ChunkHeap(0, KMinHeapSize, KAppUiHeapMaxSize);
    // Store the Java VM's heap.
    mProcessHeap = &User::Heap();
#endif // RD_JAVA_UITHREAD_OWN_HEAP
}

CoreUiAvkonLcduiSupportImpl::~CoreUiAvkonLcduiSupportImpl()
{
    JELOG2(EJavaUI);
    mRlcdui.Close();
    if (mUiHeap != mProcessHeap)
    {
        mProcessHeap->Close();
    }
}

void CoreUiAvkonLcduiSupportImpl::startUiThread(CoreUiAvkonImpl& ui)
{
    JELOG2(EJavaUI);
    mCoreUi = &ui;
    _LIT(KUiThreadName, "JavaMidpUi");
    TRAP_IGNORE(mHandle = TJavaEventServer::NewL(KUiThreadName,
                          &UiEventServerThread,
                          (void*)this,
                          JAVA_STACKSIZE,
                          mUiHeap).Handle());
}

void CoreUiAvkonLcduiSupportImpl::closeUi()
{
    JELOG2(EJavaUI);
    if (mHandle)
    {
        TJavaEventServer(mHandle).Shutdown();
        mMonitor->wait();
    }
}

TInt CoreUiAvkonLcduiSupportImpl::UiEventServerThread(void* arg)
{
    JELOG2(EJavaUI);
    MEventServer* server = static_cast<MEventServer*>(arg);
    CoreUiAvkonLcduiSupportImpl* thisPtr =
        reinterpret_cast<CoreUiAvkonLcduiSupportImpl*>(server->Arg());
    RServer2 serverHandle;
    try
    {
#ifdef RD_JAVA_UITHREAD_OWN_HEAP
        __UHEAP_MARK;
#endif // RD_JAVA_UITHREAD_OWN_HEAP
        CoreUiAvkonImpl* ui = thisPtr->mCoreUi;
        ui->createUi();
        TRAPD(err, RLcdui::Get()->OpenL(serverHandle));
        server->Started(err, serverHandle);
        server = 0;         // don't touch again - owned by parent thread
        ui->startScheduler();
        RLcdui::Get()->Close();

#ifdef RD_JAVA_UITHREAD_OWN_HEAP
        __UHEAP_MARKEND;
#endif // RD_JAVA_UITHREAD_OWN_HEAP
    }
    catch (java::runtime::RuntimeException& e)
    {
        ELOG1(EJavaUI, "UiEventServerThread RuntimeException catched: %s ",
              e.toString().c_str());
    }

    catch (ExceptionBase& e)
    {
        ELOG1(EJavaUI, "UiEventServerThread ExceptionBase catched: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {

        ELOG1(EJavaUI, "UiEventServerThread Exception %s catched", e.what());
    }
    if (server != 0)
    {
        server->Started(-1, serverHandle);
    }
    thisPtr->mMonitor->notify();
    return 0;
}

RLcdui* CoreUiAvkonLcduiSupportImpl::getRlcdUi()
{
    JELOG2(EJavaUI);
    return &mRlcdui;
}

int CoreUiAvkonLcduiSupportImpl::getEventServerHandle()
{
    JELOG2(EJavaUI);
    return mHandle;
}

RHeap* CoreUiAvkonLcduiSupportImpl::getProcessHeap() const
{
    JELOG2(EJavaUI);
    return mProcessHeap;
}

RHeap* CoreUiAvkonLcduiSupportImpl::getUiThreadHeap() const
{
    JELOG2(EJavaUI);
    return mUiHeap;
}

RLcdui::RLcdui() : iPlugin(0), iServer(0)
{
    JELOG2(EJavaUI);
}

void RLcdui::OpenL(RServer2& serverHandle)
{
    JELOG2(EJavaUI);
    iServer = new(ELeave) CJesServer(this);
    iServer->StartL(KNullDesC);
    serverHandle = iServer->Server();
}

EXPORT_C TInt RLcdui::CreateAppUi()
{
    int err = iLibrary.Load(KUiLibName);
    if (err == KErrNone)
    {
        return ((TLcduiPluginFunction)iLibrary.Lookup(1))();
    }
    return err;
}


void RLcdui::Close()
{
    JELOG2(EJavaUI);
    iLibrary.Close();
    Detach();
}

void RLcdui::AttachL(MLcduiPlugin* aPlugin)
{
    JELOG2(EJavaUI);
    if (0 == aPlugin)
    {
        User::Leave(KErrArgument);
    }
    iPlugin = aPlugin;

#ifdef LCDUI_LEAK_TEST
    User::Leave(KErrGeneral);
#endif
}

void RLcdui::Detach()
{
    JELOG2(EJavaUI);
    delete iServer;
    iServer = NULL;
    iPlugin = NULL;
}

MLcduiPlugin* RLcdui::Plugin()
{
    JELOG2(EJavaUI);
    return iPlugin;
}

// MJesShutdown
void RLcdui::Shutdown()
{
    JELOG2(EJavaUI);
    // Terminate the UI thread.
    java::ui::CoreUiAvkonLcdui::getInstance().getJavaAknAppUi()->Exit();
}

EXPORT_C RLcdui* RLcdui::Get()
{
    return CoreUiAvkonLcdui::getInstance().getLcduiSupport().getRlcdUi();
}

