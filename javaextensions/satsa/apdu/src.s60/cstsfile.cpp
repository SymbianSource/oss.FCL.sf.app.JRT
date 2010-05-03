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

#include  "cstsfile.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TUint8 KSTSSequenceTag = 0x30;
//sequence byte + 3 bytes for long form length
const TInt KSTSTLVHeaderLength = 4;
const TInt KSTSFirstOctet = 8;
const TUint8 KSTSInitialLengthOctet = 0x82;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSFile::CSTSFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSFile::CSTSFile()
{
}

// Destructor
CSTSFile::~CSTSFile()
{
    delete iDataBuf;
}

// -----------------------------------------------------------------------------
// CSTSFile::ReadL
// Reads and decodes file data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSFile::ReadL(TInt aMaxValidDataAmount)
{
    ResetData();

    TPtrC8 fileData;
    fileData.Set(RetrieveContentL());
    iFileSize = fileData.Length();
    iMaxValidDataAmount = aMaxValidDataAmount;
    //allocate buffer for file data
    HBufC8* tmp = HBufC8::NewL(iFileSize + KSTSTLVHeaderLength);
    delete iDataBuf;
    iDataBuf = tmp;

    //append sequence tag and size bytes
    iDataBuf->Des().Append(KSTSSequenceTag);
    iDataBuf->Des().Append(KSTSInitialLengthOctet);
    iDataBuf->Des().Append(iFileSize >> KSTSFirstOctet);//first octet=high byte of length
    iDataBuf->Des().Append(iFileSize); //second octet=low byte of length

    TPtr8 wholeData(iDataBuf->Des());
    wholeData.Append(fileData);

    DecodeL(wholeData);
}

// -----------------------------------------------------------------------------
// CSTSFile::FoundRecords
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CSTSFile::FoundRecords() const
{
    return iFoundRecords;
}

// -----------------------------------------------------------------------------
// CSTSFile::DecodeL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSFile::DecodeL(const TDesC8& aRawData)
{
    CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC(EFalse, //must be sequence
                                            aRawData);

    TInt count = itemsData->Count();
    TBool validDataFound = EFalse;

    for (TInt i = 0; (i < count) && !validDataFound; i++)
    {
        validDataFound = DecodeAndAppendRowL(itemsData->At(i)->Encoding(),
                                             iMaxValidDataAmount);
    }
    CleanupStack::PopAndDestroy(itemsData);

}

} // namespace satsa
} // namespace java
//  End of File
