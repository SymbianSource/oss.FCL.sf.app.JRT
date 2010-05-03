/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: RecordComparator interface to compare two records
*
*/


package javax.microedition.rms;

/**
 * RecordComparator interface to compare two records
 *
 * @see RecordEnumeration
 */
public interface RecordComparator
{
    /**
     * EQUIVALENT means the two records are the same.
     */
    public static final int EQUIVALENT = 0;

    /**
     * FOLLOWS means the left record follows the right
     */
    public static final int FOLLOWS = 1;

    /**
     * PRECEDES means the left record precedes the right
     */
    public static final int PRECEDES = -1;

    /**
     * Compares two records
     * @param aRec1 first record
     * @param aRec2 second record
     * @return RecordComparator.PRECEDES if aRec1 precedes aRec2
     * @return RecordComparator.FOLLOWS if aRec1 follows aRec2
     * @return RecordComparator.EQUIVALENT if aRec1 and aRec2 are equivalent
     */
    public int compare(byte[] aRec1, byte[] aRec2);
}
