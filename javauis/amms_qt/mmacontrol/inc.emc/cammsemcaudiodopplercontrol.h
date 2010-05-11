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
* Description:  Manipulates the settings of an effect called Doppler.
*
*/


#ifndef CAMMSEMCAUDIODOPPLERCONTROL_H
#define CAMMSEMCAUDIODOPPLERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include<SourceDopplerControl.h>
#include <DopplerData.h>
#include "cammsdopplercontrol.h"

#include <cmmaemcaudioplayer.h>

using multimedia :: MSourceDopplerControl;
using multimedia :: KSourceDopplerEffectControl;


// CONSTANTS
_LIT(KAMMSEMCAudioDopplerControl, "DopplerControl");

// CLASS DECLARATION

/**
*
*  Controls for the Doppler Control effect for the Audio source.
*  This class delegates Doppler Control effect method calls to
*  MEffectControl.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSEMCAudioDopplerControl): public CAMMSDopplerControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    *
    */
    static CAMMSEMCAudioDopplerControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSEMCAudioDopplerControl();

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
    CAMMSEMCAudioDopplerControl(CMMAPlayer* aPlayer);


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
    * EMC SourceDoppler Control
    */
    CMultimediaFactory* iFactory;
    MStreamControl* iStreamControl;
    MSourceDopplerControl *iMSourceDopplerControl;
};

#endif // CAMMSEMCAUDIODOPPLERCONTROL_H
