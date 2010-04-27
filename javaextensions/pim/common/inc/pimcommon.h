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
* Description:  PIM API common types and constants
 *
*/


#ifndef PIMCOMMON_H
#define PIMCOMMON_H

/** @file
 * This file contains common definitions and constants for the PIM API
 * implementation generated from the Java side.
 */

#include "pimtypes.h"

/**
 * Field data type constants.
 * EPIMFieldInvalid added for native side implementation.
 * [From javax.microedition.pim.PIMItem]
 */
enum TPIMFieldDataType
{
    EPIMFieldInvalid = -1,
    EPIMFieldBinary = 0,
    EPIMFieldBoolean = 1,
    EPIMFieldDate = 2,
    EPIMFieldInt = 3,
    EPIMFieldString = 4,
    EPIMFieldStringArray = 5
};

enum TPIMItemSelect
{
    EPIMItemAll,
    EPIMItemMatchingItem,
    EPIMItemMatchingString,
    EPIMItemMatchingCategory,
    EPIMItemCorrupt
};

/**
 * Field constant and type compound.
 */
struct TPIMFieldAndType
{
public:

    TPIMFieldAndType() :
            iField(0), iType(EPIMFieldInvalid)
    {
    }

    TPIMFieldAndType(TPIMField aField, TPIMFieldDataType aType) :
            iField(aField), iType(aType)
    {
    }

    TPIMField iField;
    TPIMFieldDataType iType;
};

/**
 * Attribute constant indicating "no attributes".
 * [From javax.microedition.pim.PIMItem]
 */
const TPIMAttribute KPIMAttrNone = 0;

/**
 * Extended field minimum value.
 * [From javax.microedition.pim.PIMItem]
 */
const TPIMField KPIMExtendedFieldMinValue = 16777216;

/**
 * Extended attribute minimum value.
 * [From javax.microedition.pim.PIMItem]
 */
const TPIMField KPIMExtendedAttributeMinValue = 16777216;

/**
 * PIMException reason codes.
 * [From javax.microedition.pim.PIMException]
 */
enum TPIMExceptionReasonCode
{
    EPIMFeatureNotSupported = 0,
    EPIMGeneralError = 1,
    EPIMListClosed = 2,
    EPIMListNotAccessible = 3,
    EPIMMaxCategoriesExceeded = 4,
    EPIMUnsupportedVersion = 5,
    EPIMUpdateError = 6
};

/**
 * PIM list types.
 * [From javax.microedition.pim.PIM]
 */
enum TPIMListType
{
    EPIMContactList = 1,
    EPIMEventList = 2,
    EPIMToDoList = 3
};

/**
 * PIM list modes.
 * [From javax.microedition.pim.PIM]
 */
enum TPIMListMode
{
    EPIMReadOnly = 1,
    EPIMWriteOnly = 2,
    EPIMReadWrite = 3
};

/**
 * MIDlet security domains. Corresponding values from
 * the Java-side are found from PIMManager
 */
enum TPIMSecurityDomain
{
    EPIMSecurityDomainUntrusted = 0,
    EPIMSecurityDomainTTP = 1,
    EPIMSecurityDomainOperator = 2,
    EPIMSecurityDomainManufacturer = 3
};

#endif // PIMCOMMON_H
