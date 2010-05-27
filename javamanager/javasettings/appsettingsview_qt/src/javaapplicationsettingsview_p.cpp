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
#include <hbpushbutton.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hblabel.h>
#include <hbaction.h>
#include <QApplication>
#include <QTranslator>
#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration.h>

#include "javaapplicationsettings.h"
#include "javaapplicationsettingsview.h"
#include "javaapplicationsettingsview_p.h"
#include "connectionmanager.h"
#include "javacommonutils.h"

#include "securitystoragedatadefs.h"
#include "javastoragenames.h"

QTM_USE_NAMESPACE

using namespace java::storage;
using namespace java::util;
using namespace std;

const wchar_t ON_SCREEN_KEYPAD_VALUE_NO[] = L"0";
const wchar_t ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS[] = L"1";
const wchar_t ON_SCREEN_KEYPAD_VALUE_NAVIGATION[] = L"2";

JavaApplicationSettingsViewPrivate::JavaApplicationSettingsViewPrivate(const QString& aJavaAppUid):
        mainForm(0), model(0), generalSettingsGroup(0), securitySettingsGroup(0)
{
    // init access point settings ui
    netConnSettingsUi = new CmApplSettingsUi(this);
    
    // init storage
    iStorage.reset(JavaStorage::createInstance());
    try
    {
        iStorage->open();
    }
    catch (JavaStorageException& aJse) {}
    
    // init the suite UID from the application UID
    readSuiteUid(aJavaAppUid);
    if (iSuiteUid.size() <= 0)
    {
        // TODO: display a no settings available message
        return;
    }
    QTranslator translator;
    // load the correct translation of the localized strings for the cmmanager. 
    // Load this one first since it contains the smallest amount of strings 
    // (so it's ok to be searched last)
    if (translator.load("z:/resource/qt/translations/cmapplsettingsui_" + QLocale::system().name()))
    {
        qApp->installTranslator(&translator);
    }
    if (translator.load("z:/resource/qt/translations/cmmanager_" + QLocale::system().name()))
    {
        qApp->installTranslator(&translator);
    }
    // load the correct translation of the localized strings for the java settings
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
    NETWORK_CONNECTION = hbTrId("txt_occ_title_network_connection");

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
    delete netConnSettingsUi;
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
        readFromStorage(generalSettings[0]);
    }
    settingsValues = QStringList();
    netConn.id = ConnectionManager::getDestinationNetworkIdL(iSuiteUid);
    readNetworkConnectionName();
    settingsValues<<netConn.name;
    generalSettings.append(JavaApplicationSettings(NETWORK_CONNECTION, settingsValues));
    netSettIndex = generalSettings.size() - 1;

    // security settings
    settingsValues = QStringList();
    storageValues.clear();
    settingsValues<<hbTrId("txt_java_sett_setlabel_security_level_val_default")<<USER_DEFINED;
    storageValues.push_back(SECURITY_WARNINGS_DEFAULT_MODE);
    storageValues.push_back(SECURITY_WARNINGS_USER_DEFINED_MODE);
    securitySettings.append(JavaApplicationSettings(SECURITY_LEVEL, settingsValues, SECURITY_WARNINGS, MIDP_PACKAGE_TABLE, storageValues));
    readFromStorage(securitySettings[0]);

    // extra settings
    int i=0;
    settingsValues = QStringList();
    storageValues.clear();
    settingsValues<<ONESHOT<<SESSION<<BLANKET<<DENIED;
    storageValues.push_back(ONESHOT_INTERACTION_MODE);
    storageValues.push_back(SESSION_INTERACTION_MODE);
    storageValues.push_back(BLANKET_INTERACTION_MODE);
    storageValues.push_back(DENIED_INTERACTION_MODE);
    extraSettings.append(JavaApplicationSettings(NET_ACCESS, settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, NET_ACCESS_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(LOW_LEVEL_NET_ACCESS, settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, LOW_LEVEL_NET_ACCESS_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_messaging"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, MESSAGING_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_restricted_messaging"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, RESTRICTED_MESSAGING_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_call_control"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, CALL_CONTROL_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_local_conn"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, LOCAL_CONNECTIVITY_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_mm_record"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, MULTIMEDIA_RECORDING_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_write_data"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, WRITE_USER_DATA_ACCESS_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_read_data"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, READ_USER_DATA_ACCESS_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_location"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, LOCATION_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_landmarks"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, LANDMARK_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_auth"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, AUTHENTICATION_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_smartcard"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, SMART_CARD_COMMUNICATION_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_app_auto_invoc"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, APPLICATION_AUTO_INVOCATION_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_broadcast"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, BROADCAST_SETTINGS, ALLOWED_SETTINGS));
    readFromStorage(extraSettings[i]);
    i++;
    extraSettings.append(JavaApplicationSettings(hbTrId("txt_java_sett_setlabel_nfc_write_access"), settingsValues, CURRENT_SETTING, MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, NFC_WRITE_ACCESS_SETTINGS, ALLOWED_SETTINGS));
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
        HbDataFormModelItem * appSettings = NULL;
        int storageValue = 0;
        int currentValue = 0;        
        switch(settings[i].getValues().size())
        {
            case 1:
                appSettings  = model->appendDataFormItem(
                        HbDataFormModelItem::ToggleValueItem,
                        settings[i].getName(), parent);
                appSettings->setContentWidgetData(QString("text"), settings[i].getValues()[0]);
                appSettings->setContentWidgetData(QString("additionalText"), settings[i].getValues()[0]);
                break;
            case 2:
                appSettings  = model->appendDataFormItem(
                        HbDataFormModelItem::ToggleValueItem,
                        settings[i].getName(), parent);
                storageValue = readFromStorage(settings[i]);
                if (storageValue >= 0)
                {
                    currentValue = storageValue;
                }
                // make it a toogle button (-> get rid of the "pressed" ui effect)
                appSettings->setContentWidgetData(QString("text"), settings[i].getValues()[settings[i].getCurrentValue()]);
                appSettings->setContentWidgetData(QString("additionalText"),settings[i].getValues()[1 - currentValue]);
                break;
            default:
                appSettings  = model->appendDataFormItem(
                        HbDataFormModelItem::ComboBoxItem,
                        settings[i].getName(), parent);
                appSettings->setContentWidgetData(QString("items"), settings[i].getValues());
                appSettings->setContentWidgetData(QString("currentIndex"),settings[i].getCurrentValue());
        }
        settings[i].setId(appSettings);
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
                HbComboBox * settingsCombo = static_cast<HbComboBox*>(itemToWidget(settings.getId()));
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
                if (!securityWarningAccepted(
                    secWarning,OK,CANCEL ,SECURITY_WARNING_TITLE))
                {
                    return false;
                }
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
                bool isBlanketAllowed = securityWarningAccepted(
                    MUTUALLY_EXCLUSIVE_SETTINGS, OK, CANCEL, SECURITY_WARNING_TITLE);
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
                            HbComboBox * settingsCombo = static_cast<HbComboBox*>(itemToWidget(
                                                             mutuallyExclusiveList[i]->getId()));
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

void JavaApplicationSettingsViewPrivate::_q_settingsChanged(bool)
{
    HbPushButton * sender = static_cast<HbPushButton*>(iPublicView->sender());
    if (sender)
    {
        JavaApplicationSettings* settings = findSettings(sender);
        if (settings->getName() == SECURITY_LEVEL)
        {
            QString newValue = settings->getValue(1 - settings->getCurrentValue());
            securityWarningsChanged(newValue);
            writeSettings(*settings, newValue);
        }
        else if (settings->getName() == NETWORK_CONNECTION)
        {
            handleNetworkSettings();
        }
    }
}
void JavaApplicationSettingsViewPrivate::_q_settingsChanged(const QString &newValue)
{
    HbComboBox * sender = static_cast<HbComboBox*>(iPublicView->sender());
    if (sender)
    {
        JavaApplicationSettings* settings = findSettings(sender);
        if (settings != NULL)
        {
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
            if (extraSettings[i].getCurrentValue() >= 0 && extraSettings[i].getId() == 0)
            {
                HbDataFormModelItem * appSettings = model->appendDataFormItem(
                                                        HbDataFormModelItem::ComboBoxItem,
                                                        extraSettings[i].getName(), securitySettingsGroup);
                extraSettings[i].setId(appSettings);
                // when settings the items, the settingsChanged signal gets emitted with the first value 
                // as being the current value. This signal should be discarded (the first value is just 
                // an intermediate value towards the actual current value which gets set shortly). 
                // Therefore set the first value as the current value (this way the settingsChanged 
                // signal gets discarded) and change the current value back to the real current value
                int currentValue = extraSettings[i].getCurrentValue();
                extraSettings[i].setCurrentValue(0);
                appSettings->setContentWidgetData(QString("items"), extraSettings[i].getValues());                
                extraSettings[i].setCurrentValue(currentValue);
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
                HbComboBox * extraSettingsId = static_cast<HbComboBox*>(itemToWidget(extraSettings[i].getId()));
                if (extraSettingsId)
                {
                    iPublicView->disconnect(extraSettingsId, 0, 0, 0);
                    model->removeItem(model->indexFromItem(extraSettings[i].getId()));
                    extraSettings[i].setId(0);
                }
            }
        }
    }
}

void JavaApplicationSettingsViewPrivate::handleNetworkSettings()
{
    // init flags to show destinations and connection methods
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowDestinations;
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
        
    // empty filter -> all bearer types included
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    // reset the result
    netConnSelection.result = CmApplSettingsUi::SelectionTypeDestination;       
    netConnSelection.id = netConn.id;
    // init settings ui with destinations, filter and initial selection
    netConnSettingsUi->setOptions(listItems, filter);
    netConnSettingsUi->setSelection(netConnSelection);
    // Connect the setting ui's signal with own slot where the netConnSelection is stored
    connect(netConnSettingsUi, SIGNAL(finished(uint)), this, SLOT(netConnSelected(uint)));
    // launch the setting ui    
    netConnSettingsUi->open();
}

void JavaApplicationSettingsViewPrivate::_q_dataItemDisplayed(const QModelIndex dataItemIndex)
{
    HbDataFormModelItem *item = static_cast<HbDataFormModel*>(
                                    mainForm->model())->itemFromIndex(dataItemIndex);
    int itemType = item->data(HbDataFormModelItem::ItemTypeRole).toInt();
    HbComboBox * comboBox = NULL;
    HbPushButton * pushButton = NULL;    
    switch(HbDataFormModelItem::DataItemType(itemType))
    {
        case HbDataFormModelItem::ComboBoxItem:
            comboBox = static_cast<HbComboBox*>(
                (qobject_cast<HbDataFormViewItem *> 
                (mainForm->itemByIndex(dataItemIndex)))->dataItemContentWidget());
            iPublicView->connect(comboBox,
                                 SIGNAL(currentIndexChanged(const QString &)),
                                 iPublicView, SLOT(_q_settingsChanged(const QString &)),
                                 Qt::UniqueConnection);
            break;
        case HbDataFormModelItem::ToggleValueItem:
            pushButton = static_cast< HbPushButton*>(
                (qobject_cast<HbDataFormViewItem *> 
                (mainForm->itemByIndex(dataItemIndex)))->dataItemContentWidget());
            iPublicView->connect(pushButton,
                                 SIGNAL(clicked(bool)),
                                 iPublicView, SLOT(_q_settingsChanged(bool)),
                                 Qt::UniqueConnection);
            break;
    }
}

void JavaApplicationSettingsViewPrivate::netConnSelected(uint netConnSelectionStatus)
{
    if (netConnSelectionStatus == CmApplSettingsUi::ApplSettingsErrorNone) 
    {    
        netConnSelection = netConnSettingsUi->selection();
        // store the selection
        if (netConnSelection.id != netConn.id)
        {
            ConnectionManager::setDestinationNetworkIdL(iSuiteUid, netConnSelection.id);
            netConn.id = netConnSelection.id;
            readNetworkConnectionName();
            generalSettings[netSettIndex].getId()->setContentWidgetData(QString("text"), netConn.name);
            generalSettings[netSettIndex].getId()->setContentWidgetData(QString("additionalText"), netConn.name);
        }
    }
}

void JavaApplicationSettingsViewPrivate::readNetworkConnectionName()
{
    QNetworkConfigurationManager manager;
    QNetworkConfiguration cfg;    
    if (netConn.id == 0)
    {
        // figure out the default configuration
        cfg = manager.defaultConfiguration();
    }
    else
    {
        cfg = manager.configurationFromIdentifier(QString::number(netConn.id));
    }
    if (cfg.isValid())
    {
        netConn.name = hbTrId(cfg.name().toUtf8());
    }
}

JavaApplicationSettings* JavaApplicationSettingsViewPrivate::findSettings(HbWidget* id)
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

JavaApplicationSettings* JavaApplicationSettingsViewPrivate::findSettings(HbWidget* id, QVector<JavaApplicationSettings>& allSettings)
{
    for (int i=0; i<allSettings.size(); i++)
    {
        if (itemToWidget(allSettings[i].getId()) == id)
        {
            return &allSettings[i];
        }
    }
    return NULL;
}

HbWidget * JavaApplicationSettingsViewPrivate::itemToWidget(const HbDataFormModelItem * item)
{
    HbDataFormViewItem * viewItem = qobject_cast<HbDataFormViewItem *> (mainForm->itemByIndex(
                                        model->indexFromItem(item)));
    if (viewItem)
    {
        return static_cast<HbWidget*>(viewItem->dataItemContentWidget());
    }
    return NULL;
}

bool JavaApplicationSettingsViewPrivate::securityWarningAccepted(const QString& text, const QString& acceptActionLabel, const QString& rejectActionLabel, const QString& headingText)
{
    return true;
/*    HbMessageBox securityWarning(HbMessageBox::MessageTypeQuestion);
    securityWarning.setDismissPolicy(HbDialog::NoDismiss);
    securityWarning.setTimeout(HbDialog::NoTimeout);
    securityWarning.setIconVisible(false);
    securityWarning.setText(text);
    securityWarning.addAction(new HbAction(
            acceptActionLabel, &securityWarning));
    securityWarning.addAction(new HbAction(
           rejectActionLabel, &securityWarning));
   securityWarning.setHeadingWidget(new HbLabel(headingText));
   HbAction *selected = securityWarning.exec();
   return (selected == securityWarning.primaryAction());*/
}

int JavaApplicationSettingsViewPrivate::readFromStorage(JavaApplicationSettings& settings)
{
    int currentValue = -1;
    if (settings.getColumnName().size() > 0 
            && settings.getTableName().size() > 0)
    {
        wstring value = L"";

        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, iSuiteUid);
        query.insert(attr);
        if (settings.getFilterColumnName().size() > 0)
        {
            attr.setEntry(settings.getFilterColumnName(), settings.getFilterColumnValue());
            query.insert(attr);
        }
        attr.setEntry(settings.getColumnName(), L"");
        query.insert(attr);
        if (settings.getValuesColumnName().size() > 0)
        {
            attr.setEntry(settings.getValuesColumnName(), L"");
            query.insert(attr);
        }

        try
        {
            iStorage->search(settings.getTableName(), query, queryResult);
        }
        catch (JavaStorageException& aJse)
        {
            // Don't leave. Set defaults.
        }

        if (settings.getValuesColumnName().size() > 0)
        {
            findEntry(queryResult, settings.getValuesColumnName(), value);
            if (value.size() > 0)
            {
                settings.setStorageValuesFilter(value);
                filterSecuritySettings(settings);
            }
        }
        
        value = L"";
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

void JavaApplicationSettingsViewPrivate::filterSecuritySettings(JavaApplicationSettings& settings)
{
    std::wstring allowedModes = settings.getStorageValuesFilter();
    if (allowedModes.size() <= 0)
    {
            return;
    }
    // The allowed modes info is stored as a 4-bit constant:
    //    X(oneshot)X(session)X(blanket)X(no)
    // e.g. 1011 (=11) means that oneshot, blanket and no are allowed.
    // The following constants are used to encode/decode the allowed modes
    // into/from a 4-bit number
    int INTERACTION_MODE_DENIED = 1;
    int INTERACTION_MODE_BLANKET = 2;
    int INTERACTION_MODE_SESSION = 4;
    int INTERACTION_MODE_ONESHOT = 8;
    TInt allowedInteractionModes = JavaCommonUtils::wstringToInt(allowedModes);
    int index = 0;
    int tmp = allowedInteractionModes & INTERACTION_MODE_ONESHOT;
    if (tmp <= 0)
    {
        settings.removeValue(index); 
        index--;
    }
    index++;
    tmp = allowedInteractionModes & INTERACTION_MODE_SESSION;
    if (tmp <= 0)
    {
        settings.removeValue(index); 
        index--;
    }
    index++;
    tmp = allowedInteractionModes & INTERACTION_MODE_BLANKET;
    if (tmp <= 0)
    {
        settings.removeValue(index); 
        index--;
    }
    index++;
    tmp = allowedInteractionModes & INTERACTION_MODE_DENIED;
    if (tmp <= 0)
    {
        settings.removeValue(index); 
    }
}

void JavaApplicationSettingsViewPrivate::writeToStorage(JavaApplicationSettings& settings)
{
    if (settings.getTableName().size() <= 0 
            || settings.getColumnName().size() <= 0)
    {
        // storage details are not provided -> no point in going further
        return;
    }
    JavaStorageApplicationEntry_t oldEntry;
    JavaStorageEntry attr;
    attr.setEntry(ID, iSuiteUid);
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
    attr.setEntry(ID, iSuiteUid);
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

void JavaApplicationSettingsViewPrivate::readSuiteUid(const QString& aAppUid)
{
    iSuiteUid = L"";
    bool ok;
    int tmp = aAppUid.toInt( &ok, 10 );
    if (!ok)
    {
        return;
    }
    wstring appUid = L""; 
    appUid.append(L"[").append(QString("%1").arg(tmp, 0, 16).toStdWString()).append(L"]");

    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, appUid);
    query.insert(attr);
    attr.setEntry(PACKAGE_ID, L"");
    query.insert(attr);
    try
    {
        iStorage->search(APPLICATION_TABLE, query, queryResult);
    }
    catch (JavaStorageException& aJse)
    {
        // Don't leave. Set defaults.
    }
    findEntry(queryResult, PACKAGE_ID, iSuiteUid);
}


#ifdef WIN32
#include "./moc/moc_javaapplicationsettingsview.cpp"
#else
#include "moc_javaapplicationsettingsview.cpp"
#endif
