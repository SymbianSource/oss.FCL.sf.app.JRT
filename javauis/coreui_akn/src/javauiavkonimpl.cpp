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
* Description: Core ui implementation for the Avkon based UI framework.
*
*/

#include "logger.h"
#include "coreuiavkonimpl.h"
#include "javauiavkonimpl.h"
#include "startscreencontainer.h"

using namespace java::ui;


JavaUiAvkonAppUi::JavaUiAvkonAppUi()
{
    JELOG2(EJavaUI);
    CoreUiAvkonImpl::getInstanceImpl().setJavaUiAppUi(this);
    TTime time;
    time.UniversalTime();
    mStartupTime = time.Int64();
}

JavaUiAvkonAppUi::~JavaUiAvkonAppUi()
{
    JELOG2(EJavaUI);
    if (mAppView)
    {
        delete mAppView;
        mAppView = 0;
    }
    removeChild(mLcduiChild);
    removeChild(mEswtChild);
    if (mOrientationTimer)
    {
        if (mOrientationTimer->IsActive())
        {
            mOrientationTimer->Cancel();
        }
        delete mOrientationTimer;
        mOrientationTimer = 0;
    }
}

void JavaUiAvkonAppUi::ConstructL()
{
    JELOG2(EJavaUI);

    CoreUiAvkonImpl& coreUiAvkon = CoreUiAvkonImpl::getInstanceImpl();
    const CoreUiParams& coreUiParams = coreUiAvkon.getCoreUiParams();

    TInt appUiFlags = ENoAppResourceFile |
                      EAknEnableSkin |
                      EAknEnableMSK |
                      EAknTouchCompatible;

#ifdef RD_JAVA_S60_RELEASE_9_2
    appUiFlags |= EAknSingleClickCompatible;
#endif

    switch (coreUiParams.getOrientation())
    {
    case java::ui::PORTRAIT:
        appUiFlags |= EAppOrientationPortrait;
        break;
    case java::ui::LANDSCAPE:
        appUiFlags |= EAppOrientationLandscape;
        break;
    }
    BaseConstructL(appUiFlags);
    mIsForeground = true;

    // Set the priority of the process to background for short time
    // in order to avoid Window server panicing the starting app
    // (e.g. menu) if the MIDlet starts to consume 100% cpu.
    const int timeForBg = 1000;
    setBackgroundProcessPriority(timeForBg);
    if (coreUiAvkon.showStartScreen())
    {
        TRAPD(err, mAppView = CStartScreenContainer::NewL(*this, coreUiParams));
        if (err != KErrNone)
        {
            ELOG1(EJavaUI, "JavaUiAvkonAppUi::ConstructL, "
                  "CStartScreenContainer::NewL failed: %d", err);
            delete mAppView;
            mAppView = 0;
        }
        else
        {
            iEikonEnv->RootWin().EnableScreenChangeEvents();
        }
    }
}



void JavaUiAvkonAppUi::HandleForegroundEventL(TBool foreground)
{
    JELOG2(EJavaUI);
    CAknAppUi::HandleForegroundEventL(foreground);
    if (hasStartScreen())
    {
        ASSERT(mAppView);
        mAppView->HandleForeground(foreground);
    }
    if (mActiveChild)
    {
        mActiveChild->HandleForegroundEventL(foreground);
    }
}

void JavaUiAvkonAppUi::HandleSwitchOnEventL(CCoeControl* destination)
{
    JELOG2(EJavaUI);
    if (mActiveChild)
    {
        mActiveChild->HandleSwitchOnEventL(destination);
    }
}

void JavaUiAvkonAppUi::HandleResourceChangeL(TInt type)
{
    JELOG2(EJavaUI);
    CAknAppUi::HandleResourceChangeL(type);
    if (mActiveChild)
    {
        mActiveChild->HandleResourceChangeL(type);
    }
}

void JavaUiAvkonAppUi::HandleApplicationSpecificEventL(TInt type,const TWsEvent& event)
{
    JELOG2(EJavaUI);
    if (mActiveChild)
    {
        mActiveChild->HandleApplicationSpecificEventL(type, event);
    }
    CAknAppUi::HandleApplicationSpecificEventL(type, event);
}

void JavaUiAvkonAppUi::HandleCommandL(TInt command)
{
    JELOG2(EJavaUI);

    if (hasStartScreen())
    {
        if (command == EEikCmdExit ||
                command == EAknSoftkeyBack ||
                command == EAknSoftkeyExit ||
                command == EAknSoftkeyClose)
        {
            RWindowGroup& group = iCoeEnv->RootWin();
            const int KWgOrdinalBackground = -1;
            group.SetOrdinalPosition(KWgOrdinalBackground);
            CoreUiAvkonImpl::getInstanceImpl().shutDownRequestFromWindowServer();
            mStartupCancelled = true;
        }
    }
    else
    {
        if (mActiveChild)
        {
            mActiveChild->HandleCommandL(command);
        }
    }
}

