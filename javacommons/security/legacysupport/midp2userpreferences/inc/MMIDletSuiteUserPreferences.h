/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


/**
 *
 * @file    MMIDletSuiteUserPreferences.h
 *
 * @internalComponent
 *
 */

#ifndef MMIDLETSUITEUSERPREFERENCES_H
#define MMIDLETSUITEUSERPREFERENCES_H

#include <e32std.h>

namespace MIDP
{
class MSecurityPreferences;

/**
 *
 * An abstract operational interface to user preferences for an
 * individual MIDlet suite.
 *
 */

class MMIDletSuiteUserPreferences
{
public:

    virtual ~MMIDletSuiteUserPreferences() = 0;

    /**
     *
     * Returns the id of the MIDlet suite,
     *
     */

    virtual TUint32 Id(void) const = 0;

    /**
     *
     * Returns the named property of the MIDlet suite.
     *
     * @param aName     the name of the property
     *
     * @param aValue    where to 'place' the value of the property if found.
     *
     * @return KErrNone if the named property is found, KErrNotFound otherwise.
     *
     */

    virtual TInt PropertyL(const TDesC& aName, TPtrC& aValue) const = 0;

    /**
     *
     * Returns the user's security preferences for the MIDlet suite.
     *
     */

    virtual MSecurityPreferences& SecurityPreferencesL(void)  = 0;
};

inline MMIDletSuiteUserPreferences::~MMIDletSuiteUserPreferences()
{
}

} // end of namespace MIDP

#endif // MMIDLETSUITEUSERPREFERENCES_H
