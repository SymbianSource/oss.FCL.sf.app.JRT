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
* Description:
 *
*/


// INCLUDE FILES
#include <asn1dec.h>
#include    "cstspath.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSMinNumberOfSubModules = 1;
const TInt KSTSMaxNumberOfSubModules = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSPath::CSTSPath
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSPath::CSTSPath()
{
}

// -----------------------------------------------------------------------------
// CSTSPath::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSPath::ConstructL()
{
    iPath = KNullDesC8().AllocL();
}

// -----------------------------------------------------------------------------
// CSTSPath::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSPath* CSTSPath::NewLC()
{
    CSTSPath* self = new(ELeave) CSTSPath();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CSTSPath::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSPath* CSTSPath::NewL()
{
    CSTSPath* self = CSTSPath::NewLC();
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CSTSPath::~CSTSPath()
{
    delete iPath;
}

// -----------------------------------------------------------------------------
// CSTSPath::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CSTSPath::DecodeL(const TDesC8& aRawData)
{
    CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC(ETrue, //must be sequence
                                            aRawData, KSTSMinNumberOfSubModules, KSTSMaxNumberOfSubModules);

    // we would not get this far if there is not 1-3 elements
    TInt pos = 0;
    if (pos >= itemsData->Count())
    {
        User::Leave(KErrArgument);
    }

    //decoding Path (octet string)
    TASN1DecOctetString path;
    HBufC8* tmpPath = path.DecodeDERL(*itemsData->At(pos++));
    delete iPath;
    iPath = tmpPath;
    //optional index and optional length are not needed

    CleanupStack::PopAndDestroy(itemsData);

}

// -----------------------------------------------------------------------------
// CSTSPath::Path
// Getter for Path
// -----------------------------------------------------------------------------
const TDesC8& CSTSPath::Path() const
{
    return *iPath;
}

} // namespace satsa
} // namespace java
//  End of File
