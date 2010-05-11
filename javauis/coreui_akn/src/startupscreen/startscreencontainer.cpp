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
 * Description: The CCoeControl for the start screen.
 *
 */

#include <aknlayoutscalable_apps.cdl.h>
#include <data_caging_path_literals.hrh>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#ifdef RD_JAVA_S60_RELEASE_9_2
#include <alf/alfdrawer.h>
#endif // RD_JAVA_S60_RELEASE_9_2

#include "startscreencontainer.h"
#include "startscreen.h"
#include "logger.h"
#include "eswt.h"

const int KStartScreenTimeLimit = 10000; // miliseconds
const int KStartScreenFadeTimeLimit = 3000; // miliseconds
const int KStartScreenFadeTreshold = 500; // miliseconds

CStartScreenContainer* CStartScreenContainer::NewL(
    java::ui::JavaUiAvkonAppUi& appUi,
    const java::ui::CoreUiParams& coreUiParams)
{
    JELOG2(EJavaUI);
    CStartScreenContainer* self = new(ELeave) CStartScreenContainer(appUi,
            coreUiParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CStartScreenContainer::~CStartScreenContainer()
{
    JELOG2(EJavaUI);

    if (!mAppUi.destroying()) 
    {
        if (HidesIndicators())
        {
            ShowIndicators(ETrue);
        }
    }

    mFader.FadeBehindPopup(this, this, EFalse);
    mAppUi.RemoveFromStack(this);

    if (mStartScreenTimer)
    {
        mStartScreenTimer->Stop();
        delete mStartScreenTimer;
        mStartScreenTimer = 0;
    }
    if (mStartScreen)
    {
        delete mStartScreen;
        mStartScreen = 0;
    }
    if (mCba)
    {
        delete mCba;
        mCba = 0;
    }
    if (mBgContext)
    {
        delete mBgContext;
        mBgContext = 0;
    }
}

void CStartScreenContainer::HandleForeground(bool aForeground)
{
    JELOG2(EJavaUI);

    if (!aForeground)
    {
        if (!mCba)
        {
            mStartScreenTimer->Pause();
        }
    }
    else
    {
        if (!mCba)
        {
            mStartScreenTimer->Start();
        }
    }
}

void CStartScreenContainer::ShowExitScreen()
{
    JELOG2(EJavaUI);

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, rect);

    // Turn CBA on
    TRAPD(err, mCba = CEikButtonGroupContainer::NewL(
                          CEikButtonGroupContainer::ECba,
                          CEikButtonGroupContainer::EHorizontal,
                          &mAppUi,
                          0,
                          CEikButtonGroupContainer::EAddToStack));
    if (err != KErrNone)
    {
        ELOG1(EJavaUI, "CStartScreenContainer::ShowExitScreen, "
              "CEikButtonGroupContainer::NewL failed: %d", err);
        return;
    }
    mCba->SetBoundingRect(rect);

    // Show "Exit" RSK on CBA
    TRAP(err, mCba->SetCommandSetL(R_AVKON_SOFTKEYS_EXIT));
    if (err != KErrNone)
    {
        ELOG1(EJavaUI, "CStartScreenContainer::ShowExitScreen, "
              "CEikButtonGroupContainer::SetCommandSetL failed: %d", err);
    }

    // Create background, as we stop being transparent
    if (!mBgContext)
    {
        mBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,
                     rect, ETrue);
    }

    // Add to stack if not already added.
    if (!mStartScreen || mStartScreen->IsEmpty())
    {
        SetRect(rect);
        mAppUi.AddToStackL(this);
        ActivateL();
        MakeVisible(ETrue);
    }

    // Stop fading behind if user defined start screen.
    if (mStartScreen && mStartScreen->Type() == CStartScreen::EStartScreenUserDefined)
    {
        mFader.FadeBehindPopup(this, this, EFalse);
        DrawDeferred();
    }

    // Show the status pane (title pane)
    CEikStatusPane* pane = mAppUi.StatusPane();
    pane->MakeVisible(ETrue);
    if (HidesIndicators())
        ShowIndicators(EFalse);

    // Push behind SP and CBA
    Window().SetOrdinalPosition(0, -1);

    mExitScreenOn = ETrue;
}

void CStartScreenContainer::AsyncSaveL(TCallBack aCallBack)
{
    if (!mStartScreen || mStartScreen->Type() != CStartScreen::EStartScreenAutomatic)
    {
        aCallBack.CallBack();
        return;
    }

    // Save the new start time if going over the treshold.
    TTime time;
    time.UniversalTime();
    TInt64 newTime = time.Int64() - mAppUi.startupTime();
    newTime /= 1000; // from microseconds to miliseconds
    if (mStartTimeKnown)
    {
        if (mFaded)
        {
            // The application started in more than 3 seconds.
            // Override only if lower than 2.5 seconds.
            if (newTime + KStartScreenFadeTreshold < KStartScreenFadeTimeLimit)
                mStartScreen->SaveStartupTime(newTime);
        }
        else
        {
            // The application started in less than 3 seconds.
            // Override only if higher than 3.5 seconds.
            if (newTime - KStartScreenFadeTreshold > KStartScreenFadeTimeLimit)
                mStartScreen->SaveStartupTime(newTime);
        }
    }
    else
    {
        mStartScreen->SaveStartupTime(newTime);
    }

    // Take away the start screen, to uncover the app's UI.
    HideL();

    // Ensure the app's UI is flushed to the screen and take the screenshot.
    FlushScreenL();
    mStartScreen->AsyncSaveScreenL(aCallBack);
}

