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
package com.nokia.mj.test.pim.automatic;
//IMPORTS
import java.io.*;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.toSerialFormat()
 */
public class PtoSerialFormat extends MIDPTest
{

    public PtoSerialFormat(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    protected int contactBinaryField = 110;
    protected int contactBooleanField = 0;
    protected int contactDateField = 101;
    protected int contactIntField = 0;
    protected int contactStringArray = 100;
    protected int contactStringField = 103;
    protected int eventBinaryField = 0;
    protected int eventBooleanField = 0;
    protected int eventDateField = 106;
    protected int eventIntField = 100;
    protected int eventStringArray = 0;
    protected int eventStringField = 107;
    protected int todoBinaryField = 0;
    protected int todoBooleanField = 0;
    protected int todoDateField = 103;
    protected int todoIntField = 105;
    protected int todoStringArray = 0;
    protected int todoStringField = 107;


    public void runTest() throws TestEndThrowable
    {
    }

    protected Contact createContact(PIM aP) throws TestEndThrowable
    {
        ContactList cList = null;
        Contact contact = null;
        try
        {
            cList =
                (ContactList)aP.openPIMList(PIM.CONTACT_LIST, PIM.READ_ONLY);
            contact = cList.createContact();
            fillItem(contact, cList);
        }
        catch (Throwable e)
        {
            failTest("could not create contact: " + e);
        } // end of try-catch
        return contact;
    }

    protected Event createEvent(PIM aP) throws TestEndThrowable
    {
        EventList eList = null;
        Event event = null;

        try
        {
            eList =
                (EventList)aP.openPIMList(PIM.EVENT_LIST, PIM.READ_ONLY);
        }
        catch (Throwable e)
        {
            failTest("Could not open EventList: " + e);

        } // end of try-catch
        if (eList == null)
        {
            failTest("eventList is null");
        } // end of if (eList == null)

        try
        {
            event = eList.createEvent();
        }
        catch (Throwable e)
        {
            failTest("Could not create Event: " + e);
        }
        try
        {
            fillItem(event, eList);
        }
        catch (Throwable e)
        {
            failTest("Could not fill Event: " + e);
        }
        return event;
    }

    protected ToDo createToDo(PIM aP) throws TestEndThrowable
    {
        ToDoList tList = null;
        ToDo todo = null;
        try
        {
            tList =
                (ToDoList)aP.openPIMList(PIM.TODO_LIST, PIM.READ_ONLY);
            todo = tList.createToDo();
            fillItem(todo, tList);
        }
        catch (PIMException e)
        {
            failTest("Could not create ToDo: " + e);
        } // end of try-catch
        return todo;
    }

    private void fillItem(PIMItem aItem, PIMList aList) throws TestEndThrowable
    {
        int[] supportedFields = aList.getSupportedFields();
        int fieldCount = supportedFields.length;
        for (int i = 0; i < fieldCount; i++)
        {
            int field = supportedFields[ i ];
            int fieldType = aList.getFieldDataType(field);
            fillField(aItem, field, fieldType, aList);
        } // end of for (int i = 0; i < fieldCount; i++)
    }

    private void fillField(PIMItem aItem, int aField, int aFieldType,
                           PIMList aList)
    {
        // Get supported attributes and combine them into a single integer
        int[] supportedAttributes = aList.getSupportedAttributes(aField);
        int supportedAttributesCombined = 0;
        StringBuffer supportedAttributesString = new StringBuffer();

        for (int i = 0; i < supportedAttributes.length; i++)
        {
            supportedAttributesCombined += supportedAttributes[ i ];
            supportedAttributesString
            .append(aList.getAttributeLabel(supportedAttributes[ i ]))
            .append(' ');
        }

        String itemType = null;

        if (aItem instanceof Contact)
        {
            itemType = "Contact";
        }
        else if (aItem instanceof Event)
        {
            itemType = "Event";
        }
        else if (aItem instanceof ToDo)
        {
            itemType = "ToDo";
        }
        else
        {
            itemType = "UNKNOWN";
        }

        switch (aFieldType)
        {
        case PIMItem.INT:
        {
            fillIntField(aItem,  aField, supportedAttributesCombined);
            break;
        }

        case PIMItem.BINARY:
        {
            fillBinaryField(aItem,  aField, supportedAttributesCombined);
            break;
        }

        case PIMItem.BOOLEAN:
        {
            fillBooleanField(aItem,
                             aField,
                             supportedAttributesCombined);
            break;
        }

        case PIMItem.DATE:
        {
            fillDateField(aItem,  aField, supportedAttributesCombined);
            break;
        }

        case PIMItem.STRING_ARRAY:
        {
            fillStringArrayField(aItem,
                                 aField,
                                 aList.stringArraySize(aField),
                                 supportedAttributesCombined);
            break;
        }

        case PIMItem.STRING:
        {
            fillStringField(aItem,  aField, supportedAttributesCombined);
            break;
        }
        default:
        {
            break;
        }
        } // end of switch (aFieldType)
    }

    private void fillIntField(PIMItem aItem, int aField, int aAttributes)
    {
        try
        {
            // Priority needs to be small number as Sync is high
            if (aField == ToDo.PRIORITY)
                aItem.addInt(aField, aAttributes, 1);
            else
                aItem.addInt(aField, aAttributes, 200);
        }
        catch (Throwable e)
        {
            // We are not testing addInt here; if adding some int fails,
            // we may still test toSerialFormat
        } // end of try-catch
    }

    private void fillBinaryField(PIMItem aItem, int aField, int aAttributes)
    {
        String binaryDotGif =
            "R0lGODdhAQABAPYAAAAAAP////8AAP8UAP8sAP9FAP9dAP91AP+OAP+mAP++AP/XAP/vAPf/\r\nAN//AMf/AK7/AJb/AH3/AGX/AE3/ADT/AAT/AAD/DAD/JAD/PAD/VQD/bQD/hgD/ngD/tgD/\r\nzwD/5wD//wDn/wDP/wC2/wCe/wCG/wBt/wBV/wA8/wAk/wAM/wQA/xwA/zQA/00A/2UA/30A\r\n/5YA/64A/8cA/98A//cA//8A7/8A1/8Avv8Apv8Ajv8Adf8AXf8ARf8ALP8AFMPDw89dXX19\r\nfXV1dQAAgoKCgv//AFFR+/sUQWFhYZqamjz7NAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\nAAAAAAAAAAAAAAAAAAAAAAAAACwAAAAAAQABAAAHA4AFgQA7";
        byte[] binaryBytes = binaryDotGif.getBytes();
        try
        {
            aItem.addBinary(aField, aAttributes, binaryBytes, 0,
                            binaryBytes.length);
        }
        catch (Throwable e)
        {
            // We are not testing addBinary here; if adding some binary fails,
            // we may still test toSerialFormat
        } // end of try-catch
    }

    private void fillBooleanField(PIMItem aItem, int aField, int aAttributes)
    {
        try
        {
            aItem.addBoolean(aField, aAttributes, false);
        }
        catch (Throwable e)
        {
        } // end of try-catch
    }

    private void fillDateField(PIMItem aItem, int aField, int aAttributes)
    {
        java.util.Date date = new java.util.Date();
        long time = date.getTime();
        try
        {
            aItem.addDate(aField, aAttributes, time);
        }
        catch (Throwable e)
        {
        } // end of try-catch
    }

    private void fillStringArrayField(PIMItem aItem,
                                      int aField,
                                      int aArraySize,
                                      int aAttributes)
    {
        String[] stringArray = new String[ aArraySize ];
        for (int i = 0; i < aArraySize; i++)
        {
            stringArray[ i ] = "Test Data";
        } // end of for (int i = 0; i < aArraySize; i++)
        try
        {
            aItem.addStringArray(aField, aAttributes, stringArray);
        }
        catch (Throwable e)
        {
        } // end of try-catch
    }

    protected void fillStringField(PIMItem aItem, int aField, int aAttributes)
    {
        try
        {
            aItem.addString(aField, aAttributes, "Test Data");
        }
        catch (Throwable e)
        {
            // no problem here
        } // end of try-catch
    }

    protected void testSerialFormats(PIM aP, PIMItem aItem,
                                     String[] aFormats)
    throws TestEndThrowable
    {
        try
        {
            int formatCount = aFormats.length;
            for (int i = 0; i < formatCount; i++)
            {
                testSerialFormat(aP, aItem, aFormats[ i ]);
            }
        }
        catch (TestEndThrowable e)
        {
            throw e;
        } // end of catch
        catch (Throwable e)
        {
            failTest("unexcpected throwable: " + e);
        } // end of try-catch
    }

    protected void testNullPointer(PIM aP, PIMItem aItem, String[] aFormats,
                                   OutputStream aOs)
    throws TestEndThrowable
    {
        try
        {
            int formatCount = aFormats.length;
            for (int i = 0; i < formatCount; i++)
            {
                testNullPointer(aP, aItem, aFormats[ i ], aOs);
            } // end of for (int i = 0; i < formatCount; i++)
        }
        catch (TestEndThrowable e)
        {
            throw e;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not a NullPointerException: " + e);
        } // end of catch
    }

    protected void testUEE(PIM aP, PIMItem aItem, String[] aFormats,
                           OutputStream aOs)
    throws TestEndThrowable
    {
        // No reason to test all possible formats, the first one will do
        String format = aFormats [ 0 ];
        boolean exceptionThrown = false;
        try
        {
            aP.toSerialFormat(aItem, aOs, "NotAnEncodingMethod", format);
        }
        catch (java.io.UnsupportedEncodingException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not a UEE: " + e);
        } // end of catch

        if (!exceptionThrown)
        {
            failTest("UEE not thrown");
        } // end of if (!exceptionThrown)
    }

    protected void testIAE(PIM aP, PIMItem aItem,
                           OutputStream aOs)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aP.toSerialFormat(aItem, aOs, null, "NotASerialFormat");
        }
        catch (java.lang.IllegalArgumentException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not a IAE: " + e);
        } // end of catch
        if (!exceptionThrown)
        {
            failTest("IAE not thrown");
        } // end of if (!exceptionThrown)
    }

    protected void testNullPointer(PIM aP, PIMItem aItem, String aFormat,
                                   OutputStream aOs)
    throws TestEndThrowable, PIMException, UnsupportedEncodingException
    {
        boolean exceptionThrown = false;
        try
        {
            aP.toSerialFormat(aItem, null, null, aFormat);
        }
        catch (java.lang.NullPointerException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("No NullPointer when os is null");
        } // end of if (!exceptionThrown)

        exceptionThrown = false;
        try
        {
            aP.toSerialFormat(aItem, aOs, null, null);
        }
        catch (java.lang.NullPointerException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("No NullPointer when dataFormat is null");
        } // end of if (!exceptionThrown)

        exceptionThrown = false;
        try
        {
            aP.toSerialFormat(null, aOs, null, aFormat);
        }
        catch (java.lang.NullPointerException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("No NullPointer when item is null");
        } // end of if (!exceptionThrown)
    }

    protected void testSerialFormat(PIM aP, PIMItem aItem, String aFormat)
    throws TestEndThrowable
    {
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        try
        {
            aP.toSerialFormat(aItem, os, null, aFormat);
            // serial format is not logged due to bugs in javatest
            System.out.println("testSerialFormat(): toSerialFormat results in: " + os.toString());
        }
        catch (Throwable e)
        {
            failTest("toSerialFormat threw: " + e);
        } // end of try-catch

        byte[] byteArray = os.toByteArray();
        String string = new String(byteArray);
        // versit object is not logged due bugs in javatest
        System.out.println("versit object: " + string);
        ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
        PIMItem[] items = null;
        try
        {
            items = aP.fromSerialFormat(is, null);
        }
        catch (PIMException e)
        {
            failTest("fromSerialFormat threw PIMException: " +
                     e.getMessage());
        }
        catch (java.io.UnsupportedEncodingException e)
        {
            failTest("fromSerialFormat threw UEE: " + e.getMessage());
        } // end of catch
        catch (java.lang.OutOfMemoryError e)
        {
            failTest("fromSerialFormat threw OOME: " + e.getMessage());
        } // end of catch
        catch (java.lang.IndexOutOfBoundsException e)
        {
            failTest("fromSerial threw IOOBE: " + e.getMessage());
        } // end of catch

        try
        {
            checkItems(aItem, items[ 0 ]);
        }
        catch (java.lang.OutOfMemoryError e)
        {
            failTest("checkItems threw OOME: " + e.getMessage());
        } // end of try-catch
        catch (java.lang.IndexOutOfBoundsException e)
        {
            failTest("checkItems threw IOOBE: " + e.getMessage());
        } // end of catch
    }

    private void checkItems(PIMItem aOldItem, PIMItem aNewItem)
    throws TestEndThrowable
    {
        // we only test some fields
        if (aOldItem instanceof Contact)
        {
            int field = contactIntField;
            checkIntField(aOldItem, aNewItem, field);

            field = contactBinaryField;
            checkBinaryField(aOldItem, aNewItem, field);

            field = contactBooleanField;
            checkBooleanField(aOldItem, aNewItem, field);

            field = contactDateField;
            checkDateField(aOldItem, aNewItem, field);

            field = contactStringArray;
            checkStringArray(aOldItem, aNewItem, field);

            field = contactStringField;
            checkStringField(aOldItem, aNewItem, field);
        } // end of if (aOldItem instanceof Contact)

        if (aOldItem instanceof Event)
        {
            int field = eventIntField;
            checkIntField(aOldItem, aNewItem, field);

            field = eventBinaryField;
            checkBinaryField(aOldItem, aNewItem, field);

            field = eventBooleanField;
            checkBooleanField(aOldItem, aNewItem, field);

            field = eventDateField;
            checkDateField(aOldItem, aNewItem, field);

            field = eventStringArray;
            checkStringArray(aOldItem, aNewItem, field);

            field = eventStringField;
            checkStringField(aOldItem, aNewItem, field);
        } // end of if (aOldItem instanceof Event)

        if (aOldItem instanceof ToDo)
        {
            int field = todoIntField;
            checkIntField(aOldItem, aNewItem, field);

            field = todoBinaryField;
            checkBinaryField(aOldItem, aNewItem, field);

            field = todoBooleanField;
            checkBooleanField(aOldItem, aNewItem, field);

            field = todoDateField;
            checkDateField(aOldItem, aNewItem, field);

            field = todoStringArray;
            checkStringArray(aOldItem, aNewItem, field);

            field = todoStringField;
            checkStringField(aOldItem, aNewItem, field);
        }
    }

    private void checkIntField(PIMItem aOldItem, PIMItem aNewItem, int aField)
    throws TestEndThrowable
    {
        if (aField == 0)
        {
            return;
        } // end of if (field == 0)
        int oldInt = 0;
        try
        {
            oldInt = aOldItem.getInt(aField, 0);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            failTest("old item does not have field:" + aField);
        } // end of try-catch
        int newInt = 0;
        try
        {
            newInt = aNewItem.getInt(aField, 0);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            failTest("new item does not have field:" + aField);
        } // end of try-catch
        if (oldInt != newInt)
        {
            failTest("int field does not match");
        } // end of if (oldInt != newInt)
    }

    private void checkBinaryField(PIMItem aOldItem, PIMItem aNewItem,
                                  int field)
    throws TestEndThrowable
    {
        if (field == 0)
        {
            return;
        } // end of if (field == 0)
        byte[] oldBinary = aOldItem.getBinary(field, 0);
        byte[] newBinary = aNewItem.getBinary(field, 0);
        int byteCount = oldBinary.length;
        if (newBinary.length != byteCount)
        {
            failTest("Binaries have different length");
        } // end of if (newBinary.length != byteCount)
        for (int i = 0; i < byteCount; i++)
        {
            if (oldBinary[ i ] != newBinary[ i ])
            {
                failTest("Binaries don't match");
            } // end of if (oldBinary[ i ] != newBinary[ i ])
        } // end of for (int i = 0; i < byteCount; i++)
    }

    private void checkBooleanField(PIMItem aOldItem, PIMItem aNewItem,
                                   int field)
    throws TestEndThrowable
    {
        if (field == 0)
        {
            return;
        } // end of if (field == 0)
        boolean oldBool = aOldItem.getBoolean(field, 0);
        boolean newBool = aNewItem.getBoolean(field, 0);
        if (oldBool != newBool)
        {
            failTest("booleans don't match");
        } // end of if (oldBool != newBool)
    }

    private void checkDateField(PIMItem aOldItem, PIMItem aNewItem,
                                int aField)
    throws TestEndThrowable
    {
        if (aField == 0)
        {
            return;
        } // end of if (field == 0)
        long oldDate = 0;
        try
        {
            oldDate = aOldItem.getDate(aField, 0);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            failTest("Old item does not have date: " + aField);
        } // end of try-catch
        long newDate = 0;
        try
        {
            newDate = aNewItem.getDate(aField, 0);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            failTest("New item does not have date: " + aField);
        } // end of try-catch

        if (aField == Contact.BIRTHDAY)
        {
            // Comparing the dates would not be wise, as versit does not store
            // milliseconds. Lets use Calendar and compare only YEAR+MONTH+DATE
            Calendar oldCal = Calendar.getInstance();
            oldCal.setTime(new Date(oldDate));
            Calendar newCal = Calendar.getInstance();
            newCal.setTime(new Date(newDate));

            if (oldCal.get(Calendar.YEAR) != newCal.get(Calendar.YEAR) ||
                    oldCal.get(Calendar.MONTH) != newCal.get(Calendar.MONTH) ||
                    oldCal.get(Calendar.DATE) != newCal.get(Calendar.DATE))
            {
                log("old date (" + oldDate + " ms): " +
                    oldCal.get(Calendar.DATE) + "." +
                    (oldCal.get(Calendar.MONTH) + 1) + "." +     // January = 0
                    oldCal.get(Calendar.YEAR) + "  " +
                    oldCal.get(Calendar.HOUR_OF_DAY) + ":" +
                    oldCal.get(Calendar.MINUTE) + ":" +
                    oldCal.get(Calendar.SECOND) + "." +
                    oldCal.get(Calendar.MILLISECOND));
                log("new date (" + newDate + " ms): " +
                    newCal.get(Calendar.DATE) + "." +
                    (newCal.get(Calendar.MONTH) + 1) + "." +     // January = 0
                    newCal.get(Calendar.YEAR) + "  " +
                    newCal.get(Calendar.HOUR_OF_DAY) + ":" +
                    newCal.get(Calendar.MINUTE) + ":" +
                    newCal.get(Calendar.SECOND) + "." +
                    newCal.get(Calendar.MILLISECOND));
                failTest("dates don't match");
            }
        }

        // Comparing the dates would not be wise, as versit does not store
        // milliseconds. Lets do some rounding
        else if (((oldDate / 1000) * 1000) != ((newDate / 1000) * 1000))
        {
            log("old date: " + oldDate);
            log("new date: " + newDate);
            failTest("dates don't match");
        } // end of if (oldDate != newDate)
    }

    private void checkStringArray(PIMItem aOldItem, PIMItem aNewItem,
                                  int field)
    throws TestEndThrowable
    {
        if (field == 0)
        {
            return;
        } // end of if (field == 0)
        String[] aOldArray = aOldItem.getStringArray(field, 0);
        String[] aNewArray = aNewItem.getStringArray(field, 0);
        int stringCount = aOldArray.length;
        for (int i = 0; i < stringCount; i++)
        {
            if (aOldArray[ i ].compareTo(aNewArray[ i ]) != 0)
            {
                failTest("string array does not match");
            } // end of if (aOldArray[ i ].compareTo( aNewArray[ i ] ) != 0)
        } // end of for (int i = 0; i < stringCount; i++)
    }

    private void checkStringField(PIMItem aOldItem, PIMItem aNewItem,
                                  int aField)
    throws TestEndThrowable
    {
        if (aField == 0)
        {
            return;
        } // end of if (field == 0)
        String oldString = null;
        try
        {
            oldString = aOldItem.getString(aField, 0);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            failTest("Old item does not have string: " + aField);
        } // end of try-catch
        String newString = null;
        try
        {
            newString = aNewItem.getString(aField, 0);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            failTest("New item does not have string: " + aField);
        } // end of try-catch
        if (oldString.compareTo(newString) != 0)
        {
            failTest("Strings don't match");
        } // end of if (aOldString.compareTo( aNewString ) != 0)
    }
}



