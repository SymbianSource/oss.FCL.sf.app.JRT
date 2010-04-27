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
 * @file    MMIDletSuiteUserPreferencesIterator.h
 *
 * @internalComponent
 *
 */

#ifndef MMIDLETSUITEUSERPREFERENCESITERATOR_H
#define MMIDLETSUITEUSERPREFERENCESITERATOR_H

#include <e32base.h>

namespace MIDP
{
class MMIDletSuiteUserPreferences;

/**
 *
 * An abstract operational interface to an iterator to be used to
 * iterate over the user preferences for all installed MIDlet suites.
 *
 */

class MMIDletSuiteUserPreferencesIterator
{
public:

    virtual ~MMIDletSuiteUserPreferencesIterator() = 0;

    /**
     *
     * Returns 'true' if a subsequent call to NextL() will succeed.
     *
     */

    virtual TBool HasNext(void) = 0;

    /**
     *
     * Returns a pointer to an instance of MMIDletSuiteUserPreferences
     * if an immediately preceding call to HasNext() returns 'true',
     * otherwise leaves.
     *
     */

    virtual MMIDletSuiteUserPreferences* NextL(void) = 0;
};

inline MMIDletSuiteUserPreferencesIterator::~MMIDletSuiteUserPreferencesIterator()
{
}

} // end of namespace MIDP

#endif // MMIDLETSUITEUSERPREFERENCESITERATOR_H