void CStartScreenContainer::HideL()
{
    mFaded = EFalse;

    // The application is starting to draw so no need for exit screen.
    if (mStartScreenTimer)
    {
        mStartScreenTimer->Stop();
    }

    // If the exit screen has been shown, hide the CBA
    if (mCba)
    {
        mCba->MakeVisible(EFalse);
    }

    // Keep the window on the screen but lower the z-order
    // The following lines of code must stay in this order!
    FlushScreenL();
    Window().SetOrdinalPosition(0, ESwtWinPriorityInactiveTopShell);
    DrawNow();
}

TBool CStartScreenContainer::HidesIndicators() const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    return (mStartScreen && mStartScreen->Type() == CStartScreen::EStartScreenAutomatic);
#else
    return EFalse;
#endif
}

void CStartScreenContainer::Draw(const TRect& aRect) const
{
    JELOG2(EJavaUI);
    CWindowGc& gc = SystemGc();
    if (mBgContext)
    {
        AknsDrawUtils::Background(AknsUtils::SkinInstance(), mBgContext, this, gc, aRect);
    }
    if (mStartScreen)
    {
        mStartScreen->Draw(gc, Rect());
    }
    if (mFaded)
    {
        TRgb col(0, 0, 0);
        col.SetAlpha(125);
        gc.SetBrushColor(col);
        gc.SetPenStyle(CGraphicsContext::ENullPen);
        gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        gc.DrawRect(Rect());
    }
}

void CStartScreenContainer::HandleResourceChange(TInt aType)
{
    JELOG2(EJavaUI);
    CCoeControl::HandleResourceChange(aType);

    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, rect);
        SetRect(rect);
        if (mBgContext)
        {
            mBgContext->SetRect(rect);
        }

        if (mStartScreen)
        {
            mStartScreen->HandleResolutionChange();
            if (!mExitScreenOn)
            {
                MakeVisible(!mStartScreen->IsEmpty());
            }
        }

        DrawDeferred();
    }
}

TInt CStartScreenContainer::CountFadedComponents()
{
    return 1;
}

CCoeControl* CStartScreenContainer::FadedComponent(TInt /*aIndex*/)
{
    return this;
}

CStartScreenContainer::CStartScreenContainer(java::ui::JavaUiAvkonAppUi& appUi,
        const java::ui::CoreUiParams& coreUiParams)
        : CCoeControl()
        , mAppUi(appUi)
        , mCoreUiParams(coreUiParams)
{
    JELOG2(EJavaUI);
}

void CStartScreenContainer::ConstructL()
{
    JELOG2(EJavaUI);

    mStartScreenTimer = CStartScreenTimer::NewL(*this);
    mStartScreenTimer->Start();

    CreateWindowL();
    SetMopParent(&mAppUi);

    // The start screen window needs to be on top of the ui,
    // in order to hide the quirks of ui initialization.
    Window().SetOrdinalPosition(0, 10);

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, rect);

    TBool transparent = EFalse;
    if (mCoreUiParams.getScreenMode() == java::ui::MIDLET_DEFINED_SCREEN ||
            mCoreUiParams.getScreenMode() == java::ui::USER_DEFINED_SCREEN)
    {
        // User defined start screen.
        TRAPD(err,
              mStartScreen = CStartScreen::NewL(CStartScreen::EStartScreenUserDefined,
                                                mCoreUiParams));
        if (KErrNone != err)
        {
            ELOG1(EJavaUI, "CStartScreenContainer::ConstructL, "
                  "CStartScreen::NewL failed: %d", err);
        }
        else
        {
            // There is no point in having empty user defined start screen.
            if (mStartScreen->IsEmpty())
            {
                delete mStartScreen;
                mStartScreen = 0;
            }
        }

        if (mStartScreen)
        {
            RWindow& window = Window();

            // Try to enable window transparency
            if (CAknEnv::Static()->TransparencyEnabled())
            {
                window.SetRequiredDisplayMode(EColor16MA);
                if (window.SetTransparencyAlphaChannel() == KErrNone)
                {
                    window.SetBackgroundColor(~0);
                    transparent = ETrue;
                }
            }

            // If the user start image is fullscreen, no need for fading behind
            TSize imgSize = mStartScreen->Size();
            if (imgSize.iWidth < rect.Width() || imgSize.iHeight < rect.Height())
            {
                mFader.FadeBehindPopup(this, this, ETrue);
            }

            if (!transparent)
            {
                mBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,
                             rect, ETrue);
            }
        }
    }

    if (!mStartScreen)
    {
        // Automatic start screen
        mStartScreen = CStartScreen::NewL(CStartScreen::EStartScreenAutomatic,
                                          mCoreUiParams);
        if (!mStartScreen->IsEmpty())
        {
            TInt64 period = mStartScreen->LoadStartupTime();
            if (period > 0)
            {
                mStartTimeKnown = ETrue;
                if (period > KStartScreenFadeTimeLimit)
                {
                    // Possible alternative, but too dark:
                    // Window().SetFaded(ETrue, RWindowTreeNode::EFadeWindowOnly);
                    mFaded = ETrue;
                }
            }
        }
    }

    if (HidesIndicators())
        ShowIndicators(EFalse);

    if (mStartScreen && !mStartScreen->IsEmpty())
    {
        SetRect(rect);
        mAppUi.AddToStackL(this);
        ActivateL();
        MakeVisible(ETrue);
    }
    else
    {
        MakeVisible(EFalse);
    }
}

