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


#ifndef WIMCERT_H
#define WIMCERT_H

#include <e32base.h>
#include <e32std.h>
#include <ct.h>

enum TCertLocation
{
    EUnknown_CDF,
    ECertificates_CDF,
    ETrustedCerts_CDF,
    EUsefulCerts_CDF
};

class CWimCert : public CBase
{
public:

    IMPORT_C static CWimCert* NewL();
    IMPORT_C virtual ~CWimCert();
    IMPORT_C void Initialize(TRequestStatus& aStatus);
    IMPORT_C void CancelInitialize();
    IMPORT_C void GetExtras(const TDesC8& aKeyId,
                            RCPointerArray<HBufC>& aTrustedUsages,
                            TCertLocation& aLocation,
                            TRequestStatus& aStatus);
    IMPORT_C void CancelGetExtras();
private:
    CWimCert();
};
#endif // WIMCERT_H
