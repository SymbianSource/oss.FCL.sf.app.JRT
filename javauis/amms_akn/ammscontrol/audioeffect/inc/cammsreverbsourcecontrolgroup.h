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
* Description:  Group for reverb source controls
*
*/


#ifndef CAMMSREVERBSOURCECONTROLGROUP_H
#define CAMMSREVERBSOURCECONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammscontrolgroup.h"

// CONSTANTS
_LIT(KAMMSReverbSourceControlClassName,
     ".amms.control.audioeffect.ReverbSourceControl");

// FORWARD DECLARATIONS
class CAMMSBaseReverbSourceControl;


// CLASS DECLARATION
/**
 *  Group for reverb controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSReverbSourceControlGroup): public CAMMSControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSReverbSourceControlGroup* NewLC();

    /**
     * destructor
     */
    ~CAMMSReverbSourceControlGroup();

public: // New functions

    /**
     * Gets the gain level of the reverberation
     *
     * @return the gain level of the reverberation
     */
    TInt RoomLevel();

    /**
     * Sets the gain level of the reverberation
     *
     * @param the gain level of the reverberation to be set
     */
    void SetRoomLevelL(TInt aLevel);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSBaseReverbSourceControl* TypeSafeControl(TInt aIndex) const;

public: // Functions from base classes
    /**
     * Returns class name that identifies this control group.
     *
     * @return Control group name.
     */
    const TDesC16& ClassName();

protected: // Functions from base classes

    /**
     * Called by PlayerAddedL when new player is added
     *
     * @param aPlayer   The player being added
     * @param aControl  The player's control relevant to this group
     */
    void NotifyPlayerAddedL(CMMAPlayer* aPlayer, CMMAControl* aControl);

private:
    /**
     * C++ default constructor.
     */
    CAMMSReverbSourceControlGroup();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // data

    // the actual room level for this control group
    TInt iRoomLevel;

};

#endif // CAMMSREVERBSOURCECONTROLGROUP_H
