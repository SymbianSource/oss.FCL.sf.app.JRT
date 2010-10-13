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
* Description: A CoreUi Avkon implementation.
*
*/

#include <string.h>
#include <apgwgnam.h>
#include <akntitle.h>

#include "logger.h"
#include "coreuiavkonimpl.h"
#include "javauiavkonimpl.h"
#include "runtimeexception.h"
#include "javacommonutils.h"

using namespace java::ui;
using namespace java::util;

// ======== STATIC VARIABLES ========

/**
* Singleton
*/

NONSHARABLE_CLASS(CoreUiGlobals)
{
public:
    CoreUiGlobals() : mUiCore(0)
    {
    }

public:
    java::ui::CoreUiAvkonImpl* mUiCore;
};


#if defined(__WINSCW__)

#include <pls.h>
CoreUiGlobals* getCoreUiGlobals()
{
    // Access the PLS of this process
    CoreUiGlobals* globals  =
        Pls<CoreUiGlobals>(TUid::Uid(0x2002703C));
    return globals;
}

#else


static CoreUiGlobals* sGlobals = 0;

CoreUiGlobals* getCoreUiGlobals()
{
    if (sGlobals == 0)
    {
        sGlobals = new CoreUiGlobals(); // codescanner::nonleavenew
    }
    return sGlobals;
}
#endif


CoreUi& getUiInstance()
{
    JELOG2(EJavaUI);
    CoreUiGlobals* globals = getCoreUiGlobals();
    if (globals->mUiCore == 0)
    {
        globals->mUiCore = new CoreUiAvkonImpl(); // codescanner::nonleavenew
    }

    return *globals->mUiCore;
}

void releaseUi()
{
    JELOG2(EJavaUI);
    CoreUiGlobals* globals = getCoreUiGlobals();
    if (globals->mUiCore)
    {
        globals->mUiCore->dispose();
    }
}

EXPORT_C FuncPtr findDllMethod(const char* funcName)
{
    JELOG2(EJavaUI);
    FuncPtr ptr = 0;
    if (strcmp(funcName, "getUiInstance") == 0)
    {
        ptr = (FuncPtr)getUiInstance;
    }
    else if (strcmp(funcName, "releaseUi") == 0)
    {
        ptr = (FuncPtr)releaseUi;
    }
    return ptr;
}

EXPORT_C bool CoreUiAvkon::isCoreUiCreated()
{
    JELOG2(EJavaUI);
    CoreUiGlobals* globals = getCoreUiGlobals();
    return globals->mUiCore != 0;
}

EXPORT_C CoreUiAvkonLcdui& CoreUiAvkonLcdui::getInstance()
{
    JELOG2(EJavaUI);
    return static_cast<CoreUiAvkonLcdui&>(
               static_cast<CoreUiAvkonImpl&>(getUiInstance()));
}

EXPORT_C CoreUiAvkonEswt& CoreUiAvkonEswt::getInstance()
{
    JELOG2(EJavaUI);
    return static_cast<CoreUiAvkonEswt&>(
               static_cast<CoreUiAvkonImpl&>(getUiInstance()));
}

CoreUiAvkonImpl& CoreUiAvkonImpl::getInstanceImpl()
{
    JELOG2(EJavaUI);
    return static_cast<CoreUiAvkonImpl&>(::getUiInstance());
}

CoreUiAvkonImpl::CoreUiAvkonImpl() : mJavaUiAvkonEnv(0), mWgName(0),
        mJavaUiAppUi(0),
        mState(Constructed), mShowStartScreen(true),
        mJavaVm(0), mShutDownPending(false),
        mShutdownTimer(0)
{
    JELOG2(EJavaUI);
    mLock.CreateLocal();

}


CoreUiAvkonImpl::~CoreUiAvkonImpl()
{
    JELOG2(EJavaUI);
    delete mWgName;
    mWgName = 0;
    mLock.Close();
}

void CoreUiAvkonImpl::ensureInitialized(const TUid& appUid,
                                        CoreUiParams* params)
{
    JELOG2(EJavaUI);
    if (mJavaUiAvkonEnv == 0)
    {
        java::util::Uid uid;
        TUidToUid(appUid, uid);

        start(uid, params);
    }
}


CoreUiAvkonLcduiSupport& CoreUiAvkonImpl::getLcduiSupport()
{
    return mLcduiSupport;
}

CoreUiAvkonEswtSupport& CoreUiAvkonImpl::getEswtSupport()
{
    return mEswtSupport;
}

void CoreUiAvkonImpl::start(const java::util::Uid& midletUid,
                            CoreUiParams* uiParams)
{
    mMidletUid = midletUid;
    if (uiParams != 0)
    {
        mCoreUiParams = *uiParams;
    }
    mLcduiSupport.startUiThread(*this);
}

