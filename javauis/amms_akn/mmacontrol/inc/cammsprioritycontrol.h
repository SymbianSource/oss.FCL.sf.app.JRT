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
* Description:  Manipulates the priority of a Player.
*
*/


#ifndef CAMMSPRIORITYCONTROL_H
#define CAMMSPRIORITYCONTROL_H

//  INCLUDES
#include "cammscontrol.h"
#include <mmmaplayerstatelistener.h>

// CONSTANTS
_LIT(KAMMSPriorityControl, "PriorityControl");

// CLASS DECLARATION

/**
*
*  Controls for the priority.
*  This class delegates priority setting calls to RMMFController.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSPriorityControl)
        : public CAMMSControl, public MMMAPlayerStateListener
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSPriorityControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSPriorityControl();

public: // New functions

    /**
    * Sets the priority.
    *
    * @param aPriority The new priority to be set.
    */
    void SetPriorityL(TInt aPriority);

    /**
    * Gets the priority.
    *
    * @return The set priority.
    */
    TInt Priority();

public:  // From MMMAPlayerStateListener

    void StateChanged(TInt aState);

public: // Functions from base classes

    const TDesC& ClassName() const;

protected:  // New functions

    /**
     * Scales the given AMMS priority to MMF priority
     * and sets it to MMF if the priority has changed.
     * @param aAmmsPriority AMMS priority to be set.
     */
    void SetPriorityToMmfL(TInt aAmmsPriority);

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSPriorityControl(CMMAPlayer* aPlayer);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private:  // data

    // Priority cannot be set if player is REALIZED. The priority is set
    // when the player is prefetched.

    TInt iVisiblePriority;  // Priority visible in AMMS.

    TInt iActualPriority;   // Priority set in MMF.

};

#endif // CAMMSPRIORITYCONTROL_H


