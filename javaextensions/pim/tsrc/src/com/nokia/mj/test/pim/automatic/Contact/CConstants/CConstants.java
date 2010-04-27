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
package com.nokia.mj.test.pim.automatic.Contact.CConstants;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.Contact;
import com.nokia.mj.test.pim.adaptation.*;


// CLASS DEFINITION
/**
 * Tests that Contact class constants equal specification.
 *
 */
public class CConstants
        extends MIDPTest
{

    public CConstants(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        if (Contact.ADDR_POBOX != 0)
        {
            failTest("Contact.ADDR_POBOX != 0");
        }
        if (Contact.NAME_FAMILY != 0)
        {
            failTest("Contact.NAME_FAMILY != 0");
        }
        if (Contact.ADDR_EXTRA != 1)
        {
            failTest("Contact.ADDR_EXTRA != 1");
        }
        if (Contact.ATTR_ASST != 1)
        {
            failTest("Contact.ATTR_ASST != 1");
        }
        if (Contact.NAME_GIVEN != 1)
        {
            failTest("Contact.NAME_GIVEN != 1");
        }
        if (Contact.ADDR_STREET != 2)
        {
            failTest("Contact.ADDR_STREET != 2");
        }
        if (Contact.ATTR_AUTO != 2)
        {
            failTest("Contact.ATTR_AUTO != 2");
        }
        if (Contact.NAME_OTHER != 2)
        {
            failTest("Contact.NAME_OTHER != 2");
        }
        if (Contact.ADDR_LOCALITY != 3)
        {
            failTest("Contact.ADDR_LOCALITY != 3");
        }
        if (Contact.NAME_PREFIX != 3)
        {
            failTest("Contact.NAME_PREFIX != 3");
        }
        if (Contact.ADDR_REGION != 4)
        {
            failTest("Contact.ADDR_REGION != 4");
        }
        if (Contact.ATTR_FAX != 4)
        {
            failTest("Contact.ATTR_FAX != 4");
        }
        if (Contact.NAME_SUFFIX != 4)
        {
            failTest("Contact.NAME_SUFFIX != 4");
        }
        if (Contact.ADDR_POSTALCODE != 5)
        {
            failTest("Contact.ADDR_POSTALCODE != 5");
        }
        if (Contact.ADDR_COUNTRY != 6)
        {
            failTest("Contact.ADDR_COUNTRY != 6");
        }
        if (Contact.ATTR_HOME != 8)
        {
            failTest("Contact.ATTR_HOME != 8");
        }
        if (Contact.ATTR_MOBILE != 16)
        {
            failTest("Contact.ATTR_MOBILE != 16");
        }
        if (Contact.ATTR_OTHER != 32)
        {
            failTest("Contact.ATTR_OTHER != 32");
        }
        if (Contact.ATTR_PAGER != 64)
        {
            failTest("Contact.ATTR_PAGER != 64");
        }
        if (Contact.ADDR != 100)
        {
            failTest("Contact.ADDR != 100");
        }
        if (Contact.BIRTHDAY != 101)
        {
            failTest("Contact.BIRTHDAY != 101");
        }
        if (Contact.CLASS != 102)
        {
            failTest("Contact.CLASS != 102");
        }
        if (Contact.EMAIL != 103)
        {
            failTest("Contact.EMAIL != 103");
        }
        if (Contact.FORMATTED_ADDR != 104)
        {
            failTest("Contact.FORMATTED_ADDR != 104");
        }
        if (Contact.FORMATTED_NAME != 105)
        {
            failTest("Contact.FORMATTED_NAME != 105");
        }
        if (Contact.NAME != 106)
        {
            failTest("Contact.NAME != 106");
        }
        if (Contact.NICKNAME != 107)
        {
            failTest("Contact.NICKNAME != 107");
        }
        if (Contact.NOTE != 108)
        {
            failTest("Contact.NOTE != 108");
        }
        if (Contact.ORG != 109)
        {
            failTest("Contact.ORG != 109");
        }
        if (Contact.PHOTO != 110)
        {
            failTest("Contact.PHOTO != 110");
        }
        if (Contact.PHOTO_URL != 111)
        {
            failTest("Contact.PHOTO_URL != 111");
        }
        if (Contact.PUBLIC_KEY != 112)
        {
            failTest("Contact.PUBLIC_KEY != 112");
        }
        if (Contact.PUBLIC_KEY_STRING != 113)
        {
            failTest("Contact.PUBLIC_KEY_STRING != 113");
        }
        if (Contact.REVISION != 114)
        {
            failTest("Contact.REVISION != 114");
        }
        if (Contact.TEL != 115)
        {
            failTest("Contact.TEL != 115");
        }
        if (Contact.TITLE != 116)
        {
            failTest("Contact.TITLE != 116");
        }
        if (Contact.UID != 117)
        {
            failTest("Contact.UID != 117");
        }
        if (Contact.URL != 118)
        {
            failTest("Contact.URL != 118");
        }
        if (Contact.ATTR_PREFERRED != 128)
        {
            failTest("Contact.ATTR_PREFERRED != 128");
        }
        if (Contact.CLASS_CONFIDENTIAL != 200)
        {
            failTest("Contact.CLASS_CONFIDENTIAL != 200");
        }
        if (Contact.CLASS_PRIVATE != 201)
        {
            failTest("Contact.CLASS_PRIVATE != 201");
        }
        if (Contact.CLASS_PUBLIC != 202)
        {
            failTest("Contact.CLASS_PUBLIC != 202");
        }
        if (Contact.ATTR_SMS != 256)
        {
            failTest("Contact.ATTR_SMS != 256");
        }
        if (Contact.ATTR_WORK != 512)
        {
            failTest("Contact.ATTR_WORK != 512");
        }
        passTest();
    }
}
