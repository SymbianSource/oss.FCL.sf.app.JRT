/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMMAMIDIMetaDataControl is a concrete class for getting
*                metadata from midi engine.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmamidimetadatacontrol.h"

CMMAMIDIMetaDataControl::CMMAMIDIMetaDataControl(
    CMMAMIDIPlayer* aPlayer)
        : iPlayer(aPlayer)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAMIDIMetaDataControl constructor called.");
}

TInt CMMAMIDIMetaDataControl::KeyCountL()
{
    LOG(EJavaMMAPI, EInfo, "MMA: CMMAMIDIMetaDataControl::KeyCountL");
    TInt entries = iPlayer->MidiClient()->NumberOfMetaDataEntriesL();
    return entries;
}

HBufC* CMMAMIDIMetaDataControl::KeyL(TInt aIndex)
{
    LOG(EJavaMMAPI, EInfo, "MMA: CMMAMIDIMetaDataControl::KeyL");
    CMMFMetaDataEntry* currEntry =
        iPlayer->MidiClient()->GetMetaDataEntryL(aIndex);
    CleanupStack::PushL(currEntry);
    HBufC* key = currEntry->Name().AllocL();
    CleanupStack::PopAndDestroy(); // currEntry
    return key;
}


/*
 * Get the value of given midi metadata key. The ownership of the created value
 * (descriptor) is passed to the caller.
 */
HBufC* CMMAMIDIMetaDataControl::KeyValueL(const TDesC& aKey)
{
    LOG(EJavaMMAPI, EInfo, "MMA: CMMAMIDIMetaDataControl::KeyValueL");

    HBufC* retVal = NULL;
    CMMFMetaDataEntry* currEntry = NULL;

    TInt nEntries = iPlayer->MidiClient()->NumberOfMetaDataEntriesL();

    for (TInt i = 0; i < nEntries; ++i)
    {
        currEntry = iPlayer->MidiClient()->GetMetaDataEntryL(i);

        if (0 == aKey.Compare(currEntry->Name()))
        {
            CleanupStack::PushL(currEntry);
            retVal = currEntry->Value().AllocL();
            CleanupStack::PopAndDestroy(); // currEntry
            break;
        }

        delete currEntry;
    }

    User::LeaveIfNull(retVal);
    return retVal;
}

//  END OF FILE
