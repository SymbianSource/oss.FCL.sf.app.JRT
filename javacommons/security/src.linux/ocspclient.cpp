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


#include "ocspclient.h"

using namespace java::security;
using namespace std;

OcspClient* OcspClient::createInstance(long iap, const char* defaultUrl)
{
    return new OcspClient();
}
void OcspClient::startOcspCheck(const char ** certChain, int certChainLen)
{
}

OcspResponse OcspClient::getOcspCheckResponse()
{
    OcspResponse response;
    response.iSummary = RESPONSE_GOOD;
    return response;
}

void OcspClient::cancelOcspCheck(bool doCleanup)
{
}
