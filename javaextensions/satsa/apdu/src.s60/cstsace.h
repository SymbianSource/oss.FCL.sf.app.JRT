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


#ifndef CSTSACE_H
#define CSTSACE_H

//  INCLUDES
#include "cstsasn1sequence.h"
#include "cstsprincipal.h"
#include "cstsapdumaskpermission.h"
#include "cstsuserauth.h"
#include "stscommon.h"

namespace java
{
namespace satsa
{

class CSTSPrincipal;
class CSTSAPDUMaskPermission;
class CSTSUserAuth;

// CLASS DECLARATION
/**
 *  ACE type module
 *
 *  ACE ::= SEQUENCE {
 *      principals [0] Principals OPTIONAL,
 *      permissions [1] Permissions OPTIONAL,
 *      userAuthentications [2] UserAuthentications OPTIONAL,
 *      ... -- for future extensions
 *  }
 */
NONSHARABLE_CLASS(CSTSAce): public CSTSASN1Sequence
{
public:

    static CSTSAce* NewLC();

    virtual ~CSTSAce();

public:

    /**
     * Getter for Principals
     * @return array of Principals
     */
    const CArrayPtr< CSTSPrincipal>& Principals() const;

    /**
     * Getter for APDUMaskPermissions
     * @return array of APDUMaskPermissions
     */
    const CArrayPtr< CSTSAPDUMaskPermission>& APDUMaskPermissions() const;

    /**
     * Getter for UserAuthentications
     * @return array of UserAuthentications
     */
    const CArrayPtr< CSTSUserAuth>& UserAuthentications() const;

public:

    /**
     * From MSTSASN1Module
     * @param aRawData raw-data to be parsed in this instance
     */
    void DecodeL(const TDesC8& aRawData);

protected:

    void ConstructL();

    CSTSAce();

protected:
    // principals, owned
    CArrayPtr< CSTSPrincipal>* iPrincipals;

    // APDU mask permissions, owned
    CArrayPtr< CSTSAPDUMaskPermission>* iApduMaskPermissions;

    // User authentications, owned
    CArrayPtr< CSTSUserAuth>* iUserAuthentications;

};

} // namespace satsa
} // namespace java
#endif // CSTSACE_H
// End of File
