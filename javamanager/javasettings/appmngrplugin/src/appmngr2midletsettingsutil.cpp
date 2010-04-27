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
* Description:  MidletAppInfo implementation
*
*/


#include "appmngr2midletsettingsutil.h"

#include "javacommonutils.h"
#include "securitystoragedatadefs.h"
#include <StringLoader.h>                       // StringLoader
#include <javaapplicationsettings.rsg>                   // Midlet resource IDs
#include <cmmanager.rsg>                   // Midlet resource IDs
#include "connectionmanager.h"
#include <algorithm>

#include "appmngr2midletconstants.h"

const wchar_t* const ON_SCREEN_KEYPAD_SETTINGS = L"On screen keypad";
const wchar_t* const SECURITY_WARNINGS_SETTINGS = L"Security warnings";
const wchar_t* const ACCESS_POINT_SETTINGS = L"Access Point";

using namespace std;
using namespace java::util;

// ======== MEMBER FUNCTIONS ========
LocalizedString::LocalizedString(const std::wstring& aId,const std::wstring& aValue)
        : iId(aId), iValue(aValue), iIndex(-1)
{
}

LocalizedString::LocalizedString(const std::wstring& aId,const std::wstring& aValue, int aIndex)
        : iId(aId), iValue(aValue), iIndex(aIndex)
{
}

LocalizedString::LocalizedString(TUint aId,const std::wstring& aValue)
        : iValue(aValue),iIndex(-1)
{
    iId = JavaCommonUtils::intToWstring(aId);
}

LocalizedString& LocalizedString::operator=(const LocalizedString& aLocalizedString)
{
    iId = aLocalizedString.iId;
    iValue = aLocalizedString.iValue;
    iIndex = aLocalizedString.iIndex;
    return *this;
}

LocalizedString::LocalizedString(const LocalizedString& aLocalizedString)
{
    iId = aLocalizedString.iId;
    iValue = aLocalizedString.iValue;
    iIndex = aLocalizedString.iIndex;
}

LocalizedString::LocalizedString()
        : iId(L""), iValue(L""), iIndex(-1)
{
}

const std::wstring& LocalizedString::getId() const
{
    return iId;
}

int LocalizedString::getIndex() const
{
    return iIndex;
}

const std::wstring& LocalizedString::getValue() const
{
    return iValue;
}

ListItem::ListItem(const LocalizedString& aName, const vector<LocalizedString>& aPossibleValues)
        : iName(aName), iPossibleValues(aPossibleValues), iCurrentValueIndex(0), iEnabled(true), iIndex(-1)
{
}

ListItem::ListItem(const LocalizedString& aName, const vector<LocalizedString>& aPossibleValues, bool aEnabled)
        : iName(aName), iPossibleValues(aPossibleValues), iCurrentValueIndex(0), iEnabled(aEnabled), iIndex(-1)
{
}

ListItem::ListItem(const LocalizedString& aName,
                   const vector<LocalizedString>& aPossibleValues,
                   bool aEnabled,
                   int aCurrentValueIndex)
        : iName(aName), iPossibleValues(aPossibleValues), iCurrentValueIndex(aCurrentValueIndex), iEnabled(aEnabled), iIndex(-1)
{
}

ListItem::ListItem(const LocalizedString& aName,
                   const vector<LocalizedString>& aPossibleValues,
                   bool aEnabled,
                   int aCurrentValueIndex,
                   int aIndex)
        : iName(aName), iPossibleValues(aPossibleValues), iCurrentValueIndex(aCurrentValueIndex), iEnabled(aEnabled), iIndex(aIndex)
{
}

ListItem& ListItem::operator=(const ListItem& aListItem)
{
    iName = aListItem.iName;
    iPossibleValues = aListItem.iPossibleValues;
    iCurrentValueIndex = aListItem.iCurrentValueIndex;
    iEnabled = aListItem.iEnabled;
    iIndex = aListItem.iIndex;
    return *this;
}

ListItem::ListItem(const ListItem& aListItem)
{
    iName = aListItem.iName;
    iPossibleValues = aListItem.iPossibleValues;
    iCurrentValueIndex = aListItem.iCurrentValueIndex;
    iEnabled = aListItem.iEnabled;
    iIndex = aListItem.iIndex;
}

