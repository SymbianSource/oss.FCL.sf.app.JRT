/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for playing MIDI.
*
*/


#ifndef CMMAMIDIPLAYER_H
#define CMMAMIDIPLAYER_H


//  INCLUDES
#include "cmmaplayer.h"
#include <midiclientutility.h>
#include <e32base.h>

// CONSTANTS
_LIT(KMMAMIDIPlayer, "MIDIPlayer");

const TMdaPriorityPreference KMMAMIDIPriorityPreference =
    EMdaPriorityPreferenceTimeAndQuality;

// FORWARD DECLARATIONS
class CMMAEventSource;

//  CLASS DECLARATION
/**
*   This class is used for playing MIDI.
*
*
*/

class CMMAMIDIPlayer : public CMMAPlayer, public MMidiClientUtilityObserver
{
public: // Construction
    static CMMAMIDIPlayer* NewLC(const TDesC& aContentType,
                                 TFileName aFileName);

    //   Destructor
    ~CMMAMIDIPlayer();

protected:
    //   C++ constructor
    CMMAMIDIPlayer(TFileName aFileName);
    void ConstructL(const TDesC& aContentType);

public: // new methods
    IMPORT_C CMidiClientUtility* MidiClient() const;
    void ReInitializeMidiEngineL(const TDesC8* aMidiSequence);
    void addObserverL(MMidiClientUtilityObserver* aObserver);

public: // from CMMAPlayer
    void StartL(TBool aPostEvent);
    void StopL(TBool aPostEvent);
    void RealizeL();
    void PrefetchL();
    void DeallocateL();
    void GetDuration(TInt64* aDuration);
    void SetMediaTimeL(TInt64* aTime);
    void GetMediaTime(TInt64* aMediaTime);
    void CloseL();
    const TDesC& Type();

public: // from CMMAPlayer/MMMASourceStreamListener
    void ReadCompletedL(TInt aStatus, const TDesC8& aData);
    void PlayCompleteL(TInt aError);

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

protected:
    void CloseClientUtility();

private: // Data
    CMidiClientUtility* iMidi;

    // CActiveShedulerWait object for reinitializing midi engine
    CActiveSchedulerWait* iActiveSchedulerWait;

    // Array for subsequent MMidiClientUtilityObservers
    RPointerArray<MMidiClientUtilityObserver> iObservers;

    TFileName iFileName;

    /**
    * Cached media time
    */
    TInt64 iMediaTime;

    /**
    * The time that will be sent with CMMAPlayerEvent::EStarted
    * (may be different from iMediaTime).
    */
    TInt64 iStartedEventTime;
};

#endif // CMMAMIDIPLAYER_H
