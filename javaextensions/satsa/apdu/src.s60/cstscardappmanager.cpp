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
#include "cstscardappmanager.h"
#include "cstsselectfile.h"
#include "cstsrespapdu.h"
#include "cstsapduexchanger.h"

namespace java
{
namespace satsa
{

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSCardAppManager::CSTSCardAppManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSCardAppManager::CSTSCardAppManager(CSTSApduExchanger* aApduExchanger)
{
    iApduExchanger = aApduExchanger;
}

// -----------------------------------------------------------------------------
// CSTSCardAppManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSCardAppManager::ConstructL(const TDesC8& aAID)
{
    iSelectFileApdu = CSTSSelectFile::NewL(aAID,
                                           CSTSSelectFile::ESTSAIDActivation, CSTSApdu::ESTSUICC);

    iRespApdu = CSTSRespApdu::NewL(0xFF, CSTSApdu::ESTSUICC); //maximum size
}

// -----------------------------------------------------------------------------
// CSTSCardAppManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSCardAppManager* CSTSCardAppManager::NewL(CSTSApduExchanger* aApduExchanger,
        const TDesC8& aAID)
{
    CSTSCardAppManager* self = new(ELeave) CSTSCardAppManager(aApduExchanger);

    CleanupStack::PushL(self);
    self->ConstructL(aAID);

    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSCardAppManager::~CSTSCardAppManager()
{
    delete iSelectFileApdu;
    delete iRespApdu;
}

// -----------------------------------------------------------------------------
// CSTSCardAppManager::SelectApplication
//
// -----------------------------------------------------------------------------
//
TInt CSTSCardAppManager::SelectApplication()
{
    iSelectFileApdu->SetApduType(CSTSSelectFile::ESTSAIDActivation);
    return DoExchange();
}

// -----------------------------------------------------------------------------
// CSTSCardAppManager::DeselectApplication
//
// -----------------------------------------------------------------------------
//
TInt CSTSCardAppManager::DeselectApplication()
{
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CSTSCardAppManager::DoExchange
//
// -----------------------------------------------------------------------------
//
TInt CSTSCardAppManager::DoExchange()
{
    TInt err = iApduExchanger->ExchangeApdu(*iSelectFileApdu, *iRespApdu);
    if (err == KErrNone)
    {
        if (!iRespApdu->IsNormalEnding())
        {
            err = KErrGeneral;
        }
    }
    return err;
}

} // namespace satsa
} // namespace java
//  End of File

