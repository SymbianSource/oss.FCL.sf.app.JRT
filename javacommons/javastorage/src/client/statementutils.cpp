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


#include "javacommonutils.h"
#include "logger.h"
#include "statementutils.h"

using namespace java::storage;
using namespace java::util;
using namespace std;

StatementUtils::StatementUtils()
{
}

StatementUtils::~StatementUtils()
{
}

void StatementUtils::createWhereStatement(
    const JavaStorageApplicationEntry_t& aEntry,
    wstring& aStatement)
{
    aStatement.append(L" WHERE ");

    wstring insertion = L" AND ";
    JavaStorageApplicationEntry_t::const_iterator iter;
    for (iter = aEntry.begin(); iter != aEntry.end(); iter++)
    {
        aStatement.append((*iter).entryName()).append(L"=");
        addValueByType(aStatement, (*iter));
        appendCond(aStatement, insertion, iter != --(aEntry.end()));
    }
}

void StatementUtils::appendCond(
    wstring& aStatement, const wstring& aValue, bool aCond)
{
    if (aCond)
    {
        aStatement.append(aValue);
    }
}

void StatementUtils::createWriteStatement(
    const JavaStorageApplicationEntry_t& aEntry,
    const string& aTableName,
    const wstring& aInsertion,
    wstring& aStatement)
{
    aStatement.append(L"INSERT INTO ")
    .append(JavaCommonUtils::utf8ToWstring(aTableName.c_str()))
    .append(L"(");

    JavaStorageApplicationEntry_t::const_iterator iter;

    for (iter = aEntry.begin(); iter != aEntry.end(); iter++)
    {
        aStatement.append((*iter).entryName());
        appendCond(aStatement, aInsertion, iter != --(aEntry.end()));
    }

    aStatement.append(L") VALUES (");

    for (iter = aEntry.begin(); iter != aEntry.end(); iter++)
    {
        addValueByType(aStatement, (*iter));
        appendCond(aStatement, aInsertion, iter != --(aEntry.end()));
    }

    aStatement.append(L");");
}

void StatementUtils::createTableStatement(
    const JavaStorageApplicationEntry_t& aEntry,
    const string& aTableName,
    const wstring& aInsertion,
    wstring& aStatement)
{
    aStatement.append(L"CREATE TABLE ")
    .append(JavaCommonUtils::utf8ToWstring(aTableName.c_str()))
    .append(L"(");

    JavaStorageApplicationEntry_t::const_iterator colIter;
    for (colIter = aEntry.begin(); colIter != aEntry.end(); colIter++)
    {
        if ((*colIter).entryName() == L"")
        {
            throw JavaStorageException(EInvalidDataStructure,
                                       "Column name not defined",
                                       __FILE__, __FUNCTION__, __LINE__);
        }

        aStatement.append((*colIter).entryName());

        if (JavaStorageEntry::STRING == (*colIter).entryType())
        {
            aStatement.append(L" varchar");
        }
        else if (JavaStorageEntry::INT == (*colIter).entryType())
        {
            aStatement.append(L" int");
        }
        else
        {
            ELOG(EJavaStorage, "Unknown column type");
            throw JavaStorageException(EInvalidDataStructure,
                                       "Unknown column type",
                                       __FILE__, __FUNCTION__, __LINE__);
        }

        appendCond(aStatement, aInsertion, colIter != --(aEntry.end()));
    }

    aStatement.append(L");");
}

void StatementUtils::createSearchStatement(
    const JavaStorageApplicationEntry_t& aEntry,
    const std::string& aTableName,
    std::wstring& aStatement)
{
    wstring selectStatement;
    wstring searchPattern;

    JavaStorageApplicationEntry_t::const_iterator colIter;

    for (colIter = aEntry.begin(); colIter != aEntry.end(); colIter++)
    {
        // Value is used as search query.
        if (((*colIter).entryValue()).size() > 0)
        {
            searchPattern.append((*colIter).entryName()).append(L"=");
            addValueByType(searchPattern, (*colIter));
            searchPattern += L" AND ";
        }
        else
        {
            selectStatement.append((*colIter).entryName()).append(L", ");
        }
    }

    if (selectStatement.size() > 0)
    {
        // Remove trailing ',' string. Preserve white space
        selectStatement.erase((selectStatement.size() - 2), 1);
    }
    else
    {
        selectStatement = L"*";  // Select * search;
    }

    aStatement.append(L"SELECT ").append(selectStatement).append(L" FROM ")
    .append(JavaCommonUtils::utf8ToWstring(aTableName.c_str()));

    if (searchPattern.size() > 0)
    {
        // Remove railing ' AND' string.
        searchPattern.erase((searchPattern.size() - 5), 5);

        // Define search pattern
        aStatement.append(L" WHERE " + searchPattern + L";");
    }
    else
    {
        aStatement.append(L";");
    }
}

void StatementUtils::createUpdateStatement(
    const JavaStorageApplicationEntry_t& aUpdate,
    const JavaStorageApplicationEntry_t& aMatch,
    const std::string& aTableName,
    std::wstring& aStatement)
{
    aStatement.append(L"UPDATE ")
    .append(JavaCommonUtils::utf8ToWstring(aTableName.c_str()))
    .append(L" SET ");

    JavaStorageApplicationEntry_t::const_iterator colIter;
    wstring insertion(L", ");

    for (colIter = aUpdate.begin();
            colIter != aUpdate.end();
            colIter++)
    {
        aStatement.append((*colIter).entryName()).append(L"=");
        addValueByType(aStatement, (*colIter));
        appendCond(
            aStatement, insertion, colIter != --(aUpdate.end()));
    }

    createWhereStatement(aMatch, aStatement);
}

void StatementUtils::addValueByType(wstring& aStatement,
                                    const JavaStorageEntry& aEntry)
{
    if (aEntry.entryType() == JavaStorageEntry::STRING
            || aEntry.entryType() == JavaStorageEntry::NOT_DEFINED)
    {
        aStatement.append(L"'");
        addValue(aStatement, aEntry);
        aStatement.append(L"'");
    }
    else if (aEntry.entryType() == JavaStorageEntry::INT)
    {
        addValue(aStatement, aEntry);
    }
    else
    {
        aStatement.append(L"NULL");
    }
}

void StatementUtils::addValue(wstring& aStatement,
                              const JavaStorageEntry& aEntry)
{
    wstring val = aEntry.entryValue();
    escape(val);
    aStatement.append(val);
}

void StatementUtils::escape(wstring& aUnescaped)
{
    wstring escapedChar = L"\'";
    wstring::size_type idx = 0;
    if ((idx = aUnescaped.find(escapedChar)) != string::npos)
    {
        do
        {
            aUnescaped.insert(idx, escapedChar);
            idx += escapedChar.size() + 1;
        }
        while ((idx = aUnescaped.find(escapedChar, idx)) != string::npos);
    }
}

