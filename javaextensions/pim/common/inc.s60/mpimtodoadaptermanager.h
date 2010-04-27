/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ToDo adapter interface, provides access to a ToDo list.
 *
*/


#ifndef MPIMTODOADAPTERMANAGER_H
#define MPIMTODOADAPTERMANAGER_H

//  FORWARD DECLARATIONS
class MPIMToDoListAdapter;
class MPIMAdapterManager;

// CLASS DECLARATION

/**
 *  Provides access to a single todo list. See also \ref MPIMAdapterManager
 *  class description.
 */
class MPIMToDoAdapterManager
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMToDoAdapterManager()
    {
    }

public:
    // New functions

    /**
     * Provides access to the \ref MPIMAdapterManager representation of
     * this object.
     *
     * @return The \ref MPIMAdapterManager representation of this object.
     */
    virtual MPIMAdapterManager* GetAdapterManager() = 0;

protected:
    // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMToDoAdapterManager()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor.
    MPIMToDoAdapterManager(const MPIMToDoAdapterManager&)
    {
    }

    // Prohibit assigment operator.
    MPIMToDoAdapterManager& operator=(const MPIMToDoAdapterManager&)
    {
        return *this;
    }

};

#endif // MPIMTODOADAPTERMANAGER_H
// End of File

