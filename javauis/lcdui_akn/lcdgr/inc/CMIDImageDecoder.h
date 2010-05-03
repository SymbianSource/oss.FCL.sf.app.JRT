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
* Description:
*
*/

#ifndef CMIDIMAGEDECODER_H
#define CMIDIMAGEDECODER_H

#include <e32base.h>
#include "lcdgr.h"

#include <imageconversion.h>
#include "CMIDBitmapImage.h"

/**
 * Peered, probably singleton facade to image operations.
 *
 * Methods invoked by server on behalf of java threads:
 *  DecodeL
 *
 * expect java thread to block in waitForResult on peer's
 * condition variable.
 */
NONSHARABLE_CLASS(CMIDImageDecoder)
        : public  CActive
        , public  MMIDImageDecoder
{
public:
    CMIDImageDecoder(RFs& aFileServer, TDisplayMode aDisplayMode);
    void ConstructL();
    ~CMIDImageDecoder();

    //
    // From MMIDImageDecoder
    //
    void AppendL(const TDesC8& aBuffer);
    void DecodeL(MMIDImageObserver* aObserver);
    void Dispose();
    MMIDBitmapImage* BitmapImage() const;

private:
    void Complete(TInt aResult);

    void ConvertL();
    void CreateImageL();

private:
    void RunL();
    TInt RunError(TInt);
    void DoCancel();

private:
    enum TState
    {
        EStart,
        EOpenPending,
        EOpenComplete,
        EConvertPending,
        EComplete,
        ECancelled,
        EError=-1
    };

private:
    CBufferedImageDecoder* iDecoder;
    TFrameInfo iFrameInfo;
    TState iState;
    CMIDBitmapImage* iBitmapImage;
    MMIDImageObserver* iCallback;
    RFs& iRFsSession;
    TDisplayMode                iDisplayMode;       // display mode from CMIDGraphicsFactory
};

#endif // CMIDIMAGEDECODER_H
