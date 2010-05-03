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
#include  "cstsapdu.h"
#include  "logger.h"

namespace java
{
namespace satsa
{

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSApdu::CSTSApdu
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSApdu::CSTSApdu(TSTSApduStandard aStandard) :
        iApduBytesPtr(NULL, 0)
{
    iStandard = aStandard;
}

// -----------------------------------------------------------------------------
// CSTSApdu::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSApdu::ConstructL(TDesC8& aApduBytes)
{
    LOG(ESATSA, EInfo, "+ CSTSApdu::ConstructL");
    iApduBytes = aApduBytes.AllocL();
    iApduBytesPtr.Set(iApduBytes->Des());
    LOG(ESATSA, EInfo, "-- CSTSApdu::ConstructL");
}

// -----------------------------------------------------------------------------
// CSTSApdu::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSApdu::ConstructL(TInt aMaxLength)
{
    iApduBytes = HBufC8::NewL(aMaxLength);
    iApduBytesPtr.Set(iApduBytes->Des());
}

// -----------------------------------------------------------------------------
// CSTSApdu::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSApdu* CSTSApdu::NewLC(TDesC8& aApduBytes, TSTSApduStandard aStandard)
{
    LOG(ESATSA, EInfo, "+ CSTSApdu::NewLC");
    CSTSApdu* self = new(ELeave) CSTSApdu(aStandard);
    CleanupStack::PushL(self);
    self->ConstructL(aApduBytes);
    LOG(ESATSA, EInfo, "-- CSTSApdu::NewLC");
    return self;
}

// -----------------------------------------------------------------------------
// CSTSApdu::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSApdu* CSTSApdu::NewL(TDesC8& aApduBytes, TSTSApduStandard aStandard)
{
    LOG(ESATSA, EInfo, "+ CSTSApdu::NewL");
    CSTSApdu* self = CSTSApdu::NewLC(aApduBytes, aStandard);
    CleanupStack::Pop(self);
    LOG(ESATSA, EInfo, "-- CSTSApdu::NewL");
    return self;
}

// -----------------------------------------------------------------------------
// CSTSApdu::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSApdu* CSTSApdu::NewLC(TInt aMaxLength, TSTSApduStandard aStandard)
{
    LOG(ESATSA, EInfo, "+ CSTSApdu::NewLC");
    CSTSApdu* self = new(ELeave) CSTSApdu(aStandard);
    CleanupStack::PushL(self);
    self->ConstructL(aMaxLength);
    LOG(ESATSA, EInfo, "-- CSTSApdu::NewLC");
    return self;
}

// -----------------------------------------------------------------------------
// CSTSApdu::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSApdu* CSTSApdu::NewL(TInt aMaxLength, TSTSApduStandard aStandard)
{
    CSTSApdu* self = CSTSApdu::NewLC(aMaxLength, aStandard);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CSTSApdu::~CSTSApdu()
{
    delete iApduBytes;
}

// -----------------------------------------------------------------------------
// CSTSApdu::ApduPtr
// Getter for APDU ptr
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TPtr8& CSTSApdu::ApduPtr()
{
    iApduBytesPtr.Set(iApduBytes->Des());
    return iApduBytesPtr;
}

// -----------------------------------------------------------------------------
// CSTSApdu::ReAllocL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSApdu::ReAllocL(TInt aMaxLength)
{
    if (!iApduBytes)
    {
        iApduBytes = HBufC8::NewL(aMaxLength);
    }
    else if (iApduBytesPtr.MaxLength() < aMaxLength)
    {
        iApduBytes = iApduBytes->ReAllocL(aMaxLength);
    }
    iApduBytesPtr.Set(iApduBytes->Des());
}

// -----------------------------------------------------------------------------
// CSTSApdu::Standard
// Returns used standard
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CSTSApdu::TSTSApduStandard CSTSApdu::Standard() const
{
    return iStandard;
}

} // namespace satsa
} // namespace java
//  End of File
