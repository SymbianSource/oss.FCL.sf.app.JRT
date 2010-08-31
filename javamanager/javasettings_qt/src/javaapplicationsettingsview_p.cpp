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
#include <QLibraryInfo>
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

#include "logger.h"

QTM_USE_NAMESPACE

using namespace java::settings;
using namespace java::storage;
using namespace java::util;
using namespace std;

const wchar_t ON_SCREEN_KEYPAD_VALUE_NO[] = L"0";
const wchar_t ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS[] = L"1";
const wchar_t ON_SCREEN_KEYPAD_VALUE_NAVIGATION[] = L"2";

JavaApplicationSettingsViewPrivate::JavaApplicationSettingsViewPrivate(const QString& aJavaAppUid):
        mMainForm(0), mModel(0), mGeneralSettingsGroup(0), mSecuritySettingsGroup(0), mNetConnSettingsUi(0), mAsyncToSyncCallEventLoop(0), mSecWarningAccepted(false), mDefaultConnId(0)
{
    // init common values
    init();

    // init the suite UID from the application UID
    readSuiteUid(aJavaAppUid);
    if (mSuiteUid.size() <= 0)
    {
        ELOG1(EJavaSettings, "No suite found with uid %S", aJavaAppUid.toStdWString().c_str());
        return;
    }
    ELOG1(EJavaSettings, "Initializing settings for suite %S", mSuiteUid.c_str());
    
    // init access point settings ui
    mNetConnSettingsUi = new CmApplSettingsUi(this);
    mAsyncToSyncCallEventLoop = new QEventLoop();

    // read all settings
    readAllSettings();

    // init form
    mMainForm = new HbDataForm();
    mMainForm->setHeading(QString::fromStdWString(readFromStorage(PACKAGE_NAME, L"", L"", APPLICATION_PACKAGE_TABLE)));
    mModel = new HbDataFormModel();

    // init settings
    mGeneralSettingsGroup = mModel->appendDataFormGroup(
        QString(hbTrId(ID_GENERAL_GROUP_TITLE)), mModel->invisibleRootItem());
    initSettings(mGeneralSettings, mGeneralSettingsGroup);
    // append the security settings only if there are any user settings
    if (mExtraSettings.size() > 0)
    {
        mSecuritySettingsGroup = mModel->appendDataFormGroup(
            QString(hbTrId(ID_SECURITY_GROUP_TITLE)), mModel->invisibleRootItem());
        initSettings(mSecuritySettings, mSecuritySettingsGroup);

        // if security warnings is user defined -> add the extra settings, expand
        if (mSecuritySettings[0].getCurrentValue() == 1)
        {
            securityWarningsChanged(TXT_USER_DEFINED);
        }
    }

    // link form and model
    mMainForm->setModel(mModel);
    mMainForm->setExpanded(mModel->indexFromItem(mGeneralSettingsGroup), true);
    if (mExtraSettings.size() > 0)
    {
        mMainForm->setExpanded(mModel->indexFromItem(mSecuritySettingsGroup), (mSecuritySettings[0].getCurrentValue() == 1));
    }
}

void JavaApplicationSettingsViewPrivate::init(JavaApplicationSettingsView* aPublicView)
{
    mPublicView = aPublicView;

    if (mMainForm)
    {
        // do the connect for the main form
        mPublicView->connect(mMainForm, SIGNAL(itemShown(const QModelIndex)),
                             mPublicView, SLOT(_q_dataItemDisplayed(const QModelIndex)));

        // set the form as view's widget
        mPublicView->setWidget(mMainForm);
    }
    else
    {
        // no settings available
        HbLabel* label = new HbLabel(TXT_SETTINGS_NOT_AVAILABLE, mPublicView);
        label->setAlignment(Qt::AlignVCenter);
    }

    // set title
    mPublicView->setTitle(TXT_SETTINGS_TITLE);
}

JavaApplicationSettingsViewPrivate::~JavaApplicationSettingsViewPrivate()
{
    delete mMainForm;
    delete mModel;
    delete mNetConnSettingsUi;
    delete mAsyncToSyncCallEventLoop;
    foreach (QTranslator *translator, mTranslators) {
        qApp->removeTranslator(translator);
    }
    qDeleteAll(mTranslators);
    mTranslators.clear();
}

void JavaApplicationSettingsViewPrivate::readAllSettings()
{
    // general settings
    initGeneralSettings();

    // security settings
    initSecuritySettings();
}

