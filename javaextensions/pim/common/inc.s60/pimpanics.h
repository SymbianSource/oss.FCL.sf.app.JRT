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
* Description:  PIM panic numbers.
 *
*/


#ifndef PIMPANICS_H
#define PIMPANICS_H

/** @file
 * This file contains panic numbers for the PIM implementation.
 */

/**
 * PIM panic category. Used as the first parameter in User::Panic().
 */
_LIT(KPIMPanicCategory, "JAVA-PIM");

/**
 * PIM panic numbers and descriptions.
 */
typedef enum
{
    EPIMPanicGeneral = 0,
    /**< 0 - General PIM panic; More descriptive panics should be used
     instead of this. */

    EPIMPanicListCorrupt = 1,
    /**< 1 - A list has become corrupt but it was not noticed at the
     time it happened. */

    EPIMPanicEventItemsSearchType = 2,
    /**< 2 - Invalid Event item search type for Event enumeration. */

    EPIMPanicInvalidNativeChangeType = 3,
    /**< 3 - Invalid native-originated change type. */

    EPIMPanicInvalidItemID = 4,
    /**< 4 - Item ID is not KPIMNullItemID */

    EPIMPanicUnsupportedField = 5,
    /**< 5 - Item contains unsupported fields */

    EPIMPanicUnsupportedDateField = 6,
    /**< 6 - Item contains unsupported date fields */

    EPIMPanicUnsupportedStringField = 7,
    /**< 7 - Item contains unsupported string fields */

    EPIMPanicInvalidNativeChangeClass = 8,
    /**< 8 - Invalid change class for native-originated changes. */

    EPIMPanicUnexpectedNullExternalChangeList = 9,
    /**< 9 - List of external changes is NULL when non-NULL list was
     expected. */

    EPIMPanicExternalChangeNewExpected = 10,
    /**< 10 - EPIMExternalChangeNew type change expected while something
     else got. */

    EPIMPanicExternalChangeUpdatingNonExistentItem = 11,
    /**< 11 - An adapter indicated that an item should be updated but
     the item could not be found. */

    EPIMPanicArgumentNotZero = 12,
    /**< 12 - Localization Manager is asked for a subtyped data when the
     type does not support subtypes. */

    EPIMPanicNullArgument = 13,
    /**< 13 - NULL argument; generic panic for catching illegal NULL
     arguments. */

    EPIMPanicInvalidState = 14,
    /**< 14 - Invalid state. */

    EPIMPanicCommittedExists = 15,
    /**< 15 - An item, supposedly committed the first time after its
     creation, already exists in the list. */

    EPIMPanicInvalidInOutArg = 16,
    /**< 16 - Invalid argument for passing the results to the caller. */

    EPIMPanicBase64Error = 17,
    /**< 17 - Base64 encoding or decoding -related error. */

    EPIMPanicUnsupportedIntField = 18,
    /**< 18 - Item contains unsupported int fields. */

    EPIMPanicInvalidListType = 19,
    /**< 19 - Event localization data has invalid list type. */

    EPIMPanicInvalidFieldType = 20,
    /**< 20 - An invalid field type emerged. */

    EPIMPanicExternalChangeInit = 21,
    /**< 21 - An error in external change item initialization. */

    EPIMPanicTargetDesTooShort = 22,
    /**< 22 - Given target descriptor is too short. */

    EPIMPanicJniErrorArrayTooShort = 23,
    /**< 23 - Given JNI error in-out array is too short. */

    EPIMPanicJniInvalidLabelSpec = 24,
    /**< 24 - Given combination of field, attribute and array
     element is invalid. */

    EPIMPanicInvalidLabel = 25,
    /**< 25 - Given label is somehow errornous */
} TPIMPanic;

#endif // PIMPANICS_H
// End of File
