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
package com.nokia.mj.test.pim.automatic.EventList.ELstringMaxLen;
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
 * EventList.
 *
 * Requirement: a PIMException is thrown when an item with string
 * value or string array element too long to fit in the corresponding
 * native application UI. Maximum editable lengths are specified in
 * Phonebook and Calendar UI specs.
 *
 * The test body is in PLstringMaxLen class.
 */
public class ELstringMaxLen
        extends PLstringMaxLen
{

    public ELstringMaxLen(
        String sTestName,
        j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     * Creates test items.
     */
    public void initTest() throws TestEndThrowable
    {
        //log( "ELstringMaxLen.initTest()" );

        // TEST DATA: Event string field max lengths
        // LOCATION, NOTE, SUMMARY, UID
        iStringFieldMaxLengths.put(new Integer(Event.LOCATION), new Integer(160));
        iStringFieldMaxLengths.put(new Integer(Event.NOTE), new Integer(160));
        iStringFieldMaxLengths.put(new Integer(Event.SUMMARY), new Integer(160));
        iStringFieldMaxLengths.put(new Integer(Event.UID), new Integer(IGNORE));            // internal

        try
        {
            PIM pim = PIM.getInstance();

            iPimList = pim.openPIMList(
                           PIM.EVENT_LIST,
                           PIM.READ_WRITE);

            iPimItem = ((EventList) iPimList).createEvent();
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
        //log( "ELstringMaxLen.cleanupTest()" );
        try
        {
            ((EventList) iPimList).removeEvent((Event) iPimItem);
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
