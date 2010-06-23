/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/


#include "swtmobiledevice.h"

using namespace Java::eSWT;

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
// ---------------------------------------------------------------------------
// CSwtFlipWatch::NewL
// ---------------------------------------------------------------------------
//
CSwtMobileDevice* CSwtMobileDevice::NewL()
    {
    CSwtMobileDevice* self = new (ELeave) CSwtMobileDevice();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CSwtMobileDevice::CSwtMobileDevice
// ---------------------------------------------------------------------------
//
CSwtMobileDevice::CSwtMobileDevice()
    : iVibra(NULL),
      iHWRMLight(NULL),
      iSupportedLights(CHWRMLight::ENoTarget),
      iLightLevel(KErrNotFound)
    {
    }

// ---------------------------------------------------------------------------
// CSwtMobileDevice::~CSwtMobileDevice
// ---------------------------------------------------------------------------
//
CSwtMobileDevice::~CSwtMobileDevice()
    {
    if (iVibra)
        {
        delete iVibra;
        iVibra = NULL;
        }

    if (iHWRMLight)
        {
        // Do nothing if backlights cannot be controlled.
        if (iSupportedLights != CHWRMLight::ENoTarget)
            {
            TRAP_IGNORE(iHWRMLight->LightOnL(iSupportedLights,
                    KHWRMInfiniteDuration,
                    KHWRMDefaultIntensity, EFalse));

            iHWRMLight->ReleaseLight(iSupportedLights);
            }
        delete iHWRMLight;
        iHWRMLight = NULL;
        }

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
    }

// ---------------------------------------------------------------------------
// CSwtMobileDevice::CreateFlipWatchL
// ---------------------------------------------------------------------------
//
void CSwtMobileDevice::CreateFlipWatchL()
    {
    iFlipWatch = CSwtFlipWatch::NewL();
    }

// ---------------------------------------------------------------------------
// CSwtFlipWatch::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtMobileDevice::CreateLightsL()
    {
    iHWRMLight = CHWRMLight::NewL(this);

    // Find out light devices on the phone that are supported
    // by our API.
    TInt supportedLights = iHWRMLight->SupportedTargets();
    TInt mask = (CHWRMLight::EPrimaryDisplay | CHWRMLight::EPrimaryKeyboard);
    iSupportedLights = (supportedLights & mask);
    }

// -----------------------------------------------------------------------------
// CSwtMobileDevice::resetUserInactivityTime
// Reset the user inactivity time
// -----------------------------------------------------------------------------
//
TInt CSwtMobileDevice::GetUserInactivityTime()
    {
    TInt time = 0;
    TTimeIntervalSeconds ValInterval = User::InactivityTime();
    time = ValInterval.Int();
    return time;
    }

// -----------------------------------------------------------------------------
// CSwtMobileDevice::resetUserInactivityTime
// Reset the user inactivity time
// -----------------------------------------------------------------------------
//
void CSwtMobileDevice::ResetUserInactivityTime()
    {
    User::ResetInactivityTime();
    }

// ---------------------------------------------------------------------------
// CSwtMobileDevice::SetLights
// ---------------------------------------------------------------------------
//
void CSwtMobileDevice::SetLights(TInt aLevel)
    {
    if ((aLevel < 0) || (aLevel > KMIDMaxLightLevel))
        {
        return;
        }

    if (!iHWRMLight)
        {
        TRAP_IGNORE(CreateLightsL());
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
            TRAP_IGNORE(DoSetLightsL(aLevel));
            }
        }
    }

// ---------------------------------------------------------------------------
// CSwtMobileDevice::FlashBacklightL
// ---------------------------------------------------------------------------
//
#if defined(__WINSCW__)

TBool CSwtMobileDevice::FlashBacklightL(TInt /*aDuration*/)
    {
    return EFalse;
    }
