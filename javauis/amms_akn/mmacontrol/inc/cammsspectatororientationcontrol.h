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
* Description:  Manipulates the virtual orientation of the spectator.
*
*/


#ifndef CAMMSSPECTATORORIENTATIONCONTROL_H
#define CAMMSSPECTATORORIENTATIONCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <ListenerOrientationBase.h>
#include <CustomCommandUtility.h>
#include "cammsorientationcontrol.h"

// CONSTANTS
_LIT(KAMMSSpectatorOrientationControl, "OrientationControl");

// CLASS DECLARATION

/**
*
*  Controls for the Orientation Control effect for the Spectator.
*  This class delegates Orientation Control effect method calls to
*  CListenerOrientation.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSSpectatorOrientationControl): public CAMMSOrientationControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSSpectatorOrientationControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSSpectatorOrientationControl();

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
    CAMMSSpectatorOrientationControl(CMMAPlayer* aPlayer);

};

#endif // CAMMSSPECTATORORIENTATIONCONTROL_H


