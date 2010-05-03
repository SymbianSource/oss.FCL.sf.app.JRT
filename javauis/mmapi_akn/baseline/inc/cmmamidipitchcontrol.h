/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements PitchControl functionality.
*
*/


#ifndef CMMAMIDIPITCHCONTROL_H
#define CMMAMIDIPITCHCONTROL_H

// EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "cmmacontrol.h" // base class

// CONSTANTS

// Constant for control name. Name is used in Java side to instantiate
// Java Class which uses CMMAMIDIPitchControl.
_LIT(KMIDIPitchControlName, "PitchControl");

// Maximum and minimun pitch values
const TInt KMIDIPitchControlMaxPitch = 2400;
const TInt KMIDIPitchControlMinPitch = -2400;


class CMMAMIDIPlayer;

//  CLASS DECLARATION
/**
*   This class implements MIDIPitchControl interface.
*
*
*/
NONSHARABLE_CLASS(CMMAMIDIPitchControl): public CMMAControl
{
public:
    /**
     * Creates new CMMAMIDIPitchControl.
     *
     * @param aPlayer Player that plays the content.
     */
    static CMMAMIDIPitchControl* NewL(CMMAMIDIPlayer* aPlayer);

    /**
     * Destructor.
     */
    ~CMMAMIDIPitchControl();
protected:
    /**
     * Constructor.
     * @param aPlayer Player that plays the content.
     */
    CMMAMIDIPitchControl(CMMAMIDIPlayer* aPlayer);

public: // From CMMAControl
    const TDesC& ClassName() const;

public: // New methods
    /**
     * @return Pitch, if not available the default.
     */
    TInt PitchL();

    /**
     * @param aPitch Pitch to set in milli-beats per minute.
     * @return Actual Pitch set.
     */
    TInt SetPitchL(TInt aPitch);

    /**
     * @return The maximum rate supported.
     */
    TInt MaxPitchL();

    /**
     * @return The minimum rate supported.
     */
    TInt MinPitchL();

private: // Data
    /**
     * Used to control MIDI playback.
     */
    CMMAMIDIPlayer* iPlayer;
};


#endif // CMMAMIDIPITCHCONTROL_H