ListItem::ListItem()
        : /*iName(LocalizedString()),*/ iCurrentValueIndex(-1), iIndex(-1)
{
}

const LocalizedString& ListItem::getName() const
{
    return iName;
}

const std::vector<LocalizedString>& ListItem::getValues() const
{
    return iPossibleValues;
}

const LocalizedString& ListItem::getValue() const
{
    return iPossibleValues[iCurrentValueIndex];
}

const LocalizedString& ListItem::getValue(int aValueIndex) const
{
    return iPossibleValues[aValueIndex];
}

void ListItem::setCurrentValue(int aCurrentValueIndex)
{
    iCurrentValueIndex = aCurrentValueIndex;
}

int ListItem::getCurrentValue() const
{
    return iCurrentValueIndex;
}

int ListItem::getIndex() const
{
    return iIndex;
}

bool ListItem::getEnabled() const
{
    return iEnabled;
}

void ListItem::setEnabled(bool aEnabled)
{
    iEnabled = aEnabled;
}

MidletSuiteSecuritySettings::MidletSuiteSecuritySettings(const std::wstring& aSettingsName,
        const std::wstring& aCurrentInteractionMode,
        const std::wstring& aAllowedInteractionModes)
        : iSettingsName(aSettingsName), iCurrentInteractionMode(aCurrentInteractionMode), iAllowedInteractionModes(aAllowedInteractionModes)
{
}

MidletSuiteSecuritySettings& MidletSuiteSecuritySettings::operator=(const MidletSuiteSecuritySettings& aMidletSuiteSecuritySettings)
{
    iSettingsName = aMidletSuiteSecuritySettings.iSettingsName;
    iCurrentInteractionMode = aMidletSuiteSecuritySettings.iCurrentInteractionMode;
    iAllowedInteractionModes = aMidletSuiteSecuritySettings.iAllowedInteractionModes;
    return *this;
}

MidletSuiteSecuritySettings::MidletSuiteSecuritySettings(const MidletSuiteSecuritySettings& aMidletSuiteSecuritySettings)
{
    iSettingsName = aMidletSuiteSecuritySettings.iSettingsName;
    iCurrentInteractionMode = aMidletSuiteSecuritySettings.iCurrentInteractionMode;
    iAllowedInteractionModes = aMidletSuiteSecuritySettings.iAllowedInteractionModes;
}

MidletSuiteSecuritySettings::MidletSuiteSecuritySettings()
        : iSettingsName(L""), iCurrentInteractionMode(L""), iAllowedInteractionModes(L"")
{
}

const std::wstring MidletSuiteSecuritySettings::getName() const
{
    return iSettingsName;
}

const std::wstring MidletSuiteSecuritySettings::getCurrentInteractionMode() const
{
    return iCurrentInteractionMode;
}

const std::wstring MidletSuiteSecuritySettings::getAllowedInteractionModes() const
{
    return iAllowedInteractionModes;
}

CAppMngr2SuiteSnapItem::CAppMngr2SuiteSnapItem()
        : iId(0), iName(L"")
{
}

CAppMngr2SuiteSnapItem::CAppMngr2SuiteSnapItem(const CAppMngr2SuiteSnapItem& aSnap)
{
    iId = aSnap.iId;
    iName = aSnap.iName;
}

