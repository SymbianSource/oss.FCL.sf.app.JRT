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

JavaApplicationSettings::JavaApplicationSettings():
    iName(QString("")), 
    iValues(QStringList()), 
    iId(0), iCurrentValue(0), 
    iColumnName(L""), 
    iTableName(""), 
    iStorageValuesFilter(L""), 
    iValuesColumnName(L""), 
    iFilterColumnName(L""), 
    iFilterColumnValue(L""), 
    connectedToUi(false)
{
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues):
        iName(aName), 
        iValues(aValues),
        iId(0), 
        iCurrentValue(0), 
        iColumnName(L""), 
        iTableName(""), 
        iStorageValuesFilter(L""), 
        iValuesColumnName(L""), 
        iFilterColumnName(L""), 
        iFilterColumnValue(L""), 
        connectedToUi(false)
{
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues, const std::wstring& aColumnName, const std::string& aTableName, const std::vector<std::wstring>& aStorageValues):
        iName(aName), 
        iValues(aValues), 
        iId(0), 
        iCurrentValue(0), 
        iColumnName(aColumnName), 
        iTableName(aTableName), 
        iStorageValues(aStorageValues), 
        iStorageValuesFilter(L""), 
        iValuesColumnName(L""), 
        iFilterColumnName(L""), 
        iFilterColumnValue(L""), 
        connectedToUi(false)
{
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues, const std::wstring& aColumnName, const std::string& aTableName, const std::vector<std::wstring>& aStorageValues, const std::wstring& aFilterColumnName, const std::wstring& aFilterColumnValue,  const std::wstring& aValuesColumnName):
        iName(aName), 
        iValues(aValues), 
        iId(0), 
        iCurrentValue(0), 
        iColumnName(aColumnName), 
        iTableName(aTableName), 
        iStorageValues(aStorageValues), 
        iStorageValuesFilter(L""), 
        iValuesColumnName(aValuesColumnName), 
        iFilterColumnName(aFilterColumnName), 
        iFilterColumnValue(aFilterColumnValue), 
        connectedToUi(false)
{
}

void JavaApplicationSettings::setCurrentValue(int aCurrentValue)
{
    iCurrentValue = aCurrentValue;
}

void JavaApplicationSettings::connectToUi()
{
    connectedToUi = true;
}

void JavaApplicationSettings::disconnectFromUi()
{
    connectedToUi = false;
}

bool JavaApplicationSettings::isConnectedToUi()
{
    return connectedToUi;
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

void JavaApplicationSettings::removeValue(int aValueIndex)
{
    if (aValueIndex >= 0 && aValueIndex < iStorageValues.size())
        iStorageValues.erase(iStorageValues.begin() + aValueIndex);
    if (aValueIndex >= 0 && aValueIndex < iValues.size())
        iValues.erase(iValues.begin() + aValueIndex);
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

HbDataFormModelItem * JavaApplicationSettings::getId()
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

const std::wstring& JavaApplicationSettings::getValuesColumnName()
{
    return iValuesColumnName;
}
    
const std::string& JavaApplicationSettings::getTableName()
{
    return iTableName;
}

const std::vector<std::wstring>& JavaApplicationSettings::getStorageValues()
{
    return iStorageValues;
}

void JavaApplicationSettings::setStorageValuesFilter(const std::wstring& aStorageValuesFilter)
{
    iStorageValuesFilter = aStorageValuesFilter;
}

const std::wstring& JavaApplicationSettings::getStorageValuesFilter()
{
    return iStorageValuesFilter;
}

const std::wstring& JavaApplicationSettings::getFilterColumnName()
{
    return iFilterColumnName;
}

const std::wstring& JavaApplicationSettings::getFilterColumnValue()
{
    return iFilterColumnValue;
}
