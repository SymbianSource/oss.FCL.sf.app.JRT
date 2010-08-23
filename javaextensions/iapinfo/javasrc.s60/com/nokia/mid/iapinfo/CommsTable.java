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
* Description:  Base class for CommDB Tables.
 *
*/

package com.nokia.mid.iapinfo;

import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.gcf.utils.NativeError;
import com.nokia.mj.impl.utils.Logger;

//import com.nokia.mj.impl.vmport.VmPort;

/**
 * <p>
 * Base class for <B>CommDB</B> tables. The descendant tables are defining the
 * CommDB data structure through their public member variables.<br>
 * The possible data operations are the followings:
 * <ul>
 * <li>list the records of a given table
 * <li>find a record by its name
 * <li>find a record by its record ID
 * </ul>
 * <p>
 * Before the user can access the actual table has to be opened with the
 * <CODE>open()</CODE> function of the current descendant table implementation.
 * <br>
 *
 *
 */
abstract class CommsTable
{

    /*
     * static { VmPort.getInstance().enableFinalization(CommsTable.class); }
     */

    /**
     * Unique identifier for the record;
     */
    public int iRecordId;

    /**
     * User-defined numeric tag for this record. Can be null.
     */
    public int iRecordTag;

    /**
     * User-defined name for this record. Should be unique in this table
     */
    public String iRecordName;

    /**
     * Error code - can be used for error code checking.
     */
    public static final int ERROR_NONE = 0;

    /**
     * Error code - indicates that the table cannot be opened.
     */
    public static final int ERROR_OPEN_FAILED = -1;

    /**
     * Error code - indicates that the table is closed but the actual operation
     * would require an open table.
     */
    public static final int ERROR_TABLE_CLOSED = -2;

    /**
     * Return value for search functions - indicates that the required record
     * not found.
     */
    public static final int RECORD_NOT_FOUND = -3;

    /**
     * Error code - indicates that the actual parameter is invalid (e.g. null).
     */
    public static final int ERROR_INVALID_PARAMETER = -4;

    /**
     * Error code - indicates that reading from the table failed and the field
     * values are not filled correctly.
     */
    public static final int ERROR_TABLE_READ = -5;

    /**
     * Error code - indicates that the end of table reached.
     */
    public static final int ERROR_TABLE_END = -6;

    protected String iTableName = "";

    protected static final String TABLE_IAP = "IAP";

    protected static final String TABLE_CONNECTION_PREFERENCES = "ConnectionPreferences";

    protected static final String TABLE_NETWORK = "Network";

    protected static final String TABLE_GLOBALSETTINGS = "GlobalSettings";

    /*
     * Handle for native peer.
     */
    private int iHandle;

    private int state = UNKNOWN;

    private static final int UNKNOWN = -1;

    private static final int CREATED = 0;

    private static final int CLOSED = 1;

    private static final int OPENED = 2;

    private Finalizer iFinalizer;

    public CommsTable(String tableName) throws CommDBException
    {

        iTableName = tableName;

        // we need this so we can do some cleanup when we are garbage collected
        iFinalizer = createFinalizer();
        state = CREATED;
    }

    /**
     * Opens the corresponding CommDB table.
     */
    public void open(int aNativeSessionHandle) throws CommDBException
    {

        iHandle = _open(aNativeSessionHandle, iTableName);
        Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "iHandle ="+iHandle);

