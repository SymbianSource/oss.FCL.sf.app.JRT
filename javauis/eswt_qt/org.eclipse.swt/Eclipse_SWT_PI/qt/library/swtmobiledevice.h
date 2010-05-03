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

#ifndef SWTMOBILEDEVICE_H
#define SWTMOBILEDEVICE_H

#include <hwrmlight.h> 
#include <hwrmvibra.h> 
#include "swtflipwatch.h"


namespace Java { namespace eSWT {

/**
 * CSwtMobileDevice
 */
class CSwtMobileDevice
: public CBase, MHWRMLightObserver
{
  
public:
    static CSwtMobileDevice* NewL();
    virtual ~CSwtMobileDevice();
 
private:
    CSwtMobileDevice();
    void ConstructL();

public:
    void CreateFlipWatchL();
    /**
     * Gets the time since the last user activity.
     * @return returns user inactivity time in seconds
     * @since 9.1
     */
    TInt GetUserInactivityTime();
    /**
     * Resets all user inactivity timers.
     * Can be used to prevent e.g. screen saver to appear on screen.
     * @since 9.1
     */
    void ResetUserInactivityTime(); 
    /**
     * Sets the backlight level.
     * @since 3.0
     * @param aLevel Backlight level, 0-100, 0 = off.
     */
    void SetLights(TInt aLevel);
    /**
     * Flashes the backlight for given duration
     * @since 3.1
     * @param aDuration Duration of flashing, in milliseconds
     * @return ETrue if flashing is possible
     */
    TBool FlashBacklightL(TInt aDuration);

    /**
     * There is no API for vibrate. Left to be customised according to each device
     * Printing InfoMsg for the user for the duration given to vibrate
     * Must return true if the vibrator can be controlled by the application
     */
    TBool Vibrate(const TTimeIntervalMicroSeconds32& aDuration);

    void AppForeground();
public: // From MHWRMLightObserver
    void LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus);
private:
   /**
    * By default Symbian 2nd phase constructor is private.
    */
    void CreateLightsL();
    /**
     * Does the actual setting of lights
     */
    void DoSetLightsL(TInt aLevel);
    /**
     * Checks whether lights are blinking.
     * @return ETrue if at least one supported light is blinking; else
     * EFalse is returned.
     */
    TBool LightsBlinking();    
// Data
private:
    CHWRMVibra* iVibra;
    CHWRMLight* iHWRMLight;
    CSwtFlipWatch* iFlipWatch;
    TInt iSupportedLights;

    // value of last setLights() call,
    // KErrNotFound if setLights() hasn't been called yet
    TInt iLightLevel;
    // timestamp of start of flashing
    TTime* iStartTime;
    // remaining duration for flashing
    TInt iDuration;
};
}}
#endif // SWTMOBILEDEVICE_H
