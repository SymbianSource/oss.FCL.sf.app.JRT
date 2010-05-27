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
#ifndef JAVAAPPLICATIONSETTINGSVIEW_P_H
#define JAVAAPPLICATIONSETTINGSVIEW_P_H

#include <QVector>
#include <QModelIndex>
#include <memory>
#include <cmapplsettingsui.h>
#include "javaapplicationsettings.h"
#include "javastorage.h"

class HbWidget;
class HbPushButton;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class JavaApplicationSettingsView;

class JavaApplicationSettingsViewPrivate: public QObject
{
    Q_OBJECT
    
public:
    JavaApplicationSettingsViewPrivate(const QString& aApplication);
    virtual ~JavaApplicationSettingsViewPrivate();
    void init(JavaApplicationSettingsView*);

    // slots
    void _q_settingsChanged(const QString &newValue);
    void _q_settingsChanged(bool);
    void _q_dataItemDisplayed(const QModelIndex);

private slots:
    void netConnSelected(uint netConnSelectionStatus);

private:
    void readAllSettings();
    void initSettings(QVector<JavaApplicationSettings>& settings, HbDataFormModelItem * parent);
    void writeSettings(JavaApplicationSettings& settings, const QString &newValue);
    void securityWarningsChanged(const QString &);
    bool blanketAllowed(const JavaApplicationSettings &settings);
    HbWidget * itemToWidget(const HbDataFormModelItem *);
    JavaApplicationSettings* findSettings(HbWidget* id);
    JavaApplicationSettings* findSettings(HbWidget* id, QVector<JavaApplicationSettings>& settings);
    int readFromStorage(JavaApplicationSettings& settings);
    void writeToStorage(JavaApplicationSettings& settings);
    bool findFromStorage(const std::wstring&, const std::wstring&, const std::wstring&, const std::string&);
    std::wstring readFromStorage(const std::wstring&, const std::wstring&, const std::wstring&, const std::string&);
    void filterSecuritySettings(JavaApplicationSettings& settings);
    void findEntry(const java::storage::JavaStorageApplicationList_t&, const std::wstring&, std::wstring& eValue);
    void readSuiteUid(const QString& aAppUid);
    bool securityWarningAccepted(const QString& text, const QString& acceptActionLabel, const QString& rejectActionLabel, const QString& headingText);
    void handleNetworkSettings();
    void readNetworkConnectionName();
private:
    HbDataForm * mainForm;
    HbDataFormModel *model;
    HbDataFormModelItem * generalSettingsGroup;
    HbDataFormModelItem * securitySettingsGroup;
    QVector<JavaApplicationSettings> generalSettings;
    QVector<JavaApplicationSettings> securitySettings;
    QVector<JavaApplicationSettings> extraSettings;
    JavaApplicationSettingsView* iPublicView;
    std::auto_ptr<java::storage::JavaStorage> iStorage;
    std::wstring iSuiteUid;
    QString BLANKET,SESSION,ONESHOT,DENIED,SECURITY_LEVEL,USER_DEFINED,SENSITIVE_SETTINGS,SENSITIVE_SETTINGS_NET_USAGE,MUTUALLY_EXCLUSIVE_SETTINGS,OK,CANCEL,SECURITY_WARNING_TITLE,NET_ACCESS,LOW_LEVEL_NET_ACCESS,NETWORK_CONNECTION,NETWORK_CONNECTION_CHANGE;
    CmApplSettingsUi* netConnSettingsUi;
    CmApplSettingsUi::SettingSelection netConnSelection;
    JavaApplicationSettings::NetworkConnection netConn;
    int netSettIndex;
};


#endif // JAVAAPPLICATIONSETTINGSVIEW_P_H
