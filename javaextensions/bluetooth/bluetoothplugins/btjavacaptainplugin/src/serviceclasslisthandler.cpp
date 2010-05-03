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


#include "serviceclasslisthandler.h"
#include "logger.h"

using namespace std;

namespace java
{
namespace bluetooth
{

ServiceClassListHandler::ServiceClassListHandler()
{
    JELOG2(EJavaBluetooth);
}

ServiceClassListHandler::~ServiceClassListHandler()
{
    JELOG2(EJavaBluetooth);
}

void ServiceClassListHandler::add(wstring aKey, unsigned int aData)
{
    JELOG2(EJavaBluetooth);
    LOG2(EJavaBluetooth, EInfo,
         "+ ServiceClassListHandler::add Key:%S Data:0x%x",
         aKey.c_str(), aData);
    mServiceClassBitsMap[aKey] = aData;
}

void ServiceClassListHandler::remove(wstring aKey)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo, "+ ServiceClassListHandler::remove Key:%S",
         aKey.c_str());
    map<wstring, unsigned int>::iterator it;

    it = mServiceClassBitsMap.find(aKey);
    if (it != mServiceClassBitsMap.end())
    {
        LOG(EJavaBluetooth, EInfo, "  ServiceClassListHandler::remove erasing");
        mServiceClassBitsMap.erase(it);
    }
}

unsigned int ServiceClassListHandler::get(wstring aKey)
{
    JELOG2(EJavaBluetooth);
    map<wstring, unsigned int>::iterator it;

    it = mServiceClassBitsMap.find(aKey);
    if (it != mServiceClassBitsMap.end())
    {
        LOG2(EJavaBluetooth, EInfo,
             "- ServiceClassListHandler::get Key:%S result:0x%x",
             aKey.c_str(), (*it).second);

        return (*it).second;
    }

    return 0;
}

unsigned int ServiceClassListHandler::getCollectiveData()
{
    JELOG2(EJavaBluetooth);
    map<wstring, unsigned int>::iterator it;
    unsigned int retVal = 0;

    // show content:
    for (it = mServiceClassBitsMap.begin(); it != mServiceClassBitsMap.end(); it++)
    {
        retVal |= (*it).second;
    }
    return retVal;
}

} // namespace java
} // namespace bluetooth