const ListItem AppMngr2MidletSettingsUtil::SettingsToListItem(MidletSuiteSecuritySettings aSettings, bool aSettingsEnabled)
{
    // The allowed modes info is stored as a 4-bit constant:
    //    X(oneshot)X(session)X(blanket)X(no)
    // e.g. 1011 (=11) means that oneshot, blanket and no are allowed.
    // The following constants are used to encode/decode the allowed modes
    // into/from a 4-bit number
    TInt currentInteractionMode = JavaCommonUtils::wstringToInt(aSettings.getCurrentInteractionMode());
    TInt allowedInteractionModes = JavaCommonUtils::wstringToInt(aSettings.getAllowedInteractionModes());
    vector<LocalizedString> itemValues;
    int tmp = allowedInteractionModes & INTERACTION_MODE_ONESHOT;
    if (tmp > 0)
    {
        itemValues.push_back(LocalizedString(ONESHOT_INTERACTION_MODE,getLocalizedSettingsInteractionMode(INTERACTION_MODE_ONESHOT),ONESHOT_INTERACTION_MODE_DISPLAY_INDEX));
    }
    tmp = allowedInteractionModes & INTERACTION_MODE_SESSION;
    if (tmp > 0)
    {
        itemValues.push_back(LocalizedString(SESSION_INTERACTION_MODE,getLocalizedSettingsInteractionMode(INTERACTION_MODE_SESSION),SESSION_INTERACTION_MODE_DISPLAY_INDEX));
    }
    tmp = allowedInteractionModes & INTERACTION_MODE_BLANKET;
    if (tmp > 0)
    {
        itemValues.push_back(LocalizedString(BLANKET_INTERACTION_MODE,getLocalizedSettingsInteractionMode(INTERACTION_MODE_BLANKET), BLANKET_INTERACTION_MODE_DISPLAY_INDEX));
    }
    tmp = allowedInteractionModes & INTERACTION_MODE_DENIED;
    if (tmp > 0)
    {
        itemValues.push_back(LocalizedString(DENIED_INTERACTION_MODE,getLocalizedSettingsInteractionMode(INTERACTION_MODE_DENIED),DENIED_INTERACTION_MODE_DISPLAY_INDEX));
    }
    // sort the vector
    std::sort(itemValues.begin(), itemValues.end(), AscendingLocalizedStringSort());
    LocalizedString itemName = LocalizedString(aSettings.getName(), getLocalizedSettingsName(aSettings.getName()));
    return ListItem(itemName, itemValues, aSettingsEnabled, findItem(itemValues, aSettings.getCurrentInteractionMode()), getSettingsDisplayIndex(aSettings.getName()));
}

const MidletSuiteSecuritySettings AppMngr2MidletSettingsUtil::ListItemToSettings(const ListItem& aListItem)
{
    return MidletSuiteSecuritySettings(aListItem.getName().getId(), aListItem.getValue().getId(), L"");
}

const ListItem AppMngr2MidletSettingsUtil::OnScreenKeypadToListItem(wstring aOnScreenKeypadValue, bool aOnScreenKeypadEnabled)
{
    LocalizedString onScreenKeypadName = LocalizedString(ON_SCREEN_KEYPAD_SETTINGS,getLocalizedSettingsName(ON_SCREEN_KEYPAD_SETTINGS));
    vector<LocalizedString> onScreenKeypadValues;
    onScreenKeypadValues.push_back(LocalizedString(ON_SCREEN_KEYPAD_VALUE_NO,getLocalizedOnScreenKeypadValue(ON_SCREEN_KEYPAD_VALUE_NO), ON_SCREEN_KEYPAD_VALUE_NO_DISPLAY_INDEX));
    onScreenKeypadValues.push_back(LocalizedString(ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS, getLocalizedOnScreenKeypadValue(ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS), ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS_DISPLAY_INDEX));
    onScreenKeypadValues.push_back(LocalizedString(ON_SCREEN_KEYPAD_VALUE_NAVIGATION,getLocalizedOnScreenKeypadValue(ON_SCREEN_KEYPAD_VALUE_NAVIGATION), ON_SCREEN_KEYPAD_VALUE_NAVIGATION_DISPLAY_INDEX));
    // sort the vector
    std::sort(onScreenKeypadValues.begin(), onScreenKeypadValues.end(), AscendingLocalizedStringSort());
    return ListItem(onScreenKeypadName, onScreenKeypadValues, aOnScreenKeypadEnabled, findItem(onScreenKeypadValues, aOnScreenKeypadValue));
}

