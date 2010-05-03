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
package com.nokia.mj.test.pim.automatic.PIMItem.PIcommit2;
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
 * Tests javax.microedition.pim.PIMItem.commit(),
 * javax.microedition.pim.PIMItem.isModified()
 */
public class PIcommit2
        extends MIDPTest
{

    public PIcommit2(
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
            contactLists = openLists(p, PIM.CONTACT_LIST, PIM.READ_WRITE,
                                     contactListNames);
            eventLists = openLists(p, PIM.EVENT_LIST, PIM.READ_WRITE,
                                   eventListNames);
            todoLists = openLists(p, PIM.TODO_LIST, PIM.READ_WRITE,
                                  todoListNames);
            contactItems = getItems(contactLists);
            eventItems = getItems(eventLists);
            todoItems = getItems(todoLists);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // test steps
        // step s1-s4
        testCommit(contactItems, contactLists);
        testCommit(eventItems, eventLists);
        testCommit(todoItems, todoLists);

        // step s5
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test steps s1
     */
    private void testCommit(Enumeration[] aItems,
                            PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aItems.length;
        for (int i = 0; i < listCount; i++)
        {
            testCommit(aItems[ i ], aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    /**
     * Test steps s1
     */
    private void testCommit(Enumeration aItems, PIMList aList)
    throws TestEndThrowable
    {
        while (aItems.hasMoreElements())
        {
            PIMItem item = (PIMItem)aItems.nextElement();
            testCommit(item, aList);
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test step s1-s4
     */
    private void testCommit(PIMItem aItem, PIMList aList)
    throws TestEndThrowable
    {
        int[] fields = null;
        try
        {
            fields = aItem.getFields();
        }
        catch (Throwable e)
        {
            failTest("getFields threw: " + e.toString());
        } // end of try-catch
        int fieldCount = fields.length;
        boolean stop = false;
        for (int i = 0; (i < fieldCount) && (!stop) ; i++)
        {
            // test step s2
            stop = testFieldDataType(aItem, fields[ i ], aList);
        } // end of for (int i = 0; i < fieldCount; i++)
        // test step s3
        if (!stop)
        {
            log("Item had no string fields, it can't be tested");
            return;
        } // end of if (!stop)

        testIsModified(aItem, true);
        // test step s4
        try
        {
            aItem.commit();
        }
        catch (Throwable e)
        {
            failTest("commit threw: " + e.toString());
        } // end of try-catch
        testIsModified(aItem, false);
    }

    /**
     * Test step s2
     */
    private boolean testFieldDataType(PIMItem aItem, int aField,
                                      PIMList aList)
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
        if (dataType == PIMItem.STRING)
        {
            if ((aItem instanceof Contact) && (aField == Contact.UID))
            {
                return false;
            }
            else if ((aItem instanceof Event) && (aField == Event.UID))
            {
                return false;
            }
            else if ((aItem instanceof ToDo) && (aField == ToDo.UID))
            {
                return false;
            }

            String data = testGetString(aItem, aField);
            testSetString(aItem, aField, data, 0);
            return true;
        }
        return false;
    }

    private String testGetString(PIMItem aItem, int aField)
    throws TestEndThrowable
    {
        String data = null;
        try
        {
            data = aItem.getString(aField, 0);
        }
        catch (Throwable e)
        {
            failTest("getString threw: " + e.toString());
        } // end of try-catch
        return data;
    }

    private void testSetString(PIMItem aItem, int aField, String aData,
                               int aAttributes)
    throws TestEndThrowable
    {
        try
        {
            aItem.setString(aField, 0, aAttributes, aData);
        }
        catch (Throwable e)
        {
            failTest("setString threw: " + e.toString());
        } // end of try-catch
    }
    /**
     * Test step s3, s4
     */
    private void testIsModified(PIMItem aItem, boolean aExpected)
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
        if (isModified != aExpected)
        {
            if (aExpected)
            {
                failTest("isModified returned false");
            } // end of if (aExpected)
            else
            {
                failTest("isModified returned true");
            } // end of else
        } // end of if (!isModified)
    }

    private PIMList[] openLists(PIM aP, int aListType, int aMode,
                                String[] aListNames)
    throws PIMException
    {
        int listCount = aListNames.length;
        PIMList[] pimLists = new PIMList[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            pimLists[ i ] = aP.openPIMList(aListType, aMode,
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
