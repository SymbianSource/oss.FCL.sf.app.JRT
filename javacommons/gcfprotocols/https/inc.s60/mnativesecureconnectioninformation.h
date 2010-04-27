/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MNATIVESECURECONNECTIONINFORMATION_H
#define MNATIVESECURECONNECTIONINFORMATION_H

#include <e32std.h>
#include <x509cert.h>
#include <badesca.h>


class MNativeSecureConnectionInformation
{
public:
    virtual void GetSecurityInfoL(CDesCArray& aStringResult, TTime& aStartTime, TTime& aEndTime) = 0;
};



#endif // MNATIVESECURECONNECTIONINFORMATION_H
