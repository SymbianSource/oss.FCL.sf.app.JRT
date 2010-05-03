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
* Description:  CAppMngr2MidletSettingsHandler implementation
*
*/


// INCLUDE FILES
#include <featmgr.h>
#include <StringLoader.h>                       // StringLoader
#include <cmmanagerext.h>                       // RCmManagerExt
#include <cmdestinationext.h>                   // RCmDestinationExt
#include "appmngr2midletsettingshandler.h"
#include "appmngr2midletconstants.h"
#include "securitystoragedatadefs.h"

#include <javaapplicationsettings.rsg>                   // Midlet resource IDs

#include "connectionmanager.h"                  // ConnectionManager.
#include "javastoragenames.h"                   // Storage constants.
#include "javasymbianoslayer.h"                 // STL <-- --> Symbian conv.
#include "javauid.h"                            // Uid.
#include "logger.h"                             // LOG.
#include "appmngr2midletresourcehandler.h" // CAppMngr2MidletResourceHandler

using namespace java::storage;
using namespace std;
using namespace java::util;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CAppMngr2MidletSettingsHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAppMngr2MidletSettingsHandler* CAppMngr2MidletSettingsHandler::NewL(const TDesC& aMidletSuiteName,TUid aMidletSuiteUid, CAppMngr2MidletResourceHandler& aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsHandler::NewL ");
    CAppMngr2MidletSettingsHandler* self
    = new(ELeave) CAppMngr2MidletSettingsHandler(aResourceHandler);
    CleanupStack::PushL(self);
    self->ConstructL(aMidletSuiteName,aMidletSuiteUid);
    CleanupStack::Pop(self);

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::NewL ");

    return self;
}

// -----------------------------------------------------------------------------
// Destructor
//
// Deletes database and ApUtils and cleans and deletes Midlet User Preferences.
// -----------------------------------------------------------------------------
//
CAppMngr2MidletSettingsHandler::~CAppMngr2MidletSettingsHandler()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsHandler::~CAppMngr2MidletSettingsHandler ");

    delete iMidletSuiteName;
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::~CAppMngr2MidletSettingsHandler ");
}

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
CAppMngr2MidletSettingsHandler::CAppMngr2MidletSettingsHandler(CAppMngr2MidletResourceHandler& aResourceHandler)
        : iMidletSuiteName(NULL), iResourceHandler(aResourceHandler)
{
}

// -----------------------------------------------------------------------------
// CAppMngr2MidletSettingsHandler::ConstructL
//
// Symbian OS two phased constructor
// -----------------------------------------------------------------------------
//
void CAppMngr2MidletSettingsHandler::ConstructL(const TDesC& aMidletSuiteName,TUid aMidletSuiteUid)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsHandler::ConstructL ");

    iStorage.reset(JavaStorage::createInstance());

    try
    {
        iStorage->open();
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaAppMngrPlugin, "SettingsHandler: Cannot open storage: %d",
              aJse.mStatus);
        User::Leave(aJse.mStatus);
    }
    iMidletSuiteName = aMidletSuiteName.AllocL();
    TUidToUid(aMidletSuiteUid, iMidletSuiteUid);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::ConstructL ");
}

