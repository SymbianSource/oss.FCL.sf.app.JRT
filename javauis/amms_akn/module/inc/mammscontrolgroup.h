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
* Description:  Interface for all control groups.
*
*/


#ifndef MAMMSCONTROLGROUP_H
#define MAMMSCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*
*  Interface for all control groups.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(MAMMSControlGroup)
{
protected: // Enumerations

    enum TCommitMode { EImmediate, EDeferred };

public:  // New functions
    /**
    * Returns class name that identifies this control group.
    * @return Control group name.
    */
    virtual const TDesC& ClassName() = 0;

    /**
     * Sets the mode of the CommitControl.
     *
     * @param aMode commit mode
     */
    virtual void SetModeL(TCommitMode /*aMode*/) {};

    /**
     * Commit all the controls in the group
     * in immediate mode commits, in deferred mode marks what variables
     * need to be commited later
     */
    virtual void CommitGroupL() {};

protected:
    /**
    * Protected destructor not allow delete throw this interface.
    */
    virtual ~MAMMSControlGroup() {};
};
#endif // MAMMSCONTROLGROUP_H
