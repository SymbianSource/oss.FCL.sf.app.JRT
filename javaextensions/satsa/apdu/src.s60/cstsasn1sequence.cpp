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
#include "cstsasn1sequence.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSDefaultGranularity = 5;
const TUint8 KSTSPaddingByte = 0xFF;
const TUint8 KSTSRemovedRecordTag = 0x00;
const TInt KSTSRecordHeaderLength = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSASN1Sequence::CSTSASN1Sequence
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSASN1Sequence::CSTSASN1Sequence()
{
}

// -----------------------------------------------------------------------------
// CSTSASN1Sequence::DecodeSequenceLC
// Decodes raw-data to ASN.1 modules, pushes items to cleanupstack
// -----------------------------------------------------------------------------

CArrayPtr<TASN1DecGeneric>* CSTSASN1Sequence::DecodeSequenceLC(TBool aCheckTag,
        const TDesC8& aRawData)
{
    CArrayPtr<TASN1DecGeneric>* items = NULL;

    // Check we've got a sequence
    TASN1DecGeneric decGen(aRawData);
    decGen.InitL();
    TTagType tag = decGen.Tag();
    // Accept only sequences or set if tag must be checked
    if ((tag != EASN1Sequence) && (tag != EASN1Set) && (aCheckTag))
    {
        User::Leave(KErrArgument);
    }
    else
    {
        // Decode sequences to array
        items = DecodeItemsLC(decGen);
    }
    return items;
}

// -----------------------------------------------------------------------------
// CSTSASN1Sequence::DecodeSequenceLC
// Decodes raw-data to ASN.1 modules, pushes items to cleanupstack
// Validates that there is right number of modules, leaves with KErrArgument if
// not.
// -----------------------------------------------------------------------------

CArrayPtr<TASN1DecGeneric>* CSTSASN1Sequence::DecodeSequenceLC(TBool aCheckTag,
        const TDesC8& aRawData, TInt aMinTerms, TInt aMaxTerms)
{
    CArrayPtr<TASN1DecGeneric>* items = DecodeSequenceLC(aCheckTag, aRawData);
    TInt count = items->Count();
    if ((count < aMinTerms) || (count > aMaxTerms))
    {
        // not in the range, leave
        User::Leave(KErrArgument);
    }

    return items;
}

// -----------------------------------------------------------------------------
// CSTSASN1Sequence::DecodeSequenceLC
// Decodes raw-data to ASN.1 modules, pushes items to cleanupstack
// Validates that there is right number of modules, leaves with KErrArgument if
// not.
// -----------------------------------------------------------------------------

CArrayPtr<TASN1DecGeneric>* CSTSASN1Sequence::DecodeSequenceLC(TBool aCheckTag,
        const TDesC8& aRawData, TInt aMinTerms)
{
    CArrayPtr<TASN1DecGeneric>* items = DecodeSequenceLC(aCheckTag, aRawData);
    TInt count = items->Count();
    if (count < aMinTerms)
    {
        // not in the range, leave
        User::Leave(KErrArgument);
    }

    return items;
}

// -----------------------------------------------------------------------------
// CSTSASN1Sequence::DecodeItemsLC
// Decodes raw-data to ASN.1 modules, pushes items to cleanupstack
// Checks removed records and padding bytes and ignores them.
// -----------------------------------------------------------------------------
CArrayPtrFlat<TASN1DecGeneric>* CSTSASN1Sequence::DecodeItemsLC(
    const TASN1DecGeneric& aRawData)
{
    CArrayPtrFlat<TASN1DecGeneric>* items = new(ELeave) CArrayPtrFlat<
    TASN1DecGeneric> (KSTSDefaultGranularity);
    TCleanupItem cleanupSeq(CleanupSequence, items);
    CleanupStack::PushL(cleanupSeq);

    TPtrC8 source(aRawData.GetContentDER());
    TInt sourceLength = source.Length();

    TInt pos = 0;
    while (pos < sourceLength)
    {
        TPtrC8 restOfData(source.Right(sourceLength - pos));
        TUint8 tag = restOfData[0];//first byte is tag

        TInt restOfDataLength = restOfData.Length();
        //check does contains removed record or padding bytes
        if (tag == KSTSRemovedRecordTag)
        {
            //we can jump over this record
            if (restOfDataLength >= KSTSRecordHeaderLength)
            {
                //2. byte is record length
                TInt recordDataLength = restOfData[1];
                //update position after this record
                pos += recordDataLength + KSTSRecordHeaderLength;
            }
            else
            {
                //there is only one byte left so in this Removed Tag case
                //we can ignore this and continue without leaving
                pos++;
            }
        }
        else if (tag == KSTSPaddingByte)
        {
            //we can jump over padding bytes
            TInt amountOfPaddingBytes = 1; //one padding byte is already found
            TBool paddingByte = ETrue;

            //we start from second byte, because first is already read
            for (TInt i = 1; (i < restOfDataLength) && (paddingByte); i++)
            {
                //take next value
                TUint8 nextByte = restOfData[i];
                //if padding byte, increase amount of padding bytes
                if (nextByte == KSTSPaddingByte)
                {
                    amountOfPaddingBytes++;
                }
                else //end loop
                {
                    paddingByte = EFalse;
                }
            }
            //update position after padding bytes
            pos += amountOfPaddingBytes;
        }
        else //we can append record to array
        {
            //take next record
            TASN1DecGeneric* decGenRecord =
                new(ELeave) TASN1DecGeneric(restOfData);
            CleanupStack::PushL(decGenRecord);
            decGenRecord->InitL();
            //append record to array
            items->AppendL(decGenRecord);
            //update position after record
            pos += decGenRecord->LengthDER();
            CleanupStack::Pop(decGenRecord);
        }
    }

    return items;

}

// -----------------------------------------------------------------------------
// CSTSASN1Sequence::CleanupSequence
//
// -----------------------------------------------------------------------------
void CSTSASN1Sequence::CleanupSequence(TAny* aArray)
{
    CArrayPtrFlat< TASN1DecGeneric>* array =
        REINTERPRET_CAST(CArrayPtrFlat< TASN1DecGeneric>*, aArray);
    array->ResetAndDestroy();
    delete array;
}

} // namespace satsa
} // namespace java
//  End of File
