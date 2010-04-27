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
#include <memory>
#include "javastorage.h"

class HbComboBox;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class JavaApplicationSettingsView;
class JavaApplicationSettings;

class JavaApplicationSettingsViewPrivate
{

public:
    JavaApplicationSettingsViewPrivate(const QString& aApplication);
    virtual ~JavaApplicationSettingsViewPrivate();
    void init(JavaApplicationSettingsView*);

    // slots
    void _q_settingsChanged(const QString &newValue);
    void _q_dataItemDisplayed(const QModelIndex);

private:
    void readAllSettings();
    void initSettings(QVector<JavaApplicationSettings>& settings, HbDataFormModelItem * parent);
    void writeSettings(JavaApplicationSettings& settings, const QString &newValue);
    void securityWarningsChanged(const QString &);
    bool blanketAllowed(const JavaApplicationSettings &settings);
    HbComboBox * itemToComboBox(const HbDataFormModelItem *);
    JavaApplicationSettings* findSettings(HbComboBox* id);
    JavaApplicationSettings* findSettings(HbComboBox* id, QVector<JavaApplicationSettings>& settings);
    int readFromStorage(JavaApplicationSettings& settings);
    void writeToStorage(JavaApplicationSettings& settings);
    bool findFromStorage(const std::wstring&, const std::wstring&, const std::wstring&, const std::string&);
    std::wstring readFromStorage(const std::wstring&, const std::wstring&, const std::wstring&, const std::string&);
    void findEntry(const java::storage::JavaStorageApplicationList_t&, const std::wstring&, std::wstring& eValue);
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
    std::wstring iJavaAppUid;
    QString BLANKET,SESSION,ONESHOT,DENIED,SECURITY_LEVEL,USER_DEFINED,SENSITIVE_SETTINGS,SENSITIVE_SETTINGS_NET_USAGE,MUTUALLY_EXCLUSIVE_SETTINGS,OK,CANCEL,SECURITY_WARNING_TITLE,NET_ACCESS,LOW_LEVEL_NET_ACCESS;
};


#endif // JAVAAPPLICATIONSETTINGSVIEW_P_H
