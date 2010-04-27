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
* Description: RecordListener
*
*/


package javax.microedition.rms;

/**
 * Interface for receiving record store change events
 */
public interface RecordListener
{
    /**
     * Called when record is added.
     *
     * @param aRecordStore record store in which the change occurred
     * @param aRecordId id of the added record
     */
    public void recordAdded(RecordStore aRecordStore, int aRecordId);

    /**
     * Called when record is changed.
     *
     * @param aRecordStore record store in which the change occurred
     * @param aRecordId id of the changed record
     */
    public void recordChanged(RecordStore aRecordStore, int aRecordId);

    /**
     * Called when record is deleted.
     *
     * @param aRecordStore record store in which the change occurred
     * @param aRecordId id of the deleted record
     */
    public void recordDeleted(RecordStore aRecordStore, int aRecordId);
}
