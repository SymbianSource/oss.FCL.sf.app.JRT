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
#include <hbmessagebox.h>
#include <hbdataformviewitem.h>
#include <hbcombobox.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hblabel.h>
#include <QApplication>
#include <QTranslator>

#include "javaapplicationsettings.h"
#include "javaapplicationsettingsview.h"
#include "javaapplicationsettingsview_p.h"

#include "securitystoragedatadefs.h"
#include "javastoragenames.h"

using namespace java::storage;
using namespace std;

const wchar_t ON_SCREEN_KEYPAD_VALUE_NO[] = L"0";
const wchar_t ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS[] = L"1";
const wchar_t ON_SCREEN_KEYPAD_VALUE_NAVIGATION[] = L"2";

JavaApplicationSettingsViewPrivate::JavaApplicationSettingsViewPrivate(const QString& aJavaAppUid):
        mainForm(0), model(0), generalSettingsGroup(0), securitySettingsGroup(0), iJavaAppUid(aJavaAppUid.toStdWString())
{
    // load the correct translation of the localized strings
    QTranslator translator;
    // Current solution reads it from Z only (this does not work with IAD)
    // -> check if translator can handle path without drive letter (e.g. the resource
    // is loaded from the same drive where the DLL is loaded)
    if (translator.load("z:/resource/qt/translations/javaruntimeapplicationsettings_" + QLocale::system().name()))
    {
        qApp->installTranslator(&translator);
    }
    // init strings
    BLANKET = hbTrId("txt_java_sett_setlabel_permission_val_blanket");
    SESSION = hbTrId("txt_java_sett_setlabel_permission_val_session");
    ONESHOT = hbTrId("txt_java_sett_setlabel_permission_val_oneshot");
    DENIED = hbTrId("txt_java_sett_setlabel_permission_val_no");
    SECURITY_LEVEL = hbTrId("txt_java_sett_setlabel_security_level");
    USER_DEFINED = hbTrId("txt_java_sett_setlabel_security_level_val_user_defined");
    SENSITIVE_SETTINGS = hbTrId("txt_java_sett_info_query_perm_sec");
    SENSITIVE_SETTINGS_NET_USAGE = hbTrId("txt_java_sett_info_query_perm_net");
    MUTUALLY_EXCLUSIVE_SETTINGS = hbTrId("txt_java_sett_info_query_perm_warn");
    OK = hbTrId("txt_java_sett_button_settings_ok");
    CANCEL = hbTrId("txt_java_sett_button_settings_cancel");
    SECURITY_WARNING_TITLE = hbTrId("txt_java_sett_title_note_security_warn");
    NET_ACCESS = hbTrId("txt_java_sett_setlabel_net_access");
    LOW_LEVEL_NET_ACCESS = hbTrId("txt_java_sett_setlabel_low_level_net_access");

    // storage
    iStorage.reset(JavaStorage::createInstance());
    try
    {
        iStorage->open();
    }
    catch (JavaStorageException& aJse) {}

    // read all settings
    readAllSettings();

    // init form
    mainForm = new HbDataForm();
    mainForm->setHeading(hbTrId("txt_java_sett_title_settings"));
    model = new HbDataFormModel();

    // init settings
    generalSettingsGroup = model->appendDataFormGroup(
                               hbTrId("txt_java_sett_subtitle_general"), model->invisibleRootItem());
    initSettings(generalSettings, generalSettingsGroup);
    securitySettingsGroup = model->appendDataFormGroup(
                                hbTrId("txt_java_sett_subtitle_security"), model->invisibleRootItem());
    initSettings(securitySettings, securitySettingsGroup);

    // if security warnings is user defined -> add the extra settings, expand
    if (securitySettings[0].getCurrentValue() == 1)
    {
        securityWarningsChanged(USER_DEFINED);
    }

    // link form and model
    mainForm->setModel(model);
    mainForm->setExpanded(model->indexFromItem(generalSettingsGroup), true);
    mainForm->setExpanded(model->indexFromItem(securitySettingsGroup), (securitySettings[0].getCurrentValue() == 1));
}

void JavaApplicationSettingsViewPrivate::init(JavaApplicationSettingsView* aPublicView)
{
    iPublicView = aPublicView;

    // do the connect for the main form
    iPublicView->connect(mainForm, SIGNAL(activated(const QModelIndex)),
                         iPublicView, SLOT(_q_dataItemDisplayed(const QModelIndex)));

    // set the form as view's widget
    iPublicView->setWidget(mainForm);

    // set title
    iPublicView->setTitle(QString::fromStdWString(readFromStorage(PACKAGE_NAME, L"", L"", APPLICATION_PACKAGE_TABLE)));
}

