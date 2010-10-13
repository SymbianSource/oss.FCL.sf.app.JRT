/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef CAMMSTUNERPLAYER_H
#define CAMMSTUNERPLAYER_H

//  INCLUDES
#include <CMMAPlayer.h>
#include "CAMMSTunerControl.h"

#include <e32base.h>
#include <tuner.h>


// CLASS DECLARATION

/**
*  Player for tuner
*
*/
NONSHARABLE_CLASS(CAMMSTunerPlayer) : public CMMAPlayer,
        public MMMTunerAudioPlayerObserver
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CAMMSTunerPlayer* NewL(CAMMSTunerControl* aTunerControl);

    /**
    * Destructor.
    */
    virtual ~CAMMSTunerPlayer();

public: // from CMMAPlayer

    void PrefetchL();
    void StartL();
    void StopL(TBool aPostEvent);
    void DeallocateL();
    void GetMediaTime(TInt64* aMediaTime);

public: // from MMMTunerAudioPlayerObserver

    void MTapoInitializeComplete(TInt aError);
    void MTapoPlayEvent(MMMTunerAudioPlayerObserver::TEventType aEvent, TInt aError, TAny* aAdditionalInfo);

private:

    /**
    * C++ default constructor.
    */
    CAMMSTunerPlayer();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(CAMMSTunerControl* aTunerControl);


    TInt64 CurrentTime();

private:    // Data
    CMMTunerAudioPlayerUtility* iTunerPlayerUtility;
    CAMMSTunerControl* iTunerControl;
    CActiveSchedulerWait* iActiveSchedulerWait;

    TBool  iIsInitialized;
    TInt64 iMediaTime;
    TInt64 iStartTime;
};

#endif // CAMMSTUNERPLAYER_H

// End of File
