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

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
//#include <ImplementationProxy.h>
#include "javacertstoretokentype.h"

const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(0x200213A3, java::security::CJavaCertStoreTokenType::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

TInt E32Dll()
{
    return KErrNone;
}

/*
const TImplementationProxy ImplementationTable[] =
  {
    IMPLEMENTATION_PROXY_ENTRY(0x101F5B71, java::security::CJavaCertStoreImpl::NewL)
  };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
*/
