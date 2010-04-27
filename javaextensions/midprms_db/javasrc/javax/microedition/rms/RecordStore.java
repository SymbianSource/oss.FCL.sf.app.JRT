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
* Description: RecordStore
*
*/


package javax.microedition.rms;
import com.nokia.mj.impl.rms.*;
import java.util.Vector;

/**
 * Record store can be used to store data persistently.
 */
import java.io.*;


public class RecordStore
{
    /**
     * Authorization modes.
     * AUTHMODE_PRIVATE allows access only to the owning MIDlet suite.
     * AUTHMODE_ANY allows access to any MIDlet suites.
     * AUTHMODE_APPLEVEL allows access to certain identified MIDlets.
     */
    public static final int AUTHMODE_PRIVATE = 0;
    public static final int AUTHMODE_ANY = 1;
    /*public in MIPD3*/
    static final int AUTHMODE_APPLEVEL = 2;

    /**
     * Collection of open record stores.
     * If existing record store is opened again then a reference to it is returned
     */
    private static Vector iOpenRecordStores = new Vector();

    private int iOpenCount = 0; // how many times this record store has been opened
    private Rms iRms;
    private RmsName iName;
    private RecordStoreInfo iInfo;
    // all RMS API operations are synchronized with this object
    private Object iSync = new Object();

    /**
     * Adds a new record to the record store
     *
     * @param aData data to be stored
     * @param aOffset index into the buffer
     * @param aNumBytes number of bytes to use
     * @return recordId for the new record
     * @throws RecordStoreNotOpenException if not open
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreFullException if no more room
     * @throws SecurityException if only read-only access
     */
    public int addRecord(byte[] aData, int aOffset, int aNumBytes)
    throws RecordStoreNotOpenException, RecordStoreException, RecordStoreFullException
    {
        return addRecord(aData, aOffset, aNumBytes, 0);
    }

    /**
     * Adds a new record to the record store
     *
     * @param aData data to be stored
     * @param aOffset index into the buffer
     * @param aNumBytes number of bytes to use
     * @param aTag a tag value to identify the record
     * @return recordId for the new record
     * @throws RecordStoreNotOpenException if not open
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreFullException if no more room
     * @throws SecurityException if only read-only
     */
    /*public in MIPD3*/ int addRecord(byte[] aData, int aOffset, int aNumBytes, int aTag)
    throws RecordStoreNotOpenException, RecordStoreException, RecordStoreFullException
    {
        Record r = new Record(aData, aOffset, aNumBytes, aTag);
        return iRms.addRecord(r);
    }

    /**
     * Adds the record listener
     *
     * @param aListener record listener to be added
     * @see #removeRecordListener(RecordListener)
     */
    public void addRecordListener(RecordListener aListener)
    {
        iRms.addRecordListener(aListener);
    }

    /**
     * Closes the record store.
     *
     * @throws RecordStoreNotOpenException if not open
     * @throws RecordStoreException if record store related error occurred
     */
    public void closeRecordStore() throws RecordStoreNotOpenException, RecordStoreException
    {
        synchronized (iOpenRecordStores)
        {
            iOpenCount--;
            if (iOpenCount < 1)
            {
                removeRecordStore(this);
                iRms.close();
            }
        }
    }

