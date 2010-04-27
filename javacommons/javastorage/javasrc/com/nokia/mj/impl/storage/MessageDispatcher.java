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
import com.nokia.mj.impl.utils.Tokenizer;
import java.util.Enumeration;

/**
 * MessageDispatcher does message conversions. It creates SQL statements ouf of
 * API data types and constructs response objects from the storageresponse.
 */
final class MessageDispatcher
{
    private static final String APP_DELIM = ";#\n;";
    private static final String ATTR_DELIM = ";\n;";
    private StorageConnection iStorageConn;

    /**
     * Constructs communication channel to the storage.
     */
    MessageDispatcher()
    {
        iStorageConn = new StorageConnection();
    }

    /*** ----------------------------- PUBLIC ------------------------------ */
    /*** ---------------------------- PROTECTED ---------------------------- */
    /*** ----------------------------- PACKAGE ----------------------------- */

    /**
     * Create message and send it to storage for execution.
     *
     * @param aMsgId message identifier.
     * @param aHeaders connection headers.
     * @param aStorageName storage to be opened, not used otherwise.
     * @return session id if storage opened, not used otherwise.
     * @throws StorageException if statement execution or server connection
     *                          fails.
     */
    String createAndSendMessage(int aMsgId,
                                String aHeaders,
                                String aStorageName)
    throws StorageException
    {
        String receivedMessage = "";
        String headers = createHeaders(aMsgId, aHeaders);

        switch (aMsgId)
        {
        case StorageSessionImpl.OPEN_CONN:
        {
            // SessionID received.
            receivedMessage = iStorageConn.open(
                                  headers, aStorageName);
            break;
        }
        case StorageSessionImpl.CLOSE_CONN:
        {
            iStorageConn.close(headers);
            break;
        }
        case StorageSessionImpl.START_TRANSACTION:
        {
            iStorageConn.execute(headers, "BEGIN;");
            break;
        }
        case StorageSessionImpl.COMMIT:
        {
            iStorageConn.execute(headers, "COMMIT;");
            break;
        }
        case StorageSessionImpl.ROLLBACK:
        {
            iStorageConn.execute(headers, "ROLLBACK;");
            break;
        }
        case StorageSessionImpl.DESTROY_SESSION:
        {
            iStorageConn.closeSession();
            break;
        }
        default:
        {
            break;
        }
        }

        return receivedMessage;
    }

    /**
     * Create message and send it to server for execution. Message is headers
     * and SQL statement constructed from StorageEntry.
     *
     * @param aMsgId message identifier.
     * @param aHeaders connection headers.
     * @param aTableName table where statement is executed.
     * @param aEntry storage entry is used to construct SQL statement.
     * @return StorageEntry[] or empty array if no response is expected.
     * @throws StorageException if statement execution or server connection
     *                          fails.
     */
    StorageEntry[] createAndSendMessage(int aMsgId,
                                        String aHeaders,
                                        String aTableName,
                                        StorageEntry aEntry)
    throws StorageException
    {
        ensureNonEmpty(aTableName);

        StorageEntry[] responseEntry = null;
        StringBuffer sqlStatement = new StringBuffer();
        String headers = createHeaders(aMsgId, aHeaders);

        switch (aMsgId)
        {
        case StorageSessionImpl.SEARCH:
        {
            createSearchStatement(aEntry, aTableName, sqlStatement);

            String response = iStorageConn.execute(
                                  headers, sqlStatement.toString());

            // If entries found. This works until more than 10 000 rows
            // are handled without matching one.
            if (response.length() > APP_DELIM.length())
            {
                responseEntry = populateEntries(response);
            }
            break;
        }
        case StorageSessionImpl.WRITE:
        {
            createWriteStatement(aEntry, aTableName, sqlStatement);
            iStorageConn.execute(
                headers, sqlStatement.toString());
            break;
        }
        case StorageSessionImpl.REMOVE:
        {
            sqlStatement.append("DELETE FROM ").append(aTableName);

            if (aEntry != null && aEntry.size() > 0)
            {
                createWhereStatement(aEntry, sqlStatement);
            }
            else
            {
                // Remove all.
                sqlStatement.append(";");
            }

            String response = iStorageConn.execute(
                                  headers, sqlStatement.toString());

            // Get amount of removed entries.
            try
            {
                int removedEntries = Integer.parseInt(response);

                // As StorageEntry[] is returned, amount of removed entries
                // is delivered as entry array size. This is done to avoid
                //  overloading reateAndSendMessage and create unnecessary
                // overhead. Justification for this is that usually
                // a few entries are removed by the one execution.
                responseEntry = new StorageEntry[removedEntries];
            }
            catch (NumberFormatException nfe)
            {
                Logger.ELOG(Logger.EJavaStorage,
                            "Failed to parse amount of removed entries:"
                            + response);
            }
            break;
        }
        case StorageSessionImpl.APPEND_TABLE:
        {
            ensureNonEmpty(aEntry);
            String statementPrefix = "ALTER TABLE " + aTableName + " ADD ";

            /* NOTE: SQLite supports adding only one column
             * at the time. For this reason several calls need
             * to be executed.
             */
            Enumeration attrs = aEntry.elements();
            while (attrs.hasMoreElements())
            {
                StorageAttribute attribute =
                    (StorageAttribute) attrs.nextElement();

                sqlStatement.append(statementPrefix)
                .append(attribute.getName());

                // Append values statement
                if (attribute.getType() == StorageAttribute.INT_TYPE)
                {
                    sqlStatement.append(" INT;");
                }
                else    // STRING_TYPE
                {
                    sqlStatement.append(" VARCHAR;");
                }

                // Execute statement
                iStorageConn.execute(
                    headers, sqlStatement.toString());

                // clear previous statemt
                sqlStatement.delete(0, sqlStatement.length());
            }
            break;
        }
        default:
        {
            break;
        }
        }

        return responseEntry;
    }

