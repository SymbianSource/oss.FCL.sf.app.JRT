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


#ifndef OCSPCLIENT_H
#define OCSPCLIENT_H

#include "ocspchecker.h"
#include <vector>

namespace java
{
namespace security
{

class OcspResponse
{

public:

    int iSummary;
    std::vector<int> iIndividualResponses;
};

class OcspClient
{
public:
    static OcspClient* createInstance(long iap, const char* defaultUrl);
    void startOcspCheck(const char **, int);
    OcspResponse getOcspCheckResponse();
    void cancelOcspCheck(bool doCleanup);
};

} //end namespace security
} //end namespace java

#endif // OCSPCLIENT_H