void JavaApplicationSettingsViewPrivate::init()
{
    // init storage
    try
    {
        mStorage.reset(JavaStorage::createInstance());
        mStorage->open();
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaSettings, "Error while opening the storage %S", aJse.toString().c_str());
    }

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
    TXT_BLANKET = QString(hbTrId(ID_BLANKET));
    TXT_SESSION = QString(hbTrId(ID_SESSION));
    TXT_ONESHOT = QString(hbTrId(ID_ONESHOT));
    TXT_DENIED = QString(hbTrId(ID_DENIED));
    TXT_SECURITY_LEVEL = QString(hbTrId(ID_SECURITY_LEVEL));
    TXT_USER_DEFINED = QString(hbTrId(ID_USER_DEFINED));
    TXT_SENSITIVE_SETTINGS = QString(hbTrId(ID_SENSITIVE_SETTINGS));
    TXT_SENSITIVE_SETTINGS_NET_USAGE = QString(hbTrId(ID_SENSITIVE_SETTINGS_NET_USAGE));
    TXT_MUTUALLY_EXCLUSIVE_SETTINGS = QString(hbTrId(ID_MUTUALLY_EXCLUSIVE_SETTINGS));
    TXT_OK = QString(hbTrId(ID_OK));
    TXT_CANCEL = QString(hbTrId(ID_CANCEL));
    TXT_SECURITY_WARNING_TITLE = QString(hbTrId(ID_SECURITY_WARNING_TITLE));
    TXT_NET_ACCESS = QString(hbTrId(ID_NET_ACCESS));
    TXT_LOW_LEVEL_NET_ACCESS = QString(hbTrId(ID_LOW_LEVEL_NET_ACCESS));
    TXT_NETWORK_CONNECTION = QString(hbTrId(ID_NETWORK_CONNECTION));
    TXT_SETTINGS_TITLE = QString(hbTrId(ID_SETTINGS_TITLE));
    TXT_SETTINGS_NOT_AVAILABLE = QString(hbTrId(ID_SETTINGS_NOT_AVAILABLE));

    // init the known localized settings names
    mLocalizedSettingsNames[QString::fromStdWString(NET_ACCESS_SETTINGS)] = TXT_NET_ACCESS;
    mLocalizedSettingsNames[QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS)] = TXT_LOW_LEVEL_NET_ACCESS;
    mLocalizedSettingsNames[QString::fromStdWString(MESSAGING_SETTINGS)] = QString(hbTrId(ID_MESSAGING));
    mLocalizedSettingsNames[QString::fromStdWString(RESTRICTED_MESSAGING_SETTINGS)] = QString(hbTrId(ID_RESTRICTED_MESSAGING));
    mLocalizedSettingsNames[QString::fromStdWString(CALL_CONTROL_SETTINGS)] = QString(hbTrId(ID_CALL_CONTROL));
    mLocalizedSettingsNames[QString::fromStdWString(LOCAL_CONNECTIVITY_SETTINGS)] = QString(hbTrId(ID_LOCAL_CONNECTIVITY));
    mLocalizedSettingsNames[QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS)] = QString(hbTrId(ID_MULTIMEDIA_RECORDING));
    mLocalizedSettingsNames[QString::fromStdWString(WRITE_USER_DATA_ACCESS_SETTINGS)] = QString(hbTrId(ID_WRITE_USER_DATA_ACCESS));
    mLocalizedSettingsNames[QString::fromStdWString(READ_USER_DATA_ACCESS_SETTINGS)] = QString(hbTrId(ID_READ_USER_DATA_ACCESS));
    mLocalizedSettingsNames[QString::fromStdWString(LOCATION_SETTINGS)] = QString(hbTrId(ID_LOCATION));
    mLocalizedSettingsNames[QString::fromStdWString(LANDMARK_SETTINGS)] = QString(hbTrId(ID_LANDMARK));
    mLocalizedSettingsNames[QString::fromStdWString(AUTHENTICATION_SETTINGS)] = QString(hbTrId(ID_AUTHENTICATION));
    mLocalizedSettingsNames[QString::fromStdWString(SMART_CARD_COMMUNICATION_SETTINGS)] = QString(hbTrId(ID_SMART_CARD_COMMUNICATION));
    mLocalizedSettingsNames[QString::fromStdWString(APPLICATION_AUTO_INVOCATION_SETTINGS)] = QString(hbTrId(ID_APPLICATION_AUTO_INVOCATION));
    mLocalizedSettingsNames[QString::fromStdWString(BROADCAST_SETTINGS)] = QString(hbTrId(ID_BROADCAST));
    mLocalizedSettingsNames[QString::fromStdWString(NFC_WRITE_ACCESS_SETTINGS)] = QString(hbTrId(ID_NFC_WRITE_ACCESS));
    mLocalizedSettingsNames[QString::fromStdWString(URL_START_SETTINGS)] = QString(hbTrId(ID_URL_START));
    // init the display indexes associated with each settings
    mSettingsNamesIndexes[QString::fromStdWString(NET_ACCESS_SETTINGS)] = NET_ACCESS_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS)] = LOW_LEVEL_NET_ACCESS_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(MESSAGING_SETTINGS)] = MESSAGING_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(RESTRICTED_MESSAGING_SETTINGS)] = RESTRICTED_MESSAGING_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(CALL_CONTROL_SETTINGS)] = CALL_CONTROL_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(LOCAL_CONNECTIVITY_SETTINGS)] = LOCAL_CONNECTIVITY_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS)] = MULTIMEDIA_RECORDING_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(WRITE_USER_DATA_ACCESS_SETTINGS)] = WRITE_USER_DATA_ACCESS_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(READ_USER_DATA_ACCESS_SETTINGS)] = READ_USER_DATA_ACCESS_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(LOCATION_SETTINGS)] = LOCATION_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(LANDMARK_SETTINGS)] = LANDMARK_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(AUTHENTICATION_SETTINGS)] = AUTHENTICATION_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(SMART_CARD_COMMUNICATION_SETTINGS)] = SMART_CARD_COMMUNICATION_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(APPLICATION_AUTO_INVOCATION_SETTINGS)] = APPLICATION_AUTO_INVOCATION_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(BROADCAST_SETTINGS)] = BROADCAST_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(NFC_WRITE_ACCESS_SETTINGS)] = NFC_WRITE_ACCESS_SETTINGS_INDEX;
    mSettingsNamesIndexes[QString::fromStdWString(URL_START_SETTINGS)] = URL_START_SETTINGS_INDEX;
}


