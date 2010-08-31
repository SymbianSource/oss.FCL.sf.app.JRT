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

#ifndef CMMAEMCRESOLVER_H
#define CMMAEMCRESOLVER_H

#include <MimeTypes.h>
#include "cmmaplayer.h"

//Supported file extensions

_LIT(K3g2FileExtension, ".3g2");
_LIT(K3gpFileExtension, ".3gp");
_LIT(KAacFileExtension, ".aac");
_LIT(KAmrFileExtension, ".amr");
_LIT(KAwbFileExtension, ".awb");
_LIT(KM4aFileExtension, ".m4a");
_LIT(KMp3FileExtension, ".mp3");
_LIT(KMp4FileExtension, ".mp4");
_LIT(KWmaFileExtension, ".wma");
_LIT(KRmFileExtension, ".rm");
_LIT(KRaFileExtension, ".ra");

const TInt KContentTypeMaxLength = 30;


//  CLASS DECLARATION

NONSHARABLE_CLASS(CMMAEMCResolver): public CBase

{
public: // Construction
    /**
     * Constructs a new mmf resolver object.
     * @return The new object created.  Note that the new object will be
     * left on the cleanup stack.
     */
    static CMMAEMCResolver* NewLC();

public: // New methods.

    HBufC* ContentTypeOwnership();

    HBufC8* MimeTypeOwnership();

    /**
     * Returns content type
     */
    HBufC* ContentType();

    /**
     * Sets filename
     * @param aFileName File name to be set or NULL
     */

    void SetFileNameL(const TDesC* aFileName);

    /**
     * Sets Mime Type
     * @param aFileName File name to be set or NULL
     */

    void SetMimeTypeL(const TDesC* aFileName);

    /**
     * Returns possible filename or NULL if not set
     * Ownership is transfered.
     * @return Content type or NULL if type is not available.
     */
    HBufC* FileNameOwnership();

    /**
     * Get all supported content types.
     * @param aMimeTypeArray Will contain supported mime types.
     */
    void GetSupportedContentTypesL(CDesC16Array& aMimeTypeArray);


private:

    /** Finds content type from given File name.
    */
    void ResolveContentTypeL(const TDesC& aFileName, TDes8& aMimeType);

    void ResolveContentTypeL();

private:
    ~CMMAEMCResolver();

private:

    // Owned. Ownership can be transferred with ContentType method.
    HBufC* iContentType;

    // Owned. Ownership can be transferred with MimeType method.
    HBufC8* iMimeType;

    // Owned. Ownership can be transferred with FileName method.
    HBufC* iFileName;

public: // new method
    void SetSourceInfoL(const HBufC8* aHeaderData);

private:
    TBool IsRealVideoTypeL(const TDesC8& aHeader);

    TBool IsRealMimeTypeSupported(const TDesC& aMimeType);

private: // Data
    const HBufC8* iHeaderData; // not owned
};

#endif // CMMAEMCRESOLVER_H
