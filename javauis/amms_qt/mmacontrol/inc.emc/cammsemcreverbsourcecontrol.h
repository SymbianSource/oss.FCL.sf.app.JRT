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
* Description:  Manipulates the settings of an audio effect reverb source.
*
*/


#ifndef CAMMSEMCREVERBSOURCECONTROL_H
#define CAMMSEMCREVERBSOURCECONTROL_H

//  INCLUDES
#include <e32base.h>
#include <RoomLevelControl.h>
#include "cammsbasereverbsourcecontrol.h"
#include <cmmaemcaudioplayer.h>
// CONSTANTS
_LIT(KAMMSEMCReverbSourceControl, "ReverbSourceControl");

using multimedia::MRoomLevelControl;
using multimedia::KRoomLevelEffectControl;

// FORWARD DECLARATIONS
class CAMMSBaseReverbControl;

// CLASS DECLARATION

/**
*
*  Controls for the Reverb Source effect.
*  This class delegates Reverb Source effect method calls to CRoomLevel.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSEMCReverbSourceControl): public CAMMSBaseReverbSourceControl  /*CAMMSControl*/
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    * @param aReverbControl Reverb control belonging to aPlayer.
    */
    static CAMMSEMCReverbSourceControl* NewLC(CMMAPlayer* aPlayer,
    CAMMSBaseReverbControl* aBaseReverbControl);

    /**
    * Destructor.
    */
    ~CAMMSEMCReverbSourceControl();

public:  // New functions

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
    void SetRoomLevelL(TInt aLevel);

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
    * @param aReverbControl Reverb control belonging to aPlayer.
    */
    CAMMSEMCReverbSourceControl(CMMAPlayer* aPlayer,
                                CAMMSBaseReverbControl* aReverbControl);

private:  // Data

    /** Reverb control belonging to the player */
    CAMMSBaseReverbControl* iBaseReverbControl;

private:

    CMMAPlayer *iMMAPlayer;
    /**
    * EMC ReverbSource Control
    */

    CMultimediaFactory* iFactory;
    MStreamControl* iStreamControl;
    MRoomLevelControl* iRoomLevelControl;
};

#endif // CAMMSEMCREVERBSOURCECONTROL_H