void JavaApplicationSettingsViewPrivate::initGeneralSettings()
{
    QStringList settingsValues;
    vector<wstring> storageValues;
    if (!isInStorage(VALUE, NAME, L"Nokia-MIDlet-On-Screen-Keypad", APPLICATION_PACKAGE_ATTRIBUTES_TABLE))
    {
        // if the on screen keypad is predefined, it should not be changable by user
        // -> right now it is not added to settings list. It should be changed so that
        // the setting should not be editable by user
        settingsValues<<QString(hbTrId(ID_OSK_NO))<<QString(hbTrId(ID_OSK_GAME))<<QString(hbTrId(ID_OSK_NAVIGATION));
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_NO);
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_GAMEACTIONS);
        storageValues.push_back(ON_SCREEN_KEYPAD_VALUE_NAVIGATION);
        mGeneralSettings.append(JavaApplicationSettings(QString(hbTrId(ID_OSK)), settingsValues, ON_SCREEN_KEYPAD, MIDP_PACKAGE_TABLE, storageValues));
        readFromStorage(mGeneralSettings[0]);
    }
    settingsValues = QStringList();
    initNetworkConnection();
    settingsValues<<mNetConn.name;
    mGeneralSettings.append(JavaApplicationSettings(TXT_NETWORK_CONNECTION, settingsValues));
    mNetSettIndex = mGeneralSettings.size() - 1;
}

void JavaApplicationSettingsViewPrivate::initSecuritySettings()
{
    QStringList settingsValues;
    vector<wstring> storageValues;
    settingsValues = QStringList();
    storageValues.clear();
    settingsValues<<QString(hbTrId(ID_SECURITY_LEVEL_DEFAULT))<<TXT_USER_DEFINED;
    storageValues.push_back(SECURITY_WARNINGS_DEFAULT_MODE);
    storageValues.push_back(SECURITY_WARNINGS_USER_DEFINED_MODE);
    mSecuritySettings.append(JavaApplicationSettings(TXT_SECURITY_LEVEL, settingsValues, SECURITY_WARNINGS, MIDP_PACKAGE_TABLE, storageValues));
    readFromStorage(mSecuritySettings[0]);

    // extra settings
    settingsValues = QStringList();
    storageValues.clear();
    settingsValues<<TXT_ONESHOT<<TXT_SESSION<<TXT_BLANKET<<TXT_DENIED;
    storageValues.push_back(ONESHOT_INTERACTION_MODE);
    storageValues.push_back(SESSION_INTERACTION_MODE);
    storageValues.push_back(BLANKET_INTERACTION_MODE);
    storageValues.push_back(DENIED_INTERACTION_MODE);
    vector<IndexedSettingsName> allSecuritySettings = readFromStorage(FUNCTION_GROUP, MIDP_FUNC_GRP_SETTINGS_TABLE, MIDP_PERMISSIONS_TABLE);
    // sort the security settings according to how they should be displayed
    std::sort(allSecuritySettings.begin(), allSecuritySettings.end(), AscendingSort());
    for(int i=0; i<allSecuritySettings.size(); i++)
    {
        QString rawSettingsName = QString::fromStdWString(allSecuritySettings[i].name);
        QString localizedSettingsName = rawSettingsName;
        if (mLocalizedSettingsNames.contains(rawSettingsName))
            localizedSettingsName = mLocalizedSettingsNames.value(rawSettingsName);
        mExtraSettings.append(JavaApplicationSettings(
            localizedSettingsName, settingsValues, CURRENT_SETTING,
            MIDP_FUNC_GRP_SETTINGS_TABLE, storageValues, FUNCTION_GROUP,
            allSecuritySettings[i].name, ALLOWED_SETTINGS));
        readFromStorage(mExtraSettings[i]);
        mSettingsIndexes[rawSettingsName] = i;
    }

    // configure the high risk lists
    configureLists();
}

