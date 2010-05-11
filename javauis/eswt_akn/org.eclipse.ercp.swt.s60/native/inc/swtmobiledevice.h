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


#ifndef SWTMOBILEDEVICE_H
#define SWTMOBILEDEVICE_H


#include <aknappui.h>
#include "eswtmobileextensions.h"
#include "swtflipwatch.h"


/**
 * CSwtMobileDevice
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtMobileDevice)
        : public CBase
        , public MSwtMobileDevice
        , public MSwtFlipObserver
{
// Types
public:
    /** Input info */
    struct TSwtHwInput
    {
        TInt iType;
        TInt iLocation;
    };

// Own Methods
public:
    static CSwtMobileDevice* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer);
    static TInt HwScreenCount();
    static void GetHwInputs(RArray<TSwtHwInput>& aInputs);
private:
    inline CSwtMobileDevice(MSwtDisplay& aDisplay, TSwtPeer aPeer);
    virtual ~CSwtMobileDevice();
    void ConstructL();

// From MSwtMobileDevice
public:
    void Alert(TAlertLevel aLevel);
    TSwtPeer Dispose();
    TInt GetInputNumber() const;
    TInt GetScreenNumber() const;
    TBool IsOpen() const;
    TBool AddObserverL(const MSwtMobileDeviceObserver* aObserver);
    TBool RemoveObserver(const MSwtMobileDeviceObserver* aObserver);

// From MSwtFlipObserver
public:
    void FlipChangingL();
    void FlipChangedL(TBool aState);

// Data
private:
    MSwtDisplay& iDisplay;
    const TSwtPeer iPeer;
    TBool iFlipOpen;
    TBool iFlipChanging;
    TInt iScreenCount;
    TInt iActiveScreenId;
    RPointerArray<MSwtMobileDeviceObserver> iObservers;
    CSwtFlipWatch* iFlipWatch;
};


#endif // SWTMOBILEDEVICE_H
