/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class
*
*/


// CLASS HEADER
#include "pimlocalizationutils.h"
#include "s60commonutils.h"

// EXTERNAL INCLUDES
#include <barsc.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PIMLocalizationUtils::OpenResourceFileLC
// -----------------------------------------------------------------------------
//
void PIMLocalizationUtils::OpenResourceFileLC(
    RFs& aRfs,
    RResourceFile& aResourceFile,
    const TDesC& aFileName)
{
    // Get nearest language file
    TFileName langFile =
        java::util::S60CommonUtils::ResourceLanguageFileNameL(aFileName);

    // Open the requested resource file
    aResourceFile.OpenL(aRfs, langFile),
    CleanupClosePushL(aResourceFile);

    // Confirm signature
    aResourceFile.ConfirmSignatureL();
}

// End of file
