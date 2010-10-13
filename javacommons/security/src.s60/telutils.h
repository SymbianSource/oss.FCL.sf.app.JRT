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
#include <etelmm.h>
#include <mmtsy_names.h>
#include <e32base.h>

namespace java
{
namespace security
{

class TelUtils: public CBase
{
public:
    static TelUtils* createInstance();
    void getNetworkCodes(std::string& mnc, std::string& mcc);
    void getImei(std::string& imei);
    static signed int getSecureTime();
    virtual ~TelUtils();

private: //Methods
    TelUtils();
    void ConstructL();
    void descToStr(HBufC * , std::string&);
    RTelServer                                  iServer;
    RMobilePhone                                iPhone;
    HBufC*                                      iImei;
    HBufC*                                      iMnc;
    HBufC*                                      iMcc;
};

} //end namespace security
} //end namespace java

#endif // TELUTILS_H
