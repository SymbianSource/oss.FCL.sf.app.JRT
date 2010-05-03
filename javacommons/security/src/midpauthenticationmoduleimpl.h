/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIDPAUTHENTICATIONMODULEIMPL_H
#define MIDPAUTHENTICATIONMODULEIMPL_H

#include "midpauthenticationmodule.h"

namespace java
{
namespace security
{

// as a convention, the most descriptive result/failure code has the highest index value
const int KCertAndSignatureOk = 1;
const int KSignatureVerificationFailure = -1;
const int KCertNotYetValidFailure = -2;
const int KCertExpiredFailure = -3;
const int KCertValidationFailure = -4;
const int KSelfSignedCertInChainFailure = -5;
const int KMissingRoot = -6;
const int KUnknownExtendedKeyUsage = -7;
// the default is always the last one
const int KDefault = -8;

class MIDPAuthenticationModuleImpl: public MIDPAuthenticationModule
{
public:

    MIDPAuthenticationModuleImpl();

    OS_IMPORT virtual void getCertChains(const java::util::Uid& aUid,
                                         std::list<std::string>& aChains);

};

} // end namespace security
} // end namespace java

#endif // MIDPAUTHENTICATIONMODULEIMPL_H

