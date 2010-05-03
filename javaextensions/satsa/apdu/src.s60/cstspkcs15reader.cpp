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

#include "cstspkcs15reader.h"
#include "cstspkcs15objects.h"
#include "cstspath.h"
#include "cstsacie.h"
#include "cstsefacifile.h"
#include "cstsefacfile.h"
#include "cstsfiledatamanager.h"
#include "cstsuserauth.h"
#include "cstsace.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSDefaultGranularity = 2;

CSTSPKCS15Reader::CSTSPKCS15Reader()
{
    iState = ENotOpened;
}

void CSTSPKCS15Reader::ConstructL()
{
    iAcies = new(ELeave) CArrayPtrFlat<CSTSAcie> (KSTSDefaultGranularity);
    iAID = KNullDesC8().AllocL();
    iFileDataManager = CSTSFileDataManager::NewL();
}

CSTSPKCS15Reader* CSTSPKCS15Reader::NewL()
{
    CSTSPKCS15Reader* self = new(ELeave) CSTSPKCS15Reader();

    CleanupStack::PushL(self);
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSPKCS15Reader::~CSTSPKCS15Reader()
{
    Close();
    delete iFileDataManager;
    delete iAID;
    if (iAcies)
    {
        iAcies->ResetAndDestroy();
        delete iAcies;
    }
}

// -----------------------------------------------------------------------------
// CSTSPKCS15Reader::OpenL
// Opens channel to card and tries to select PKCS15 application with AID. If
// directly selection was not allowed, reads EF(DIR) file for finding path to
// PCKS15 application. Reads EF(ODF) and reads path to EF(DODF) and EF(AODF).
// Finally reads EF(TokenInfo) for finding label for token.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPKCS15Reader::OpenL()
{
    iFileDataManager->InitializeL();
    // if no leaves, pkcs15 application is opened succesfully
    iState = EPCKS15Opened;
}

// -----------------------------------------------------------------------------
// CSTSPKCS15Reader::Close
// Close channnel to card
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPKCS15Reader::Close()
{
    iState = EClosed;
}

// -----------------------------------------------------------------------------
// CSTSPKCS15Reader::ReadACFL
// Reads first EF(DODF) for finding path to ACIF. Reads ACIF for finding path
// to Access Control Files (ACF). Reads all Access Control files.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPKCS15Reader::ReadACFL(CArrayPtr<CSTSAce>* aAces)
{

    ReadACIFL();

    TInt foundCount = 0;
    TInt aciesCount = iAcies->Count();

    CSTSEFACFile* acf = CSTSEFACFile::NewLC(iFileDataManager);
    acf->SetArray(aAces);

    // read all Access control files
    for (TInt i = 0; i < aciesCount; i++)
    {
        // select ACF
        acf->SetPath(iAcies->At(i)->ACFPath().Path());
        acf->ReadL(0); // we read all data to previous set array
        foundCount += acf->FoundRecords();
    }
    CleanupStack::PopAndDestroy(acf);

    // compress the size of array
    aAces->Compress();

    // The ACF contains an Access Control List (ACL). The ACL consists of
    // zero or more Access Control Entries(ACE). If the ACL is empty, access
    // is denied for all domains.
    if (foundCount == 0)
    {
        User::Leave(KSTSErrSecurity + KSTSErrSECAccessNotAllowed);
    }
}

// -----------------------------------------------------------------------------
// CSTSPKCS15Reader::ReadAuthObjectsL
// Read the Authentication files to get authId
// (other items were commented in a header).
// -----------------------------------------------------------------------------

void CSTSPKCS15Reader::ReadAuthObjectsL(CArrayPtr<CSTSAuthType>* aAuthTypes,
                                        const CArrayPtr<CSTSAce>& aAces)
{
    // create a list of used authIds in loop
    RArray<TInt> authIdList;

    // go throuhg all aces
    TInt acesCount = aAces.Count();
    for (TInt x = 0; x < acesCount; x++)
    {
        CSTSAce* currentAce = aAces.At(x);

        TInt userAuthsCount = currentAce->UserAuthentications().Count();
        // take authIds which are needed
        for (TInt i = 0; i < userAuthsCount; i++)
        {
            CSTSUserAuth* currentUserAuth =
                currentAce->UserAuthentications().At(i);
            // first element of authId descriptor is the authId
            authIdList.AppendL(currentUserAuth->AuthId()[0]);
        }
    }

    iFileDataManager->GetAuthObjectsL(aAuthTypes, authIdList);

}

// -----------------------------------------------------------------------------
// CSTSPKCS15Reader::SetAIDL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPKCS15Reader::SetAIDL(const TDesC8& aAID)
{
    HBufC8* tmp = aAID.AllocL();
    delete iAID;
    iAID = tmp;
}

// -----------------------------------------------------------------------------
// CSTSPKCS15Reader::ReadACIFL
// Selects Access Control Index File (ACIF). Reads every record of that file
// and finds with AID correct record and saves it to memeber array. Saves that
// kind of records too, where AID is missing, because it is meant for all
// applications.
// If correct records is not found, leaves with KSTSErrSecurity error
// code.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPKCS15Reader::ReadACIFL()
{
    // select EF(DODF)
    CSTSEFACIFile* acif = CSTSEFACIFile::NewLC(iFileDataManager);
    acif->SetAIDL(*iAID);
    acif->SetArray(iAcies);
    acif->ReadL(0); // we read all data

    CleanupStack::PopAndDestroy(acif);

    TInt foundCount = acif->FoundRecords();

    // there can be only 2 of correct Acies, otherwise something is wrong
    if (foundCount > 2)
    {
        User::Leave(KSTSErrConnectionNotFound);
    }

    // compress the size of array
    iAcies->Compress();

    if (foundCount == 0)
    {
        // In the absence of an ACF related to the entire SE access to
        // applications that do not have their respective ACFs, is not
        // allowed.
        User::Leave(KSTSErrSecurity + KSTSErrSECAccessNotAllowed);
    }
}

// -----------------------------------------------------------------------------
// CSTSPKCS15Reader::GetTokenLabel
// Returns token label.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CSTSPKCS15Reader::TokenLabelL()
{
    return iFileDataManager->RetrieveWIMLabelL();
}

} // namespace satsa
} // namespace java
//  End of File

