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
#ifndef JAVAAPPLICATIONSETTINGS_H
#define JAVAAPPLICATIONSETTINGS_H

#include <string>
#include <vector>
#include <QString>
#include <QStringList>
#include <hbdataformmodelitem.h>
#include <cmapplsettingsui.h>

namespace java
{
namespace settings
{

/**
 * Encapsulation of java settings:
 *   - name (as localized string displayed to user)
 *   - values (as a list of localized strings displayed to user)
 *   - current value (one of the values)
 *   - storage details (name of table, name of column, storage values)
 * MIDP spec sets requirements for some combinations of settings:
 *   - some combinations of settings values are considered as sensitive,
 *     since they can cause privacy exposure or network costs. Whenever
 *     such a combination is about to be set, the user needs to be
 *     notified about possible consequences
 *   - such combinations of settings values are forbidden (mutually exclusive)
 * Each setting is initialized with a list of sensitive settings counterparts
 * and mutually exclusive counterparts respectively
 */
class JavaApplicationSettings
{
public:

    struct NetworkConnection
        {
        QString name;
        uint id;
        CmApplSettingsUi::SelectionType type;
        };

    JavaApplicationSettings(const QString& aName,
                            const QStringList& aValues);
    JavaApplicationSettings(const QString& aName,
                            const QStringList& aValues,
                            const std::wstring& aColumnName,
                            const std::string& aTableName,
                            const std::vector<std::wstring>& aStorageValues);
    JavaApplicationSettings(const QString& aName,
                            const QStringList& aValues,
                            const std::wstring& aColumnName,
                            const std::string& aTableName,
                            const std::vector<std::wstring>& aStorageValues,
                            const std::wstring& aFilterColumnName,
                            const std::wstring& aFilterColumnValue,
                            const std::wstring& aValuesColumnName);
    JavaApplicationSettings();

    const QString& getName() const;
    const QStringList& getValues() const;
    void setCurrentValue(int aCurrentValue);
    int getCurrentValue();
    const QString getValue(int aValueIndex);
    int getValueIndex(const QString& aValue);
    void removeValue(int aValueIndex);
    void setId(HbDataFormModelItem * aId);
    HbDataFormModelItem * getId();
    const QList<JavaApplicationSettings*>& getHighRiskList() const;
    void setHighRiskList(const QList<JavaApplicationSettings*>& aHighRiskList);
    const QList<JavaApplicationSettings*>& getMutuallyExclusiveList() const;
    void setMutuallyExclusiveList(const QList<JavaApplicationSettings*>& aMutuallyExclusiveList);
    const std::wstring& getColumnName();
    const std::string& getTableName();
    const std::wstring& getValuesColumnName();
    const std::wstring& getFilterColumnName();
    const std::wstring& getFilterColumnValue();
    const std::vector<std::wstring>& getStorageValues();
    void setStorageValuesFilter(const std::wstring& aStorageValuesFilter);
    const std::wstring& getStorageValuesFilter();
    void connectToUi();
    void disconnectFromUi();
    bool isConnectedToUi();

private:
    QString mName;
    QStringList mValues;
    HbDataFormModelItem * mId;
    QList<JavaApplicationSettings*> mHighRiskList;
    QList<JavaApplicationSettings*> mMutuallyExclusiveList;
    int mCurrentValue;
    std::wstring mColumnName;
    std::string mTableName;
    std::vector<std::wstring> mStorageValues;
    std::wstring mStorageValuesFilter;
    std::wstring mValuesColumnName;
    std::wstring mFilterColumnName;
    std::wstring mFilterColumnValue;
    bool mConnectedToUi;
};

} // namespace settings
} // namespace java

#endif // JAVAAPPLICATIONSETTINGS_H
