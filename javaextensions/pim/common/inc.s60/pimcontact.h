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
* Description:  Contact types and constants
 *
*/


#ifndef PIMCONTACT_H
#define PIMCONTACT_H

/** @file
 * This file contains the PIM API constants and definitions for Contacts.
 */

/**
 * Expresses that no preferred index has been set for a particular
 * Contact field.
 */
#define KPIMNoPreferredIndex -1

/**
 * Contact field constants.
 * [From javax.microedition.pim.Contact]
 */
enum TPIMContactField
{
    EPIMContactInvalidField = 0,
    /** Invalid contact field */
    EPIMContactAddr = 100,
    /**< \e String \e array, elements: \ref TPIMContactAddrElement. */
    EPIMContactBirthday = 101,
    /**< \e Date. */
    EPIMContactClass = 102,
    /**< \e Integer, values from \ref TPIMContactClassValue. */
    EPIMContactEmail = 103,
    /**< \e String. */
    EPIMContactFormattedAddr = 104,
    /**< \e String, may be compiled from \ref EPIMContactAddr field. */
    EPIMContactFormattedName = 105,
    /**< \e String, may be compiled from \ref EPIMContactName field. */
    EPIMContactName = 106,
    /**< \e String \e array, elements: \ref TPIMContactNameElement. */
    EPIMContactNickname = 107,
    /**< \e String. */
    EPIMContactNote = 108,
    /**< \e String. */
    EPIMContactOrg = 109,
    /**< \e String. */
    EPIMContactPhoto = 110,
    /**< \e Byte \e array, bundled with \ref EPIMContactPhotoUrl (see
     PIM API javadoc). */
    EPIMContactPhotoUrl = 111,
    /**< \e String, bundled with \ref EPIMContactPhoto (see PIM API
     javadoc). */
    EPIMContactPublicKey = 112,
    /**< \e Byte \e array, bundled with \ref
     EPIMContactPublicKeyString (see PIM API javadoc). */
    EPIMContactPublicKeyString = 113,
    /**< \e String, bundled with \ref EPIMContactPublicKey (see PIM
     API javadoc). */
    EPIMContactRevision = 114,
    /**< \e Date, read-only when persisted (the time of last modification). */
    EPIMContactTel = 115,
    /**< \e String. */
    EPIMContactTitle = 116,
    /**< \e String. */
    EPIMContactUid = 117,
    /**< \e String, read-only when persisted (converted from the Item ID). */
    EPIMContactUrl = 118,
    /**< \e String. */
    EPIMContactExtWvUserId = 0x1005001,
    /**< String. */
    EPIMContactExtSip = 0x1005002,
    /**< String. */
    EPIMContactExtDtmf = 0x1005003,
    /**< String. */
    EPIMContactExtDepartment = 0x1005004,
    /**< String */
    EPIMContactExtAssistantName = 0x1005005,
    /**< String */
    EPIMContactExtChildren = 0x1005006,
    /**< String */
    EPIMContactExtSpouse = 0x1005007,
    /**< String */
    EPIMContactExtAnniversary = 0x1005008,
    /**< Date */
    EPIMContactExtVoip = 0x1005009,
    /**< String */
    EPIMContactExtPTT = 0x100500A,
    /**< String */
    EPIMContactExtSWIS = 0x100500B
    /**< String */
};

/**
 * Contact field generic attributes and extended attributes.
 * [From javax.microedition.pim.Contact]
 */
enum TPIMContactFieldAttribute
{
    EPIMContactAttrAsst = 1,
    EPIMContactAttrAuto = 2,
    EPIMContactAttrFax = 4,
    EPIMContactAttrHome = 8,
    EPIMContactAttrMobile = 16,
    EPIMContactAttrOther = 32,
    EPIMContactAttrPager = 64,
    EPIMContactAttrPreferred = 128,
    EPIMContactAttrSms = 256,
    EPIMContactAttrWork = 512,
    EPIMContactAttrExtVideoCall = 16777216
};

/**
 * Contact \ref EPIMContactAddr field (string array) elements.
 * \ref EPIMContactFormattedAddr field is compiled from the elements.
 * [From javax.microedition.pim.Contact]
 */
enum TPIMContactAddrElement
{
    EPIMContactAddrPoBox = 0,
    EPIMContactAddrExtra = 1,
    EPIMContactAddrStreet = 2,
    EPIMContactAddrLocality = 3,
    EPIMContactAddrRegion = 4,
    EPIMContactAddrPostalCode = 5,
    EPIMContactAddrCountry = 6,
    EPIMContactAddrNumElements = 7
    /**< Index of last element + 1. */
};

/**
 * Contact \ref EPIMContactClass field values.
 * [From javax.microedition.pim.Contact]
 */
enum TPIMContactClassValue
{
    EPIMContactClassConfidential = 200,
    EPIMContactClassPrivate = 201,
    EPIMContactClassPublic = 202
};

/**
 * Contact \ref EPIMContactName field (string array) elements.
 * \ref EPIMContactFormattedName field is compiled from the elements.
 * [From javax.microedition.pim.Contact]
 */

enum TPIMContactNameElement
{
    EPIMContactNameFamily = 0,
    EPIMContactNameGiven = 1,
    EPIMContactNameOther = 2,
    EPIMContactNamePrefix = 3,
    EPIMContactNameSuffix = 4,
    EPIMContactExtFamilyNameReading = 5,
    EPIMContactExtGivenNameReading = 6,
    EPIMContactNameNumElements = 7
    /**< Index of last element + 1. */
};

#endif // PIMCONTACT_H
// End of file
