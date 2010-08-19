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
* Description:  This class implements beep playing and OTA ringingtone playing.
*
*/


#ifndef CMIDTONE_H
#define CMIDTONE_H

//  INCLUDES
#include <e32base.h>
#include <mdaaudiotoneplayer.h>

#include "CMIDSoundImpl.h"

// FORWARD DECLARATIONS
class MMIDEventSender;

//  CLASS DEFINITION
/**
 *  This class implements beep playing and OTA ringingtone playing.
 *
 */
NONSHARABLE_CLASS(CMIDTone): public CMIDSoundImpl, public MMdaAudioToneObserver
{
public:

    static CMIDTone* NewL(TInt aFreq, TInt64 aDuration,
    MMIDEventSender* aEventSender);
    static CMIDTone* NewL(const TDesC8& aData, MMIDEventSender* aEventSender);
    //   Destructor
    ~CMIDTone();

protected:

    //   C++  constructor
    CMIDTone(MMIDEventSender* aEventSender);
    //   Second phase
    void ConstructL(TInt aFreq, TInt64 aDuration);
    void ConstructL(const TDesC8& aData);

public:

    virtual TInt Play(TInt aLoop);
    virtual void Stop();
    virtual void SetVolume(TInt aVolume);
    virtual TInt Volume();
    virtual void Release();

public: // from MMdaAudioToneObserver

    void MatoPrepareComplete(TInt aError);
    void MatoPlayComplete(TInt aError);

private:

    TInt iFreq;
    TInt64 iDuration;
    CMdaAudioToneUtility* iMdaAudioToneUtility;
};

#endif // CMIDTONE_H
