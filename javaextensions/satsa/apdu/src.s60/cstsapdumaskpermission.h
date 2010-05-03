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


#ifndef CSTSAPDUMASKPERMISSION_H
#define CSTSAPDUMASKPERMISSION_H

//  INCLUDES
#include "cstsasn1sequence.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  APDUMaskPermission type module
 *
 *  APDUMaskPermission ::= SEQUENCE {
 *      apduHeader APDUHeader,
 *      mask APDUMask
 *  }
 *  APDUHeader ::= OCTET STRING (SIZE(4))
 *  APDUMask ::= OCTET STRING (SIZE(4))
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSAPDUMaskPermission): public CSTSASN1Sequence
{
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSAPDUMaskPermission* NewLC();

    /**
     * Destructor.
     */
    virtual ~CSTSAPDUMaskPermission();

public: // New functions

    /**
     * Getter for apdu mask
     * @since 3.0
     * @return apdu mask in TDesC reference
     */
    const TDesC8& ApduMask() const;

    /**
     * Getter for apdu header
     * @since 3.0
     * @return apdu header in TDesC reference
     */
    const TDesC8& ApduHeader() const;

public: // Functions from base classes

    /**
     * From MSTSASN1Module
     * @since 3.0
     * @param aRawData raw-data to be parsed in this instance
     */
    void DecodeL(const TDesC8& aRawData);

protected: // New functions

    /**
     * Protected construction to allow derivation
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CSTSAPDUMaskPermission();

protected: // Data
    // ApduMask, owned
    HBufC8* iApduMask;
    // ApduHeader, owned
    HBufC8* iApduHeader;

};

} // namespace satsa
} // namespace java
#endif // CSTSAPDUMASKPERMISSION_H
// End of File

