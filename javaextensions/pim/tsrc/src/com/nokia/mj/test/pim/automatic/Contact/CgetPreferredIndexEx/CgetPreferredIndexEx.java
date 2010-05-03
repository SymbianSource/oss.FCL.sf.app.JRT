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
package com.nokia.mj.test.pim.automatic.Contact.CgetPreferredIndexEx;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Contact;
import javax.microedition.pim.ContactList;
import javax.microedition.pim.UnsupportedFieldException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.Contact.getPreferredIndex()
 *
 */
public class CgetPreferredIndexEx
        extends MIDPTest
{

    public CgetPreferredIndexEx(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public CgetPreferredIndexEx()
    {
        log("DEBUG Constructor");
    }

    public void initTest() throws TestEndThrowable
    {
        log("DEBUG initTest");
    }

    public void runTest() throws TestEndThrowable
    {
        initTest();
        log("DEBUG 1");
        // preconditions
        PIM p = null;
        ContactList contactList = null;
        Contact c1 = null;
        int unsupportedField = 0;
        log("DEBUG 2");
        try
        {
            p = PIM.getInstance();
            contactList =
                (ContactList)p.openPIMList(PIM.CONTACT_LIST, PIM.READ_ONLY);
            c1 = contactList.createContact();
            // 119 is the largest field + 1
            log("DEBUG 3");
            for (int i = Contact.ADDR; i < 119; i++)
            {
                log("DEBUG 4");
                if (!contactList.isSupportedField(i))
                {
                    unsupportedField = i;
                    break;
                } // end of if (!contactList.isSupportedField( i ))
            }
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch


        // Test steps
        // step s1
        log("DEBUG 5");
        testIAE(c1, 888);

        // step s2
        log("DEBUG 6");
        testIAE(c1, -1);

        // step s3
        log("DEBUG 7");
        testUFE(c1, unsupportedField);

        passTest();
    }

    /**
     * Test steps s1, s2
     */
    private void testIAE(Contact aContact, int aField)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            log("DEBUG 8");
            aContact.getPreferredIndex(aField);
            log("DEBUG 9");
        }
        catch (IllegalArgumentException e)
        {
            log("DEBUG 10");
            exceptionThrown = true;
        } // end of catch
        catch (Throwable e)
        {
            log("DEBUG 11");
            failTest("Not IAE: " + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            log("DEBUG 12");
            failTest("IAE not thrown for: " + aField);
        } // end of if (!exceptionThrown)
    }

    /**
     * Test step s3
     */
    private void testUFE(Contact aContact, int aField)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            log("DEBUG 13");
            aContact.getPreferredIndex(aField);
            log("DEBUG 14");
        }
        catch (UnsupportedFieldException e)
        {
            log("DEBUG 15");
            exceptionThrown = true;
        } // end of catch
        catch (Throwable e)
        {
            log("DEBUG 16");
            failTest("Not UFE: " + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            log("DEBUG 17");
            failTest("UFE not thrown for: " + aField);
        } // end of if (!exceptionThrown)
    }

}


