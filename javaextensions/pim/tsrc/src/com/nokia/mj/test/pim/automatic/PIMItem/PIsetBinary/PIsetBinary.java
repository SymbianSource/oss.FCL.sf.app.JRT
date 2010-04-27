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
package com.nokia.mj.test.pim.automatic.PIMItem.PIsetBinary;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Contact;
import javax.microedition.pim.Event;
import javax.microedition.pim.ToDo;
import javax.microedition.pim.ContactList;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.UnsupportedFieldException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIMItem.setBinary() and
 * javax.microedition.pim.PIMItem.getBinary()
 */
public class PIsetBinary
        extends MIDPTest
{

    public PIsetBinary(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] contactListNames = null;
        String[] eventListNames = null;
        String[] todoListNames = null;
        PIMList[] contactLists = null;
        PIMList[] eventLists = null;
        PIMList[] todoLists = null;
        Enumeration[] contactItems = null;
        Enumeration[] eventItems = null;
        Enumeration[] todoItems = null;
        try
        {
            p = PIM.getInstance();
            contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            todoListNames = p.listPIMLists(PIM.TODO_LIST);
            contactLists = openLists(p, PIM.CONTACT_LIST, contactListNames);
            eventLists = openLists(p, PIM.EVENT_LIST, eventListNames);
            todoLists = openLists(p, PIM.TODO_LIST, todoListNames);
            contactItems = getItems(contactLists);
            eventItems = getItems(eventLists);
            todoItems = getItems(todoLists);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // test steps
        // step s1-s7
        testGetFields(contactItems, contactLists);
        testGetFields(eventItems, eventLists);
        testGetFields(todoItems, todoLists);

        // step s8
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test steps s1 -s7
     */
    private void testGetFields(Enumeration[] aItems,
                               PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aItems.length;
        for (int i = 0; i < listCount; i++)
        {
            testGetFields(aItems[ i ], aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    /**
     * Test steps s1-s7
     */
    private void testGetFields(Enumeration aItems, PIMList aList)
    throws TestEndThrowable
    {
        while (aItems.hasMoreElements())
        {
            PIMItem item = (PIMItem)aItems.nextElement();
            int[] fields = null;
            try
            {
                fields = item.getFields();
            }
            catch (Throwable e)
            {
                failTest("getFields threw: " + e.toString());
            } // end of try-catch
            int fieldCount = fields.length;
            for (int i = 0; i < fieldCount; i++)
            {
                // test steps s2-s7
                testFieldDataType(item, fields[ i ], aList);
            } // end of for (int i = 0; i < fieldCount; i++)
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test steps s2-s7
     */
    private void testFieldDataType(PIMItem aItem, int aField, PIMList aList)
    throws TestEndThrowable
    {
        int dataType = 0;
        try
        {
            dataType = aList.getFieldDataType(aField);
        }
        catch (Throwable e)
        {
            failTest("getFieldDataType threw:" + e.toString());
        } // end of try-catch
        if (dataType == PIMItem.BINARY)
        {
            // test step s3
            byte[] binary = testGetBinary(aItem, aField);
            // test step s4
            testSetBinary(aItem, aField, binary, 0, 0);
            // test step s5
            // Minor deviation from test plan: isModified is not tested
            // for each field, as the method only tests the entire item.
            // however, we still run it only for thos items which we have
            // changed
            testIsModified(aItem);
            // test step s6
            testSetBinary(aItem, aField, binary, -1, 0);
            // test step s7
            testSetBinary(aItem, aField, binary, 0, binary.length - 2);
        } // end of if (dataType = PIMItem.BINARY)
    }

    /**
     * Test step s3
     */
    private byte[] testGetBinary(PIMItem aItem, int aField)
    throws TestEndThrowable
    {
        byte[] data = null;
        try
        {
            data = aItem.getBinary(aField, 0);
        }
        catch (Throwable e)
        {
            failTest("getBinary threw: " + e.toString());
        } // end of try-catch
        return data;
    }

    private void compareData(PIMItem aItem, int aField, byte[] aData)
    throws TestEndThrowable
    {
        byte[] newData = null;
        try
        {
            newData = aItem.getBinary(aField, 0);
        }
        catch (Throwable e)
        {
            failTest("S4: getBinary threw:" + e.toString());
        } // end of try-catch
        int dataLength = aData.length;
        if (newData.length != dataLength)
        {
            failTest("data has changed, datalength:" + newData.length);
        } // end of if (newData.length != dataLength)
        for (int i = 0; i < dataLength; i++)
        {
            if (newData[ i ] != aData[ i ])
            {
                failTest("data has changed at offset:" + i);
            } // end of if (newData[ i ] != aData[ i ])
        } // end of for (int i = 0; i < dataLength; i++)
    }

    /**
     * Test steps s4,s6,s7
     */
    private void testSetBinary(PIMItem aItem, int aField, byte[] aData,
                               int aAttributes, int aOffset)
    throws TestEndThrowable
    {
        try
        {
            aItem.setBinary(aField, 0, aAttributes, aData, aOffset,
                            aData.length);
        }
        catch (Throwable e)
        {
            failTest("setBinary threw: " + e.toString());
        } // end of try-catch
        if ((aAttributes == 0) && (aOffset == 0))
        {
            compareData(aItem, aField, aData);
        } // end of if (( aAttributes == 0 ) && ( aOffset == 0 ))
    }

    /**
     * Test step s5
     */
    private void testIsModified(PIMItem aItem)
    throws TestEndThrowable
    {
        boolean isModified = false;
        try
        {
            isModified = aItem.isModified();
        }
        catch (Throwable e)
        {
            failTest("isModified threw: " + e.toString());
        } // end of try-catch
        if (!isModified)
        {
            failTest("isModified returned false");
        } // end of if (!isModified)
    }


    private PIMList[] openLists(PIM aP, int aListType, String[] aListNames)
    throws PIMException
    {
        int listCount = aListNames.length;
        PIMList[] pimLists = new PIMList[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            pimLists[ i ] = aP.openPIMList(aListType, PIM.READ_WRITE,
                                           aListNames[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
        return pimLists;
    }

    private Enumeration[] getItems(PIMList[] aLists)
    throws PIMException
    {
        int listCount = aLists.length;
        Enumeration[] items = new Enumeration[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            items[ i ] = aLists[ i ].items();
        } // end of for (int i = 0; i < listCount; i++)
        return items;
    }

    private void closeLists(PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            try
            {
                aLists[ i ].close();
            }
            catch (Throwable e)
            {
                failTest("close threw: " + e.toString());
            } // end of try-catch
        } // end of for (int i = 0; i < listCount; i++)
    }

}
