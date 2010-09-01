/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manipulates the panning of a Player in the stereo output mix.
*
*/


#ifndef CAMMSPANCONTROL_H
#define CAMMSPANCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <midiclientutility.h>
#include "CAMMSControl.h"

// CONSTANTS
_LIT(KAMMSPanControl, "PanControl");

// CLASS DECLARATION

/**
*
*  Controls for the Panning effect.
*  This class delegates Pan effect method calls to CBalance.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSPanControl): public CAMMSControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSPanControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSPanControl();

public: // New functions

    /**
    * Gets the current panning set.
    *
    * @return The current balance or panning setting.
    */
    TInt PanL();

    /**
    * Sets the panning using a linear point scale with values between -100
    * and 100. 0 represents panning for both channels, -100 full panning to
    * the left and 100 full panning to the right. If the given panning
    * value is less than -100 or greater than 100, the panning will be set
    * to -100 or 100, respectively.
    *
    * @param aPan The new panning to be set.
    *
    * @return The panning that was actually set.
    */
    TInt SetPanL(TInt aPan);

public: // Functions from base classes
    const TDesC& ClassName() const;

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSPanControl(CMMAPlayer* aPlayer);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();


private:  // Data

    /** Client class to access Audio Play Device functionality, owned */
    RMMFAudioPlayDeviceCustomCommands* iRMMFAudioPlayDeviceCustomCommands;

    /** CMidiClientUtility, not owned. */
    CMidiClientUtility* iMidiClientUtility;
};

#endif // CAMMSPANCONTROL_H


