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


#ifndef SWTSCREEN_H
#define SWTSCREEN_H


#include "eswtmobileextensions.h"
#include "swtmobiledevice.h"


/**
 * CSwtScreen
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtScreen)
        : public CBase
        , public MSwtScreen
        , public MSwtResourceChangeObserver
        , public MSwtMobileDeviceObserver
{
// Types
private:
    enum TOrientationState
    {
        EOrientationNotSet,
        EOrientationPending,
        EOrientationSet,
        EOrientationRejected,
        EOrientationAccepted
    };

// Own methods
public:
    /**
     * Two phase constructor
     * @param aDisplay parent display
     * @param aPeer java peer of this class
     * @param aId this object's related hardware screen ordinal number
     * @return pointer to an object of this class
     */
    static CSwtScreen* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aId);

private:
    /**
     * Default constructor
     * @param aDisplay parent display
     * @param aPeer java peer of this class
     * @param aId this object's related hardware screen ordinal number
     */
    CSwtScreen(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aId);

    /**
     * Destructor
     */
    virtual ~CSwtScreen();

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * The only way to check if the related hardware screen is on. At the
     * moment of implementation the framework can have a single screen on
     * at a time. Querying information about the hardware screen should be
     * done only when the related hardware screen is on otherwise the results
     * will be inconsistent.
     * @return ETrue if related harware screen on
     */
    TBool IsScreenOn() const;

    /**
     * Set iOrientation to the AppUi.
     */
    void DoSetOrientationL();

    /**
     * Reset iOrientation to the AppUi.
     */
    void DoRefreshOrientationL();

    /**
     * Get the orientation of the application at a given moment.
     */
    TInt CurrentOrientation();

// From MSwtScreen
public:
    TSwtPeer Dispose();
    TRect GetBounds();
    TInt GetColorDepth();
    TInt GetLocation();
    TInt GetOrientation();
    TInt GetUsage();
    TBool IsActive();
    TBool IsTouchScreen();
    void SetOrientationL(TInt aOrientation);

// From MSwtResourceChangeObserver
public:
    void OfferResourceChangeL(TInt aType);

// From MSwtMobileDeviceObserver
public:
    void ScreenActivatedL(TInt aScreenId);
    void ScreenDeactivatedL(TInt aScreenId);

// Data
private:
    /**
     * Related display.
     */
    MSwtDisplay& iDisplay;

    /**
     * Java peer.
     */
    const TSwtPeer iPeer;

    /**
     * Related hardware screen device identifier.
     */
    TInt iId;

    /**
     * Holds the previous orientation of this screen instance in the case
     * where the user has not set a custom orientation or otherwise the
     * user's custom orientation.
     */
    TInt iOrientation;

    /**
     * State of the orientation setting.
     */
    TOrientationState iOrientationState;

    /**
     * ETrue if activated event received.
     */
    TBool iActivationState;

    /**
     * ETrue if the device has only one screen.
     */
    TBool iIsTheOnlyScreen;

    /**
     * Pointer to the one and only software screen device.
     */
    CWsScreenDevice* iScreenDevice;
};


#endif // SWTSCREEN_H