// -----------------------------------------------------------------------------
// CAppMngr2MidletSettingsHandler::GetCurrentSuiteSnapL
//
// Reads the current Midlet destination network from DB.
// -----------------------------------------------------------------------------
//
CAppMngr2SuiteSnapItem* CAppMngr2MidletSettingsHandler::GetSnapL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsHandler::GetCurrentSuiteSnapL ");

    CAppMngr2SuiteSnapItem* snap = new(ELeave) CAppMngr2SuiteSnapItem;
    unsigned int apnId = ConnectionManager::getDestinationNetworkIdL(iMidletSuiteUid);
    snap->iId = apnId;

    HBufC* name = NULL;
    TInt resourceOffset = iResourceHandler.AddResourceFileL();
    switch (apnId)
    {
    case(TUint)KAlwaysAskId:
    {
        name = StringLoader::LoadL(R_JAVA_CONSET_SETT_ALWAYS_ASK);
        break;
    }
    case(TUint)KJavaNetworkAccessNotSpecified:
    case(TUint)KDefaultConnection:
    {
        name = StringLoader::LoadL(R_JAVA_CONSET_SETT_DEFAULT_CONNECTION);
        break;
    }
    default:
    {
        RCmManagerExt manager;
        manager.OpenLC();
        RCmDestinationExt destination = manager.DestinationL(apnId);
        CleanupClosePushL(destination);
        name = destination.NameLC();
        CleanupStack::Pop(name);
        CleanupStack::PopAndDestroy(&destination);
        CleanupStack::PopAndDestroy(&manager);
    }
    }
    if (resourceOffset > 0)
    {
        iResourceHandler.DeleteResourceFile(resourceOffset);
    }
    snap->iName = wstring((const wchar_t*)name->Ptr(), name->Length());
    delete name;
    name = NULL;

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::GetCurrentSuiteSnapL ");

    return snap;
}


// ---------------------------------------------------------------------------
// CAppMngr2MidletSettingsHandler::GetOnScreenKeyboardValueL()
//
// Reads custom attribute value KOnScreenKeyboardAttribute
// ---------------------------------------------------------------------------
//
const std::wstring CAppMngr2MidletSettingsHandler::GetOnScreenKeypadValueL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsHandler::GetOnScreenKeyboardValueL ");

    wstring value = L"";

    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, iMidletSuiteUid.toString());
    query.insert(attr);
    attr.setEntry(ON_SCREEN_KEYPAD, L"");
    query.insert(attr);

    try
    {
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaAppMngrPlugin, "ON_SCREEN_KEYPAD value read failed: %d", aJse.mStatus);
        // Don't leave. Set defaults.
    }

    findEntry(queryResult, ON_SCREEN_KEYPAD, value);

    if (value.size() <= 0)
    {
        // default is KValueGameactions if value is not defined
        value = ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS;
    }
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::GetOnScreenKeyboardValueL ");

    return value;
}

bool CAppMngr2MidletSettingsHandler::OnScreenKeypadValuePreDefinedL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsHandler::OnScreenKeypadValuePreDefinedL ");

    wstring value = L"";
    bool predefined = false;

    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, iMidletSuiteUid.toString());
    query.insert(attr);
    attr.setEntry(NAME, L"Nokia-MIDlet-On-Screen-Keypad");
    query.insert(attr);
    attr.setEntry(VALUE, L"");
    query.insert(attr);

    try
    {
        iStorage->search(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, query, queryResult);
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaAppMngrPlugin, "Nokia-MIDlet-On-Screen-Keypad value read failed: %d", aJse.mStatus);
        // Don't leave. Set defaults.
    }

    findEntry(queryResult, VALUE, value);

    if (value.size() > 0)
    {
        // default is KValueGameactions if value is not defined
        predefined = true;
    }
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::OnScreenKeypadValuePreDefinedL ");

    return predefined;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletSettingsHandler::SetOnScreenKeyboardValueL()
//
// Writes custom attribute value KOnScreenKeyboardAttribute
// ---------------------------------------------------------------------------
//
int CAppMngr2MidletSettingsHandler::SetOnScreenKeypadValueL(const std::wstring& aValue)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsHandler::SetOnScreenKeyboardValueL ");

    JavaStorageApplicationEntry_t oldEntry;
    JavaStorageEntry attr;
    attr.setEntry(ID, iMidletSuiteUid.toString());
    oldEntry.insert(attr);
    JavaStorageApplicationEntry_t entry;
    attr.setEntry(ON_SCREEN_KEYPAD, aValue);
    entry.insert(attr);

    try
    {
        iStorage->update(MIDP_PACKAGE_TABLE, entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaAppMngrPlugin, "Failed to set ON_SCREEN_KEYPAD: %d", aJse.mStatus);
        return KErrNotFound;
    }

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::SetOnScreenKeyboardValueL ");

    return KErrNone;

}


