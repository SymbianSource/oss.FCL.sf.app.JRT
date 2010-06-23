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
#include <algorithm>

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
        mainForm(0), model(0), generalSettingsGroup(0), securitySettingsGroup(0), netConnSettingsUi(0), asyncToSyncCallEventLoop(0), secWarningAccepted(false), defaultConnId(0)
{
    // init storage
    iStorage.reset(JavaStorage::createInstance());
    try
    {
        iStorage->open();
    }
    catch (JavaStorageException& aJse) {}
    
    QTranslator translator;
    // load the correct translation of the localized strings for the cmmanager. 
    // Load this one first since it contains the smallest amount of strings 
    // (so it's ok to be searched last)
    installTranslator("cmapplsettingsui");
    installTranslator("cmmanager");
    // load the correct translation of the localized strings for the java settings
    // Current solution reads it from Z only (this does not work with IAD)
    // -> check if translator can handle path without drive letter (e.g. the resource
    // is loaded from the same drive where the DLL is loaded)
    installTranslator("javaruntimeapplicationsettings");
    // init strings
    BLANKET = QString(hbTrId("txt_java_sett_setlabel_permission_val_blanket"));
    SESSION = QString(hbTrId("txt_java_sett_setlabel_permission_val_session"));
    ONESHOT = QString(hbTrId("txt_java_sett_setlabel_permission_val_oneshot"));
    DENIED = QString(hbTrId("txt_java_sett_setlabel_permission_val_no"));
    SECURITY_LEVEL = QString(hbTrId("txt_java_sett_setlabel_security_level"));
    USER_DEFINED = QString(hbTrId("txt_java_sett_setlabel_security_level_val_user_defined"));
    SENSITIVE_SETTINGS = QString(hbTrId("txt_java_sett_info_query_perm_sec"));
    SENSITIVE_SETTINGS_NET_USAGE = QString(hbTrId("txt_java_sett_info_query_perm_net"));
    MUTUALLY_EXCLUSIVE_SETTINGS = QString(hbTrId("txt_java_sett_info_query_perm_warn"));
    OK = QString(hbTrId("txt_java_sett_button_settings_ok"));
    CANCEL = QString(hbTrId("txt_java_sett_button_settings_cancel"));
    SECURITY_WARNING_TITLE = QString(hbTrId("txt_java_sett_title_note_security_warn"));
    NET_ACCESS = QString(hbTrId("txt_java_sett_setlabel_net_access"));
    LOW_LEVEL_NET_ACCESS = QString(hbTrId("txt_java_sett_setlabel_low_level_net_access"));
    NETWORK_CONNECTION = QString(hbTrId("txt_occ_title_network_connection"));
    SETTINGS_TITLE = QString(hbTrId("txt_java_sett_title_settings"));
    SETTINGS_NOT_AVAILABLE = QString(hbTrId("txt_java_sett_info_settings_not_available"));

    // init the suite UID from the application UID
    readSuiteUid(aJavaAppUid);
    if (iSuiteUid.size() <= 0)
    {
        return;
    }
    
    // init access point settings ui
    netConnSettingsUi = new CmApplSettingsUi(this);
    asyncToSyncCallEventLoop = new QEventLoop();
    
    // read all settings
    readAllSettings();

    // init form
    mainForm = new HbDataForm();
    mainForm->setHeading(QString::fromStdWString(readFromStorage(PACKAGE_NAME, L"", L"", APPLICATION_PACKAGE_TABLE)));
    model = new HbDataFormModel();

    // init settings
    generalSettingsGroup = model->appendDataFormGroup(
        QString(hbTrId("txt_java_sett_subtitle_general")), model->invisibleRootItem());
    initSettings(generalSettings, generalSettingsGroup);
    // append the security settings only if there are any user settings
    if (extraSettings.size() > 0)
    {
        securitySettingsGroup = model->appendDataFormGroup(
            QString(hbTrId("txt_java_sett_subtitle_security")), model->invisibleRootItem());
        initSettings(securitySettings, securitySettingsGroup);

        // if security warnings is user defined -> add the extra settings, expand
        if (securitySettings[0].getCurrentValue() == 1)
        {
            securityWarningsChanged(USER_DEFINED);
        }
    }

    // link form and model
    mainForm->setModel(model);
    mainForm->setExpanded(model->indexFromItem(generalSettingsGroup), true);
    if (extraSettings.size() > 0)
    {
        mainForm->setExpanded(model->indexFromItem(securitySettingsGroup), (securitySettings[0].getCurrentValue() == 1));
    }
}