JavaApplicationSettingsViewPrivate::~JavaApplicationSettingsViewPrivate()
{
    delete mainForm;
    delete model;
}

void JavaApplicationSettingsViewPrivate::readAllSettings()
{
    // general settings
    QStringList settingsValues;
    vector<wstring> storageValues;
    if (!findFromStorage(VALUE, NAME, L"Nokia-MIDlet-On-Screen-Keypad", APPLICATION_PACKAGE_ATTRIBUTES_TABLE))
    {
        // if the on screen keypad is predefined, it should not be changable by user
        // -> right now it is not added to settings list. It should be changed so that
        // the setting should not be editable by user
        settingsValues<<hbTrId("txt_java_sett_setlabel_osk_val_no")<<hbTrId("txt_java_sett_setlabel_osk_val_game")<<hbTrId("txt_java_sett_setlabel_osk_val_navigation");
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_NO);
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS);
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_NAVIGATION);
        generalSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_osk"), settingsValues, ON_SCREEN_KEYPAD, MIDP_PACKAGE_TABLE, storageValues));
    }
    settingsValues = QStringList();
    settingsValues<<hbTrId("txt_java_sett_setlabel_network_conn_val_default")<<hbTrId("txt_java_sett_setlabel_network_conn_val_ask_user");
    generalSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_network_conn"), settingsValues));

    // security settings
    settingsValues = QStringList();
    storageValues.clear();
    settingsValues<<hbTrId("txt_java_sett_setlabel_security_level_val_default")<<hbTrId("txt_java_sett_setlabel_security_level_val_user_defined");
    storageValues.push_back(SECURITY_WARNINGS_DEFAULT_MODE);
    storageValues.push_back(SECURITY_WARNINGS_USER_DEFINED_MODE);
    securitySettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_security_level"), settingsValues, SECURITY_WARNINGS, MIDP_PACKAGE_TABLE, storageValues));

    // extra settings
    int i=0;
    settingsValues = QStringList();
    storageValues.clear();
    settingsValues<<hbTrId("txt_java_sett_setlabel_permission_val_oneshot")<<hbTrId("txt_java_sett_setlabel_permission_val_session")<<hbTrId("txt_java_sett_setlabel_permission_val_blanket")<<hbTrId("txt_java_sett_setlabel_permission_val_no");
    storageValues.push_back(ONESHOT_INTERACTION_MODE);
    storageValues.push_back(SESSION_INTERACTION_MODE);
    storageValues.push_back(BLANKET_INTERACTION_MODE);
    storageValues.push_back(DENIED_INTERACTION_MODE);
    extraSettings.append(JavaApplicationSettings(NET_ACCESS, settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, NET_ACCESS_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(LOW_LEVEL_NET_ACCESS, settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, LOW_LEVEL_NET_ACCESS_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_messaging"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, MESSAGING_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_restricted_messaging"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, RESTRICTED_MESSAGING_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_call_control"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, CALL_CONTROL_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_local_conn"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, LOCAL_CONNECTIVITY_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_mm_record"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, MULTIMEDIA_RECORDING_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_write_data"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, WRITE_USER_DATA_ACCESS_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_read_data"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, READ_USER_DATA_ACCESS_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_location"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, LOCATION_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_landmarks"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, LANDMARK_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_auth"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, AUTHENTICATION_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_smartcard"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, SMART_CARD_COMMUNICATION_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_app_auto_invoc"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, APPLICATION_AUTO_INVOCATION_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_broadcast"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, BROADCAST_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_nfc_write_access"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, NFC_WRITE_ACCESS_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;

    // configure the high risk lists
    QList<JavaApplicationSettings*> highRiskList;
    // net access with multimedia and read user data
    highRiskList << &(extraSettings[6]) << &(extraSettings[8]);
    extraSettings[0].setHighRiskList(highRiskList);
    // low level net access with multimedia and read user data
    extraSettings[1].setHighRiskList(highRiskList);
    // messaging with multimedia and read user data
    extraSettings[2].setHighRiskList(highRiskList);
    // restricted messaging with multimedia and read user data
    extraSettings[3].setHighRiskList(highRiskList);
    // call control with multimedia and read user data
    extraSettings[4].setHighRiskList(highRiskList);
    // local connectivity with multimedia and read user data
    extraSettings[5].setHighRiskList(highRiskList);
    // multimedia with net access, low level net access, messaging,
    // restricted messaging, call control and local connectivity
    highRiskList.clear();
    highRiskList << &(extraSettings[0]) << &(extraSettings[1])
    << &(extraSettings[2]) << &(extraSettings[3])
    << &(extraSettings[4]) << &(extraSettings[5]);
    extraSettings[6].setHighRiskList(highRiskList);
    // read user data with net access, low level net access, messaging,
    // restricted messaging, call control and local connectivity
    extraSettings[8].setHighRiskList(highRiskList);

    // configure the mutually exclusive list
    QList<JavaApplicationSettings*> mutuallyExclusiveList;
    //net access with application auto invocation
    mutuallyExclusiveList << &(extraSettings[13]);
    extraSettings[0].setMutuallyExclusiveList(mutuallyExclusiveList);
    //low level net access with application auto invocation
    extraSettings[1].setMutuallyExclusiveList(mutuallyExclusiveList);
    //application auto invocation with net access and low level net access
    mutuallyExclusiveList.clear();
    mutuallyExclusiveList << &(extraSettings[0]) << &(extraSettings[1]);
    extraSettings[13].setMutuallyExclusiveList(mutuallyExclusiveList);
}

void JavaApplicationSettingsViewPrivate::initSettings(QVector<JavaApplicationSettings>& settings, HbDataFormModelItem * parent)
{
    for (int i=0; i<settings.size(); i++)
    {
        HbDataFormModelItem * appSettings = model->appendDataFormItem(
                                                HbDataFormModelItem::ComboBoxItem,
                                                settings[i].getName(), parent);
        settings[i].setId(appSettings);
        appSettings->setContentWidgetData(QString("items"), settings[i].getValues());
        appSettings->setContentWidgetData(QString("currentIndex"),readFromStorage(settings[i]));
    }
}

void JavaApplicationSettingsViewPrivate::writeSettings(JavaApplicationSettings& settings, const QString &aNewValue)
{
    QStringList values = settings.getValues();
    int currentValue = settings.getCurrentValue();
    int newValue = settings.getValue(aNewValue);
    if (newValue != -1 && currentValue != newValue)
    {
        // handle blanket settings
        if (aNewValue == BLANKET)
        {
            // check blanket settings
            if (!blanketAllowed(settings))
            {
                // change back to the old value
                HbComboBox * settingsCombo = itemToComboBox(settings.getId());
                settingsCombo->setCurrentIndex(currentValue);
                return;
            }
        }
        settings.setCurrentValue(newValue);
        writeToStorage(settings);
    }
}

bool JavaApplicationSettingsViewPrivate::blanketAllowed(const JavaApplicationSettings &settings)
{
    // handle the high risk items
    QList<JavaApplicationSettings*> highRiskList = settings.getHighRiskList();
    if (highRiskList.size() > 0)
    {
        // go through the list and see if any of the setings are in blanket
        for (int i=0; i<highRiskList.size(); i++)
        {
            if (highRiskList[i]->getValue(highRiskList[i]->getCurrentValue())
                    == BLANKET)
            {
                QString secWarning = SENSITIVE_SETTINGS;
                if (settings.getName() == NET_ACCESS
                        || highRiskList[i]->getName() == NET_ACCESS
                        || settings.getName() == LOW_LEVEL_NET_ACCESS
                        || highRiskList[i]->getName() == LOW_LEVEL_NET_ACCESS)
                {
                    secWarning = SENSITIVE_SETTINGS_NET_USAGE;
                }
                /*if (!(HbMessageBox::launchQuestionMessageBox(
                        secWarning,OK,CANCEL ,new HbLabel(SECURITY_WARNING_TITLE))))
                {
                    return false;
                }*/
                break;
            }
        }
    }
    // handle the mutually exclusive rules
    QList<JavaApplicationSettings*> mutuallyExclusiveList = settings
            .getMutuallyExclusiveList();
    if (mutuallyExclusiveList.size() > 0)
    {
        // go through the list and see if any of the setings are in blanket
        for (int i=0; i<mutuallyExclusiveList.size(); i++)
        {
            if (mutuallyExclusiveList[i]->getValue(
                        mutuallyExclusiveList[i]->getCurrentValue()) == BLANKET)
            {
                bool isBlanketAllowed = true /*(HbMessageBox::launchQuestionMessageBox(
                        MUTUALLY_EXCLUSIVE_SETTINGS,OK,CANCEL, new HbLabel(SECURITY_WARNING_TITLE)))*/;
                if (isBlanketAllowed)
                {
                    // change  the current value to the maximum allowed
                    bool isBlanket = true;
                    do
                    {
                        if (isBlanket)
                        {
                            QStringList allowedValues = mutuallyExclusiveList[i]->getValues();
                            int selectedValue = mutuallyExclusiveList[i]->getCurrentValue();
                            // select the maximum allowed value
                            for (int j=0; j<allowedValues.size(); j++)
                            {
                                if (allowedValues[j] == SESSION)
                                {
                                    selectedValue = j;
                                    break;
                                }
                                if (allowedValues[j] == ONESHOT)
                                {
                                    selectedValue = j;
                                }
                                else if (allowedValues[j] == DENIED
                                         && allowedValues[selectedValue] == BLANKET)
                                {
                                    selectedValue = j;
                                }
                            }
                            // set the maximum allowed value
                            mutuallyExclusiveList[i]->setCurrentValue(selectedValue);
                            HbComboBox * settingsCombo = itemToComboBox(
                                                             mutuallyExclusiveList[i]->getId());
                            settingsCombo->setCurrentIndex(selectedValue);
                            writeToStorage(*mutuallyExclusiveList[i]);
                        }
                        i++;
                        // end of list?
                        if (i >= mutuallyExclusiveList.size())
                        {
                            break;
                        }
                        // move on into the list
                        isBlanket = (mutuallyExclusiveList[i]->getValue(
                                         mutuallyExclusiveList[i]->getCurrentValue()) == BLANKET);
                    }
                    while (true);
                }
                return isBlanketAllowed;
            }
        }
    }
    return true;
}

void JavaApplicationSettingsViewPrivate::_q_settingsChanged(const QString &newValue)
{
    HbComboBox * sender = static_cast<HbComboBox*>(iPublicView->sender());
    if (sender)
    {
        JavaApplicationSettings* settings = findSettings(sender);
        if (settings != NULL)
        {
            // security warnings
            if (settings->getName() == SECURITY_LEVEL)
            {
                securityWarningsChanged(newValue);
//                return;
            }
            // any other settings are treated same
            writeSettings(*settings, newValue);
        }
    }
}

void JavaApplicationSettingsViewPrivate::securityWarningsChanged(const QString &newValue)
{
    if (newValue == USER_DEFINED)
    {
        // append the extra settings and values
        for (int i=0; i<extraSettings.size(); i++)
        {
            if (extraSettings[i].getCurrentValue() >= 0)
            {
                HbDataFormModelItem * appSettings = model->appendDataFormItem(
                                                        HbDataFormModelItem::ComboBoxItem,
                                                        extraSettings[i].getName(), securitySettingsGroup);
                extraSettings[i].setId(appSettings);
                appSettings->setContentWidgetData(QString("currentIndex"),extraSettings[i].getCurrentValue());
            }
        }
    }
    else
    {
        // remove the extra settings
        for (int i=0; i<extraSettings.size(); i++)
        {
            if (extraSettings[i].getId())
            {
                HbComboBox * extraSettingsId = itemToComboBox(extraSettings[i].getId());
                if (extraSettingsId)
                {
                    JavaApplicationSettingsView::disconnect(extraSettingsId, 0, 0, 0);
                    model->removeItem(model->indexFromItem(extraSettings[i].getId()));
                    extraSettings[i].setId(0);
                }
            }
        }
    }
}

void JavaApplicationSettingsViewPrivate::_q_dataItemDisplayed(const QModelIndex dataItemIndex)
{
    HbDataFormModelItem *item = static_cast<HbDataFormModel*>(
                                    mainForm->model())->itemFromIndex(dataItemIndex);
    int itemType = item->data(HbDataFormModelItem::ItemTypeRole).toInt();
    if (HbDataFormModelItem::DataItemType(itemType)
            == HbDataFormModelItem::ComboBoxItem)
    {
        HbComboBox * comboBox = static_cast<HbComboBox*>(
                                    mainForm->dataFormViewItem(dataItemIndex)->dataItemContentWidget());
        // add the extra settings values
        for (int i=0; i<extraSettings.size(); i++)
        {
            if (extraSettings[i].getId() == item)
            {
                if (comboBox->count() == 0)
                {
                    comboBox->addItems(extraSettings[i].getValues());
                }
                comboBox->setCurrentIndex(extraSettings[i].getCurrentValue());
                break;
            }
        }
        iPublicView->connect(comboBox,
                             SIGNAL(currentIndexChanged(const QString &)),
                             iPublicView, SLOT(_q_settingsChanged(const QString &)));
    }
}

JavaApplicationSettings* JavaApplicationSettingsViewPrivate::findSettings(HbComboBox* id)
{
    JavaApplicationSettings* settings = findSettings(id, generalSettings);
    if (settings == NULL)
    {
        settings = findSettings(id, securitySettings);
        if (settings == NULL)
        {
            settings = findSettings(id, extraSettings);
        }
    }
    return settings;
}

JavaApplicationSettings* JavaApplicationSettingsViewPrivate::findSettings(HbComboBox* id, QVector<JavaApplicationSettings>& allSettings)
{
    for (int i=0; i<allSettings.size(); i++)
    {
        if (itemToComboBox(allSettings[i].getId()) == id)
        {
            return &allSettings[i];
        }
    }
    return NULL;
}

HbComboBox * JavaApplicationSettingsViewPrivate::itemToComboBox(const HbDataFormModelItem * item)
{
    HbDataFormViewItem * viewItem = mainForm->dataFormViewItem(
                                        model->indexFromItem(item));
    if (viewItem)
    {
        return static_cast<HbComboBox*>(viewItem->dataItemContentWidget());
    }
    return NULL;
}

int JavaApplicationSettingsViewPrivate::readFromStorage(JavaApplicationSettings& settings)
{
    int currentValue = -1;
    if (settings.getColumnName().size() > 0 && settings.getTableName().size() > 0)
    {
        wstring value = L"";

        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, iJavaAppUid);
        query.insert(attr);
        if (settings.getFilterColumnName().size() > 0)
        {
            attr.setEntry(settings.getFilterColumnName(), settings.getFilterColumnValue());
            query.insert(attr);
        }
        attr.setEntry(settings.getColumnName(), L"");
        query.insert(attr);

        try
        {
            iStorage->search(settings.getTableName(), query, queryResult);
        }
        catch (JavaStorageException& aJse)
        {
            // Don't leave. Set defaults.
        }

        findEntry(queryResult, settings.getColumnName(), value);

        if (value.size() > 0)
        {
            vector<wstring> storageValues = settings.getStorageValues();
            for (int i=0; i<storageValues.size(); i++)
            {
                if (storageValues[i] == value)
                {
                    currentValue = i;
                    break;
                }
            }
        }
    }

    settings.setCurrentValue(currentValue);
    return currentValue;
}

void JavaApplicationSettingsViewPrivate::writeToStorage(JavaApplicationSettings& settings)
{
    JavaStorageApplicationEntry_t oldEntry;
    JavaStorageEntry attr;
    attr.setEntry(ID, iJavaAppUid);
    oldEntry.insert(attr);
    JavaStorageApplicationEntry_t entry;
    if (settings.getFilterColumnName().size() > 0)
    {
        attr.setEntry(settings.getFilterColumnName(), settings.getFilterColumnValue());
        oldEntry.insert(attr);
    }
    attr.setEntry(settings.getColumnName(), settings.getStorageValues()[settings.getCurrentValue()]);
    entry.insert(attr);

    try
    {
        iStorage->update(settings.getTableName(), entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
    }
}

void JavaApplicationSettingsViewPrivate::findEntry(const JavaStorageApplicationList_t& queryResult,
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

bool JavaApplicationSettingsViewPrivate::findFromStorage(const std::wstring& aColumnName, const std::wstring& aColumnFilterName, const std::wstring& aColumnFilterValue, const std::string& aTableName)
{
    wstring value = readFromStorage(aColumnName, aColumnFilterName, aColumnFilterValue, aTableName);

    return (value.size() > 0);
}

wstring JavaApplicationSettingsViewPrivate::readFromStorage(const std::wstring& aColumnName, const std::wstring& aColumnFilterName, const std::wstring& aColumnFilterValue, const std::string& aTableName)
{
    wstring value = L"";

    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, iJavaAppUid);
    query.insert(attr);
    if (aColumnFilterName.size() > 0)
    {
        attr.setEntry(aColumnFilterName, aColumnFilterValue);
        query.insert(attr);
    }
    attr.setEntry(aColumnName, L"");
    query.insert(attr);

    try
    {
        iStorage->search(aTableName, query, queryResult);
    }
    catch (JavaStorageException& aJse)
    {
        // Don't leave. Set defaults.
    }

    findEntry(queryResult, aColumnName, value);

    return value;
}


#ifdef WIN32
#include "./moc/moc_javaapplicationsettingsview.cpp"
#else
#include "moc_javaapplicationsettingsview.cpp"
#endif
