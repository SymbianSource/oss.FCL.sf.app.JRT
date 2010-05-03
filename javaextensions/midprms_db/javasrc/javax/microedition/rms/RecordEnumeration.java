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
* Description: RecordEnumeration provides bidirectional record enumeration
*
*/

package javax.microedition.rms;

/**
 * RecordEnumeration provides bidirectional record enumeration
 */

public interface RecordEnumeration
{
    /**
     * Frees internal resources
     */
    public void destroy();

    /**
     * Gets the recordId of the specified index of the record enumeration.
     *
     * @return recordId
     * @throws IllegalArgumentException if index is out-of-bounds
     */
    /*public in MIPD3*/
    //public int getRecordId(int aIndex) throws IllegalArgumentException;

    /**
     * Check if next element exists
     *
     * @return true if next elements exist
     */
    public boolean hasNextElement();

    /**
     * Check if previous element exists
     *
     * @return true if previous elements exist
     */
    public boolean hasPreviousElement();

    /**
     * Check if the enumeration is kept updated
     * @return true if the enumeration is kept updated
     */
    public boolean isKeptUpdated();

    /**
     * Used to control whether the enumeration will be kept current with the changes
     * to record store.
     *
     * @param aKeepUpdated if true, the enumerator will be kept current
     *                     with changes to record store.
     */
    public void keepUpdated(boolean aKeepUpdated);

    /**
     * Returns the next record in enumeration.
     *
     * @return next record
     * @throws InvalidRecordIDException if no more records
     * @throws RecordStoreNotOpenException if not open
     * @throws RecordStoreException if record store related exception occurs
     */
    public byte[] nextRecord() throws InvalidRecordIDException,
                RecordStoreNotOpenException,
                RecordStoreException;

    /**
     * Returns the recordId of the next record in enumeration
     *
     * @return next record
     * @throws InvalidRecordIDException if no more records
     */
    public int nextRecordId() throws InvalidRecordIDException;

    /**
     * Returns the number of records in enumeration
     *
     * @return number of records in enumeration
     */
    public int numRecords();

    /**
     * Returns the previous record in enumeration
     *
     * @return previous record
     * @throws InvalidRecordIDException if no more records
     * @throws RecordStoreNotOpenException if not open
     * @throws RecordStoreException if record store related exception occurs
     */
    public byte[] previousRecord() throws InvalidRecordIDException,
                RecordStoreNotOpenException,
                RecordStoreException;

    /**
     * Returns the previous recordId in this enumeration
     *
     * @return recordId of the previous record
     * @throws InvalidRecordIDException if no more records
     */
    public int previousRecordId() throws InvalidRecordIDException;

    /**
     * Method updates the enumeration to reflect the current state of
     * the record store.
     *
     * @see #keepUpdated(boolean)
     */
    public void rebuild();

    /**
     * Resets the iteration index back to 0
     */
    public void reset();

}
