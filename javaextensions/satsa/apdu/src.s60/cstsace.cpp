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


// INCLUDE FILES
#include <asn1dec.h>

#include "cstsace.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TTagType KSTSPrincipalsTag = 0;
const TTagType KSTSPermissionsTag = 1;
const TTagType KSTSUserAuthenticationsTag = 2;
const TTagType KSTSApduMaskPermissionTag = 0;

const TInt KSTSDefaultGranularity = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSAce::CSTSAce
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSAce::CSTSAce()
{
}

// -----------------------------------------------------------------------------
// CSTSAce::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSAce::ConstructL()
{
    // creating with empty values
    iPrincipals
    = new(ELeave) CArrayPtrFlat<CSTSPrincipal> (KSTSDefaultGranularity);

    iApduMaskPermissions
    = new(ELeave) CArrayPtrFlat<CSTSAPDUMaskPermission> (KSTSDefaultGranularity);

    iUserAuthentications
    = new(ELeave) CArrayPtrFlat<CSTSUserAuth> (KSTSDefaultGranularity);
}

// -----------------------------------------------------------------------------
// CSTSAce::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSAce* CSTSAce::NewLC()
{
    CSTSAce* self = new(ELeave) CSTSAce();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// Destructor
CSTSAce::~CSTSAce()
{
    if (iPrincipals)
    {
        iPrincipals->ResetAndDestroy();
        delete iPrincipals;
    }
    if (iApduMaskPermissions)
    {
        iApduMaskPermissions->ResetAndDestroy();
        delete iApduMaskPermissions;
    }
    if (iUserAuthentications)
    {
        iUserAuthentications->ResetAndDestroy();
        delete iUserAuthentications;
    }
}

// -----------------------------------------------------------------------------
// CSTSAce::DecodeL
// Decrypts raw data to this instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSAce::DecodeL(const TDesC8& aRawData)
{
    CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC(ETrue, aRawData); //must be sequence
    TInt pos = 0;
    //decode possible sequence of principal (in loop)
    if ((pos < itemsData->Count()) && (itemsData->At(pos)->Tag()
                                       == KSTSPrincipalsTag))
    {
        //no need to check sequence tag
        CArrayPtr<TASN1DecGeneric>* principals = DecodeSequenceLC(EFalse,
                itemsData->At(pos++)->Encoding());

        TInt numOfPrincipals = principals->Count();

        if (numOfPrincipals)
        {
            CArrayPtr<CSTSPrincipal>* princArray = new(ELeave) CArrayPtrFlat<
            CSTSPrincipal> (numOfPrincipals);
            CleanupStack::PushL(princArray);
            //if leave occures, array's ResetAndDestroy method will be called
            CleanupResetAndDestroyPushL(*princArray);

            for (TInt i = 0; i < numOfPrincipals; i++)
            {
                CSTSPrincipal* principal = CSTSPrincipal::NewLC();
                principal->DecodeL(principals->At(i)->Encoding());
                princArray->AppendL(principal);
                CleanupStack::Pop(principal);
            }

            if (iPrincipals)
            {
                iPrincipals->ResetAndDestroy();
            }
            delete iPrincipals;
            iPrincipals = princArray;

            CleanupStack::Pop(princArray);//ResetAndDestroy
            CleanupStack::Pop(princArray);
        }
        CleanupStack::PopAndDestroy(principals);
    }

    //decode possible sequence of permission (in loop)
    if ((pos < itemsData->Count()) && (itemsData->At(pos)->Tag()
                                       == KSTSPermissionsTag))
    {
        //no need to check sequence tag
        CArrayPtr<TASN1DecGeneric>* permissions = DecodeSequenceLC(EFalse,
                itemsData->At(pos++)->Encoding());

        TInt numOfPermissions = permissions->Count();
        if (numOfPermissions)
        {
            CArrayPtr<CSTSAPDUMaskPermission>
            * permissionArray = new(ELeave) CArrayPtrFlat<
            CSTSAPDUMaskPermission> (numOfPermissions);
            CleanupStack::PushL(permissionArray);
            //if leave occures, array's ResetAndDestroy method will be called
            CleanupResetAndDestroyPushL(*permissionArray);

            for (TInt i = 0; i < numOfPermissions; i++)
            {
                //we need only APDUMAskPermissions
                if (permissions->At(i)->Tag() == KSTSApduMaskPermissionTag)
                {
                    CSTSAPDUMaskPermission* permission =
                        CSTSAPDUMaskPermission::NewLC();
                    permission->DecodeL(permissions->At(i)->Encoding());
                    permissionArray->AppendL(permission);
                    CleanupStack::Pop(permission);
                }
            }

            if (iApduMaskPermissions)
            {
                iApduMaskPermissions->ResetAndDestroy();
            }
            delete iApduMaskPermissions;
            iApduMaskPermissions = permissionArray;

            CleanupStack::Pop(permissionArray); //ResetAndDestroy
            CleanupStack::Pop(permissionArray);
        }
        CleanupStack::PopAndDestroy(permissions);
    }

    //decode possible sequence of UserAuthentication (in loop)
    if ((pos < itemsData->Count()) && (itemsData->At(pos)->Tag()
                                       == KSTSUserAuthenticationsTag))
    {
        //no need to check sequence tag
        CArrayPtr<TASN1DecGeneric>* userAuthentications = DecodeSequenceLC(
                    EFalse, itemsData->At(pos++)->Encoding());

        TInt numOfUserAuthentications = userAuthentications->Count();
        if (numOfUserAuthentications)
        {

            CArrayPtr<CSTSUserAuth>
            * userAuthArray =
                new(ELeave) CArrayPtrFlat<CSTSUserAuth> (numOfUserAuthentications);
            CleanupStack::PushL(userAuthArray);
            //if leave occures, array's ResetAndDestroy method will be called
            CleanupResetAndDestroyPushL(*userAuthArray);

            for (TInt i = 0; i < numOfUserAuthentications; i++)
            {
                CSTSUserAuth* userAuthentication = CSTSUserAuth::NewLC();
                userAuthentication->DecodeL(
                    userAuthentications->At(i)->Encoding());
                //we need only EAPDUPinEntry
                if (userAuthentication->Type() == CSTSUserAuth::EAPDUPinEntry)
                {
                    userAuthArray->AppendL(userAuthentication);
                }
                CleanupStack::Pop(userAuthentication);
            }

            if (iUserAuthentications)
            {
                iUserAuthentications->ResetAndDestroy();
            }
            delete iUserAuthentications;
            iUserAuthentications = userAuthArray;
            CleanupStack::Pop(userAuthArray);//ResetAndDestroy
            CleanupStack::Pop(userAuthArray);
        }
        CleanupStack::PopAndDestroy(userAuthentications);
    }
    CleanupStack::PopAndDestroy(itemsData);
}

// -----------------------------------------------------------------------------
// CSTSAce::Principals
// Getter for principals
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const CArrayPtr<CSTSPrincipal>& CSTSAce::Principals() const
{
    return *iPrincipals;
}

// -----------------------------------------------------------------------------
// CSTSAce::APDUMaskPermissions
// Getter for APDUMaskPermissions
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const CArrayPtr<CSTSAPDUMaskPermission>& CSTSAce::APDUMaskPermissions() const
{
    return *iApduMaskPermissions;
}

// -----------------------------------------------------------------------------
// CSTSAce::UserAuthentications
// Getter for UserAuthentications
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const CArrayPtr<CSTSUserAuth>& CSTSAce::UserAuthentications() const
{
    return *iUserAuthentications;
}

} // namespace satsa
} // namespace java
//  End of File
