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
* Description:  This class is a RecordControl.
*
*/


#ifndef CMMARECORDCONTROL_H
#define CMMARECORDCONTROL_H

//  INTERNAL INCLUDES
#include <f32file.h>
#include "cmmaoutputstream.h"
#include "cmmacontrol.h"
#include "mmmaplayerstatelistener.h"

//  FORWARD DECLARATIONS
class CMMAPlayer;

// CONSTANTS

// Too fast start and stop can cause problems,
// 0.5s is enough to make pause work.
const TInt KMinRecordPauseInterval = 500000;

_LIT(KMMARecordTempDirectory, "d:\\");   // CSI: 25 dir for temporary recording file #
_LIT(KMMARecordErrorMsg, "Recording error: %d");
const TInt KRecordErrorMessageSize = 25;

//  CLASS DECLARATION
/**
*   This class is the native side of the RecordControl Java class. This class
*   delegates static method calls to virtual methods which are implemented in
*   derived classes. This class only implements ClassName method.
*
*
*/
NONSHARABLE_CLASS(CMMARecordControl):
        public CMMAControl,
        public MMMAPlayerStateListener
{
protected:
    // Internal state of this class.
    enum TRecordState
    {
        // recorder has not been initialized (startRecord has not been called)
        ERecordUninitialized = 1,       //Recorder uninitialized
        // recorder has been initialized (startRecord) however,
        // recodring has been stopped after that (stopRecord)
        ERecordInitialized,
        // recorder has been initialized (startRecord) but player is not in
        // STARTED state
        ERecordStandBy,
        // recorder has been initialized (startRecord) and player is in
        // STARTED state. i.e. recording is ON
        ERecordRecording
    };

protected: // proteced constructor and destructor for preventing direct use
    CMMARecordControl(CMMAPlayer* aPlayer);
    ~CMMARecordControl();
    void ConstructL();

    /**
     * Waits until recording can be paused after start.
     * @param aMediaTime Current media time.
     */
    void WaitForPauseL(const TInt64& aMediaTime);

public: // new methods
    /**
     * Start recording.
     */
    virtual void StartRecordL();

    /**
     * Stops recording.
     */
    virtual void StopRecordL();

    /**
     * Completes current recording. The rest of the data will written
     * to the output stream. This method may not be called when recording.
     */
    virtual void CommitL();

    /**
     * Erase current recording.
     */
    virtual void ResetL();

    /**
     * Set the output stream where the data will be recorded.
     * Existing stream will be deleted.
     * @param aStream New stream to use.
     */
    virtual void SetRecordStream(CMMAOutputStream* aStream);

    /**
     * Return the content type of the recorded media.
     * @return Content type used for recording.
     */
    virtual HBufC* GetContentTypeL();

public: // new pure virtual methods

    /**
     * Set the record size limit. This limits the size of the recorded
     * media to the number of bytes specified.
     *
     * When recording is in progress, commit will be called implicitly
     * in the following cases:
     *  - Record size limit is reached
     *  - If the requested size is less than the already recorded size
     *  - No more space is available.
     *
     * Once a record size limit has been set,
     * it will remain so for future recordings until it is
     * changed by another setRecordSizeLimit call.
     *
     * @param aSize New size limit.
     * @return Actual value set.
     */
    virtual TInt SetRecordSizeLimitL(TInt aSize) = 0;

    /**
     *  Start the recording in implementation
     */
    virtual void DoStartRecordL() = 0;

    /**
     *  Start the recording in implementation
     */
    virtual void DoStopRecordL() = 0;

    /**
     *  Initialize native recorder
     */
    virtual void InitializeL() = 0;

    /**
     *  Reset recorder
     */
    virtual void DoResetL() = 0;

public: // From CMMAControl
    const TDesC& ClassName() const;


public: // from MMMAPlayerListener
    void StateChanged(TInt aState);

protected: // new methods
    /**
     * Sends record error event to Java
     */
    void Error(TInt aErrorCode);

    /**
     * Handles situation when record size limit is reached
     */
    void HandleRecordSizeLimit();

private: // new methods
    void NextStateL(TInt aPlayerState);

protected: // Members
    // Current state of recording
    TRecordState iState;

    // Where to record
    RFile iFile;
    TFileName iFilename;

    // Recording player
    CMMAPlayer* iPlayer;

    // Rfs used for creating temp file and deleting it
    RFs iFs;
private: // Members

    // Stream to write data, owned
    CMMAOutputStream* iOutputStream;

    /**
     * Start time of recording.
     */
    TInt64 iStartTime;

    /**
     * Owned. Used to wait before pause if it
     * is called too fast after starting.
     */
    RTimer iRecordPauseTimer;
};


#endif // CMMARECORDCONTROL_H
