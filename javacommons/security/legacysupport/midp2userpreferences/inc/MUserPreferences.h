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
 * @file    MUserPreferences.h
 *
 * @internalComponent
 *
 */

#ifndef MUSERPREFERENCES_H
#define MUSERPREFERENCES_H

#include <e32std.h>

namespace MIDP
{
class MMIDletSuiteInfo;
class MSecurityPreferences;
class MMIDletSuiteUserPreferences;
class MMIDletSuiteUserPreferencesIterator;
class MMIDletSuiteCustomAttributes;

/**
 *
 * An abstract operational interface to the user's per MIDlet suite preferences.
 *
 */

class MUserPreferences
{
public:

    virtual ~MUserPreferences() = 0;

    /**
     *
     * Returns the security preferences for a given MIDlet suite.
     *
     * Ownership of the returned object is transferred to the caller.
     *
     * @param aInfo information identifying the MIDlet suite
     *
     * @return the security preferences for the given MIDlet suite,
     *         if any.
     *
     *         [Ownership of the returned object is transferred to the caller].
     *
     */

    virtual MSecurityPreferences* SecurityPreferencesL(const MMIDletSuiteInfo& aInfo) = 0;

    /**
     *
     * Returns an iterator which can be used to iterate over the user
     * preferences for all installed MIDlet suites.
     *
     * @return a preferences iterator
     *
     * [Ownership of the returned object is transferred to the caller].
     *
     */

    virtual MMIDletSuiteUserPreferencesIterator* IteratorL(void) = 0;

    /**
     * Returns the custom attributes of the given MIDlet suite.
     * The custom attributes are modifiable.
     *
     * Ownership of the returned object is transferred to the caller.
     *
     * @param aSuiteId the Id of the MIDlet suite
     *
     * @return the custom attributes of the given MIDlet suite
     *
     *  [Ownership of the returned object is transferred to the caller].
     *
     */

    virtual MMIDletSuiteCustomAttributes* CustomAttributesL(TUint32 aSuiteId) = 0;

};

inline MUserPreferences::~MUserPreferences()
{
}

} // end of namespace MIDP

#endif // MUSERPREFERENCES_H
