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
package com.nokia.mj.test.pim.automatic.ContactList.CLcreateContact;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ContactList.createContact()
 *
 */
public class CLcreateContact
        extends MIDPTest
{

    public CLcreateContact(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] contactListNames = null;
        ContactList cl = null;
        int count = 0;

        // open first Contact list
        try
        {
            p = PIM.getInstance();
            contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            cl = (ContactList)p.openPIMList(PIM.CONTACT_LIST, PIM.READ_WRITE, contactListNames[0]);

            Enumeration e = cl.items();
            count = itemCount(e);
        }
        catch (Throwable t)
        {
            failTest("preconditions failed: " + t);
        }

        log("ContactList has "+ count + " items.");

        log("Test that creating new Contact item does not increase item count in ContactList.");

        Contact c = null;

        try
        {
            c = cl.createContact();

            Enumeration en = cl.items();
            cl.close();

            if (itemCount(en) != count)
            {
                failTest("Creating new Contact changed ContactList item count!");
            }
        }
        catch (Throwable t)
        {
            failTest("Failed to test Contact item creation: " + t);
        }

        log("Test that created Contact item is empty.");
        int[] fields = c.getFields();
        if (fields.length != 0)
        {
            failTest("Created new Contact item has not got all fields empty!");
        }

        passTest();
    }

    private int itemCount(Enumeration e)
    {
        int tmp = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            tmp++;
        }
        return tmp;
    }
}
