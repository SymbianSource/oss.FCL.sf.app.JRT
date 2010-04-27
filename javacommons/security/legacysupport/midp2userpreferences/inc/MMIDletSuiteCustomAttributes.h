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
 * @file    MMIDletSuiteCustomAttributes.h
 *
 * @internalComponent
 *
 */

#ifndef MMIDLETSUITECUSTOMATTRIBUTES_H
#define MMIDLETSUITECUSTOMATTRIBUTES_H

#include <e32std.h>

namespace MIDP
{

/**
 *
 * The abstract interface to the modifiable custom attributes
 * of an individual MIDlet suite.
 *
 */

/**
 * The names of the known common custom attributes
 */
_LIT(KOnScreenKeyboardAttribute, "Nokia-MIDlet-On-Screen-Keypad");

/**
 * The valid values of custom attribute KOnScreenKeyboardAttribute
 */
_LIT(KValueNo, "no");
_LIT(KValueGameactions,    "gameactions");
_LIT(KValueNavigationkeys, "navigationkeys");

/**
 * The known flags for the values of the custom attributes.
 */
const TUint32 KFlagUserModifiable    = 0x00000000;
const TUint32 KFlagNonUserModifiable = 0x00000001;

class MMIDletSuiteCustomAttributes
{
public:

    virtual ~MMIDletSuiteCustomAttributes() = 0;

    /**
     * Add new custom attribute for the MIDlet suite.
     *
     * @param aName     the name of the attribute
     * @param aValue    the value of the attribute
     * @param aFlag     any 32 bit flag, e.g. KFlagNonUserModifiable or 0x12345678
     *
     */
    virtual void AddL(const TDesC& aName, const TDesC& aValue, const TUint32 aFlag) = 0;

    /**
     * Get the value of custom attribute aName.
     *
     * @param aName     the name of the attribute
     * @param aValue    the returned value of the attribute, ownership is transferred to caller
     * @param aFlag     used to return the flag, e.g. KFlagNonUserModifiable
     *
     * @return          the value of the attribute
     *
     * @throws KErrNotFound if attribute does exist
     *
     * [Ownership of the returned object is transferred to the caller].
     *
     */
    virtual HBufC* GetL(const TDesC& aName, TUint32& aFlag) = 0;

    /**
     * Update the value of an existing custom attribute for the MIDlet suite.
     *
     * @param aName     the name of the attribute
     * @param aValue    the value of the attribute
     * @param aFlag     flag, e.g. KFlagNonUserModifiable
     */
    virtual void UpdateL(const TDesC& aName, const TDesC& aValue, const TUint32 aFlag) = 0;

    /**
     * Removes all custom attributes of the MIDlet suite.
     * Call only when removing MIDlet suite.
     */
    virtual void RemoveAllL() = 0;

};

inline MMIDletSuiteCustomAttributes::~MMIDletSuiteCustomAttributes()
{
}

} // end of namespace MIDP

#endif // MMIDLETSUITECUSTOMATTRIBUTES_H
