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
* Description:  This class implements AnimationRateControl functionality.
*
*/


#ifndef CMMAANIMATIONRATECONTROL_H
#define CMMAANIMATIONRATECONTROL_H

// EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "cmmaratecontrol.h" // base class

// CONSTANTS

class CMMAAnimationPlayer;

//  CLASS DECLARATION
/**
*   This class implements AnimationRateControl interface.
*
*
*/
NONSHARABLE_CLASS(CMMAAnimationRateControl): public CMMARateControl
{
public:
    /**
     * Creates new CMMAAnimationRateControl.
     *
     * @param aPlayer Player that plays the content.
     */
    static CMMAAnimationRateControl* NewL(CMMAAnimationPlayer* aPlayer);

    /**
     * Destructor.
     */
    ~CMMAAnimationRateControl();
protected:
    /**
     * Constructor.
     * @param aPlayer Player that plays the content.
     */
    CMMAAnimationRateControl(CMMAAnimationPlayer* aPlayer);

    /**
     * Initializes this control.
     */
    void ConstructL();

public: // From CMMARateControl
    /**
     * @param aRate Rate to set in "milli-percentage"..
     * @return Actual rate set.
     */
    TInt SetRateL(TInt aRate);

    /**
     * @return The current playback rate in "milli-percentage".
     */
    TInt RateL();

private: // Data
    /**
     * Used to control animation.
     */
    CMMAAnimationPlayer* iPlayer;

};


#endif // CMMAANIMATIONRATECONTROL_H
