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


package com.nokia.mj.test.storage.utils;

import com.nokia.mj.impl.storage.*;
import java.util.Enumeration;

public class StorageSessionTestUtils implements StorageNames
{
    public StorageEntry getDefaultEntry()
    {
        StorageEntry entry = new StorageEntry();
        StorageAttribute attr =
            new StorageAttribute(PACKAGE_NAME, "JavaGame");
        entry.addAttribute(attr);

        attr = new StorageAttribute(VENDOR, "JavaVendor");
        entry.addAttribute(attr);

        attr = new StorageAttribute(VERSION, "4.3.2.1");
        entry.addAttribute(attr);

        return entry;
    }

    public boolean populateWithDefault(StorageSession aSession,
                                       String aPrimaryKey)
    {
        boolean populated = false;
        StorageEntry temp = getDefaultEntry();
        temp.addAttribute(new StorageAttribute(
                              StorageAttribute.ID, aPrimaryKey));

        try
        {
            aSession.write(APPLICATION_PACKAGE_TABLE, temp);
            populated = true;
        }
        catch (Exception e)
        {
            System.out.println("Populate with default failed: "
                               + e.toString());
        }
        return populated;
    }

    public boolean checkDefaultParams(StorageSession aSession,
                                      String aPrimaryKey)
    {
        boolean found = false;

        StorageEntry temp = getDefaultEntry();
        temp.addAttribute(new StorageAttribute(
                              StorageAttribute.ID, aPrimaryKey));

        try
        {
            StorageEntry[] foundEntries = aSession.search(
                                              APPLICATION_PACKAGE_TABLE, temp);

            if (foundEntries == null)
            {
                System.out.println("No entries found!");
            }
            else if (foundEntries.length != 1)
            {
                System.out.println("Wrong amout of entries found. \n"
                                   + "Expected: 1 found: " + foundEntries.length);
            }
            else
            {
                found = compareEntry(temp, foundEntries[0]);
            }
        }
        catch (Exception e)
        {
            System.out.println("Search default failed: "
                               + e.toString());
        }

        return found;
    }

    /**
     * Remove default entry by given application. It is also verified
     * that application is removed.
     *
     * @param aSession used StorageSession.
     * @param aPrimaryKey application UID to be removed.
     * @return true if removal was successful, false otherwise.
     */
    public boolean removeDefaultParams(StorageSession aSession,
                                       String aPrimaryKey)
    {
        boolean success = false;

        StorageEntry temp = getDefaultEntry();
        temp.addAttribute(new StorageAttribute(
                              StorageAttribute.ID, aPrimaryKey));

        try
        {
            aSession.remove(APPLICATION_PACKAGE_TABLE, temp);

            // Verify removal
            StorageEntry[] foundEntries = aSession.search(
                                              APPLICATION_PACKAGE_TABLE, temp);

            if (foundEntries == null)
            {
                success = true;
            }
            else
            {
                System.out.println("After default deletion. \n"
                                   + "Expected: 0 found: " + foundEntries.length);
            }
        }
        catch (Exception e)
        {
            System.out.println("Remove default failed: "
                               + e.toString());
        }

        return success;
    }