// -----------------------------------------------------------------------------
// CAppMngr2MidletSettingsHandler::SetCurrentSuiteSnapL
//
// Set selected access point to DB
// -----------------------------------------------------------------------------
//
void CAppMngr2MidletSettingsHandler::SetSnapL(const CAppMngr2SuiteSnapItem& aSnap)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsHandler::SetCurrentSuiteSnapL ");

    ConnectionManager::setDestinationNetworkIdL(iMidletSuiteUid, aSnap.iId);

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsHandler::SetCurrentSuiteSnapL ");
}

const std::wstring CAppMngr2MidletSettingsHandler::GetSecurityDomainCategory()
{
    wstring securityDomainCategory = L"";

    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, iMidletSuiteUid.toString());
    query.insert(attr);
    attr.setEntry(SECURITY_DOMAIN_CATEGORY, L"");
    query.insert(attr);

    try
    {
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaAppMngrPlugin, "SECURITY_DOMAIN_CATEGORY value read failed: %d", aJse.mStatus);
    }

    findEntry(queryResult, SECURITY_DOMAIN_CATEGORY, securityDomainCategory);

    return securityDomainCategory;

}

const std::wstring CAppMngr2MidletSettingsHandler::GetSecurityDomainName()
{
    wstring securityDomainName = L"";

    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, iMidletSuiteUid.toString());
    query.insert(attr);
    attr.setEntry(SECURITY_DOMAIN, L"");
    query.insert(attr);

    try
    {
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaAppMngrPlugin, "SECURITY_DOMAIN value read failed: %d", aJse.mStatus);
    }

    findEntry(queryResult, SECURITY_DOMAIN, securityDomainName);

    return securityDomainName;

}

const wstring CAppMngr2MidletSettingsHandler::GetSecurityWarningsMode()
{
    wstring warningsMode = L"";

    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, iMidletSuiteUid.toString());
    query.insert(attr);
    attr.setEntry(SECURITY_WARNINGS, L"");
    query.insert(attr);

    try
    {
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaAppMngrPlugin, "ON_SCREEN_KEYPAD value read failed: %d", aJse.mStatus);
        // Don't leave. Set defaults.
    }

    findEntry(queryResult, SECURITY_WARNINGS, warningsMode);
    if (warningsMode.size() == 0)
    {
        warningsMode = SECURITY_WARNINGS_DEFAULT_MODE;
    }

    return warningsMode;
}

int CAppMngr2MidletSettingsHandler::SetSecurityWarningsMode(const wstring& aSecurityWarningsMode)
{
    JavaStorageApplicationEntry_t oldEntry;
    JavaStorageEntry attr;
    attr.setEntry(ID, iMidletSuiteUid.toString());
    oldEntry.insert(attr);
    JavaStorageApplicationEntry_t entry;
    attr.setEntry(SECURITY_WARNINGS, aSecurityWarningsMode);
    entry.insert(attr);

    try
    {
        iStorage->update(MIDP_PACKAGE_TABLE, entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaAppMngrPlugin, "Failed to set SECURITY_WARNINGS: %d", aJse.mStatus);
        return KErrNotFound;
    }
    return KErrNone;
}

