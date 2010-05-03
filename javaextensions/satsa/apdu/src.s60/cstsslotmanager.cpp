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

#include "cstscardoperationsfacade.h"
#include "cstsslotmanager.h"
#include "stsapduconstants.h"
#include "cstsslotinfo.h"
#include "logger.h"
#include "javasymbianoslayer.h"

namespace java
{
namespace satsa
{

//  CONSTANTS
const TInt KSTSDefaulGranularity = 1;
const TUint8 KSTSSlotZeroColdSwappable = 0xD0;

// ============================ MEMBER FUNCTIONS ===============================


CSTSSlotManager::CSTSSlotManager(CSTSCardOperationsFacade* aCOFacade)
{
    iCOFacade = aCOFacade;
    iGetPerformed = EFalse;
}

void CSTSSlotManager::ConstructL()
{
    iSlotArray
    = new(ELeave) CArrayPtrFlat<CSTSSlotInfo> (KSTSDefaulGranularity);
}

CSTSSlotManager* CSTSSlotManager::NewL(CSTSCardOperationsFacade* aCOFacade)
{
    LOG(ESATSA, EInfo, "+ CSTSSlotManager::NewL");
    CSTSSlotManager* self = new(ELeave) CSTSSlotManager(aCOFacade);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    LOG(ESATSA, EInfo, "--- CSTSSlotManager::NewL");
    return self;
}

CSTSSlotManager::~CSTSSlotManager()
{
    delete iSlotString;
    if (iSlotArray)
    {
        iSlotArray->ResetAndDestroy();
    }
    delete iSlotArray;
}

// -----------------------------------------------------------------------------
// CSTSSlotManager::GetSlotsL
// Get the slots available in the card
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
std::wstring CSTSSlotManager::GetSlotsL()
{

    LOG(ESATSA, EInfo, "+ CSTSClotManager::GetSlotsL() called");

    //TPtr8 respPtr( NULL, 0 );
    HBufC* tmp = HBufC::NewL(KMaxReaderAmount);
    delete iSlotString;
    iSlotString = tmp;

    TInt readerCount = 1; // one reader at this point
    for (TInt reader = 0; reader < readerCount; reader++)
    {
        LOG(ESATSA, EInfo, "CSTSSlotManager::GetSlotsL:Going to get slot information");
        CSTSSlotInfo* slotInfo = CSTSSlotInfo::NewL(KSTSSlotZeroColdSwappable);
        LOG(ESATSA, EInfo, "After initializing slotInfo object");
        CleanupStack::PushL(slotInfo);
        //ownership moved to array
        iSlotArray->AppendL(slotInfo);
        CleanupStack::Pop(slotInfo);

        if (reader > 0)
        {
            LOG(ESATSA, EInfo, "CSTSSlotManager::GetSlotsL:reader > 0");
            // comma-separator between slots info
            iSlotString->Des().Append(',');
        }
        LOG(ESATSA, EInfo, "CSTSSlotManager::GetSlotsL: Append the slotInfo");
        iSlotString->Des().AppendNum(slotInfo->Slot());
        iSlotString->Des().Append(slotInfo->Type());

    }

    iGetPerformed = ETrue;
    LOG(ESATSA, EInfo, "CSTSSlotManager::GetSlotsL:Start Convertion!");
    TPtr toConvert(iSlotString->Des());
    std::wstring slotString = desToWstring(toConvert);
    LOG(ESATSA, EInfo, "-- CSTSClotManager::GetSlotsL()");
    return slotString;

}

// -----------------------------------------------------------------------------
// CSTSSlotManager::DoesExistL
// Verifies whether the slot exists in the card
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSTSSlotManager::DoesExistL(TInt aSlot)
{
    // get slots if needed and check does the gived slot exist on the phone
    LOG(ESATSA, EInfo, "+ CSTSSlotManager::DoesExistL called");

    if (!iGetPerformed)
    {
        std::wstring slots = GetSlotsL();
    }
    for (TInt i = 0; i < iSlotArray->Count(); i++)
    {
        if (iSlotArray->At(i)->Slot() == aSlot)
        {
            LOG(ESATSA, EInfo, "CSTSSlotManager::DoesExistL: Slot exists");
            return ETrue;
        }
    }

    ELOG(ESATSA, "CSTSSlotManager::DoesExistL: Slot does NOT exist");
    return EFalse;

}

} // namespace satsa
} // namespace java
//  End of File

