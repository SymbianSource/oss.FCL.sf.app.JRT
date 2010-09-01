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
* Description:  Native side interface for DirectGraphics java class.
*
*/


#ifndef CMIDDIRECTGRAPHICS_H
#define CMIDDIRECTGRAPHICS_H

// INCLUDES
#include <e32base.h>

#include "TMIDTriangleFiller.h"
#include "TMIDInternalARGB.h"

// CONSTANTS
const TInt KDottedLine = 1;
const TInt KDottedLineStep = 4;

// FORWARD DECLARATIONS
class CPolygonFiller;
class CMIDConvertFactory;
class CMIDNativeConverter;
class TMIDBitmapParameters;
#ifdef RD_JAVA_NGA_ENABLED
class MMIDCanvas;
#endif

//  CLASS DEFINITION
/**
 * This class is native side interface for DirectGraphics java class.
 *
 */
NONSHARABLE_CLASS(CMIDDirectGraphics): public CBase
{
public: // polygon drawing parameter
    class TPolygonParameters
    {
    public:

        TInt* aXPoints;
        TInt aXPointsLength;
        TInt aXOffset;
        TInt aTransX;
        TInt* aYPoints;
        TInt aYPointsLength;
        TInt aYOffset;
        TInt aTransY;
        TInt aNumberOfPoints;
        TInt aARGBColor;
        TBool aFill;
    };

public: // Construction
    static CMIDDirectGraphics* NewL(CMIDGraphics* aGraphics,
                                    MMIDImage* aImage);

public: // Destruction
    ~CMIDDirectGraphics();

public: // New methods

    // Sets clip rect. This method is normally called from CMIDGraphics
    void SetClippingRect(TRect aRect);

    // Sets specified color to image. Includes transparency
    void SetColorL(TUint32 aColor, TInt aImageHandle);

    // Drawing and getting bitmaps
    void DrawBitmapL(TMIDBitmapParameters* aParams);
    void GetBitmapL(TMIDBitmapParameters* aParams);

    // Polygon drawing
    void DrawPolygonL(TPolygonParameters* aParams);

    // Triangle drawing
    void DrawTriangle(const TInt aX1, const TInt aY1, const TInt aX2, const TInt aY2,
                      const TInt aX3, const TInt aY3,
                      const TUint32 aColor);
    void FillTriangle(const TInt aX1, const TInt aY1, const TInt aX2, const TInt aY2,
                      const TInt aX3, const TInt aY3,
                      const TUint32 aColor);
    // Native pixel format
    inline TInt GetNativePixelFormat()
    {
        return iNativePixelFormat;
    }

protected:
    // protected construction
    CMIDDirectGraphics();
    void ConstructL(CMIDGraphics* aGraphics, MMIDImage* aImage);

private:
    void DrawLine(const TPoint& aPointA, const TPoint& aPointB,
                  const TMIDInternalARGB& aColor);

    TInt CalculateScanLength(const TInt aWidth,
                             const TDisplayMode aDisplayMode);

private: // data
    // screen size
    TSize iScreenSize;

    // clipping rect
    TRect iClipRect;

    // converter to bitmap
    CMIDNativeConverter* iNativeConverter;

    CPolygonFiller* iPolygonFiller;
    TBool iDottedLine;

    // not owned
    CMIDGraphics* iGraphics;

    TInt iNativePixelFormat;
    CMIDConvertFactory* iConverterFactory;

#ifdef RD_JAVA_NGA_ENABLED
    MMIDCanvas* iCanvasTarget;
#endif
};



#endif // CMIDDIRECTGRAPHICS_H