        if (iHandle > 0)
        {
            state = OPENED;
            readFieldValues();
        }
        else
        {
            throw new CommDBException("Open failed!", ERROR_OPEN_FAILED, iHandle);
        }

    }

    /**
     * Close the actual table and release associated resources.
     */
    public void close()
    {// throws CommDBException
        state = CLOSED;
        _close(iHandle);
        clearFieldValues();
    }

    /**
     * Finds a record in the current table by its name. The search is case
     * sensitive. If the record exists the current record will be changed to
     * this record.
     *
     * @param recordName name of the record
     * @return with the RecordId of the found record or with RECORD_NOT_FOUND if
     *         no record found with the given name.
     */
    public int findByName(String recordName) throws CommDBException
    {

        int rec = 0;

        rec = _findByName(iHandle, recordName);
        if (rec >= 0)
        {
            readFieldValues();
        }
        else
        {
            return RECORD_NOT_FOUND;
        }
        return rec;

    }

    /**
     * Finds a record in the current table by its record RecordId. If the record
     * exists the current record will be changed to this record.
     *
     * @param id RecordId of the record
     * @return with the RecordId of the found record or with RECORD_NOT_FOUND if
     *         no record found with the given RecordId.
     */
    public int findById(int id) throws CommDBException
    {

        int rec = 0;

        rec = _findById(iHandle, id);
        if (rec >= 0)
        {
            readFieldValues();
        }
        else
        {
            return RECORD_NOT_FOUND;
        }
        return rec;
    }

    /**
     * Retreives the number of records in the current table.
     *
     * @return with the number of records
     */
    public int getRecordCount() throws CommDBException
    {
        Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo,"+getRecordCount()");
        return _getRecordCount(iHandle);
    }

    /**
     * Retrieves data from the next record in the database. Throws
     * CommDBException when the current record is the last record.
     */
    public void nextRecord() throws CommDBException
    {

        int ret = _next(iHandle);
        if (NativeError.KErrEof == ret)
        {
            throw new CommDBException("End of table reached!", ERROR_TABLE_END);
        }
        readFieldValues();
    }

    /**
     * Retrieves data from the previous record in the database. Throws
     * CommDBException when the current record is the first record.
     */
    public void previousRecord() throws CommDBException
    {
        int ret = _previous(iHandle);
        if (NativeError.KErrEof == ret)
        {
            throw new CommDBException("End of table reached!", ERROR_TABLE_END);
        }
        readFieldValues();
    }

    /*
     * Read field values of the current record.
     */
    abstract protected void readFieldValues() throws CommDBException;

    /*
     * Sets default values for all fields in the current record.
     */
    abstract protected void clearFieldValues();

    protected int readIntFieldValue(String fieldName) throws CommDBException
    {

        // Retreive native error code if any
        int[] error = new int[1];
        int value = _readIntFieldValue(iHandle, fieldName, error);
        if (NativeError.KErrNone != error[0])
        {
            clearFieldValues();
            throw new CommDBException("Error while reading table data!",
                                      ERROR_TABLE_READ, error[0]);
        }
        return value;
    }

    protected String readStringFieldValue(String fieldName)
    throws CommDBException
    {

        // Retreive native error code if any
        int[] error = new int[1];
        String value = _readStringFieldValue(iHandle, fieldName, error);
        if (NativeError.KErrNone != error[0])
        {
            clearFieldValues();
            throw new CommDBException("Error while reading table data!",
                                      ERROR_TABLE_READ, error[0]);
        }
        return value;
    }

    Finalizer createFinalizer()
    {
        // Logger.LOG(Logger.ESOCKET,
        // Logger.EInfo,"creating a socket finalizer object ");
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }
    // Gets called when the object is garbage collected
    public void doFinalize()
    {
        // Logger.LOG(Logger.ESOCKET,
        // Logger.EInfo,"socket doFinalize() called :");
        _destroy(iHandle);
    }
    /*----------------------------------------------------------------*/
    /* Native funcions */
    /*----------------------------------------------------------------*/
    /*
     * Destructs native peer.
     *
     * @param aHandle - handle for native peer
     */
    private static native void _destroy(int aHandle);

    /*
     * Initializes and opens the actual CommDB table for reading.
     *
     * @param table - the name of the current table.
     *
     * @return - error code
     */
    private static native int _open(int aSessionHandle, String tableName);

    /*
     * Close the actual CommDB table.
     *
     * @param aHandle - handle for native peer
     */
    private static native void _close(int aHandle);

    /*
     * Retrieves the next record from the database.
     *
     * @param aHandle - handle for native peer
     *
     * @return - error code
     */
    private static native int _next(int aHandle);

    /*
     * Retrieves the next record from the database.
     *
     * @param aHandle - handle for native peer
     *
     * @return - error code
     */
    private static native int _previous(int aHandle);

    /*
     * Retrieves the record count from the database.
     *
     * @param aHandle - handle for native peer
     *
     * @return - number of records in the current table
     */
    private static native int _getRecordCount(int aHandle);

    /*
     * Finds a record in the current table by its record name.
     *
     * @param recordName - the name of the record.
     *
     * @return - record ID, KErrNotFound if no record found
     */
    private static native int _findByName(int aHandle, String recordName);

    /*
     * Finds a record in the current table by its record ID.
     *
     * @param id - the ID of the record.
     *
     * @return - record ID, KErrNotFound if no record found
     */
    private static native int _findById(int aHandle, int id);

    /*
     * Retrieves the integer type field value for a given field. The values will
     * be set by the actual table descendant. All field has the corresponding
     * member variable in the appropriate class.
     *
     * @param aHandle - handle for native peer
     *
     * @param fieldName - name of the field to be read.
     *
     * @param aError - array for native error code
     *
     * @return - integer field value
     */
    private static native int _readIntFieldValue(int aHandle, String fieldName,
            int[] aError);

    /*
     * Retrieves the String type field value for a given field. The values will
     * be set by the actual table descendant. All field has the corresponding
     * member variable in the appropriate class.
     *
     * @param aHandle - handle for native peer
     *
     * @param fieldName - name of the field to be read.
     *
     * @param aError - array for native error code
     *
     * @return - string field value
     */
    private static native String _readStringFieldValue(int aHandle,
            String fieldName, int[] aError);

}
