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


//  INCLUDE FILES
#include "cstsaccesscontrol.h"
#include "cstspkcs15reader.h"
#include "cstsauthtype.h"
#include "cstsace.h"
#include "cstsprincipal.h"
#include "cstsapdumaskpermission.h"
#include "cstsuserauth.h"
#include "cstsmidletinfo.h"
#include "cstspinattributes.h"
#include "logger.h"

namespace java
{
namespace satsa
{

//  CONSTANTS
const TInt KSTSAidFirstBytesLength = 7;

//3GPP standard AIDs
_LIT8(KUiccAidFirstBytes, "\xA0\x00\x00\x00\x87\x10\x01");
_LIT8(KUsimAidFirstBytes, "\xA0\x00\x00\x00\x87\x10\x02");
_LIT8(KUsimToolkitAidFirstBytes, "\xA0\x00\x00\x00\x87\x10\x03");
_LIT8(KIsimAidFirstBytes, "\xA0\x00\x00\x00\x87\x10\x04");
_LIT8(KUsimApiMAidFirstBytes, "\xA0\x00\x00\x00\x87\x10\x05");

//ETSI standard AIDs
_LIT8(KGsmAisFirstBytes, "\xA0\x00\x00\x00\x09\x00\x01");
_LIT8(KGsmSimToolkitAidFirstBytes, "\xA0\x00\x00\x00\x09\x00\x02");
_LIT8(KGsmSimApiAidFirstBytes, "\xA0\x00\x00\x00\x09\x00\x03");
_LIT8(KTetraAidFirstBytes, "\xA0\x00\x00\x00\x09\x00\x04");
_LIT8(KUiccApiAidFirstBytes, "\xA0\x00\x00\x00\x09\x00\x05");

_LIT8(KSTSPkcs15Aid, "\xA0\x00\x00\x00\x63\x50\x4B\x43\x53\x2D\x31\x35");
_LIT8(KSTSWimAid, "\xA0\x00\x00\x00\x63\x57\x41\x50\x2D\x57\x49\x4D");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSAccessControl::CSTSAccessControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSAccessControl::CSTSAccessControl(CSTSMidletInfo* aMidletInfo)
{
    iMidletInfo = aMidletInfo;
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSAccessControl::ConstructL()
{
    iPKCS15Reader = CSTSPKCS15Reader::NewL();
    iAuthType = CSTSAuthType::NewL();

    iAces = new(ELeave) CArrayPtrFlat<CSTSAce> (1);  //must be atleast one
    iAuthTypes = new(ELeave) CArrayPtrFlat<CSTSAuthType> (1);  //must be atleast one
    iApduHeader = HBufC8::NewL(KSTSApduMandatoryHeaderLen);

}

// -----------------------------------------------------------------------------
// CSTSAccessControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSAccessControl* CSTSAccessControl::NewL(CSTSMidletInfo* aMidletInfo)
{
    CSTSAccessControl* self = new(ELeave) CSTSAccessControl(aMidletInfo);

    CleanupStack::PushL(self);
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSAccessControl::~CSTSAccessControl()
{
    //for safety, close is also called here
    if (iPKCS15Reader)
    {
        iPKCS15Reader->Close();
    }
    delete iApduHeader;

    if (iAuthTypes)
    {
        iAuthTypes->ResetAndDestroy();
        delete iAuthTypes;
    }

    if (iAces)
    {
        iAces->ResetAndDestroy();
        delete iAces;
    }
    delete iAuthType;
    delete iPKCS15Reader;
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::ReadFilesL
// Fills midlet related info. Reads all Access Control related information from
// the card. Opens connection to card, reads files and closes the connection.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSAccessControl::ReadFilesL()
{
    iPKCS15Reader->OpenL();
    iPKCS15Reader->ReadACFL(iAces);
    iPKCS15Reader->ReadAuthObjectsL(iAuthTypes, *iAces);
    iPKCS15Reader->Close();

}

// -----------------------------------------------------------------------------
// CSTSAccessControl::IsAllowedDomain
// Domain checking is done acoording to document Security and Trust Services
// API (SATSA), Appendix A Recommended Security Element Access Control,
// chapter A.4.2.3.1.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSTSAccessControl::IsAllowedDomain()
{
    TBool returnValue = EFalse;
    TInt acesCount = iAces->Count();

    for (TInt i = 0; i < acesCount && !returnValue; i++)
    {
        CSTSAce* ace = iAces->At(i);
        returnValue = IsProperAce(ace);
    }

    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::IsAllowedApduL
// APDU checking is done acoording to document Security and Trust Services
// API (SATSA), Appendix A Recommended Security Element Access Control,
// chapter A.4.2.3.1.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSTSAccessControl::IsAllowedApduL(const TDesC8& aMidletApduHeader)
{
    TBool returnValue = EFalse;
    TInt acesCount = iAces->Count();
    for (TInt i = 0; i < acesCount && !returnValue; i++)
    {
        CSTSAce* ace = iAces->At(i);

        if (IsProperAce(ace))
        {
            TInt permissionCount = ace->APDUMaskPermissions().Count();
            for (TInt x = 0; x < permissionCount && !returnValue; x++)
            {
                CSTSAPDUMaskPermission* permission =
                    ace->APDUMaskPermissions().At(x);

                TPtrC8 apduMaskPtr(permission->ApduMask());
                TPtrC8 apduHeaderPtr(permission->ApduHeader());

                //If the bitwise AND between the MIDlet APDU header and the
                //ACE mask equals to the ACE APDU header, return true.
                HBufC8* resultBuf = HBufC8::NewLC(KSTSApduMandatoryHeaderLen);
                TPtr8 resultPtr(resultBuf->Des());
                resultPtr.FillZ(KSTSApduMandatoryHeaderLen);

                //first we do bitwise AND for descriptors and save the result
                for (TInt y = 0; y < KSTSApduMandatoryHeaderLen; y++)
                {
                    TUint8 claMidlet = aMidletApduHeader[y];
                    TUint8 mask1 = apduMaskPtr[y];
                    TUint8 result = (TUint8)(claMidlet & mask1);
                    resultPtr[y] = result;
                }

                //then we compare result to apdu header found from ACE
                if (resultPtr == apduHeaderPtr)
                {
                    returnValue = ETrue;
                }
                CleanupStack::PopAndDestroy(resultBuf);
            }
            //If there is no permission, all operations are allowed
            if (permissionCount == 0)
            {
                returnValue = ETrue;
            }
        }
    }

    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::IsAllowedPinMethod
// Checks whether there are appropriate permissions to execute pin methods
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSTSAccessControl::IsAllowedPinMethod()
{
    LOG(ESATSA, EInfo, "+ CSTSAccessControl::IsAllowedPinMethod");
    TInt allUserAuthentications = 0;
    TInt acesCount = iAces->Count();
    for (TInt i = 0; i < acesCount; i++)
    {
        CSTSAce* ace = iAces->At(i);
        if (IsProperAce(ace))
        {
            TInt count = ace->UserAuthentications().Count();
            allUserAuthentications += count;
        }
    }
    //Missing userAuthentications field indicates that PIN-related operations
    //are not allowed
    LOG1(ESATSA, EInfo, "-- CSTSAccessControl::IsAllowedPinMethod with %d",
         allUserAuthentications);
    return allUserAuthentications != 0;
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::GetPinHeaderL
// Returns empty descriptor, if amount of Aces is zero.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CSTSAccessControl::GetPinHeaderL(TInt aPinID,
        TPinApduType aPinApduType)
{
    //empty pointer descriptor, which represents no data and it's lenght is 0
    TPtrC8 returnValue;

    FindAuthTypeL(aPinID);

    TInt acesCount = iAces->Count();

    for (TInt i = 0; i < acesCount; i++)
    {
        CSTSAce* ace = iAces->At(i);
        if (IsProperAce(ace))
        {
            TInt count = ace->UserAuthentications().Count();
            for (TInt x = 0; x < count; x++)
            {
                CSTSUserAuth* userAuth = ace->UserAuthentications().At(x);
                if ((userAuth->Type() == CSTSUserAuth::EAPDUPinEntry)
                        && (userAuth->AuthId() == iAuthType->AuthID())
                        && (userAuth->ApduPinHeaders().Count() > aPinApduType))
                {
                    returnValue.Set(userAuth->ApduPinHeaders()[aPinApduType]);
                }
                //if pin header does not exist, we return empty descriptor
            }
        }
    }

    //copies header to the member buffer and replaces old data
    *iApduHeader = returnValue;
    return *iApduHeader;
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::GetPinAttributesL
// Saves found authType to member variable, which will be returned
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CSTSAuthType& CSTSAccessControl::GetPinAttributesL(TInt aPinID)
{
    FindAuthTypeL(aPinID);
    return *iAuthType;
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::TokenLabelL
// Returns the PKCS15 token label
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CSTSAccessControl::TokenLabelL()
{
    return iPKCS15Reader->TokenLabelL();
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::SetAIDL
// Used standard "ETSI TS 101 220 V7.0.0 (2004-12), Smart Cards;ETSI numbering
// system for telecommunication application providers (Release 7)" for AIDs.
// AIDs are specified to start as following: 3GPP RID 'A000000087', ETSI RID
// 'A000000009'. After that is application code depending on application.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSAccessControl::SetAIDL(const TDesC8& aAID)
{
    //check is aid restricted and leave if it is
    TPtrC8 aidFirstBytes(aAID.Ptr(), KSTSAidFirstBytesLength);
    if ((aidFirstBytes == KUiccAidFirstBytes()) || (aidFirstBytes
            == KUsimAidFirstBytes()) || (aidFirstBytes
                                         == KUsimToolkitAidFirstBytes()) || (aidFirstBytes
                                                                             == KIsimAidFirstBytes()) || (aidFirstBytes
                                                                                                          == KUsimApiMAidFirstBytes()) || (aidFirstBytes
                                                                                                                                           == KGsmAisFirstBytes()) || (aidFirstBytes
                                                                                                                                                                       == KGsmSimToolkitAidFirstBytes()) || (aidFirstBytes
                                                                                                                                                                                                             == KGsmSimApiAidFirstBytes()) || (aidFirstBytes
                                                                                                                                                                                                                                               == KTetraAidFirstBytes()) || (aidFirstBytes
                                                                                                                                                                                                                                                                             == KUiccApiAidFirstBytes()) || (aAID == KSTSPkcs15Aid()) || (aAID
                                                                                                                                                                                                                                                                                     == KSTSWimAid()))
    {
        User::Leave(KSTSErrSecurity + KSTSErrSECAccessNotAllowed);
    }
    iPKCS15Reader->SetAIDL(aAID);
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::FindAuthTypeL
// Saves found authType to member variable, which will be returned
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSAccessControl::FindAuthTypeL(TInt aPinID)
{

    CSTSAuthType* authType = NULL;
    TPtrC8 authIdPtr;

    //go through all authTypes in loop and find correct authId
    TInt authTypesCount = iAuthTypes->Count();
    TBool authTypeNotFound = ETrue;
    for (TInt x = 0; x < authTypesCount && authTypeNotFound; x++)
    {

        //points to array's object
        authType = iAuthTypes->At(x);

        //get CommonAuthenticationObjectAttributes.authID
        authIdPtr.Set(authType->AuthID());
        if (authIdPtr.Length() != 0)
        {

            if (authIdPtr[0] == aPinID) //first value can be compared to PinID
            {
                authTypeNotFound = EFalse;

                CSTSAuthType* tmp = CSTSAuthType::NewLC();
                tmp->CopyL(*authType);

                CleanupStack::Pop(tmp);
                delete iAuthType;
                iAuthType = tmp;
            }
        }
    }
    if (authTypeNotFound)
    {
        User::Leave(KSTSErrSecurity);
    }
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::IsProperAce
// Checks that is Ace entry meant for current domain, rootId or EndEntityId.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSTSAccessControl::IsProperAce(const CSTSAce* aAce) const
{
    TBool returnValue = EFalse;
    TInt principalsCount = aAce->Principals().Count();
    for (TInt x = 0; x < principalsCount && !returnValue; x++)
    {
        CSTSPrincipal* principal = aAce->Principals().At(x);
        CSTSPrincipal::TType type = principal->Type();

        if (type == CSTSPrincipal::EDomain)
        {
            //if OID is indicating ’operator’, ’manufacturer’, or
            //’trusted third party’ and the application belongs to
            //the same domain category
            if (principal->Domain() == iMidletInfo->DomainOID())
            {
                returnValue = ETrue;
            }
        }
        else if (type == CSTSPrincipal::ERootID)
        {
            //if principalID matches with the hash of the root
            //certificate in the path used to sign the J2ME application

            const TDesC& rootId = iMidletInfo->RootID();
            if (rootId == (TDesC&) principal->PrincipalID())
            {
                returnValue = ETrue;
            }

        }
        else if (type == CSTSPrincipal::EEndEntityID)
        {
            //if principalID matches with the end-entity certificate
            //used to sign the J2ME application
            const TDesC& endEntityId = iMidletInfo->EndEntityID();
            if (endEntityId == (TDesC&) principal->PrincipalID())
            {
                returnValue = ETrue;
            }

        }
        else
        {

        }
    }

    //A missing principals field indicates that the ACE applies to all
    //identified parties.
    if (principalsCount == 0)
    {
        returnValue = ETrue;
    }

    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::Close
// Closes pkcs15 reader
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSAccessControl::Close()
{

    if (iPKCS15Reader)
    {
        iPKCS15Reader->Close();
    }
}

// -----------------------------------------------------------------------------
// CSTSAccessControl::CheckPinFlagsL
// Checks from PIN flags is change disabled, unblock disabled or disable allowed.
// Leaves for example if PIN apdu type is Change Pin and change is disabled with
// security leave.
// NOTE: GetPinHeaderL or GetPinAttributesL must be called before this method
// because iAuthType is set there.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSAccessControl::CheckPinFlagsL(TPinApduType aPinApduType)
{
    LOG(ESATSA, EInfo, "CSTSAccessControl::CheckPinFlagsL ++");
    if (aPinApduType == EChangePinAPDU)
    {
        // change is disabled
        if (iAuthType->PinAttributes().IsPinFlagSet(
                    CSTSPinAttributes::EChangeDisabled))
        {
            LOG(ESATSA, EInfo, "Change is disabled");
            User::Leave(KSTSErrSecurity + KSTSErrSECNoRightsPin);
        }
    }
    else if (aPinApduType == EUnblockPinAPDU)
    {
        // unblock is disabled
        if (iAuthType->PinAttributes().IsPinFlagSet(
                    CSTSPinAttributes::EUnblockDisabled))
        {
            LOG(ESATSA, EInfo, "unblock is disabled");
            User::Leave(KSTSErrSecurity + KSTSErrSECNoRightsPin);
        }
    }
    else if (aPinApduType == EDisablePinAPDU)
    {
        // disable is not allowed
        if (!iAuthType->PinAttributes().IsPinFlagSet(
                    CSTSPinAttributes::EDisableAllowed))
        {
            LOG(ESATSA, EInfo, "disable is not allowed");
            User::Leave(KSTSErrSecurity + KSTSErrSECNoRightsPin);
        }
    }
    //else no actions
}

} // namespace satsa
} // namespace java
//  End of File