    /**
     * Deletes record from the record store.
     *
     * @param aRecordId recordId of the record to be deleted
     * @throws RecordStoreNotOpenException if not open
     * @throws InvalidRecordIDException if recordId is invalid
     * @throws RecordStoreException if record store related error occurred
     * @throws SecurityException if only read-only access
     */
    public void deleteRecord(int aRecordId)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        Record r = new Record(aRecordId);
        iRms.deleteRecord(r);
    }

    /**
     * Deletes the record store.
     *
     * @param aRecordStoreName name of the record store to be deleted
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreNotFoundException if not found
     */
    public static void deleteRecordStore(String aRecordStoreName)
    throws RecordStoreException, RecordStoreNotFoundException
    {
        if (aRecordStoreName == null)
        {
            throw new RecordStoreNotFoundException(RmsErrorStrings.INVALID_NAME);
        }

        synchronized (iOpenRecordStores)
        {
            //check if record store open
            RmsName name = new RmsName(aRecordStoreName);
            RecordStore recordStore = findRecordStore(name);
            if (recordStore != null)
            {
                throw new RecordStoreException(RmsErrorStrings.RS_IS_OPEN + aRecordStoreName);
            }
            RmsFileManager.deleteRecordStore(name);
        }
    }

    /**
     * Returns record store enumeration
     *
     * @param aFilter can be used to get user defined subset of
     *        the record store records
     * @param aComparator can be used for ordering the records in the enumeration
     * @param aKeepUpdated should the enumerator be kept current with
     *        changes in the record store
     * @return record enumeration
     * @throws RecordStoreNotOpenException if not open
     */
    public RecordEnumeration enumerateRecords(RecordFilter aFilter,
            RecordComparator aComparator,
            boolean aKeepUpdated)
    throws RecordStoreNotOpenException
    {
        return enumerateRecords(aFilter, aComparator, aKeepUpdated, null);
    }

    /**
     * Returns record store enumeration
     *
     * @param aFilter can be used to get user defined subset of
     *        the record store records
     * @param aComparator can be used for ordering the records in the enumeration
     * @param aKeepUpdated should the enumerator be kept current with
     *        changes in the record store
     * @param aTags only records with given tags will be included in the enumeration
     * @return record enumeration
     * @throws RecordStoreNotOpenException if not open
     */
    /*public in MIPD3*/ RecordEnumeration enumerateRecords(RecordFilter aFilter,
            RecordComparator aComparator,
            boolean aKeepUpdated,
            int[] aTags)
    throws RecordStoreNotOpenException
    {
        return new RecordEnumerationImpl(iRms, iSync, aFilter, aComparator, aKeepUpdated, aTags);
    }

    /**
     * Exports the record store.
     *
     * @param aOs Output stream where the record store is written
     * @param aRecordStoreName Name of the record store
     * @param aInternalPassword Password for the record store.
     * @param aExportPassword Password for encryption of the stream.
     * @throws IOException if there was an stream error
     * @throws RecordStoreException if record store cannot be read
     * @throws IllegalArgumentException if the recordStoreName is invalid.
     * @throws RecordStoreNotFoundException if not found
     * @throws SecureRecordStoreException if there is an error in
     *         decrypting or encrypting operations
     */
    /*public in MIPD3*/ static void exportRecordStore(OutputStream aOs,
            String aRecordStoreName,
            String aInternalPassword,
            String aExportPassword)
    throws IOException, RecordStoreException, IllegalArgumentException,
                RecordStoreNotFoundException, SecureRecordStoreException
    {
        throw new RecordStoreException();
    }

    /**
     * Returns the last time the record store was modified
     *
     * @return last modified time
     * @throws RecordStoreNotOpenException if not open
     */
    public long getLastModified() throws RecordStoreNotOpenException
    {
        try
        {
            RmsInfo info = iRms.getInfo();
            return info.getLastModified();
        }
        catch (RecordStoreException e)
        {
            throw new RecordStoreNotOpenException(e.getMessage());
        }
    }

    /**
     * Returns the name of RecordStore.
     *
     * @return record store name
     * @throws RecordStoreNotOpenException if not open
     */
    public String getName() throws RecordStoreNotOpenException
    {
        try
        {
            RmsInfo info = iRms.getInfo();
            return info.getName();
        }
        catch (RecordStoreException e)
        {
            throw new RecordStoreNotOpenException(e.getMessage());
        }
    }

    /**
     * Returns the recordId of the next record to be added to the record store.
     *
     * @return next recordId
     * @throws RecordStoreNotOpenException if not open
     * @throws RecordStoreException if record store related error occurred
     */
    public int getNextRecordID() throws RecordStoreNotOpenException, RecordStoreException
    {
        RmsInfo info = iRms.getInfo();
        return info.getNextRecordId();
    }

    /**
     * Returns the number of records in the record store.
     *
     * @return the number of records
     * @throws RecordStoreNotOpenException if not open
     */
    public int getNumRecords() throws RecordStoreNotOpenException
    {
        try
        {
            return iRms.getNumRecords();
        }
        catch (RecordStoreException e)
        {
            throw new RecordStoreNotOpenException(e.getMessage());
        }
    }

    /**
     * Returns the data stored in the given record.
     *
     * @param aRecordId recordId of the record
     * @return data stored in the given record
     * @throws RecordStoreNotOpenException if not open
     * @throws InvalidRecordIDException if the recordId is invalid
     * @throws RecordStoreException if record store related error occurred
     */
    public byte[] getRecord(int aRecordId)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        Record r = iRms.getRecord(new Record(aRecordId));
        return r.getData();
    }

    /**
     * Gets the record data to given buffer.
     *
     * @param aRecordId recordId of the record
     * @param aBuffer buffer where to copy the data
     * @param aOffset index into the buffer
     * @return number of bytes copied into the buffer
     * @throws RecordStoreNotOpenException if not open
     * @throws InvalidRecordIDException if the recordId is invalid
     * @throws RecordStoreException if record store related error occurred
     * @throws ArrayIndexOutOfBoundsException if buffer is too small
     */
    public int getRecord(int aRecordId, byte[] aBuffer, int aOffset)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        Record r = iRms.getRecord(new Record(aRecordId));
        if (r.getData() != null)
        {
            System.arraycopy(r.getData(), 0, aBuffer, aOffset, r.size());
        }
        return r.size();
    }

    /**
     * Returns the size of the record data.
     *
     * @param aRecordId recordId of the record
     * @return record size
     * @throws RecordStoreNotOpenException if not open
     * @throws InvalidRecordIDException if the recordId is invalid
     * @throws RecordStoreException if record store related error occurred
     */
    public int getRecordSize(int aRecordId)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        Record r = iRms.getRecord(new Record(aRecordId));
        return r.size();
    }

    /**
     * Gets RecordStoreInfo instance.
     *
     * @return RecordStoreInfo object
     * @throws RecordStoreNotOpenException if not open
     */
    /*public in MIPD3*/ RecordStoreInfo getRecordStoreInfo() throws RecordStoreNotOpenException
    {
        iRms.ensureOpen();
        return iInfo;
    }

    /**
     * Returns the size of the record store.
     *
     * @return the size of the record store
     * @throws RecordStoreNotOpenException if not open
     */
    public int getSize() throws RecordStoreNotOpenException
    {
        long size = iInfo.getSize();
        if (size > Integer.MAX_VALUE)
        {
            return Integer.MAX_VALUE;
        }
        return (int)size;
    }

    /**
     * Returns the amount of available room for this record store to grow.
     *
     * @return available size
     * @throws RecordStoreNotOpenException if not open
     */
    public int getSizeAvailable() throws RecordStoreNotOpenException
    {
        long size = iInfo.getSizeAvailable();
        if (size > Integer.MAX_VALUE)
        {
            return Integer.MAX_VALUE;
        }
        return (int)size;
    }

    /**
     * Returns the tag associated with the recordId.
     *
     * @param aRecordId recordId of the record
     * @return tag
     * @throws RecordStoreNotOpenException if not open
     * @throws InvalidRecordIDException if the recordId is invalid
     * @throws RecordStoreException if record store related error occurred
     */
    /*public in MIPD3*/ int getTag(int aRecordId)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        Record r = iRms.getRecord(new Record(aRecordId));
        return r.getTag();
    }

    /**
     * Returns record store version.
     *
     * @return record store version
     * @throws RecordStoreNotOpenException if not open
     */
    public int getVersion() throws RecordStoreNotOpenException
    {
        try
        {
            RmsInfo info = iRms.getInfo();
            return info.getVersion();
        }
        catch (RecordStoreException e)
        {
            throw new RecordStoreNotOpenException(e.getMessage());
        }
    }

    /**
     * Imports the record store.
     *
     * @return RecordStore record store
     * @throws IOException if the stream cannot be read
     * @throws SecureRecordStoreException if there is error during
     *         decrypting or encrypting
     * @throws RecordStoreException if record store cannot be read
     * @throws RecordStoreNotFoundException if record store cannot be created
     */
    /*public in MIPD3*/ static RecordStore importRecordStore(InputStream aIs,
            String aImportPassword,
            String aInternalPassword)
    throws IOException, RecordStoreException, SecureRecordStoreException
    {
        throw new RecordStoreException();
    }

    /**
     * Returns list of record stores names owned by the MIDlet suite.
     *
     * @return list of record stores
     */
    public static String[] listRecordStores()
    {
        return RmsFileManager.listRecordStores();
    }

    /**
     * Open and possibly create a record store.
     *
     * @param aRecordStoreName name for the record store
     * @param aCreateIfNecessary should the record store be created
     * @return RecordStore the record store object
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreNotFoundException if not found
     * @throws RecordStoreFullException if full
     * @throws IllegalArgumentException if record store name is invalid
     */
    public static RecordStore openRecordStore(String aRecordStoreName,
            boolean aCreateIfNecessary)
    throws RecordStoreException, RecordStoreFullException, RecordStoreNotFoundException
    {
        return openRecordStore(aRecordStoreName, aCreateIfNecessary, AUTHMODE_PRIVATE, true);
    }

    /**
     * Open and possibly create a record store.
     *
     * @param aRecordStoreName name for the record store
     * @param aCreateIfNecessary should the record store be created
     * @param aAuthmode the mode under which to check or create access
     * @param aWritable should the record store be writeable by other MIDlet suites
     * @return RecordStore the record store object
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreNotFoundException if not found
     * @throws RecordStoreFullException if full
     * @throws IllegalArgumentException if record store name is invalid
     */
    public static RecordStore openRecordStore(String aRecordStoreName,
            boolean aCreateIfNecessary,
            int aAuthmode,
            boolean aWritable)
    throws RecordStoreException, RecordStoreFullException, RecordStoreNotFoundException
    {
        return openRecordStore(aRecordStoreName, aCreateIfNecessary, aAuthmode, aWritable, null);
    }

    /**
     * Open and possibly create a record store.
     *
     * @param aRecordStoreName name for the record store
     * @param aCreateIfNecessary should the record store be created
     * @param aAuthmode the mode under which to check or create access
     * @param aWritable should the record store be writeable by other MIDlet suites
     * @param aPassword password for generating an encryption key
     * @return RecordStore the record store object
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreNotFoundException if not found
     * @throws RecordStoreFullException if full
     * @throws SecureRecordStoreException if security related error occurred
     * @throws IllegalArgumentException if record store name is invalid
     */
    /*public in MIPD3*/ static RecordStore openRecordStore(String aRecordStoreName,
            boolean aCreateIfNecessary,
            int aAuthmode,
            boolean aWritable,
            String aPassword)
    throws RecordStoreException, RecordStoreFullException, RecordStoreNotFoundException
    {
        RecordStoreOpenArgs args = new RecordStoreOpenArgs(aRecordStoreName, aCreateIfNecessary,
                aAuthmode, aWritable, aPassword);
        return openRecordStore(args);
    }

    /**
     * Opens record store.
     *
     * @param aRecordStoreName record store name
     * @param aVendorName MIDlet vendor name
     * @param aSuiteName MIDlet suite name
     * @return RecordStore record store object
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreNotFoundException if not found
     * @throws SecurityException if not allowed to open
     * @throws IllegalArgumentException if record store name is invalid
     */
    public static RecordStore openRecordStore(String aRecordStoreName,
            String aVendorName,
            String aSuiteName)
    throws RecordStoreException, RecordStoreNotFoundException
    {
        return openRecordStore(aRecordStoreName, aVendorName, aSuiteName, null);
    }

    /**
     * Opens record store.
     *
     * @param aRecordStoreName record store name
     * @param aVendorNamethe MIDlet vendor name
     * @param aSuiteName MIDlet suite name
     * @param aPassword password for decrypting the record store data
     * @return RecordStore record store object
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreNotFoundException if not found
     * @throws SecurityException if not allowed to open
     * @throws IllegalArgumentException if record store name is invalid
     * @throws SecureRecordStoreException if there is security related error
     */
    /*public in MIPD3*/ static RecordStore openRecordStore(String aRecordStoreName,
            String aVendorName,
            String aSuiteName,
            String aPassword)
    throws RecordStoreException, RecordStoreNotFoundException, SecureRecordStoreException
    {
        RecordStoreOpenArgs args = new RecordStoreOpenArgs(aRecordStoreName, aVendorName, aSuiteName,
                aPassword);
        return openRecordStore(args);
    }

    /**
     * Removes the given record listener.
     *
     * @param aListener record listener to be removed
     */
    public void removeRecordListener(RecordListener aListener)
    {
        iRms.removeRecordListener(aListener);
    }

    /**
     * Changes the access mode for record store.
     *
     * @param aAuthmode is the record store sharable with other MIDlet suites
     * @param aWritable should record store be writeable by other MIDlet suites
     * @throws RecordStoreException if record store related error occurred
     * @throws SecurityException if not allowed
     * @throws IllegalArgumentException if authmode is invalid
     * @throws IllegalStateException if another MIDlet has RecordStore is open
     */
    public void setMode(int aAuthmode, boolean aWritable) throws RecordStoreException
    {
        validateAuthmode(aAuthmode);
        iRms.setMode(aAuthmode, aWritable);
    }

    /**
     * Sets record data.
     *
     * @param aRecordId recordId
     * @param aNewData data for the record
     * @param aOffset index into the buffer
     * @param aNumBytes the number of bytes to use
     * @throws RecordStoreNotOpenException if not open
     * @throws InvalidRecordIDException if the recordId is invalid
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreFullException if full
     * @throws SecurityException if only read-only access
     */
    public void setRecord(int aRecordId, byte[] aNewData, int aOffset, int aNumBytes)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException,
                RecordStoreFullException
    {
        Record r = new Record(aRecordId, aNewData, aOffset, aNumBytes);
        r.setKeepExistingTag(true);
        iRms.setRecord(r);
    }

    /**
     * Sets record data.
     *
     * @param aRecordId recordId
     * @param aNewData data for the record
     * @param aOffset index into the buffer
     * @param aNumBytes the number of bytes to use
     * @param aTag tag for the record
     * @throws RecordStoreNotOpenException if not open
     * @throws InvalidRecordIDException if the recordId is invalid
     * @throws RecordStoreException if record store related error occurred
     * @throws RecordStoreFullException if full
     * @throws SecurityException if only read-only access
     */
    /*public in MIPD3*/ void setRecord(int aRecordId, byte[] aNewData, int aOffset, int aNumBytes, int aTag)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException,
                RecordStoreFullException
    {
        Record r = new Record(aRecordId, aNewData, aOffset, aNumBytes, aTag);
        r.setKeepExistingTag(false);
        iRms.setRecord(r);
    }

    RecordStore(RecordStoreOpenArgs aArgs) throws RecordStoreNotFoundException, RecordStoreException
    {
        iName = aArgs.getRecordStoreName();
        iRms = new Rms(new RecordListeners(this, iSync), iSync);
        iRms.open(aArgs);
        iInfo = new RecordStoreInfo(iRms);
    }

    private static RecordStore openRecordStore(RecordStoreOpenArgs aArgs)
    throws RecordStoreException, RecordStoreFullException, RecordStoreNotFoundException
    {
        validateRecordStoreName(aArgs.getRecordStoreName().getName());
        validateAuthmode(aArgs.getAuthmode());

        synchronized (iOpenRecordStores)
        {
            RecordStore recordStore = findRecordStore(aArgs.getRecordStoreName());
            if (recordStore == null)
            {
                recordStore = new RecordStore(aArgs);
                addRecordStore(recordStore);
            }
            recordStore.iOpenCount++;
            return recordStore;
        }
    }

    private static RecordStore findRecordStore(RmsName aName)
    {
        int num = iOpenRecordStores.size();
        for (int i = 0; i < num; i++)
        {
            RecordStore rs = (RecordStore) iOpenRecordStores.elementAt(i);
            if (rs.iName.equals(aName))
            {
                return rs;
            }
        }
        return null;
    }

    private static void addRecordStore(RecordStore aRecordStore)
    {
        iOpenRecordStores.addElement(aRecordStore);
    }

    private static void removeRecordStore(RecordStore aRecordStore)
    {
        iOpenRecordStores.removeElement(aRecordStore);
    }

    private static void validateRecordStoreName(String aRecordStoreName)
    {
        if ((aRecordStoreName.length() > 32) || (aRecordStoreName.length() < 1))
        {
            throw new IllegalArgumentException(RmsErrorStrings.INVALID_NAME);
        }
    }

    private static void validateAuthmode(int aAuthmode)
    {
        if ((aAuthmode != AUTHMODE_PRIVATE) && (aAuthmode != AUTHMODE_ANY))
        {
            throw new IllegalArgumentException(RmsErrorStrings.INVALID_MODE);
        }
    }

}
