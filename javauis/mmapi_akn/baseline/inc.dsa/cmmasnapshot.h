/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class takes snapshot and resizes bitmap if needed.
*
*/

#ifndef CMMASNAPSHOT_H
#define CMMASNAPSHOT_H

//  INCLUDES
#include <bitmaptransforms.h>
#include "tmmaparametervalidator.h"
#include "mmmasnapshot.h"

//  FORWARD DECLARATIONS
class MMMAGuiPlayer;
class MMMAEventPoster;
class MMMASnapshotReadyCallback;


//  CLASS DECLARATION
/**
*   This class takes snapshot from MMMAGuiPlayer and resizes bitmap
*   if needed. Then bitmap is converted to wanted image format and posted
*   to java side. This class is used only from CMMAVideoControl and
*   this class is made to separate many asynchronous calls from control and
*   resource releasing.
*
*
*/

NONSHARABLE_CLASS(CMMASnapshot): public CActive
{
private: // private state
    enum TMMASnapshotState
    {
        EIdle,
        ETakingSnapshot,
        EResizing,
        EEncoding
    };
public:
    /**
     * Creates new CMMASnapshot which will be associated to gui player.
     *
     * @param aGuiPlayer Player used to take snapshot bitmap.
     * @param aCallBack Will be informed when completed.
     */
    static CMMASnapshot* NewL(MMMAGuiPlayer* aGuiPlayer,
                              MMMASnapshotReadyCallback& aCallBack);
    /**
     * Destructor.
     */
    virtual ~CMMASnapshot();

private:
    /**
     * Creates required member variables.
     */
    void ConstructL();

    /**
     * Contructs new CMMASnapshot.
     *
     * @param aGuiPlayer Used to take snapshot bitmap.
     * @param aCallBack Will be informed when completed.
     */
    CMMASnapshot(MMMAGuiPlayer* aGuiPlayer,
                 MMMASnapshotReadyCallback& aCallBack);

public: // new methods
    /**
     * Takes snapshot from player and converts it to according to
     * the properties.
     *
     * @param aProperties Snapshot's properties.
     */
    void TakeSnapShotL(const TDesC& aProperties);

    /**
     * Caller takes the ownership of the buffer.
     *
     * @return Image buffer. Ownership is transferred.
     */
    HBufC8* ImageBuffer();


private:// New methods
    /**
     * Resizing image to size of settings
     */
    void ResizeL();

    /**
     * Encodes bitmap to wanted mime-type
     */
    void EncodeL();

    /**
     * Called when snapshot is ready or error occures. This method will
     * inform also the MMMASnapshotReadyCallback interface.
     *
     * @param aError Error code for the caller class.
     */
    void Completed(TInt aError);

public: // CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

private:
    /**
     * Bitmap encoder, owned
     */
    CImageEncoder* iEncoder;

    /**
     * Bitmap will be got from gui player and ownership is
     * transfers to this class, owned
     */
    CFbsBitmap* iBitmap;

    /**
     * Bitmap scaler, owned
     */
    CBitmapScaler* iScaler;

    /**
     * Buffer used for storing encoded data, owned
     */
    HBufC8* iBuffer;

    /**
     * Bitmap settings, owned
     */
    CMMAImageSettings* iSettings;

    /**
     * Snapshot encoding
     */
    MMMASnapshot::TEncoding iEncoding;

    /**
     * GUI player interface, used for taking snapshot, not owned
     */
    MMMAGuiPlayer* iGUIPlayer;

    /**
     * When snapshot is ready or error occures callback will be informed.
     */
    MMMASnapshotReadyCallback& iCallBack;

    /**
     * Internal state
     */
    TMMASnapshotState iState;
};

#endif // CMMASNAPSHOT_H






