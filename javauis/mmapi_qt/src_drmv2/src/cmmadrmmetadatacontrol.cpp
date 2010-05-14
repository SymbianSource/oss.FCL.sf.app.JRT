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
* Description:  CMMADRMMetaDataControl is a concrete class for getting
*                metadata from DRM files.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmadrmmetadatacontrol.h"

CMMADRMMetaDataControl::CMMADRMMetaDataControl(
    CMMADRMAudioPlayer* aPlayer)
        : iPlayer(aPlayer)
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMADRMMetaDataControl constructor called.");
}

TInt CMMADRMMetaDataControl::KeyCountL()
{
    TInt entries;
    User::LeaveIfError(iPlayer->DRMUtility()->GetNumberOfMetaDataEntries(entries));
    return entries;
}

HBufC* CMMADRMMetaDataControl::KeyL(TInt aIndex)
{
    LOG( EJavaMMAPI, EInfo, "MMA: CMMADRMMetaDataControl::KeyL");
    CMMFMetaDataEntry* currEntry =
        iPlayer->DRMUtility()->GetMetaDataEntryL(aIndex);
    CleanupStack::PushL(currEntry);
    HBufC* key = currEntry->Name().AllocL();
    CleanupStack::PopAndDestroy(); // currEntry
    return key;
}


/*
 * Get the value of given drm metadata key. The ownership of the created value
 * (descriptor) is passed to the caller.
 */
HBufC* CMMADRMMetaDataControl::KeyValueL(const TDesC& aKey)
{
    LOG( EJavaMMAPI, EInfo, "MMA: CMMADRMMetaDataControl::KeyValueL");

    HBufC* retVal = NULL;
    CMMFMetaDataEntry* currEntry = NULL;

    TInt nEntries;
    User::LeaveIfError(iPlayer->DRMUtility()->GetNumberOfMetaDataEntries(nEntries));

    for (TInt i = 0; i < nEntries; ++i)
    {
        currEntry = iPlayer->DRMUtility()->GetMetaDataEntryL(i);

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
