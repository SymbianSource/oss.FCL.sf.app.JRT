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


#ifndef CSTSPKCS15OBJECTS_H
#define CSTSPKCS15OBJECTS_H

//  INCLUDES
#include <e32base.h>
#include "mstsasn1module.h"

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSPath;

// CLASS DECLARATION
/**
 *  DataType type module
 *
 *  ASN.1 Definition:
 *  PKCS15Objects ::= CHOICE {
 *      privateKeys         [0] PrivateKeys,
 *      publicKeys          [1] PublicKeys,
 *      trustedPublicKeys   [2] PublicKeys,
 *      secretKeys          [3] SecretKeys,
 *      certificates        [4] Certificates,
 *      trustedCertificates [5] Certificates,
 *      usefulCertificates  [6] Certificates,
 *      dataObjects         [7] DataObjects,
 *      authObjects         [8] AuthObjects,
 *      ... -- For future extensions
 *  }
 *  PrivateKeys ::=     PathOrObjects {PrivateKeyType}
 *  SecretKeys  ::=     PathOrObjects {SecretKeyType}
 *  PublicKeys  ::=     PathOrObjects {PublicKeyType}
 *  Certificates ::=    PathOrObjects {CertificateType}
 *  DataObjects ::=     PathOrObjects {DataType}
 *  AuthObjects ::=     PathOrObjects {AuthenticationType}
 *
 *  PathOrObjects {ObjectType} ::= CHOICE {
 *      path        Path,
 *      objects     [0] SEQUENCE OF ObjectType,
 *      ...,
 *  indirect-protected [1] ReferencedValue {EnvelopedData {SEQUENCE OF      ObjectType}},
 *  direct-protected [2] EnvelopedData {SEQUENCE OF ObjectType}
 *  }
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPKCS15Objects): public CBase, MSTSASN1Module
{
public: // data types
    enum TType
    {
        EDataObjectsPath = 7,
        EAuthObjectsPath = 8,
        EOther = 9,
        ENotInitialized = 10
    };
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSPKCS15Objects* NewLC();

    /**
     * Destructor.
     */
    virtual ~CSTSPKCS15Objects();

public: // New functions

    /**
     * Getter for data object type
     * @since 3.0
     * @return data object type
     */
    CSTSPKCS15Objects::TType Type() const;

    /**
     * Getter for Path
     * @since 3.0
     * @return Path
     */
    const CSTSPath& Path() const;

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
    CSTSPKCS15Objects();

protected: // Data
    //type of the choice
    TType iType;
    // Path, owned
    CSTSPath* iPath;
};

} //namespace satsa
} //namespace java
#endif // CSTSPKCS15OBJECTS_H
// End of File
