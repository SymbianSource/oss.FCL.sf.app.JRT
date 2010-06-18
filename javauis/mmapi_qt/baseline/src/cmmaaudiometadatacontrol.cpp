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
* Description:  CMMAAudioMetaDataControl is a concrete class for getting
*                metadata from an audio media.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmaaudiometadatacontrol.h"

CMMAAudioMetaDataControl::CMMAAudioMetaDataControl(
    RMMFController& aController)
        : iController(aController)
{
    LOG(EJavaMMAPI, EInfo, "CMMAAudioMetaDataControl constructor called.");
}

TInt CMMAAudioMetaDataControl::KeyCountL()
{
    TInt entries = 0;

    User::LeaveIfError(iController.GetNumberOfMetaDataEntries(entries));
    return entries;
}

HBufC* CMMAAudioMetaDataControl::KeyL(TInt aIndex)
{
    CMMFMetaDataEntry* currEntry = NULL;

    currEntry = iController.GetMetaDataEntryL(aIndex);
    CleanupStack::PushL(currEntry);
    HBufC* key = currEntry->Name().AllocL();
    CleanupStack::PopAndDestroy(); // currEntry

    return key;
}


/*
 * Get the value of given audio metadata key. The ownership of the created value
 * (descriptor) is passed to the caller.
 */
HBufC* CMMAAudioMetaDataControl::KeyValueL(const TDesC& aKey)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAAudioMetaDataControl::GetKeyValueL() called.");

    HBufC* retVal = NULL;
    CMMFMetaDataEntry* currEntry = NULL;

    TInt nEntries = 0;
    User::LeaveIfError(iController.GetNumberOfMetaDataEntries(nEntries));

    for (TInt i = 0; i < nEntries; ++i)
    {
        currEntry = iController.GetMetaDataEntryL(i);

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
