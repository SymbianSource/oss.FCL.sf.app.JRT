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
#include "cstsefacfile.h"
#include "cstsace.h"
#include "cstsfiledatamanager.h"

namespace java
{
namespace satsa
{

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSEFACFile::CSTSEFACFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSEFACFile::CSTSEFACFile(CSTSFileDataManager* aFileDataManager)
{
    iFileDataManager = aFileDataManager;
}

// -----------------------------------------------------------------------------
// CSTSEFACFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSEFACFile* CSTSEFACFile::NewLC(CSTSFileDataManager* aFileDataManager)
{
    CSTSEFACFile* self = new(ELeave) CSTSEFACFile(aFileDataManager);
    CleanupStack::PushL(self);
    return self;
}

// Destructor
CSTSEFACFile::~CSTSEFACFile()
{
}

// -----------------------------------------------------------------------------
// CSTSEFACFile::SetArray
// Sets the array, where suitable recors will be added. This array will own the
// objects after they are added there.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSEFACFile::SetArray(CArrayPtr<CSTSAce>* aRecordsArr)
{
    iRecordsArr = aRecordsArr;
}

// -----------------------------------------------------------------------------
// CSTSEFACFile::SetPath
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSEFACFile::SetPath(const TDesC8& aPath)
{
    iPath.Set(aPath);
}

// -----------------------------------------------------------------------------
// CSTSEFACFile::DecodeAndAppendRowL
// Decodes row and appends data to correct array. Ignores types which are
// not needed. Returns false, if maximum valid data amount is not reached.
// Returns true, if maxumum valid data amount is reached.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSEFACFile::DecodeAndAppendRowL(const TDesC8& aRowData,
                                        TInt aMaxValidDataAmount)
{
    TBool returnValue = EFalse;

    //decode CSTSAce
    CSTSAce* currentObject = CSTSAce::NewLC();
    currentObject->DecodeL(aRowData);

    //ownership moved to aAces array
    iRecordsArr->AppendL(currentObject);
    CleanupStack::Pop(currentObject);
    iFoundRecords++;

    if ((aMaxValidDataAmount != 0) && //if zero, no data limits used
            (iFoundRecords >= aMaxValidDataAmount))
    {
        //all needed data is found
        returnValue = ETrue;
    }
    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSEFACFile::ResetData
// Resets amount of found records and arrays of objects
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSEFACFile::ResetData()
{
    iFoundRecords = 0;
}

// -----------------------------------------------------------------------------
// CSTSEFACFile::RetrieveContentL
// Gets content of some ACF file identified by path
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC8& CSTSEFACFile::RetrieveContentL()
{
    return iFileDataManager->RetrieveACFContentL(iPath);
}

} // namespace satsa
} // namespace java
//  End of File