#else // __WINSCW__
TBool CSwtMobileDevice::FlashBacklightL(TInt aDuration)
    {
    if (!iHWRMLight)
        {
        CreateLightsL();
        }

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
        iHWRMLight->ReserveLightL(iSupportedLights);
        }

    if (!aDuration)
        {
        // 0 would be infinite
        aDuration = 1;
        }

    iDuration = aDuration;
    iStartTime = new TTime();
    iStartTime->HomeTime();
    iHWRMLight->LightBlinkL(iSupportedLights, aDuration);
    return ETrue;
    }

#endif // __WINSCW__

// ---------------------------------------------------------------------------
// CSwtMobileDevice::Vibrate
// ---------------------------------------------------------------------------
//
#if defined(__WINSCW__)
TBool CSwtMobileDevice::Vibrate(const TTimeIntervalMicroSeconds32& /*aDuration*/)
    {
    return EFalse;
    }
#else // __WINSCW__
TBool CSwtMobileDevice::Vibrate(const TTimeIntervalMicroSeconds32& aDuration)
    {
    TInt duration = aDuration.Int() / 1000; // convert micro to milli
    if (!iVibra)
        {
        TRAPD(err1, iVibra = CHWRMVibra::NewL());
        if (err1 != KErrNone)
            {
            return EFalse;
            }
        }
 
    TInt err2 = KErrNone;
    if (duration)
        {
        TRAP(err2, iVibra->StartVibraL(duration));
        }
    else
        {
        TRAP(err2, iVibra->StopVibraL());
        }

    if (err2 == KErrNone)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
#endif // __WINSCW__

// -----------------------------------------------------------------------------
// CSwtMobileDevice::AppForegroundL
// -----------------------------------------------------------------------------
//
void CSwtMobileDevice::AppForeground()
    {
    if (iDuration > 0)
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
            TRAP_IGNORE(FlashBacklightL(iDuration - nowDuration));
            }
        }
    }

// -----------------------------------------------------------------------------
// CSwtMobileDevice::LightStatusChanged
// If there is pending setLights call, do it now
// -----------------------------------------------------------------------------
//
void CSwtMobileDevice::LightStatusChanged(TInt aTarget,CHWRMLight::TLightStatus aStatus)
    {
    (void) aTarget; // Just to suppress warning in release build

    // Do nothing if the backlights cannot be controlled.
    // Adjust the lights only if blinking has finished.
    // If the status informs about some error, the light devices have to be
    // released if blinking is not ongoing.
    if (iHWRMLight && (aStatus != CHWRMLight::ELightBlink) &&
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
                iHWRMLight->ReleaseLight(iSupportedLights);
                }

            // Set the stored level to the lights only if the lights are in a
            // valid state (if the given status informed about some error, the
            // level is not tried to be set to the lights).
            else if ((aStatus == CHWRMLight::ELightOn) || (aStatus == CHWRMLight::ELightOff))
                {
                TInt err;
                TRAP(err, DoSetLightsL(iLightLevel));
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CSwtMobileDevice::DoSetLightsL
// Actually sets the lights
// -----------------------------------------------------------------------------
//
void CSwtMobileDevice::DoSetLightsL(TInt aLevel)
    {
    iHWRMLight->ReserveLightL(iSupportedLights);

    // 0 means off in our API, but default in HWRMLight API.
    if (aLevel == 0)
        {
        iHWRMLight->LightOffL(iSupportedLights, KHWRMInfiniteDuration, EFalse);
        }
    else
        {
        iHWRMLight->LightOnL(iSupportedLights, KHWRMInfiniteDuration, aLevel,
                EFalse);
        }
    }

// -----------------------------------------------------------------------------
// CSwtMobileDevice::LightsBlinking
// Checks whether lights are blinking.
// -----------------------------------------------------------------------------
//
TBool CSwtMobileDevice::LightsBlinking()
    {
    // Check all the supported lights.
    TBool isBlinking = EFalse;
    for (TInt i = 0; i < KSupportedLightCount; i++)
        {
        TInt light = KSupportedLights[i];
        if ((iSupportedLights & light) == light)
            {
            CHWRMLight::TLightStatus lightStatus = iHWRMLight->LightStatus(
                    light);
            TBool blinking = (lightStatus == CHWRMLight::ELightBlink);
            isBlinking |= blinking;
            }
        }
    return isBlinking;
    }


