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

#ifndef STARTSCREENCONTAINER_H
#define STARTSCREENCONTAINER_H

#include <AknsBasicBackgroundControlContext.h>
#include <AknPopupFader.h>

#include "javauiavkonimpl.h"
#include "javacoreuiparams.h"
#include "startscreen.h"
#include "javauid.h"

class CStartScreenTimer;
class CAknsBasicBackgroundControlContext;
class CStartScreenContainer;

NONSHARABLE_CLASS(CStartScreenContainer): public CCoeControl,
        public MAknFadedComponent
{
public:
    /**
     * Constructs a start screen container.
     * @param aStartTime The UI initialization start moment.
     */
    static CStartScreenContainer* NewL(java::ui::JavaUiAvkonAppUi& appUi,
    const java::ui::CoreUiParams& coreUiParams);
    virtual ~CStartScreenContainer();

    /**
     * Pause the start screen if going to background.
     */
    void HandleForeground(bool aForeground);

    /**
     * Give the user the option of cancelling the slow startup.
     */
    void ShowExitScreen();

    /**
     * Conclude the start screen. Data can be saved to fasten next startup.
     */
    void AsyncSaveL(TCallBack aCallBack);

    /**
     * Either make invisible or go to back of z order.
     */
    void HideL();

    /**
     * Check if status pane indicators are hidden.
     */
    TBool HidesIndicators() const;

// From CCoeControl
private:
    void Draw(const TRect& aRect) const;
    void HandleResourceChange(TInt aType);

// From MAknFadedComponent
public:
    TInt CountFadedComponents();
    CCoeControl* FadedComponent(TInt aIndex);

// Methods
private:
    CStartScreenContainer(java::ui::JavaUiAvkonAppUi& appUi,
                          const java::ui::CoreUiParams& coreUiParams);
    void ConstructL();
    void ShowIndicator(CEikStatusPane* aSp, TInt aId, TBool aVisible);
    void ShowIndicators(TBool aVisible);
    void FlushScreenL();

// Data
private:
    java::ui::JavaUiAvkonAppUi& mAppUi;
    const java::ui::CoreUiParams& mCoreUiParams;
    CStartScreen* mStartScreen;
    CStartScreenTimer* mStartScreenTimer;
    CEikButtonGroupContainer* mCba;
    CAknsBasicBackgroundControlContext* mBgContext;
    TAknPopupFader mFader;
    TBool mStartTimeKnown;
    TBool mFaded;
    TBool mExitScreenOn;
};

NONSHARABLE_CLASS(CStartScreenTimer): public CTimer
{
public:
    static CStartScreenTimer* NewL(CStartScreenContainer& aSc);
    virtual ~CStartScreenTimer();

    void Start();
    void Pause();
    void Stop();

// From CTimer.
public:
    virtual void RunL();
    virtual TInt RunError(TInt aError)
    {
        return aError;
    }
    virtual void DoCancel();

// Methods
private:
    CStartScreenTimer(CStartScreenContainer& aSc);
    void ConstructL();

// Data
private:
    CStartScreenContainer& mSc;
    TTimeIntervalMicroSeconds mTimerValue;
    TTime mTimerStartTime;
};

#endif // STARTSCREENCONTAINER_H
