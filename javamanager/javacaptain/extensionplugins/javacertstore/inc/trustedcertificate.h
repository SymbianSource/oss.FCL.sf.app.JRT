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
* Description:  JavaCertStore
*
*/

#ifndef TRUSTEDCERTIFICATE_H
#define TRUSTEDCERTIFICATE_H

#include "javaosheaders.h"

#include <string>
#include <vector>

using namespace std;

namespace java
{
namespace security
{

OS_NONSHARABLE_CLASS(TrustedCertificate)
{

public:
    TrustedCertificate(const string& aData, const string& aId, const vector<string>& aTrustedUsage);

    string getData();
    string getId();
    vector<string> getTrustedUsage();

private:
    string iData;
    string iId;
    vector<string> iTrustedUsage;
};

inline TrustedCertificate::TrustedCertificate(const string& aData, const string& aId, const vector<string>& aTrustedUsage)
        : iData(aData), iId(aId), iTrustedUsage(aTrustedUsage) {}

inline string TrustedCertificate::getData()
{
    return iData;
}
inline string TrustedCertificate::getId()
{
    return iId;
}
inline vector<string> TrustedCertificate::getTrustedUsage()
{
    return iTrustedUsage;
}

} // namespace security
} // namespace java

#endif // TRUSTEDCERTIFICATE_H

