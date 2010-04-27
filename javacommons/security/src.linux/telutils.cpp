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


#include "telutils.h"

using namespace java::security;

TelUtils* TelUtils::createInstance()
{
    return new TelUtils();
}

void TelUtils::getNetworkCodes(std::string& mnc, std::string& mcc)
{
    //JELOG2(EUtils);
    mnc += "000";
    mcc += "000";
}

void TelUtils::getImei(std::string& imei)
{
    //JELOG2(EUtils);
    imei += "000000000000000";
}
