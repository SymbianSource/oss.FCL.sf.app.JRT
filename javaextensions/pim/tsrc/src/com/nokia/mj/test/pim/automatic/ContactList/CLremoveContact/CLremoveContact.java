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
package com.nokia.mj.test.pim.automatic.ContactList.CLremoveContact;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.Contact;
import javax.microedition.pim.ContactList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ContactList.removeContact()
 */
public class CLremoveContact
        extends MIDPTest
{

    public CLremoveContact(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // preconditions
    PIM p = null;
    String[] contactListNames = null;
    ContactList cl = null;
    int count = 0;

    public void runTest() throws TestEndThrowable
    {

        // open first Contact list
        try
        {
            p = PIM.getInstance();
            contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            cl = (ContactList)p.openPIMList(PIM.CONTACT_LIST, PIM.READ_WRITE, contactListNames[0]);

            count = countItems(cl);
        }
        catch (Throwable t)
        {
            failTest("preconditions failed: " + t);
        }

        log("ContactList has "+ count + " items.");

        log("Test that creating new Contact item and adding it to the list does increase item count in ContactList.");

        Contact c = null;

        try
        {
            c = cl.createContact();
            if (cl.isSupportedField(Contact.URL))
            {
                c.addString(Contact.URL, PIMItem.ATTR_NONE, "testing");
            }
            else
            {
                failTest("Error while creating Contact item!");
            }
            c.commit();

            if (countItems(cl) != count+1)
            {
                failTest("Creating new Contact did not increase ContactList item count by one!");
            }

        }
        catch (Throwable t)
        {
            failTest("Failed to test Contact item creation: " + t);
        }

        log("Test that removing Contact item decreases ContactList item count by one.");

        try
        {
            cl.removeContact(c);
        }
        catch (Throwable t)
        {
            failTest("Failed to test Contact item removal: " + t);
        }

        if (countItems(cl) != count)
        {
            failTest("Removing Contact did not decrease ContactList item count by one!");
        }

        try
        {
            cl.close();
        }
        catch (Throwable t)
        {
            failTest("Error while closing list: " + t);
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
