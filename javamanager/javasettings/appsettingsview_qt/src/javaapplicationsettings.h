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
#ifndef JAVAAPPLICATIONSETTINGS_H
#define JAVAAPPLICATIONSETTINGS_H

#include <string>
#include <vector>
#include <QString>
#include <QStringList>
#include <hbdataformmodelitem.h>
#include <cmapplsettingsui.h>

class JavaApplicationSettings
{
public:
    
    struct NetworkConnection
        {
        QString name;
        uint id;
        CmApplSettingsUi::SelectionType type;
        };
        
    JavaApplicationSettings(const QString&,
                            const QStringList&);
    JavaApplicationSettings(const QString&,
                            const QStringList&,
                            const std::wstring&,
                            const std::string&,
                            const std::vector<std::wstring>&);
    JavaApplicationSettings(const QString&,
                            const QStringList&,
                            const std::wstring&,
                            const std::string&,
                            const std::vector<std::wstring>&,
                            const std::wstring&,
                            const std::wstring&,
                            const std::wstring&);
    JavaApplicationSettings();

    const QString& getName() const;
    const QStringList& getValues() const;
    void setCurrentValue(int);
    int getCurrentValue();
    const QString& getValue(int);
    int getValue(const QString&);
    void removeValue(int);
    void setId(HbDataFormModelItem *);
    HbDataFormModelItem * getId();
    const QList<JavaApplicationSettings*>& getHighRiskList() const;
    void setHighRiskList(const QList<JavaApplicationSettings*>&);
    const QList<JavaApplicationSettings*>& getMutuallyExclusiveList() const;
    void setMutuallyExclusiveList(const QList<JavaApplicationSettings*>&);
    const std::wstring& getColumnName();
    const std::string& getTableName();
    const std::wstring& getValuesColumnName();
    const std::wstring& getFilterColumnName();
    const std::wstring& getFilterColumnValue();
    const std::vector<std::wstring>& getStorageValues();
    void setStorageValuesFilter(const std::wstring&);
    const std::wstring& getStorageValuesFilter();
    void connectToUi();
    void disconnectFromUi();
    bool isConnectedToUi();
        
private:
    QString iName;
    QStringList iValues;
    HbDataFormModelItem * iId;
    QList<JavaApplicationSettings*> iHighRiskList;
    QList<JavaApplicationSettings*> iMutuallyExclusiveList;
    int iCurrentValue;
    std::wstring iColumnName;
    std::string iTableName;
    std::vector<std::wstring> iStorageValues;
    std::wstring iStorageValuesFilter;
    std::wstring iValuesColumnName;
    std::wstring iFilterColumnName;
    std::wstring iFilterColumnValue;
    bool connectedToUi;
};

#endif // JAVAAPPLICATIONSETTINGS_H
