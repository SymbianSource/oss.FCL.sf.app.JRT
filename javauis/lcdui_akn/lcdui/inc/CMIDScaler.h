/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CMIDSCALER_H
#define CMIDSCALER_H

#include <e32base.h>
#include <w32std.h>
#include <fbs.h>

class CMIDUIManager;

/**
 *
 */
NONSHARABLE_CLASS(CMIDScaler) : public CBase
{
public:

    friend class CMIDUIManager;

    /**
     * Double ctor. Creates a new object.
     *
     * @return CMIDScaler instance
     * @see CMIDUImanager::ReserveScalerL()
     * @since s60
     */
    static CMIDScaler* NewL();

    /**
     * To delete this object.
     *
     * @Preconditions The following conditions must be true prior the calling this
     * method; otherwise, its operation is undefined.
     * - None
     *
     * @Postconditions The following conditions are true immediately after
     * returning from this method, if the bitmap is processed.
     * - None
     *
     * @param None.
     * @return None.
     * @exception None.
     */
    ~CMIDScaler();

    /**
     * To interpolate an input image.
     *
     * @Definition This method interpolates the input bitmap. It returns the
     * reference to the interpolated bitmap or NULL, if it is not able to
     * process the input bitmap for some reason. The ownership of the output
     * bitmap is not change.
     *
     * This method is synchronized.
     *
     * @Preconditions The following conditions must be true prior the calling this
     * method; otherwise, its operation is undefined.
     * - None
     *
     * @Postconditions The following conditions are true immediately after
     * returning from this method, if the bitmap is processed.
     * - aImgIn is untouched.
     * - iBmap.size == outWidth*outHeight.
     * - iBmap.mode == aMode.
     *
     * @param aMode - A display mode.
     * @param aImgIn - A pointer to the input image.
     * @param aWidth - A width of an input image.
     * @param aHeight - A height of an input image.
     * @param aOffset - A offset of an input image: line = offset + width.
     * @param aOutWidth - A width of an output image.
     * @param aOutWeight - A height of an output image.
     * @return Reference to the interpolated bitmap, or NULL.
     * @exception None.
     */
    CFbsBitmap* Process(TDisplayMode aMode,
                        const TUint32* imgIn,
                        TUint width,
                        TUint height,
                        TUint offset,
                        TUint outWidth,
                        TUint outHeight);

private:
    /**
     * To do 1st phase construction for this object.
     *
     * @Preconditions The following conditions must be true prior the calling this
     * method; otherwise, its operation is undefined.
     * - None
     *
     * @Postconditions The following conditions are true immediately after
     * returning from this method, if the bitmap is processed.
     * - None
     *
     * @param None.
     * @return None.
     * @exception None.
     */
    CMIDScaler();

    /**
     * To do 2nd phase construction for this object.
     *
     * @Preconditions The following conditions must be true prior the calling this
     * method; otherwise, its operation is undefined.
     * - None
     *
     * @Postconditions The following conditions are true immediately after
     * returning from this method, if the bitmap is processed.
     * - None
     *
     * @param None.
     * @return None.
     * @exception If the method is not able to allocate necessary buffers.
     */
    void ConstructL();

    /**
     * To do differential interpolation for an input image.
     *
     * @Preconditions The following conditions must be true prior the calling this
     * method; otherwise, its operation is undefined.
     * - 2 <= width <= maxX
     * - 2 <= height
     * - width <= outWidth
     * - height <= outHeight
     * - outWidth <= intX
     * - outHeight <= intY
     * - log2(INT_FULL_PXL*width) <= 32
     * - log2(INT_FULL_PXL*height) <= 32
     * - aMode == EColor16MU || EColor16MA
     * - IsCompressedInRAM == EFalse
     *
     * @Postconditions The following conditions are true immediately after
     * returning from this method.
     * - iBmap.size == outWidth*outHeight.
     * - iBmap.mode == aMode.
     *
     * @param imgIn - A pointer to the input image.
     * @param width - A width of an input image.
     * @param height - A height of an input image.
     * @param offset - A offset of an input image: line = offset + width.
     * @param outWidth - A width of an output image.
     * @param outWeight - A height of an output image.
     * @param aMode - A display mode.
     * @return None.
     * @exception None.
     *
     * @Note This method has originally been a bilinear interpolation. Hence the
     * element of the weight tables has 16 bits, but here only two of them are
     * used.
     *
     */
    void Interpolate(const TUint32* aImgIn,
                     TUint aWidth,
                     TUint aHeight,
                     TUint aOffset,
                     TUint aOutWidth,
                     TUint aOutHeight,
                     TDisplayMode aMode);

    /**
     * To double scale the bitmap.
     *
     * @Preconditions The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - width(src) > 0
     * - height(src) > 0
     * - width(dst) == 2*width(src)
     * - height(dst) == 2*height(src)
     * - IsCompressedInRAM == EFalse
     * - DisplayMode() == EColor16MU || EColor16MA
     *
     * @Postconditions The following condition is true immediately after
     * returning from this method.
     * - iBmap.size == outWidth*outHeight.
     * - iBmap.mode == aMode.
     *
     * @param aImgIn - A pointer to the input image.
     * @param aWidth - A width of an input image.
     * @param aHeight - A height of an input image.
     * @param aOffset - A offset of an input image: line = offset + width.
     * @param aMode - A display mode.
     * @return None.
     * @exception None.
     */
    void InterpolateOne2Four(const TUint32* aImgIn,
                             TUint aWidth,
                             TUint aHeight,
                             TUint aOffset,
                             TDisplayMode aMode);

private:
    TUint16 iRefC;
    TUint16 iBpp;
    TDisplayMode iMode;
    TUint16 iMaxX;
    TUint16 iMaxY;
    TUint16 iIntX;
    TUint16 iIntY;
    HBufC8* iBuf0;
    HBufC8* iBuf1;
    CFbsBitmap* iBmap;
    TBool iIsBm;
    RMutex iLock;
};

#endif // CMIDSCALER_H
