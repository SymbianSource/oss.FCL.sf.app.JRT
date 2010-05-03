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


#ifndef SERVICECLASSLISTHANDLER_H
#define SERVICECLASSLISTHANDLER_H

#include <map>
#include <string>

namespace java
{
namespace bluetooth
{

class ServiceClassListHandler
{
public:
    ServiceClassListHandler();

    ~ServiceClassListHandler();

    void add(std::wstring aKey, unsigned int aData);

    void remove(std::wstring aKey);

    unsigned int get(std::wstring aKey);

    unsigned int getCollectiveData();

private:

    std::map<std::wstring, unsigned int> mServiceClassBitsMap;
};

} // namespace java
} // namespace bluetooth

#endif // SERVICECLASSLISTHANDLER_H
