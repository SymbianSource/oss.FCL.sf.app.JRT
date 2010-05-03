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
* Description:  Group for panning controls
*
*/


#ifndef CAMMSPANCONTROLGROUP_H
#define CAMMSPANCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammscontrolgroup.h"

// CONSTANTS
_LIT(KAMMSPanControlClassName, ".amms.control.PanControl");

// FORWARD DECLARATIONS
class CAMMSPanControl;

// CLASS DECLARATION

/**
 *  Group for panning controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSPanControlGroup): public CAMMSControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSPanControlGroup* NewLC();

    /**
     * Destructor.
     */
    ~CAMMSPanControlGroup();

public: // New functions

    /**
     * Gets the current panning set.
     *
     * @return The current panning.
     */
    TInt Pan();

    /**
     * Sets the panning using a linear point scale with values between -100
     * and 100. 0 represents panning for both channels, -100 full panning to
     * the left and 100 full panning to the right. If the given panning
     * value is less than -100 or greater than 100, the panning will be set
     * to -100 or 100, respectively.
     *
     * @param aPan The new panning to be set.
     * @param aSetPan Returned pan that has been set.
     */
    void SetPanL(TInt aPan, TInt& aSetPan);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSPanControl* TypeSafeControl(TInt aIndex) const;

public: // Functions from base classes
    /**
     * Returns class name that identifies this control group.
     *
     * @return Control group name.
     */
    const TDesC16& ClassName();

protected:
    /*
     * Called by when a new player is added
     *
     * @param aPlayer   The player being added
     * @param aControl  The player's control relevant to this group
     */
    void NotifyPlayerAddedL(CMMAPlayer *aPlayer, CMMAControl* aControl);

private:
    /**
     * C++ default constructor.
     */
    CAMMSPanControlGroup();

private: //Data

    // the actual volume for this control group
    TInt iPan;
};

#endif // CAMMSPANCONTROLGROUP_H
