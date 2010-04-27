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


#ifndef JAVASMARTCARDCERTIFICATESREADER_H
#define JAVASMARTCARDCERTIFICATESREADER_H

#include "javaosheaders.h"
#include "trustedcertificate.h"
#include <string>
#include <vector>

using namespace std;

namespace java
{
namespace security
{

OS_NONSHARABLE_CLASS(JavaSmartCardCertificatesReader)
{
public:
    static void retrieveTrustedCertificates(vector<TrustedCertificate>& aTrustedCerts);
};

} //end namespace security
} //end namespace java

#endif // JAVASMARTCARDCERTIFICATESREADER_H