void JavaApplicationSettingsViewPrivate::configureLists()
{
    QList<JavaApplicationSettings*> highRiskList;
    QStringList highRiskSettingsNames;
    highRiskSettingsNames
        <<QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS)
        <<QString::fromStdWString(READ_USER_DATA_ACCESS_SETTINGS);
    configureList(highRiskList, highRiskSettingsNames, mSettingsIndexes);
    // net access with multimedia and read user data
    attachList(QString::fromStdWString(NET_ACCESS_SETTINGS),highRiskList, mSettingsIndexes);
    // low level net access with multimedia and read user data
    attachList(QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS),highRiskList, mSettingsIndexes);
    // messaging with multimedia and read user data
    attachList(QString::fromStdWString(MESSAGING_SETTINGS),highRiskList, mSettingsIndexes);
    // restricted messaging with multimedia and read user data
    attachList(QString::fromStdWString(RESTRICTED_MESSAGING_SETTINGS),highRiskList, mSettingsIndexes);
    // call control with multimedia and read user data
    attachList(QString::fromStdWString(CALL_CONTROL_SETTINGS),highRiskList, mSettingsIndexes);
    // local connectivity with multimedia and read user data
    attachList(QString::fromStdWString(LOCAL_CONNECTIVITY_SETTINGS),highRiskList, mSettingsIndexes);
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
    configureList(highRiskList, highRiskSettingsNames, mSettingsIndexes);
    attachList(QString::fromStdWString(MULTIMEDIA_RECORDING_SETTINGS),highRiskList, mSettingsIndexes);
    // read user data with net access, low level net access, messaging,
    // restricted messaging, call control and local connectivity
    attachList(QString::fromStdWString(READ_USER_DATA_ACCESS_SETTINGS),highRiskList, mSettingsIndexes);

    // configure the mutually exclusive list
    QList<JavaApplicationSettings*> mutuallyExclusiveList;
    QStringList mutuallyExclusiveSettingsNames;
    mutuallyExclusiveSettingsNames
        <<QString::fromStdWString(APPLICATION_AUTO_INVOCATION_SETTINGS);
    configureList(mutuallyExclusiveList, mutuallyExclusiveSettingsNames, mSettingsIndexes);
    //net access with application auto invocation
    attachList(QString::fromStdWString(NET_ACCESS_SETTINGS),mutuallyExclusiveList, mSettingsIndexes, false /* is high risk list */);
    //low level net access with application auto invocation
    attachList(QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS),mutuallyExclusiveList, mSettingsIndexes, false /* is high risk list */);
    //application auto invocation with net access and low level net access
    mutuallyExclusiveList.clear();
    mutuallyExclusiveSettingsNames.clear();
    mutuallyExclusiveSettingsNames
        <<QString::fromStdWString(NET_ACCESS_SETTINGS)
        <<QString::fromStdWString(LOW_LEVEL_NET_ACCESS_SETTINGS);
    configureList(mutuallyExclusiveList, mutuallyExclusiveSettingsNames, mSettingsIndexes);
    attachList(QString::fromStdWString(APPLICATION_AUTO_INVOCATION_SETTINGS),mutuallyExclusiveList, mSettingsIndexes, false /* is high risk list */);
}

void JavaApplicationSettingsViewPrivate::initSettings(QVector<JavaApplicationSettings>& aSettings, HbDataFormModelItem * aParent)
{
    for (int i=0; i<aSettings.size(); i++)
    {
        HbDataFormModelItem * appSettings = NULL;
        int currentValue = 0;
        switch(aSettings[i].getValues().size())
        {
            case 1:
                appSettings  = mModel->appendDataFormItem(
                        HbDataFormModelItem::ToggleValueItem,
                        aSettings[i].getName(), aParent);
                // make it a toogle button (-> get rid of the "pressed" ui effect)
                if (appSettings)
                {
                    appSettings->setContentWidgetData(QString("text"), aSettings[i].getValues()[0]);
                    appSettings->setContentWidgetData(QString("additionalText"), aSettings[i].getValues()[0]);
                }
                break;
            case 2:
                appSettings  = mModel->appendDataFormItem(
                        HbDataFormModelItem::ToggleValueItem,
                        aSettings[i].getName(), aParent);
                if (appSettings)
                {
                    currentValue = aSettings[i].getCurrentValue();
                    if (currentValue < 0)
                    {
                        currentValue = 0;
                    }
                    appSettings->setContentWidgetData(QString("text"), aSettings[i].getValues()[currentValue]);
                    appSettings->setContentWidgetData(QString("additionalText"),aSettings[i].getValues()[1 - currentValue]);
                }
                break;
            default:
                appSettings  = mModel->appendDataFormItem(
                        HbDataFormModelItem::ComboBoxItem,
                        aSettings[i].getName(), aParent);
                if (appSettings)
                {
                    appSettings->setContentWidgetData(QString("items"), aSettings[i].getValues());
                    appSettings->setContentWidgetData(QString("currentIndex"),aSettings[i].getCurrentValue());
                }
        }
        aSettings[i].setId(appSettings);
    }
}

void JavaApplicationSettingsViewPrivate::writeSettings(JavaApplicationSettings& aSettings, const QString &aNewValue)
{
    QStringList values = aSettings.getValues();
    int currentValue = aSettings.getCurrentValue();
    int newValue = aSettings.getValueIndex(aNewValue);
    if (newValue != -1 && currentValue != newValue)
    {
        // handle blanket settings
        if (aNewValue == TXT_BLANKET)
        {
            // check blanket settings
            if (!isBlanketAllowed(aSettings))
            {
                // change back to the old value
                HbComboBox * settingsCombo = static_cast<HbComboBox*>(itemToWidget(aSettings.getId()));
                if (settingsCombo)
                {
                    settingsCombo->setCurrentIndex(currentValue);
                }
                return;
            }
        }
        aSettings.setCurrentValue(newValue);
        writeToStorage(aSettings);
    }
}

