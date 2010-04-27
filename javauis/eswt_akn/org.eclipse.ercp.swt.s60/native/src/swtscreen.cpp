/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <w32std.h>
#include <hal.h>
#include <aknappui.h>
#include <eikenv.h>
#include <swtlaffacade.h>
#include "swtscreen.h"
#include "swtmobiledevice.h"


// Id of the primary hardware screen
const TInt KDefaultHwScreenId = 0;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtScreen::NewL
// ---------------------------------------------------------------------------
//
CSwtScreen* CSwtScreen::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aId)
{
    CSwtScreen* self = new(ELeave) CSwtScreen(aDisplay, aPeer, aId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtScreen::CSwtScreen
// ---------------------------------------------------------------------------
//
CSwtScreen::CSwtScreen(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aId)
        : iDisplay(aDisplay), iPeer(aPeer), iId(aId)
{
}

// ---------------------------------------------------------------------------
// CSwtScreen::~CSwtScreen
// ---------------------------------------------------------------------------
//
CSwtScreen::~CSwtScreen()
{
    iDisplay.MobileDevice()->RemoveObserver(this);
    iDisplay.RemoveResourceChangeObserver(this);
}

// ---------------------------------------------------------------------------
// CSwtScreen::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtScreen::ConstructL()
{
    iDisplay.AddResourceChangeObserverL(this);
    iDisplay.MobileDevice()->AddObserverL(this);
    iScreenDevice = iDisplay.CoeEnv()->ScreenDevice();
    if (!iScreenDevice)
    {
        User::Leave(ESwtErrorCannotGetSelection);
    }
    iIsTheOnlyScreen = (iDisplay.MobileDevice()->GetScreenNumber() == 1);
    iOrientationState = EOrientationNotSet;
    if (IsScreenOn())
    {
        iOrientation = CurrentOrientation();
    }
    else
    {
        iOrientation = KErrNotFound;
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::IsScreenOn
// ---------------------------------------------------------------------------
//
TBool CSwtScreen::IsScreenOn() const
{
    if (!iIsTheOnlyScreen)
    {
        if (iScreenDevice->GetScreenNumber() == iId)
        {
            return iActivationState;
        }
        else
        {
            return EFalse;
        }
    }
    else
    {
        return ETrue;
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::DoSetOrientationL
// ---------------------------------------------------------------------------
//
void CSwtScreen::DoSetOrientationL()
{
    if (iOrientation == MSwtScreen::ELandscape)
    {
        iAvkonAppUi->SetOrientationL(CAknAppUiBase::EAppUiOrientationLandscape);
    }
    else
    {
        iAvkonAppUi->SetOrientationL(CAknAppUiBase::EAppUiOrientationPortrait);
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::DoRefreshOrientationL
// ---------------------------------------------------------------------------
//
void CSwtScreen::DoRefreshOrientationL()
{
    // Just to be safe clear and restore.
    if (iOrientation == MSwtScreen::ELandscape)
    {
        iAvkonAppUi->SetOrientationL(
            CAknAppUiBase::EAppUiOrientationPortrait);
        iAvkonAppUi->SetOrientationL(
            CAknAppUiBase::EAppUiOrientationLandscape);
    }
    else
    {
        iAvkonAppUi->SetOrientationL(
            CAknAppUiBase::EAppUiOrientationLandscape);
        iAvkonAppUi->SetOrientationL(
            CAknAppUiBase::EAppUiOrientationPortrait);
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::CurrentOrientation
// ---------------------------------------------------------------------------
//
TInt CSwtScreen::CurrentOrientation()
{
    if (CSwtLafFacade::IsLandscapeOrientation())
    {
        return MSwtScreen::ELandscape;
    }
    else
    {
        return MSwtScreen::EPortrait;
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::OfferResourceChangeL
// From MSwtResourceChangeObserver
// ---------------------------------------------------------------------------
//
void CSwtScreen::OfferResourceChangeL(TInt aType)
{
    // During a fast repetitive swap of the flip no screen will be active. This
    // is ensured by the MobileDevice implementation. Therefore only when the
    // fast repetitive swap ends a this layout change will be handled once.
    if (aType == KEikDynamicLayoutVariantSwitch && IsScreenOn())
    {
        TInt currOrientation = CurrentOrientation();
        switch (iOrientationState)
        {
        case EOrientationSet:
            if (iOrientation == currOrientation)
            {
                iOrientationState = EOrientationAccepted;
            }
            else
            {
                iOrientationState = EOrientationRejected;
            }
            break;

        case EOrientationNotSet:
        case EOrientationRejected:
            if (iOrientation != currOrientation
                    && iOrientation != KErrNotFound)
            {
                iDisplay.PostScreenEventL(iPeer,
                                          KEventScreenOrientationChanged);
            }
            iOrientation = currOrientation;
            break;

        default:
            // Do nothing.
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::ScreenDeactivatedL
// From MSwtMobileDeviceObserver
// ---------------------------------------------------------------------------
//
void CSwtScreen::ScreenDeactivatedL(TInt aScreenId)
{
    if (iId == aScreenId)
    {
        iActivationState = EFalse;
        iDisplay.PostScreenEventL(iPeer, KEventScreenDeactivated);
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::ScreenActivatedL
// From MSwtMobileDeviceObserver
// ---------------------------------------------------------------------------
//
void CSwtScreen::ScreenActivatedL(TInt aScreenId)
{
    if (iId == aScreenId)
    {
        iActivationState = ETrue;
        iDisplay.PostScreenEventL(iPeer, KEventScreenActivated);

        switch (iOrientationState)
        {
        case EOrientationPending:
            // Do not change instruction order!
            iOrientationState = EOrientationSet;
            DoSetOrientationL();
            break;

        case EOrientationAccepted:
            DoRefreshOrientationL();
            break;

        default:
            // Do nothing.
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::Dispose
// From MSwtScreen
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtScreen::Dispose()
{
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}

// ---------------------------------------------------------------------------
// CSwtScreen::GetBounds
// From MSwtScreen
// ---------------------------------------------------------------------------
//
TRect CSwtScreen::GetBounds()
{
    if (IsScreenOn())
    {
        return TRect(iScreenDevice->SizeInPixels());
    }
    else
    {
        return TRect();
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::GetColorDepth
// From MSwtScreen
// ---------------------------------------------------------------------------
//
TInt CSwtScreen::GetColorDepth()
{
    if (IsScreenOn())
    {
        return TDisplayModeUtils::NumDisplayModeBitsPerPixel(
                   iScreenDevice->DisplayMode());
    }
    else
    {
        return KErrNotFound;
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::GetLocation
// From MSwtScreen
// ---------------------------------------------------------------------------
//
TInt CSwtScreen::GetLocation()
{
    if (iDisplay.MobileDevice()->GetScreenNumber() > 1)
    {
        if (iId == KDefaultHwScreenId)
        {
            return MSwtMobileDevice::EInternal;
        }
        else
        {
            return MSwtMobileDevice::EExternal;
        }
    }
    else
    {
        return MSwtMobileDevice::ELocal;
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::GetOrientation
// From MSwtScreen
// ---------------------------------------------------------------------------
//
TInt CSwtScreen::GetOrientation()
{
    if (IsScreenOn())
    {
        return CurrentOrientation();
    }
    else
    {
        return KErrNotFound;
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::GetUsage
// From MSwtScreen
// ---------------------------------------------------------------------------
//
TInt CSwtScreen::GetUsage()
{
    return MSwtScreen::EPrimary;
}

// ---------------------------------------------------------------------------
// CSwtScreen::IsActive
// From MSwtScreen
// ---------------------------------------------------------------------------
//
TBool CSwtScreen::IsActive()
{
    return IsScreenOn();
}

// ---------------------------------------------------------------------------
// CSwtScreen::IsTouchScreen
// From MSwtScreen
// ---------------------------------------------------------------------------
//
TBool CSwtScreen::IsTouchScreen()
{
    if (IsScreenOn())
    {
        return AknLayoutUtils::PenEnabled();
    }
    else
    {
        return EFalse;
    }
}

// ---------------------------------------------------------------------------
// CSwtScreen::SetOrientationL
// From MSwtScreen
// ---------------------------------------------------------------------------
//
void CSwtScreen::SetOrientationL(TInt aOrientation)
{
    iOrientation = aOrientation;
    if (IsScreenOn())
    {
        // Do not change instruction order!
        iOrientationState = EOrientationSet;
        DoSetOrientationL();
    }
    else
    {
        iOrientationState = EOrientationPending;
    }
}
