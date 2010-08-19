/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper for CMIDClip and CMIDTone class initialisations.
*
*/


//  INCLUDES
#include <mevents.h>
#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include "MMIDEventSender.h"
#include "functionserver.h"

// FORWARD DECLARATIONS
class CMIDSoundImpl;

// CLASS DEFINITION
/**
 * CMIDSound is a wrapper that handles which class is
 * wanted to be initialised, CMIDClip or CMIDTone
 *
 */
NONSHARABLE_CLASS(CMIDSound):
        public CBase, MCenRepNotifyHandlerCallback, MMIDEventSender, java::util::FunctionServer
{

public:

    //   Destructor
    ~CMIDSound();

public: // From MCenRepNotifyHandlerCallback
    /**
     * Notifies this class about changes in the audio enable status
     */
    void HandleNotifyInt(TUint32 aId, TInt aNewValue);

public: // From MMIDEventSender
    /**
    * Sends event to Java SoundListener.
    * @since 3.0
    * @param aEventType Type of the event to send
    */
    void SendEvent(TInt aEventType);

protected:

    //  C++ default constructor
    CMIDSound();

private:
    TBool IsAudioEnabled();

public:

    static CMIDSound* NewL(JNIEnv& aJni, jobject aSound);
    void SetBeep(TInt aFreq, TInt64 aDuration);
    TInt Init(TInt aType, const TDesC8* aData);
    void InitL(TInt aType, const TDesC8* aData);
    TInt InitProfileListener();
    void InitProfileListenerL();
    TInt Play(TInt aLoop);
    TInt DoPlay(TInt aLoop);
    void Resume();
    void DoResume();
    void Stop();
    void DoStop();
    TInt SoundVolume();
    TInt Volume();
    void SetVolume(TInt aVolume);
    void DoSetVolume(TInt aVolume);
    TInt State();
    TInt PlayerState();
    void Release();
    void DoRelease();


private:

    CMIDSoundImpl* iMIDSound;
    TInt iFreq;
    TInt64 iDur;
    TInt iSource;
    TInt iRealVolume;

    // owned, informs about changes in iRepository
    CCenRepNotifyHandler* iProfileListener;

    // owned, current profile
    CRepository* iRepository;

    TInt iHandle;
};

