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
* Description:  Implements interface that exports ECOM implementation
 *                factories within this .dll
 *
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "javainstalllauncher.h"
#include "javauids.h"

// CONSTANTS
const TImplementationProxy KImplementationTable[] =
    { IMPLEMENTATION_PROXY_ENTRY(KInstallerFrontEndEcomImplUid,
                                 Java::Installer::CJavaInstallLauncher::NewL)
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns the implementations in this DLL
// Returns: The ECOM implementations in this DLL
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
}

//  End of File
