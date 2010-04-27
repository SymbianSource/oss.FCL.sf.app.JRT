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


#ifndef CAMMSAUDIODOPPLERCONTROL_H
#define CAMMSAUDIODOPPLERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <SourceDopplerBase.h>
#include <CustomCommandUtility.h>
#include "cammsdopplercontrol.h"

// CONSTANTS
_LIT(KAMMSAudioDopplerControl, "DopplerControl");

// CLASS DECLARATION

/**
*
*  Controls for the Doppler Control effect for the Audio source.
*  This class delegates Doppler Control effect method calls to
*  CSourceDoppler.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSAudioDopplerControl): public CAMMSDopplerControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    *
    */
    static CAMMSAudioDopplerControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSAudioDopplerControl();

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
    CAMMSAudioDopplerControl(CMMAPlayer* aPlayer);
};

#endif // CAMMSAUDIODOPPLERCONTROL_H
