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
* Description: RecordStoreInfo
*
*/


package javax.microedition.rms;
import com.nokia.mj.impl.rms.*;

/**
 * RecordStoreInfo can be used to get information about RecordStore
 * attributes.
 */
/*public in MIPD3*/
final class RecordStoreInfo
{
    private Rms iRms;

    /**
     * Returns the access mode for the RecordStore
     *
     * @return access mode
     * @throws RecordStoreNotOpenException if not open
     */
    public int getAuthMode() throws RecordStoreNotOpenException
    {
        try
        {
            RmsInfo info = iRms.getInfo();
            return info.getAuthMode();
        }
        catch (RecordStoreException e)
        {
            throw new RecordStoreNotOpenException(e.getMessage());
        }
    }

    /**
     * Returns the record store size.
     *
     * @return record store size
     * @throws RecordStoreNotOpenException if not open
     */
    public long getSize() throws RecordStoreNotOpenException
    {
        return iRms.getSize();
    }

    /**
     * Returns the amount of available room for record store to grow.
     *
     * @return available size
     * @throws RecordStoreNotOpenException if not open
     */
    public long getSizeAvailable() throws RecordStoreNotOpenException
    {
        return iRms.getSizeAvailable();
    }

    /**
     * Checks if the RecordStore is encrypted.
     *
     * @return true if encrypted, false otherwise
     * @throws RecordStoreNotOpenException if not open
     */
    public boolean isEncrypted() throws RecordStoreNotOpenException
    {
        try
        {
            RmsInfo info = iRms.getInfo();
            return info.isEncrypted();
        }
        catch (RecordStoreException e)
        {
            throw new RecordStoreNotOpenException(e.getMessage());
        }
    }

    /**
     * Checks the writeable state for the RecordStore
     *
     * @return true if RecordStore is writable by other MIDlet suites, false otherwise
     * @throws RecordStoreNotOpenException if not open
     */
    public boolean isWriteable() throws RecordStoreNotOpenException
    {
        try
        {
            RmsInfo info = iRms.getInfo();
            return info.isWriteable();
        }
        catch (RecordStoreException e)
        {
            throw new RecordStoreNotOpenException(e.getMessage());
        }
    }

    RecordStoreInfo(Rms aRms)
    {
        iRms = aRms;
    }

}
