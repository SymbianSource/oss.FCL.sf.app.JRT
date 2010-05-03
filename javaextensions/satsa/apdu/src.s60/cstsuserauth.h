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


#ifndef CSTSUSERAUTH_H
#define CSTSUSERAUTH_H

//  INCLUDES
#include <badesca.h>
#include "cstsasn1sequence.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  UserAuthentication type module
 *
 *  ASN.1 Definition:
 *  UserAuthentication ::= SEQUENCE {
 *      authID OCTET STRING,
 *      userAuthenticationMethod UserAuthenticationMethod,
 *      ... -- for future extensions
 *  }
 *  UserAuthenticationMethod ::= CHOICE {
 *      apduPinEntry [0]APDUPinEntry,
 *      jcrmiPinEntry [1]JCRMIPinEntry,
 *      ... -- for future extensions
 *      }
 *  APDUPinEntry ::= SEQUENCE {
 *      verifyPinAPDU [0]APDUHeader OPTIONAL,
 *      changePinAPDU [1]APDUHeader OPTIONAL,
 *      disablePinAPDU [2]APDUHeader OPTIONAL,
 *      enablePinAPDU [3]APDUHeader OPTIONAL,
 *      unblockPinAPDU [4]APDUHeader OPTIONAL,
 *      ... -- for future extensions
 *  }
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSUserAuth): public CSTSASN1Sequence
{
public: // data types
    enum TType
    {
        EAPDUPinEntry = 0,
        EOther = 1,
        ENotInitialized = 2
    };
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     * NOTE: Encrypt will leave with KErrBadDescriptor if
     * valid authentication ID has not setted.
     */
    static CSTSUserAuth* NewLC();

    virtual ~CSTSUserAuth();

public: // New functions

    /**
     * Getter for authentication method type
     * @return type
     */
    TType Type() const;

    /**
     * Getter for authentication ID
     * @return authentication ID in TDesC reference
     */
    const TDesC8& AuthId() const;

    /**
     * Getter for apdu PIN headers
     * @return apdu PIN headers in CDesC8ArrayFlat reference
     */
    const CDesC8ArrayFlat& ApduPinHeaders() const;

public: // Functions from base classes

    /**
     * From MSTSASN1Module
     * @param aRawData raw-data to be parsed in this instance
     */
    void DecodeL(const TDesC8& aRawData);

protected: // New functions

    void ConstructL();

    CSTSUserAuth();

protected: // Data
    // type of the choice
    TType iType;
    // Authentication ID, owned
    HBufC8* iAuthId;
    // apdu PIN headers, owned
    CDesC8ArrayFlat* iApduPinHeaders;

};

} // namespace satsa
} // namespace java
#endif // CSTSUSERAUTH_H
// End of File
