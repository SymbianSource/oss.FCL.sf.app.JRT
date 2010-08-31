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
* Description:  Manipulates the virtual location of the spectator.
*
*/


#ifndef CAMMSSPECTATORLOCATIONCONTROL_H
#define CAMMSSPECTATORLOCATIONCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <ListenerLocationBase.h>
#include <CustomCommandUtility.h>
#include "cammslocationcontrol.h"

// CONSTANTS
_LIT(KAMMSSpectatorLocationControl, "LocationControl");

// CLASS DECLARATION

/**
*
*  Controls for the Location Control effect for the Spectator.
*  This class delegates Location Control effect method calls to
*  CListenerLocation.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSSpectatorLocationControl): public CAMMSLocationControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSSpectatorLocationControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSSpectatorLocationControl();

public: // Functions from base classes
    const TDesC& ClassName() const;

    /**
    * Prepares the Control.
    */
    void PrepareControlL();

    /**
    * Deallocates the Control.
    */
    void DeallocateControl();

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSSpectatorLocationControl(CMMAPlayer* aPlayer);

};

#endif // CAMMSSPECTATORLOCATIONCONTROL_H


