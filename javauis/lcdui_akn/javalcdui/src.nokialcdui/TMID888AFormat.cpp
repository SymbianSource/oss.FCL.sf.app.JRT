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
* Description:  8 bits for red, green and blue component in a pixel.
*
*/


//  INCLUDE FILES
#include "TMID888AFormat.h"

void TMID888AFormat::InitializeL(const TMIDBitmapParameters& aParameters)
{
    TMIDFormatConverter::InitializeL(aParameters);
    iOffset += iTransformer.iPoint.iX + iTransformer.iPoint.iY * iScanlength;
    iBitmap = aParameters.iPixels;
    iAlphaBitmap32 = (TUint32*)aParameters.iAlpha;
}

TUint8 TMID888AFormat::GetAlpha() const
{
    // we need only last 8 bits
    return (TUint8)(iAlphaBitmap32[ iOffset ] & KMIDEightRightBits);      // CSI: 2 Wrong offset means implementation error #
}
