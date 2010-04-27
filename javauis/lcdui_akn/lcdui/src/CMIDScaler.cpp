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

#include <e32cmn.h>
#include <j2me/jdebug.h>

#include "CMIDScaler.h"

#define REL_MODE

#ifdef REL_MODE
#include "CMIDMenuHandler.h" // For TlsStruct
#else
struct TlsStruct
{
    CMIDScaler* iScaler;
};

TlsStruct* tls = NULL;
#endif

/*
 * Internal definitions.
 */
#define INT_PRECISION 12 // !Max 12!
#define INT_FULL_PXL (1<<INT_PRECISION)
#define INT_HALF_PXL (1<<(INT_PRECISION-1))

/*
 * Internal macros.
 */
#define SET_SWI_BIT(x) ((x) | 0x8000)
#define CLR_SWI_BIT(x) ((x) & 0x7fff)
#define TST_SWI_BIT(x) (((x) & 0x8000) != 0)

#if !defined (__WINS__) && !defined (__WINSCW__)
#define ARMV5_OPTIMIZATIONS
#endif


#if defined(ARMV5_OPTIMIZATIONS)
void InterpolateOne2Four_kernel(const TUint32* in,
                                TUint32* out,
                                TUint32 width,
                                TUint32 height,
                                TUint32 offset);
#endif
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDScaler* CMIDScaler::NewL()
{
    // For Some reason codescanner sets error "error: leave: high: canpanic:
    // leaving function called in non-leaving function" even though this is
    // a leaving function!

    CMIDScaler* sca = new(ELeave) CMIDScaler();  // codescanner::leave
    CleanupStack::PushL(sca);   // codescanner::leave
    sca->ConstructL(); // codescanner::leave
    CleanupStack::Pop(sca);
    return sca;
}

// ---------------------------------------------------------------------------
// To delete this object.
//
// @Preconditions The following conditions must be true prior the calling this
// method; otherwise, its operation is undefined.
// - None
//
// @Postconditions The following conditions are true immediately after
// returning from this method, if the bitmap is processed.
// - None
//
// @param None.
// @return None.
// @exception None.
//
// ---------------------------------------------------------------------------
//
CMIDScaler::~CMIDScaler()
{
    iLock.Close();
    delete iBmap;
}