bool JavaApplicationSettingsViewPrivate::isBlanketAllowed(const JavaApplicationSettings& aSettings)
{
    // handle the high risk items
    QList<JavaApplicationSettings*> highRiskList = aSettings.getHighRiskList();
    // go through the list and see if any of the setings are in blanket
    for (int i=0; i<highRiskList.size(); i++)
    {
        if (highRiskList[i] &&
                highRiskList[i]->getValue(highRiskList[i]->getCurrentValue())
                == TXT_BLANKET)
        {
            QString secWarning = TXT_SENSITIVE_SETTINGS;
            QString LOCAL_CONNECTIVITY = QString(hbTrId(ID_LOCAL_CONNECTIVITY));
            if (aSettings.getName() != LOCAL_CONNECTIVITY
                && highRiskList[i]->getName() != LOCAL_CONNECTIVITY)
            {
                secWarning = TXT_SENSITIVE_SETTINGS_NET_USAGE;
            }
            if (!isSecurityWarningAccepted(
                secWarning,TXT_OK,TXT_CANCEL ,TXT_SECURITY_WARNING_TITLE))
            {
                LOG3(EJavaSettings, EInfo,"User did not accept the high risk due to having both \"%S\" and \"%S\" set to \"%S\"", aSettings.getName().toStdWString().c_str(), highRiskList[i]->getName().toStdWString().c_str(), TXT_BLANKET.toStdWString().c_str());
                return false;
            }
            LOG3(EJavaSettings, EInfo,"User accepted the high risk due to having both \"%S\" and \"%S\" set to \"%S\"", aSettings.getName().toStdWString().c_str(), highRiskList[i]->getName().toStdWString().c_str(), TXT_BLANKET.toStdWString().c_str());
            break;
        }
    }
    // handle the mutually exclusive rules
    QList<JavaApplicationSettings*> mutuallyExclusiveList = aSettings
            .getMutuallyExclusiveList();
    // go through the list and see if any of the setings are in blanket
    for (int i=0; i<mutuallyExclusiveList.size(); i++)
    {
        if (mutuallyExclusiveList[i] && mutuallyExclusiveList[i]->getValue(
                    mutuallyExclusiveList[i]->getCurrentValue()) == TXT_BLANKET)
        {
            bool isBlanketAllowed = isSecurityWarningAccepted(
                TXT_MUTUALLY_EXCLUSIVE_SETTINGS, TXT_OK, TXT_CANCEL, TXT_SECURITY_WARNING_TITLE);
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
                            if (allowedValues[j] == TXT_SESSION)
                            {
                                selectedValue = j;
                                break;
                            }
                            if (allowedValues[j] == TXT_ONESHOT)
                            {
                                selectedValue = j;
                            }
                            else if (allowedValues[j] == TXT_DENIED
                                     && allowedValues[selectedValue] == TXT_BLANKET)
                            {
                                selectedValue = j;
                            }
                        }
                        // set the maximum allowed value
                        mutuallyExclusiveList[i]->setCurrentValue(selectedValue);
                        HbComboBox * settingsCombo = static_cast<HbComboBox*>(itemToWidget(
                                                         mutuallyExclusiveList[i]->getId()));
                        if (settingsCombo)
                        {
                            settingsCombo->setCurrentIndex(selectedValue);
                        }
                        writeToStorage(*mutuallyExclusiveList[i]);
                        LOG4(EJavaSettings, EInfo,"Changing \"%S\" to \"%S\" led to changing \"%S\" to \"%S\" due to mutually exclusive rules", aSettings.getName().toStdWString().c_str(), TXT_BLANKET.toStdWString().c_str(), mutuallyExclusiveList[i]->getName().toStdWString().c_str(), allowedValues[selectedValue].toStdWString().c_str());
                    }
                    i++;
                    // end of list?
                    if (i >= mutuallyExclusiveList.size())
                    {
                        break;
                    }
                    // move on into the list
                    isBlanket = (mutuallyExclusiveList[i]->getValue(
                                     mutuallyExclusiveList[i]->getCurrentValue()) == TXT_BLANKET);
                }
                while (true);
            }
            else
            {
                LOG1(EJavaSettings, EInfo,"User did not accept changes in \"%S\" due to mutually exclusive rules", mutuallyExclusiveList[i]->getName().toStdWString().c_str());
            }
            return isBlanketAllowed;
        }
    }
    return true;
}

void JavaApplicationSettingsViewPrivate::_q_settingsChanged(bool)
{
    HbPushButton * sender = static_cast<HbPushButton*>(mPublicView->sender());
    if (sender)
    {
        JavaApplicationSettings* settings = findSettings(sender);
        if (settings == NULL)
        {
            return;
        }
        if (settings->getName() == TXT_SECURITY_LEVEL)
        {
            QString newValue = settings->getValue(1 - settings->getCurrentValue());
            securityWarningsChanged(newValue);
            writeSettings(*settings, newValue);
        }
        else if (settings->getName() == TXT_NETWORK_CONNECTION)
        {
            handleNetworkSettings();
        }
    }
}
void JavaApplicationSettingsViewPrivate::_q_settingsChanged(const QString& aNewValue)
{
    HbComboBox * sender = static_cast<HbComboBox*>(mPublicView->sender());
    if (sender)
    {
        JavaApplicationSettings* settings = findSettings(sender);
        if (settings != NULL)
        {
            // any other settings are treated same
            writeSettings(*settings, aNewValue);
        }
    }
}

