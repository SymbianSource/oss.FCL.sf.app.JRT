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
* Description:  This class implements MIDITempoControl functionality.
*
*/


#ifndef CMMAMIDITEMPOCONTROL_H
#define CMMAMIDITEMPOCONTROL_H

// EXTERNAL INCLUDES
#include <midiclientutility.h> // MMidiClientUtilityObserver

//  INTERNAL INCLUDES
#include "cmmaratecontrol.h" // base class
#include "mmmaplayerstatelistener.h"

// CONSTANTS

// Constant for control name. Name is used in Java side to instantiate
// Java Class which uses CMMAMIDITempoControl.
_LIT(KMIDITempoControlName, "TempoControl");

class CMMAMIDIPlayer;

//  CLASS DECLARATION
/**
*   This class implements MIDITempoControl interface.
*
*
*/
NONSHARABLE_CLASS(CMMAMIDITempoControl): public CMMARateControl,
        public MMidiClientUtilityObserver, public MMMAPlayerStateListener
{
public:
    /**
     * Creates new CMMAMIDITempoControl.
     *
     * @param aPlayer Player that plays the content.
     */
    static CMMAMIDITempoControl* NewL(CMMAMIDIPlayer* aPlayer);

    /**
     * Destructor.
     */
    ~CMMAMIDITempoControl();
protected:
    /**
     * Constructor.
     * @param aPlayer Player that plays the content.
     */
    CMMAMIDITempoControl(CMMAMIDIPlayer* aPlayer);

    /**
     * Initializes this control.
     */
    void ConstructL();

public: // From CMMAControl
    const TDesC& ClassName() const;

public: // From CMMARateControl
    /**
     * @param aRate Rate to set in "milli-percentage"..
     * @return Actual rate set.
     */
    TInt SetRateL(TInt aRate);

    /**
     * @return The current playback rate in "milli-percentage".
     */
    TInt RateL();

    /**
     * @return The maximum rate supported.
     */
    TInt MaxRateL();

    /**
     * @return The minimum rate supported.
     */
    TInt MinRateL();

public: // New methods
    /**
     * @return Tempo, if not available the default.
     */
    TInt TempoL();

    /**
     * @param aTempo Tempo to set in milli-beats per minute.
     * @return Actual Tempo set.
     */
    TInt SetTempoL(TInt aTempo);

public:   // From MMMAPlayerStateListener
    void StateChanged(TInt aState);

public: // from MMidiClientUtilityObserver
    void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
    void MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
    void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels);
    void MmcuoMuteChanged(TInt aChannel,TBool aMuted);
    void MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats);
    void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition);
    void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aMessage);
    void MmcuoPolyphonyChanged(TInt aNewPolyphony);
    void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId);

private: // Data
    /**
     * Used to control MIDI playback.
     */
    CMMAMIDIPlayer* iPlayer;

    /**
     * Current tempo. Unit of the variable is milli-beat.
     * Tempo cannot be set to the midi client before the player is
     * prefetched. Thus, when the player is in REALIZED state, tempo is
     * saved to this variable and set to the client when the player is
     * prefetched.
     */
    TInt iTempo;

};


#endif // CMMAMIDITEMPOCONTROL_H
