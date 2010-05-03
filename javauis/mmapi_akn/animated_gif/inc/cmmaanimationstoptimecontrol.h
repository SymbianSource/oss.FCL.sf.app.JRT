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
* Description:  This class is used for stoptime controlling for animation player
*
*/


#ifndef CMMAANIMATIONSTOPTIMECONTROL_H
#define CMMAANIMATIONSTOPTIMECONTROL_H

//  INCLUDES
#include "cmmaanimationplayer.h"
#include "cmmastoptimecontrol.h"

//  CLASS DECLARATION
/**
*   This class is used for stoptime controlling for animation
*
*
*/

NONSHARABLE_CLASS(CMMAAnimationStopTimeControl): public CMMAStopTimeControl,
        public MMMAAnimationObserver
{
public:
    static CMMAAnimationStopTimeControl* NewL(CMMAAnimationPlayer* aPlayer);
    ~CMMAAnimationStopTimeControl();

protected:
    CMMAAnimationStopTimeControl(CMMAAnimationPlayer* aPlayer);

public: // from CMMAStopTimeControl
    void SetStopTimeL(const TInt64& aTime);

public: // from MMMAAnimationObserver
    void AnimationAdvancedL(TInt aFrame, TInt64 aMediaTime);
};

#endif // CMMAANIMATIONSTOPTIMECONTROL_H