void JavaUiAvkonAppUi::HandleWsEventL(const TWsEvent& event,
                                      CCoeControl* destination)
{
    JELOG2(EJavaUI);

    if (event.Type() == EEventScreenDeviceChanged)
    {
        LOG(EJavaUI, EInfo, "Got orientation change event");
        // This is for handling the rotation event.
        // We keep the current priority for a short period. If we are in
        // foreground, this gives us all the available CPU to do the
        // orientation change as fast as possible.
        // After the timer elapses, we set the process priority to BG for
        // some period. This is done in order to avoid problems in such
        // MIDlets that consume 100% cpu.

        const int timeForCurrentPriority = 1000;
        setDefaultProcessPriority(timeForCurrentPriority);
    }

    if (hasStartScreen())
    {
        ASSERT(mAppView);
        if (event.Type() == EEventFocusLost)
        {
            mIsForeground = false;
            mAppView->HandleForeground(false);
        }
        else if (event.Type() == EEventFocusGained)
        {
            mIsForeground = true;
            mAppView->HandleForeground(true);
        }
    }
    bool eventBlocked = false;
    if (mActiveChild)
    {
        eventBlocked = mActiveChild->HandleWsEventL(event, destination);
    }
    if (!eventBlocked)
    {
        CAknAppUi::HandleWsEventL(event, destination);
    }
}

void JavaUiAvkonAppUi::setLcduiChild(CoreAppUiChild* child, int resourceFileOffset)
{
    JELOG2(EJavaUI);
    if (!mLcduiChild)
    {
        mLcduiChild = child;
        mActiveChild = mLcduiChild;
        mPassiveChild = mEswtChild;
        mLcduiResourceFile = resourceFileOffset;
    }
}

void JavaUiAvkonAppUi::setEswtChild(CoreAppUiChild* child)
{
    JELOG2(EJavaUI);
    if (!mEswtChild)
    {
        mEswtChild = child;
        mActiveChild = mEswtChild;
        mPassiveChild = mLcduiChild;
    }
}

void JavaUiAvkonAppUi::removeChild(CoreAppUiChild* child)
{
    JELOG2(EJavaUI);

    if (child == mLcduiChild)
    {
        delete mLcduiChild;
        CCoeEnv::Static()->DeleteResourceFile(mLcduiResourceFile);
        mLcduiChild = 0;
        mLcduiResourceFile = 0;
        mActiveChild = mEswtChild;
    }
    else if (child == mEswtChild)
    {
        delete mEswtChild;
        mEswtChild = 0;
        mActiveChild = mLcduiChild;
    }
    if (child == mPassiveChild)
    {
        mPassiveChild = 0;
    }
}

CoreAppUiChild* JavaUiAvkonAppUi::getLcduiChild() const
{
    JELOG2(EJavaUI);
    return mLcduiChild;
}

bool JavaUiAvkonAppUi::hasStartScreen() const
{
    return mAppView && !mAppViewExiting;
}

void JavaUiAvkonAppUi::stopStartScreen(bool aFullScreenUi /*=true*/)
{
    JELOG2(EJavaUI);

    if (hasStartScreen())
    {
        if (aFullScreenUi && mIsForeground && IsForeground())
        {
            TRAPD(err, mAppView->AsyncSaveL(TCallBack(JavaUiAvkonAppUi::AsyncSaveComplete, this)));
            if (KErrNone != err)
            {
                ELOG1(EJavaUI, "JavaUiAvkonAppUi::stopStartScreen, "
                      "CStartScreenContainer::AsyncSaveL failed: %d", err);
            }
            else
            {
                mAppViewExiting = ETrue;
            }
        }

        if (!mAppViewExiting)
        {
            delete mAppView;
            mAppView = 0;
        }
    }
}

bool JavaUiAvkonAppUi::isClosingPossible() const
{
    JELOG2(EJavaUI);
    return mActiveChild == 0;
}

TInt64 JavaUiAvkonAppUi::startupTime() const
{
    return mStartupTime;
}

bool JavaUiAvkonAppUi::isForeground() const
{
    JELOG2(EJavaUI);
    return mIsForeground;
}

void JavaUiAvkonAppUi::glueSetKeyBlockMode(TAknKeyBlockMode aMode)
{
    JELOG2(EJavaUI);
    SetKeyBlockMode(aMode);
}

bool JavaUiAvkonAppUi::hidesIndicators() const
{
    return hasStartScreen() && mAppView->HidesIndicators();
}

bool JavaUiAvkonAppUi::isStartupCancelled() const
{
    return mStartupCancelled;
}

