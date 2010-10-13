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


#include <hal.h>
#include <aknsoundsystem.h>
#include <aknappui.h>
#include <aknenv.h>
#include "swtmobiledevice.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtMobileDevice::NewL
// ---------------------------------------------------------------------------
//
CSwtMobileDevice* CSwtMobileDevice::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer)
{
    CSwtMobileDevice* self = new(ELeave) CSwtMobileDevice(aDisplay, aPeer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::HwScreenCount
// ---------------------------------------------------------------------------
//
TInt CSwtMobileDevice::HwScreenCount()
{
    TInt count(1);
    HAL::Get(HAL::EDisplayNumberOfScreens, count);
    return count;
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::GetHwInputs
// ---------------------------------------------------------------------------
//
void CSwtMobileDevice::GetHwInputs(
    RArray<CSwtMobileDevice::TSwtHwInput>& aInputs)
{
    TInt mask;
    TSwtHwInput tempInput;

    aInputs.Reset();
    HAL::Get(HALData::EKeyboard, mask);

    if (mask & EKeyboard_Full)
    {
        if (mask & EKeyboard_Keypad)
        {
            tempInput.iLocation = MSwtMobileDevice::EInternal;
        }
        else
        {
            tempInput.iLocation = MSwtMobileDevice::ELocal;
        }
        tempInput.iType = MSwtInput::ESoftKeys;
        aInputs.Append(tempInput);
        tempInput.iType = MSwtInput::EFullKeyboard;
        aInputs.Append(tempInput);
    }

    if (mask & EKeyboard_Keypad)
    {
        if (mask & EKeyboard_Full)
        {
            tempInput.iLocation = MSwtMobileDevice::EExternal;
        }
        else
        {
            tempInput.iLocation = MSwtMobileDevice::ELocal;
        }
        tempInput.iType = MSwtInput::ESoftKeys;
        aInputs.Append(tempInput);
        tempInput.iType = MSwtInput::EKeyPad;
        aInputs.Append(tempInput);
    }
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::CSwtMobileDevice
// ---------------------------------------------------------------------------
//
inline CSwtMobileDevice::CSwtMobileDevice(MSwtDisplay& aDisplay, TSwtPeer aPeer)
        : iDisplay(aDisplay), iPeer(aPeer)
{
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::~CSwtMobileDevice
// ---------------------------------------------------------------------------
//
CSwtMobileDevice::~CSwtMobileDevice()
{
    iObservers.Close();
    if (iFlipWatch)
    {
        delete iFlipWatch;
        iFlipWatch = NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtMobileDevice::ConstructL()
{
    // Register with the display
    iDisplay.SetMobileDevice(this);

    // By default the device is open
    iFlipOpen = ETrue;

    // ASSUMPTION: The number of screens does not change
    iScreenCount = HwScreenCount();

    // ASSUMPTION: No need for watching the flip for one screen
    if (iScreenCount > 1)
    {
        iFlipWatch = CSwtFlipWatch::NewL(this);
    }

    iActiveScreenId = iDisplay.CoeEnv()->ScreenDevice()->GetScreenNumber();
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::FlipChangingL
// From MSwtFlipObserver
// ---------------------------------------------------------------------------
//
void CSwtMobileDevice::FlipChangingL()
{
    // For performance reasons while in fast sequence of swaps the observers
    // will get screen deactivated event only once.
    if (!iFlipChanging)
    {
        for (TInt i = 0; i < iObservers.Count(); i++)
        {
            iObservers[i]->ScreenDeactivatedL(iActiveScreenId);
        }
    }
    iFlipChanging = ETrue;
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::FlipChangedL
// From MSwtFlipObserver
// ---------------------------------------------------------------------------
//
void CSwtMobileDevice::FlipChangedL(TBool aState)
{
    iFlipChanging = EFalse;

    // Report flip change
    TEventType eswtEvent;
    if (aState)
    {
        eswtEvent = EEventOpened;
    }
    else
    {
        eswtEvent = EEventClosed;
    }
    iFlipOpen = aState;
    iDisplay.PostMobileDeviceEventL(iPeer, eswtEvent);

    // For performance reasons while in fast sequence of swaps the observers
    // will get screen deactivated event only once.
    // ASSUMPTION: The screen changes if the flip changes.
    iActiveScreenId = iDisplay.CoeEnv()->ScreenDevice()->GetScreenNumber();
    for (TInt i = 0; i < iObservers.Count(); i++)
    {
        iObservers[i]->ScreenActivatedL(iActiveScreenId);
    }
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::AddObserverL
// From MSwtMobileDevice
// ---------------------------------------------------------------------------
//
TBool CSwtMobileDevice::AddObserverL(
    const MSwtMobileDeviceObserver* aObserver)
{
    if (iObservers.Find(aObserver) != KErrNotFound)
    {
        return EFalse;
    }
    User::LeaveIfError(iObservers.Append(aObserver));
    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::Alert
// From MSwtMobileDevice
// ---------------------------------------------------------------------------
//
void CSwtMobileDevice::Alert(TAlertLevel aLevel)
{
    TAvkonSystemSID toneType(EAvkonSIDNoSound);

    switch (aLevel)
    {
    case EAlertLowImportance:
        toneType = EAvkonSIDInformationTone;
        break;
    case EAlertHighImportance:
        toneType = EAvkonSIDWarningTone;
        break;
    case EAlertCritical:
        toneType = EAvkonSIDErrorTone;
        break;
    default:
        ASSERT(EFalse);
        break;
    }

    CEikonEnv* eikonEnv = iDisplay.CoeEnv();
    if (eikonEnv)
    {
        CAknKeySoundSystem* soundSystem = static_cast<CAknAppUi*>(
                                              eikonEnv->EikAppUi())->KeySounds();
        if (soundSystem)
        {
            soundSystem->PlaySound(toneType);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::Dispose
// From MSwtMobileDevice
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtMobileDevice::Dispose()
{
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::GetInputNumber
// From MSwtMobileDevice
// ---------------------------------------------------------------------------
//
TInt CSwtMobileDevice::GetInputNumber() const
{
    TInt res = 0;
    TInt mask = 0;

    HAL::Get(HALData::EKeyboard, mask);

    // WARNING: The platform returns always a single full keyboard so this will
    // not work properly till it is fixed by the platform.
    // ASSUMPTION: For each keyboard count the softkeys also.
    if (mask & EKeyboard_Full)
    {
        res += 2;
    }
    if (mask & EKeyboard_Keypad)
    {
        res += 2;
    }

    return res;
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::GetScreenNumber
// From MSwtMobileDevice
// ---------------------------------------------------------------------------
//
TInt CSwtMobileDevice::GetScreenNumber() const
{
    return HwScreenCount();
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::IsOpen
// From MSwtMobileDevice
// ---------------------------------------------------------------------------
//
TBool CSwtMobileDevice::IsOpen() const
{
    return iFlipOpen;
}

// ---------------------------------------------------------------------------
// CSwtMobileDevice::RemoveObserver
// From MSwtMobileDevice
// ---------------------------------------------------------------------------
//
TBool CSwtMobileDevice::RemoveObserver(
    const MSwtMobileDeviceObserver* aObserver)
{
    TInt pos = iObservers.Find(aObserver);
    if (pos == KErrNotFound)
    {
        return EFalse;
    }
    iObservers.Remove(pos);
    return ETrue;
}
