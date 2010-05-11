/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include    "CAMMSTunerPresetsHandler.h"
#include    "TAMMSTunerPreset.h"

#include    <FMRadioEngineCRKeys.h>
#include    <s32mem.h>
#include <logger.h>


// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSTunerPresetsHandler::CAMMSTunerPresetsHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAMMSTunerPresetsHandler::CAMMSTunerPresetsHandler()
{
}

// -----------------------------------------------------------------------------
// CAMMSTunerPresetsHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAMMSTunerPresetsHandler::ConstructL()
{
    iCentralRepository = CRepository::NewL(KCRUidFMRadioEngine);
}

// -----------------------------------------------------------------------------
// CAMMSTunerPresetsHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAMMSTunerPresetsHandler* CAMMSTunerPresetsHandler::NewL()
{
    CAMMSTunerPresetsHandler* self = new(ELeave) CAMMSTunerPresetsHandler;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}


// Destructor
CAMMSTunerPresetsHandler::~CAMMSTunerPresetsHandler()
{
    delete iCentralRepository;
}


// -----------------------------------------------------------------------------
// CAMMSTunerPresetsHandler::GetPreset
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSTunerPresetsHandler::GetPresetL(TInt aPreset, TAMMSTunerPreset &aTunerPreset)
{
    // KFmRadioPresetChannel1 = The first preset channel's location in central repository
    TInt tempChannelIndex = KFmRadioPresetChannel1 + aPreset;

    HBufC8* buf = HBufC8::NewLC(sizeof(TAMMSTunerPreset));
    TPtr8 bufPtr = buf->Des();

    TInt err = iCentralRepository->Get(tempChannelIndex, bufPtr);
    if (err == KErrNone)
    {
        RDesReadStream inStream;
        inStream.Open(bufPtr);
        CleanupClosePushL(inStream);
        aTunerPreset.InternalizeL(inStream);
        CleanupStack::PopAndDestroy(&inStream);
    }
    else
    {
        User::Leave(err);
    }
    CleanupStack::PopAndDestroy(); // buf
}

// -----------------------------------------------------------------------------
// CAMMSTunerPresetsHandler::SavePreset
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAMMSTunerPresetsHandler::SavePresetL(TInt aPreset, TAMMSTunerPreset aTunerPreset)
{
    // KFmRadioPresetChannel1 = The first preset channel's location in central repository
    TInt fieldNameIndex = KFmRadioPresetChannel1 + TUint32(aPreset);

    HBufC8* buf = HBufC8::NewLC(sizeof(TAMMSTunerPreset));
    TPtr8 bufPtr = buf->Des();

    RDesWriteStream outStream(bufPtr);
    CleanupClosePushL(outStream);
    aTunerPreset.ExternalizeL(outStream);
    outStream.CommitL();
    CleanupStack::PopAndDestroy(&outStream);

    iCentralRepository->Set(fieldNameIndex, bufPtr);
    CleanupStack::PopAndDestroy(); //buf
}


//  End of File
