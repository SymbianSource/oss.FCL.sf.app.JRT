/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@publishedPartner
@released
*/

#ifndef MMIDLETSUITEINFO_H
#define MMIDLETSUITEINFO_H

#include <e32base.h>

namespace MIDP
{

/**
 * Interface class to provide API for maintaining information about a MIDlet Suite.
 */
class MMIDletSuiteInfo
{
public:
    /** Enumeration used to refer to a particular property. */
    enum TPropertyId
    {
        EHomeDir = 1,
        EProtectionDomain
    };

public:

    /** Destructor */
    virtual ~MMIDletSuiteInfo() = 0;

    /**
     * Return the suite's id.
     * @return  The integer id.
     */
    virtual TUint32 Id(void) const = 0;

    /**
     * Return the suite's name information.
     * @return  Descriptor reference containing the name information.
     */
    virtual const TDesC& Name(void) const = 0;

    /**
     * Return the suite's vendor information.
     * @return  Descriptor reference containing the vendor information.
     */
    virtual const TDesC& Vendor(void) const = 0;

    /**
     * Return the suite's signer information.
     * @return  Descriptor reference containing the signer information.
     */
    virtual const TDesC& Signer(void) const = 0;

    /**
     * Return the path to the jar file
     * @return  Descriptor reference containing the jar path
     */
    virtual const TDesC& JarPath(void) const = 0;

    /**
      * Return the "country" info contained in the suite's signer cert
      * (C attribute of Subject field of certificate)
      * @return  Pointer to descriptor containing the "country",
      *          NULL if there is no "country" information in certificate.
      * Ownership is retained (not transferred to caller)
      */
    virtual const HBufC* SignerCertCountry(void) const = 0;

    /**
      * Return the "organisation" info contained in the suite's signer cert
      * (O attribute of Subject field of certificate)
      * @return  Pointer to descriptor containing the "organisation".
      *          NULL if there is no "organisation" information in certificate.
      * Ownership is retained (not transferred to caller)
      */
    virtual const HBufC* SignerCertOrg(void) const = 0;

    /**
     * Return the common name contained in the suite's signer cert
     * (CN attribute of Subject field)
     * @return  Pointer to descriptor containing the common name.
     *          NULL if there is no "common name" information in certificate.
     * Ownership is retained (not transferred to caller)
     */
    virtual const HBufC* SignerCertCN(void) const = 0;

    /**
     * Return whether the midlet is signed or not
     * @return  whether the midlet is signed or not
     */
    virtual TBool IsSigned(void) const = 0;

    /**
     * Retrieve the property specified by aId.
     * @see TPropertyId
     * @param   aId
     *          Used to specify the property to retrieve.
     * @param   aValue
     *          Descriptor to receive the property value.
     * @return  KErrNone if successful, else a system wide error code.
     */
    virtual TInt PropertyL(TInt aId, TPtrC& aValue) const = 0;
};

inline MMIDletSuiteInfo::~MMIDletSuiteInfo()
{
}

} // end of namespace MIDP

#endif // MMIDLETSUITEINFO_H
