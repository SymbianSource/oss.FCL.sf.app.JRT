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
* Description:  Manipulates the doppler of the spectator.
*
*/


#ifndef CAMMSEMCSPECTATORDOPPLERCONTROL_H
#define CAMMSEMCSPECTATORDOPPLERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include<ListenerDopplerControl.h>
#include <DopplerData.h>
#include "cammsdopplercontrol.h"
#include <cmmaemcaudioplayer.h>  // from MMAPI

using multimedia :: MListenerDopplerControl;
using multimedia :: KListenerDopplerEffectControl;

// CONSTANTS
_LIT(KAMMSEMCListenerDopplerControl, "DopplerControl");

// CLASS DECLARATION

/**
*
*  Controls for the Doppler Control effect for the Spectator.
*  This class delegates Doppler Control effect method calls to
*  MListenerDopplerControl.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSEMCSpectatorDopplerControl): public CAMMSDopplerControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSEMCSpectatorDopplerControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSEMCSpectatorDopplerControl();

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
    CAMMSEMCSpectatorDopplerControl(CMMAPlayer* aPlayer);

public:

    void SetEnabledL(TBool aDopplerEnabled);

    TBool Enabled();

    void SetVelocityCartesianL(TInt aX, TInt aY, TInt aZ);

    void VelocityCartesian(TInt& aX, TInt& aY, TInt& aZ);

    void SetVelocitySphericalL(TInt aAzimuth, TInt aElevation,
                               TInt aRadius);

private:

    CMMAPlayer *iMMAPlayer;
    /**
    * EMC Spectator Doppler Control
    */

    CMultimediaFactory* iFactory;
    MStreamControl* iStreamControl;
    MListenerDopplerControl *iMListenerDopplerControl;
};

#endif // CAMMSEMCSPECTATORDOPPLERCONTROL_H