void CAppMngr2MidletSettingsHandler::GetSecuritySettings(std::vector<MidletSuiteSecuritySettings>&  aMidletSuiteSecuritySettings)
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, iMidletSuiteUid.toString());
        query.insert(attr);
        attr.setEntry(FUNCTION_GROUP, L"");
        query.insert(attr);
        iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, queryResult);
        JavaStorageApplicationList_t::const_iterator iterator;
        JavaStorageApplicationList_t settingsQueryResult;
        MidletSuiteSecuritySettings settings;
        for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
        {
            std::wstring settingsName = L"";
            JavaStorageApplicationEntry_t entry = (*iterator);
            findColumn(entry, FUNCTION_GROUP, settingsName);
            entry.clear();
            query.clear();
            attr.setEntry(ID, iMidletSuiteUid.toString());
            query.insert(attr);
            attr.setEntry(FUNCTION_GROUP, settingsName);
            query.insert(attr);
            attr.setEntry(CURRENT_SETTING, L"");
            query.insert(attr);
            attr.setEntry(ALLOWED_SETTINGS , L"");
            query.insert(attr);
            settingsQueryResult.clear();
            iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, settingsQueryResult);
            if (settingsQueryResult.size() > 0)
            {
                wstring currentInteractionMode = L"";
                wstring allowedInteractionModes = L"";
                findEntry(settingsQueryResult,CURRENT_SETTING , currentInteractionMode);
                findEntry(settingsQueryResult,ALLOWED_SETTINGS , allowedInteractionModes);
                if (currentInteractionMode.size() > 0 && allowedInteractionModes.size() > 0)
                {
                    settings = MidletSuiteSecuritySettings(
                                   settingsName,
                                   currentInteractionMode,
                                   allowedInteractionModes);
                }
            }
            aMidletSuiteSecuritySettings.push_back(settings);
        }
    }
    catch (JavaStorageException& aJse)
    {
    }
}

int CAppMngr2MidletSettingsHandler::SetSecuritySettings(const std::wstring& aSettingsName, const std::wstring& aCurrentInteractionMode)
{
    try
    {
        JavaStorageApplicationEntry_t entry;
        JavaStorageApplicationEntry_t oldEntry;
        JavaStorageEntry attr;
        attr.setEntry(ID, iMidletSuiteUid.toString());
        oldEntry.insert(attr);
        attr.setEntry(FUNCTION_GROUP, aSettingsName);
        oldEntry.insert(attr);
        attr.setEntry(CURRENT_SETTING, aCurrentInteractionMode);
        entry.insert(attr);
        iStorage->update(MIDP_FUNC_GRP_SETTINGS_TABLE, entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
        return KErrNotFound;
    }
    return KErrNone;
}

int CAppMngr2MidletSettingsHandler::SetUserSecuritySettingsPromptFlag(const std::wstring& aSettingsName, bool aPromptFlag)
{
    try
    {
        JavaStorageApplicationEntry_t entry;
        JavaStorageApplicationEntry_t oldEntry;
        JavaStorageEntry attr;
        attr.setEntry(ID, iMidletSuiteUid.toString());
        oldEntry.insert(attr);
        attr.setEntry(FUNCTION_GROUP, aSettingsName);
        oldEntry.insert(attr);
        std::wstring blanketPrompt;
        if (aPromptFlag)
        {
            blanketPrompt = L"1";
        }
        else
        {
            blanketPrompt = L"0";
        }
        attr.setEntry(BLANKET_PROMPT, blanketPrompt);
        entry.insert(attr);
        iStorage->update(MIDP_FUNC_GRP_SETTINGS_TABLE, entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
        return KErrNotFound;
    }
    return KErrNone;
}

void CAppMngr2MidletSettingsHandler::findColumn(const JavaStorageApplicationEntry_t& aEntry,
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

void CAppMngr2MidletSettingsHandler::findEntry(const JavaStorageApplicationList_t& queryResult,
        const std::wstring& eName,
        std::wstring& eValue)
{
    if (queryResult.size() > 0)
    {
        JavaStorageApplicationEntry_t entry = queryResult.front();
        JavaStorageEntry findPattern;
        findPattern.setEntry(eName, L"");
        JavaStorageApplicationEntry_t::const_iterator findIterator =
            entry.find(findPattern);
        if (findIterator != entry.end())
        {
            eValue = findIterator->entryValue();
        }
    }
}

// End of file
