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
* Description:
*
*/


#ifndef APPMNGR2MIDLETSETTINGSUTIL_H
#define APPMNGR2MIDLETSETTINGSUTIL_H

// INCLUDES
#include <e32base.h>
#include <string>
#include <vector>

// FORWARD DECLARATIONS
// CONSTANTS
const int INTERACTION_MODE_DENIED = 1;
const int INTERACTION_MODE_BLANKET = 2;
const int INTERACTION_MODE_SESSION = 4;
const int INTERACTION_MODE_ONESHOT = 8;

// interaction modes display indexes
const int DENIED_INTERACTION_MODE_DISPLAY_INDEX = 0;
const int ONESHOT_INTERACTION_MODE_DISPLAY_INDEX = 1;
const int SESSION_INTERACTION_MODE_DISPLAY_INDEX = 2;
const int BLANKET_INTERACTION_MODE_DISPLAY_INDEX = 3;

// on screen keypad values display indexes
const int ON_SCREEN_KEYPAD_VALUE_NO_DISPLAY_INDEX = 0;
const int ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS_DISPLAY_INDEX = 1;
const int ON_SCREEN_KEYPAD_VALUE_NAVIGATION_DISPLAY_INDEX = 2;

// security warnings values display indexes
const int SECURITY_WARNINGS_DEFAULT_MODE_DISPLAY_INDEX = 0;
const int SECURITY_WARNINGS_USER_DEFINED_MODE_DISPLAY_INDEX = 1;

// settings display indexes
const int ON_SCREEN_KEYPAD_SETTINGS_DISPLAY_INDEX = 0;
const int SECURITY_WARNINGS_SETTINGS_DISPLAY_INDEX = 1;
const int NET_ACCESS_SETTINGS_DISPLAY_INDEX = 2;
const int LOW_LEVEL_NET_ACCESS_SETTINGS_DISPLAY_INDEX = 3;
const int CALL_CONTROL_SETTINGS_DISPLAY_INDEX = 4;
const int MESSAGING_SETTINGS_DISPLAY_INDEX = 5;
const int RESTRICTED_MESSAGING_SETTINGS_DISPLAY_INDEX = 6;
const int APPLICATION_AUTO_INVOCATION_SETTINGS_DISPLAY_INDEX = 7;
const int LOCAL_CONNECTIVITY_SETTINGS_DISPLAY_INDEX = 8;
const int MULTIMEDIA_RECORDING_SETTINGS_DISPLAY_INDEX = 9;
const int READ_USER_DATA_ACCESS_SETTINGS_DISPLAY_INDEX = 10;
const int WRITE_USER_DATA_ACCESS_SETTINGS_DISPLAY_INDEX = 11;
const int LOCATION_SETTINGS_DISPLAY_INDEX = 12;
const int LANDMARK_SETTINGS_DISPLAY_INDEX = 12;
const int AUTHENTICATION_SETTINGS_DISPLAY_INDEX = 14;
const int SMART_CARD_COMMUNICATION_SETTINGS_DISPLAY_INDEX = 15;
const int BROADCAST_SETTINGS_DISPLAY_INDEX = 16;
const int NFC_WRITE_ACCESS_SETTINGS_DISPLAY_INDEX = 17;
const int URL_START_SETTINGS_DISPLAY_INDEX = 18;
const int LAST_SETTINGS_DISPLAY_INDEX = 19;

// CLASS DECLARATION
class MidletSuiteSecuritySettings: public CBase
{
public:
    MidletSuiteSecuritySettings(const std::wstring& aSettingsName,
                                const std::wstring& aCurrentInteractionMode,
                                const std::wstring& aAllowedInteractionModes);

    MidletSuiteSecuritySettings &operator=(const MidletSuiteSecuritySettings&);

    MidletSuiteSecuritySettings(const MidletSuiteSecuritySettings&);

    MidletSuiteSecuritySettings();

    const std::wstring getName() const;
    const std::wstring getCurrentInteractionMode() const;
    const std::wstring getAllowedInteractionModes() const;

private:
    std::wstring iSettingsName;
    std::wstring iCurrentInteractionMode;
    std::wstring iAllowedInteractionModes;
};

