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
package com.nokia.mj.test.pim.automatic.Contact.CgetPreferredIndex;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Contact;
import javax.microedition.pim.ContactList;
import com.nokia.mj.test.pim.adaptation.*;


// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.Contact.getPreferredIndex()
 *
 */
public class CgetPreferredIndex
        extends MIDPTest
{

    public CgetPreferredIndex(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        ContactList contactList = null;
        Contact c1 = null;
        try
        {
            p = PIM.getInstance();
            contactList =
                (ContactList)p.openPIMList(PIM.CONTACT_LIST, PIM.READ_ONLY);
            c1 = contactList.createContact();
            String[] nameArray = new String[ contactList.stringArraySize(Contact.NAME)];
            nameArray[ 0 ] = "Smith";
            nameArray[ 1 ] = "John";
            c1.addStringArray(Contact.NAME, PIMItem.ATTR_NONE, nameArray);
            if (!contactList.isSupportedField(Contact.TEL))
            {
                failTest("TEL is not Supported");
            } // end of if (!contactList.isSupportedField( Contact.TEL ))
            c1.addString(Contact.TEL, PIMItem.ATTR_NONE, "+1234567890");
            if (!contactList.isSupportedAttribute(Contact.TEL,
                                                  Contact.ATTR_PREFERRED))
            {
                log("ATTR_PREFERRED is not supported, cannot be tested.");
                passTest();
            }
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        } // end of try-catch


        // Test steps
        // step s1
        testGetPreferredIndex(c1, Contact.TEL, -1);

        // step s2
        testAddString(c1, Contact.TEL, Contact.ATTR_PREFERRED,
                      "+0987654321");

        // step s3
        testGetPreferredIndex(c1, Contact.TEL, 1);

        // step s4
        testGetAttributes(c1, Contact.TEL, 0, 0);

        // step s5
        testGetAttributes(c1, Contact.TEL, 1, Contact.ATTR_PREFERRED);

        // step s6
        testSetString(c1, Contact.TEL, 0, Contact.ATTR_PREFERRED,
                      "+1234567890");

        // step s7
        testGetPreferredIndex(c1, Contact.TEL, 0);

        // step s8
        testRemoveValue(c1, Contact.TEL, 0);

        // test step s9
        testGetPreferredIndex(c1, Contact.TEL, -1);

        passTest();
    }

    /**
     * Test steps s1, s3, s7, s9
     */
    private void testGetPreferredIndex(Contact aContact, int aField,
                                       int aExpected)
    throws TestEndThrowable
    {
        int preferredIndex = 0;
        try
        {
            preferredIndex = aContact.getPreferredIndex(aField);

        }
        catch (Throwable e)
        {
            failTest("getPreferredIndex threw: " + e.toString());
        } // end of try-catch
        if (preferredIndex != aExpected)
        {
            failTest("getPreferredIndex returned " + preferredIndex +
                     ", expecting " + aExpected);
        } // end of if (preferredIndex != aExpected)
    }

    /**
     * Test step s2
     */
    private void testAddString(Contact aContact, int aField, int aAttributes,
                               String aString)
    throws TestEndThrowable
    {
        try
        {
            aContact.addString(aField, aAttributes, aString);
        }
        catch (Throwable e)
        {
            failTest("addString threw: " + e.toString());
        } // end of try-catch
    }

    /**
     * Test step s4
     */
    private void testGetAttributes(Contact aContact, int aField, int aIndex,
                                   int aExpected)
    throws TestEndThrowable
    {
        int attributes = 0;
        try
        {
            attributes = aContact.getAttributes(aField, aIndex);
        }
        catch (Throwable e)
        {
            failTest("getAttributes threw: " + e.toString());
        } // end of try-catch
        if (attributes != aExpected)
        {
            failTest("getAttributes returned " + attributes +
                     ", instead of " + aExpected);
        }
    }

    /**
     * Test step s6
     */
    private void testSetString(Contact aContact, int aField, int aIndex,
                               int aAttributes,
                               String aString)
    throws TestEndThrowable
    {
        try
        {
            aContact.setString(aField, aIndex, aAttributes, aString);
        }
        catch (Throwable e)
        {
            failTest("steString threw: " + e.toString());
        } // end of try-catch
    }

    /**
     * Test step s8
     */
    private void testRemoveValue(Contact aContact, int aField, int aIndex)
    throws TestEndThrowable
    {
        try
        {
            aContact.removeValue(aField, aIndex);
        }
        catch (Throwable e)
        {
            failTest("removeValue threw: " + e.toString());
        } // end of try-catch
    }

}


