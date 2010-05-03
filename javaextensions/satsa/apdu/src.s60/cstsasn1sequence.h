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


#ifndef CSTSASN1SEQUENCE_H
#define CSTSASN1SEQUENCE_H

//  INCLUDES
#include    <asn1dec.h>
#include    "mstsasn1module.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Base class for sequence (or set) type of modules.
 *
 *  @since 3.0
 */
class CSTSASN1Sequence: public CBase, public MSTSASN1Module
{
public:
    // New functions
    /**
     * C++ default constructor.
     */
    CSTSASN1Sequence();

    /**
     * Decode a generic ASN1 sequence
     * @since 3.0
     * @param aCheckTag If true, sequence or set tag is be checked
     * @param aRawData raw-data to be splitted in ASN.1 Modules
     * @return Array of generic asn.1 modules.
     */
    static CArrayPtr<TASN1DecGeneric>* DecodeSequenceLC(TBool aCheckTag,
            const TDesC8& aRawData);

    /**
     * Decode a generic ASN1 sequence, with a minimum/maximum number
     * of top level terms. Will leave with KErrArgument if number of terms
     * is not between valid range (including min and max)
     * @since 3.0
     * @param aCheckTag If true, sequence or set tag is be checked
     * @param aRawData raw-data to be splitted in ASN.1 Modules
     * @param aMinTerms minimum of top level terms.
     * @param aMaxTerms maximum number of top level terms.
     * @return Array of generic asn.1 modules.
     */
    static CArrayPtr<TASN1DecGeneric>* DecodeSequenceLC(TBool aCheckTag,
            const TDesC8& aRawData, TInt aMinTerms, TInt aMaxTerms);
    /**
     * Decode a generic ASN1 sequence, with a minimum number
     * of top level terms. Will leave with KErrArgument if number of terms
     * is not at least the minimum amount
     * @since 3.0
     * @param aCheckTag If true, sequence or set tag is be checked
     * @param aRawData raw-data to be splitted in ASN.1 Modules
     * @param aMinTerms minimum of top level terms.
     * @return Array of generic asn.1 modules.
     */
    static CArrayPtr<TASN1DecGeneric>* DecodeSequenceLC(TBool aCheckTag,
            const TDesC8& aRawData, TInt aMinTerms);

private:
    // New functions

    /**
     * Decode a generic ASN1 sequence. Ignores removed records and
     * padding bytes
     * @since 3.0
     * @param aRawData raw-data to be splitted in ASN.1 Modules
     * @return Array of generic asn.1 modules.
     */
    static CArrayPtrFlat<TASN1DecGeneric>*
    DecodeItemsLC(const TASN1DecGeneric& aRawData);

    static void CleanupSequence(TAny* aArray);
};

} // namespace satsa
} // namespace java
#endif // CSTSASN1SEQUENCE_H
// End of File

