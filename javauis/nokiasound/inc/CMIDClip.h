/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements wav playing.
*
*/


#ifndef CMIDCLIP_H
#define CMIDCLIP_H

//  INCLUDES
#include <e32base.h>
#include <mdaaudiosampleplayer.h>

#include "CMIDSoundImpl.h"

// FORWARD DECLARATIONS
class MMIDEventSender;

//  CLASS DEFINITION
/**
 * This class implements wav playing.
 *
 */
NONSHARABLE_CLASS(CMIDClip): public CMIDSoundImpl,
        public MMdaAudioPlayerCallback
{
public:

    static CMIDClip* NewL(const TDesC8* aData, MMIDEventSender* aEventSender);
    //   Destructor
    ~CMIDClip();

protected:

    //   C++ constructor
    CMIDClip(MMIDEventSender* aEventSender);
    //   Second phase
    void ConstructL(const TDesC8* aData);

public:

    virtual TInt Play(TInt aLoop);
    virtual void Stop();
    virtual void SetVolume(TInt aVolume);
    virtual TInt Volume();
    virtual void Release();

public: // from MMdaAudioPlayerCallback
    void MapcInitComplete(TInt aError,
                          const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);

private:

    HBufC8* iClip;
    CMdaAudioPlayerUtility* iMdaAudioPlayerUtility;
    TInt iLoops;
    TInt iVolume;
};

#endif // CMIDCLIP_H
