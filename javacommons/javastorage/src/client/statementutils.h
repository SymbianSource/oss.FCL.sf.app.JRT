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
* Description:  StatementUtils
*
*/


#ifndef STATEMENTUTILS_H
#define STATEMENTUTILS_H

#include <string>
#include "javastorage.h"

namespace java
{
namespace storage
{

class StatementUtils
{
public:

    StatementUtils();
    virtual ~StatementUtils();

    /**
     * Add value to statement.
     *
     * @param[out] aStatement where value is added.
     * @param aEntry entry where value is read.
     */
    void addValue(std::wstring& aStatement,
                  const JavaStorageEntry& aEntry);

    /**
     * Add according to its type.
     *
     * @param[out] aStatement where value is added.
     * @param aEntry entry where value is read.
     */
    void addValueByType(std::wstring& aStatement,
                        const JavaStorageEntry& aEntry);

    /**
     * Escape unescaped characters. Currently only escaped char is \'.
     * Unescape is not needed as storage does that automatically.
     *
     * @param aUnescaped value to be escaped.
     */
    void escape(std::wstring& aUnescaped);

    /**
     * Append value to statement if condition fulfills.
     *
     * @param aStatement where to append.
     * @param aValue to be appended.
     * @param aCond if condition is true value is appended to statement.
     */
    void appendCond(
        std::wstring& aStatement, const std::wstring& aValue, bool aCond);

    /**
     * Create where statement from the entry.
     *
     * @param aEntry entry where statement is read.
     * @param[out] aStatement where statement is added.
     */
    void createWhereStatement(const JavaStorageApplicationEntry_t& aEntry,
                              std::wstring& aStatement);


    void createWriteStatement(const JavaStorageApplicationEntry_t& aEntry,
                              const std::string& aTableName,
                              const std::wstring& aInsertion,
                              std::wstring& aStatement);

    void createTableStatement(const JavaStorageApplicationEntry_t& aEntry,
                              const std::string& aTableName,
                              const std::wstring& aInsertion,
                              std::wstring& aStatement);

    void createSearchStatement(const JavaStorageApplicationEntry_t& aEntry,
                               const std::string& aTableName,
                               std::wstring& aStatement);

    void createUpdateStatement(const JavaStorageApplicationEntry_t& aUpdate,
                               const JavaStorageApplicationEntry_t& aMatch,
                               const std::string& aTableName,
                               std::wstring& aStatement);
};

} //end namespace storage
} //end namespace java

#endif // STATEMENTUTILS_H

