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
package com.nokia.mj.test.pim.automatic.PIMItem.PIgetFields;
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
 * Tests javax.microedition.pim.PIMItem.getFields(),
 * javax.microedition.pim.PIMItem.countValues() and
 * javax.microedition.pim.PIMItem.isModified()
 */
public class PIgetFields
        extends MIDPTest
{

    public PIgetFields(
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
        // steps s1-s7
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
     * Test steps s1-s7
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
                if (!aList.isSupportedField(fields[ i ]))
                {
                    failTest("getFields returned unsupported field: " +
                             fields[ i ]);
                } // end of if (!aList.isSupportedField( fields[ i ] ))
                // test steps s2-s4
                testCountValues(item, fields[ i ], aList);
            } // end of for (int i = 0; i < fieldCount; i++)
            // test step s5
            testIAE(item, 1);
            // test step s6
            testUFE(item, aList);
            // test step s7
            testIsModified(item);
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test steps s2-s4
     */
    private void testCountValues(PIMItem aItem, int aField, PIMList aList)
    throws TestEndThrowable
    {
        int valueCount = 0;
        try
        {
            valueCount = aItem.countValues(aField);
        }
        catch (Throwable e)
        {
            failTest("countValues threw: " + e.toString());
        } // end of try-catch
        int dataType = 0;
        try
        {
            dataType = aList.getFieldDataType(aField);
        }
        catch (Throwable e)
        {
            failTest("getFieldDataType threw: " + e.toString());
        } // end of try-catch
        // Test step s3
        testGetData(aItem, aField, valueCount, dataType);
        // Test step s4
        testIOOBE(aItem, aField, valueCount, dataType);
    }

    private void getData(PIMItem aItem, int aField, int aIndex,
                         int aDataType)
    throws TestEndThrowable
    {
        switch (aDataType)
        {
        case PIMItem.BINARY:
        {
            aItem.getBinary(aField, aIndex);
            break;
        }
        case PIMItem.BOOLEAN:
        {
            aItem.getBoolean(aField, aIndex);
            break;
        }
        case PIMItem.DATE:
        {
            aItem.getDate(aField, aIndex);
            break;
        }
        case PIMItem.INT:
        {
            aItem.getInt(aField, aIndex);
            break;
        }
        case PIMItem.STRING:
        {
            aItem.getString(aField, aIndex);
            break;
        }
        case PIMItem.STRING_ARRAY:
        {
            aItem.getStringArray(aField, aIndex);
            break;
        }
        default:
        {
            failTest("unknown datatype: " + aDataType);
            break;
        }
        } // end of switch (aDataType)
    }

    /**
     * Test step s3
     */
    private void testGetData(PIMItem aItem, int aField, int aValueCount,
                             int aDataType)
    throws TestEndThrowable
    {
        for (int i = 0; i < aValueCount; i++)
        {
            try
            {
                getData(aItem, aField, i, aDataType);
            }
            catch (Throwable e)
            {
                failTest("get<datatype> threw: " + e.toString());
            } // end of try-catch
        } // end of for (int i = 0; i < aValueCount; i++)
    }

    /**
     * Test step s4
     */
    private void testIOOBE(PIMItem aItem, int aField, int aValueCount,
                           int aDataType)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            getData(aItem, aField, -1, aDataType);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            exceptionThrown = true;
        } // end of catch
        catch (Throwable e)
        {
            failTest("get<datatype> threw instead of IOOBE: "
                     + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("getData did not throw IOOBE for -1");
        } // end of if (!exceptionThrown)
        exceptionThrown = false;
        try
        {
            getData(aItem, aField, aValueCount, aDataType);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            exceptionThrown = true;
        } // end of catch
        catch (Throwable e)
        {
            failTest("get<datatype> threw instead of IOOBE: "
                     + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("getData did not throw IOOBE for " + aValueCount);
        } // end of if (!exceptionThrown)
    }

    /**
     * Test step s5
     */
    private void testIAE(PIMItem aItem, int aField)
    throws TestEndThrowable
    {
        // 0 = BINARY, 6 = one larger than the largest data type
        for (int i = 0; i < 6; i++)
        {
            boolean exceptionThrown = false;
            try
            {
                getData(aItem, aField, 0, i);
            }
            catch (java.lang.IllegalArgumentException e)
            {
                exceptionThrown = true;
            } // end of try-catch
            catch (UnsupportedFieldException e)
            {
                failTest("getData threw UFE: " + e.getMessage() +
                         ", dataType: " + i);
            } // end of catch

            catch (Throwable e)
            {
                failTest("getData threw instead of IAE: " + e.toString());
            } // end of catch
            if (!exceptionThrown)
            {
                failTest("getData did not throw IAE");
            } // end of if (!exceptionThrown)
        } // end of for (int i = 0; i < 6; i++)
    }

    /**
     * Test step s6
     */
    private void testUFE(PIMItem aItem, PIMList aList)
    throws TestEndThrowable
    {
        int maxField = 0;
        if (aItem instanceof Contact)
        {
            maxField = Contact.URL + 1;
        }
        if (aItem instanceof Event)
        {
            maxField = Event.UID + 1;
        }
        if (aItem instanceof ToDo)
        {
            maxField = ToDo.UID + 1;
        }
        for (int i = Contact.ADDR; i < maxField; i++)
        {
            if (!aList.isSupportedField(i))
            {
                // FIXED: do not test with illegal field data types
                //for( int j = 0; j < 6; j++ )
                //    {
                boolean exceptionThrown = false;
                try
                {
                    getData(aItem, i, 0, aList.getFieldDataType(i));
                }
                catch (UnsupportedFieldException e)
                {
                    exceptionThrown = true;
                } // end of try-catch
                catch (Throwable e)
                {
                    failTest("getData threw instead of UFE: "
                             + e.toString());
                } // end of catch
                if (!exceptionThrown)
                {
                    failTest("getData did not throw UFE");
                } // end of if (!exceptionThrown)
                //    } // end of for (int j = 0; j < 6; j++)
            } // end of if (!aList.isSupportedField( i ))
        }
    }

    /**
     * Test step s7
     */
    private void testIsModified(PIMItem aItem)
    throws TestEndThrowable
    {
        if (aItem.isModified())
        {
            failTest("isModified returned true");
        } // end of if (aItem.isModified())
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
