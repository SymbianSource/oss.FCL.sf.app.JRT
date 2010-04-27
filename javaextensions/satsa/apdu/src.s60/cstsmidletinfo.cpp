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
#include "cstsmidletinfo.h"

namespace java
{
namespace satsa
{

//CONSTANTS

// Domain category OID value
_LIT(KSTSDomainOperator, "1.3.6.1.4.1.42.2.110.2.2.2.1");
const TInt KSTSDomainOperatorLength = 16;
_LIT(KDom, "OPD");
const TInt KSTSDomNameLength = 6;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSMidletInfo::CSTSMidletInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSMidletInfo::CSTSMidletInfo()
{
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSMidletInfo::ConstructL(const TDesC& aName, const TDesC& aVendor,
                                const TDesC& aVersion, const TDesC& aDomain, const TDesC& aRoot,
                                const TDesC& aEnd)
{

    iName = aName.AllocL();
    iVendor = aVendor.AllocL();
    iVersion = aVersion.AllocL();
    iDomain = aDomain.AllocL();
    iRootCert = aRoot.AllocL();
    iSignCert = aEnd.AllocL();
    iDomainOID = KNullDesC().AllocL();
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSMidletInfo* CSTSMidletInfo::NewLC(const TDesC& aName, const TDesC& aVendor,
                                      const TDesC& aVersion, const TDesC& aDomain, const TDesC& aRoot,
                                      const TDesC& aEnd)
{
    CSTSMidletInfo* self = new(ELeave) CSTSMidletInfo();
    CleanupStack::PushL(self);
    self->ConstructL(aName, aVendor, aVersion, aDomain, aRoot, aEnd);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSMidletInfo* CSTSMidletInfo::NewL(const TDesC& aName, const TDesC& aVendor,
                                     const TDesC& aVersion, const TDesC& aDomain, const TDesC& aRoot,
                                     const TDesC& aEnd)
{
    CSTSMidletInfo* self = CSTSMidletInfo::NewLC(aName, aVendor, aVersion,
                           aDomain, aRoot, aEnd);

    CleanupStack::Pop(self);

    return self;
}

// Destructor
CSTSMidletInfo::~CSTSMidletInfo()
{
    delete iName;
    delete iVendor;
    delete iVersion;
    delete iDomain;
    delete iRootCert;
    delete iSignCert;
    delete iDomainOID;
    iRootIds.ResetAndDestroy();
    iRootIds.Close();
    iEndEntityIds.ResetAndDestroy();
    iEndEntityIds.Close();
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::Name
// Getter for Name
// -----------------------------------------------------------------------------
const TDesC& CSTSMidletInfo::Name() const
{
    return *iName;
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::Vendor
// Getter for Vendor
// -----------------------------------------------------------------------------
const TDesC& CSTSMidletInfo::Vendor() const
{
    return *iVendor;
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::Version
// Getter for Version
// -----------------------------------------------------------------------------
const TDesC& CSTSMidletInfo::Version() const
{
    return *iVersion;
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::Version
// Getter for Version
// -----------------------------------------------------------------------------
const TDesC& CSTSMidletInfo::Domain() const
{
    return *iDomain;
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::DomainOIDL
// Getter for domain OID value
// -----------------------------------------------------------------------------
const TDesC& CSTSMidletInfo::DomainOID()
{

    TBufC16<KSTSDomNameLength> buf1(KDom);
    HBufC* domain = buf1.AllocLC();

    if (domain->Compare(iDomain->Des()))
    {
        TBufC<KSTSDomainOperatorLength> buf2(KSTSDomainOperator);
        HBufC* tmp = buf2.AllocL();
        delete iDomainOID;
        iDomainOID = tmp;
    }
    else
    {
        //at this point this is not needed, because oid value is not
        //specified anywhere
    }

    return *iDomainOID;
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::RootID
// Getter for root ID values (hash of the root certificate)
// -----------------------------------------------------------------------------
const TDesC& CSTSMidletInfo::RootID() const
{
    return *iRootCert;
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::EndEntityID
// Getter for end entity ID value (hash of the end entity certificate)
// -----------------------------------------------------------------------------
const TDesC& CSTSMidletInfo::EndEntityID() const
{
    return *iSignCert;
}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::SetDomainOIDL
//
// -----------------------------------------------------------------------------
void CSTSMidletInfo::SetDomainOIDL(const TDesC& aDomainOID)
{

    HBufC* tmp = aDomainOID.AllocL();
    delete iDomainOID;
    iDomainOID = tmp;

}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::AppendEndEntityIdL
// Appends end entity ID value (hash of the end entity certificate) to array
// -----------------------------------------------------------------------------
void CSTSMidletInfo::AppendEndEntityIdL(const TDesC8& aEndEntityId)
{
    HBufC8* endEntityBuf = aEndEntityId.AllocLC();
    //array takes the ownership
    User::LeaveIfError(iEndEntityIds.Append(endEntityBuf));
    CleanupStack::Pop(endEntityBuf);

}

// -----------------------------------------------------------------------------
// CSTSMidletInfo::AppendRootIdL
// Appends end entity ID value (hash of the end entity certificate) to array
// -----------------------------------------------------------------------------
void CSTSMidletInfo::AppendRootIdL(const TDesC8& aRootId)
{
    HBufC8* rootBuf = aRootId.AllocLC();
    //array takes the ownership
    User::LeaveIfError(iRootIds.Append(rootBuf));
    CleanupStack::Pop(rootBuf);

}

} // namespace satsa
} // namespace java
//  End of File
