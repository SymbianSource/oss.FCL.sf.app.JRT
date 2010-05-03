/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.utils;

public class Base64
{
    private final static byte[] ENCODE_ALPHABET =
    {
        (byte)'A', (byte)'B', (byte)'C', (byte)'D', (byte)'E', (byte)'F', (byte)'G',
        (byte)'H', (byte)'I', (byte)'J', (byte)'K', (byte)'L', (byte)'M', (byte)'N',
        (byte)'O', (byte)'P', (byte)'Q', (byte)'R', (byte)'S', (byte)'T', (byte)'U',
        (byte)'V', (byte)'W', (byte)'X', (byte)'Y', (byte)'Z',
        (byte)'a', (byte)'b', (byte)'c', (byte)'d', (byte)'e', (byte)'f', (byte)'g',
        (byte)'h', (byte)'i', (byte)'j', (byte)'k', (byte)'l', (byte)'m', (byte)'n',
        (byte)'o', (byte)'p', (byte)'q', (byte)'r', (byte)'s', (byte)'t', (byte)'u',
        (byte)'v', (byte)'w', (byte)'x', (byte)'y', (byte)'z',
        (byte)'0', (byte)'1', (byte)'2', (byte)'3', (byte)'4', (byte)'5',
        (byte)'6', (byte)'7', (byte)'8', (byte)'9', (byte)'+', (byte)'/'
    };

    private final static byte[] DECODE_ALPHABET =
    {
        -9,-9,-9,-9,-9,-9,-9,-9,-9,                 // Decimal  0 -  8
        -5,-5,                                      // Whitespace: Tab and Linefeed
        -9,-9,                                      // Decimal 11 - 12
        -5,                                         // Whitespace: Carriage Return
        -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,     // Decimal 14 - 26
        -9,-9,-9,-9,-9,                             // Decimal 27 - 31
        -5,                                         // Whitespace: Space
        -9,-9,-9,-9,-9,-9,-9,-9,-9,-9,              // Decimal 33 - 42
        62,                                         // Plus sign at decimal 43
        -9,-9,-9,                                   // Decimal 44 - 46
        63,                                         // Slash at decimal 47
        52,53,54,55,56,57,58,59,60,61,              // Numbers zero through nine
        -9,-9,-9,                                   // Decimal 58 - 60
        -1,                                         // Equals sign at decimal 61
        -9,-9,-9,                                      // Decimal 62 - 64
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,            // Letters 'A' through 'N'
        14,15,16,17,18,19,20,21,22,23,24,25,        // Letters 'O' through 'Z'
        -9,-9,-9,-9,-9,-9,                          // Decimal 91 - 96
        26,27,28,29,30,31,32,33,34,35,36,37,38,     // Letters 'a' through 'm'
        39,40,41,42,43,44,45,46,47,48,49,50,51,     // Letters 'n' through 'z'
        -9,-9,-9,-9                                 // Decimal 123 - 126
    };


    private final static byte WHITE_SPACE_ENC = -5;
    private final static byte EQUALS_SIGN_ENC = -1;
    private final static byte EQUALS_SIGN = (byte)'=';

    public static byte[] decode(String s)
    {
        if (s == null)
        {
            return null;
        }
        byte[] source = s.getBytes();
        int    len34   = source.length * 3 / 4;
        byte[] outBuff = new byte[ len34 ];
        int    outBuffPosn = 0;

        byte[] b4        = new byte[4];
        int    b4Posn    = 0;
        int    i         = 0;
        byte   sbiCrop   = 0;
        byte   sbiDecode = 0;
        for (i = 0; i < source.length; i++)
        {
            sbiCrop = (byte)(source[i] & 0x7f);
            sbiDecode = DECODE_ALPHABET[ sbiCrop ];

            if (sbiDecode >= WHITE_SPACE_ENC)
            {
                if (sbiDecode >= EQUALS_SIGN_ENC)
                {
                    b4[ b4Posn++ ] = sbiCrop;
                    if (b4Posn > 3)
                    {
                        outBuffPosn += decode4to3(b4, 0, outBuff, outBuffPosn);
                        b4Posn = 0;

                        if (sbiCrop == EQUALS_SIGN)
                            break;
                    }
                }
            }
            else
            {
                return null;
            }
        }
        if (outBuffPosn == 0)
        {
            return null;
        }
        byte[] out = new byte[ outBuffPosn ];
        System.arraycopy(outBuff, 0, out, 0, outBuffPosn);
        return out;
    }

