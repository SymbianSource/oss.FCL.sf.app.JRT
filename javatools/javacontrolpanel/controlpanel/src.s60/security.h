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

#ifndef SECURITY_H
#define SECURITY_H

#include <string>
#include <memory>
#include "javastorage.h"

class Security
{
public:
    Security();
    ~Security();

    std::wstring getSecurityWarningsMode();
    void setSecurityWarningsMode(const std::wstring& aMode);
    int getSecurityPolicy();
    void setSecurityPolicy(int aPolicyId);

private:
    void findColumn(const java::storage::JavaStorageApplicationEntry_t& aEntry,
                    const std::wstring& aColName, std::wstring& aColValue);

    std::auto_ptr<java::storage::JavaStorage> iStorage;
};

#endif // SECURITY_H