void CoreUiAvkonImpl::startScheduler()
{
    JELOG2(EJavaUI);
    if (mJavaUiAvkonEnv != 0)
    {
        mState = CoreUiCreated;
        mJavaUiAvkonEnv->ExecuteD();
        cancelShutdownTimer();
        mEswtSupport.dispose();
        mJavaUiAvkonEnv = 0;
        delete mWgName;
        mWgName = 0;
    }
    mState = Destroyed;
}

void CoreUiAvkonImpl::createUi()
{
    JELOG2(EJavaUI);
    // Construct the environment (EikonEnv + AppUi + server)

    // The cleanup stack is created here
    mJavaUiAvkonEnv = new CEikonEnv; // codescanner::nonleavenew

    if (mJavaUiAvkonEnv != 0)
    {
        uidToTUid(mMidletUid, mMidletTUid);
        if (mMidletUid == Uid(L"[2001843a]") || // Is it installer.
                mMidletUid == Uid(L"[2002121e]"))    // Is it TCK runner.
        {
            mShowStartScreen = false;
        }
        TRAPD(status, initAvkonUiL());
        if (status != KErrNone)
        {
            // Deletes AS and CleanupTrap. No need to call 'delete env'.
            mJavaUiAvkonEnv->DestroyEnvironment();
            std::string errorStr("Ui creation failed: ");
            errorStr.append(JavaCommonUtils::intToString(status));
            throw java::runtime::RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
        mEswtSupport.uiThreadInit();
    }
}

void CoreUiAvkonImpl::initAvkonUiL()
{
    JELOG2(EJavaUI);
    TApaApplicationFactory appFactory(newJavaApplication);

    CApaCommandLine* appCmdLine = CApaCommandLine::NewLC();
    appCmdLine->SetExecutableNameL(KNullDesC);          // None - app function provided by the app factory
    appCmdLine->SetDefaultScreenL(0);
    appCmdLine->SetServerNotRequiredL();                //
    if (mCoreUiParams.isBackgroundStart())
    {
        appCmdLine->SetCommandL(EApaCommandBackground);
    }
    mJavaUiAvkonEnv->DisableExitChecks(ETrue);
    mJavaUiAvkonEnv->ConstructAppFromCommandLineL(appFactory, *appCmdLine);

    CleanupStack::PopAndDestroy(appCmdLine);

    mJavaUiAvkonEnv->AppUiFactory()->StatusPane()->MakeVisible(EFalse);


    // Contruct the WindowsGroupName, used to change the displayed name in the tasklist
    mWgName = CApaWindowGroupName::NewL(mJavaUiAvkonEnv->WsSession());
    mWgName->SetRespondsToSwitchFilesEvent(EFalse);
    mWgName->SetRespondsToShutdownEvent(ETrue);
    mWgName->SetHidden(EFalse);
    mWgName->SetAppUid(mMidletTUid);
    mWgName->SetWindowGroupName(mJavaUiAvkonEnv->RootWin());

    mJavaUiAvkonEnv->RootWin().EnableReceiptOfFocus(ETrue);
}


void CoreUiAvkonImpl::dispose()
{
    JELOG2(EJavaUI);
    bool canBeDisposed = mJavaUiAppUi == 0 ||
                         (mJavaUiAppUi && mJavaUiAppUi->isClosingPossible());
    if (canBeDisposed)
    {
        if (mState != Destroyed)
        {
            mLcduiSupport.closeUi();
        }
        delete this;
        CoreUiGlobals* globals = getCoreUiGlobals();
        globals->mUiCore = 0;
    }
}

CApaWindowGroupName* CoreUiAvkonImpl::getWindowGroupName() const
{
    JELOG2(EJavaUI);
    return mWgName;
}

bool  CoreUiAvkonImpl::showStartScreen() const
{
    JELOG2(EJavaUI);
    return mShowStartScreen && mCoreUiParams.getScreenMode() != NO_START_SCREEN;
}


CoreUiAvkonAppUi* CoreUiAvkonImpl::getJavaUiAppUi() const
{
    JELOG2(EJavaUI);
    return mJavaUiAppUi;
}

CAknAppUi* CoreUiAvkonImpl::getJavaAknAppUi() const
{
    JELOG2(EJavaUI);
    return mJavaUiAppUi;
}

void CoreUiAvkonImpl::setJavaUiAppUi(JavaUiAvkonAppUi* appUi)
{
    JELOG2(EJavaUI);
    mJavaUiAppUi = appUi;
}

RHeap* CoreUiAvkonImpl::getProcessHeap() const
{
    JELOG2(EJavaUI);
    return mLcduiSupport.getProcessHeap();
}

RHeap* CoreUiAvkonImpl::getUiThreadHeap() const
{
    JELOG2(EJavaUI);
    return mLcduiSupport.getUiThreadHeap();
}


bool CoreUiAvkonImpl::setJavaVm(JNIEnv* env)
{
    JELOG2(EJavaUI);
    mLock.Wait();
    bool result = false; // Meaning the starting of the MIDlet
    // should not be allowed.
    LOG(EJavaUI, EInfo, "Setting JavaVm");
    if (!mShutDownPending)
    {
        LOG(EJavaUI, EInfo, "  No pending shutdown");
        env->GetJavaVM(&mJavaVm); // Get pointer to VM
        if (mJavaVm)
        {
            LOG(EJavaUI, EInfo, "  Got the VM");
            result = true;
        }
        else
        {
            ELOG(EJavaUI, "JNI attachUiToVm(), GetJavaVM(() failed");
        }
    }
    mLock.Signal();
    LOG1(EJavaUI, EInfo, "CoreUiAvkonImpl::setJavaPeer result = %d", result);
    return result;
}

void CoreUiAvkonImpl::shutDownRequestFromWindowServer()
{
    JELOG2(EJavaUI);
    LOG(EJavaUI, EInfo, "shutDownRequestFromWindowServer");
    mLock.Wait();
    if (mState != ShuttingDown)
    {
        mState = ShuttingDown;
        if (mShutdownTimer == 0)
        {
            TRAP_IGNORE(setShutdownTimerL());
        }
        LOG(EJavaUI, EInfo, "  mState != SHUTTING_DOWN");
        if (mJavaVm) // Check if the VM already running.
        {
            LOG(EJavaUI, EInfo, "  We have the VM");

            bool success =
                callStaticVoidJavaMethod("com/nokia/mj/impl/coreui/CoreUi",
                                         "shutdownRequest",
                                         "()V");

            if (!success)
            {
                ELOG(EJavaUI, "Killing process forcefully");
                RProcess().Kill(-1);
            }


        }
        else
        {
            LOG(EJavaUI, EInfo, "  Pending request");
            // No, pend the request.
            mShutDownPending = true;
        }
    }
    mLock.Signal();
}

bool CoreUiAvkonImpl::callStaticVoidJavaMethod(const char* className,
        const char* methodName,
        const char* methodSignature,
        ...)
{
    JELOG2(EJavaUI);
    bool success = false;

    JNIEnv* env;
#ifdef RD_JAVA_UITHREAD_OWN_HEAP
    User::SwitchHeap(getProcessHeap());
#endif // RD_JAVA_UITHREAD_OWN_HEAP
    int result = mJavaVm->AttachCurrentThread((void**)&env, 0);
    if (result == 0)
    {
        LOG(EJavaUI, EInfo, "  VM Attached");
        jclass clz = env->FindClass(className);
        if (clz != 0)
        {
            LOG(EJavaUI, EInfo, "  jclass created");
            jmethodID methodId =
                env->GetStaticMethodID(clz, methodName, methodSignature);
            if (methodId != 0)
            {
                LOG(EJavaUI, EInfo, "  methodId created");
                va_list args;
                va_start(args, methodSignature);
                env->CallStaticVoidMethodV(clz, methodId, args);
                LOG(EJavaUI, EInfo, "  CallStaticVoidMethodV done");
                va_end(args);
                // Close message sent successfully to Java side.
                success = true;
            }
            else
            {
                ELOG(EJavaUI, "callStaticVoidJavaMethod, "
                     "GetMethodID(() failed");
            }
            env->DeleteLocalRef(clz);
        }
        else
        {
            ELOG(EJavaUI, "callStaticVoidJavaMethod, "
                 "GetObjectClass() failed");
        }
        result = mJavaVm->DetachCurrentThread();
        LOG1(EJavaUI, EInfo, "  DetachCurrentThread done, st = %d", result);
    }
    else
    {
        ELOG(EJavaUI, "callStaticVoidJavaMethod, "
             "AttachCurrentThread() failed");
    }
#ifdef RD_JAVA_UITHREAD_OWN_HEAP
    User::SwitchHeap(getUiThreadHeap());
#endif // RD_JAVA_UITHREAD_OWN_HEAP
    return success;
}

void CoreUiAvkonImpl::setShutdownTimerL()
{
    JELOG2(EJavaUI);
    LOG(EJavaUI, EInfo, "setShutdownTimerL");
    mShutdownTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    mShutdownTimer->Start(8000000, 1000000,
                          TCallBack(shutdownTimerCallback, 0));
}

void CoreUiAvkonImpl::cancelShutdownTimer()
{
    JELOG2(EJavaUI);
    LOG(EJavaUI, EInfo, "cancelShutdownTimer");
    if (mShutdownTimer)
    {
        if (mShutdownTimer->IsActive())
        {
            LOG(EJavaUI, EInfo, "  It was active");
            mShutdownTimer->Cancel();
        }
        delete mShutdownTimer;
        mShutdownTimer = 0;
    }
}

TInt CoreUiAvkonImpl::shutdownTimerCallback(TAny*)
{
    JELOG2(EJavaUI);
    ELOG(EJavaUI, "Killing process forcefully due to timer");
    RProcess().Kill(-1);
    return 0;
}
