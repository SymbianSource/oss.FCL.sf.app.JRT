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
* Description:  This class is used for recording Audio
*
*/


#ifndef CMMAAUDIORECORDER_H
#define CMMAAUDIORECORDER_H

#include <mmf/common/mmfstandardcustomcommands.h>
#include "cmmammfplayerbase.h"
#include "cmmaplayerproperties.h"
#include "tmmaparametervalidator.h"
#include "rmmatempfile.h"

class CMMAOutputStream;

_LIT(KMMAAudioRecorder, "AudioRecorder");

//  CLASS DECLARATION
/**
*   This class is used for recording Audio
*
*
*/
NONSHARABLE_CLASS(CMMAAudioRecorder): public CMMAMMFPlayerBase
{
public: // Construction
    static CMMAAudioRecorder* NewLC(
        CMMAMMFResolver* aResolver,
        CMMAAudioSettings* aAudioSettings,
        TInt aMIDletSuiteID);

    //   Destructor
    ~CMMAAudioRecorder();

protected:
    //   C++ constructor
    CMMAAudioRecorder(
        CMMAMMFResolver* aResolver, TInt aMIDletSuiteID);

protected: // new methods
    void DoOpenL();

public: // from CMMAPlayer
    void StartL();
    void StopL(TBool aPostEvent = ETrue);
    void PrefetchL();
    void GetDuration(TInt64* aDuration);
    const TDesC& Type();
    void DeallocateL();

public: // new methods, for CMMAAudioRecordControl
    void InitializeL(RFile* aFile,
                     MMMFControllerEventMonitorObserver* aObserver);
    void Deinitialize();

    void StartRecordL();
    void StopRecordL();
    void ResetL();
    TInt SetRecordSizeLimitL(TInt aSize);

public: // from MMMFControllerEventMonitorObserver
    void HandleEvent(const class TMMFEvent& aEvent);

private:
    // owned
    CMMAAudioSettings* iSettings;

    RMMFAudioRecordControllerCustomCommands iAudioRecordControllerCustomCommands;
    RMMFAudioControllerCustomCommands iAudioControllerRecCustomCommands;

    CMMAOutputStream* iOutputStream;

    TInt iRecordSizeLimit;

    // error code returned from RMMFController::Pause method. Used check if
    // setting position is needed before recording.
    TInt iPauseError;

    // Data sink info
    TMMFMessageDestination iSinkInfo;

    // File to record
    RFile iFile;

    // Pass events ahead
    MMMFControllerEventMonitorObserver* iObserver;

    // Supplier of current controller
    TPtrC iSupplier;

    // Wait for native events
    CActiveSchedulerWait* iWait;
    TInt iError;

    TBool iResetController;
    
    // flag to hold EOF status
    TBool iEOFReached;
};

#endif // CMMAAUDIORECORDER_H