void JavaApplicationSettingsViewPrivate::init(JavaApplicationSettingsView* aPublicView)
{
    iPublicView = aPublicView;

    if (mainForm)
    {        
        // do the connect for the main form
        iPublicView->connect(mainForm, SIGNAL(itemShown(const QModelIndex)),
                             iPublicView, SLOT(_q_dataItemDisplayed(const QModelIndex)));
    
        // set the form as view's widget
        iPublicView->setWidget(mainForm);
    }
    else
    {
        // no settings available
        HbLabel* label = new HbLabel(SETTINGS_NOT_AVAILABLE, iPublicView);
        label->setAlignment(Qt::AlignVCenter);
    }        

    // set title
    iPublicView->setTitle(SETTINGS_TITLE);
}

JavaApplicationSettingsViewPrivate::~JavaApplicationSettingsViewPrivate()
{
    delete mainForm;
    delete model;
    delete netConnSettingsUi;
    delete asyncToSyncCallEventLoop;
    foreach (QTranslator *translator, translators) {
        qApp->removeTranslator(translator);
    }    
    qDeleteAll(translators);
    translators.clear();
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
        settingsValues<<QString(hbTrId("txt_java_sett_setlabel_osk_val_no"))<<QString(hbTrId("txt_java_sett_setlabel_osk_val_game"))<<QString(hbTrId("txt_java_sett_setlabel_osk_val_navigation"));
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_NO);
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS);
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_NAVIGATION);
        generalSettings.append(JavaApplicationSettings(QString(hbTrId("txt_java_sett_setlabel_osk")), settingsValues, ON_SCREEN_KEYPAD, MIDP_PACKAGE_TABLE, storageValues));
        readFromStorage(generalSettings[0]);
    }
    settingsValues = QStringList();
    initNetworkConnection();
    settingsValues<<netConn.name;
    generalSettings.append(JavaApplicationSettings(NETWORK_CONNECTION, settingsValues));
    netSettIndex = generalSettings.size() - 1;

    // security settings
    settingsValues = QStringList();
    storageValues.clear();
    settingsValues<<QString(hbTrId("txt_java_sett_setlabel_security_level_val_default"))<<USER_DEFINED;
    storageValues.push_back(SECURITY_WARNINGS_DEFAULT_MODE);
    storageValues.push_back(SECURITY_WARNINGS_USER_DEFINED_MODE);
    securitySettings.append(JavaApplicationSettings(SECURITY_LEVEL, settingsValues, SECURITY_WARNINGS, MIDP_PACKAGE_TABLE, storageValues));
    readFromStorage(securitySettings[0]);

    // extra settings
    settingsValues = QStringList();
    storageValues.clear();
    settingsValues<<ONESHOT<<SESSION<<BLANKET<<DENIED;
    storageValues.push_back(ONESHOT_INTERACTION_MODE);
    storageValues.push_back(SESSION_INTERACTION_MODE);
    storageValues.push_back(BLANKET_INTERACTION_MODE);
    storageValues.push_back(DENIED_INTERACTION_MODE);
    // init the known localized settings names
    QHash<QString, QString> localizedSettingsNames;
    localizedSettingsNames[QString::fromStdWString(NET_ACCESS_SETTINGS)] = NET_ACCESS; 
    localizedSettingsNames[QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS)] = LOW_LEVEL_NET_ACCESS;
    localizedSettingsNames[QString::fromStdWString(MESSAGING_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_messaging"));
    localizedSettingsNames[QString::fromStdWString(RESTRICTED_MESSAGING_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_restricted_messaging"));
    localizedSettingsNames[QString::fromStdWString(CALL_CONTROL_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_call_control"));
    localizedSettingsNames[QString::fromStdWString(LOCAL_CONNECTIVITY_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_local_conn"));
    localizedSettingsNames[QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_mm_record"));
    localizedSettingsNames[QString::fromStdWString(WRITE_USER_DATA_ACCESS_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_write_data"));
    localizedSettingsNames[QString::fromStdWString(READ_USER_DATA_ACCESS_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_read_data"));
    localizedSettingsNames[QString::fromStdWString(LOCATION_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_location"));
    localizedSettingsNames[QString::fromStdWString(LANDMARK_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_landmarks"));
    localizedSettingsNames[QString::fromStdWString(AUTHENTICATION_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_auth"));
    localizedSettingsNames[QString::fromStdWString(SMART_CARD_COMMUNICATION_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_smartcard"));
    localizedSettingsNames[QString::fromStdWString(APPLICATION_AUTO_INVOCATION_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_app_auto_invoc"));
    localizedSettingsNames[QString::fromStdWString(BROADCAST_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_broadcast"));
    localizedSettingsNames[QString::fromStdWString(NFC_WRITE_ACCESS_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_nfc_write_access"));
    localizedSettingsNames[QString::fromStdWString(URL_START_SETTINGS)] = QString(hbTrId("txt_java_sett_setlabel_url_start"));
    vector<IndexedSettingsName> allSecuritySettings = readFromStorage(FUNCTION_GROUP, MIDP_FUNC_GRP_SETTINGS_TABLE, MIDP_PERMISSIONS_TABLE);
    // sort the security settings according to how they should be displayed
    std::sort(allSecuritySettings.begin(), allSecuritySettings.end(), AscendingSort());
    QHash<QString, int> settingsIndexes;
    for(int i=0; i<allSecuritySettings.size(); i++)
    {
        QString rawSettingsName = QString::fromStdWString(allSecuritySettings[i].name);
        QString localizedSettingsName = rawSettingsName;
        if (localizedSettingsNames.contains(rawSettingsName))
            localizedSettingsName = localizedSettingsNames.value(rawSettingsName);
        extraSettings.append(JavaApplicationSettings(
            localizedSettingsName, settingsValues, CURRENT_SETTING, 
            MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP, 
            allSecuritySettings[i].name, ALLOWED_SETTINGS));
        readFromStorage(extraSettings[i]);
        settingsIndexes[rawSettingsName] = i;
    }

    // configure the high risk lists
    QList<JavaApplicationSettings*> highRiskList;
    QStringList highRiskSettingsNames;
    highRiskSettingsNames
        <<QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS)
        <<QString::fromStdWString(READ_USER_DATA_ACCESS_SETTINGS);
    configureList(highRiskList, highRiskSettingsNames, settingsIndexes);
    // net access with multimedia and read user data
    attachList(QString::fromStdWString(NET_ACCESS_SETTINGS),highRiskList, settingsIndexes);
    // low level net access with multimedia and read user data
    attachList(QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS),highRiskList, settingsIndexes);
    // messaging with multimedia and read user data
    attachList(QString::fromStdWString(MESSAGING_SETTINGS),highRiskList, settingsIndexes);
    // restricted messaging with multimedia and read user data
    attachList(QString::fromStdWString(RESTRICTED_MESSAGING_SETTINGS),highRiskList, settingsIndexes);
    // call control with multimedia and read user data
    attachList(QString::fromStdWString(CALL_CONTROL_SETTINGS),highRiskList, settingsIndexes);
    // local connectivity with multimedia and read user data
    attachList(QString::fromStdWString(LOCAL_CONNECTIVITY_SETTINGS),highRiskList, settingsIndexes);
    // multimedia with net access, low level net access, messaging,
    // restricted messaging, call control and local connectivity
    highRiskList.clear();
    highRiskSettingsNames.clear();
    highRiskSettingsNames
        <<QString::fromStdWString(NET_ACCESS_SETTINGS)
        <<QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS)
        <<QString::fromStdWString(MESSAGING_SETTINGS)
        <<QString::fromStdWString(RESTRICTED_MESSAGING_SETTINGS)
        <<QString::fromStdWString(CALL_CONTROL_SETTINGS)
        <<QString::fromStdWString(LOCAL_CONNECTIVITY_SETTINGS)
        <<QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS);
    configureList(highRiskList, highRiskSettingsNames, settingsIndexes);
    attachList(QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS),highRiskList, settingsIndexes);
    // read user data with net access, low level net access, messaging,
    // restricted messaging, call control and local connectivity
    attachList(QString::fromStdWString(READ_USER_DATA_ACCESS_SETTINGS),highRiskList, settingsIndexes);

    // configure the mutually exclusive list
    QList<JavaApplicationSettings*> mutuallyExclusiveList;
    QStringList mutuallyExclusiveSettingsNames;
    mutuallyExclusiveSettingsNames
        <<QString::fromStdWString(APPLICATION_AUTO_INVOCATION_SETTINGS);
    configureList(mutuallyExclusiveList, mutuallyExclusiveSettingsNames, settingsIndexes);
    //net access with application auto invocation
    attachList(QString::fromStdWString(NET_ACCESS_SETTINGS),mutuallyExclusiveList, settingsIndexes, false /* is high risk list */);
    //low level net access with application auto invocation
    attachList(QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS),mutuallyExclusiveList, settingsIndexes, false /* is high risk list */);
    //application auto invocation with net access and low level net access
    mutuallyExclusiveList.clear();
    mutuallyExclusiveSettingsNames.clear();
    mutuallyExclusiveSettingsNames
        <<QString::fromStdWString(NET_ACCESS_SETTINGS)
        <<QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS);
    configureList(mutuallyExclusiveList, mutuallyExclusiveSettingsNames, settingsIndexes);
    attachList(QString::fromStdWString(APPLICATION_AUTO_INVOCATION_SETTINGS),mutuallyExclusiveList, settingsIndexes, false /* is high risk list */);
}

void JavaApplicationSettingsViewPrivate::initSettings(QVector<JavaApplicationSettings>& settings, HbDataFormModelItem * parent)
{
    for (int i=0; i<settings.size(); i++)
    {
        HbDataFormModelItem * appSettings = NULL;
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
                currentValue = settings[i].getCurrentValue();
                if (currentValue < 0)
                {
                    currentValue = 0;
                }
                // make it a toogle button (-> get rid of the "pressed" ui effect)
                appSettings->setContentWidgetData(QString("text"), settings[i].getValues()[currentValue]);
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
                    extraSettings[i].disconnectFromUi();
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
    netConnSelection.result = netConn.type;       
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
    HbWidget * widget = (qobject_cast<HbDataFormViewItem *> 
        (mainForm->itemByIndex(dataItemIndex)))->dataItemContentWidget();
    JavaApplicationSettings* settings = findSettings(widget);
    if (settings == NULL || settings->isConnectedToUi())
    {
        return;
    }
    settings->connectToUi();
    switch(HbDataFormModelItem::DataItemType(itemType))
    {
        case HbDataFormModelItem::ComboBoxItem:
            comboBox = static_cast<HbComboBox*>(widget);
            iPublicView->connect(comboBox,
                                 SIGNAL(currentIndexChanged(const QString &)),
                                 iPublicView, SLOT(_q_settingsChanged(const QString &)),
                                 Qt::UniqueConnection);
            break;
        case HbDataFormModelItem::ToggleValueItem:
            pushButton = static_cast< HbPushButton*>(widget);
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
        if (netConnSelection.id != netConn.id 
            && netConnSelection.id != defaultConnId)
        {
            TRAPD(err, 
                switch(netConnSelection.result)
                {
                    case CmApplSettingsUi::SelectionTypeDestination:
                        ConnectionManager::setDestinationNetworkIdL(iSuiteUid, netConnSelection.id);
                        break;
                    case CmApplSettingsUi::SelectionTypeConnectionMethod:
                        ConnectionManager::setApnIdL(iSuiteUid, netConnSelection.id);
                        break;
                }                
            );
            if (err == KErrNone)
            {
                netConn.id = netConnSelection.id;
                netConn.type = netConnSelection.result;
                readNetworkConnectionName();
                generalSettings[netSettIndex].getId()->setContentWidgetData(QString("text"), netConn.name);
                generalSettings[netSettIndex].getId()->setContentWidgetData(QString("additionalText"), netConn.name);
            }
        }
    }
}

void JavaApplicationSettingsViewPrivate::securityWarningDismissed(HbAction* action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    secWarningAccepted = (action == dlg->actions().at(0));
    asyncToSyncCallEventLoop->exit();
}

void JavaApplicationSettingsViewPrivate::initNetworkConnection()
{
    TRAP_IGNORE(
        netConn.id = ConnectionManager::getDestinationNetworkIdL(iSuiteUid);
        if (netConn.id == (uint)KJavaNetworkAccessNotSpecified)
        {
            netConn.id = ConnectionManager::getApnIdL(iSuiteUid);
        }
    );
    QNetworkConfigurationManager manager;
    QNetworkConfiguration defaultCfg = manager.defaultConfiguration();
    defaultConnId = defaultCfg.identifier().toInt();
    QNetworkConfiguration cfg = manager.configurationFromIdentifier(
        QString::number(netConn.id));
    if (!cfg.isValid())
    {
        cfg = defaultCfg;
    }
    netConn.name = QString(hbTrId(cfg.name().toUtf8()));
}

void JavaApplicationSettingsViewPrivate::readNetworkConnectionName()
{
    QNetworkConfigurationManager manager;
    QNetworkConfiguration cfg = manager.configurationFromIdentifier(
        QString::number(netConn.id));
    if (!cfg.isValid())
    {
        cfg = manager.defaultConfiguration();
    }
    netConn.name = QString(hbTrId(cfg.name().toUtf8()));
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
    HbMessageBox securityWarning(HbMessageBox::MessageTypeQuestion);
    securityWarning.setDismissPolicy(HbDialog::NoDismiss);
    securityWarning.setTimeout(HbDialog::NoTimeout);
    securityWarning.setIconVisible(false);
    securityWarning.setText(text);
    securityWarning.addAction(new HbAction(
            acceptActionLabel, &securityWarning));
    securityWarning.addAction(new HbAction(
            rejectActionLabel, &securityWarning));
    securityWarning.setHeadingWidget(new HbLabel(headingText));
    securityWarning.open(this, SLOT(securityWarningDismissed(HbAction*)));
    asyncToSyncCallEventLoop->exec();
    return secWarningAccepted;
}

void JavaApplicationSettingsViewPrivate::configureList(QList<JavaApplicationSettings*>& list, const QStringList& settingsNames,  const QHash<QString, int>& settingsIndexes)
{
    for(int i=0; i<settingsNames.size(); i++)
    {
        if (settingsIndexes.contains(settingsNames[i]))
        {
            list << &(extraSettings[settingsIndexes.value(settingsNames[i])]);
        }
    }
}

void JavaApplicationSettingsViewPrivate::attachList(const QString& settingsName, const QList<JavaApplicationSettings*>& list, const QHash<QString, int>& settingsIndexes, bool isHighRiskList)
{
    if (settingsIndexes.contains(settingsName))
    {
        if (isHighRiskList)
        {
            extraSettings[settingsIndexes.value(settingsName)].setHighRiskList(list);
        }
        else
        {
            extraSettings[settingsIndexes.value(settingsName)].setMutuallyExclusiveList(list);
        }
    }
}

void JavaApplicationSettingsViewPrivate::installTranslator(const QString& translationFileName)
{
    QTranslator* translator = new QTranslator;

    // load the correct translation of the localized strings for the java settings
    // Current solution reads it from Z only (this does not work with IAD)
    // -> check if translator can handle path without drive letter (e.g. the resource
    // is loaded from the same drive where the DLL is loaded)
    if (translator->load("z:/resource/qt/translations/" 
        + translationFileName + "_" + QLocale::system().name()))
    {
        translators.append(translator); 
        qApp->installTranslator(translator);
    } 
    else 
    {
        delete translator; 
        translator = NULL;
    }
}

void JavaApplicationSettingsViewPrivate::readFromStorage(JavaApplicationSettings& settings)
{
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
                    settings.setCurrentValue(i);
                    break;
                }
            }
        }
    }
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

vector<IndexedSettingsName> JavaApplicationSettingsViewPrivate::readFromStorage(const std::wstring& aColumnName, const std::string& aPrimaryTableName, const std::string& aSecondaryTableName)
{
    vector<IndexedSettingsName> values;
    
    // hash for associating settings names with display indexes: 
    // a settings with lower index is shown before a settings 
    // with a higher index
    QHash<QString, int> settingsNamesIndexes;
    settingsNamesIndexes[QString::fromStdWString(NET_ACCESS_SETTINGS)] = 0; 
    settingsNamesIndexes[QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS)] = 1;
    settingsNamesIndexes[QString::fromStdWString(MESSAGING_SETTINGS)] = 2;
    settingsNamesIndexes[QString::fromStdWString(RESTRICTED_MESSAGING_SETTINGS)] = 3;
    settingsNamesIndexes[QString::fromStdWString(CALL_CONTROL_SETTINGS)] = 4;
    settingsNamesIndexes[QString::fromStdWString(LOCAL_CONNECTIVITY_SETTINGS)] = 5;
    settingsNamesIndexes[QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS)] = 6;
    settingsNamesIndexes[QString::fromStdWString(WRITE_USER_DATA_ACCESS_SETTINGS)] = 7;
    settingsNamesIndexes[QString::fromStdWString(READ_USER_DATA_ACCESS_SETTINGS)] = 8;
    settingsNamesIndexes[QString::fromStdWString(LOCATION_SETTINGS)] = 9;
    settingsNamesIndexes[QString::fromStdWString(LANDMARK_SETTINGS)] = 10;
    settingsNamesIndexes[QString::fromStdWString(AUTHENTICATION_SETTINGS)] = 11;
    settingsNamesIndexes[QString::fromStdWString(SMART_CARD_COMMUNICATION_SETTINGS)] = 12;
    settingsNamesIndexes[QString::fromStdWString(APPLICATION_AUTO_INVOCATION_SETTINGS)] = 13;
    settingsNamesIndexes[QString::fromStdWString(BROADCAST_SETTINGS)] = 14;
    settingsNamesIndexes[QString::fromStdWString(NFC_WRITE_ACCESS_SETTINGS)] = 15;
    settingsNamesIndexes[QString::fromStdWString(URL_START_SETTINGS)] = 16;
    int last_index = 16;
    
    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, iSuiteUid);
    query.insert(attr);
    attr.setEntry(aColumnName, L"");
    query.insert(attr);

    try
    {
        iStorage->search(aPrimaryTableName, query, queryResult);
        
        JavaStorageApplicationList_t::const_iterator iterator;
        JavaStorageApplicationList_t secondaryQueryResult;
        for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
        {
            std::wstring name = L"";
            JavaStorageApplicationEntry_t entry = (*iterator);
            JavaStorageEntry findPattern;
            findPattern.setEntry(aColumnName, L"");
            JavaStorageApplicationEntry_t::const_iterator findIterator =
                    entry.find(findPattern);
            if (findIterator != entry.end())
            {
                name = findIterator->entryValue();
            }            
            if (name.size() > 0)
            {
                entry.clear();
                query.clear();
                attr.setEntry(ID, iSuiteUid);
                query.insert(attr);
                attr.setEntry(aColumnName, name);
                query.insert(attr);
                secondaryQueryResult.clear();
                iStorage->search(aSecondaryTableName, query, secondaryQueryResult);
                if (secondaryQueryResult.size() > 0)
                {
                    IndexedSettingsName value;
                    value.name = name;
                    value.index = last_index + 1;
                    if (settingsNamesIndexes.contains(QString::fromStdWString(name)))
                    {
                        value.index = settingsNamesIndexes.value(QString::fromStdWString(name));
                    }
                    values.push_back(value);
                }
            }
        }
    }
    catch (JavaStorageException& aJse)
    {
        // Don't leave. Set defaults.
    }

    return values;
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
