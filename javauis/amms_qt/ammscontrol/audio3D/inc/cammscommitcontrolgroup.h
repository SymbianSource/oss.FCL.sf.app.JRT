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
* Description:  Group for commit controls
*
*/


#ifndef CAMMSCOMMITCONTROLGROUP_H
#define CAMMSCOMMITCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammsaudio3dcontrolgroup.h"
#include "cammsmodulecontainer.h"
#include "cammsmodule.h"

// FORWARD DECLARATIONS
class CAMMSModule;

// CONSTANTS
_LIT(KAMMSCommitControlClassName, ".amms.control.audio3d.CommitControl");
_LIT(KAMMSCommitControl, "CommitControl");

// CLASS DECLARATION
/**
 *  Group for commit controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSCommitControlGroup): public CAMMSAudio3DControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSCommitControlGroup* NewLC(
        CAMMSModule& aSpectator,
        CAMMSModuleContainer& aSoundSource3Ds);

    /**
     * Destructor.
     */
    ~CAMMSCommitControlGroup();

public: // New functions

    /**
     * Transfers all the pending parameters to the audio processing system
     * for all ControlGroups in this controllable
     */
    void CommitAllControlsL();

    /**
     * Sets the mode of the CommitControl
     *
     * @param aDeferred     deferred mode flag
     */
    void SetDeferredL(TBool aDeferred);

public: // Functions from base classes
    /**
     * Returns class name that identifies this control group.
     *
     * @return Control group name.
     */
    const TDesC16& ClassName();

protected:
    /**
     * Transfers all the pending parameters to the audio processing system.
     *
     * @param aCommit   variable id that need to be commited
     */
    void CommitL(TInt aCommit);

private:
    /**
     * C++ default constructor.
     */
    CAMMSCommitControlGroup(
        CAMMSModule& aSpectator,
        CAMMSModuleContainer& aSoundSource3Ds);

private:    // Data
    // Reference to spectator, owned by GlobalManager
    CAMMSModule& iSpectator;

    // Reference to sound source 3Ds, owned by GlobalManager
    CAMMSModuleContainer& iSoundSource3Ds;

};

#endif // CAMMSCOMMITCONTROLGROUP_H