// ---------------------------------------------------------------------------
// To interpolate an input image.
//
// @Definition This method interpolates the input bitmap. It returns the
// reference to the interpolated bitmap or NULL, if it is not able to
// process the input bitmap for some reason. The ownership of the output
// bitmap is not change.
//
// This method is synchronized.
//
// @Preconditions The following conditions must be true prior the calling this
// method; otherwise, its operation is undefined.
// - None
//
// @Postconditions The following conditions are true immediately after
// returning from this method, if the bitmap is processed.
// - aImgIn is untouched.
// - iBmap.size == outWidth*outHeight.
// - iBmap.mode == aMode.
//
// @param aMode - A display mode.
// @param aImgIn - A pointer to the input image.
// @param aWidth - A width of an input image.
// @param aHeight - A height of an input image.
// @param aOffset - A offset of an input image: line = offset + width.
// @param aOutWidth - A width of an output image.
// @param aOutWeight - A height of an output image.
// @return Reference to the interpolated bitmap, or NULL.
// @exception None.
//
// ---------------------------------------------------------------------------
//
CFbsBitmap* CMIDScaler::Process(TDisplayMode aMode,
                                const TUint32* aImgIn,
                                TUint aWidth,
                                TUint aHeight,
                                TUint aOffset,
                                TUint aOutWidth,
                                TUint aOutHeight)
{
    // Debug macros.
    //__UHEAP_MARK;

    DEBUG_INT("CMIDScaler::Process - mod: %d", aMode);
    DEBUG_INT("CMIDScaler::Process - wid: %d", aWidth);
    DEBUG_INT("CMIDScaler::Process - hei: %d", aHeight);
    DEBUG_INT("CMIDScaler::Process - off: %d", aOffset);
    DEBUG_INT("CMIDScaler::Process - owi: %d", aOutWidth);
    DEBUG_INT("CMIDScaler::Process - ohe: %d", aOutHeight);
    DEBUG_INT("CMIDScaler::Process - maX: %d", iMaxX);
    DEBUG_INT("CMIDScaler::Process - maY: %d", iMaxY);
    DEBUG_INT("CMIDScaler::Process - inX: %d", iIntX);
    DEBUG_INT("CMIDScaler::Process - inY: %d", iIntY);

    CFbsBitmap* bmap = NULL;

    // Wait for the access to the lock object.
    iLock.Wait();

    if ((aMode == EColor16MU || aMode == EColor16MA) &&
            aWidth >= 2 &&
            aWidth <= aOutWidth &&
            aWidth <= iMaxX &&
            aHeight >= 2 &&
            aHeight <= aOutHeight &&
            aHeight <= iMaxY &&
            aOutWidth <= iIntX &&
            aOutHeight <= iIntY)
    {
        TInt err = KErrNone;

        // If this is the first call of this method, create new bitmap.
        if (iIsBm == EFalse)
        {
            iBmap = new CFbsBitmap;
            if (iBmap)
            {
                err = iBmap->Create(TSize(aOutWidth, aOutHeight), aMode);
                if (err != KErrNone)
                {
                    delete iBmap;
                    iBmap = NULL;
                }
                else
                {
                    iIsBm = ETrue;
                }
            }
        }

        // Do interpolation.
        if (iIsBm)
        {
            if (aOutWidth == 2*aWidth && aOutHeight == 2*aHeight)
            {
                InterpolateOne2Four(aImgIn, aWidth, aHeight, aOffset, aMode);
            }
            else
            {
                Interpolate(aImgIn, aWidth, aHeight, aOffset, aOutWidth, aOutHeight, aMode);
            }
            bmap = iBmap;
        }
    }

    // Release the lock object.
    iLock.Signal();

    // Debug macros.
    //__UHEAP_MARKEND;

    return bmap;
}

// ---------------------------------------------------------------------------
// To do 1st phase construction for this object.
//
// @Preconditions The following conditions must be true prior the calling this
// method; otherwise, its operation is undefined.
// - None
//
// @Postconditions The following conditions are true immediately after
// returning from this method, if the bitmap is processed.
// - None
//
// @param None.
// @return None.
// @exception None.
//
// ---------------------------------------------------------------------------
//
CMIDScaler::CMIDScaler()
{
    iBpp  = 4;
    iRefC = 0;
    iMode = EColor16MU;
    iMaxX = iIntX = 640;
    iMaxY = iIntY = 640;
    iIsBm = EFalse;
    iLock.CreateLocal();
}

// ---------------------------------------------------------------------------
// To do 2nd phase construction for this object.
//
// @Preconditions The following conditions must be true prior the calling this
// method; otherwise, its operation is undefined.
// - None
//
// @Postconditions The following conditions are true immediately after
// returning from this method, if the bitmap is processed.
// - None
//
// @param None.
// @return None.
// @exception If the method is not able to allocate necessary buffers.
//
// ---------------------------------------------------------------------------
//
void CMIDScaler::ConstructL()
{
}


