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
* Description:  Location API Panic description and codes
 *
*/


#ifndef LAPIPANICS_H
#define LAPIPANICS_H

#ifdef _DEBUG

// EXTERNAL INCLUDES
#include <e32std.h>

/**
 * Panic codes for Location API
 */
enum TLAPIPanicCode
{
    // Indicates that the passed argument is null
    ELAPIPanicNullArgument,
    // Indicates that the name of the landmarks is null
    ELAPIPanicNullLandmarkName,
    // Indicates that the passed array had insufficient number of elements
    ELAPIPanicInvalidAddressInfoCount,
    // Indicates that the passed array was too short
    ELAPIPanicJniIncorrectArrayLength,
    // Indicates that the passed id is not valid
    ELAPIPanicInvalidFieldId,
    // Indicates that the passed landmark id is incorrect
    ELAPIPanicInvalidLandmarkId
};

/**
 * Location API internal panic tool. Note that this tool must only
 * be used in debug builds
 */
namespace LAPIError
{
/// Panic category for Location API Panics
_LIT(KPanicCategory, "java-location");

/**
 * Called if an unrecoverable error is detected.
 * Stops the thread with User::Panic.
 */
inline void Panic(TLAPIPanicCode aPanicCode);
}

inline void LAPIError::Panic(TLAPIPanicCode aPanicCode)
{
    User::Panic(KPanicCategory, aPanicCode);
}

#endif // _DEBUG
#endif // LAPIPANICS_H
// End of file
