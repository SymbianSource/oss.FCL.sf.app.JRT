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


#ifndef CSTSMIDLETINFO_H
#define CSTSMIDLETINFO_H

//  INCLUDES
#include "cstsasn1sequence.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Capsulated current midlet related info for native side needs.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSMidletInfo): public CBase
{
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */

    static CSTSMidletInfo* NewLC(const TDesC& aName,
    const TDesC& aVendor,
    const TDesC& aVersion,
    const TDesC& aDomain,
    const TDesC& aRoot,
    const TDesC& aEnd);

    static CSTSMidletInfo* NewL(const TDesC& aName,
                                const TDesC& aVendor,
                                const TDesC& aVersion,
                                const TDesC& aDomain,
                                const TDesC& aRoot,
                                const TDesC& aEnd);

    /**
     * Destructor.
     */
    virtual ~CSTSMidletInfo();

public: // New functions

    /**
     * Getter for midlet name
     * @since 3.0
     * @return Name in TDesC reference
     */
    const TDesC& Name() const;

    /**
     * Getter for midlet vendor
     * @since 3.0
     * @return Vendor in TDesC reference
     */
    const TDesC& Vendor() const;

    /**
     * Getter for midlet version
     * @since 3.0
     * @return Version in TDesC reference
     */
    const TDesC& Version() const;

    /**
     * Getter for midlet domain
     * @since 3.0
     * @return Domain in TDesC reference
     */
    const TDesC& Domain() const;
    /**
     * Getter for midlet domain OID value
     * @since 3.0
     * @return Domain OID in TDesC reference
     */
    const TDesC& DomainOID();

    /**
     * Getter for midlet root ID value
     * @since 3.0
     * @return Root ID in TDesC reference
     */
    //const RPointerArray< HBufC8 >& RootID() const;
    const TDesC& RootID() const;
    /**
     * Getter for midlet end entity ID value
     * @since 3.0
     * @return End entity ID in TDesC reference
     */
    //const RPointerArray< HBufC8 >& EndEntityID() const;
    const TDesC& EndEntityID() const;
    /**
     * Sets Domain OID value
     * @since 3.0
     * @param aDomainOID Domain OID value
     */
    void SetDomainOIDL(const TDesC& aDomainOID);

    /**
     * Appends midlet end entity ID value to member array.
     * Ownership transferred.
     * @since 3.0
     * @param aEndEntityId Pointer to End entity Id value
     */
    void AppendEndEntityIdL(const TDesC8& aEndEntityId);

    /**
     * Appends midlet root ID value to member array.
     * Ownership transferred.
     * @since 3.0
     * @param aRootId Pointer to Root Id value
     */
    void AppendRootIdL(const TDesC8& aRootId);

protected: // New functions

    /**
     * Protected construction to allow derivation
     */
    void ConstructL(const TDesC& aName,
                    const TDesC& aVendor,
                    const TDesC& aVersion,
                    const TDesC& aDomain,
                    const TDesC& aRoot,
                    const TDesC& aEnd);

    /**
     * C++ default constructor.
     */
    CSTSMidletInfo();

protected: // Data

    // Midlet name, owned
    HBufC* iName;
    // Midlet vendor, owned
    HBufC* iVendor;
    // Midlet version, owned
    HBufC* iVersion;
    // Midlet domain, owned
    HBufC* iDomain;
    // Midlet root cert hash, owned
    HBufC* iRootCert;
    // Midlet end-entity cert hash, owned
    HBufC* iSignCert;

    // Midlet domain OID value
    HBufC* iDomainOID; //owned
    // Array of Midlet root ID values
    RPointerArray< HBufC8> iRootIds;
    // Array of Midlet end entity ID values
    RPointerArray< HBufC8> iEndEntityIds;

};

} // namespace satsa
} // namespace java
#endif // CSTSMIDLETINFO_H
// End of File
