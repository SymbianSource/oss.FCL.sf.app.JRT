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
package com.nokia.mj.test.pim.automatic.ContactList.CLstringMaxLen;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.Hashtable;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * Tests maximum lengths of string values and string array elements in
 * ContactList.
 *
 * Requirement: a PIMException is thrown when an item with string
 * value or string array element too long to fit in the corresponding
 * native application UI. Maximum editable lengths are specified in
 * Phonebook and Calendar UI specs.
 *
 * The test body is in PLstringMaxLen class.
 */
public class CLstringMaxLen
        extends PLstringMaxLen
{

    public CLstringMaxLen(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     * Creates test items.
     */
    public void initTest() throws TestEndThrowable
    {
        log("CLstringMaxLen.initTest()");

        // TEST DATA: Contact string field max lengths
        iStringFieldMaxLengths.put(new Integer(Contact.EMAIL), new Integer(150));
        iStringFieldMaxLengths.put(new Integer(Contact.FORMATTED_NAME), new Integer(IGNORE));       // not supported
        iStringFieldMaxLengths.put(new Integer(Contact.NICKNAME), new Integer(50));             // not supported
        iStringFieldMaxLengths.put(new Integer(Contact.NOTE), new Integer(1000));
        iStringFieldMaxLengths.put(new Integer(Contact.ORG), new Integer(50));
        iStringFieldMaxLengths.put(new Integer(Contact.TEL), new Integer(48));
        iStringFieldMaxLengths.put(new Integer(Contact.TITLE), new Integer(50));
        iStringFieldMaxLengths.put(new Integer(Contact.UID), new Integer(IGNORE));                  // internal
        iStringFieldMaxLengths.put(new Integer(Contact.URL), new Integer(1000));
        iStringFieldMaxLengths.put(new Integer(0x1005001), new Integer(50));                     // WV ID
        iStringFieldMaxLengths.put(new Integer(0x1005002), new Integer(100));                    // SIP
        iStringFieldMaxLengths.put(new Integer(0x1005003), new Integer(60));                     // DTMF
        iStringFieldMaxLengths.put(new Integer(0x1005004), new Integer(50));                     // Department
        iStringFieldMaxLengths.put(new Integer(0x1005005), new Integer(50));                     // Assistant name
        iStringFieldMaxLengths.put(new Integer(0x1005006), new Integer(50));                     // Children
        iStringFieldMaxLengths.put(new Integer(0x1005007), new Integer(50));                     // Spouse
        iStringFieldMaxLengths.put(new Integer(0x1005009), new Integer(100));                    // VOIP
        iStringFieldMaxLengths.put(new Integer(0x100500A), new Integer(100));                    // PTT
        iStringFieldMaxLengths.put(new Integer(0x100500B), new Integer(100));                    // SWIS

        // TEST DATA: Contact NAME string array element max lengths

        int[] nameElemLengths = new int[ 7 ];
        nameElemLengths[ 0 ] = 50; // NAME_FAMILY
        nameElemLengths[ 1 ] = 50; // NAME_GIVEN
        nameElemLengths[ 2 ] = 50; // NAME_OTHER
        nameElemLengths[ 3 ] = 10; // NAME_PREFIX
        nameElemLengths[ 4 ] = 10; // NAME_SUFFIX
        nameElemLengths[ 5 ] = 50; // NAME_READING
        nameElemLengths[ 6 ] = 50; // NAME_READING

        // TEST DATA: Contact ADDR string array element max lengths

        int[] addrElemLengths = new int[ 7 ];
        addrElemLengths[ 0 ] = 20; // ADDR_POBOX
        addrElemLengths[ 1 ] = 50; // ADDR_EXTRA
        addrElemLengths[ 2 ] = 50; // ADDR_STREET
        addrElemLengths[ 3 ] = 50; // ADDR_LOCALITY
        addrElemLengths[ 4 ] = 50; // ADDR_REGION
        addrElemLengths[ 5 ] = 20; // ADDR_POSTALCODE
        addrElemLengths[ 6 ] = 50; // ADDR_COUNTRY

        iStringArrayElemMaxLengths.put(new Integer(Contact.NAME), nameElemLengths);
        iStringArrayElemMaxLengths.put(new Integer(Contact.ADDR), addrElemLengths);

        try
        {
            PIM pim = PIM.getInstance();

            iPimList = pim.openPIMList(
                           PIM.CONTACT_LIST,
                           PIM.READ_WRITE);

            iPimItem = ((ContactList) iPimList).createContact();
            iPimItem.commit();
        }
        catch (PIMException pe)
        {
            failTest("Unexpected PIMException in initTest().");
            pe.printStackTrace();
        }
    }

    /**
     * Removes the test items.
     */
    public void cleanupTest()
    {
        log("CLstringMaxLen.cleanupTest()");
        try
        {
            ((ContactList) iPimList).removeContact((Contact) iPimItem);
            iPimList.close();
        }
        catch (PIMException pe)
        {
            // cannot call failTest
            log("Unexpected PIMException in cleanupTest().");
            pe.printStackTrace();
        }
    }
}

// End of Filé
