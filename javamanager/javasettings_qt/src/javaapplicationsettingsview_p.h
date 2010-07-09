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
#ifndef JAVAAPPLICATIONSETTINGSVIEW_P_H
#define JAVAAPPLICATIONSETTINGSVIEW_P_H

#include <QVector>
#include <QList>
#include <QTranslator>
#include <QModelIndex>
#include <QEventLoop>
#include <memory>
#include <cmapplsettingsui.h>
#include "javaapplicationsettings.h"
#include "javastorage.h"

class HbWidget;
class HbPushButton;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbAction;

namespace java
{
namespace settings
{

// constants used for retrieving the translations of different texts via hbTrId function
const char* const ID_BLANKET = "txt_java_sett_setlabel_permission_val_blanket";
const char* const ID_SESSION = "txt_java_sett_setlabel_permission_val_session";
const char* const ID_ONESHOT = "txt_java_sett_setlabel_permission_val_oneshot";
const char* const ID_DENIED = "txt_java_sett_setlabel_permission_val_no";
const char* const ID_SECURITY_LEVEL = "txt_java_sett_setlabel_security_level";
const char* const ID_USER_DEFINED = "txt_java_sett_setlabel_security_level_val_user_defined";
const char* const ID_SENSITIVE_SETTINGS = "txt_java_sett_info_query_perm_sec";
const char* const ID_SENSITIVE_SETTINGS_NET_USAGE = "txt_java_sett_info_query_perm_net";
const char* const ID_MUTUALLY_EXCLUSIVE_SETTINGS = "txt_java_sett_info_query_perm_warn";
const char* const ID_OK = "txt_java_sett_button_settings_ok";
const char* const ID_CANCEL = "txt_java_sett_button_settings_cancel";
const char* const ID_SECURITY_WARNING_TITLE = "txt_java_sett_title_note_security_warn";
const char* const ID_NET_ACCESS = "txt_java_sett_setlabel_net_access";
const char* const ID_LOW_LEVEL_NET_ACCESS = "txt_java_sett_setlabel_low_level_net_access";
const char* const ID_NETWORK_CONNECTION = "txt_occ_title_network_connection";
const char* const ID_SETTINGS_TITLE = "txt_java_sett_title_settings";
const char* const ID_SETTINGS_NOT_AVAILABLE = "txt_java_sett_info_settings_not_available";
const char* const ID_GENERAL_GROUP_TITLE = "txt_java_sett_subtitle_general";
const char* const ID_SECURITY_GROUP_TITLE = "txt_java_sett_subtitle_security";
const char* const ID_OSK_NO = "txt_java_sett_setlabel_osk_val_no";
const char* const ID_OSK_GAME = "txt_java_sett_setlabel_osk_val_game";
const char* const ID_OSK_NAVIGATION = "txt_java_sett_setlabel_osk_val_navigation";
const char* const ID_OSK = "txt_java_sett_setlabel_osk";
const char* const ID_SECURITY_LEVEL_DEFAULT = "txt_java_sett_setlabel_security_level_val_default";
const char* const ID_MESSAGING = "txt_java_sett_setlabel_messaging";
const char* const ID_RESTRICTED_MESSAGING = "txt_java_sett_setlabel_restricted_messaging";
const char* const ID_CALL_CONTROL = "txt_java_sett_setlabel_call_control";
const char* const ID_LOCAL_CONNECTIVITY = "txt_java_sett_setlabel_local_conn";
const char* const ID_MULTIMEDIA_RECORDING = "txt_java_sett_setlabel_mm_record";
const char* const ID_WRITE_USER_DATA_ACCESS = "txt_java_sett_setlabel_write_data";
const char* const ID_READ_USER_DATA_ACCESS = "txt_java_sett_setlabel_read_data";
const char* const ID_LOCATION = "txt_java_sett_setlabel_location";
const char* const ID_LANDMARK = "txt_java_sett_setlabel_landmarks";
const char* const ID_AUTHENTICATION = "txt_java_sett_setlabel_auth";
const char* const ID_SMART_CARD_COMMUNICATION = "txt_java_sett_setlabel_smartcard";
const char* const ID_APPLICATION_AUTO_INVOCATION = "txt_java_sett_setlabel_app_auto_invoc";
const char* const ID_BROADCAST = "txt_java_sett_setlabel_broadcast";
const char* const ID_NFC_WRITE_ACCESS = "txt_java_sett_setlabel_nfc_write_access";
const char* const ID_URL_START = "txt_java_sett_setlabel_url_start";

class JavaApplicationSettingsView;

// this structures are used for ensuring the settings are always
// displayed in same order: each setting has associated an index
// - the lower the index is the higher in the list the setting is
// displayed
enum SettingsIndexes
{
    NET_ACCESS_SETTINGS_INDEX,
    LOW_LEVEL_NET_ACCESS_SETTINGS_INDEX,
    MESSAGING_SETTINGS_INDEX,
    RESTRICTED_MESSAGING_SETTINGS_INDEX,
    CALL_CONTROL_SETTINGS_INDEX,
    LOCAL_CONNECTIVITY_SETTINGS_INDEX,
    MULTIMEDIA_RECORDING_SETTINGS_INDEX,
    WRITE_USER_DATA_ACCESS_SETTINGS_INDEX,
    READ_USER_DATA_ACCESS_SETTINGS_INDEX,
    LOCATION_SETTINGS_INDEX,
    LANDMARK_SETTINGS_INDEX,
    AUTHENTICATION_SETTINGS_INDEX,
    SMART_CARD_COMMUNICATION_SETTINGS_INDEX,
    APPLICATION_AUTO_INVOCATION_SETTINGS_INDEX,
    BROADCAST_SETTINGS_INDEX,
    NFC_WRITE_ACCESS_SETTINGS_INDEX,
    URL_START_SETTINGS_INDEX,
    LAST_SETTINGS_INDEX,
};
struct IndexedSettingsName
{
    std::wstring name;
    int index;
};
struct AscendingSort
{
    bool operator()(const IndexedSettingsName& i1, const IndexedSettingsName& i2)
    {
        return i1.index < i2.index;
    }
};

/**
 * The Ui is build around the Orbit's HbDataForm, with one group for the general settings
 * (including network settings and On screen keypad settings) and another group for
 * security settings (including the security warnings mode and the actual security settings).
 * The items inside the group are either of type tooglebutton (network settings and security
 * warnings mode) or of type combobox. When the plugin is instantiated the two groups are
 * created and the settings are read from storage. When the plugin receives an initialization
 * signal the HbDataModel's  (associated to the main HbDataForm) itemShown signal is connected
 * to the plugin's slot _q_dataItemDisplayed. While the groups become visible they are populated
 * with dataitems of either type tooglebutton or combobox. After being added to their parent's
 * group the dataitems's currentIndexChanged signal is connected to the plugin's _q_settingsChanged
 * slot where the settings changes are handled.
 */
class JavaApplicationSettingsViewPrivate: public QObject
{
    Q_OBJECT

public:
    JavaApplicationSettingsViewPrivate(const QString& aJavaAppUid);
    virtual ~JavaApplicationSettingsViewPrivate();
    void init(JavaApplicationSettingsView* aPublicView);

