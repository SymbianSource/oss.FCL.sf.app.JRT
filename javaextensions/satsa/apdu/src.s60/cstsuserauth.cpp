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
#include "cstsuserauth.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSMinNumberOfSubModules = 2;

const TInt KSTSDefaultGranularity = 1;
const TTagType KSTSAPDUPinEntryTag = 0;

// ============================ MEMBER FUNCTIONS ===============================


CSTSUserAuth::CSTSUserAuth()
{
    iType = ENotInitialized;
}

void CSTSUserAuth::ConstructL()
{
    // creating with empty values
    iAuthId = KNullDesC8().AllocL();

    iApduPinHeaders = new(ELeave) CDesC8ArrayFlat(KSTSDefaultGranularity);
}

CSTSUserAuth* CSTSUserAuth::NewLC()
{
    CSTSUserAuth* self = new(ELeave) CSTSUserAuth();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CSTSUserAuth::~CSTSUserAuth()
{
    delete iAuthId;
    delete iApduPinHeaders;
}

// -----------------------------------------------------------------------------
// CSTSUserAuth::DecodeL
// Decrypts raw data to this instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSUserAuth::DecodeL(const TDesC8& aRawData)
{
    CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC(ETrue, // must be sequence
                                            aRawData, KSTSMinNumberOfSubModules);

    // we would not get this far if there was atleast two element
    TInt pos = 0;
    // decoding authentication id (octet string)
    TASN1DecOctetString authId;
    HBufC8* tmp = authId.DecodeDERL(*itemsData->At(pos++));
    delete iAuthId;
    iAuthId = tmp;

    // decoding user authentication method (choice)
    // we need: apduPinEntry [0]APDUPinEntry
    TASN1DecGeneric taggedEntryDec(*itemsData->At(pos++));
    taggedEntryDec.InitL();
    if (taggedEntryDec.Tag() == KSTSAPDUPinEntryTag)
    {
        // set information that apduHeaders used
        iType = EAPDUPinEntry;
        // decoding apduPinEntry
        TASN1DecGeneric decGen(taggedEntryDec.GetContentDER());
        TRAPD(err, decGen.InitL());
        // it there is some optional headers inside APDUPINEntry
        if (!err)
        {
            CleanupStack::PopAndDestroy(itemsData);
            itemsData = DecodeSequenceLC(EFalse, taggedEntryDec.Encoding());
            TInt numOfHeaders = itemsData->Count();

            if (numOfHeaders)
            {
                CDesC8ArrayFlat* tmpArray =
                    new(ELeave) CDesC8ArrayFlat(numOfHeaders);
                CleanupStack::PushL(tmpArray);

                TUint currentTag = 0;
                TInt itemsLeft = numOfHeaders;
                // decode all pinheaders(octet string) in loop
                for (TInt i = 0; i < numOfHeaders; i++)
                {
                    TBool ready = EFalse;
                    TASN1DecGeneric decGenApduHeader(*itemsData->At(i));
                    // we must check tags in order 0 to ...
                    while (!ready && (currentTag <= (TUint)(itemsLeft
                                                            + currentTag)))
                    {
                        // add found header to correct place in buffer
                        if (decGenApduHeader.Tag() == currentTag)
                        {
                            TASN1DecGeneric decGenApduHeaderDER(
                                itemsData->At(i)->Encoding());
                            decGenApduHeaderDER.InitL();
                            TASN1DecOctetString apduHeader;
                            HBufC8* headerBuf = apduHeader.DecodeDERL(
                                                    decGenApduHeaderDER);
                            CleanupStack::PushL(headerBuf);

                            tmpArray->AppendL(headerBuf->Des());
                            CleanupStack::PopAndDestroy(headerBuf);
                            ready = ETrue;
                            itemsLeft--;
                        }
                        // there was no header for this tag, so add empty
                        else
                        {
                            tmpArray->AppendL(KNullDesC8());
                        }
                        currentTag++;
                    }
                }
                delete iApduPinHeaders;
                iApduPinHeaders = tmpArray;
                // compress to get rid of possible unneccessary slots
                iApduPinHeaders->Compress();
                CleanupStack::Pop(tmpArray);
            }
        }
    }
    else
    {
        iType = EOther;
        // we won't need others at all
    }
    CleanupStack::PopAndDestroy(itemsData);
}
// -----------------------------------------------------------------------------
// CSTSUserAuth::Type
// Getter for type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CSTSUserAuth::TType CSTSUserAuth::Type() const
{
    return iType;
}

// -----------------------------------------------------------------------------
// CSTSUserAuth::AuthId
// Getter for authentication id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC8& CSTSUserAuth::AuthId() const
{
    return *iAuthId;
}

// -----------------------------------------------------------------------------
// CSTSUserAuth::ApduPinHeaders
// Getter for PIN headers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const CDesC8ArrayFlat& CSTSUserAuth::ApduPinHeaders() const
{
    return *iApduPinHeaders;
}

} // namespace satsa
} // namespace java
//  End of File

