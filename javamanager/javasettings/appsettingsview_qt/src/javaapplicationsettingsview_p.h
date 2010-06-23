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
class JavaApplicationSettingsView;
class HbAction;

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
    void securityWarningDismissed(HbAction*);

private:
    void readAllSettings();
    void initSettings(QVector<JavaApplicationSettings>& settings, HbDataFormModelItem * parent);
    void writeSettings(JavaApplicationSettings& settings, const QString &newValue);
    void securityWarningsChanged(const QString &);
    bool blanketAllowed(const JavaApplicationSettings &settings);
    HbWidget * itemToWidget(const HbDataFormModelItem *);
    JavaApplicationSettings* findSettings(HbWidget* id);
    JavaApplicationSettings* findSettings(HbWidget* id, QVector<JavaApplicationSettings>& settings);
    void readFromStorage(JavaApplicationSettings& settings);
    void writeToStorage(JavaApplicationSettings& settings);
    bool findFromStorage(const std::wstring&, const std::wstring&, const std::wstring&, const std::string&);
    std::wstring readFromStorage(const std::wstring&, const std::wstring&, const std::wstring&, const std::string&);
    std::vector<IndexedSettingsName> readFromStorage(const std::wstring& aColumnName, const std::string& aPrimaryTableName,const std::string& aSecondaryTableName);
    void filterSecuritySettings(JavaApplicationSettings& settings);
    void findEntry(const java::storage::JavaStorageApplicationList_t&, const std::wstring&, std::wstring& eValue);
    void readSuiteUid(const QString& aAppUid);
    bool securityWarningAccepted(const QString& text, const QString& acceptActionLabel, const QString& rejectActionLabel, const QString& headingText);
    void handleNetworkSettings();
    void readNetworkConnectionName();
    void initNetworkConnection();
    void configureList(QList<JavaApplicationSettings*>&, const QStringList&, const QHash<QString, int>&);
    void attachList(const QString& settingsName, const QList<JavaApplicationSettings*>&, const QHash<QString, int>&, bool isHighRiskList = true);
    void installTranslator(const QString& translationFileName);
    
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
    QString BLANKET,SESSION,ONESHOT,DENIED,SECURITY_LEVEL,USER_DEFINED,SENSITIVE_SETTINGS,SENSITIVE_SETTINGS_NET_USAGE,MUTUALLY_EXCLUSIVE_SETTINGS,OK,CANCEL,SECURITY_WARNING_TITLE,NET_ACCESS,LOW_LEVEL_NET_ACCESS,NETWORK_CONNECTION,NETWORK_CONNECTION_CHANGE,SETTINGS_TITLE,SETTINGS_NOT_AVAILABLE;
    CmApplSettingsUi* netConnSettingsUi;
    CmApplSettingsUi::SettingSelection netConnSelection;
    JavaApplicationSettings::NetworkConnection netConn;
    int netSettIndex;
    QEventLoop* asyncToSyncCallEventLoop;
    bool secWarningAccepted;
    int defaultConnId;
    QList<QTranslator *> translators;
};


#endif // JAVAAPPLICATIONSETTINGSVIEW_P_H
