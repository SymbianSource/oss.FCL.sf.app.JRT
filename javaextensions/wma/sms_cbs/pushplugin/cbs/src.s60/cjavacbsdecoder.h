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

#ifndef CJAVACBSDECODER_H
#define CJAVACBSDECODER_H

#include <gsmuelem.h>
#include "javasmsattributes.h"

namespace java
{
namespace wma
{

// Value used if the message page number is indeterminable
const TInt KUnknownPageNumber = -1;

// Maximum number of pages
const TUint KMaximumNumOfPages = 15;

// Values used by all decoding classes
const TUint8 KMaskUpperNibble = 0xf0;
const TUint8 KMaskLowerNibble = 0x0f;

// Note bits go : 76543210
const TUint8 KMaskBits76 = 0xc0;
const TUint8 KMaskBit2 = 0x04;
const TUint8 KMaskBits32 = 0x0C;

// 16 bit masks
// MSB is on LHS
const TUint8 KUnpackMaskBits = 0x7F;
const TUint16 KMaskBits_4_to_13 = 0x3ff0;

const TInt KSerialNumberUpperpart = 0;
const TInt KSerialNumberLowerpart = 1;
const TInt KMessageIdUpperpart = 2;
const TInt KMessageIdLowerpart = 3;
const TInt KDataCodingSchemeIndex = 4;
const TInt KPageInfoIndex = 5;
const TInt KGsmHeaderSize = 6;

class CJavaCbsDecoder: public CBase
{

public:
    //Constructor
    CJavaCbsDecoder();
    //Destructor
    ~CJavaCbsDecoder();

    /*
    * Gets the message code
    */
    TUint getMessageCode();

    /*
     * Gets the coding scheme
     */
    TSmsDataCodingScheme::TSmsAlphabet getEncoding();

    /**
     *This finds the total number of pages & page number of the CBS message
     *@param[in] aPageInfo  Contains info about pageNumber & total Num of pages.
     *@param1[out] aPageNumber  page number of the Cbs message
     *@param2[out] aTotalNumPages  total number of pages in the Cbs message
     */
    void findPageInfo(const TUint8& aPageInfo,TInt& aPageNumber,
                      TUint& aTotalNumPages);
    /**
     *This takes the data held in aInData and converts it as specified by
     *the CodingScheme, unpacking it if necessary.
     *@param1 aIndata - The incoming cbs data
     *@param2 aOutData - the converted and unpacked cbs data.
     *@param3 aCodingScheme -
     *@param4 aRfs - handle to file server
     */
    void decodeMessageL(const TDesC8& aInData, TCBSDataBuf& aOutData,
                        const TUint8 aCodingScheme,RFs& aRFs);
    /**
     * This function identifies the serial number of this page .
     * @param aSerialNum -This parameter is a 16-bit integer which identifies
     * a particular CBS message (which may be 1 to 15 pages in length).
     */
    void findMessageCode(const TUint16 aSerialNum);
protected:
    /**
     *This is used to calculate the data coding scheme of the message
     *this is done according to the 3 GPP TS 23.038.
     *@param aCodingScheme -
     */
    void findCodingSchemeL(const TUint8 aCodingScheme);

    /**
     *This function takes packed 7 bit data and pads it out to 8 bit data
     *Function will actually append data onto end of an existing buffer.
     *@param1 aInData - the packed 7 bit data
     *@param2 aOutData - the unpacked data.
     */
    void unpack7BitData(const TDesC8& aInData, TDes8& aOutData);
private:
    bool isGSM7(const TUint8 aCodingScheme);
    bool isUCS2(const TUint8 aCodingScheme);
    bool isBinary(const TUint8 aCodingScheme);
protected:
    TSmsDataCodingScheme::TSmsAlphabet mCodingScheme;
    TUint mMessageCode;
};

inline TSmsDataCodingScheme::TSmsAlphabet CJavaCbsDecoder::getEncoding()
{
    return mCodingScheme;
}

inline TUint CJavaCbsDecoder::getMessageCode()
{
    return mMessageCode;
}

} // end of namespace wma
} // end of namespace java


#endif // CJAVACBSDECODER_H