    /**
     * Create update message and send it to server for execution.
     *
     * @param aHeaders connection headers.
     * @param aTableName table where statement is executed.
     * @param aUpdate StorageEntry containing updated attributes.
     * @param aMatch StorageEntry containing attributes used matching updated
     *               entries.
     * @throws StorageException if statement execution or server connection
     *                          fails.
     */
    void handleUpdate(String aHeaders,
                      String aTableName,
                      StorageEntry aUpdate,
                      StorageEntry aMatch)
    throws StorageException
    {
        ensureNonEmpty(aTableName);
        ensureNonEmpty(aUpdate);
        ensureNonEmpty(aMatch);

        String headers = createHeaders(StorageSessionImpl.UPDATE, aHeaders);
        StringBuffer sqlStatement = new StringBuffer();

        sqlStatement.append("UPDATE ").append(aTableName).append(" SET ");

        Enumeration updateAttrs = aUpdate.elements();

        while (updateAttrs.hasMoreElements())
        {
            StorageAttribute attribute =
                (StorageAttribute) updateAttrs.nextElement();

            sqlStatement.append(attribute.getName()).append(" = ");
            appendQuoted(sqlStatement,
                         attribute.getEscapedValue(),
                         "'",
                         attribute.getType() == StorageAttribute.STRING_TYPE);
            appendCond(sqlStatement, ", ", updateAttrs.hasMoreElements());
        }

        createWhereStatement(aMatch, sqlStatement);

        iStorageConn.execute(
            headers, sqlStatement.toString());
    }

