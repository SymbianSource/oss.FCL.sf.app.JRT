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
* Description:  Controls display backlight
*
*/



#ifndef CMIDDEVICECONTROL_H
#define CMIDDEVICECONTROL_H

//  INCLUDES
#include <e32std.h>
#include <lcdui.h>
#include <hwrmlight.h>

// CLASS DECLARATION

/**
*  Controls display backlight.
*  With this class it is possible to set the backlight level.
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CMIDDeviceControl): public CBase,
        public MMIDComponent, public MHWRMLightObserver, public MMIDEnvObserver
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * Leaves object in cleanup stack.
    */
    static CMIDDeviceControl* NewLC(MMIDEnv& aEnv);

    /**
    * Destructor.
    */
    virtual ~CMIDDeviceControl();

public: // New functions

    /**
    * Gets the time since the last user activity.
    * @return returns user inactivity time in seconds
    * @since 9.1
    */
    void GetUserInactivityTimeL(TInt* aRetVal);
    static void StaticGetUserInactivityTimeL(CMIDDeviceControl* aDC,
            TInt* aRetVal);


    /**
    * Resets all user inactivity timers.
    * Can be used to prevent e.g. screen saver to appear on screen.
    * @since 9.1
    */
    void ResetUserInactivityTimeL();
    static void StaticResetUserInactivityTimeL(CMIDDeviceControl* aDC);

    /**
    * Sets the backlight level.
    * @since 3.0
    * @param aLevel Backlight level, 0-100, 0 = off.
    */
    void SetLightL(TInt aLevel);
    static void StaticSetLightL(CMIDDeviceControl* aDC, TInt aLevel);

    /**
     * Flashes the backlight for given duration
     * @since 3.1
     * @param aDuration Duration of flashing, in milliseconds
     * @return ETrue if flashing is possible
     */
    TBool FlashBacklightL(TInt aDuration);
    static void StaticFlashBacklightL(
        CMIDDeviceControl* aDC,
        TInt aDuration,
        TBool* aRetVal);

    /**
     * Returns the backlight to sensor mode, and releases resources.
     * @since 3.0
     */
    void Release();
    static void StaticRelease(CMIDDeviceControl* aDC);

public: // From MMIDComponent
    void Dispose();

public: // From MHWRMLightObserver
    void LightStatusChanged(TInt aTarget,
                            CHWRMLight::TLightStatus aStatus);


    // From MMIDEnvObserver
    void HandleSwitchOnL(TBool aSwitchOn);
    void HandleForegroundL(TBool aForeground);
    void HandleResourceChangeL(TInt aType);


private:

    /**
    * C++ default constructor.
    */
    CMIDDeviceControl(MMIDEnv& aEnv);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

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

private:    // Data

    MMIDEnv& iEnv;

    // timestamp of start of flashing
    TTime* iStartTime;

    // remaining duration for flashing
    TInt iDuration;


    // Light Client API, owned
    CHWRMLight* iLight;

    // value of last setLights() call,
    // KErrNotFound if setLights() hasn't been called yet
    TInt iLightLevel;

    // Bitmask having information which ones of the backlights
    // are supported (primary/secondary displays and primary/secondary
    // keyboards).
    TInt iSupportedLights;

};

#endif // CMIDDEVICECONTROL_H

// End of File
