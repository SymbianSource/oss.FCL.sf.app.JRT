/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef STORAGETESTUTILS_H
#define STORAGETESTUTILS_H

#include <string>

#include "javastorage.h"
#include "javastorageentry.h"

namespace java
{
namespace storage
{

class JavaStorageTestUtils
{
public:
    OS_IMPORT JavaStorageTestUtils();
    OS_IMPORT ~JavaStorageTestUtils();


    OS_IMPORT bool populate(JavaStorage& aStorage,
                            const std::string& aTableName,
                            const JavaStorageApplicationEntry_t& aRef);

    OS_IMPORT bool update(JavaStorage& aStorage,
                          const std::string& aTableName,
                          const JavaStorageApplicationEntry_t& aUpdate,
                          const JavaStorageApplicationEntry_t& aMatch);


    OS_IMPORT bool checkDefaultParams(JavaStorage& aStorage);

    OS_IMPORT bool checkParams(JavaStorage& aStorage,
                               const std::string& aTableName,
                               const JavaStorageApplicationEntry_t& aRef,
                               int aFoundApps);

    OS_IMPORT bool compareEntry(const JavaStorageEntry& aRef,
                                const JavaStorageEntry& aSource);

    OS_IMPORT bool compareEntries(const JavaStorageApplicationEntry_t& aRef,
                                  const JavaStorageApplicationEntry_t& aSource);

    OS_IMPORT bool remove(JavaStorage& aStorage,
                          const std::string& aTableName,
                          const JavaStorageApplicationEntry_t& aRef);

    OS_IMPORT void createAppDataEntry(JavaStorageApplicationEntry_t& aEntry);
    OS_IMPORT void createDefaultEntry(JavaStorageApplicationEntry_t& aEntry);

    OS_IMPORT void databasePath(std::string& aDatabasePath /*OUT*/);

    OS_IMPORT void printEntry(JavaStorageApplicationEntry_t aEntry);

};

} // end namespace storage
} // end namespace java

#endif // STORAGETESTUTILS_H

