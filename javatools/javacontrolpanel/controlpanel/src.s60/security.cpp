/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <centralrepository.h>
#include <SWInstallerInternalCRKeys.h>

#include "security.h"
#include "logger.h"
#include "javastoragenames.h"
#include "securitystoragedatadefs.h"

using namespace java::storage;
using namespace std;

_LIT(KS60SecurityPolicy,  "s60");
_LIT(KMSASecurityPolicy,  "msa");
_LIT(KATTSecurityPolicy,  "att");
// Java Security Policy Central Repository UID
static const TUid KCRUidJavaSecurity = { 0x2001B289 };
// IDs for security folder and file keys
static const TUint32 KPolicy = 0x03;
static const TUint32 KSecurityWarnings = 0x04;

const int S60_SECURITY_POLICY_ID = 1;
const int MSA_SECURITY_POLICY_ID = 2;
const int ATT_SECURITY_POLICY_ID = 3;


Security::Security()
{
    try
    {
        iStorage.reset(JavaStorage::createInstance());
        iStorage->open();
    }
    catch (JavaStorageException& e)
    {
        ELOG1(EUtils, "open storage connection failed, %s",  e.toString().c_str());
    }
}

Security::~Security()
{
}

std::wstring Security::getSecurityWarningsMode()
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, L"");
        query.insert(attr);
        attr.setEntry(SECURITY_WARNINGS, L"");
        query.insert(attr);
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
        JavaStorageApplicationList_t::const_iterator iterator;
        JavaStorageApplicationList_t domainQueryResult;
        for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
        {
            std::wstring securityWarnings = L"";
            JavaStorageApplicationEntry_t entry = (*iterator);
            findColumn(entry, SECURITY_WARNINGS, securityWarnings);
            if (securityWarnings == SECURITY_WARNINGS_USER_DEFINED_MODE)
            {
                // make them all USER
                setSecurityWarningsMode(SECURITY_WARNINGS_USER_DEFINED_MODE);
                return SECURITY_WARNINGS_USER_DEFINED_MODE;
            }
        }
    }
    catch (JavaStorageException& aJse)
    {
    }
    // one more check: central repository
    std::wstring securityWarningsMode = SECURITY_WARNINGS_DEFAULT_MODE;
    TRAP_IGNORE
    (
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        TInt warningsMode;
        User::LeaveIfError(repository->Get(KSecurityWarnings, warningsMode));
        switch (warningsMode)
        {
        case 1:
            securityWarningsMode = SECURITY_WARNINGS_USER_DEFINED_MODE;
            break;
        case 2:
            securityWarningsMode = SECURITY_WARNINGS_DEFAULT_MODE;
            break;
        }
        CleanupStack::PopAndDestroy(repository);
    );
    return securityWarningsMode;
}

void Security::setSecurityWarningsMode(const std::wstring& aSecurityWarningsMode)
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, L"");
        query.insert(attr);
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
        JavaStorageApplicationList_t::const_iterator iterator;
        JavaStorageApplicationList_t domainQueryResult;
        for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
        {
            std::wstring uid = L"";
            JavaStorageApplicationEntry_t entry = (*iterator);
            findColumn(entry, ID, uid);
            JavaStorageApplicationEntry_t oldEntry;
            JavaStorageEntry oldAttr;
            oldAttr.setEntry(ID, uid);
            oldEntry.insert(oldAttr);
            attr.setEntry(SECURITY_WARNINGS, aSecurityWarningsMode);
            entry.clear();
            entry.insert(attr);
            iStorage->update(MIDP_PACKAGE_TABLE, entry, oldEntry);
        }
    }
    catch (JavaStorageException& aJse)
    {
    }
    // set the cenRep key as well
    TRAP_IGNORE
    (
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        if (aSecurityWarningsMode == SECURITY_WARNINGS_USER_DEFINED_MODE)
        {
            repository->Set(KSecurityWarnings, 1);
        }
        else
        {
            repository->Set(KSecurityWarnings, 2);
        }
        CleanupStack::PopAndDestroy(repository);
    );
}

void Security::findColumn(const JavaStorageApplicationEntry_t& aEntry,
                           const wstring& aColName, wstring& aColValue)
{
    JavaStorageEntry findPattern;
    findPattern.setEntry(aColName, L"");
    JavaStorageApplicationEntry_t::const_iterator findIterator =
        aEntry.find(findPattern);
    if (findIterator != aEntry.end())
    {
        aColValue = findIterator->entryValue();
    }
}

int Security::getSecurityPolicy()
{
    int securityPolicyId = S60_SECURITY_POLICY_ID;
    TRAP_IGNORE
    (
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        HBufC* buf = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
        TPtr policy(buf->Des());
        User::LeaveIfError(repository->Get(KPolicy, policy));
        if (policy.Compare(KS60SecurityPolicy) == 0)
        {
            securityPolicyId = S60_SECURITY_POLICY_ID;
        }
        else if (policy.Compare(KMSASecurityPolicy) == 0)
        {
            securityPolicyId = MSA_SECURITY_POLICY_ID;
        }
        else if (policy.Compare(KATTSecurityPolicy) == 0)
        {
            securityPolicyId = ATT_SECURITY_POLICY_ID;
        }
        CleanupStack::PopAndDestroy(buf);
        CleanupStack::PopAndDestroy(repository);
    );
    return securityPolicyId;

}

void Security::setSecurityPolicy(int aSecurityPolicyId)
{
    TRAP_IGNORE
    (
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        switch (aSecurityPolicyId)
        {
        case S60_SECURITY_POLICY_ID:
            User::LeaveIfError(repository->Set(KPolicy, KS60SecurityPolicy));
            break;
        case MSA_SECURITY_POLICY_ID:
            User::LeaveIfError(repository->Set(KPolicy, KMSASecurityPolicy));
            break;
        case ATT_SECURITY_POLICY_ID:
            User::LeaveIfError(repository->Set(KPolicy, KATTSecurityPolicy));
            break;
        }
    CleanupStack::PopAndDestroy(repository);
    );
}


