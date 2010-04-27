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
* Description:  Triangle filler.
*
*/


//  Include Files
#include <e32std.h>
#include "TMIDTriangleFiller.h"


//  LOCAL CONSTANTS AND MACROS



//  LOCAL FUNCTIONS

void TMIDTriangleFiller::Construct(TInt aX1, TInt aY1,
                                   TInt aX2, TInt aY2,
                                   TInt aX3, TInt aY3)
{
    iX1 = aX1 << KShift;
    iY1 = aY1 << KShift;
    iX2 = aX2 << KShift;
    iY2 = aY2 << KShift;
    iX3 = aX3 << KShift;
    iY3 = aY3 << KShift;

    if (iY1 > iY2)
    {
        // Swap P1 and P2
        SwapPoints(iX1, iY1, iX2, iY2);
    }
    if (iY2 > iY3)
    {
        // Swap P2 and P3
        SwapPoints(iX2, iY2, iX3, iY3);
    }
    if (iY1 > iY2)
    {
        // Swap P1 and P2
        SwapPoints(iX1, iY1, iX2, iY2);
    }

    iNormalCase = EFalse;
    if (iY1 == iY2)
    {
        if (iY2 == iY3)
        {
            ConstructSingleLine();
        }
        else
        {
            ConstructSpecialCaseOne();
        }
    }
    else if (iY2 == iY3)
    {
        ConstructSpecialCaseTwo();
    }
    else
    {
        ConstructNormalCase();
    }
    iY1 = iY1 >> KShift;
}



void TMIDTriangleFiller::SecondHalf()
{
    if (iLeftDelta == iDx13)
    {
        iRightDelta = iDx23;
    }
    else
    {
        iLeftDelta = iDx23;
    }


    iScanlines = (iY3 - iY2) >> KShift;
    iNormalCase = EFalse;
}

void TMIDTriangleFiller::ConstructNormalCase()
{
    // The normal case:
    //      1
    //     /|
    //    / |    first half
    //   /  |                 dx12 < dx13
    // 2/___|____
    //  \_  |
    //    \_|    second half
    //      3

    // Calculate step values for edges
    iNormalCase = ETrue;
    iDx12 = (FPDIV((iX2 - iX1), (iY2 - iY1)));
    iDx23 = (FPDIV((iX3 - iX2), (iY3 - iY2)));
    iDx13 = (FPDIV((iX3 - iX1), (iY3 - iY1)));



    // Check which edge (13 or 12) is the left side edge
    if (iDx12 < iDx13)
    {
        iLeftDelta = iDx12;
        iRightDelta = iDx13;
    }
    else
    {
        iLeftDelta = iDx13;
        iRightDelta = iDx12;
    }



    // Initialize the span
    iSpanStart = iX1;
    iSpanStop = iX1;

    // Do the first half of the polygon ( 1-2 )
    iScanlines = (iY2 - iY1) >> KShift;
}

void TMIDTriangleFiller::ConstructSingleLine()
{
    iScanlines = 1;
    iSpanStart = Min(iX1, iX2);
    iSpanStart = Min(iSpanStart, iX3);

    iSpanStop = Max(iX1, iX2);
    iSpanStop = Max(iSpanStop, iX3);
}

void TMIDTriangleFiller::ConstructSpecialCaseOne()
{
    // Special case:
    //    1________2
    //     \      /
    //      \    /
    //       \  /
    //        \/
    //         3
    if (iX1 > iX2)
    {
        SwapPoints(iX1, iY1, iX2, iY2);
    }

    iDx13 = (FPDIV((iX3 - iX1), (iY3 - iY1)));
    iDx23 = (FPDIV((iX3 - iX2), (iY3 - iY2)));

    // check which edge is left edge
    if (iDx13 < iDx23)
    {
        iLeftDelta = iDx23;
        iRightDelta = iDx13;
    }
    else
    {
        iLeftDelta = iDx13;
        iRightDelta = iDx23;
    }

    // initialize span
    iSpanStart = iX1;
    iSpanStop = iX2;

    iScanlines = (iY3 - iY1) >> KShift;

}
void TMIDTriangleFiller::ConstructSpecialCaseTwo()
{
    /*
    // Special case:
    //     1
    //    /\
    //   /  \
    //  /    \
    // /      \
    //2--------3
    */

    // calculate slopes
    iDx12 = (FPDIV((iX2 - iX1), (iY2 - iY1)));
    iDx13 = (FPDIV((iX3 - iX1), (iY3 - iY1)));

    // check which edge is left edge
    if (iDx12 < iDx13)
    {
        iLeftDelta = iDx12;
        iRightDelta = iDx13;
    }
    else
    {
        iLeftDelta = iDx13;
        iRightDelta = iDx12;
    }

    // Initialize the span
    iSpanStart = iX1;// + iLeftDelta;
    iSpanStop = iX1;// + iRightDelta;

    iScanlines = (iY2 - iY1) >> KShift;
}

void TMIDTriangleFiller::GetNextPixelRun(TBool& aExists, TInt& aScanLine,
        TInt& aStart, TInt& aEnd)
{
    aScanLine = iY1++;
    aStart = (iSpanStart >> KShift) + 1;
    aEnd = iSpanStop >> KShift;

    iSpanStart += iLeftDelta;
    iSpanStop += iRightDelta;
    aExists = ETrue;
    if (--iScanlines < 1)
    {
        if (iNormalCase)
        {
            SecondHalf();
        }
        else
        {
            aExists = EFalse;
        }
    }
}
//  End of File

