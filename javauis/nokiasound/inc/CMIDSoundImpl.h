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
* Description:  Interface and a base class for CMIDClip and CMIDTone.
*
*/



#ifndef CMIDSOUNDIMPL_H
#define CMIDSOUNDIMPL_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MMIDEventSender;


//  CLASS DEFINITION
/**
 * This class is interface and a base class for CMIDClip and CMIDTone
 *
 */
NONSHARABLE_CLASS(CMIDSoundImpl): public CBase
{

public:

    enum TState
    {
        ENotReady,
        EReadyToPlay,
        EPlaying,
        ENotSupported,
        EInitialising
    };

public:

    // destructor
    ~CMIDSoundImpl();

protected:

    void ConstructL();

    // C++ constructor
    CMIDSoundImpl(MMIDEventSender* aEventSender);

public:

    void Resume();

    virtual TInt Play(TInt aLoop) = 0;
    virtual void Stop() = 0;
    virtual void SetVolume(TInt aVolume) = 0;
    virtual TInt Volume() = 0;
    virtual void Release() = 0;

    inline TInt State() const;

protected:


    TInt iState;

    // true if playback has been started.
    TBool iPlayed;

    CActiveSchedulerWait* iSchedulerWait;

    // events are sent through this interface
    MMIDEventSender* iEventSender;

};

inline TInt CMIDSoundImpl::State() const
{
    return iState;
};

#endif // CMIDSOUNDIMPL_H
