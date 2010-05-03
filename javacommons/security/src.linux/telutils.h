/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TELUTILS_H
#define TELUTILS_H

#include <string>

namespace java
{
namespace security
{

class TelUtils
{
public:
    static TelUtils* createInstance();
    void getNetworkCodes(std::string& mnc, std::string& mcc);
    void getImei(std::string& imei);
};

} //end namespace security
} //end namespace java

#endif // TELUTILS_H
