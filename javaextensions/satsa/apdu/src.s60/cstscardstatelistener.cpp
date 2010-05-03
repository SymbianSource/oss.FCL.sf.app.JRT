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

#include "cstscardstatelistener.h"
#include "mstscloseobserver.h"

#include    <startupdomainpskeys.h> // Property values

namespace java
{
namespace satsa
{

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSCardStateListener::CSTSCardStateListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSCardStateListener::CSTSCardStateListener(MSTSCloseObserver* aObserver)
{
    iObserver = aObserver;
}

// -----------------------------------------------------------------------------
// CSTSCardStateListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSCardStateListener* CSTSCardStateListener::NewLC(
    MSTSCloseObserver* aObserver)
{
    CSTSCardStateListener* self = new(ELeave) CSTSCardStateListener(aObserver);
    CleanupStack::PushL(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSCardStateListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSCardStateListener* CSTSCardStateListener::NewL(MSTSCloseObserver* aObserver)
{
    CSTSCardStateListener* self = CSTSCardStateListener::NewLC(aObserver);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CSTSCardStateListener::~CSTSCardStateListener()
{
}

// -----------------------------------------------------------------------------
// CSTSEFAODFFile::CardIsAvailable
// This method can be called without to call start method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSCardStateListener::CardIsAvailable()
{

    TBool returnValue = EFalse;
    TInt cardState;

    RProperty::Get(KPSUidStartup, KPSSimStatus, cardState);
    if (cardState == ESimUsable)  // The Sim card is fully usable.

    {
        returnValue = ETrue;
    }
    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSCardStateListener::DoStart
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCardStateListener::DoStart()
{
    iStatus = KRequestPending;

    TInt err = iProperty.Attach(KPSUidStartup, KPSSimStatus);
    if (err == KErrNone)
    {
        iProperty.Subscribe(iStatus);
    }
    SetActive();
}

// -----------------------------------------------------------------------------
// CSTSCardStateListener::RunL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCardStateListener::RunL()
{
    TInt cardState;
    iProperty.Get(cardState);
    switch (cardState)
    {
    case ESimUsable: //Card is usable

    {
        DoStart();
        break;
    }

    // Card is not usable
    case ESimReadable:
    case ESimNotReady:
    case ESimNotPresent:
    case ESimNotSupported:
    case ESimStatusUninitialized:// Some fatal errors

    {
        iObserver->NotifyClose(MSTSCloseObserver::ESTSCardNotAvailable);
        DoStart(); //Start to listen again
        break;
    }
    default:
    {

        break;
    }
    }

}

} // namespace satsa
} // namespace java
//  End of File
