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
* Description:  This class is used for playing MIDI.
*
*/


#ifndef CMMADRMAUDIOPLAYER_H
#define CMMADRMAUDIOPLAYER_H

//  INCLUDES
#include "cmmaaudioplayer.h"
#include <DrmAudioSamplePlayer.h>
#include <e32base.h>

// CONSTANTS
_LIT(KMMADRMPlayer, "DRMPlayer");

// FORWARD DECLARATIONS
class CMMAEventSource;

//  CLASS DECLARATION
/**
*   This class is used for playing DRM Audio.
*
*/

NONSHARABLE_CLASS(CMMADRMAudioPlayer): public CMMAPlayer,
        public MDrmAudioPlayerCallback

{
public: // Construction
    static CMMADRMAudioPlayer* NewLC(const TDesC& aContentType,
    TFileName aFileName);

    //   Destructor
    ~CMMADRMAudioPlayer();

protected:
    //   C++ constructor
    CMMADRMAudioPlayer(TFileName aFileName);
    void ConstructL(const TDesC& aContentType);

public: // new methods
    CDrmPlayerUtility* DRMUtility() const;

public: // from CMMAPlayer
    void StartL();
    void StopL(TBool aPostEvent);
    void RealizeL();
    void PrefetchL();
    void DeallocateL();
    void GetDuration(TInt64* aDuration);
    void SetMediaTimeL(TInt64* aTime);
    void GetMediaTime(TInt64* aMediaTime);
    void CloseL();
    const TDesC& Type();

public: // from CMMAPlayer
    void PlayCompleteL(TInt aError);

public: // from MDrmAudioPlayerCallback
    void MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MdapcPlayComplete(TInt aError);

protected:
    void CloseClientUtility();
    void ErrorPlaybackComplete(TInt aError);

private: // Data
    CDrmPlayerUtility* iUtility;

    TFileName iFileName;
};

#endif // CMMADRMAUDIOPLAYER_H
