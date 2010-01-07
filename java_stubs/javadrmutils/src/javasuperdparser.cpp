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
* Description:  CJavaSuperDParser implementation (EXCLUDE_JAVA version)
*
*/


#include "javasuperdparser.h"

using namespace Java;


// ======== MEMBER FUNCTIONS ========


// ----------------------------------------------------------------------------
// CJavaSuperDParser::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CJavaSuperDParser* CJavaSuperDParser::NewL(RFs& /* aFileSession */,
        const TDesC& /* aJarFileName */,
        const TDesC& /* aDestinationFolder */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ----------------------------------------------------------------------------
// CJavaSuperDParser::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CJavaSuperDParser* CJavaSuperDParser::NewL(RFs& /* aFileSession */,
        RFile& /* aJarFileHandle */,
        const TDesC& /* aDestinationFolder */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ----------------------------------------------------------------------------
// CJavaSuperDParser::~CJavaSuperDParser()
// ----------------------------------------------------------------------------
//
CJavaSuperDParser::~CJavaSuperDParser()
{
}

// ----------------------------------------------------------------------------
// CJavaSuperDParser::CreateSuperDPackageL()
// ----------------------------------------------------------------------------
//
EXPORT_C void CJavaSuperDParser::CreateSuperDPackageL(RFs& /* aFileSession */,
        const TDesC& /* aDestinationFileName */,
        const TDesC& /* aDescriptorFileName */,
        const TDesC& /* aContentFileName */)
{
    User::Leave(KErrNotSupported);
}

// ----------------------------------------------------------------------------
// CJavaSuperDParser::DescriptorFileName
// ----------------------------------------------------------------------------
//
EXPORT_C TFileName CJavaSuperDParser::DescriptorFileName() const
{
    return TFileName(KNullDesC);
}

// ----------------------------------------------------------------------------
// CJavaSuperDParser::ContentFileName
// ----------------------------------------------------------------------------
//
EXPORT_C TFileName CJavaSuperDParser::ContentFileName() const
{
    return TFileName(KNullDesC);
}

// End of File
