/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Group for 3D audio controls
*
*/


#ifndef CAMMSAUDIO3DCONTROLGROUP_H
#define CAMMSAUDIO3DCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammscontrolgroup.h"

// CLASS DECLARATION

/**
 *  Group for 3D audio controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSAudio3DControlGroup): public CAMMSControlGroup
{
public:  // Constructors and destructor

    /**
     * Destructor.
     */
    ~CAMMSAudio3DControlGroup();

protected: // New functions

    /**
     * Update the controls depending on commit mode
     *
     * @param aCommit   variable id that need to be commited
     */
    void UpdateL(TInt aCommit);

    /**
     * Transfers all the pending parameters to the audio processing system.
     *
     * @param aCommit   variable id that need to be commited
     */
    virtual void CommitL(TInt aCommit) = 0;

public:  // From MAMMSControlGroup
    /**
     * Sets the mode of the CommitControl.
     *
     * @param aMode     commit mode
     */
    void SetModeL(TCommitMode aMode);

    /**
     * Commit all the controls in the group
     * in immediate mode commits, in deferred mode marks what variables
     * need to be commited later
     */
    void CommitGroupL();

protected:
    /**
     * Constructor.
     *
     * @param aName     The name of the corresponding amms control
     * @param aControlType Special AMMS type of the Control
     */
    CAMMSAudio3DControlGroup(
        const TDesC& aName,
        TAMMSControlTypes aControlType = EAMMSBaseControl);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

protected: // Data

    // Commit mode
    TCommitMode iCommitMode;

private:     // Data

    // bit field of variables that have to be commited
    // (used in CommitL())
    TInt iCommit;
};

#endif // CAMMSAUDIO3DCONTROLGROUP_H
