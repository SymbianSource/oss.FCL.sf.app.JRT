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
#include "javaapplicationsettings.h"

JavaApplicationSettings::JavaApplicationSettings()
{
    iId = 0;
    iName = "";
    iCurrentValue = 0;
    iColumnName = L"";
    iTableName = "";
    iFilterColumnName = L"";
    iFilterColumnValue = L"";
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues):
        iValues(aValues)
{
    iName = aName;
    iId = 0;
    iCurrentValue = 0;
    iColumnName = L"";
    iTableName = "";
    iFilterColumnName = L"";
    iFilterColumnValue = L"";
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues, const std::wstring& aColumnName, const std::string& aTableName, const std::vector<std::wstring>& aStorageValues):
        iValues(aValues)
{
    iName = aName;
    iId = 0;
    iCurrentValue = 0;
    iColumnName = aColumnName;
    iTableName = aTableName;
    iStorageValues = aStorageValues;
    iFilterColumnName = L"";
    iFilterColumnValue = L"";
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues, const std::wstring& aColumnName, const std::string& aTableName, const std::vector<std::wstring>& aStorageValues, const std::wstring& aFilterColumnName, const std::wstring& aFilterColumnValue):
        iValues(aValues)
{
    iName = aName;
    iId = 0;
    iCurrentValue = 0;
    iColumnName = aColumnName;
    iTableName = aTableName;
    iStorageValues = aStorageValues;
    iFilterColumnName = aFilterColumnName;
    iFilterColumnValue = aFilterColumnValue;
}

void JavaApplicationSettings::setCurrentValue(int aCurrentValue)
{
    iCurrentValue = aCurrentValue;
}

int JavaApplicationSettings::getCurrentValue()
{
    return iCurrentValue;
}

const QString& JavaApplicationSettings::getValue(int aValueIndex)
{
    static const QString null;
    if (aValueIndex >= 0 && aValueIndex < iValues.size())
        return iValues[aValueIndex];
    return null;
}

int JavaApplicationSettings::getValue(const QString& aValue)
{
    int value = -1;
    for (int i = 0; i < iValues.size(); i++)
    {
        if (iValues[i] == aValue)
        {
            return i;
        }
    }
    return value;
}

void JavaApplicationSettings::setId(HbDataFormModelItem * aId)
{
    iId = aId;
}

const HbDataFormModelItem * JavaApplicationSettings::getId()
{
    return iId;
}

const QString& JavaApplicationSettings::getName() const
{
    return iName;
}

const QStringList& JavaApplicationSettings::getValues() const
{
    return iValues;
}

const QList<JavaApplicationSettings*>& JavaApplicationSettings::getHighRiskList() const
{
    return iHighRiskList;
}

void JavaApplicationSettings::setHighRiskList(const QList<JavaApplicationSettings*>& aHighRiskList)
{
    iHighRiskList = aHighRiskList;
}

const QList<JavaApplicationSettings*>& JavaApplicationSettings::getMutuallyExclusiveList() const
{
    return iMutuallyExclusiveList;
}

void JavaApplicationSettings::setMutuallyExclusiveList(const QList<JavaApplicationSettings*>& aMutuallyExclusiveList)
{
    iMutuallyExclusiveList = aMutuallyExclusiveList;
}

const std::wstring& JavaApplicationSettings::getColumnName()
{
    return iColumnName;
}

const std::string& JavaApplicationSettings::getTableName()
{
    return iTableName;
}

const std::vector<std::wstring>& JavaApplicationSettings::getStorageValues()
{
    return iStorageValues;
}

const std::wstring& JavaApplicationSettings::getFilterColumnName()
{
    return iFilterColumnName;
}

const std::wstring& JavaApplicationSettings::getFilterColumnValue()
{
    return iFilterColumnValue;
}
