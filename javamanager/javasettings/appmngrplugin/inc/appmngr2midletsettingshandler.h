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
* Description:  Defines class for handling SNAP selection and read/write functions
*
*/


#ifndef APPMNGR2MIDLETSETTINGSHANDLER_H
#define APPMNGR2MIDLETSETTINGSHANDLER_H

// INCLUDES
#include <vector>
#include <string>
#include <memory>

#include "javastorage.h"
#include "javauid.h"                            // Uid.
#include "appmngr2midletsettingsutil.h"

// FORWARD DECLARATIONS
// CONSTANTS

// CLASS DECLARATION
class CAppMngr2MidletResourceHandler;

/**
*  CAppMngr2MidletSettingsHandler
*
*  CAppMngr2MidletSettingsHandler is used for getting/setting midlet suite settings
*  (access point, on screen keypad and permissions)
*
*  @since S60 v9.2
*
*/
class CAppMngr2MidletSettingsHandler : public CBase
{
public:

    /**
    * Two-phased constructor.
    */
    static CAppMngr2MidletSettingsHandler* NewL(const TDesC& aMidletSuiteName,TUid aMidletSuiteUid, CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
    * Destructor.
    */
    ~CAppMngr2MidletSettingsHandler();

private:
    /**
    * C++ default constructor.
    */
    CAppMngr2MidletSettingsHandler(CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(const TDesC& aMidletSuiteName,TUid aMidletSuiteUid);

public:

    /**
     * Gets destination network info.
     * @return Destination network name and id
     */
    CAppMngr2SuiteSnapItem* GetSnapL();

    /**
     * Set destination network to store.
     * @param aItem  Includes destination network name and id
     */
    void SetSnapL(const CAppMngr2SuiteSnapItem& aItem);

    /**
     * Gets on-screen keyboard custom attribute for the selected MIDlet
     * @return Value of the on-screen attribute (ON_SCREEN_KEYPAD_VALUE_NO,
     *                ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS, or
     *                ON_SCREEN_KEYPAD_VALUE_NAVIGATION)
     * @see appmngr2midletconstants.h
     */
    const std::wstring GetOnScreenKeypadValueL();

    /**
     * Sets on-screen keyboard custom attribute for the selected MIDlet
     * @param aValue  Value of the on-screen attribute (ON_SCREEN_KEYPAD_VALUE_NO,
     *                ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS, or
     *                ON_SCREEN_KEYPAD_VALUE_NAVIGATION)
     * @return KErrNone or KErrNotFound
     * @see appmngr2midletconstants.h
    */
    int SetOnScreenKeypadValueL(const std::wstring& aValue);

    /**
    * Checks if the on-screen keyboard custom attribute was defined by
    * the selected MIDlet JAD
    *
    * @return true if the MIdlet suite defines the on-screen keyboard
    *         attribute, false otherwise
    */
    bool OnScreenKeypadValuePreDefinedL();

    /**
     * Gets security domain category
     * @return One of the values:
     *         ApplicationInfo::MANUFACTURER_DOMAIN
     *         ApplicationInfo::IDENTIFIED_THIRD_PARTY_DOMAIN
     *         ApplicationInfo::OPERATOR_DOMAIN
     *         ApplicationInfo::UNIDENTIFIED_THIRD_PARTY_DOMAIN
     */
    const std::wstring GetSecurityDomainCategory();

    /**
     * Gets security domain name
     */
    const std::wstring GetSecurityDomainName();

    /**
     * Gets security warnings mode value
     * @return One of the values (defined in securitystoragedatadefs.h):
     *         SECURITY_WARNINGS_USER_DEFINED_MODE
     *         SECURITY_WARNINGS_DEFAULT_MODE
     */
    const std::wstring GetSecurityWarningsMode();

    /**
     * Sets security warnings mode value
     * @param aSecurityWarningsMode One of the values (defined in securitystoragedatadefs.h):
     *        SECURITY_WARNINGS_USER_DEFINED_MODE
     *        SECURITY_WARNINGS_DEFAULT_MODE
     */
    int SetSecurityWarningsMode(const std::wstring& aSecurityWarningsMode);

    /**
     * Retrieves all the security settings (function groups and their settings)
     */
    void GetSecuritySettings(std::vector<MidletSuiteSecuritySettings>&  aMidletSuiteSecuritySettings);

    /**
     * Stores the settings for a single function group
     * @param aSettingsName Identifier of the function group
     * @param aCurrentInteractionMode the settings for the function group
     */
    int SetSecuritySettings(const std::wstring& aSettingsName, const std::wstring& aCurrentInteractionMode);

    /**
     * Sets the flag which indicates if the security prompt was shown in blanket mode
     * (this is related to the definition of blanket, as an interaction mode which guarantes a single prompt)
     * @param aSettingsName Identifier of the function group
     * @param aPromptFlag the flag which indicates if the
     *        security prompt was shown in blanket mode
     */
    int SetUserSecuritySettingsPromptFlag(const std::wstring& aSettingsName, bool aPromptFlag);

private:

    void findColumn(const java::storage::JavaStorageApplicationEntry_t& aEntry, const std::wstring& aColName, std::wstring& aColValue);
    void findEntry(const java::storage::JavaStorageApplicationList_t&, const std::wstring&, std::wstring& eValue);

private:

    std::auto_ptr<java::storage::JavaStorage> iStorage;
    HBufC* iMidletSuiteName;
    /**
     * MIDlet suite UID
     */
    java::util::Uid iMidletSuiteUid;
    CAppMngr2MidletResourceHandler& iResourceHandler;
};

#endif // APPMNGR2MIDLETSETTINGSHANDLER_H


// End of file