void CStartScreenContainer::ShowIndicator(CEikStatusPane* aSp, TInt aId, TBool aVisible)
{
    TUid uid = TUid::Uid(aId);
    CEikStatusPaneBase::TPaneCapabilities subPane = aSp->PaneCapabilities(uid);
    if (subPane.IsPresent() && subPane.IsAppOwned())
    {
        CCoeControl* ctrl = NULL;
        TRAP_IGNORE(ctrl = aSp->ControlL(uid));
        if (ctrl)
        {
            ctrl->MakeVisible(aVisible);
        }
    }
}

void CStartScreenContainer::ShowIndicators(TBool aVisible)
{
    CEikStatusPane* sp = mAppUi.StatusPane();
    if (!sp)
        return;

    ShowIndicator(sp, EEikStatusPaneUidSignal, aVisible);
    ShowIndicator(sp, EEikStatusPaneUidBattery, aVisible);
    ShowIndicator(sp, EEikStatusPaneUidIndic, aVisible);
    ShowIndicator(sp, EEikStatusPaneUidMessage, aVisible);
    ShowIndicator(sp, EEikStatusPaneUidClock, aVisible);
    ShowIndicator(sp, EEikStatusPaneUidDigitalClock, aVisible);

    if (aVisible)
        sp->DrawNow();
}

void CStartScreenContainer::FlushScreenL()
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    CAlfDrawer* drawer = CAlfDrawer::NewL();
    CleanupStack::PushL(drawer);
    TInt err = drawer->Finish();
    CleanupStack::PopAndDestroy(drawer);

    if (KErrNone != err)
    {
        // Try again
        ELOG1(EJavaUI, "CStartScreenContainer::FlushScreenL, "
              "CAlfDrawer::Finish failed: %d", err);
        iEikonEnv->WsSession().Flush();
        iEikonEnv->WsSession().Finish();
    }
#else
    iEikonEnv->WsSession().Flush();
    iEikonEnv->WsSession().Finish();
#endif // RD_JAVA_S60_RELEASE_9_2
}

CStartScreenTimer* CStartScreenTimer::NewL(CStartScreenContainer& aSc)
{
    JELOG2(EJavaUI);

    CStartScreenTimer* self = new(ELeave) CStartScreenTimer(aSc);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CStartScreenTimer::~CStartScreenTimer()
{
    JELOG2(EJavaUI);
    Cancel();
}

void CStartScreenTimer::Start()
{
    JELOG2(EJavaUI);
    // Converting secs to micro secs.
    mTimerStartTime.HomeTime();
    if (!IsActive())
    {
        TInt timerVal = (TInt)(mTimerValue.Int64());
        if (timerVal > 0)
        {
            After(timerVal);
        }
        else
        {
            mSc.ShowExitScreen();
        }
    }
}

void CStartScreenTimer::Pause()
{
    JELOG2(EJavaUI);
    if (IsActive())
    {
        TTime currentTime;
        currentTime.HomeTime();
        TTimeIntervalMicroSeconds delta(currentTime.MicroSecondsFrom(
                                            mTimerStartTime));
        mTimerValue = mTimerValue.Int64() - delta.Int64();
        Stop();
    }
}

void CStartScreenTimer::Stop()
{
    JELOG2(EJavaUI);
    if (IsActive())
        Cancel();
}

void CStartScreenTimer::RunL()
{
    JELOG2(EJavaUI);
    mSc.ShowExitScreen();
}

void CStartScreenTimer::DoCancel()
{
    CTimer::DoCancel();
}

CStartScreenTimer::CStartScreenTimer(
    CStartScreenContainer& aSc) :
        CTimer(CActive::EPriorityStandard), mSc(aSc), mTimerValue(
            KStartScreenTimeLimit * 1000)

{
    JELOG2(EJavaUI);
    CActiveScheduler::Add(this);
}

void CStartScreenTimer::ConstructL()
{
    JELOG2(EJavaUI);
    CTimer::ConstructL();
}
