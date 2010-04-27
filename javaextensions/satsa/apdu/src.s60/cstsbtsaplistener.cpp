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
#include "cstsbtsaplistener.h"
#include "mstscloseobserver.h"
#include <BTSapDomainPSKeys.h> // Property values
namespace java
{
namespace satsa
{

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSBTSapListener::CSTSBTSapListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSBTSapListener::CSTSBTSapListener(MSTSCloseObserver* aObserver)
{
    iObserver = aObserver;
}

// -----------------------------------------------------------------------------
// CSTSBTSapListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSBTSapListener* CSTSBTSapListener::NewLC(MSTSCloseObserver* aObserver)
{
    CSTSBTSapListener* self = new(ELeave) CSTSBTSapListener(aObserver);
    CleanupStack::PushL(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSBTSapListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSBTSapListener* CSTSBTSapListener::NewL(MSTSCloseObserver* aObserver)
{
    CSTSBTSapListener* self = CSTSBTSapListener::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSBTSapListener::~CSTSBTSapListener()
{

}

// -----------------------------------------------------------------------------
// CSTSEFAODFFile::IsInUse
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSBTSapListener::IsInUse()
{
    TBool returnValue = EFalse;
    TInt btSapState;
    RProperty::Get(KPSUidBluetoothSapConnectionState, KBTSapConnectionState,
                   btSapState);

    if (btSapState == EBTSapConnected)
    {
        returnValue = ETrue;
    }
    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSBTSapListener::DoStart
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSBTSapListener::DoStart()
{
    iStatus = KRequestPending;
    TInt err = iProperty.Attach(KPSUidBluetoothSapConnectionState,
                                KBTSapConnectionState);
    if (err == KErrNone)
    {
        iProperty.Subscribe(iStatus);
        SetActive();
    }
}

// -----------------------------------------------------------------------------
// CSTSBTSapListener::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSBTSapListener::RunL()
{
    TInt btSapState;
    iProperty.Get(btSapState);
    switch (btSapState)
    {
    case EBTSapNotConnected:
    case EBTSapConnecting: // BT Sap not connected, no actions
    {
        DoStart();
        break;
    }
    case EBTSapConnected:
    {
        //close the connection
        iObserver->NotifyClose(MSTSCloseObserver::ESTSBTSapInUse);
        break;
    }
    default:
    {
        //no actions needed
        break;
    }
    }
}

} // namespace satsa
} // namespace java
//  End of File