class LocalizedString: public CBase
{
public:
    LocalizedString(const std::wstring& aId,
                    const std::wstring& aValue);

    LocalizedString(const std::wstring& aId,
                    const std::wstring& aValue,
                    int aIndex);

    LocalizedString(TUint aId,
                    const std::wstring& aValue);

    LocalizedString &operator=(const LocalizedString&);

    LocalizedString(const LocalizedString&);

    LocalizedString();

    const std::wstring& getId() const;
    const std::wstring& getValue() const;
    int getIndex() const;

private:
    std::wstring iId;
    std::wstring iValue;
    int iIndex;
};

class ListItem: public CBase
{
public:
    ListItem(const LocalizedString& aName,
             const std::vector<LocalizedString>& aPossibleValues);

    ListItem(const LocalizedString& aName,
             const std::vector<LocalizedString>& aPossibleValues,
             bool aEnabled);

    ListItem(const LocalizedString& aName,
             const std::vector<LocalizedString>& aPossibleValues,
             bool aEnabled,
             int aCurrentValueIndex);

    ListItem(const LocalizedString& aName,
             const std::vector<LocalizedString>& aPossibleValues,
             bool aEnabled,
             int aCurrentValueIndex,
             int aIndex);

    ListItem &operator=(const ListItem&);

    ListItem(const ListItem&);

    ListItem();

    void setCurrentValue(int aSelectedValueIndex);
    int getCurrentValue() const;

    const LocalizedString& getName() const;
    const std::vector<LocalizedString>& getValues() const;
    const LocalizedString& getValue() const;
    const LocalizedString& getValue(int aValueIndex) const;
    bool getEnabled() const;
    void setEnabled(bool aEnabled);
    int getIndex() const;

private:
    LocalizedString iName;
    std::vector<LocalizedString> iPossibleValues;
    int iCurrentValueIndex;
    bool iEnabled;
    int iIndex;
};

/**
* CAppMngr2SuiteSnapItem
*
*/
class CAppMngr2SuiteSnapItem: public CBase
{
public:
    CAppMngr2SuiteSnapItem();
    CAppMngr2SuiteSnapItem(const CAppMngr2SuiteSnapItem&);

    TUint32 iId;
    std::wstring iName;
};

// CLASS DECLARATION

class AppMngr2MidletSettingsUtil
{
public:
    struct AscendingListItemSort
    {
        bool operator()(const ListItem& i1, const ListItem& i2)
        {
            return i1.getIndex() < i2.getIndex();
        }
    };
    struct AscendingLocalizedStringSort
    {
        bool operator()(const LocalizedString& i1, const LocalizedString& i2)
        {
            return i1.getIndex() < i2.getIndex();
        }
    };

    static const ListItem SettingsToListItem(MidletSuiteSecuritySettings aSettings, bool aSettingsEnabled);
    static const MidletSuiteSecuritySettings ListItemToSettings(const ListItem& aListItem);
    static const ListItem OnScreenKeypadToListItem(std::wstring aOnScreenKeypadValue, bool aOnScreenKeypadEnabled);
    static const ListItem SnapToListItem(CAppMngr2SuiteSnapItem aSnap, bool aSnapEnabled);
    static const ListItem SecurityWarningsModeToListItem(std::wstring aSecurityWarningsModeValue, bool aSecurityWarningsModeEnabled);
    static TInt GetLocalizedSettingsName(std::wstring aSettingsName);

private:
    static const std::wstring getLocalizedSettingsName(std::wstring aSettingsName);
    static const std::wstring getLocalizedSettingsInteractionMode(int aInteractionMode);
    static const std::wstring getLocalizedOnScreenKeypadValue(std::wstring aOnScreenKeypadValue);
    static const std::wstring getLocalizedSecurityWarningsModeValue(std::wstring aSecurityWarningsModeValue);
    static int getSettingsDisplayIndex(std::wstring aSettingsName);
    static int findItem(std::vector<LocalizedString> aListItems, std::wstring aListItemId);

};

#endif // APPMNGR2MIDLETSETTINGSUTIL_H


// End of file
