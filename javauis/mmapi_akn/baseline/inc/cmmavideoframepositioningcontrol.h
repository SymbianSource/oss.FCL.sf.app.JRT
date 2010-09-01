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
* Description:  This class implements FramePositioningControl for video player
*
*/


#ifndef CMMAVIDEOFRAMEPOSITIONINGCONTROL_H
#define CMMAVIDEOFRAMEPOSITIONINGCONTROL_H

//  INTERNAL INCLUDES
#include "cmmavideoplayer.h"
#include "cmmaframepositioningcontrol.h" // base class

// CONSTANTS

//  CLASS DECLARATION
/**
*   This class implements FramePositioningControl for video player
*/
NONSHARABLE_CLASS(CMMAVideoFramePositioningControl) :
        public CMMAFramePositioningControl
{
public:
    static CMMAVideoFramePositioningControl* NewL(CMMAVideoPlayer* aPlayer);

    /**
     * Destructor.
     */
    ~CMMAVideoFramePositioningControl();

protected:
    /**
     * Constructor.
     */
    CMMAVideoFramePositioningControl(CMMAVideoPlayer* aPlayer);

public: // From CMMAFramePositioningControl
    TInt SeekL(TInt aFrameNumber);
    TInt SkipL(TInt aFramesToSkip);
    void MapFrameToTimeL(TInt aFrameNumber, TInt64* aMediaTime);
    TInt MapTimeToFrameL(TInt64* aMediaTime);

private: // New methods
    /**
     * Get framerate of video and check that it is > 0 .
     */
    void GetAndCheckFrameRateL(TReal32& aFrameRate);

private:
    CMMAVideoPlayer* iPlayer;

};


#endif // CMMAVIDEOFRAMEPOSITIONINGCONTROL_H
