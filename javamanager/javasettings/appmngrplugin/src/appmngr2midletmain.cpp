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
* Description:  ECom implementation
*
*/


#include <ecom/implementationproxy.h>   // TImplementationProxy
#include "appmngr2midletruntime.h"      // CAppMngr2MidletRuntime


// ---------------------------------------------------------------------------
// Array that maps the ECom implementation UIDs to factory functions
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(KAppMngr2MidletUidValue, CAppMngr2MidletRuntime::NewL),
};

// ---------------------------------------------------------------------------
// ImplementationGroupProxy()
// Exported proxy for instantiation method resolution
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

