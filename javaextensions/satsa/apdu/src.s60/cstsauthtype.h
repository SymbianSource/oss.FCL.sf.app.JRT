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


#ifndef CSTSAUTHTYPE_H
#define CSTSAUTHTYPE_H

//  INCLUDES
#include <e32base.h>
#include "cstspinattributes.h"

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSPinAttributes;

// CLASS DECLARATION
/**
 *  AuthenticationType type module
 *
 *  AuthenticationType ::= CHOICE {
 *      pin AuthenticationObject { PinAttributes },
 *      ...,
 *      biometricTemplate [0] AuthenticationObject {BiometricAttributes},
 *      authKey [1] AuthenticationObject {AuthKeyAttributes},
 *      external [2] AuthenticationObject {ExternalAuthObjectAttributes}
 *  }
 *  AuthenticationObject {AuthObjectAttributes} ::= PKCS15Object {
 *      CommonAuthenticationObjectAttributes, NULL, AuthObjectAttributes}
 *
 *  PKCS15Object{ClassAttributes,SubClassAttributes,TypeAttributes}::=SEQUENCE{
 *      commonObjectAttributes  CommonObjectAttributes,
 *      classAttributes ClassAttributes,
 *      subClassAttributes  [0] SubClassAttributes OPTIONAL,
 *      typeAttributes  [1] TypeAttributes
 *  }
 *  CommonObjectAttributes ::= SEQUENCE {
 *      label   Label OPTIONAL,
 *      flags   CommonObjectFlags OPTIONAL,
 *      authId  Identifier OPTIONAL,
 *      ...,
 *      userConsent INTEGER (1..pkcs15-ub-userConsent) OPTIONAL,
 *      accessControlRules SEQUENCE SIZE (1..MAX) OF AccessControlRule OPTIONAL
 *      } (CONSTRAINED BY {-- authId should be present in the IC
 *          card case if flags.private is set.
 *      -- It must equal an authID in one AuthRecord in the AODF -- })
 *
 *  Label ::= UTF8String (SIZE(0..pkcs15-ub-label))
 *
 *  CommonAuthenticationObjectAttributes ::= SEQUENCE {
 *      authId Identifier,
 *  ... -- For future extensions
 *  }
 *  Identifier ::= OCTET STRING (SIZE (0..pkcs15-ub-identifier))
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSAuthType): public CBase
{
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSAuthType* NewLC();

    static CSTSAuthType* NewL();

    /**
     * Destructor.
     */
    virtual ~CSTSAuthType();

public: // New functions

    /**
     * Getter for authentication ID
     * @since 3.0
     * @return authentication ID in TDesC reference
     */
    const TDesC8& AuthID() const;

    /**
     * Setter for authentication ID
     * @since 3.0
     * @param aAuthId authentication ID as a integer value
     */
    void SetAuthIDL(TInt aAuthId);

    /**
     * Getter for label
     * @since 3.0
     * @return label in TDesC reference
     */
    const TDesC& Label() const;

    /**
     * Setter for label
     * @since 3.0
     * @param aLabel Label in TDesC reference
     */
    void SetLabelL(const TDesC& aLabel);

    /**
     * Getter for PIN attributes
     * @since 3.0
     * @return pin attributes
     */
    const CSTSPinAttributes& PinAttributes() const;

    /**
     * Setter for PIN attributes
     * @since 3.0
     * @param pin attributes
     */
    void SetPinAttributesL(const CSTSPinAttributes& aPinAttr);

    /**
     * Corresponds to assigment operator.
     * @since 3.0
     * @param aAuthType reference which data will be used
     */
    void CopyL(const CSTSAuthType& aAuthType);

protected: // New functions

    /**
     * Protected construction to allow derivation
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CSTSAuthType();

protected: // Data
    // authentication id, owned
    HBufC8* iAuthId;
    // label, owned
    HBufC* iLabel;
    //pinAttributes, owned
    CSTSPinAttributes* iPinAttributes;
};

} // namespace satsa
} // namespace java
#endif // CSTSAUTHTYPE_H
// End of File