void JavaApplicationSettingsViewPrivate::securityWarningsChanged(const QString& aNewValue)
{
    if (aNewValue == TXT_USER_DEFINED)
    {
        // append the extra settings and values
        for (int i=0; i<mExtraSettings.size(); i++)
        {
            if (mExtraSettings[i].getCurrentValue() >= 0 && mExtraSettings[i].getId() == 0)
            {
                HbDataFormModelItem * appSettings = mModel->appendDataFormItem(
                                                        HbDataFormModelItem::ComboBoxItem,
                                                        mExtraSettings[i].getName(), mSecuritySettingsGroup);
                if (appSettings)
                {
                    mExtraSettings[i].setId(appSettings);
                    // when settings the items, the settingsChanged signal gets emitted with the first value
                    // as being the current value. This signal should be discarded (the first value is just
                    // an intermediate value towards the actual current value which gets set shortly).
                    // Therefore set the first value as the current value (this way the settingsChanged
                    // signal gets discarded) and change the current value back to the real current value
                    int currentValue = mExtraSettings[i].getCurrentValue();
                    mExtraSettings[i].setCurrentValue(0);
                    appSettings->setContentWidgetData(QString("items"), mExtraSettings[i].getValues());
                    mExtraSettings[i].setCurrentValue(currentValue);
                    appSettings->setContentWidgetData(QString("currentIndex"),mExtraSettings[i].getCurrentValue());
                }
            }
        }
    }
    else
    {
        // remove the extra settings
        for (int i=0; i<mExtraSettings.size(); i++)
        {
            if (mExtraSettings[i].getId())
            {
                HbComboBox * extraSettingsId = static_cast<HbComboBox*>(itemToWidget(mExtraSettings[i].getId()));
                if (extraSettingsId)
                {
                    mPublicView->disconnect(extraSettingsId, 0, 0, 0);
                    mModel->removeItem(mModel->indexFromItem(mExtraSettings[i].getId()));
                    mExtraSettings[i].setId(0);
                    mExtraSettings[i].disconnectFromUi();
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
    mNetConnSelection.result = mNetConn.type;
    mNetConnSelection.id = mNetConn.id;
    // init settings ui with destinations, filter and initial selection
    mNetConnSettingsUi->setOptions(listItems, filter);
    mNetConnSettingsUi->setSelection(mNetConnSelection);
    // Connect the setting ui's signal with own slot where the mNetConnSelection is stored
    connect(mNetConnSettingsUi, SIGNAL(finished(uint)), this, SLOT(netConnSelected(uint)));
    // launch the setting ui
    mNetConnSettingsUi->open();
}

void JavaApplicationSettingsViewPrivate::_q_dataItemDisplayed(const QModelIndex aDataItemIndex)
{
    HbDataFormModelItem *item = static_cast<HbDataFormModel*>(
                                    mMainForm->model())->itemFromIndex(aDataItemIndex);
    if (item == NULL)
    {
        return;
    }
    int itemType = item->data(HbDataFormModelItem::ItemTypeRole).toInt();
    HbComboBox * comboBox = NULL;
    HbPushButton * pushButton = NULL;
    HbWidget * widget = (qobject_cast<HbDataFormViewItem *>
        (mMainForm->itemByIndex(aDataItemIndex)))->dataItemContentWidget();
    JavaApplicationSettings* settings = findSettings(widget);
    if (settings == NULL)
    {
        // stop right here
        return;
    }
    switch(HbDataFormModelItem::DataItemType(itemType))
    {
        case HbDataFormModelItem::ComboBoxItem:
            comboBox = static_cast<HbComboBox*>(widget);
            if (settings->isConnectedToUi())
            {
                // need to do a reconnect: disconnect followed by a connect
                mPublicView->disconnect(comboBox,
                        SIGNAL(currentIndexChanged(const QString &)),
                        mPublicView,
                        SLOT(_q_settingsChanged(const QString &)));
            }
            mPublicView->connect(comboBox,
                                 SIGNAL(currentIndexChanged(const QString &)),
                                 mPublicView, SLOT(_q_settingsChanged(const QString &)),
                                 Qt::UniqueConnection);
            break;
        case HbDataFormModelItem::ToggleValueItem:
            pushButton = static_cast< HbPushButton*>(widget);
            if (settings->isConnectedToUi())
            {
                // need to do a reconnect: disconnect followed by a connect
                mPublicView->disconnect(pushButton,
                        SIGNAL(clicked(bool)),
                        mPublicView,
                        SLOT(_q_settingsChanged(bool)));
            }
            mPublicView->connect(pushButton,
                                 SIGNAL(clicked(bool)),
                                 mPublicView, SLOT(_q_settingsChanged(bool)),
                                 Qt::UniqueConnection);
            break;
    }
    settings->connectToUi();
}

void JavaApplicationSettingsViewPrivate::netConnSelected(uint aNetConnSelectionStatus)
{
    if (aNetConnSelectionStatus == CmApplSettingsUi::ApplSettingsErrorNone)
    {
        mNetConnSelection = mNetConnSettingsUi->selection();
        // store the selection
        if (mNetConnSelection.id != mNetConn.id
            && mNetConnSelection.id != mDefaultConnId)
        {
            TRAPD(err,
                switch(mNetConnSelection.result)
                {
                    case CmApplSettingsUi::SelectionTypeDestination:
                        ConnectionManager::setDestinationNetworkIdL(mSuiteUid, mNetConnSelection.id);
                        break;
                    case CmApplSettingsUi::SelectionTypeConnectionMethod:
                        ConnectionManager::setApnIdL(mSuiteUid, mNetConnSelection.id);
                        break;
                }
            );
            if (err == KErrNone)
            {
                mNetConn.id = mNetConnSelection.id;
                mNetConn.type = mNetConnSelection.result;
                readNetworkConnectionName();
                mGeneralSettings[mNetSettIndex].getId()->setContentWidgetData(QString("text"), mNetConn.name);
                mGeneralSettings[mNetSettIndex].getId()->setContentWidgetData(QString("additionalText"), mNetConn.name);
            }
        }
    }
}

void JavaApplicationSettingsViewPrivate::securityWarningDismissed(HbAction* aDismissAction)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    mSecWarningAccepted = (aDismissAction == dlg->actions().at(0));
    mAsyncToSyncCallEventLoop->exit();
}

void JavaApplicationSettingsViewPrivate::initNetworkConnection()
{
    TRAP_IGNORE(
        mNetConn.id = ConnectionManager::getDestinationNetworkIdL(mSuiteUid);
        if (mNetConn.id == (uint)KJavaNetworkAccessNotSpecified)
        {
            mNetConn.id = ConnectionManager::getApnIdL(mSuiteUid);
        }
    );
    QNetworkConfigurationManager manager;
    QNetworkConfiguration defaultCfg = manager.defaultConfiguration();
    mDefaultConnId = defaultCfg.identifier().toInt();
    QNetworkConfiguration cfg = manager.configurationFromIdentifier(
        QString::number(mNetConn.id));
    if (!cfg.isValid())
    {
        cfg = defaultCfg;
    }
    mNetConn.name = QString(hbTrId(cfg.name().toUtf8()));
}

void JavaApplicationSettingsViewPrivate::readNetworkConnectionName()
{
    QNetworkConfigurationManager manager;
    QNetworkConfiguration cfg = manager.configurationFromIdentifier(
        QString::number(mNetConn.id));
    if (!cfg.isValid())
    {
        cfg = manager.defaultConfiguration();
    }
    mNetConn.name = QString(hbTrId(cfg.name().toUtf8()));
}

JavaApplicationSettings* JavaApplicationSettingsViewPrivate::findSettings(HbWidget* aId)
{
    JavaApplicationSettings* settings = findSettings(aId, mGeneralSettings);
    if (settings == NULL)
    {
        settings = findSettings(aId, mSecuritySettings);
        if (settings == NULL)
        {
            settings = findSettings(aId, mExtraSettings);
        }
    }
    return settings;
}

JavaApplicationSettings* JavaApplicationSettingsViewPrivate::findSettings(HbWidget* aId, QVector<JavaApplicationSettings>& aAllSettings)
{
    for (int i=0; i<aAllSettings.size(); i++)
    {
        if (itemToWidget(aAllSettings[i].getId()) == aId)
        {
            return &aAllSettings[i];
        }
    }
    return NULL;
}

HbWidget * JavaApplicationSettingsViewPrivate::itemToWidget(const HbDataFormModelItem * aItem)
{
    HbDataFormViewItem * viewItem = qobject_cast<HbDataFormViewItem *> (mMainForm->itemByIndex(
                                        mModel->indexFromItem(aItem)));
    if (viewItem)
    {
        return static_cast<HbWidget*>(viewItem->dataItemContentWidget());
    }
    return NULL;
}

bool JavaApplicationSettingsViewPrivate::isSecurityWarningAccepted(const QString& aText, const QString& aAcceptActionLabel, const QString& aRejectActionLabel, const QString& aHeadingText)
{
    HbMessageBox securityWarning(HbMessageBox::MessageTypeQuestion);
    securityWarning.setDismissPolicy(HbDialog::NoDismiss);
    securityWarning.setTimeout(HbDialog::NoTimeout);
    securityWarning.setIconVisible(false);
    securityWarning.setText(aText);
    securityWarning.addAction(new HbAction(
            aAcceptActionLabel, &securityWarning));
    securityWarning.addAction(new HbAction(
            aRejectActionLabel, &securityWarning));
    securityWarning.setHeadingWidget(new HbLabel(aHeadingText));
    securityWarning.open(this, SLOT(securityWarningDismissed(HbAction*)));
    mAsyncToSyncCallEventLoop->exec();
    return mSecWarningAccepted;
}

void JavaApplicationSettingsViewPrivate::configureList(QList<JavaApplicationSettings*>& aList, const QStringList& aSettingsNames,  const QHash<QString, int>& aSettingsIndexes)
{
    for(int i=0; i<aSettingsNames.size(); i++)
    {
        if (aSettingsIndexes.contains(aSettingsNames[i]))
        {
            aList << &(mExtraSettings[aSettingsIndexes.value(aSettingsNames[i])]);
        }
    }
}

void JavaApplicationSettingsViewPrivate::attachList(const QString& aSettingsName, const QList<JavaApplicationSettings*>& aList, const QHash<QString, int>& aSettingsIndexes, bool aIsHighRiskList)
{
    if (aSettingsIndexes.contains(aSettingsName))
    {
        if (aIsHighRiskList)
        {
            mExtraSettings[aSettingsIndexes.value(aSettingsName)].setHighRiskList(aList);
        }
        else
        {
            mExtraSettings[aSettingsIndexes.value(aSettingsName)].setMutuallyExclusiveList(aList);
        }
    }
}

void JavaApplicationSettingsViewPrivate::installTranslator(const QString& aTranslationFileName)
{
    QTranslator* translator = new QTranslator;

    // load the correct translation of the localized strings for the java settings
    if (translator->load(
            aTranslationFileName + "_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        mTranslators.append(translator);
        qApp->installTranslator(translator);
    }
    else
    {
        delete translator;
        translator = NULL;
    }
}

void JavaApplicationSettingsViewPrivate::readFromStorage(JavaApplicationSettings& aSettings)
{
    if (aSettings.getColumnName().size() > 0
            && aSettings.getTableName().size() > 0)
    {
        wstring value = L"";

        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, mSuiteUid);
        query.insert(attr);
        if (aSettings.getFilterColumnName().size() > 0)
        {
            attr.setEntry(aSettings.getFilterColumnName(), aSettings.getFilterColumnValue());
            query.insert(attr);
        }
        attr.setEntry(aSettings.getColumnName(), L"");
        query.insert(attr);
        if (aSettings.getValuesColumnName().size() > 0)
        {
            attr.setEntry(aSettings.getValuesColumnName(), L"");
            query.insert(attr);
        }

        try
        {
            mStorage->search(aSettings.getTableName(), query, queryResult);
        }
        catch (JavaStorageException& aJse)
        {
            // Don't leave. Set defaults.
            ELOG1(EJavaSettings, "Error while reading from storage %S", aJse.toString().c_str());
        }

        if (aSettings.getValuesColumnName().size() > 0)
        {
            findEntry(queryResult, aSettings.getValuesColumnName(), value);
            if (value.size() > 0)
            {
                aSettings.setStorageValuesFilter(value);
                filterSecuritySettings(aSettings);
            }
        }

        value = L"";
        findEntry(queryResult, aSettings.getColumnName(), value);

        if (value.size() > 0)
        {
            vector<wstring> storageValues = aSettings.getStorageValues();
            for (int i=0; i<storageValues.size(); i++)
            {
                if (storageValues[i] == value)
                {
                    aSettings.setCurrentValue(i);
                    break;
                }
            }
            LOG2(EJavaSettings, EInfo,"Setting \"%S\" initialized to \"%S\"", aSettings.getName().toStdWString().c_str(), aSettings.getValue(aSettings.getCurrentValue()).toStdWString().c_str());
        }
    }
}

void JavaApplicationSettingsViewPrivate::filterSecuritySettings(JavaApplicationSettings& aSettings)
{
    std::wstring allowedModes = aSettings.getStorageValuesFilter();
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
        aSettings.removeValue(index);
        index--;
    }
    index++;
    tmp = allowedInteractionModes & INTERACTION_MODE_SESSION;
    if (tmp <= 0)
    {
        aSettings.removeValue(index);
        index--;
    }
    index++;
    tmp = allowedInteractionModes & INTERACTION_MODE_BLANKET;
    if (tmp <= 0)
    {
        aSettings.removeValue(index);
        index--;
    }
    index++;
    tmp = allowedInteractionModes & INTERACTION_MODE_DENIED;
    if (tmp <= 0)
    {
        aSettings.removeValue(index);
    }
}

void JavaApplicationSettingsViewPrivate::writeToStorage(JavaApplicationSettings& aSettings)
{
    if (aSettings.getTableName().size() <= 0
            || aSettings.getColumnName().size() <= 0)
    {
        // storage details are not provided -> no point in going further
        return;
    }
    JavaStorageApplicationEntry_t oldEntry;
    JavaStorageEntry attr;
    attr.setEntry(ID, mSuiteUid);
    oldEntry.insert(attr);
    JavaStorageApplicationEntry_t entry;
    if (aSettings.getFilterColumnName().size() > 0)
    {
        attr.setEntry(aSettings.getFilterColumnName(), aSettings.getFilterColumnValue());
        oldEntry.insert(attr);
    }
    attr.setEntry(aSettings.getColumnName(), aSettings.getStorageValues()[aSettings.getCurrentValue()]);
    entry.insert(attr);

    try
    {
        mStorage->update(aSettings.getTableName(), entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaSettings, "Error while writing into storage %S", aJse.toString().c_str());
    }
    LOG2(EJavaSettings, EInfo,"Setting \"%S\" changed to \"%S\"", aSettings.getName().toStdWString().c_str(), aSettings.getValue(aSettings.getCurrentValue()).toStdWString().c_str());

}

void JavaApplicationSettingsViewPrivate::findEntry(const JavaStorageApplicationList_t& aQueryResult,
        const std::wstring& aName,
        std::wstring& aValue)
{
    if (aQueryResult.size() > 0)
    {
        JavaStorageApplicationEntry_t entry = aQueryResult.front();
        JavaStorageEntry findPattern;
        findPattern.setEntry(aName, L"");
        JavaStorageApplicationEntry_t::const_iterator findIterator =
            entry.find(findPattern);
        if (findIterator != entry.end())
        {
            aValue = findIterator->entryValue();
        }
    }
}

bool JavaApplicationSettingsViewPrivate::isInStorage(const std::wstring& aColumnName, const std::wstring& aColumnFilterName, const std::wstring& aColumnFilterValue, const std::string& aTableName)
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
    attr.setEntry(ID, mSuiteUid);
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
        mStorage->search(aTableName, query, queryResult);
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

    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID, mSuiteUid);
    query.insert(attr);
    attr.setEntry(aColumnName, L"");
    query.insert(attr);

    try
    {
        mStorage->search(aPrimaryTableName, query, queryResult);

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
                attr.setEntry(ID, mSuiteUid);
                query.insert(attr);
                attr.setEntry(aColumnName, name);
                query.insert(attr);
                secondaryQueryResult.clear();
                mStorage->search(aSecondaryTableName, query, secondaryQueryResult);
                if (secondaryQueryResult.size() > 0)
                {
                    IndexedSettingsName value;
                    value.name = name;
                    value.index = LAST_SETTINGS_INDEX + 1;
                    if (mSettingsNamesIndexes.contains(QString::fromStdWString(name)))
                    {
                        value.index = mSettingsNamesIndexes.value(QString::fromStdWString(name));
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
    mSuiteUid = L"";
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
        mStorage->search(APPLICATION_TABLE, query, queryResult);
    }
    catch (JavaStorageException& aJse)
    {
        // Don't leave. Set defaults.
        ELOG1(EJavaSettings, "Error while reading from storage %S", aJse.toString().c_str());
    }
    findEntry(queryResult, PACKAGE_ID, mSuiteUid);
}

#include "moc_javaapplicationsettingsview.cpp"
