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
* Description:  This class is a MIDIControl.
*
*/


#ifndef CMMAMIDICONTROL_H
#define CMMAMIDICONTROL_H

// EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "cmmacontrol.h" // base class
#include "mmmaplayerstatelistener.h"
#include <midiclientutility.h> // MMidiClientUtilityObserver
#include <TimeOutTimer.h>

// CONSTANTS

// Constant for control name. Name is used in Java side to instantiate
// Java Class which uses CMMAMIDIControl.
_LIT(KMIDIControlName, "MIDIControl");

class CMMAMIDIPlayer;

// Maximum volume defined in the Java API.
// It is used to convert volume values between native and java values.
static const TInt KMAXVolume = 127;



//  CLASS DECLARATION
/**
*   This class implements MIDIControl interface.
*
*
*/
NONSHARABLE_CLASS(CMMAMIDIControl): public CMMAControl,
        public MMidiClientUtilityObserver
{
private:

    class CChannelVolumeEventWait : public CBase,
            public MTimeOutNotify
    {
    public:  // Constructor and destructor

        static CChannelVolumeEventWait* NewL();

        ~CChannelVolumeEventWait();

    public:  // Methods from base classes

        /**
        * From MTimeOutNotify Timer expiration call back method
        */
        void TimerExpired();

    public:  // New methods

        void StartWait(TInt aChannel);

        void StopWait();

        void HandleVolumeChangedEvent(TInt aChannel);

    private: // Constructor

        CChannelVolumeEventWait();

        void ConstructL();

    private: // Data

        // Own. For waiting the correct volume changed event
        CActiveSchedulerWait* iWait;

        // Own. Time-out for waiting volume changed event
        CTimeOutTimer* iTimer;

        // Expected channel for volume event
        TInt iChannel;

    };

public:
    /**
     * Creates new CMMAMIDIControl.
     *
     * @param aPlayer Player that plays the MIDI.
     */
    static CMMAMIDIControl* NewL(CMMAMIDIPlayer* aPlayer);

    /**
     * Destructor.
     */
    ~CMMAMIDIControl();
protected:
    /**
     * Constructor.
     * @param aPlayer Player that plays the MIDI.
     */
    CMMAMIDIControl(CMMAMIDIPlayer* aPlayer);

    /**
     * Initializes this control.
     */
    void ConstructL();

public: // From CMMAControl
    const TDesC& ClassName() const;

public: // From MMMAPlayerStateListener
    void StateChanged(TInt aState);

public: // Static new methods

    /**
     * Get volume for the given channel. The return value is
     * independent of the player's volume.
     *
     * @param aChannel 0-15
     */
    TInt ChannelVolumeL(TInt aChannel);

    /**
     * Set volume for given channel.
     *
     * @param aChannel 0-15
     * @param aVolume 0-127
     */
    void SetChannelVolumeL(TInt aChannel, TInt aVolume);

    /**
     * Set program of a channel.
     *
     * @param aChannel 0-15
     * @param aBank 0-16383, or -1 for default bank
     * @param aProgram 0-127
     */
    void SetProgramL(TInt aChannel,
                     TInt aBank,
                     TInt aProgram);

    /**
     * Sends a long MIDI event to the device.
     * This method passes the data directly to the receiving device.
     * The data array's contents are not checked for validity.
     *
     * @param aData array of the bytes to send
     * @return the number of bytes actually sent to the device
     */
    TInt SendMIDIEventL(const TDesC8* aData);

    /**
     * Reinitializes native midi engine with new sequence data.
     * First closes midi engine and then opens it again with new
     * data. The data array's contents are not checked for validity.
     *
     * @param aData midi sequence data for initialization
     * @return KErrNone (return value is present only for
     * method footprint)
     */
    TInt ReInitializeMidiL(const TDesC8* aData);

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
     * Used to control MIDI events.
     */
    CMMAMIDIPlayer* iPlayer;

    /**
    * Own. Utility for waiting for channel volume events
    */
    CChannelVolumeEventWait* iVolumeEventWait;

};


#endif // CMMAMIDICONTROL_H
