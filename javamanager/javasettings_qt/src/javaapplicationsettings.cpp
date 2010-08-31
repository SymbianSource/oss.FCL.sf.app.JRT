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
#include "javaapplicationsettings.h"

using namespace java::settings;

JavaApplicationSettings::JavaApplicationSettings():
    mName(),
    mValues(),
    mId(0),
    mCurrentValue(0),
    mColumnName(),
    mTableName(),
    mStorageValuesFilter(),
    mValuesColumnName(),
    mFilterColumnName(),
    mFilterColumnValue(),
    mConnectedToUi(false)
{
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues):
        mName(aName),
        mValues(aValues),
        mId(0),
        mCurrentValue(0),
        mColumnName(),
        mTableName(),
        mStorageValuesFilter(),
        mValuesColumnName(),
        mFilterColumnName(),
        mFilterColumnValue(),
        mConnectedToUi(false)
{
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues, const std::wstring& aColumnName, const std::string& aTableName, const std::vector<std::wstring>& aStorageValues):
        mName(aName),
        mValues(aValues),
        mId(0),
        mCurrentValue(0),
        mColumnName(aColumnName),
        mTableName(aTableName),
        mStorageValues(aStorageValues),
        mStorageValuesFilter(),
        mValuesColumnName(),
        mFilterColumnName(),
        mFilterColumnValue(),
        mConnectedToUi(false)
{
}

JavaApplicationSettings::JavaApplicationSettings(const QString& aName, const QStringList& aValues, const std::wstring& aColumnName, const std::string& aTableName, const std::vector<std::wstring>& aStorageValues, const std::wstring& aFilterColumnName, const std::wstring& aFilterColumnValue,  const std::wstring& aValuesColumnName):
        mName(aName),
        mValues(aValues),
        mId(0),
        mCurrentValue(0),
        mColumnName(aColumnName),
        mTableName(aTableName),
        mStorageValues(aStorageValues),
        mStorageValuesFilter(),
        mValuesColumnName(aValuesColumnName),
        mFilterColumnName(aFilterColumnName),
        mFilterColumnValue(aFilterColumnValue),
        mConnectedToUi(false)
{
}

void JavaApplicationSettings::setCurrentValue(int aCurrentValue)
{
    mCurrentValue = aCurrentValue;
}

void JavaApplicationSettings::connectToUi()
{
    mConnectedToUi = true;
}

void JavaApplicationSettings::disconnectFromUi()
{
    mConnectedToUi = false;
}

bool JavaApplicationSettings::isConnectedToUi()
{
    return mConnectedToUi;
}

int JavaApplicationSettings::getCurrentValue()
{
    return mCurrentValue;
}

const QString JavaApplicationSettings::getValue(int aValueIndex)
{
    if (aValueIndex >= 0 && aValueIndex < mValues.size())
        return mValues[aValueIndex];
    return QString::null;
}

void JavaApplicationSettings::removeValue(int aValueIndex)
{
    if (aValueIndex >= 0 && aValueIndex < mStorageValues.size())
        mStorageValues.erase(mStorageValues.begin() + aValueIndex);
    if (aValueIndex >= 0 && aValueIndex < mValues.size())
        mValues.erase(mValues.begin() + aValueIndex);
}

int JavaApplicationSettings::getValueIndex(const QString& aValue)
{
    int value = -1;
    for (int i = 0; i < mValues.size(); i++)
    {
        if (mValues[i] == aValue)
        {
            return i;
        }
    }
    return value;
}

void JavaApplicationSettings::setId(HbDataFormModelItem * aId)
{
    mId = aId;
}

HbDataFormModelItem * JavaApplicationSettings::getId()
{
    return mId;
}

const QString& JavaApplicationSettings::getName() const
{
    return mName;
}

const QStringList& JavaApplicationSettings::getValues() const
{
    return mValues;
}

const QList<JavaApplicationSettings*>& JavaApplicationSettings::getHighRiskList() const
{
    return mHighRiskList;
}

void JavaApplicationSettings::setHighRiskList(const QList<JavaApplicationSettings*>& aHighRiskList)
{
    mHighRiskList = aHighRiskList;
}

const QList<JavaApplicationSettings*>& JavaApplicationSettings::getMutuallyExclusiveList() const
{
    return mMutuallyExclusiveList;
}

void JavaApplicationSettings::setMutuallyExclusiveList(const QList<JavaApplicationSettings*>& aMutuallyExclusiveList)
{
    mMutuallyExclusiveList = aMutuallyExclusiveList;
}

const std::wstring& JavaApplicationSettings::getColumnName()
{
    return mColumnName;
}

const std::wstring& JavaApplicationSettings::getValuesColumnName()
{
    return mValuesColumnName;
}

const std::string& JavaApplicationSettings::getTableName()
{
    return mTableName;
}

const std::vector<std::wstring>& JavaApplicationSettings::getStorageValues()
{
    return mStorageValues;
}

void JavaApplicationSettings::setStorageValuesFilter(const std::wstring& aStorageValuesFilter)
{
    mStorageValuesFilter = aStorageValuesFilter;
}

const std::wstring& JavaApplicationSettings::getStorageValuesFilter()
{
    return mStorageValuesFilter;
}

const std::wstring& JavaApplicationSettings::getFilterColumnName()
{
    return mFilterColumnName;
}

const std::wstring& JavaApplicationSettings::getFilterColumnValue()
{
    return mFilterColumnValue;
}
