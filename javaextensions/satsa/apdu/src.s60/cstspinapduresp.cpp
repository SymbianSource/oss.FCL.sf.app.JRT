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
#include  "cstspinapduresp.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TUint8 KSTSSW1RetriesLeft = 0x63;
const TUint8 KSTSSW1PinBlocked = 0x69;
const TUint8 KSTSSW2PinBlocked = 0x83;

CSTSPinApduResp::CSTSPinApduResp()
{

}

CSTSPinApduResp* CSTSPinApduResp::NewLC(TInt aMaxLength,
                                        CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSPinApduResp* self = new(ELeave) CSTSPinApduResp();
    CleanupStack::PushL(self);
    self->ConstructL(aMaxLength, aStandard);
    return self;
}

CSTSPinApduResp* CSTSPinApduResp::NewL(TInt aMaxLength,
                                       CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSPinApduResp* self = CSTSPinApduResp::NewLC(aMaxLength, aStandard);
    CleanupStack::Pop(self);

    return self;
}

CSTSPinApduResp::~CSTSPinApduResp()
{

}

// -----------------------------------------------------------------------------
// CSTSPinApduResp::IsBlocked
// Panics, if apdu data buffer contains too little information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSPinApduResp::IsBlocked()
{
    return (CountRetriesLeft(GetSW1(), GetSW2()) == 0);
}

// -----------------------------------------------------------------------------
// CSTSPinApduResp::RetriesLeft
// Panics, if apdu data buffer contains too little information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CSTSPinApduResp::RetriesLeft() const
{
    return CountRetriesLeft(GetSW1(), GetSW2());
}

// -----------------------------------------------------------------------------
// CSTSPinApduResp::CountRetriesLeft
// Panics, if apdu data buffer contains too little information.
// Checks in two ways from response apdu buffer, how many retries is left.
// Returns KErrNotFound if retries left value cannot be calculated.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CSTSPinApduResp::CountRetriesLeft(TUint8 aSW1, TUint8 aSW2) const
{
    TInt returnValue;
    if ((aSW1 == KSTSSW1PinBlocked) && (aSW2 == KSTSSW2PinBlocked))
    {
        returnValue = 0;
    }
    else if (aSW1 == KSTSSW1RetriesLeft)
    {
        TUint8 retryCounterMask = 0x03;
        returnValue = aSW2 & retryCounterMask;
    }
    else
    {
        returnValue = KErrNotFound;
    }
    return returnValue;
}

} // namespace satsa
} // namespace java
//  End of File