    public static String encode(byte[] source)
    {
        if (source == null)
        {
            return null;
        }
        int len = source.length;
        int    len43   = len * 4 / 3;
        byte[] outBuff = new byte[(len43) + ((len % 3) > 0 ? 4 : 0)];
        int d = 0;
        int e = 0;
        int len2 = len - 2;
        int lineLength = 0;
        for (; d < len2; d+=3, e+=4)
        {
            encode3to4(source, d, 3, outBuff, e);
            lineLength += 4;
        }
        if (d < len)
        {
            encode3to4(source, d, len - d, outBuff, e);
            e += 4;
        }
        if (outBuff.length == 0)
        {
            return null;
        }
        return new String(outBuff, 0, e);
    }

    private static int decode4to3(byte[] source, int srcOffset, byte[] destination, int destOffset)
    {
        if (source[ srcOffset + 2] == EQUALS_SIGN)
        {
            int outBuff = ((DECODE_ALPHABET[ source[ srcOffset    ] ] & 0xFF) << 18)
                          | ((DECODE_ALPHABET[ source[ srcOffset + 1] ] & 0xFF) << 12);
            destination[ destOffset ] = (byte)(outBuff >>> 16);
            return 1;
        }
        else if (source[ srcOffset + 3 ] == EQUALS_SIGN)
        {
            int outBuff = ((DECODE_ALPHABET[ source[ srcOffset     ] ] & 0xFF) << 18)
                          | ((DECODE_ALPHABET[ source[ srcOffset + 1 ] ] & 0xFF) << 12)
                          | ((DECODE_ALPHABET[ source[ srcOffset + 2 ] ] & 0xFF) <<  6);
            destination[ destOffset     ] = (byte)(outBuff >>> 16);
            destination[ destOffset + 1 ] = (byte)(outBuff >>>  8);
            return 2;
        }
        else
        {
            try
            {
                int outBuff = ((DECODE_ALPHABET[ source[ srcOffset     ] ] & 0xFF) << 18)
                              | ((DECODE_ALPHABET[ source[ srcOffset + 1 ] ] & 0xFF) << 12)
                              | ((DECODE_ALPHABET[ source[ srcOffset + 2 ] ] & 0xFF) <<  6)
                              | ((DECODE_ALPHABET[ source[ srcOffset + 3 ] ] & 0xFF));
                destination[ destOffset     ] = (byte)(outBuff >> 16);
                destination[ destOffset + 1 ] = (byte)(outBuff >>  8);
                destination[ destOffset + 2 ] = (byte)(outBuff);
                return 3;
            }
            catch (Exception e)
            {
                return -1;
            }
        }
    }

    private static byte[] encode3to4(byte[] source, int srcOffset, int numSigBytes, byte[] destination, int destOffset)
    {
        int inBuff = (numSigBytes > 0 ? ((source[ srcOffset ] << 24) >>>  8) : 0)
                     | (numSigBytes > 1 ? ((source[ srcOffset + 1 ] << 24) >>> 16) : 0)
                     | (numSigBytes > 2 ? ((source[ srcOffset + 2 ] << 24) >>> 24) : 0);
        switch (numSigBytes)
        {
        case 3:
            destination[ destOffset     ] = ENCODE_ALPHABET[(inBuff >>> 18)];
            destination[ destOffset + 1 ] = ENCODE_ALPHABET[(inBuff >>> 12) & 0x3f ];
            destination[ destOffset + 2 ] = ENCODE_ALPHABET[(inBuff >>>  6) & 0x3f ];
            destination[ destOffset + 3 ] = ENCODE_ALPHABET[(inBuff) & 0x3f ];
            return destination;
        case 2:
            destination[ destOffset     ] = ENCODE_ALPHABET[(inBuff >>> 18)];
            destination[ destOffset + 1 ] = ENCODE_ALPHABET[(inBuff >>> 12) & 0x3f ];
            destination[ destOffset + 2 ] = ENCODE_ALPHABET[(inBuff >>>  6) & 0x3f ];
            destination[ destOffset + 3 ] = EQUALS_SIGN;
            return destination;
        case 1:
            destination[ destOffset     ] = ENCODE_ALPHABET[(inBuff >>> 18)];
            destination[ destOffset + 1 ] = ENCODE_ALPHABET[(inBuff >>> 12) & 0x3f ];
            destination[ destOffset + 2 ] = EQUALS_SIGN;
            destination[ destOffset + 3 ] = EQUALS_SIGN;
            return destination;
        default:
            return destination;
        }
    }
}
