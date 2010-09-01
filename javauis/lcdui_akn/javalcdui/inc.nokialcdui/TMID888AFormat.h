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
* Description:  8 bits for red, green and blue component in a pixel.
*
*/


#ifndef TMID888AFORMAT_H
#define TMID888AFORMAT_H

//  INCLUDES
#include "TMID888Format.h"

//  CLASS DEFINITION
/**
 * 8 bits for red, green and blue component in a pixel (0x00RRGGBB).
 * The high order byte (alpha) is ignored by the implementation.
 * Instead a separate 32 bit alpha bitmap is used.
 *
 */
NONSHARABLE_CLASS(TMID888AFormat): public TMID888Format
{
public: // from TMID888Format

    // initialization
    void InitializeL(const TMIDBitmapParameters& aParameters);

    // Get alpha which is in same format as bitmap
    TUint8 GetAlpha() const;

private: // data
    TUint32* iAlphaBitmap32;
};

#endif // TMID888AFORMAT_H
