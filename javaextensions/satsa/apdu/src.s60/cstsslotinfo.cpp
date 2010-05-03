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
#include "cstsslotinfo.h"
#include "logger.h"

namespace java
{
namespace satsa
{

// CONSTANTS

// Reader status bits.
const TUint8 KIdentityMask = 0x07;
const TUint8 KCardReaderRemovable = 0x08;
const TUint8 KCardPresent = 0x40;

const TUint8 KHotSwappable = 'H';
const TUint8 KColdSwappable = 'C';

// ============================ MEMBER FUNCTIONS ===============================


CSTSSlotInfo::CSTSSlotInfo()
{
}

void CSTSSlotInfo::ConstructL(const TUint8& aRawData)
{
    LOG(ESATSA, EInfo, "+ CSTSSlotInfo::ConstructL");
    iRawData = aRawData;
    // parse rawdata
    iRemovable = (iRawData & KCardReaderRemovable);
    iCardPresent = (iRawData & KCardPresent);
    iSlotNumber = (iRawData & KIdentityMask);
    LOG(ESATSA, EInfo, "-- CSTSSlotInfo::ConstructL");
}

CSTSSlotInfo* CSTSSlotInfo::NewL(const TUint8& aRawData)
{
    LOG(ESATSA, EInfo, "+ CSTSSlotInfo::NewL called");
    CSTSSlotInfo* self = new(ELeave) CSTSSlotInfo;
    CleanupStack::PushL(self);
    self->ConstructL(aRawData);
    CleanupStack::Pop(self);
    LOG(ESATSA, EInfo, "- CSTSSlotInfo::NewL");
    return self;
}

CSTSSlotInfo::~CSTSSlotInfo()
{

}

TInt CSTSSlotInfo::Slot()
{
    LOG(ESATSA, EInfo, "+ CSTSSlotInfo::SLot");
    return iSlotNumber;
}

// -----------------------------------------------------------------------------
// CSTSSlotInfo::Type
// Returns type of the slot.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TChar CSTSSlotInfo::Type()
{
    LOG(ESATSA, EInfo, "+ CSTSSlotInfo::Type");
    if (iRemovable)
    {
        LOG(ESATSA, EInfo, "+ CSTSSlotInfo::Type: Card is HotSwappable");
        return KHotSwappable;
    }
    else
    {
        LOG(ESATSA, EInfo, "+ CSTSSlotInfo::Type: Card is ColdSwappable");
        return KColdSwappable;
    }
}

} // namespace satsa
} // namespace java
//  End of File

