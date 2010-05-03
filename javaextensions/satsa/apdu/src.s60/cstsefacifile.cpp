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

#include "cstsefacifile.h"
#include "cstsacie.h"
#include "cstsfiledatamanager.h"

namespace java
{
namespace satsa
{

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSEFACIFile::CSTSEFACIFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSEFACIFile::CSTSEFACIFile(CSTSFileDataManager* aFileDataManager)
{
    iFileDataManager = aFileDataManager;
}

// -----------------------------------------------------------------------------
// CSTSEFACIFile::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSEFACIFile::ConstructL()
{
    iAID = KNullDesC8().AllocL();
}

// -----------------------------------------------------------------------------
// CSTSEFACIFile::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSEFACIFile* CSTSEFACIFile::NewLC(CSTSFileDataManager* aFileDataManager)
{
    CSTSEFACIFile* self = new(ELeave) CSTSEFACIFile(aFileDataManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// Destructor
CSTSEFACIFile::~CSTSEFACIFile()
{
    delete iAID;
}

// -----------------------------------------------------------------------------
// CSTSEFAODFFile::SetArray
// Sets the array, where suitable recors will be added. This array will own the
// objects after they are added there.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSEFACIFile::SetArray(CArrayPtr<CSTSAcie>* aRecordsArr)
{
    iRecordsArr = aRecordsArr;
}

// -----------------------------------------------------------------------------
// CSTSEFACIFile::SetAIDL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSEFACIFile::SetAIDL(const TDesC8& aAID)
{
    HBufC8* tmp = aAID.AllocL();
    delete iAID;
    iAID = tmp;
}

// -----------------------------------------------------------------------------
// CSTSEFACIFile::DecodeAndAppendRowL
// Decodes row and appends data to correct array. Ignores types which are
// not needed. Returns false, if maximum valid data amount is not reached.
// Returns true, if maxumum valid data amount is reached.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSEFACIFile::DecodeAndAppendRowL(const TDesC8& aRowData,
        TInt aMaxValidDataAmount)
{

    TBool returnValue = EFalse;

    //decode CSTSAcie
    CSTSAcie* currentObject = CSTSAcie::NewLC();
    currentObject->DecodeL(aRowData);

    //if we found correct AID or acie is meant for all ( it is null )
    if (currentObject->AID() == iAID->Des() || currentObject->AID()
            == KNullDesC8())
    {
        iRecordsArr->AppendL(currentObject);
        CleanupStack::Pop(currentObject);
        iFoundRecords++;
    }
    else
    {
        CleanupStack::PopAndDestroy(currentObject);
    }

    if ((aMaxValidDataAmount != 0) && //if zero, no data limits used
            (iFoundRecords >= aMaxValidDataAmount))
    {
        //all needed data is found
        returnValue = ETrue;
    }
    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSEFACIFile::ResetData
// Resets amount of found records and arrays of objects
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSEFACIFile::ResetData()
{
    iFoundRecords = 0;
}

// -----------------------------------------------------------------------------
// CSTSEFACFile::RetrieveContentL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC8& CSTSEFACIFile::RetrieveContentL()
{
    return iFileDataManager->RetrieveACIFContentL();
}

} // namespace satsa
} // namespace java
//  End of File
