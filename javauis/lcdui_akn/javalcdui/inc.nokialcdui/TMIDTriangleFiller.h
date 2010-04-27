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
* Description:  Triangle filler.
*
*/



#ifndef TMIDTRIANGLEFILLER_H
#define TMIDTRIANGLEFILLER_H

//  INCLUDES
#include <e32def.h>


//  CONSTANTS
const TInt KShift = 8;

//  MACROS
#define FPDIV( x, y ) ( ( ( x ) << KShift ) / ( y ) )



//  CLASS DEFINITION
/**
 * Triangle filler.
 *
 */
NONSHARABLE_CLASS(TMIDTriangleFiller)    // usually not derived
{
public:     // Constructors and destructors
    void Construct(TInt aX1, TInt aY1,
                   TInt aX2, TInt aY2,
                   TInt aX3, TInt aY3);

public:     // New methods
    void GetNextPixelRun(TBool& aExists, TInt& aScanLine,
                         TInt& aStart, TInt& aEnd);

private:    // New methods

    void ConstructNormalCase();
    void ConstructSpecialCaseOne();
    void ConstructSpecialCaseTwo();
    void ConstructSingleLine();
    void SecondHalf();

    inline void SwapPoints(TInt& aX1, TInt& aY1,
                           TInt& aX2, TInt& aY2);

private:    // Data
    TInt iX1;
    TInt iY1;
    TInt iX2;
    TInt iY2;
    TInt iX3;
    TInt iY3;
    TInt iRightDelta;
    TInt iLeftDelta;
    TInt iScanlines;
    TInt iSpanStart;
    TInt iSpanStop;
    TInt iDx12;
    TInt iDx23;
    TInt iDx13;
    TBool iNormalCase;
};

void TMIDTriangleFiller::SwapPoints(TInt& aX1, TInt& aY1,
                                    TInt& aX2, TInt& aY2)
{
    TInt temp(aX1);
    aX1 = aX2;
    aX2 = temp;
    temp = aY1;
    aY1 = aY2;
    aY2 = temp;
};

#endif // TMIDTRIANGLEFILLER_H



