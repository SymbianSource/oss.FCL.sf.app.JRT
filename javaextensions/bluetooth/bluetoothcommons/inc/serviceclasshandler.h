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


#ifndef SERVICECLASSHANDLER_H
#define SERVICECLASSHANDLER_H

#include <string>
#include "commsclientendpoint.h"
#include "commsmessage.h"

namespace java
{
namespace bluetooth
{

class ServiceClassHandler
{
public:

    /*
     * If aFlag is true, the value of the aSrvClassVal is OR'd with the current entry
     * Otherwise, it will be replaced.
     */
    OS_IMPORT
    static int setDeviceServiceClass(unsigned int aSrvClassVal,
                                     bool aIsServiceBitsPersistent = false);

private:
    static std::wstring getMidletId();
    static int sendMsg(java::comms::CommsMessage aMsg);

};

} //end namespace bluetooth
} //end namespace java
#endif // SERVICECLASSHANDLER_H
