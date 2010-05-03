/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for buffer processor.
*
*/


#ifndef CMIDBUFFERPROCESSOR_H
#define CMIDBUFFERPROCESSOR_H


#include <lcdui.h>
#include "CMIDImage.h"
#include "TMIDBitmapParameters.h"

// FORWARD DECLARATIONS

class CMIDDirectGraphics;
class CMIDGraphics;
class CMIDToolkit;

NONSHARABLE_CLASS(CMIDBufferProcessor): public CBase,
        public MMIDBufferProcessor, public MMIDComponent
{
public: // Construction
    static void NewL(CMIDToolkit* aToolkit, TInt* aHandle,
    CMIDGraphics* aGraphics, MMIDImage* aImage);

public: // Destructor
    ~CMIDBufferProcessor();

public: // From MMIDComponent
    void Dispose();
    TType Type() const;
    MMIDBufferProcessor* Processor();

public:// From MMIDBufferProcessor
#ifdef RD_JAVA_NGA_ENABLED
    TBool ProcessL(const TMIDBufferOp*& aBegin,
                   const TMIDBufferOp* aEnd, TInt& aCycles, java::util::Monitor* aMonitor);
#else
    TBool ProcessL(const TMIDBufferOp*& aBegin,
                   const TMIDBufferOp* aEnd, TInt& aCycles, TRequestStatus* aStatus = NULL);
#endif
    void  AbortAsync();

public: // data
    // Owned
    CMIDDirectGraphics* iDirectGraphics;
    // Not owned
    CMIDGraphics* iGraphics;

protected: // Construction
    CMIDBufferProcessor();
    void ConstructL(CMIDGraphics* aGraphics, MMIDImage* aImage);

private:

    enum
    {
        EDrawTriangle = 102,
        EFillTriangle = 103,
        EDrawImage = 104
    };

    class TClipData
    {
    public:
        TPoint iTl;
        TSize iSize;
    };

    class TTriangleData
    {
    public:
        TInt x1;
        TInt y1;
        TInt x2;
        TInt y2;
        TInt x3;
        TInt y3;
        TInt color;
    };
    class TDirectImageData
    {
    public:
        TInt    iImage;
        TPoint  iPoint;
        TInt    iAnchor;
        TInt    iManipulation;
    };

    inline const TTriangleData& TriangleData(const TMIDBufferOp* aOp);
    inline const TRect RectData(const TMIDBufferOp* aOp);
    inline void UnlockedBitmapParameters(TMIDBitmapParameters& aParams,
                                         const TMIDBufferOp* aOp);
    inline void LockedBitmapParameters(TMIDBitmapParameters& aParams);

    /**
     * Draws image
     * @param aRead contains the image information
     * @param aUtil is used for locking the target bitmap
     */
    void DrawImageL(const TMIDBufferOp* aRead,
                    TBitmapUtil& aUtil);

    /**
     * Draws image. All bitmaps must be locked before calling this.
     * @param aParams contains the image information
     * @param aSourceUtil is used for locking the source bitmap
     * @param aTargetUtil is used for locking the target bitmap
     */
    void LockedDrawImageL(TMIDBitmapParameters& aParams,
                          TBitmapUtil& aSourceUtil,
                          TBitmapUtil& aTargetUtil);


    void DrawTriangle(const TTriangleData& aTriangle);
    void FillTriangle(const TTriangleData& aTriangle);
    void SetClipRect(const TRect& aRect);
};

template<class T>
inline const T& OpData(const TMIDBufferOp* aOp)
{
    ASSERT(aOp->Size() >= TInt(1 + (sizeof(T)/sizeof(TMIDBufferOp))));
    return *static_cast<const T*>(aOp->Data());
}

inline const CMIDBufferProcessor::TTriangleData&
CMIDBufferProcessor::TriangleData(const TMIDBufferOp* aOp)
{
    return OpData<CMIDBufferProcessor::TTriangleData>(aOp);
}

inline void
CMIDBufferProcessor::UnlockedBitmapParameters(TMIDBitmapParameters& aParams,
        const TMIDBufferOp* aOp)
{
    TDirectImageData imageData = OpData<CMIDBufferProcessor::TDirectImageData>(aOp);

    MMIDImage* image = MIDUnhandObject< MMIDImage >(imageData.iImage);

    // note: getting bitmap image increases its reference count
    MMIDBitmapImage* bitmapImage = image->BitmapImage();

    aParams.iBitmap = bitmapImage->ColorBitmap();
    CFbsBitmap* alphaBitmap = bitmapImage->AlphaBitmap();
    if (alphaBitmap)
    {
        aParams.iTransparency = ETrue;
        alphaBitmap->LockHeap();
        aParams.iAlpha = alphaBitmap->DataAddress();
        aParams.iAlphaMode = alphaBitmap->DisplayMode();
        TSize bitmapSize(alphaBitmap->SizeInPixels());
        TInt scanLineLength =
            CFbsBitmap::ScanLineLength(bitmapSize.iWidth,
                                       alphaBitmap->DisplayMode());
        TInt bitsPerPixel = alphaBitmap->Header().iBitsPerPixel;
        scanLineLength = (scanLineLength << 3) / bitsPerPixel;
        aParams.iAlphaSize = (scanLineLength * bitmapSize.iHeight *
                              bitsPerPixel) >> 3;
        alphaBitmap->UnlockHeap();
    }

    aParams.iManipulation = imageData.iManipulation;
    aParams.iAnchor = imageData.iAnchor;
    aParams.iX = imageData.iPoint.iX;
    aParams.iY = imageData.iPoint.iY;

    bitmapImage->RemoveRef();
}

inline void
CMIDBufferProcessor::LockedBitmapParameters(TMIDBitmapParameters& aParams)
{
    TSize bitmapSize(aParams.iBitmap->SizeInPixels());
    aParams.iWidth = bitmapSize.iWidth;
    aParams.iHeight = bitmapSize.iHeight;

    aParams.iScanLength =
        CFbsBitmap::ScanLineLength(bitmapSize.iWidth,
                                   aParams.iBitmap->DisplayMode());
    aParams.iScanLength = (aParams.iScanLength << 3) /
                          aParams.iBitmap->Header().iBitsPerPixel;

    aParams.iPixels = aParams.iBitmap->DataAddress();
    aParams.iPixelsSize = (aParams.iScanLength *
                           bitmapSize.iHeight *
                           aParams.iBitmap->Header().iBitsPerPixel) >> 3;

}

inline const TRect CMIDBufferProcessor::RectData(const TMIDBufferOp* aOp)
{
    const TClipData& clipData = OpData< TClipData >(aOp);
    const TRect clipRect(clipData.iTl, clipData.iSize);
    return clipRect;
}

#endif // CMIDBUFFERPROCESSOR_H
