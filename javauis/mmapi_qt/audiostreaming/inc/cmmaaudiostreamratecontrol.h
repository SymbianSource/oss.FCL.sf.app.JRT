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
* Description:  This class implements RateControl for HTTP stream audio player.
*
*/


#ifndef CMMAAUDIOSTREAMRATECONTROL_H
#define CMMAAUDIOSTREAMRATECONTROL_H

//  INTERNAL INCLUDES
#include "cmmaratecontrol.h" // base class
#include "cmmaaudiostreamplayer.h"

//  CLASS DECLARATION
/**
*   This class implements RateControl for http stream audio player.
*
*/
NONSHARABLE_CLASS(CMMAAudioStreamRateControl): public CMMARateControl,
        public MMMAPlayerStateListener
{
public:

    static CMMAAudioStreamRateControl* NewL(
        CMMAAudioStreamPlayer* aPlayer);

    /**
     * Destructor.
     */
    ~CMMAAudioStreamRateControl();

protected:

    /**
     * Constructor.
     */
    CMMAAudioStreamRateControl(CMMAAudioStreamPlayer* aPlayer);

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
    CMMAAudioStreamPlayer* iPlayer;

    /**
     * Hold current rate value.
     */
    TInt iCurrentRate;

};

#endif // CMMAAUDIOSTREAMRATECONTROL_H
