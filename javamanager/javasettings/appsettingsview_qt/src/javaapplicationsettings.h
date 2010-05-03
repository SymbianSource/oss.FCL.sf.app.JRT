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

class JavaApplicationSettings
{
public:
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
                            const std::wstring&);
    JavaApplicationSettings();

    const QString& getName() const;
    const QStringList& getValues() const;
    void setCurrentValue(int);
    int getCurrentValue();
    const QString& getValue(int);
    int getValue(const QString&);
    void setId(HbDataFormModelItem *);
    const HbDataFormModelItem * getId();
    const QList<JavaApplicationSettings*>& getHighRiskList() const;
    void setHighRiskList(const QList<JavaApplicationSettings*>&);
    const QList<JavaApplicationSettings*>& getMutuallyExclusiveList() const;
    void setMutuallyExclusiveList(const QList<JavaApplicationSettings*>&);
    const std::wstring& getColumnName();
    const std::string& getTableName();
    const std::wstring& getFilterColumnName();
    const std::wstring& getFilterColumnValue();
    const std::vector<std::wstring>& getStorageValues();
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
    std::wstring iFilterColumnName;
    std::wstring iFilterColumnValue;
};

#endif // JAVAAPPLICATIONSETTINGS_H
