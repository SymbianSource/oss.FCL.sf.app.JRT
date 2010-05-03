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

import com.nokia.mj.impl.comms.exception.CommsException;
import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;

/**
 * StorageConnection handles communication to Java Storage server databases.
 * Java Comms is used for communication.
 */
final class StorageConnection
{
    private CommsEndpoint iServerEnd;

    private static final int JAVASTORAGE_SERVER_ID = 10200;
    private static final int STORAGE_CONN_REG_ID = 401;
    private static final int TIMEOUT_SECS = 10;
    private static final int PLUGIN_ID_JAVASTORAGE_CLIENT_C = 5;

    /**
     * Constructor. Initiates comms connection to storage server.
     *
     * @throws StorageException if comms connection creation fails.
     */
    StorageConnection() throws StorageException
    {
        try
        {
            iServerEnd = new CommsEndpoint();
            iServerEnd.connect(JAVASTORAGE_SERVER_ID);
        }
        catch (CommsException ce)
        {
            throw new StorageException("Cannot connect to storage server", ce);
        }

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
        String response = "";
        try
        {
            CommsMessage msg = new CommsMessage();
            msg.setMessageId(STORAGE_CONN_REG_ID);
            msg.setModuleId(PLUGIN_ID_JAVASTORAGE_CLIENT_C);

            msg.write(aHeaders);
            msg.write(aStorageName);

            CommsMessage responseMsg =
                iServerEnd.sendReceive(msg, TIMEOUT_SECS);

            // Message body is sessionID
            response = checkReceivedMessage(responseMsg);
        }
        catch (CommsException e)
        {
            Logger.ELOG(Logger.EJavaStorage, "Communication error: "
                        + e.getMessage());
            throw new StorageException("Storage open failed", e);
        }

        return response;
    }

    /**
     * Close JavaStorage connection.
     *
     * @param aHeaders connection headers to identify closed connection.
     * @throws StorageException if closing fails.
     */
    void close(String aHeaders) throws StorageException
    {
        try
        {
            CommsMessage msg = new CommsMessage();
            msg.setMessageId(STORAGE_CONN_REG_ID);
            msg.setModuleId(PLUGIN_ID_JAVASTORAGE_CLIENT_C);

            msg.write(aHeaders);

            CommsMessage responseMsg =
                iServerEnd.sendReceive(msg, TIMEOUT_SECS);

            // Reponse contains OK if no exception thrown.
            String response = checkReceivedMessage(responseMsg);
        }
        catch (CommsException e)
        {
            Logger.ELOG(Logger.EJavaStorage, "Communication error: "
                        + e.getMessage());
            throw new StorageException("Storage close failed", e);
        }
    }

    /**
     * Execute SQL statement to JavaStorage.
     *
     * @param aHeaders Connection headers to identify used connection.
     * @param aSqlStatement SQL statement to be executed.
     * @return response to SQL query. This can be for instance queried
     *         application attributes in String format.
     */
    String execute(String aHeaders, String aSqlStatement)
    throws StorageException
    {
        String response = "";
        try
        {
            CommsMessage msg = new CommsMessage();
            msg.setMessageId(STORAGE_CONN_REG_ID);
            msg.setModuleId(PLUGIN_ID_JAVASTORAGE_CLIENT_C);

            msg.write(aHeaders);
            msg.write(aSqlStatement);

            CommsMessage responseMsg =
                iServerEnd.sendReceive(msg, TIMEOUT_SECS);

            response = checkReceivedMessage(responseMsg);
        }
        catch (CommsException e)
        {
            Logger.ELOG(Logger.EJavaStorage, "Communication error: "
                        + e.getMessage());
            throw new StorageException("Storage execute failed", e);
        }

        return response;
    }

    /**
     * Close JavaStorage server comms session.
     */
    void closeSession()
    {
        try
        {
            iServerEnd.disconnect();
        }
        catch (CommsException ce)
        {
            Logger.ELOG(Logger.EJavaStorage,
                        "Disconnect from storage server failed",
                        ce);
        }
        finally
        {
            iServerEnd.destroy();
        }
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Check received message is valid and read body of it.
     *
     * @param aReceivedMsg to be checked.
     * @return message body.
     * @throws StorageException if received message is null or message body is
     *         is empty or status code indicates error.
     */
    private String checkReceivedMessage(CommsMessage aReceivedMsg)
    throws StorageException
    {
        String response = "";

        if (aReceivedMsg != null)
        {
            int resultCode = aReceivedMsg.readInt();

            if (resultCode < 0)
            {
                // Read error description
                String errMsg = aReceivedMsg.readString();

                Logger.ELOG(Logger.EJavaStorage, "Internal Storage Error: "
                            + String.valueOf(resultCode) + " : " + errMsg);

                throw new StorageException("Internal Storage Error: "
                                           + String.valueOf(resultCode) + " : " + errMsg);
            }
            else
            {
                response = aReceivedMsg.readString();
                if (response == null || response == "")
                {
                    Logger.ELOG(Logger.EJavaStorage, "Empty message body");
                    throw new StorageException("Empty message body");
                }
            }
        }
        else
        {
            Logger.ELOG(Logger.EJavaStorage, "Empty response");
            throw new StorageException("Empty response");
        }

        return response;
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
