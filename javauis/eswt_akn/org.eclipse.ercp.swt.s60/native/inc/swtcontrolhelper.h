/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTCONTROLHELPER_H
#define SWTCONTROLHELPER_H


#include <coedef.h>
#include <eiksbobs.h>
#include <w32std.h>
#include "eswtwidgetscore.h"


class CCoeControl;
class CEikScrollBarFrame;
class CSwtScrollBar;
class CWindowGc;


/**
 * SwtControlHelper
 * @lib eswt
 */
NONSHARABLE_CLASS(SwtControlHelper)
{
// Methods
public:
    /**
     * Copies a bitmap.
     * @param aBitmap The source bitmap for copy operation.
     * @param aTargetSize Optional target size if scaling is desired.
     * @return Pointer to the copy. Copy is owned by the caller.
     */
    static CFbsBitmap* GetCopyOfBitmapLC(const CFbsBitmap* aBitmap,
                                         const TSize& aTargetSize = TSize(-1, -1));

    /**
     * Copies a bitmap.
     * @param aBitmap The source bitmap for copy operation.
     * @param aTargetSize Optional target size if scaling is desired.
     * @return Pointer to the copy. Copy is owned by the caller.
     */
    static CFbsBitmap* GetCopyOfBitmapL(const CFbsBitmap* aBitmap,
                                        const TSize& aTargetSize = TSize(-1, -1));

    /**
     * Copies a bitmap inverting it in the case it's monochrome.
     * @param aBitmap The source bitmap for copy operation.
     * @param aTargetSize Optional target size if scaling is desired.
     * @return Pointer to the copy. Copy is owned by the caller.
     */
    static CFbsBitmap* GetInvertedCopyOfMonoBitmapLC(const CFbsBitmap* aBitmap,
            const TSize& aTargetSize = TSize(-1, -1));

    /**
     * Copies a bitmap inverting it in the case it's monochrome.
     * @param aBitmap The source bitmap for copy operation.
     * @param aTargetSize Optional target size if scaling is desired.
     * @return Pointer to the copy. Copy is owned by the caller.
     */
    static CFbsBitmap* GetInvertedCopyOfMonoBitmapL(const CFbsBitmap* aBitmap,
            const TSize& aTargetSize = TSize(-1, -1));

    /**
     * Retrieves the clipboard's text content.
     * It is up to the caller to free the descriptor by calling
     * CleanupStack::PopAndDestroy() if the descriptor's
     * pointer is not NULL.
     * @return A descriptor containing the clipboard's content.
     */
    static TPtr GetClipboardTextContentLC();

    /**
     * Returns size for a bitmap that is aspect ratio scaled to fit inside
     * aMaxDestSize if aSourceSize > aMaxDestSize
     * @param aSourceSize The source bitmap size.
     * @param aMaxDestSize Maximum size for the bitmap.
     * @return Resulting size. If any scaling was needed, it is done so
     *         that the aspect ratio does not change.
     */
    static TSize GetAspectRatioScaledBitmapSize(const TSize& aSourceSize,
            const TSize& aMaxDestSize);
};


#endif // SWTCONTROLHELPER_H
