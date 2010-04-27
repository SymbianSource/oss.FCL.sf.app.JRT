/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.storage;

import com.nokia.mj.impl.storage.StorageSessionImpl;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * StorageConnection is communication channel to storage.
 */
final class StorageConnection
{
    /**
     * Native session handle
     */
    private int iSessionHandle = 0;

    static
    {
        Jvm.loadSystemLibrary("javastorage");
    }

    /**
     * Constructs native JavaDataAccess instance.
     */
    StorageConnection()
    {
        if (0 != iSessionHandle)
        {
            Logger.WLOG(Logger.EJavaStorage, "Session already created");
            return;
        }

        int ret = _startSession();

        if (ret < 0)
        {
            throw new StorageException("Native storage session failed:"
                                       + String.valueOf(ret));
        }

        iSessionHandle = ret;
    }

    /*** ----------------------------- PUBLIC ------------------------------ */
    /*** ---------------------------- PROTECTED ---------------------------- */
    /*** ----------------------------- PACKAGE ----------------------------- */

    /**
     * Open connection to Java Storage database.
     *
     * @param aHeaders Connection headers.
     * @param aStorageName Storage to open connection.
     * @return sessionID if open was successful.
     * @throws StorageException if open fails.
     */
    String open(String aHeaders, String aStorageName)
    throws StorageException
    {
        // Received message is checked and exception thrown by native side.
        return _open(iSessionHandle, aHeaders, aStorageName);
    }

    /**
     * Close JavaStorage connection.
     *
     * @param aHeaders connection headers to identify closed connection.
     * @throws StorageException if communication fails or server return error.
     */
    void close(String aHeaders) throws StorageException
    {
        // If error exception is thrown by native side.
        _close(iSessionHandle, aHeaders);
    }

    /**
     * Execute SQL statement to JavaStorage.
     *
     * @param aHeaders Connection headers to identify used connection.
     * @param aSqlStatement SQL statement to be executed.
     * @return response to SQL query. This can be for instance queried
     *         application attributes in String format.
     * @throws StorageException if communication fails or server return error.
     */
    String execute(String aHeaders, String aSqlStatement)
    throws StorageException
    {
        // Received message is checked and exception thrown by native side.
        return _execute(iSessionHandle, aHeaders, aSqlStatement);
    }

    /**
     * Closes native JavaStorage session. If it is still open to
     * free native resources
     */
    void closeSession()
    {
        // Close native JavaStorage session if it is still open
        if (0 != iSessionHandle)
        {
            _closeSession(iSessionHandle);
            iSessionHandle = 0;
        }
    }

    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Starts native application registration session.
     *
     * @return native session handle or Symbian error code (negative number)
     */
    private static native int _startSession();

    /**
     * Closes native application registration session.
     *
     * @param aSessionHandle the session to be closed
     */
    private static native void _closeSession(int aSessionHandle);

    /**
     * Open JavaStorage connection.
     *
     * @param aHeaders connection headers.
     * @param aStorageName database name.
     * @return connection sessionID.
     */
    private native String _open(int aSessionHandle,
                                String aHeaders,
                                String aStorageName);

    /**
     * Close JavaStorage connection.
     *
     * @param aHeaders connection headers to identify closed connection.
     * @return error code if closing failed.
     */
    private native void _close(int aSessionHandle, String aHeaders);

    /**
     * Execute SQL statement to JavaStorage.
     *
     * @param aHeaders Connection headers to identify used connection.
     * @param aSqlStatement SQL statement to be executed.
     * @return response to SQL query. This can be for instance queried
     *         application attributes in String format.
     */
    private native String _execute(int aSessionHandle,
                                   String aHeaders,
                                   String aSqlStatement);
}
