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
* Description: RmsFileManager
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import java.io.*;
import java.util.Vector;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.impl.rt.support.*;
import com.nokia.mj.impl.storage.*;
import com.nokia.mj.impl.utils.*;

/**
 * RmsFileManager can be used list and remove record stores and
 * to get record store file name.
 */
public class RmsFileManager
{
    private static final String EXTENSION = ".rms";
    private static final String ENCODING = "UTF-8";
    private static final String DIRECTORY = "rms" + System.getProperty("file.separator");

    static
    {
        RmsFileManager.createRmsHomeDir();
    }

    public static String getFilename(RmsName aName) throws RecordStoreNotFoundException
    {
        String rootPath = "";
        if (aName.isHomeSuite())
        {
            rootPath = getRootPath();
        }
        else
        {
            rootPath = getRootPath(aName);
        }
        String encodedName = encodeStoreName(aName.getName());
        String filename = rootPath + encodedName + EXTENSION;
        return filename;
    }

    private static String getRootPath()
    {
        String path = ApplicationInfo.getInstance().getRootPath() + DIRECTORY;
        return path;
    }

    private static String getRootPath(RmsName aName) throws RecordStoreNotFoundException
    {
        StorageSession session = null;
        try
        {
            StorageAttribute attr = new StorageAttribute(StorageNames.PACKAGE_NAME, aName.getSuiteName());
            StorageEntry query = new StorageEntry();
            query.addAttribute(attr);

            attr = new StorageAttribute(StorageNames.VENDOR, aName.getVendorName());
            query.addAttribute(attr);

            attr = new StorageAttribute(StorageNames.ROOT_PATH, "");
            query.addAttribute(attr);

            session = StorageFactory.createSession();
            session.open();
            StorageEntry[] receivedEntries = session.search(StorageNames.APPLICATION_PACKAGE_TABLE, query);

            if (receivedEntries == null || receivedEntries.length == 0)
            {
                Logger.WLOG(Logger.EMidpRms, "getRootPath() failed: " + aName);
                throw new RecordStoreNotFoundException(RmsErrorStrings.NOT_FOUND + aName);
            }
            String suiteRoot = receivedEntries[0].getAttribute(StorageNames.ROOT_PATH).getValue();
            return suiteRoot + DIRECTORY;
        }
        catch (StorageException se)
        {
            Logger.ELOG(Logger.EMidpRms, "getRootPath() failed", se);
            throw new RecordStoreNotFoundException(RmsErrorStrings.NOT_FOUND + aName);
        }
        finally
        {
            if (session != null)
            {
                session.close();
                session.destroySession();
            }
        }
    }

    private static String encodeStoreName(String aName)
    {
        String filename = aName;
        try
        {
            // use modified Base64 for filename, replace '/' with '-'
            filename = Base64.encode(aName.getBytes(ENCODING));
            byte[] utf8 = replaceChar(filename.getBytes(ENCODING),'/', '-');
            filename = new String(utf8, ENCODING);
        }
        catch (UnsupportedEncodingException e)
        {
        }
        return filename;
    }

    private static String decodeStoreName(String aName)
    {
        String filename = aName;
        try
        {
            // convert modified Base64 for filename to be valid base64
            byte[] utf8 = replaceChar(filename.getBytes(ENCODING), '-', '/');
            filename = new String(utf8, ENCODING);
            byte[] bytes = Base64.decode(filename);
            filename = new String(bytes, ENCODING);
        }
        catch (UnsupportedEncodingException e)
        {
        }
        return filename;
    }

    private static byte[] replaceChar(byte[] aArray, char aOrigChar, char aReplacement)
    {
        for (int i = 0; i < aArray.length; i++)
        {
            if (aArray[i] == (byte)aOrigChar)
            {
                aArray[i] = (byte)aReplacement;
            }
        }
        return aArray;
    }

    public static void deleteRecordStore(RmsName aName)
    throws RecordStoreException, RecordStoreNotFoundException
    {
        try
        {
            String filename = getFilename(aName);
            FileUtility file = new FileUtility(filename);
            boolean rc = file.delete();
            if (!rc)
            {
                Logger.LOG(Logger.EMidpRms, Logger.EInfo, "deleteRecordStore() not found: " + aName);
                throw new RecordStoreNotFoundException(RmsErrorStrings.NOT_FOUND + aName);
            }
        }
        catch (SecurityException se)
        {
            Logger.ELOG(Logger.EMidpRms, "deleteRecordStore() failed", se);
            throw new RecordStoreException(se.getMessage());
        }
    }

    public static String[] listRecordStores()
    {
        String[] list = listFiles();
        for (int i = 0; list != null && i < list.length; i++)
        {
            String name = removeExtension(list[i]);
            list[i] = decodeStoreName(name);
        }
        return list;
    }

    private static String[] listFiles()
    {
        String[] list = null;
        try
        {
            FileUtility fileUtils = new FileUtility(getRootPath());
            Vector v = fileUtils.listFiles("*" + EXTENSION, true);
            if (!v.isEmpty())
            {
                list = new String[v.size()];
                for (int i = 0; i < v.size(); i++)
                {
                    list[i] = (String)v.elementAt(i);
                }
            }
        }
        catch (IOException ioe)
        {
            Logger.ELOG(Logger.EMidpRms, "listFiles() failed", ioe);
        }
        return list;
    }

    private static String removeExtension(String aFilename)
    {
        int index = aFilename.indexOf(EXTENSION);
        if (index != -1)
        {
            return aFilename.substring(0,index);
        }
        return aFilename;
    }

    public static boolean exists(RmsName aName)
    {
        try
        {
            String filename = getFilename(aName);
            FileUtility file = new FileUtility(filename);
            boolean rc = file.exists();
            return rc;
        }
        catch (RecordStoreNotFoundException e)
        {
            return false;
        }
    }

    private static void createRmsHomeDir()
    {
        try
        {
            String name = getRootPath();
            FileUtility dir = new FileUtility(name);
            if (!dir.exists())
            {
                dir.mkdir();
            }
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EMidpRms, "createRmsHomeDir() failed", e);
        }
    }
}

