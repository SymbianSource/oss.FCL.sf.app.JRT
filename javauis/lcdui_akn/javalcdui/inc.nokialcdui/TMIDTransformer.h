/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used to get basic manipulations to the bitmaps.
*
*/


#ifndef TMIDTRANSFORMER_H
#define TMIDTRANSFORMER_H


//  CONSTANTS

// horizontal image flipping
const TInt KFlipHorizontal = 0x2000;

//vertical image flipping
const TInt KFlipVertical = 0x4000;

// rotating image 90 degrees counter-clockwise
const TInt KRotate90 = 90;

// rotating image 180 degrees counter-clockwise.
const TInt KRotate180 = 180;

// rotating image 270 degrees counter-clockwise.
const TInt KRotate270 = 270;

const TInt KRotates = 0x0FFF;
const TInt KFlips = 0xF000;


//  CLASS DEFINITION
/**
 * This class is used to get basic manipulations to the bitmaps.
 *
 */

NONSHARABLE_CLASS(TMIDTransformer)
{
public:
    void Transform(TRect& aDrawRect, TInt aManipulation);

    TInt Transform(const TPoint& aMovement);

    inline void NextY();
    inline void NextX();
private:
    TSize iSize;
    TInt iTransform;

public:
    TPoint iBitPosition;
    TPoint iPoint;
    TPoint iXMove;
    TPoint iYMove;
};

inline void TMIDTransformer::NextY()
{
    iBitPosition += iYMove;
    iPoint = iBitPosition;

}

inline void TMIDTransformer::NextX()
{
    iPoint += iXMove;
}

#endif // TMIDTRANSFORMER_H
