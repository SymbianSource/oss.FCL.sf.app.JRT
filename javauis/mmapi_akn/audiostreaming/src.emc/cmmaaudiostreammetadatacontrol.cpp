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
* Description:  CMMAAudioStreamMetaDataControl is a concrete class for getting
*                metadata from an audio media.
*
*/


//  INCLUDE FILES
#include <jdebug.h>

#include "cmmaaudiostreammetadatacontrol.h"

CMMAAudioStreamMetaDataControl::CMMAAudioStreamMetaDataControl(
    CMetaDataUtility* aMetadaDataUtility)
{
    DEBUG("CMMAAudioStreamMetaDataControl constructor called.");
    // this class gets the ownership of MetaDataUtility
    iMetaDataUtility = aMetadaDataUtility;
}

CMMAAudioStreamMetaDataControl::~CMMAAudioStreamMetaDataControl()
{
    DEBUG("~CMMAAudioStreamMetaDataControl called.");
    delete iMetaDataUtility;
}

TInt CMMAAudioStreamMetaDataControl::KeyCountL()
{
    TInt entries = iMetaDataUtility->MetaDataCount();
    DEBUG_INT("MMA:CMMAAudioStreamMetaDataControl::KeyCountL, count = %d", entries);

    return entries;
}

HBufC* CMMAAudioStreamMetaDataControl::KeyL(TInt aIndex)
{
    DEBUG("MMA::CMMAAudioStreamMetaDataControl::KeyL +");
    HBufC* key = NULL;

    if (KeyCountL() > 0)
    {
        TMetaDataFieldId fieldId;
        const CMetaDataFieldContainer& fieldcontainer =
            iMetaDataUtility->MetaDataFieldsL();
        fieldcontainer.FieldIdAt(aIndex, fieldId);
        DEBUG_INT("MMA:CMMAAudioStreamMetaDataControl::KeyL, fieldId = %d", fieldId);
        key = GetKeyL(fieldId);
    }

    DEBUG("MMA::CMMAAudioStreamMetaDataControl::KeyL -");
    return key;
}

/*
 * Get the value of given audio metadata key. The ownership of the created value
 * (descriptor) is passed to the caller.
 */
HBufC* CMMAAudioStreamMetaDataControl::KeyValueL(const TDesC& aKey)
{
    DEBUG("MMA::CMMAAudioStreamMetaDataControl::KeyValueL +");
    HBufC* retVal = NULL;

    if (KeyCountL() > 0)
    {
        TBool found;
        TMetaDataFieldId fieldId = GetFieldId(aKey, found);
        if (found)
        {
            const CMetaDataFieldContainer& fieldcontainer =
                iMetaDataUtility->MetaDataFieldsL();
            retVal = (fieldcontainer.Field(fieldId)).AllocL();
        }
    }

    User::LeaveIfNull(retVal);
    DEBUG_STR("MMA::CMMAAudioStreamMetaDataControl::KeyValueL, retVal = %S", *retVal);
    DEBUG("MMA::CMMAAudioStreamMetaDataControl::KeyValueL -");
    return retVal;
}

