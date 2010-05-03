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
* Description: CMMAAudioStreamMetaDataControl is a concrete class for getting
*              metadata from an audio media.
*
*/

#ifndef CMMAAUDIOSTREAMMETADATACONTROL_H
#define CMMAAUDIOSTREAMMETADATACONTROL_H

//  INCLUDES
#include <mmfcontroller.h>
#include <MetaDataUtility.h>
#include <MetaDataFieldContainer.h>
#include "cmmametadatacontrol.h"

// Constants
_LIT(KMetaDataSongTitle, "title");
_LIT(KMetaDataArtist, "author");
_LIT(KMetaDataAlbum, "album");
_LIT(KMetaDataYear, "year of recording");
_LIT(KMetaDataComment, "comment");
_LIT(KMetaDataAlbumTrack, "album track");
_LIT(KMetaDataGenre, "genre");
_LIT(KMetaDataComposer, "composer");
_LIT(KMetaDataCopyright, "copyright");
_LIT(KMetaDataOriginalArtist, "original artist");
_LIT(KMetaDataUrl, "url");
_LIT(KMetaDataUserUrl, "user url");
_LIT(KMetaDataJpeg, "front jpeg");
_LIT(KMetaDataVendor, "vendor");
_LIT(KMetaDataRating, "rating");
_LIT(KMetaDataUniqueFileIdentifier, "unique file identifier");
_LIT(KMetaDataDuration, "duration");
_LIT(KMetaDataDate, "date");

const TInt KKeyStringMaxLength = 60;


//  CLASS DECLARATION
/**
*   This is a concrete class for getting metadata from an audio media.
*
*
*/

NONSHARABLE_CLASS(CMMAAudioStreamMetaDataControl): public CMMAMetaDataControl
{
public:
    CMMAAudioStreamMetaDataControl(CMetaDataUtility* aMetadaDataUtility);
    ~CMMAAudioStreamMetaDataControl();

protected: // from CMMAMetaDataControl

    TInt KeyCountL();
    HBufC* KeyL(TInt aIndex);

    HBufC* KeyValueL(const TDesC& aKey);

private: // new methods
    HBufC* GetKeyL(TMetaDataFieldId aFieldId);
    TMetaDataFieldId GetFieldId(const TDesC& aKey, TBool& aCompare);

private: // data owned
    CMetaDataUtility* iMetaDataUtility;

};

#endif // CMMAAUDIOSTREAMMETADATACONTROL_H