TInt JavaUiAvkonAppUi::orientationTimerCallback(TAny* ptr)
{
    JELOG2(EJavaUI);
    JavaUiAvkonAppUi* appUi = reinterpret_cast<JavaUiAvkonAppUi*>(ptr);
    appUi->handleElapsedTimer();
    return 0;
}

void JavaUiAvkonAppUi::handleElapsedTimer()
{
    JELOG2(EJavaUI);
    LOG(EJavaUI, EInfo, "handleElapsedTimer");
    if (mIsDefaultPriorityInUse)
    {
        // We have now been the defined time in default priority. Now
        // it is time to explicitly set to background priority for a while.
        const int timeForBg = 6000;
        int status = setBackgroundProcessPriority(timeForBg);
        if (status != KErrNone)
        {
            ELOG1(EJavaUI, "JavaUiAvkonAppUi::handleElapsedTimer, "
                  "setBackgroundProcessPriority failed: %d", status);
            // Setting timer failed, so lets go immediately back to default priority.
            setDefaultProcessPriority(0);
        }
    }
    else
    {
        setDefaultProcessPriority(0);
        if (mOrientationTimer->IsActive())
        {
            mOrientationTimer->Cancel();
        }
        delete mOrientationTimer;
        mOrientationTimer = 0;
    }
}

int JavaUiAvkonAppUi::setDefaultProcessPriority(TInt time)
{
    JELOG2(EJavaUI);
    LOG1(EJavaUI, EInfo, "Setting default priority: %d", RProcess().Priority());
    // Client process's priority is set to EPriorityForeground when the
    // window group takes focus, and set to EPriorityBackground when it
    // loses focus.
    iCoeEnv->WsSession().ComputeMode(RWsSession::EPriorityControlComputeOff);
    mIsDefaultPriorityInUse = true;
    TRAPD(status, startTimerL(time));
    return status;
}

int JavaUiAvkonAppUi::setBackgroundProcessPriority(TInt time)
{
    JELOG2(EJavaUI);

    // eSWT MIDlets should not be put to background priority after orientation change.
    // Installer drawing is havily affected if the priority of the process is lowered.
    if (mActiveChild == mEswtChild)
        return KErrNone;
    if (mActiveChild == mLcduiChild)
        return KErrNone;

    LOG1(EJavaUI, EInfo, "Setting background priority: %d", RProcess().Priority());
    // Client process's priority is always set to EPriorityBackground.
    iCoeEnv->WsSession().ComputeMode(RWsSession::EPriorityControlComputeOn);
    mIsDefaultPriorityInUse = false;
    TRAPD(status, startTimerL(time));
    return status;
}


void JavaUiAvkonAppUi::startTimerL(TInt time)
{
    JELOG2(EJavaUI);
    if (time > 0)
    {
        LOG1(EJavaUI, EInfo, "startTimer for %d ms.", time);
        if (mOrientationTimer == 0)
        {
            mOrientationTimer = CPeriodic::NewL(CActive::EPriorityStandard);
        }
        else
        {
            if (mOrientationTimer->IsActive())
            {
                mOrientationTimer->Cancel();
            }
        }
        mOrientationTimer->Start(time * 1000, 10 * 1000 * 1000,
                                 TCallBack(orientationTimerCallback, this));
    }
}

TInt JavaUiAvkonAppUi::AsyncSaveComplete(TAny* aObj)
{
    JavaUiAvkonAppUi* appUi = static_cast<JavaUiAvkonAppUi*>(aObj);
    if (appUi)
        appUi->DoAsyncSaveComplete();
    return 0;
}

void JavaUiAvkonAppUi::DoAsyncSaveComplete()
{
    delete mAppView;
    mAppView = 0;
}

TUid JavaUiAvkonApplication::AppDllUid() const
{
    JELOG2(EJavaUI);
    return CoreUiAvkonImpl::getInstanceImpl().getMidletTUid();
}


CApaDocument* JavaUiAvkonApplication::CreateDocumentL()
{
    JELOG2(EJavaUI);
    CApaDocument* document = new(ELeave) JavaUiAvkonDocument(*this);
    return document;
}


TFileName JavaUiAvkonApplication::ResourceFileName() const
{
    JELOG2(EJavaUI);
    return TFileName();
}


JavaUiAvkonDocument::JavaUiAvkonDocument(JavaUiAvkonApplication& application)
        : CAknDocument(application)
{
    JELOG2(EJavaUI);
}


CAknAppUi* JavaUiAvkonDocument::CreateAppUiL()
{
    JELOG2(EJavaUI);
    // No need to push the AppUi onto the cleanup stack as it registers itself
    // with the EikonEnv upon creation.
    JavaUiAvkonAppUi* appUi = new(ELeave) JavaUiAvkonAppUi;
    return appUi;
}

CApaApplication* newJavaApplication()
{
    JELOG2(EJavaUI);
    CApaApplication* app = new JavaUiAvkonApplication();
    return app;
}
