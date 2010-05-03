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
* Description:  This header is for shapshots.
*
*/


#ifndef MMMASNAPSHOT_H
#define MMMASNAPSHOT_H

//  FORWARD DECLARATIONS
class CFbsBitmap;
class CMMAImageSettings;

//  CLASS DEFINITION
/**

*/

NONSHARABLE_CLASS(MMMASnapshot)
{
public:     // Type definitions
    enum TEncoding
    {
        EBitmap = 0,    // Raw image format
        EEncoded        // Encoded image format
    };

protected:  // Constructors and destructors
    virtual ~MMMASnapshot() {};     // Do not allow delete through this interface

public:     // Abstract methods
    /**
     * Takes snapshot.
     *
     * @param aStatus Status will be informed when
     *        bitmap is ready or error occures.
     * @param aSize Snapshot size. If size is not specified (width=-1 or
     *        height=-1) default size is used.
     * @return Returns snapshot image's encoding type based on the requested MIME-type and
     *         snapshot size. Returned encoding type may differ from the requested as
     *         raw bitmap type is used if possibly requested encoding type is not found.
     */
    virtual TEncoding TakeSnapshotL(TRequestStatus* aStatus,
                                    const TSize& aSize,
                                    const CMMAImageSettings& aSettings) = 0;

    /**
     * Returns snapshot image.
     *
     * @returns Snapshot bitmap or NULL if TakeSnapshotL is not called or it failed.
     *          Caller takes ownership of the bitmap.
     */
    virtual CFbsBitmap* SnapshotBitmap() = 0;

    /**
     * Returns encoded snapshot image
     *
     * @return Encoded snapshot image or NULL if TakeSnapshotL is not called or if it failed
     *         Caller takes ownership of the pointer.
     */
    virtual HBufC8* SnapshotEncoded() = 0;

    // No data in MClass!!

};

#endif // MMMASNAPSHOT_H
