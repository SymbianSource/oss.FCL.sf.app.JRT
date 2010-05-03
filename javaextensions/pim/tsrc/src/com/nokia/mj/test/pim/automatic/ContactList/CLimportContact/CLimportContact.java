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
* Description:
*
*/


//PACKAGE
package com.nokia.mj.test.pim.automatic.ContactList.CLimportContact;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ContactList.importContact()
 *
 */
public class CLimportContact
        extends MIDPTest
{

    public CLimportContact(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private ContactList cl = null;

    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] contactListNames = null;
        Contact c1 = null;
        int count = 0;

        // open first Contact list
        try
        {
            p = PIM.getInstance();
            contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            cl = (ContactList)p.openPIMList(PIM.CONTACT_LIST, PIM.READ_WRITE, contactListNames[0]);

            count = countItems(cl);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        }

        // read Contact from serial format
        InputStream is = null;
        try
        {
            is = getClass().getResourceAsStream("/res/3.vcf");
            PIMItem[] items = p.fromSerialFormat(is, "UTF-8");
            c1 = (Contact) items[0];
        }
        catch (Exception e)
        {
            failTest("Failed to read an Contact from file: " + e);
        }

        log("ContactList has "+ count + " items.");

        log("Test that importing new Contact item does not increase item count in ContactList.");
        Contact c2 = null;
        try
        {
            c2 = cl.importContact(c1);

            if (countItems(cl) != count)
            {
                failTest("Importing new Contact changed ContactList item count!");
            }
            // test that imported item is ok.
            if (cl.isSupportedField(Contact.URL))
            {
                String url = c2.getString(Contact.URL, 0);
                if (!url.equals("http://www.imc.org/"))
                {
                    failTest("Contact item import failed to set URL to http://www.imc.org/, was: "+url);
                }
            }
        }
        catch (Throwable e)
        {
            failTest("Failed to test Contact item import: " + e);
        }

        log("Test that commiting the imported Contact item increases item count in ContactList.");
        try
        {
            c2.commit();
            if (countItems(cl) != (count + 1))
            {
                failTest("Commiting imported Contact did not increase ContactList item count by one!");
            }
        }
        catch (Exception e)
        {
            failTest("Failed to commit imported Contact to list: " + e);
        }

        log("Test that removing imported Contact item decreases item count.");
        try
        {
            cl.removeContact(c2);
            if (countItems(cl) != count)
            {
                failTest("Removing imported Contact did not decrease ContactList item count by one!");
            }
            cl.close();
        }
        catch (Exception e)
        {
            failTest("Failed to remove imported Contact from list: " + e);
        }

        passTest();
    }

    private int countItems(PIMList pl) throws TestEndThrowable
    {
        int tmp = 0;
        try
        {
            Enumeration e = pl.items();
            while (e.hasMoreElements())
            {
                e.nextElement();
                tmp++;
            }
        }
        catch (PIMException pime)
        {
            failTest("Error while getting items in list!");
        }

        return tmp;
    }

}
