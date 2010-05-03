/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JavaStorageEntry
*
*/


#ifndef JAVASTORAGEENTRY_H
#define JAVASTORAGEENTRY_H

#include <stdio.h>
#include <string>
#include "javaosheaders.h"

namespace java
{
namespace storage
{

/**
 * JavaStorageEntry is object to hold JavaStorage data. One entry represents
 * one column data. These entries are passed and received from JavaStorage
 * to store and read data.<br>
 * Usage instructions:
 * @code
 *    #include "javastoragenames.h"
 *    ...
 *    JavaStorageApplicationEntry_t entries;
 *    JavaStorageEntry attribute;
 *
 *    attribute.setEntry(PACKAGE_NAME, L"MyApplicationName");
 *    entries.insert(attribute);
 *
 *    attribute.setEntry(VERSION, L"1.0.2");
 *    entries.insert(attribute);
 * @endcode
 */
class JavaStorageEntry
{
public:

    /**
     * JavaStorageEntry column type. Type is used when new storage tables
     * or columns are created.
     */
    enum JavaEntryColumnType { NOT_DEFINED = 1, STRING, INT, NULL_TYPE };

    /**
     * Set JavaStorageEntry.
     *
     * @param aName Entry name.
     * @param aValue Entry value.
     * @param aType Entry type. Default type is
     *              JavaEntryColumnType::NOT_DEFINED.
     */
    OS_IMPORT void setEntry(const std::wstring& aName,
                            const std::wstring& aValue,
                            JavaEntryColumnType aType = NOT_DEFINED);

    /**
     * Get entry type.
     *
     * @return Entry name or empty string if not set.
     */
    OS_IMPORT std::wstring entryName() const;

    /**
     * Get entry value.
     *
     * @return Entry value or empty string if not set.
     */
    OS_IMPORT std::wstring entryValue() const;

    /**
     * Get entry type.
     *
     * @return Entry type or NOT_DEFINED if not used.
     */
    OS_IMPORT JavaEntryColumnType entryType() const;

    /**
     * Operator for inserting entries at correct order. Entries are ordered
     * by their name. If name equals then they are ordered also by value.
     * However if value is empty comparison is done only by name. This is
     * for the case value is not known.
     *
     * @param aJse Entry to be compared.
     * @return true if entry must be placed earlier.
     */
    OS_IMPORT bool operator< (const JavaStorageEntry& aJse) const;

private:
    std::wstring mName;
    std::wstring mValue;
    JavaEntryColumnType mType;
};

}    // java
}    // storage

#endif // JAVASTORAGEENTRY_H

