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
* Description:  Collection of tools needed in the PIM API implementation.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// CLASS DEFINITION
/**
 * Some generic JNI-related tools are collected into this class.
 *
 * The class must be public so that it can be invoked from the
 * javax.microedition.pim package.
 */
public final class PIMTools
{
    /**
     * Hidden constructor; this class is not meant to be instantiated.
     */
    private PIMTools()
    {
    }

    /**
     * Converts given array of integers into an array of long integers,
     * combining each two integers into a single long integer.
     *
     * @param aIntArray
     *            An array of integers.
     *
     * @return An array of long integers, each even element in the original
     *         array put as the high part of an element in the result array and
     *         each odd element in the original array put as the low part of an
     *         element in the result array.
     */
    public static long[] intArrayToLongArray(int[] aIntArray)
    {
        // Check for null argument
        if (aIntArray == null)
        {
            throw new NullPointerException("Null integer array argument.");
        }

        int numIntElems = aIntArray.length;

        // Check for odd number of elements
        if ((numIntElems % 2) != 0)
        {
            throw new IllegalArgumentException();
        }

        int numLongElems = numIntElems / 2;

        long[] longArray = new long[numLongElems];

        for (int longIndex = 0; longIndex < numLongElems; longIndex++)
        {
            int intIndex = longIndex * 2;

            // high part in event element
            longArray[longIndex] = (long)(aIntArray[intIndex]) << 32;

            // low part in odd element
            longArray[longIndex] |= (aIntArray[intIndex + 1] & 0xFFFFFFFFL);
        }

        return longArray;
    }
}