const ListItem AppMngr2MidletSettingsUtil::SecurityWarningsModeToListItem(wstring aSecurityWarningsModeValue, bool aSecurityWarningsModeEnabled)
{
    LocalizedString securityWarningsModeName = LocalizedString(SECURITY_WARNINGS_SETTINGS,getLocalizedSettingsName(SECURITY_WARNINGS_SETTINGS));
    vector<LocalizedString> securityWarningsModeValues;
    securityWarningsModeValues.push_back(LocalizedString(SECURITY_WARNINGS_DEFAULT_MODE, getLocalizedSecurityWarningsModeValue(SECURITY_WARNINGS_DEFAULT_MODE), SECURITY_WARNINGS_DEFAULT_MODE_DISPLAY_INDEX));
    securityWarningsModeValues.push_back(LocalizedString(SECURITY_WARNINGS_USER_DEFINED_MODE,getLocalizedSecurityWarningsModeValue(SECURITY_WARNINGS_USER_DEFINED_MODE), SECURITY_WARNINGS_USER_DEFINED_MODE_DISPLAY_INDEX));
    // sort the vector
    std::sort(securityWarningsModeValues.begin(), securityWarningsModeValues.end(), AscendingLocalizedStringSort());
    return ListItem(securityWarningsModeName, securityWarningsModeValues, aSecurityWarningsModeEnabled, findItem(securityWarningsModeValues, aSecurityWarningsModeValue));
}

const ListItem AppMngr2MidletSettingsUtil::SnapToListItem(CAppMngr2SuiteSnapItem aSnap, bool aSnapEnabled)
{
    LocalizedString snapName = LocalizedString(ACCESS_POINT_SETTINGS,getLocalizedSettingsName(ACCESS_POINT_SETTINGS));
    vector<LocalizedString> snapValues;
    snapValues.push_back(LocalizedString(aSnap.iId, aSnap.iName));
    return ListItem(snapName, snapValues, aSnapEnabled);
}

int AppMngr2MidletSettingsUtil::getSettingsDisplayIndex(std::wstring aSettingsName)
{
    if (aSettingsName == ON_SCREEN_KEYPAD_SETTINGS)
    {
        return ON_SCREEN_KEYPAD_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == SECURITY_WARNINGS_SETTINGS)
    {
        return SECURITY_WARNINGS_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == NET_ACCESS_SETTINGS)
    {
        return NET_ACCESS_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == LOW_LEVEL_NET_ACCESS_SETTINGS)
    {
        return LOW_LEVEL_NET_ACCESS_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == CALL_CONTROL_SETTINGS)
    {
        return CALL_CONTROL_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == MESSAGING_SETTINGS)
    {
        return MESSAGING_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == RESTRICTED_MESSAGING_SETTINGS)
    {
        return RESTRICTED_MESSAGING_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == APPLICATION_AUTO_INVOCATION_SETTINGS)
    {
        return APPLICATION_AUTO_INVOCATION_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == LOCAL_CONNECTIVITY_SETTINGS)
    {
        return LOCAL_CONNECTIVITY_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == MULTIMEDIA_RECORDING_SETTINGS)
    {
        return MULTIMEDIA_RECORDING_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == READ_USER_DATA_ACCESS_SETTINGS)
    {
        return READ_USER_DATA_ACCESS_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == WRITE_USER_DATA_ACCESS_SETTINGS)
    {
        return WRITE_USER_DATA_ACCESS_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == LOCATION_SETTINGS)
    {
        return LOCATION_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == LANDMARK_SETTINGS)
    {
        return LANDMARK_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == AUTHENTICATION_SETTINGS)
    {
        return AUTHENTICATION_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == SMART_CARD_COMMUNICATION_SETTINGS)
    {
        return SMART_CARD_COMMUNICATION_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == BROADCAST_SETTINGS)
    {
        return BROADCAST_SETTINGS_DISPLAY_INDEX;
    }
    if (aSettingsName == NFC_WRITE_ACCESS_SETTINGS)
    {
        return NFC_WRITE_ACCESS_SETTINGS_DISPLAY_INDEX;
    }
    return LAST_SETTINGS_DISPLAY_INDEX;
}

int AppMngr2MidletSettingsUtil::findItem(std::vector<LocalizedString> aListItems, std::wstring aListItemId)
{
    for (int i=0; i<aListItems.size(); i++)
    {
        if (aListItems[i].getId() == aListItemId)
        {
            return i;
        }
    }
    return 0;
}


