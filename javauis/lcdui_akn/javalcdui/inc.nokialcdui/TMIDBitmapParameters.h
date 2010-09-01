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
* Description:  Contains parameters for bitmap drawing.
*
*/


#ifndef TMIDBITMAPPARAMETERS_H
#define TMIDBITMAPPARAMETERS_H

//  INCLUDES
#include <e32std.h>


//  CLASS DEFINITION
/**
 * Class contains parameters for bitmap drawing, reason for own class
 * is that its easier to handle them in one class instead of passing
 * all separated.
 */

NONSHARABLE_CLASS(TMIDBitmapParameters)
{
public: // constructor
    TMIDBitmapParameters() :
            iOffset(0),
            iScanLength(0),
            iPixels(NULL),
            iPixelsSize(0),
            iAlpha(NULL),
            iAlphaSize(0),
            iX(0),
            iY(0),
            iWidth(0),
            iHeight(0),
            iManipulation(0),
            iFormat(0),
            iAnchor(0),
            iTransparency(ETrue),
            iClipRect(),
            iAlphaMode(ENone),
            iBitmap(NULL)   // ,
    {}


public: // data
    TInt iOffset;
    TInt iScanLength;

    TUint32* iPixels;
    TInt     iPixelsSize;
    TUint32* iAlpha;
    TInt     iAlphaSize;

    TInt iX;
    TInt iY;
    TInt iWidth;
    TInt iHeight;

    TInt iManipulation;

    TInt iFormat;
    TInt iAnchor;
    TBool iTransparency;

    TRect iClipRect;

    TDisplayMode iAlphaMode;

    // bitmap holding the image, not owned
    CFbsBitmap* iBitmap;

};

#endif // TMIDBITMAPPARAMETERS_H
