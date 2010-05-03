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
* Description:  Interface to access control groups.
*
*/


#ifndef MAMMSCONTROLLABLE_H
#define MAMMSCONTROLLABLE_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MAMMSControlGroup;

// CLASS DECLARATION
/**
*
*  Interface to access control groups.
*
*  @since 3.0
*/
NONSHARABLE_CLASS(MAMMSControllable)
{
public:
    /**
    * Finds and returns control group specified with a name.
    * @since 3.0
    * @param aClassName Name of the control group.
    * @return Control group or NULL if group can't be found.
    */
    virtual MAMMSControlGroup* Find(const TDesC& aClassName) = 0;

    /**
    * Returns control group at the specified index.
    * @since 3.0
    * @param aIndex Group index.
    * @return A control group at specified index.
    */
    virtual MAMMSControlGroup* At(TInt aIndex) = 0;

    /**
    * Return number of control groups.
    * @since 3.0
    * @return Number of control groups.
    */
    virtual TInt Count() = 0;
protected:

    /**
    * Protected destructor not to allow delete through this interface.
    */
    virtual ~MAMMSControllable() {};
};

#endif // MAMMSCONTROLLABLE_H
