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
package com.nokia.mj.test.pim.automatic.PIMList.PLattributes;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.items()
 */
public class PLattributes
        extends MIDPTest
{

    public PLattributes(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // PIMItem.ATTR_NONE is not tested because it is not clear should it be returned
    // by getSupportedAttributes() and what isSupportedAttribute() should return.
    private int[] contactAttributes =
    {
        Contact.ATTR_ASST, Contact.ATTR_AUTO, Contact.ATTR_FAX,
        Contact.ATTR_HOME, Contact.ATTR_MOBILE, Contact.ATTR_OTHER,
        Contact.ATTR_PAGER, Contact.ATTR_PREFERRED, Contact.ATTR_SMS,
        Contact.ATTR_WORK
    };
    private int[] eventAttributes = { };
    private int[] todoAttributes = { };

    private int[] contactFields = { Contact.ADDR, Contact.BIRTHDAY, Contact.CLASS,
                                    Contact.EMAIL, Contact.FORMATTED_ADDR, Contact.FORMATTED_NAME,
                                    Contact.NAME, Contact.NICKNAME, Contact.NOTE, Contact.ORG,
                                    Contact.PHOTO, Contact.PHOTO_URL, Contact.PUBLIC_KEY,
                                    Contact.PUBLIC_KEY_STRING, Contact.REVISION, Contact.TEL,
                                    Contact.TITLE, Contact.UID, Contact.URL
                                  };
    private int[] eventFields = { Event.ALARM, Event.CLASS, Event.END,
                                  Event.LOCATION, Event.NOTE, Event.REVISION, Event.START,
                                  Event.SUMMARY, Event.UID
                                };
    private int[] todoFields = { ToDo.CLASS, ToDo.COMPLETED, ToDo.COMPLETION_DATE,
                                 ToDo.DUE, ToDo.NOTE, ToDo.PRIORITY, ToDo.REVISION,
                                 ToDo.SUMMARY, ToDo.UID
                               };

    private int[] invalidFields = { -100, -1, 0, 666, 2147483647 };
    private int[] invalidAttributes = { -100, -1, 2147483647 };

    private Hashtable supportedFields = new Hashtable();
    private Hashtable unsupportedFields = new Hashtable();


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIMList[] allLists = openAllLists(PIM.READ_WRITE);
        classifyFields(allLists);

        // Test steps
        // step s1-s4
        testAttributes(allLists);

        // step s5
        testAttributesUnsupportedFields(allLists);

        // step s6
        testAttributesIllegalFields(allLists);

        // step s7 & 8
        testIsSupportedFalse(allLists);

        // step 9
        testGetAttributeLabelUFExceptions(allLists);

        // step 10
        testGetAttributeLabelIAExceptions(allLists);

        // step s11
        closeLists(allLists);

        passTest();
    }

    private void classifyFields(PIMList[] lists) throws TestEndThrowable
    {
        for (int i=0; i < lists.length; i++)
        {
            try
            {
                int[] fields = lists[i].getSupportedFields();
                String name = lists[i].getName();
                supportedFields.put(name, fields);

                log("supported fields for '"+name+"': "+ asString(fields));

                int[] tmpFields = null;
                if (lists[i] instanceof ContactList)
                {
                    tmpFields = valuesNotInSecondList(contactFields, fields);
                }
                else if (lists[i] instanceof EventList)
                {
                    tmpFields = valuesNotInSecondList(eventFields, fields);
                }
                else if (lists[i] instanceof ToDoList)
                {
                    tmpFields = valuesNotInSecondList(todoFields, fields);
                }
                unsupportedFields.put(lists[i].getName(), tmpFields);

                log("unsupported fields for '"+name+"': "+ asString(tmpFields));
            }
            catch (Exception ex)
            {
                failTest("Error while classifying fields: " + ex);
            }
        }
    }

    private String asString(int[] values)
    {
        String ret = "";
        for (int i=0; i < values.length-1; i++)
        {
            ret += values[i] +", ";
        }
        if (values.length > 0)
        {
            ret += values[values.length-1];
        }
        return ret;
    }

    // returns array containing those values in the first list
    // that are not contained in the  second list
    private int[] valuesNotInSecondList(int[] first, int[] second)
    {
        int[] ret = new int[ first.length ];
        int index = 0;

        for (int i=0; i<first.length; i++)
        {
            boolean found = false;
            for (int j=0; j<second.length; j++)
            {
                if (first[i] == second[j])
                {
                    found = true;
                }
            }
            if (!found)
            {
                ret[index++] = first[i];
            }
        }

        // remove empty space from the array
        int[] ret2 = new int[ index ];
        for (int i=0; i<ret2.length; i++)
        {
            ret2[i] = ret[i];
        }
        return ret2;
    }

    private void testGetAttributeLabelUFExceptions(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            // get attributes supported in this list
            Vector supported = new Vector();
            int[] fields = (int[]) supportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                int[] attributes = lists[i].getSupportedAttributes(fields[j]);
                addAttributesTo(attributes, supported);
            }
            //test that no exception is thrown for supported attributes
            for (int j = 0; j<supported.size(); j++)
            {
                Integer tmp = (Integer)supported.elementAt(j);
                try
                {
                    lists[i].getAttributeLabel(tmp.intValue());
                }
                catch (Exception e)
                {
                    failTest("getAttributeLabel(supported) threw: " + e);
                }
            }

            // get those attributes that are not supported
            Vector unsupported = new Vector();
            int[] attributes = null;
            if (lists[i] instanceof ContactList)
            {
                attributes = contactAttributes;
            }
            else if (lists[i] instanceof EventList)
            {
                attributes = eventAttributes;
            }
            else if (lists[i] instanceof ToDoList)
            {
                attributes = todoAttributes;
            }
            // add all possible attributes...
            for (int j=0; j< attributes.length; j++)
            {
                unsupported.addElement(new Integer(attributes[j]));
            }
            // ...remove those that are supported.
            for (int j=0; j< supported.size(); j++)
            {
                if (unsupported.contains((Integer)supported.elementAt(j)))
                {
                    unsupported.removeElement((Integer)supported.elementAt(j));
                }
            }
            //test that UnsupportedFieldException is thrown for unsupported attributes
            for (int j = 0; j<unsupported.size(); j++)
            {
                Integer tmp = (Integer)unsupported.elementAt(j);
                try
                {
                    lists[i].getAttributeLabel(tmp.intValue());
                    failTest("getAttributeLabel(unsupported) did not throw UnsupportedFieldException!");
                }
                catch (UnsupportedFieldException ufe)
                {
                    //ok
                }
                catch (Exception e)
                {
                    failTest("getAttributeLabel(unsupported) threw: " + e);
                }
            }
        }
    }

    private void addAttributesTo(int[] attributes, Vector v)
    {
        for (int i=0; i < attributes.length; i++)
        {
            Integer tmp = new Integer(attributes[i]);
            if (!v.contains(tmp))
            {
                v.addElement(tmp);
            }
        }
    }

    private void testGetAttributeLabelIAExceptions(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            // test invalid attributes first
            int[] attributes = null;
            if (lists[i] instanceof ContactList)
            {
                attributes = contactAttributes;
            }
            else if (lists[i] instanceof EventList)
            {
                attributes = eventAttributes;
            }
            else if (lists[i] instanceof ToDoList)
            {
                attributes = todoAttributes;
            }
            for (int j = -100; (j < 5000) && (j < PIMItem.EXTENDED_ATTRIBUTE_MIN_VALUE); j++)
            {
                boolean found = false;
                for (int k=0; k< attributes.length; k++)
                {
                    if (j == attributes[k])
                    {
                        found = true;
                    }
                }
                // do not test valid values
                if (!found)
                {
                    try
                    {
                        lists[i].getAttributeLabel(j);
                        log("List: "+lists[i].getName()+", attribute: "+j);
                        failTest("getAttributeLabel(<invalid>) did not throw IllegalArgumentException!");
                    }
                    catch (IllegalArgumentException iae)
                    {
                        //ok
                    }
                    catch (Exception e)
                    {
                        failTest("getAttributeLabel(<invalid>) did not throw IllegalArgumentException but: " + e);
                    }
                }
            }
        }
    }

    private void testIsSupportedFalse(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            try
            {
                int[] fields = (int[]) supportedFields.get(lists[i].getName());

                for (int j = 0; j<fields.length; j++)
                {
                    //test first with valid field but unsupported attribute
                    int[] attributes = lists[i].getSupportedAttributes(fields[j]);

                    int[] unsupportedAttributes = null;
                    if (lists[i] instanceof ContactList)
                    {
                        unsupportedAttributes = valuesNotInSecondList(contactAttributes, attributes);
                    }
                    else if (lists[i] instanceof EventList)
                    {
                        unsupportedAttributes = valuesNotInSecondList(eventAttributes, attributes);
                    }
                    else if (lists[i] instanceof ToDoList)
                    {
                        unsupportedAttributes = valuesNotInSecondList(todoAttributes, attributes);
                    }

                    for (int k = 0; k < unsupportedAttributes.length; k++)
                    {
                        if (lists[i].isSupportedAttribute(fields[j], unsupportedAttributes[k]))
                        {
                            log("List: "+lists[i].getName()+", Field: "+fields[j]+", attribute: "+unsupportedAttributes[k]);
                            failTest("isSupportedAttribute() returned true for valid field & unsupported attribute.");
                        }
                    }
                    //test also with valid field but invalid attribute
                    for (int k = 0; k < invalidAttributes.length; k++)
                    {
                        if (lists[i].isSupportedAttribute(fields[j], invalidAttributes[k]))
                        {
                            failTest("isSupportedAttribute() returned true for valid field & invalid attribute ");
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                failTest("Error while testing isSupported(): " + ex);
            }
        }
    }

    private void testAttributesUnsupportedFields(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            int[] fields = (int[]) unsupportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                try
                {
                    int[] attributes = lists[i].getSupportedAttributes(fields[j]);
                    failTest("No exception was thrown for PIMList.getSupportedAttributes(<unsupportedfield>)");
                }
                catch (UnsupportedFieldException ufe)
                {
                    // ok
                }
                catch (Exception ex)
                {
                    log("list: "+lists[i].getName()+", field: "+fields[j]);
                    failTest("UnsupportedFieldException not thrown but: " + ex);
                }
            }
        }
    }

    private void testAttributesIllegalFields(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            for (int j = 0; j<invalidFields.length; j++)
            {
                try
                {
                    int[] attributes = lists[i].getSupportedAttributes(invalidFields[j]);
                    failTest("No exception was thrown for PIMList.getSupportedAttributes(<invalidfield>)");
                }
                catch (IllegalArgumentException iae)
                {
                    // ok
                }
                catch (Exception ex)
                {
                    failTest("IllegalArgumentException not thrown but: " + ex);
                }
            }
        }
    }

    private void testAttributes(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            try
            {
                int[] fields = (int[]) supportedFields.get(lists[i].getName());
                for (int j = 0; j<fields.length; j++)
                {
                    int[] attributes = lists[i].getSupportedAttributes(fields[j]);
                    for (int k = 0; k<attributes.length; k++)
                    {

                        if (!lists[i].isSupportedAttribute(fields[j], attributes[k]))
                        {
                            failTest("PIMList.isSupportedAttribute("+fields[j]+", "+
                                     attributes[k]+") failed for list "+lists[i].getName());
                        }
                        String label = lists[i].getAttributeLabel(attributes[k]);
                        if (label == null)
                        {
                            failTest("PIMList.getAttributeLabel("+ attributes[k] +
                                     ") returned null label for list "+lists[i].getName());
                        }
                        else if (label.length() == 0)
                        {
                            failTest("PIMList.getAttributeLabel(" + attributes[k] +
                                     ") returned empty label for list " + lists[i].getName());
                        }
                        log(lists[i].getName()+".field:(" + fields[j] +
                            "), attribute "+attributes[k]+" label: "+label);
                    }
                }
            }
            catch (Exception ex)
            {
                failTest("Error while testing attributes: " + ex);
            }
        }
    }

    private void closeLists(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                lists[i].close();
            }
        }
        catch (Exception e)
        {
            failTest("Error while closing lists: " +e);
        }
    }

    private PIMList[] openAllLists(int mode) throws TestEndThrowable
    {
        try
        {
            PIM p = PIM.getInstance();
            String[] contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            String[] eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            String[] todoListNames = p.listPIMLists(PIM.TODO_LIST);
            PIMList[] contactLists = openLists(p, PIM.CONTACT_LIST, contactListNames, mode);
            PIMList[] eventLists = openLists(p, PIM.EVENT_LIST, eventListNames, mode);
            PIMList[] todoLists = openLists(p, PIM.TODO_LIST, todoListNames, mode);

            return combineLists(contactLists, eventLists, todoLists);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        }
        return null;
    }

    private PIMList[] openLists(PIM aP, int aListType, String[] aNames, int aMode)
    throws PIMException
    {
        PIMList[] pimLists = new PIMList[ aNames.length ];
        for (int i = 0; i < aNames.length; i++)
        {
            pimLists[ i ] = aP.openPIMList(aListType, aMode, aNames[ i ]);
        }
        return pimLists;
    }

    private PIMList[] combineLists(PIMList[] pl1, PIMList[] pl2, PIMList[] pl3)
    {
        PIMList[] pimLists = new PIMList[ pl1.length + pl2.length + pl3.length ];
        int i = 0;
        for (int j=0; j < pl1.length; j++)
        {
            pimLists[ i++ ] = pl1[j];
        }
        for (int j=0; j < pl2.length; j++)
        {
            pimLists[ i++ ] = pl2[j];
        }
        for (int j=0; j < pl3.length; j++)
        {
            pimLists[ i++ ] = pl3[j];
        }
        return pimLists;
    }
}