    public boolean compareEntry(StorageEntry aRef, StorageEntry aSource)
    {
        // This does not apply as attribyte type information is not stored to
        // DB.
        // if (aRef.toString().equals(aSource.toString()))
        Enumeration attrs = aRef.elements();

        // Compare only reference attributes. Source can have more attributes.
        while (attrs.hasMoreElements())
        {
            // Order does not matter.
            StorageAttribute refAttr = (StorageAttribute) attrs.nextElement();
            StorageAttribute sourceAttr = aSource.getAttribute(
                                              refAttr.getName());

            if (sourceAttr != null && refAttr.getValue() != null)
            {
                if (!refAttr.getValue().equals(sourceAttr.getValue()))
                {
                    System.out.println("Attr in scope: " + refAttr.toString());
                    System.out.println("Entry does not match. \nRef: "
                                       + aRef.toString() + "\nSrc: " + aSource.toString()
                                       + "\n");
                    return false;
                }

                if (!refAttr.getName().equals(sourceAttr.getName()))
                {
                    System.out.println("Attr in scope: " + refAttr.toString());
                    System.out.println("Entry does not match. \nRef: "
                                       + aRef.toString() + "\nSrc: " + aSource.toString()
                                       + "\n");
                    return false;
                }
            }
            else
            {
                if (refAttr.getValue() != null)
                {
                    System.out.println("Attr in scope: " + refAttr.toString());
                    System.out.println("Source attr does not exists. \nRef: "
                                       + aRef.toString() + "\nSrc: " + aSource.toString()
                                       + "\n");
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * Compare StorageEntries. It is expected that they have entries in same
     * order. StorageSession should return them at same order than queried.
     *
     * @param aRef reference entry.
     * @param aSource source entry to be compared.
     * @return true if match, false otherwise.
     */
    public boolean compareEntries(StorageEntry[] aRef, StorageEntry[] aSource)
    {
        if (aRef.length != aSource.length)
        {
            System.out.println("Different amount of entries.\n"
                               + "Expected: " + aRef.length + "Received: " + aSource.length);
            return false;
        }

        for (int i = 0; i < aRef.length; i++)
        {
            boolean result = compareEntry(aRef[i], aSource[i]);

            if (!result)
            {
                return false;
            }
        }
        return true;
    }

    public boolean populate(StorageSession aSession,
                            String aTableName,
                            String aPrimaryKey,
                            StorageEntry aEntry)
    {
        boolean success = false;

        if (aPrimaryKey != null)
        {
            aEntry.addAttribute(new StorageAttribute(
                                    ID, aPrimaryKey));
        }

        try
        {
            aSession.write(aTableName, aEntry);
            success = true;

        }
        catch (Exception e)
        {
            System.out.println("Populate failed: "
                               + e.toString());
        }
        return success;
    }

    public boolean update(StorageSession aSession,
                          String aTableName,
                          String aPrimaryKey,
                          StorageEntry aUpdate,
                          StorageEntry aMatch)
    {
        boolean success = false;

        // If primary key is defined add it to match pattern.
        if (aPrimaryKey != null || !aPrimaryKey.equals(""))
        {
            aMatch.addAttribute(new StorageAttribute(
                                    ID, aPrimaryKey));
        }

        try
        {
            aSession.update(aTableName, aUpdate, aMatch);
            success = true;
        }
        catch (Exception e)
        {
            System.out.println("Update failed: "
                               + e.toString());
        }
        return success;

    }

    public boolean checkParams(StorageSession aSession,
                               String aTableName,
                               StorageEntry aEntry,
                               int aFoundApps)
    {
        boolean success = false;
        try
        {
            StorageEntry[] foundEntries = aSession.search(
                                              aTableName, aEntry);

            if (foundEntries == null)
            {
                System.out.println("No found entries");
                return false;
            }

            if (foundEntries.length == aFoundApps)
            {
                for (int i = 0; i < foundEntries.length; i++)
                {
                    success = compareEntry(aEntry, foundEntries[i]);
                    if (!success)
                    {
                        System.out.println("Entry attributes did not match.\n"
                                           + "Ref: " + aEntry.toString()
                                           + "\nSrc: " + foundEntries[i].toString());
                        return false;
                    }
                }
            }
            else
            {
                System.out.println("Check failed. \n"
                                   + "Expected: " + aFoundApps + " found: "
                                   + foundEntries.length);
            }
        }
        catch (Exception e)
        {
            System.out.println("Check failed: "
                               + e.toString());
        }

        return success;
    }

    public boolean remove(StorageSession aSession,
                          String aTableName,
                          String aPrimaryKey,
                          StorageEntry aEntry)
    {
        boolean success = false;

        if (aPrimaryKey != null)
        {
            aEntry.addAttribute(new StorageAttribute(
                                    ID, aPrimaryKey));
        }

        try
        {
            aSession.remove(aTableName, aEntry);

            // Verify removal
            StorageEntry[] foundEntries = aSession.search(
                                              aTableName, aEntry);

            if (foundEntries == null)
            {
                success = true;
            }
            else
            {
                System.out.println("After deletion. \n"
                                   + "Expected: 0 found: " + foundEntries.length);
            }
        }
        catch (Exception e)
        {
            System.out.println("Remove failed: "
                               + e.toString());
        }
        return success;
    }

    public StorageEntry createAppPackageEntry()
    {
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               PACKAGE_NAME, "JavaDefMIDlet"));
        entry.addAttribute(new StorageAttribute(
                               VENDOR, "JavaDefVendor"));
        entry.addAttribute(new StorageAttribute(
                               VERSION, "8.8.8.8"));
        entry.addAttribute(new StorageAttribute(
                               ROOT_PATH, "/JavaAPI/default/path"));
        entry.addAttribute(new StorageAttribute(
                               MEDIA_ID,
                               "56567",
                               StorageAttribute.INT_TYPE));
        entry.addAttribute(new StorageAttribute(
                               StorageAttribute.INITIAL_SIZE,
                               "8989",
                               StorageAttribute.INT_TYPE));
        entry.addAttribute(new StorageAttribute(
                               JAD_PATH, "JavaAPI/JAD/path"));
        entry.addAttribute(new StorageAttribute(
                               JAR_PATH, "JavaAPI/JAR/path"));
        entry.addAttribute(new StorageAttribute(
                               ACCESS_POINT, "JavaAPI"));

        return entry;
    }
}