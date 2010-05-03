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


#ifndef CSTSPRINCIPAL_H
#define CSTSPRINCIPAL_H

//  INCLUDES
#include <e32base.h>
#include "mstsasn1module.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Principal type module
 *
 *  ASN.1 Definition:
 *  Principal ::= CHOICE {
 *      rootID [0]PrincipalID,
 *      endEntityID [1]PrincipalID,
 *      domain [2]Domain, -- Operator, manufacturer or Trusted Third Party
 *      ... -- for future extensions
 *  }
 *  PrincipalID ::= OCTET STRING (SIZE(20))
 *  - SHA-1 hash of the domain certificate
 *  Domain ::= OBJECT IDENTIFIER
 *
 */
NONSHARABLE_CLASS(CSTSPrincipal): public CBase, MSTSASN1Module
{
public: // data types
    enum TType
    {
        ERootID = 0,
        EEndEntityID = 1,
        EDomain = 2
    };
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSPrincipal* NewLC();

    /**
     * Destructor.
     */
    virtual ~CSTSPrincipal();

public: // New functions

    /**
     * Getter for principal type
     * @since 3.0
     * @return principal type
     */
    CSTSPrincipal::TType Type() const;

    /**
     * Getter for principal ID
     * @since 3.0
     * @return principal ID in TDesC reference
     */
    const TDesC8& PrincipalID() const;

    /**
     * Getter for domain
     * @since 3.0
     * @return domain in TDesC reference
     */
    const TDesC& Domain() const;

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
    CSTSPrincipal();

protected: // Data
    // type of the choice
    TType iType;
    // rootID or endEntityID depending on type of choice, owned
    HBufC8* iPrincipalID;
    // domain object identifier, owned
    HBufC* iDomain;
};

} // namespace satsa
} // namespace java
#endif // CSTSPRINCIPAL_H
// End of File