// ---------------------------------------------------------------------------
// To do differential interpolation for an input image.
//
// @Definition The differential interpolation is based on the equation
//
//    o = | a, if l = dist(o,a)
//        | b, if l = dist(o,b)
//        | c, if l = dist(o,c)
//        | d, if l = dist(o,d)
//
// where o is an output pixel,
//
//    l = min(dist(o,a), dist(o,b), dist(o,c), dist(o,d))
//
// and dist is a component metric, i.e., dist(x,y) = |x1-y1| + |x2-y2|.
//
// Lets assume that we have to interpolate one output pixel o. It has always
// four surrounding input pixels orientated as depicted in Figure 1:
//
//    a<------x>+<------->b
//     |      : :        |
//     |      : :        |
//     |   A1 : :   A2   |
//     |      : :        |
//     |      : :        |
//     +------:-+--------+
//     |      : :        |
//     y......o :        |
//     |   A3   :   A4   |
//     |        :        |
//     |        :        |
//    c<------->+<------->d
//
// Figure 1. An output pixel o is always surrounded by four input pixels, a, b,
//           c and d. All pixels in specific area will have same value, e.g.,
//           pixel o in area A3 will have same value as c.
//
// It is obvious that c is the nearest pixel to o in Figure 1. Therefore o will
// have value of c.
//
// The basic problem of interpolation is how the last pixels are interpolated.
// Here the last row and column are replicated (look Figure 2), so the last
// pixels are interpolated same way as the others.
//
// :    :    :  :  :    :    :    :
// +----+----+ ... +----+----+.....
// | v0 | v1 | ... |vN-2|vN-1|vN-1:
// +----+----+ ... +----+----+.....
// : v0 : v1 : ... :vN-2:vN-1:vN-1:<- Last pseudo row
// ........... ... ................
//                              |
//                              +- Last pseudo column
//
// Figure 2. Pseudo pixels are added in order to get simple implementation.
//
// @Preconditions The following conditions must be true prior the calling this
// method; otherwise, its operation is undefined.
// - 2 <= width <= maxX
// - 2 <= height
// - width <= outWidth
// - height <= outHeight
// - outWidth <= intX
// - outHeight <= intY
// - log2(INT_FULL_PXL*width) <= 32
// - log2(INT_FULL_PXL*height) <= 32
// - aMode == EColor16MU || EColor16MA
// - IsCompressedInRAM == EFalse
//
// @Postconditions The following conditions are true immediately after
// returning from this method.
// - iBmap.size == outWidth*outHeight.
// - iBmap.mode == aMode.
//
// @param imgIn - A pointer to the input image.
// @param width - A width of an input image.
// @param height - A height of an input image.
// @param offset - A offset of an input image: line = offset + width.
// @param outWidth - A width of an output image.
// @param outWeight - A height of an output image.
// @param aMode - A display mode.
// @return None.
// @exception None.
//
// @Note This method has originally been a bilinear interpolation. Hence the
// element of the weight tables has 16 bits, but here only two of them are
// used.
//
// ---------------------------------------------------------------------------
//
void CMIDScaler::Interpolate(const TUint32* aImgIn,
                             TUint width,
                             TUint height,
                             TUint offset,
                             TUint outWidth,
                             TUint outHeight,
                             TDisplayMode aMode)
{

    TUint32 stepX, stepY, roundX, roundY, j, k;
    const TUint32 *src;
    TUint32 *dst;


    if (aMode != iBmap->DisplayMode())
        iBmap->SetDisplayMode(aMode);
    if ((iBmap->SizeInPixels().iWidth != outWidth)
            || (iBmap->SizeInPixels().iHeight != outHeight))
        iBmap->Resize(TSize(outWidth, outHeight));

    stepX = (TUint32)((16384*width)/outWidth);
    stepY = (TUint32)((16384*height)/outHeight);
    roundX = stepX>>1;
    roundY = stepY>>1;
    iBmap->LockHeap();
    dst = iBmap->DataAddress();

    for (j = 0; j < outHeight; j++)
    {
        // Interpolate one scanline.
        src = &aImgIn[(width+offset)*((stepY*j + roundY)>>14)];
        for (k = roundX; k < outWidth*stepX + roundX; k+=stepX)
        {
            *dst++ = src[k>>14];
        }
    }
    iBmap->UnlockHeap();

}

