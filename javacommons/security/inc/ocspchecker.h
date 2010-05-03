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


#ifndef OCSPCHECKER_H
#define OCSPCHECKER_H


namespace java
{
namespace security
{
// constants for the OCSP responses
const int RESPONSE_GOOD = 1;
const int RESPONSE_REVOKED = 2;
const int RESPONSE_UNKNOWN = 3;
const int RESPONSE_CANNOT_OBTAIN_CERT_STATUS = 4;
const int RESPONSE_INVALID_REVOCATION_SERVER_URI = 5;
const int RESPONSE_SIGNATURE_VALIDATION_FAILURE = 6;
const int RESPONSE_INVALID_REVOCATION_SERVER_RESPONSE = 7;
const int RESPONSE_MISSING_NONCE = 8;
const int RESPONSE_INVALID_CERT_STATUS_INFO = 9;
}//end namespace security
}//end namespace java
#endif // OCSPCHECKER_H
