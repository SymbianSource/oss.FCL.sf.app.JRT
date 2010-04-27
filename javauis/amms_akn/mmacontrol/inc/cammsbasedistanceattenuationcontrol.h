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
* Description:  Controls how the sound is attenuated with its distance.
*
*/


#ifndef CAMMSBASEDISTANCEATTENUATIONCONTROL_H
#define CAMMSBASEDISTANCEATTENUATIONCONTROL_H

//  INCLUDES
#include <e32base.h>
#include "cammscontrol.h"

// CONSTANTS
_LIT(KAMMSBaseDistanceAttenuationControl, "DistanceAttenuationControl");

// CLASS DECLARATION

/**
*
*  Base class for the Distance Attenuation effect.
*  This class delegates Distance Attenuation effect method calls to
*  to corresponding control.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSBaseDistanceAttenuationControl): public CAMMSControl
{
public:  //destructor
    /**
    * Destructor.
    */
    ~CAMMSBaseDistanceAttenuationControl();

public: // New functions

    virtual void SetParametersL(
        TInt/* aMinDistance*/,
        TInt /*aMaxDistance*/,
        TBool /*aMuteAfterMax*/,
        TInt /*aRolloffFactor*/) = 0;

public:
    const TDesC& ClassName() const  = 0;

protected:
    /**
    * C++ default constructor, protected to allow access from derived class
    * @param aPlayer Player that has this control.
    */
    CAMMSBaseDistanceAttenuationControl(CMMAPlayer* aPlayer);

};

#endif // CAMMSBASEDISTANCEATTENUATIONCONTROL_H