// ---------------------------------------------------------------------------
// To double scale the bitmap.
//
// @Preconditions The following conditions must be true prior to calling
// this method; otherwise, its operation is undefined.
// - width(src) > 0
// - height(src) > 0
// - width(dst) == 2*width(src)
// - height(dst) == 2*height(src)
// - IsCompressedInRAM == EFalse
// - DisplayMode() == EColor16MU || EColor16MA
//
// @Postconditions The following condition is true immediately after
// returning from this method.
// - iBmap.size == outWidth*outHeight.
// - iBmap.mode == aMode.
//
// @param aImgIn - A pointer to the input image.
// @param aWidth - A width of an input image.
// @param aHeight - A height of an input image.
// @param aOffset - A offset of an input image: line = offset + width.
// @param aMode - A display mode.
// @return None.
// @exception None.
// ---------------------------------------------------------------------------
//
void CMIDScaler::InterpolateOne2Four(const TUint32* aImgIn,
                                     TUint aWidth,
                                     TUint aHeight,
                                     TUint aOffset,
                                     TDisplayMode aMode)
{
    if (aMode != iBmap->DisplayMode())
        iBmap->SetDisplayMode(aMode);
    if ((iBmap->SizeInPixels().iWidth != 2*aWidth)
            || (iBmap->SizeInPixels().iHeight != 2*aHeight))
        iBmap->Resize(TSize(2*aWidth, 2*aHeight));

    iBmap->LockHeap();
    TUint32* tmp1 = iBmap->DataAddress();

    // Do double scaling.
#if defined(ARMV5_OPTIMIZATIONS)
    InterpolateOne2Four_kernel(aImgIn, tmp1, aWidth, aHeight, aOffset);
#else
    // Initialize variables.
    TInt32 x = 0;
    TInt32 y = 0;
    TInt32 i = 0;
    TUint32 pxl = 0;
    const TUint32* src1 = NULL;
    TUint32* dst1 = NULL;
    TUint32* dst2 = NULL;

    for (y = 0; y < aHeight; y++)
    {
        i = (aWidth + aOffset)*y;

        src1 = aImgIn;
        dst1 = dst2 = tmp1;
        src1 = &src1[i];
        dst1 = &dst1[2*aWidth*2*y];
        dst2 = &dst2[2*aWidth*(2*y+1)];

        // Copy one scanline.
        for (x = 0; x < aWidth; x++)
        {
            pxl = *src1++;
            *dst1++ = pxl;
            *dst1++ = pxl;
            *dst2++ = pxl;
            *dst2++ = pxl;
        }
    }
#endif
    iBmap->UnlockHeap();

}


#if defined(ARMV5_OPTIMIZATIONS)
__asm void InterpolateOne2Four_kernel(const TUint32* in,
                                      TUint32* out,
                                      TUint32 width,
                                      TUint32 height,
                                      TUint32 offset)
{
// r0 = in
// r1 = out
// r2 = width
// r3 = height
// sp[0] = offset

    CODE32

    CMP     r2, #0
    BXEQ    lr
    CMP     r3, #0
    BXEQ    lr

    STMFD   sp!, {r4-r12, lr}

    MOV     r12, r2                 // width
    ADD     r14, r1, r2, LSL #3     // &out[2*width]

CMIDScaler_InterpolateOne2Four_kernel_outerLoop
        MOVS    r2, r12, ASR #2         // width/4 = number of 4x32 bit reads

CMIDScaler_InterpolateOne2Four_kernel_innerLoop
// read the 4x32bit-aligned input and interpolate
    LDMIANE r0!, {r4, r6, r8, r10}
    MOVNE   r5, r4
    MOVNE   r7, r6
    MOVNE   r9, r8
    MOVNE   r11, r10
    STMIANE r1!, {r4-r11}
    STMIANE r14!, {r4-r11}
    SUBSNE  r2, #1
    BNE     CMIDScaler_InterpolateOne2Four_kernel_innerLoop

    MOV     r2, r12, ASR #2
    SUBS    r2, r12, r2, LSL #2     // number of unaligned 32-bit reads

// read the unaligned tail and interpolate
CMIDScaler_InterpolateOne2Four_kernel_tailLoop

    LDRNE   r6, [r0], #4
    STRNE   r6, [r1], #4
    STRNE   r6, [r1], #4
    STRNE   r6, [r14], #4
    STRNE   r6, [r14], #4
    SUBSNE  r2, #1
    BNE     CMIDScaler_InterpolateOne2Four_kernel_tailLoop

// update pointers to the next line
    LDR     r4, [sp, #(10*4)]
    SUBS    r3, #1
    MOV     r1, r14
    ADD     r14, r1, r12, LSL #3
    ADD     r0, r0, r4, LSL #2
    BNE     CMIDScaler_InterpolateOne2Four_kernel_outerLoop

    LDMFD   sp!, {r4-r12, lr}
    BX      lr
}
#endif