/*
* Get key string for the given fieldId
*/
HBufC* CMMAAudioStreamMetaDataControl::GetKeyL(TMetaDataFieldId aFieldId)
{
    DEBUG("MMA::CMMAAudioStreamMetaDataControl::GetKeyL +");
    HBufC* keyString = NULL;
    TBuf<KKeyStringMaxLength> tempBuf;

    switch (aFieldId)
    {
    case EMetaDataSongTitle:
        tempBuf.Copy(KMetaDataSongTitle);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataArtist:
        tempBuf.Copy(KMetaDataArtist);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataAlbum:
        tempBuf.Copy(KMetaDataAlbum);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataYear:
        tempBuf.Copy(KMetaDataYear);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataAlbumTrack:
        tempBuf.Copy(KMetaDataAlbumTrack);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataGenre:
        tempBuf.Copy(KMetaDataGenre);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataComposer:
        tempBuf.Copy(KMetaDataComposer);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataCopyright:
        tempBuf.Copy(KMetaDataCopyright);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataOriginalArtist:
        tempBuf.Copy(KMetaDataOriginalArtist);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataUrl:
        tempBuf.Copy(KMetaDataUrl);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataUserUrl:
        tempBuf.Copy(KMetaDataUserUrl);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataJpeg:
        tempBuf.Copy(KMetaDataJpeg);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataVendor:
        tempBuf.Copy(KMetaDataVendor);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataRating:
        tempBuf.Copy(KMetaDataRating);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataUniqueFileIdentifier:
        tempBuf.Copy(KMetaDataUniqueFileIdentifier);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataDuration:
        tempBuf.Copy(KMetaDataDuration);
        keyString = tempBuf.AllocL();
        break;
    case EMetaDataDate:
        tempBuf.Copy(KMetaDataDate);
        keyString = tempBuf.AllocL();
        break;
    default:
        DEBUG("MMA::CMMAAudioStreamMetaDataControl::GetKeyL, default case should not occur");
    }

    DEBUG("MMA::CMMAAudioStreamMetaDataControl::GetKeyL -");
    return keyString;
}

/*
* Get fieldId for the given key string
*/
TMetaDataFieldId CMMAAudioStreamMetaDataControl::GetFieldId(const TDesC& aKey, TBool& aCompare)
{
    DEBUG("MMA::CMMAAudioStreamMetaDataControl::GetFieldId +");
    TMetaDataFieldId fieldId = EUnknownMetaDataField;
    aCompare = true; // found

    if (0 == aKey.Compare(KMetaDataSongTitle()))
    {
        fieldId = EMetaDataSongTitle;
    }
    else if (0 == aKey.Compare(KMetaDataArtist()))
    {
        fieldId = EMetaDataArtist;
    }
    else if (0 == aKey.Compare(KMetaDataAlbum()))
    {
        fieldId = EMetaDataAlbum;
    }
    else if (0 == aKey.Compare(KMetaDataArtist()))
    {
        fieldId = EMetaDataArtist;
    }
    else if (0 == aKey.Compare(KMetaDataYear()))
    {
        fieldId = EMetaDataYear;
    }
    else if (0 == aKey.Compare(KMetaDataComment()))
    {
        fieldId = EMetaDataComment;
    }
    else if (0 == aKey.Compare(KMetaDataAlbumTrack()))
    {
        fieldId = EMetaDataAlbumTrack;
    }
    else if (0 == aKey.Compare(KMetaDataGenre()))
    {
        fieldId = EMetaDataGenre;
    }
    else if (0 == aKey.Compare(KMetaDataComposer()))
    {
        fieldId = EMetaDataComposer;
    }
    else if (0 == aKey.Compare(KMetaDataCopyright()))
    {
        fieldId = EMetaDataCopyright;
    }
    else if (0 == aKey.Compare(KMetaDataOriginalArtist()))
    {
        fieldId = EMetaDataOriginalArtist;
    }
    else if (0 == aKey.Compare(KMetaDataUrl()))
    {
        fieldId = EMetaDataUrl;
    }
    else if (0 == aKey.Compare(KMetaDataUserUrl()))
    {
        fieldId = EMetaDataUserUrl;
    }
    else if (0 == aKey.Compare(KMetaDataJpeg()))
    {
        fieldId = EMetaDataJpeg;
    }
    else if (0 == aKey.Compare(KMetaDataVendor()))
    {
        fieldId = EMetaDataVendor;
    }
    else if (0 == aKey.Compare(KMetaDataRating()))
    {
        fieldId = EMetaDataRating;
    }
    else if (0 == aKey.Compare(KMetaDataUniqueFileIdentifier()))
    {
        fieldId = EMetaDataUniqueFileIdentifier;
    }
    else if (0 == aKey.Compare(KMetaDataDuration()))
    {
        fieldId = EMetaDataDuration;
    }
    else if (0 == aKey.Compare(KMetaDataDate()))
    {
        fieldId = EMetaDataDate;
    }
    else
    {
        aCompare = false; // not found
    }

    DEBUG("MMA::CMMAAudioStreamMetaDataControl::GetFieldId -");
    return fieldId;
}

//  END OF FILE
