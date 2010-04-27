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
* Description:  Contact adapter interface, provides access to a Contact list.
 *
*/


#ifndef MPIMCONTACTADAPTERMANAGER_H
#define MPIMCONTACTADAPTERMANAGER_H

//  FORWARD DECLARATIONS
class MPIMContactListAdapter;
class MPIMAdapterManager;

// CLASS DECLARATION
/**
 *  Provides access to a single contact list. See also \ref MPIMAdapterManager
 *  class description.
 */
class MPIMContactAdapterManager
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMContactAdapterManager()
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
    MPIMContactAdapterManager()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor.
    MPIMContactAdapterManager(const MPIMContactAdapterManager&)
    {
    }

    // Prohibit assigment operator.
    MPIMContactAdapterManager& operator=(const MPIMContactAdapterManager&)
    {
        return *this;
    }

};

#endif // MPIMCONTACTADAPTERMANAGER_H
// End of File
