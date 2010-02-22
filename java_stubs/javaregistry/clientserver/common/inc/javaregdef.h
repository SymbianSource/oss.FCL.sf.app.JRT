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
* Description:  javaregdef definition
*
*/


#ifndef JAVAREGDEF_H
#define JAVAREGDEF_H

#include <e32std.h>

namespace Java
{
namespace Manager
{
namespace Registry
{

// name of java registry server
_LIT(KJavaRegServerName, "!javaregistry");

// the executable name of java registry server
_LIT(KJavaRegServerExeName, "javaregistry.exe");

// Function ids of client requests
enum TRequests
{
    EInitializeServerUseIntegr,
    EEntryExists,
    EGetEntryUids,
    EGetEntry,
    EGetEntryFromAll,
    EFindEntries,
    EFindAllEntries,
    EInitializeServerTransId,
    ESetEntry,
    ERemoveEntry,
    ERemoveEntryFromAll,
    EAllocateUids,
    ESetBackupState
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGDEF_H