TInt AppMngr2MidletSettingsUtil::GetLocalizedSettingsName(wstring aSettingsName)
{
    if (aSettingsName == ON_SCREEN_KEYPAD_SETTINGS)
    {
        return R_JAVA_SETTING_ON_SCREEN_KEYPAD_PAGE;
    }
    if (aSettingsName == SECURITY_WARNINGS_SETTINGS)
    {
        return R_JAVA_SETTING_SECURITY_WARNINGS_PAGE;
    }
    if (aSettingsName == NET_ACCESS_SETTINGS)
    {
        return R_JAVA_SETTING_NET_ACCESS_PAGE;
    }
    if (aSettingsName == LOW_LEVEL_NET_ACCESS_SETTINGS)
    {
        return R_JAVA_SETTING_LOW_LEVEL_NET_ACCESS_PAGE;
    }
    if (aSettingsName == CALL_CONTROL_SETTINGS)
    {
        return R_JAVA_SETTING_CALL_CONTROL_PAGE;
    }
    if (aSettingsName == MESSAGING_SETTINGS)
    {
        return R_JAVA_SETTING_MESSAGING_PAGE;
    }
    if (aSettingsName == RESTRICTED_MESSAGING_SETTINGS)
    {
        return R_JAVA_SETTING_RESTRICTED_MESSAGING_PAGE;
    }
    if (aSettingsName == APPLICATION_AUTO_INVOCATION_SETTINGS)
    {
        return R_JAVA_SETTING_APP_AUTO_INVOCAT_PAGE;
    }
    if (aSettingsName == LOCAL_CONNECTIVITY_SETTINGS)
    {
        return R_JAVA_SETTING_LOCAL_CONN_PAGE;
    }
    if (aSettingsName == MULTIMEDIA_RECORDING_SETTINGS)
    {
        return R_JAVA_SETTING_MM_RECORD_PAGE;
    }
    if (aSettingsName == READ_USER_DATA_ACCESS_SETTINGS)
    {
        return R_JAVA_SETTING_READ_DATA_PAGE;
    }
    if (aSettingsName == WRITE_USER_DATA_ACCESS_SETTINGS)
    {
        return R_JAVA_SETTING_WRITE_DATA_PAGE;
    }
    if (aSettingsName == LOCATION_SETTINGS)
    {
        return R_JAVA_SETTING_LOCATION_PAGE;
    }
    if (aSettingsName == LANDMARK_SETTINGS)
    {
        return R_JAVA_SETTING_LANDMARKS_PAGE;
    }
    if (aSettingsName == AUTHENTICATION_SETTINGS)
    {
        return R_JAVA_SETTING_AUTH_PAGE;
    }
    if (aSettingsName == SMART_CARD_COMMUNICATION_SETTINGS)
    {
        return R_JAVA_SETTING_SMARTCARD_PAGE;
    }
    if (aSettingsName == BROADCAST_SETTINGS)
    {
        return R_JAVA_SETTING_BROADCAST_PAGE;
    }
    if (aSettingsName == NFC_WRITE_ACCESS_SETTINGS)
    {
        return R_JAVA_SETTING_NFC_WRITE_ACCESS_PAGE;
    }
    return R_JAVA_SETTING_DEFAULT_PAGE;

}