    /**
     * Create update message and send it to server for execution.
     *
     * @param aHeaders connection headers.
     * @param aTableName table where statement is executed.
     * @param aTableColumns StorageEntry containing table columns.
     * @param aPrimaryKey primary key StorageAttribute.
     * @throws StorageException if statement execution or server connection
     *                          fails.
     */
    void handleCreateTable(String aHeaders,
                           String aTableName,
                           StorageEntry aTableColumns,
                           StorageAttribute aPrimaryKey)
    throws StorageException
    {
        ensureNonEmpty(aTableName);

        if ((aTableColumns == null || aTableColumns.size() == 0)
                && aPrimaryKey == null)
        {
            throw new StorageException("Invalid argument");
        }

        if (aTableColumns == null)
        {
            // Null is allowed argument but it will be converted to entry.
            // No separe logic needed at statement creation.
            aTableColumns = new StorageEntry();
        }

        String headers = createHeaders(StorageSessionImpl.UPDATE, aHeaders);
        StringBuffer sqlStatement = new StringBuffer();
        StringBuffer primaryKeyStmt = new StringBuffer();
        StringBuffer columnStmt = new StringBuffer();

        sqlStatement.append("CREATE TABLE ").append(aTableName).append(" (");

        if (aPrimaryKey != null)
        {
            if (aPrimaryKey.getType() == StorageAttribute.INT_TYPE)
            {
                primaryKeyStmt.append(aPrimaryKey.getName()).append(
                    " INT PRIMARY KEY");
            }
            else
            {
                primaryKeyStmt.append(aPrimaryKey.getName()).append(
                    " VARCHAR PRIMARY KEY");
            }
        }

        Enumeration attrs = aTableColumns.elements();
        while (attrs.hasMoreElements())
        {
            StorageAttribute attribute =
                (StorageAttribute) attrs.nextElement();
            columnStmt.append(attribute.getName());

            if (attribute.getType() == StorageAttribute.INT_TYPE)
            {
                columnStmt.append(" INT");
            }
            else    // STRING_TYPE
            {
                columnStmt.append(" VARCHAR");
            }
            appendCond(columnStmt, ", ", attrs.hasMoreElements());
        }

        appendCond(sqlStatement,
                   primaryKeyStmt.toString(),
                   primaryKeyStmt.length() > 0);

        if (columnStmt.length() > 0)
        {
            appendCond(sqlStatement, ", ", primaryKeyStmt.length() > 0);
            sqlStatement.append(columnStmt.toString());
        }
        sqlStatement.append(");");

        iStorageConn.execute(headers, sqlStatement.toString());
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    private StorageEntry[] populateEntries(String aReceivedMessage)
    throws StorageException
    {
        if (aReceivedMessage == null
                || aReceivedMessage.length() <= APP_DELIM.length())
        {
            throw new StorageException("Internal Error: Cannot populate "
                                       + "received entry");
        }

        StorageEntry[] populatedEntries = null;

        // Remove first token - always expected
        aReceivedMessage = aReceivedMessage.substring(APP_DELIM.length());

        String[] entries = Tokenizer.split(aReceivedMessage, APP_DELIM);

        if (entries != null)
        {
            populatedEntries = new StorageEntry[entries.length];
        }
        else
        {
            return null;
        }

        // Parse entries
        for (int i = 0; i < entries.length; i++)
        {
            StorageEntry entry = new StorageEntry();
            String[] attributes = Tokenizer.split(entries[i], ATTR_DELIM);

            // Parse attributes
            for (int j = 0; j < attributes.length; j++)
            {
                // Skip empties
                if (attributes[j].length() > 0)
                {
                    int split = attributes[j].indexOf('=');

                    // Skip invalid ones - robustness
                    if (split != -1)
                    {
                        String name = attributes[j].substring(0, split);
                        String value = attributes[j].substring(split + 1);
                        StorageAttribute attr =
                            new StorageAttribute(name, value);
                        entry.addAttribute(attr);
                    }
                }
            }
            populatedEntries[i] = entry;
        }

        return populatedEntries;
    }

    /**
     * Create SQL WHERE statement. Populates given StringBuffer with given
     * StorageEntry containing attributes to be included in the statement.
     *
     * @param aEntry containing attributes to be included in.
     * @param aWhereStatement statement to be populated.
     * @throws StorageException if attribute name is null.
     */
    private void createWhereStatement(StorageEntry aEntry,
                                      StringBuffer aWhereStatement)
    throws StorageException
    {
        aWhereStatement.append(" WHERE ");

        Enumeration attrs = aEntry.elements();
        while (attrs.hasMoreElements())
        {
            StorageAttribute attr =
                (StorageAttribute) attrs.nextElement();

            aWhereStatement.append(attr.getName()).append("=");
            appendQuoted(aWhereStatement,
                         attr.getEscapedValue(),
                         "'",
                         attr.getType() == StorageAttribute.STRING_TYPE);
            appendCond(aWhereStatement, " AND ", attrs.hasMoreElements());
        }
        aWhereStatement.append(";");
    }

    private void createSearchStatement(StorageEntry aEntry,
                                       String aTableName,
                                       StringBuffer aSqlStatement)
    {
        StringBuffer selectStmt = new StringBuffer();
        StringBuffer whereStmt = new StringBuffer();

        if (aEntry == null)
        {
            // Null is allowed argument but it will be converted to entry.
            // No separe logic needed at statement creation.
            aEntry = new StorageEntry();
        }

        Enumeration attrs = aEntry.elements();
        while (attrs.hasMoreElements())
        {
            StorageAttribute attribute =
                (StorageAttribute) attrs.nextElement();

            // Check if SELECT item
            if (attribute.getValue().equals(""))
            {
                selectStmt.append(attribute.getName()).append(',');
            }
            else  // WHERE item
            {
                whereStmt.append(attribute.getName()).append("=");
                appendQuoted(
                    whereStmt,
                    attribute.getEscapedValue(),
                    "'",
                    attribute.getType() == StorageAttribute.STRING_TYPE);
                whereStmt.append(" AND ");
            }
        }
        // remove trailing ','
        removeCond(selectStmt, selectStmt.length() - 1,
                   selectStmt.length(), selectStmt.length() > 0);
        // remove trailing ' AND '
        removeCond(whereStmt, whereStmt.length() - 5,
                   whereStmt.length(), whereStmt.length() > 0);

        appendCond(selectStmt, "*", selectStmt.length() == 0);

        aSqlStatement.append("SELECT ").append(selectStmt.toString())
        .append(" FROM ").append(aTableName);

        appendCond(aSqlStatement,
                   " WHERE " + whereStmt.toString(),
                   whereStmt.length() > 0);
        aSqlStatement.append(";");
    }

    private void createWriteStatement(StorageEntry aEntry,
                                      String aTableName,
                                      StringBuffer aSqlStatement)
    {
        ensureNonEmpty(aEntry);
        aSqlStatement.append("INSERT INTO ").append(aTableName).append(" (");

        StringBuffer values = new StringBuffer();
        Enumeration attrs = aEntry.elements();

        while (attrs.hasMoreElements())
        {
            StorageAttribute attribute =
                (StorageAttribute) attrs.nextElement();
            aSqlStatement.append(attribute.getName());
            appendCond(aSqlStatement, ", ", attrs.hasMoreElements());

            appendQuoted(values,
                         attribute.getEscapedValue(),
                         "'",
                         attribute.getType() == StorageAttribute.STRING_TYPE);

            appendCond(values, ", ", attrs.hasMoreElements());
        }
        aSqlStatement.append(')');

        aSqlStatement.append("VALUES (")
        .append(values.toString()).append(");");
    }

    /**
     * Create storage message headers.
     *
     * @param aMsgId message identifier.
     * @param aHeaders existing header. It is sessionID.
     * @return created headers.
     */
    private String createHeaders(int aMsgId, String aHeaders)
    {
        StringBuffer headers = new StringBuffer();

        // Add command ID that server can identify the needed action
        String reqID = String.valueOf(aMsgId);
        headers.append(String.valueOf(reqID.length()));
        headers.append(reqID);
        headers.append(aHeaders);
        return headers.toString();
    }

    private void ensureNonEmpty(StorageEntry aEntry)
    {
        if (aEntry == null || aEntry.size() < 1)
        {
            throw new StorageException("Invalid argument");
        }
    }

    private void ensureNonEmpty(String aValue)
    {
        if (aValue == null || aValue.equals(""))
        {
            throw new StorageException("Invalid argument");
        }
    }

    private void appendQuoted(
        StringBuffer aBuffer, String aValue, String aQuote, boolean aCondition)
    {
        if (aCondition)
        {
            aBuffer.append(aQuote).append(aValue).append(aQuote);
        }
        else
        {
            aBuffer.append(aValue);
        }
    }

    private void appendCond(
        StringBuffer aBuffer, String aValue, boolean aCondition)
    {
        if (aCondition)
        {
            aBuffer.append(aValue);
        }
    }

    private void removeCond(
        StringBuffer aBuffer, int aStart, int aEnd, boolean aCondition)
    {
        if (aCondition)
        {
            aBuffer.delete(aStart, aEnd);
        }
    }
}
