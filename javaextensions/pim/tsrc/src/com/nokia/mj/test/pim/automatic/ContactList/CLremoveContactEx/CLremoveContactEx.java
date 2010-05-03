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
package com.nokia.mj.test.pim.automatic.ContactList.CLremoveContactEx;
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
 *
 */
public class CLremoveContactEx
        extends MIDPTest
{

    public CLremoveContactEx(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // preconditions
    PIM p = null;
    ContactList cl = null;
    int count = 0;

    public void runTest() throws TestEndThrowable
    {
        try
        {
            p = PIM.getInstance();
        }
        catch (Throwable t)
        {
            failTest("preconditions failed: " + t);
        }

        cl = openContactList(PIM.READ_WRITE);
        count = countItems(cl);

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

        log("Test that removing Contact item from a read only list causes SecurityException.");
        try
        {
            cl.close();
        }
        catch (Exception ex)
        {
            failTest("Failed to close list: " + ex);
        }
        cl = openContactList(PIM.READ_ONLY);

        try
        {
            cl.removeContact(find(cl, c));
            failTest("SecurityException was not thrown when removing Contact from a read only list.");
        }
        catch (SecurityException se)
        {
            // ok.
        }
        catch (Exception ex)
        {
            failTest("Removing Contact from a read only list threw wrong exception: "+ex);
        }

        if (countItems(cl) != count+1)
        {
            failTest("Removing Contact from a read only list decreased item count by one!");
        }

        try
        {
            cl.close();
        }
        catch (Exception ex)
        {
            failTest("Failed to close list: " + ex);
        }
        cl = openContactList(PIM.READ_WRITE);

        log("Test that removing null Contact item does not change ContactList item count.");
        try
        {
            cl.removeContact(null);
            failTest("Removing null Contact did not throw any exception!");
        }
        catch (NullPointerException npe)
        {
            // ok.
        }
        catch (Exception ex)
        {
            failTest("Removing null Contact threw wrong exception: "+ex);
        }

        if (countItems(cl) != count+1)
        {
            failTest("Removing null Contact changed ContactList item count!");
        }

        log("Test that removing Contact item decreases ContactList item count by one.");

        try
        {
            c = find(cl, c);
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

        log("Test that removing Contact item not in list does not change ContactList item count.");

        try
        {
            cl.removeContact(c);
            failTest("Removing Contact not in list did not throw any exception!");
        }
        catch (PIMException pime)
        {
            // ok.
        }
        catch (Exception ex)
        {
            failTest("Removing Contact not in list threw wrong exception: "+ex);
        }

        if (countItems(cl) != count)
        {
            failTest("Removing Contact not in list changed ContactList item count!");
        }

        log("Test that removing Contact from a closed list throws PIMException.");
        try
        {
            cl.close();
        }
        catch (Exception ex)
        {
            failTest("Failed to close list: " + ex);
        }

        try
        {
            cl.removeContact(c);
            failTest("Removing Contact from a closed list did not throw any exception!");
        }
        catch (PIMException pime)
        {
            // ok.
        }
        catch (Exception ex)
        {
            failTest("Removing Contact from a closed list threw wrong exception: "+ex);
        }

        cl = openContactList(PIM.READ_WRITE);

        if (countItems(cl) != count)
        {
            failTest("Removing Contact from a closed list changed ContactList item count!");
        }
        try
        {
            cl.close();
        }
        catch (Exception ex)
        {
            failTest("Failed to close list: " + ex);
        }

        passTest();
    }

    private Contact find(ContactList cl, Contact c) throws TestEndThrowable

    {
        String id = c.getString(Contact.UID, 0);
        try
        {
            Enumeration e = cl.items();
            while (e.hasMoreElements())
            {
                Contact tmp = (Contact)e.nextElement();
                String tmpid = tmp.getString(Contact.UID, 0);
                if (tmpid.equals(id))
                {
                    return tmp;
                }
            }
        }
        catch (IllegalArgumentException iae)
        {
            // item does not originate from the list
            return null;
        }
        catch (Exception e)
        {
            failTest("Error while getting items from list!");
        }
        return null;
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

    private ContactList openContactList(int mode) throws TestEndThrowable
    {
        // open first Contact list
        try
        {
            String[] contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            cl = (ContactList)p.openPIMList(PIM.CONTACT_LIST, mode, contactListNames[0]);
            return cl;
        }
        catch (Throwable e)
        {
            failTest("ContactList opening failed: " + e);
        }
        return null;
    }
}