const wstring AppMngr2MidletSettingsUtil::getLocalizedSettingsName(wstring aSettingsName)
{
    HBufC* localizedName = NULL;
    if (aSettingsName == ACCESS_POINT_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_NETWORK_DESTINATION);
    }
    else if (aSettingsName == ON_SCREEN_KEYPAD_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_KEYPAD);
    }
    else if (aSettingsName == SECURITY_WARNINGS_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_SECURITY_WARNINGS);
    }
    else if (aSettingsName == NET_ACCESS_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_NET_ACCESS);
    }
    else if (aSettingsName == LOW_LEVEL_NET_ACCESS_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_LOW_LEVEL_NET_ACCESS);
    }
    else if (aSettingsName == CALL_CONTROL_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_RESTRICTED_CALL_CONTROL);
    }
    else if (aSettingsName == MESSAGING_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_MESSAGING);
    }
    else if (aSettingsName == RESTRICTED_MESSAGING_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_RESTRICTED_MESSAGING);
    }
    else if (aSettingsName == APPLICATION_AUTO_INVOCATION_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_APP_AUTO_INVOCAT);
    }
    else if (aSettingsName == LOCAL_CONNECTIVITY_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_LOCAL_CONN);
    }
    else if (aSettingsName == MULTIMEDIA_RECORDING_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_MM_RECORD);
    }
    else if (aSettingsName == READ_USER_DATA_ACCESS_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_READ_DATA);
    }
    else if (aSettingsName == WRITE_USER_DATA_ACCESS_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_WRITE_DATA);
    }
    else if (aSettingsName == LOCATION_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_LOCATION);
    }
    else if (aSettingsName == LANDMARK_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_LANDMARKS);
    }
    else if (aSettingsName == AUTHENTICATION_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_AUT);
    }
    else if (aSettingsName == SMART_CARD_COMMUNICATION_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_SMARTCARD);
    }
    else if (aSettingsName == BROADCAST_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_BROADCAST);
    }
    else if (aSettingsName == NFC_WRITE_ACCESS_SETTINGS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_NAME_NFC_WRITE_ACCESS);
    }
    if (localizedName != NULL)
    {
        wstring ret = wstring((const wchar_t*)localizedName->Ptr(), localizedName->Length());
        CleanupStack::PopAndDestroy(localizedName);
        return ret;
    }
    else
    {
        return aSettingsName;
    }
}

const wstring AppMngr2MidletSettingsUtil::getLocalizedSettingsInteractionMode(int aInteractionMode)
{
    HBufC* localizedName = NULL;
    switch (aInteractionMode)
    {
    case INTERACTION_MODE_BLANKET:
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_VALUE_BLANK);
        break;
    case INTERACTION_MODE_SESSION:
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_VALUE_SESSION);
        break;
    case INTERACTION_MODE_ONESHOT:
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_VALUE_ONESHOT);
        break;
    case INTERACTION_MODE_DENIED:
        localizedName = StringLoader::LoadLC(R_JAVA_SETTING_VALUE_NO);
        break;
    default:
        return L"";
    }
    wstring ret = wstring((const wchar_t*)localizedName->Ptr(), localizedName->Length());
    CleanupStack::PopAndDestroy(localizedName);
    return ret;
}

const wstring AppMngr2MidletSettingsUtil::getLocalizedSecurityWarningsModeValue(wstring aSecurityWarningsModeValue)
{
    HBufC* localizedName = NULL;
    if (aSecurityWarningsModeValue == SECURITY_WARNINGS_DEFAULT_MODE)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SECURITY_WARNINGS_SETTING_VALUE_DEFAULT);
    }
    else if (aSecurityWarningsModeValue == SECURITY_WARNINGS_USER_DEFINED_MODE)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_SECURITY_WARNINGS_SETTING_VALUE_USER_DEFINED);
    }
    else
    {
        return L"";
    }
    wstring ret = wstring((const wchar_t*)localizedName->Ptr(), localizedName->Length());
    CleanupStack::PopAndDestroy(localizedName);
    return ret;
}

const wstring AppMngr2MidletSettingsUtil::getLocalizedOnScreenKeypadValue(wstring aOnScreenKeypadValue)
{
    HBufC* localizedName = NULL;
    if (aOnScreenKeypadValue == ON_SCREEN_KEYPAD_VALUE_NO)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_KEYPAD_SETTING_VALUE_NO);
    }
    else if (aOnScreenKeypadValue == ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_KEYPAD_SETTING_VALUE_GAME);
    }
    else if (aOnScreenKeypadValue == ON_SCREEN_KEYPAD_VALUE_NAVIGATION)
    {
        localizedName = StringLoader::LoadLC(R_JAVA_KEYPAD_SETTING_VALUE_NAVIGATION);
    }
    else
    {
        return L"";
    }
    wstring ret = wstring((const wchar_t*)localizedName->Ptr(), localizedName->Length());
    CleanupStack::PopAndDestroy(localizedName);
    return ret;
}
