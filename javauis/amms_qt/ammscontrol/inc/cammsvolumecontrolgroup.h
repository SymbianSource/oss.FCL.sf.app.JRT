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
* Description:  Group for volume controls
*
*/


#ifndef CAMMSVOLUMECONTROLGROUP_H
#define CAMMSVOLUMECONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammscontrolgroup.h"

// FORWARD DECLARATIONS
class CAMMSVolumeControl;

// CONSTANTS
_LIT(KAMMSVolumeControlClassName, ".amms.control.VolumeControl");

// CLASS DECLARATION

/**
 *  Group for volume controls.
 *  This volume control group can be used with many CAMMSVolumeControl
 *  instances. Controls that belong to this group are identified with
 *  aClassName parameter given to the NewL / NewLC method.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSVolumeControlGroup): public CAMMSControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aControlName Class name that identifies controls
     * belonging tothis control group.
     */
    static CAMMSVolumeControlGroup* NewLC(const TDesC& aControlName);

    /**
     * Destructor.
     */
    ~CAMMSVolumeControlGroup();

public: // New functions
    /**
     * Sets the current volume.
     *
     * @param aVolume Volume to be set.
     */
    void SetVolumeL(TInt aVolume);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSVolumeControl* TypeSafeControl(TInt aIndex) const;

public: // Functions from base class
    /**
     * Returns class name that identifies this control group.
     *
     * @return Control group name.
     */
    const TDesC16& ClassName();

protected: // From
    /**
     * Called by when a new player is added
     *
     * @param aPlayer   The player being added
     * @param aControl  The player's control relevant to this group
     */
    void NotifyPlayerAddedL(CMMAPlayer *aPlayer, CMMAControl* aControl);

private:
    /**
     * C++ default constructor.
     * @param aControlName Class name that identifies controls
     * belonging tothis control group.
     */
    CAMMSVolumeControlGroup(const TDesC& aControlName);

protected: //Data

    TInt iVolume;
};

#endif // CAMMSVOLUMECONTROLGROUP_H


