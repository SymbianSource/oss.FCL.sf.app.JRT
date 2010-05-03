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
* Description:  Color constants.
*
*/



#ifndef NATIVECOLORS_H
#define NATIVECOLORS_H

const TUint8 KNativeColors[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                                   0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB,
                                   0xCC, 0xDD, 0xEE, 0xFF
                                 };

const TUint8 KBits[ 8 ] = { 0x80, 0x40, 0x20, 0x10,
                            0x08, 0x04, 0x02, 0x01
                          };

const TUint32 KBits32[ 32 ] =
{
    1 << 31,
    1 << 30,
    1 << 29,
    1 << 28,
    1 << 27,
    1 << 26,
    1 << 25,
    1 << 24,
    1 << 23,
    1 << 22,
    1 << 21,
    1 << 20,
    1 << 19,
    1 << 18,
    1 << 17,
    1 << 16,
    1 << 15,
    1 << 14,
    1 << 13,
    1 << 12,
    1 << 11,
    1 << 10,
    1 << 9,
    1 << 8,
    1 << 7,
    1 << 6,
    1 << 5,
    1 << 4,
    1 << 3,
    1 << 2,
    1 << 1
};

const TUint8 KFourLeftBits = 0xF0;

const TInt KWhiteBlackSumRGBMidValue = 0x17D;

const TUint8 KAlphaFullOpaque = 0xFF;
const TUint8 KAlphaFullTransparent = 0x00;

const TUint8 KPixelsInByte = 8;

#define KOpaque TMIDInternalARGB( KAlphaFullOpaque, KAlphaFullOpaque, KAlphaFullOpaque, KAlphaFullOpaque )

const TUint8 KMIDEightRightBits = 0xFF;

const TInt KMIDShift3Bits = 3;
const TInt KMIDShift4Bits = 4;
const TInt KMIDShift5Bits = 5;
const TInt KMIDShift8Bits = 8;
const TInt KMIDShift12Bits = 12;
const TInt KMIDShift16Bits = 16;

#endif // NATIVECOLORS_H
