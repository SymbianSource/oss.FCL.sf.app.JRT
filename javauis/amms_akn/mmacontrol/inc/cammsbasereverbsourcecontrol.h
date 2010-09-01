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
* Description:  Class behaves like interface of an audio effect reverb source.
*
*/


#ifndef CAMMSBASEREVERBSOURCECONTROL_H
#define CAMMSBASEREVERBSOURCECONTROL_H

//  INCLUDES
#include <e32base.h>
#include "cammscontrol.h"

// CONSTANTS
_LIT(KAMMSBaseReverbSourceControl, "ReverbSourceControl");


// CLASS DECLARATION

/**
*
*  Controls for the Reverb Source effect.
*  This class delegates Base class of Reverb Source effect method .
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSBaseReverbSourceControl): public CAMMSControl
{
public:

    /**
    * Destructor.
    */
    ~CAMMSBaseReverbSourceControl();

public:
    /**
    * Sets the object specific level for the reverberant sound.
    * The default value is 0 meaning the natural room gain (set by
    * ReverbControl's presets).
    *
    * @param aLevel The new level of the reverberation in millibels.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aLevel is greater than 0
    */
    virtual void SetRoomLevelL(TInt aLevel) = 0;

public:

    const TDesC& ClassName() const = 0;

protected:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    * @param aReverbControl Reverb control belonging to aPlayer.
    */
    CAMMSBaseReverbSourceControl(CMMAPlayer* aPlayer);

};

#endif // CAMMSBASEREVERBSOURCECONTROL_H


