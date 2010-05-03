/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  midpregistrypropertyids definition
*
*/


#include "javaregistrypropertyids.h"

#ifndef MIDPREGISTRYPROPERTYIDS_H
#define MIDPREGISTRYPROPERTYIDS_H

namespace Java
{
namespace Manager
{
namespace Registry
{

/**
 * MIDletSuite specific Id's of properties processed by
 * CWriteableJavaRegistryMidletSuiteEntry class.
 *
 * @since S60 v3.2
 */
enum TMidletSuiteEntryIDs
{
    EPropJadPath = KMidletSuiteRangeStart,
    EPropJadUrl,
    EPropJarPath,
    EPropJarUrl,
    EPropFiles,
    EPropDirectories,
    EPropApparcIdentifiers,
    EPropLegacyDump,
    EPropSystemAmsId,
    EPropNokiaBlockUninstall
};

/**
 * MIDlet specific Id's of properties processed by
 * CWriteableJavaRegistryMidletEntry class.
 *
 * @since S60 v3.2
 */
enum TMidletEntryIDs
{
};

}// namespace Registry
}// namespace Manager
}// namespace Java


#endif // MIDPREGISTRYPROPERTYIDS_H
