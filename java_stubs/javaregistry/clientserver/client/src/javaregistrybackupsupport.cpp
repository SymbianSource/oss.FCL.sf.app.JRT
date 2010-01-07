/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregistrybackupsupport implementation
*              : (__JAVA_EXCLUDED VERSION)
*
*/


// INCLUDES
#include "javaregistrybackupsupport.h"

using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaRegistryBackupSupport::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryBackupSupport* CJavaRegistryBackupSupport::NewL()
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistryBackupSupport::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryBackupSupport* CJavaRegistryBackupSupport::NewLC()
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistryBackupSupport::~CJavaRegistryBackupSupport
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryBackupSupport::~CJavaRegistryBackupSupport()
{
}

// ---------------------------------------------------------------------------
// CJavaRegistryBackupSupport::EntriesExistForDriveL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistryBackupSupport::
EntriesExistForDriveL(const TDriveNumber& /* aDrive */) const
{
    User::Leave(KErrNotSupported);
    return EFalse;
}

// ---------------------------------------------------------------------------
// CJavaRegistryBackupSupport::ChangeBURState
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CJavaRegistryBackupSupport::
ChangeBURState(const TBackupState& /* aState */) const
{
    return KErrNotSupported;
}
