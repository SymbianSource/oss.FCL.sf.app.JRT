/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Controls device backlight
*
*/



// INCLUDE FILES
#include    "CMIDDeviceControl.h"

#include <jdebug.h>

// CONSTANTS
namespace
{
// Light devices supported by this API.
const TInt KSupportedLightCount = 4;

const TInt KSupportedLights[] = { CHWRMLight::EPrimaryDisplay,
                                  CHWRMLight::EPrimaryKeyboard, CHWRMLight::ESecondaryDisplay,
                                  CHWRMLight::ESecondaryKeyboard
                                };

const TInt KMIDMaxLightLevel = 100;
}


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMIDDeviceControl::CMIDDeviceControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMIDDeviceControl::CMIDDeviceControl(MMIDEnv& aEnv)
        : iEnv(aEnv),
        iLightLevel(KErrNotFound),
        iSupportedLights(CHWRMLight::ENoTarget)
{
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMIDDeviceControl::ConstructL()
{
    DEBUG("nokialcdui::CMIDDeviceControl::ConstructL +");

    iLight = CHWRMLight::NewL(this);

    iEnv.AddObserverL(*this);

    // Find out light devices on the phone that are supported
    // by our API.

    TInt supportedLights = iLight->SupportedTargets();

    TInt mask = (CHWRMLight::EPrimaryDisplayAndKeyboard |
                 CHWRMLight::ESecondaryDisplayAndKeyboard);

    iSupportedLights = (supportedLights & mask);

    DEBUG("nokialcdui::CMIDDeviceControl::ConstructL -");
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::NewLC
// Two-phased constructor. Leaves object in cleanup stack.
// -----------------------------------------------------------------------------
//
CMIDDeviceControl* CMIDDeviceControl::NewLC(MMIDEnv& aEnv)
{
    CMIDDeviceControl* self = new(ELeave) CMIDDeviceControl(aEnv);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}


// Destructor
CMIDDeviceControl::~CMIDDeviceControl()
{
    Release();
    delete iLight;
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::resetUserInactivityTime
// Reset the user inactivity time
// -----------------------------------------------------------------------------
//
void CMIDDeviceControl::GetUserInactivityTimeL(TInt* aRetVal)
{
    DEBUG("nokialcdui::CMIDDeviceControl::GetUserInactivityTimeL +");
    TTimeIntervalSeconds ValInterval = User::InactivityTime() ;
    *aRetVal = ValInterval.Int();
    DEBUG("nokialcdui::CMIDDeviceControl::GetUserInactivityTimeL -");
}

void CMIDDeviceControl::StaticGetUserInactivityTimeL(CMIDDeviceControl* aDC,
        TInt* aRetVal)
{
    aDC->GetUserInactivityTimeL(aRetVal);
}


// -----------------------------------------------------------------------------
// CMIDDeviceControl::resetUserInactivityTime
// Reset the user inactivity time
// -----------------------------------------------------------------------------
//
void CMIDDeviceControl::ResetUserInactivityTimeL()
{
    DEBUG("nokialcdui::CMIDDeviceControl::ResetUserInactivityTime +");
    User::ResetInactivityTime();
    DEBUG("nokialcdui::CMIDDeviceControl::ResetUserInactivityTime -");
}

void CMIDDeviceControl::StaticResetUserInactivityTimeL(CMIDDeviceControl* aDC)
{
    aDC->ResetUserInactivityTimeL();
}

/**
 * Requests a flashing effect for the device's backlight
 * Returns false as the emulator does not support this feature.
 */
#if defined(__WINSCW__)

TBool CMIDDeviceControl::FlashBacklightL(TInt /*aDuration*/)
{
    DEBUG("nokialcdui::CMIDDeviceControl::FlashBacklightL");

    return EFalse;
}

#else // __WINSCW__

/**
 * Requests a flashing effect for the device's backlight
 * Must return true if the backlight can be controlled by the application
 */
TBool CMIDDeviceControl::FlashBacklightL(TInt aDuration)
{
    DEBUG_INT("nokialcdui::CMIDDeviceControl::FlashBacklightL( %d ) +",
              aDuration);

    // Return EFalse if backlights cannot be controlled.
    if (iSupportedLights == CHWRMLight::ENoTarget)
    {
        return EFalse;
    }

    // Delay the screen saver starting.
    User::ResetInactivityTime();

    // Reserve the light(s) if not yet reserved.
    if (iLightLevel == KErrNotFound)
    {
        iLight->ReserveLightL(iSupportedLights);
    }

    if (!aDuration)
    {
        // 0 would be infinite
        aDuration = 1;
    }
    iDuration = aDuration;
    iStartTime = new TTime();
    iStartTime->HomeTime();
    iLight->LightBlinkL(iSupportedLights, aDuration);

    DEBUG_INT("nokialcdui::CMIDDeviceControl::FlashBacklightL( %d ) -",
              aDuration);

    return ETrue;
}
#endif // __WINSCW__


void CMIDDeviceControl::StaticFlashBacklightL(
    CMIDDeviceControl* aDC,
    TInt aDuration,
    TBool* aRetVal)
{
    *aRetVal = aDC->FlashBacklightL(aDuration);
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::SetLightL
// sets the backlight level
// -----------------------------------------------------------------------------
//
void CMIDDeviceControl::SetLightL(TInt aLevel)
{
    DEBUG_INT("nokialcdui::CMIDDeviceControl::SetLightL( %d ) +", aLevel);

    if ((aLevel < 0) || (aLevel > KMIDMaxLightLevel))
    {
        User::Leave(KErrArgument);
    }

    // Delay the screen saver starting.
    User::ResetInactivityTime();

    // Do nothing if backlights cannot be controlled.
    if (iSupportedLights != CHWRMLight::ENoTarget)
    {
        iLightLevel = aLevel;

        // Do nothing at the moment if the blinking is ongoing.
        // Adjust the new light level when the blinking has finished
        // (LightStatusChanged() is called).
        TBool isBlinking = LightsBlinking();

        if (!isBlinking)
        {
            DoSetLightsL(aLevel);
        }
    }

    DEBUG_INT("nokialcdui::CMIDDeviceControl::SetLightL( %d ) -", aLevel);
}



void CMIDDeviceControl::HandleSwitchOnL(TBool /*aSwitchOn*/)
{
}

void CMIDDeviceControl::HandleForegroundL(TBool aForeground)
{
    DEBUG_INT2("nokialcdui::CMIDDeviceControl::HandleForegroundL; aForeground=%d; iDuration=%d", aForeground, iDuration);

    if (aForeground && iDuration > 0)
    {
        TTime* now = new TTime();
        now->HomeTime();
        TInt nowDuration = now->MicroSecondsFrom(*iStartTime).Int64()/1000;
        if (nowDuration >= iDuration)
        {
            iDuration = 0;
        }
        else
        {
            DEBUG_INT("nokialcdui::CMIDDeviceControl::HandleForegroundL - continue blinking for ( %d ) -", (iDuration - nowDuration));
            FlashBacklightL(iDuration - nowDuration);
        }
    }
}

void CMIDDeviceControl::HandleResourceChangeL(TInt /*aType*/)
{
}

void CMIDDeviceControl::StaticSetLightL(CMIDDeviceControl* aDC, TInt aLevel)
{
    aDC->SetLightL(aLevel);
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::Release
// Returns the backlight mode to sensor and releases resources
// -----------------------------------------------------------------------------
//
void CMIDDeviceControl::Release()
{
    DEBUG("nokialcdui::CMIDDeviceControl::Release +");

    // Do nothing if backlights cannot be controlled.
    if (iSupportedLights != CHWRMLight::ENoTarget)
    {
        TRAP_IGNORE(iLight->LightOnL(iSupportedLights,
                                     KHWRMInfiniteDuration,
                                     KHWRMDefaultIntensity, EFalse));

        iLight->ReleaseLight(iSupportedLights);
    }

    DEBUG("nokialcdui::CMIDDeviceControl::Release -");
}

void CMIDDeviceControl::StaticRelease(CMIDDeviceControl* aDC)
{
    aDC->Release();
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::Dispose
// Deletes this object
// -----------------------------------------------------------------------------
//
void CMIDDeviceControl::Dispose()
{
    delete this;
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::LightStatusChanged
// If there is pending setLights call, do it now
// -----------------------------------------------------------------------------
//
void CMIDDeviceControl::LightStatusChanged(
    TInt aTarget,
    CHWRMLight::TLightStatus aStatus)
{
    DEBUG_INT2("nokialcdui::CMIDDeviceControl::LightStatusChanged( %d, %d ) +",
               aTarget, aStatus);
    (void)aTarget; // Just to suppress warning in release build

    // Do nothing if the backlights cannot be controlled.
    // Adjust the lights only if blinking has finished.
    // If the status informs about some error, the light devices have to be
    // released if blinking is not ongoing.
    if ((aStatus != CHWRMLight::ELightBlink) &&
            (iSupportedLights != CHWRMLight::ENoTarget))
    {
        // Check whether the blinking has stopped. Check that none of the light
        // devices is blinking.
        TBool isBlinking = LightsBlinking();

        // Adjust the lights only if blinking has finished.
        if (!isBlinking)
        {
            if (KErrNotFound == iLightLevel)
            {
                // lights haven't been set yet, so lights must be released after
                // blinking

                iLight->ReleaseLight(iSupportedLights);
            }
            // Set the stored level to the lights only if the lights are in a
            // valid state (if the given status informed about some error, the
            // level is not tried to be set to the lights).
            else if ((aStatus == CHWRMLight::ELightOn) ||
                     (aStatus == CHWRMLight::ELightOff))
            {
                TInt err;
                TRAP(err, DoSetLightsL(iLightLevel));

                DEBUG_INT(
                    "nokialcdui::CMIDDeviceControl::DoSetLightsL, err=%d", err);
            }
        }
    }

    DEBUG_INT2("nokialcdui::CMIDDeviceControl::LightStatusChanged( %d, %d ) -",
               aTarget, aStatus);
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::DoSetLightsL
// Actually sets the lights
// -----------------------------------------------------------------------------
//
void CMIDDeviceControl::DoSetLightsL(TInt aLevel)
{
    DEBUG_INT("nokialcdui::CMIDDeviceControl::DoSetLightsL( %d ) +", aLevel);

    iLight->ReserveLightL(iSupportedLights);

    // 0 means off in our API, but default in HWRMLight API.
    if (aLevel == 0)
    {
        iLight->LightOffL(iSupportedLights,
                          KHWRMInfiniteDuration,
                          EFalse);
    }
    else
    {
        iLight->LightOnL(iSupportedLights,
                         KHWRMInfiniteDuration,
                         aLevel, EFalse);
    }

    DEBUG_INT("nokialcdui::CMIDDeviceControl::DoSetLightsL( %d ) -", aLevel);
}

// -----------------------------------------------------------------------------
// CMIDDeviceControl::LightsBlinking
// Checks whether lights are blinking.
// -----------------------------------------------------------------------------
//
TBool CMIDDeviceControl::LightsBlinking()
{
    DEBUG("nokialcdui::CMIDDeviceControl::LightsBlinking +");

    // Check all the supported lights.

    TBool isBlinking = EFalse;

    for (TInt i = 0; i < KSupportedLightCount; i++)
    {
        TInt light = KSupportedLights[ i ];

        if ((iSupportedLights & light) == light)
        {
            CHWRMLight::TLightStatus lightStatus = iLight->LightStatus(light);
            TBool blinking = (lightStatus == CHWRMLight::ELightBlink);

            isBlinking |= blinking;
        }
    }


    DEBUG_INT("nokialcdui::CMIDDeviceControl::LightsBlinking - %d",
              isBlinking);

    return isBlinking;
}


//  End of File
