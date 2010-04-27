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


#include "logger.h"
#include "cjavacbsdecoder.h"

namespace java
{
namespace wma
{

CJavaCbsDecoder::CJavaCbsDecoder()
{
}

CJavaCbsDecoder::~CJavaCbsDecoder()
{
}

/*We do not support compressed formats and ignore the
 *Message Class. Leave if type is unsupported.
 */
void CJavaCbsDecoder::findCodingSchemeL(const TUint8 aCodingScheme)
{
    JELOG2(EWMA);
    if (isGSM7(aCodingScheme))
    {
        mCodingScheme = TSmsDataCodingScheme::ESmsAlphabet7Bit;
        return;
    }
    else if (isUCS2(aCodingScheme))
    {
        mCodingScheme = TSmsDataCodingScheme::ESmsAlphabetUCS2;
        return;
    }
    else if (isBinary(aCodingScheme))
    {
        mCodingScheme = TSmsDataCodingScheme::ESmsAlphabet8Bit;
        return;
    }
    User::Leave(KErrNotSupported);
}

/* All of the values are hard coded to allow easier interpretation
 * when comparing with the docmentation.
 */
bool CJavaCbsDecoder::isGSM7(const TUint8 aCodingScheme)
{
    JELOG2(EWMA);
    TUint8 upperNibble = aCodingScheme & KMaskUpperNibble;
    TUint8 bits76 = aCodingScheme & KMaskBits76;
    TUint8 bits32 = aCodingScheme & KMaskBits32;
    TUint8 bit2 = aCodingScheme & KMaskBit2;
    if (aCodingScheme == 0xF0)
    {
        return true;
    }
    else if (upperNibble == 0x00 ||upperNibble == 0x20 ||upperNibble == 0x30)
    {
        return true;
    }
    else if ((bits76 == 0x40 && bits32 == 0x00) ||
             (bits76 == 0xC0 && bit2 == 0x00))
    {
        return true;
    }
    return false;
}

bool CJavaCbsDecoder::isUCS2(const TUint8 aCodingScheme)
{
    JELOG2(EWMA);
    if (aCodingScheme == 0xF1)
    {
        return true;
    }
    return false;
}

bool CJavaCbsDecoder::isBinary(const TUint8 aCodingScheme)
{
    JELOG2(EWMA);
    TUint8 bits76 = aCodingScheme & KMaskBits76;
    TUint8 bits32 = aCodingScheme & KMaskBits32;
    TUint8 bit2 = aCodingScheme & KMaskBit2;
    if ((bits76 == 0x40 && (bits32 == 0x04 || bits32 == 0x08)) ||
            (bits76 == 0xC0 && bit2 == 0x04))
    {
        return true;
    }
    return false;
}

void CJavaCbsDecoder::decodeMessageL(const TDesC8& aInData, TCBSDataBuf& aOutData,
                                     const TUint8 aCodingScheme,RFs& aRFs)
{
    JELOG2(EWMA);
    // Unpack the data if necessary
    TBuf8<KCBSMaxDataSize> cbsData;
    TBool isBinary = EFalse;
    findCodingSchemeL(aCodingScheme);
    if (mCodingScheme == TSmsDataCodingScheme::ESmsAlphabet7Bit)
    {
        unpack7BitData(aInData, cbsData);
    }
    else if (mCodingScheme == TSmsDataCodingScheme::ESmsAlphabet8Bit)
    {
        cbsData = aInData;
        isBinary = ETrue;

    }
    else if (mCodingScheme == TSmsDataCodingScheme::ESmsAlphabetUCS2)
    {
        cbsData = aInData;
    }
    else
    {
        User::Leave(KErrNotSupported);
    }
    LOG1(EWMA, EInfo,"WMA : InPut Data Length = %d ",aInData.Length());
    // Construct a decoder
    CCnvCharacterSetConverter* cConv = CCnvCharacterSetConverter::NewLC();
    CSmsAlphabetConverter* smsConverter = CSmsAlphabetConverter::NewLC(*cConv,
                                          aRFs, mCodingScheme, isBinary);


    TPtrC dataPtr = smsConverter->ConvertToNativeL(cbsData);


    // Strip all trailing padding characters from the message.
    TInt length = dataPtr.Length();
    TInt dataLength = 0;
    LOG1(EWMA,EInfo,"WMA : Length of the Data before stripping: %d ", length);
    TText paddingChar = 0x000D; // PADDING IS UNICODE 'CR'
    for (; dataLength < length ; dataLength++)
    {
        if (dataPtr[dataLength] == paddingChar)
        {
            break;
        }
    }

    // Copy the result to output buffer
    aOutData.Copy(dataPtr.Left(dataLength));
    LOG1(EWMA,EInfo,"WMA :Length of the Received CBS Data : %d ", dataLength);

    CleanupStack::PopAndDestroy(smsConverter);
    CleanupStack::PopAndDestroy(cConv);

}


void CJavaCbsDecoder::findPageInfo(const TUint8& aPageInfo,TInt& aPageNumber,
                                   TUint& aTotalNumPages)
{
    JELOG2(EWMA);
    // aPageInfo is coded as two 4-bit fields. So split it into 2 parts
    // upperNibble indicates binary value of the total no. of pages
    TUint8 upperNibble = static_cast<TUint8>(aPageInfo & KMaskUpperNibble);
    // lowerNibble indicates binary value of the page number within that sequence.
    TUint8 lowerNibble = static_cast<TUint8>(aPageInfo & KMaskLowerNibble);


    // If the code 0000 is received in either the upper or the
    // lower nibble then the CBS message is handled exactly the same as
    // as a single page CBS message.
    if (!upperNibble || !lowerNibble)
    {
        aTotalNumPages = 1;
        aPageNumber = 1;
    }
    else
    {
        aTotalNumPages = lowerNibble;
        aPageNumber = upperNibble >> 4;
    }

}

void CJavaCbsDecoder::findMessageCode(const TUint16 aSerialNum)
{
    JELOG2(EWMA);
    // the 16 bit serialNum is split is coded in 3 parts. Bits 0-3 indicate Update
    // Number, 4-13 indicate Message code and 14-15 indicate Geographical scope
    // Hence MessageCode is obtained by masking bits 4-13
    mMessageCode = (aSerialNum & KMaskBits_4_to_13) >> 4;
}

//This function unpacks a 7bit data into 8bit data.
void CJavaCbsDecoder::unpack7BitData(const TDesC8& aInData, TDes8& aOutData)
{
    JELOG2(EWMA);
    TInt length = aInData.Length();
    //Calculate number of GSM-7 characters present in data
    TInt count = (length * 8) / 7;
    const TUint8* inPtr = aInData.Ptr();
    TUint8* outPtr = (TUint8*) aOutData.Ptr() + aOutData.Length();

    TUint8 shiftBit = 0;
    TUint8 leftBits = 0;
    TUint8 resultBits = 0;
    // Iterate over each 8 bit element
    for (TInt ii = 0; ii < length; ii++, inPtr++)
    {
        resultBits = leftBits | (*inPtr << shiftBit);
        resultBits = KUnpackMaskBits & resultBits;
        leftBits = (*inPtr >> (7 - shiftBit));
        *outPtr++ = (TUint8) resultBits;
        //Since for every seven 8-bit data , we have eigth 7-bit data
        //that extra 7-bit data is unpacked to OutData.
        if (shiftBit == 6)
        {
            *outPtr++ = (TUint8)(leftBits & KUnpackMaskBits);
            leftBits = 0;
            shiftBit = 0;
        }
        else
        {
            shiftBit++;
        }
    }
    aOutData.SetLength(aOutData.Length() + count);

}

} // end of namespace wma
} // end of namespace java