    // slots
    void _q_settingsChanged(const QString& aNewValue);
    void _q_settingsChanged(bool aNewValue);
    void _q_dataItemDisplayed(const QModelIndex aDataItemIndex);

private slots:
    void netConnSelected(uint aNetConnSelectionStatus);
    void securityWarningDismissed(HbAction* aDismissAction);

private:
    void init();
    void initGeneralSettings();
    void initSecuritySettings();
    void readAllSettings();
    void initSettings(QVector<JavaApplicationSettings>& aSettings, HbDataFormModelItem * aParent);
    void writeSettings(JavaApplicationSettings& aSettings, const QString &aNewValue);
    void securityWarningsChanged(const QString& aNewValue);
    bool isBlanketAllowed(const JavaApplicationSettings& aSettings);
    HbWidget * itemToWidget(const HbDataFormModelItem * aItem);
    JavaApplicationSettings* findSettings(HbWidget* aId);
    JavaApplicationSettings* findSettings(HbWidget* aId, QVector<JavaApplicationSettings>& aAllSettings);
    void readFromStorage(JavaApplicationSettings& aSettings);
    void writeToStorage(JavaApplicationSettings& aSettings);
    bool isInStorage(const std::wstring& aColumnName, const std::wstring& aColumnFilterName, const std::wstring& aColumnFilterValue, const std::string& aTableName);
    std::wstring readFromStorage(const std::wstring& aColumnName, const std::wstring& aColumnFilterName, const std::wstring& aColumnFilterValue, const std::string& aTableName);
    std::vector<IndexedSettingsName> readFromStorage(const std::wstring& aColumnName, const std::string& aPrimaryTableName, const std::string& aSecondaryTableName);
    void filterSecuritySettings(JavaApplicationSettings& aSettings);
    void findEntry(const java::storage::JavaStorageApplicationList_t& aQueryResult, const std::wstring& aName, std::wstring& aValue);
    void readSuiteUid(const QString& aAppUid);
    bool isSecurityWarningAccepted(const QString& aText, const QString& aAcceptActionLabel, const QString& aRejectActionLabel, const QString& aHeadingText);
    void handleNetworkSettings();
    void readNetworkConnectionName();
    void initNetworkConnection();
    void configureList(QList<JavaApplicationSettings*>& aList, const QStringList& aSettingsNames,  const QHash<QString, int>& aSettingsIndexes);
    void attachList(const QString& aSettingsName, const QList<JavaApplicationSettings*>& aList, const QHash<QString, int>& aSettingsIndexes, bool isHighRiskList = true);
    void configureLists();
    void installTranslator(const QString& aTranslationFileName);

private:
    HbDataForm * mMainForm;
    HbDataFormModel *mModel;
    HbDataFormModelItem * mGeneralSettingsGroup;
    HbDataFormModelItem * mSecuritySettingsGroup;
    QVector<JavaApplicationSettings> mGeneralSettings;
    QVector<JavaApplicationSettings> mSecuritySettings;
    QVector<JavaApplicationSettings> mExtraSettings;
    JavaApplicationSettingsView* mPublicView;
    std::auto_ptr<java::storage::JavaStorage> mStorage;
    std::wstring mSuiteUid;
    // licalized strings used more than once: the translation is
    // retrieved only once
    QString TXT_BLANKET,TXT_SESSION,TXT_ONESHOT,TXT_DENIED,TXT_SECURITY_LEVEL,TXT_USER_DEFINED,TXT_SENSITIVE_SETTINGS,TXT_SENSITIVE_SETTINGS_NET_USAGE,TXT_MUTUALLY_EXCLUSIVE_SETTINGS,TXT_OK,TXT_CANCEL,TXT_SECURITY_WARNING_TITLE,TXT_NET_ACCESS,TXT_LOW_LEVEL_NET_ACCESS,TXT_NETWORK_CONNECTION,TXT_NETWORK_CONNECTION_CHANGE,TXT_SETTINGS_TITLE,TXT_SETTINGS_NOT_AVAILABLE;
    CmApplSettingsUi* mNetConnSettingsUi;
    CmApplSettingsUi::SettingSelection mNetConnSelection;
    JavaApplicationSettings::NetworkConnection mNetConn;
    int mNetSettIndex;
    // own event loop used for "translating" asynchronous calls into
    // synchronous calls
    QEventLoop* mAsyncToSyncCallEventLoop;
    // boolean for carying the user's input in between two async calls
    bool mSecWarningAccepted;
    // the default network connection used if user did not specify one
    // or the user specified network connection became invalid
    // (e.g. after removing the user defined snap)
    int mDefaultConnId;
    // list of installed translators: settings view uses own
    // translator ("javaruntimeapplicationsettings")
    // and for network settings it uses the cmmanager's translators:
    // "cmapplsettingsui" and "cmmanager"
    QList<QTranslator *> mTranslators;
    QHash<QString, QString> mLocalizedSettingsNames;
    // hash for associating settings names with indexes within
    // vectors (mGeneralSettings, mSecuritySettings)
    QHash<QString, int> mSettingsIndexes;
    // hash for associating settings names with display indexes:
    // a settings with lower index is shown before a settings
    // with a higher index
    QHash<QString, int> mSettingsNamesIndexes;
};

} // namespace settings
} // namespace java


#endif // JAVAAPPLICATIONSETTINGSVIEW_P_H
