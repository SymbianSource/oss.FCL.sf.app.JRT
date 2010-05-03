/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/


#include <ApUtils.h>
#include "swtbrowseraputils.h"



CSwtBrowserApUtils::CSwtBrowserApUtils()
{
}


void CSwtBrowserApUtils::ConstructL(CCommsDatabase& aDb)
{
    iApUtils = CApUtils::NewLC(aDb);
    CleanupStack::Pop(iApUtils);
}


EXPORT_C CSwtBrowserApUtils* CSwtBrowserApUtils::NewL(CCommsDatabase& aDb)
{
    CSwtBrowserApUtils* self
    = new(ELeave) CSwtBrowserApUtils();
    CleanupStack::PushL(self);
    self->ConstructL(aDb);
    CleanupStack::Pop(self);
    return self;
}

CSwtBrowserApUtils::~CSwtBrowserApUtils()
{
    delete iApUtils;
}

EXPORT_C TBool CSwtBrowserApUtils::WapApExistsL(TUint32 aUid)
{
    ASSERT(iApUtils);
    return iApUtils->WapApExistsL(aUid);
}


EXPORT_C TUint32 CSwtBrowserApUtils::IapIdFromWapIdL(TUint32 aUid)
{
    ASSERT(iApUtils);
    return iApUtils->IapIdFromWapIdL(aUid);
}

EXPORT_C TUint32 CSwtBrowserApUtils::WapIdFromIapIdL(TUint32 aIapId)
{
    ASSERT(iApUtils);
    return iApUtils->WapIdFromIapIdL(aIapId);
}

EXPORT_C TInt CSwtBrowserApUtils::BearerTypeL(TUint32 aWapId)
{

    ASSERT(iApUtils);
    return iApUtils->BearerTypeL(aWapId);
}
