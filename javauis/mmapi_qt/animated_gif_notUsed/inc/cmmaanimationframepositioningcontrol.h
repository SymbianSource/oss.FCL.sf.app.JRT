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
* Description:  This class implements FramePositioningControl for animation
*
*/


#ifndef CMMAANIMATIONFRAMEPOSITIONINGCONTROL_H
#define CMMAANIMATIONFRAMEPOSITIONINGCONTROL_H

//  INTERNAL INCLUDES
#include "cmmaanimationplayer.h"
#include "cmmaframepositioningcontrol.h" // base class


// CONSTANTS

// FORWARD DECLARATIONS
class MIHLImageViewer;

//  CLASS DECLARATION
/**
*   This class implements FramePositioningControl for video player
*/
NONSHARABLE_CLASS(CMMAAnimationFramePositioningControl) :
        public CMMAFramePositioningControl
{
public:
    static CMMAAnimationFramePositioningControl* NewL(CMMAAnimationPlayer* aPlayer);

    /**
     * Destructor.
     */
    ~CMMAAnimationFramePositioningControl();

protected:
    /**
     * Constructor.
     */
    CMMAAnimationFramePositioningControl(CMMAAnimationPlayer* aPlayer);

    void ConstructL(CMMAAnimationPlayer* aPlayer);

public: // From CMMAFramePositioningControl
    TInt SeekL(TInt aFrameNumber);
    TInt SkipL(TInt aFramesToSkip);
    void MapFrameToTimeL(TInt aFrameNumber, TInt64* aMediaTime);
    TInt MapTimeToFrameL(TInt64* aMediaTime);

private: // New methods
    TInt FindFrame(MIHLImageViewer* aViewer, TInt aFrameNumber);

private: // Data
    CMMAAnimationPlayer* iPlayer;

};


#endif // CMMAANIMATIONFRAMEPOSITIONINGCONTROL_H
