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
* Description:  This class implements RateControl for DRMv2 audio player.
*
*/


#ifndef CMMADRMRATECONTROL_H
#define CMMADRMRATECONTROL_H

//  INTERNAL INCLUDES
#include "cmmaratecontrol.h" // base class
#include "cmmadrmaudioplayer.h"

//  CLASS DECLARATION
/**
*   This class implements RateControl for DRMv2 audio player.
*
*/
NONSHARABLE_CLASS(CMMADRMRateControl): public CMMARateControl,
        public MMMAPlayerStateListener
{
public:

    static CMMADRMRateControl* NewL(CMMADRMAudioPlayer* aPlayer);

    /**
     * Destructor.
     */
    ~CMMADRMRateControl();

protected:

    /**
     * Constructor.
     */
    CMMADRMRateControl(CMMADRMAudioPlayer* aPlayer);

    void ConstructL();

public: // from MMMAPlayerStateListener
    virtual void StateChanged(TInt aState);

public: // New methods
    /**
     * @param aRate Rate to set in "milli-percentage"..
     * @return Actual rate set.
     */
    virtual TInt SetRateL(TInt aRate);

    /**
     * @return The current playback rate in "milli-percentage".
     */
    virtual TInt RateL();


private: // Member data

    /**
     * Used to obtain RMMFController reference to stop/start
     * playback.
     */
    CMMADRMAudioPlayer* iPlayer;

    /**
     * Hold current rate value.
     */
    TInt iCurrentRate;

};

#endif // CMMADRMRATECONTROL_H
