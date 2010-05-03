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
package com.nokia.mj.test.pim.automatic.Contact.CcommitUpdate;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests that partial contacts work correctly
 */
public class CcommitUpdate
        extends MIDPTest
{

    private ContactList iContactList;
    private Contact iContact;
    Date iCurrentDate = new Date();
    private static final String TEST_CATEGORY = "my category";
    private static final String UPDATE = "upd";

    private static final String iPicture =
        "/9j/4AAQSkZJRgABAQAAAQABAAD//gAoCgoKMDUuMDMuMjAwNAoxMTo0NjozNgpNb2RlPTIK\n" +
        "MC4xCjMuMAr/2wBDAAoHBwgHBgoICAgLCgoLDhgQDg0NDh0VFhEYIx8lJCIfIiEmKzcvJik0\n" +
        "KSEiMEExNDk7Pj4+JS5ESUM8SDc9Pjv/2wBDAQoLCw4NDhwQEBw7KCIoOzs7Ozs7Ozs7Ozs7\n" +
        "Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozv/wAARCABgAFADASIAAhEB\n" +
        "AxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQA\n" +
        "AAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0\n" +
        "NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZ\n" +
        "mqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3\n" +
        "+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQA\n" +
        "AQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygp\n" +
        "KjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaX\n" +
        "mJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3\n" +
        "+Pn6/9oADAMBAAIRAxEAPwCiGFO3ColjlPAFWI7G4cdMVy8jZ2upFbke4dqCxq6mlkDMkmKe\n" +
        "be1iHLFz7VSpNmbrx6GaXPpik3segq1LJGeEQAe1QhSfYVqsO3uR7fyIwJCOlTJauwyWFJwK\n" +
        "tWrEtt9a0dBJEuvIZHYFyBmm3dmkDhBycc1twRCJC561k3MhknZiMc9DWE4pIqlOUpasnRkT\n" +
        "7q804zSEYXgUskXluRjNM5Y4BrY5rjNryPjJYmmSQOZCozxV+3RUOTVyOOJ+wOaqLsS2ZBsm\n" +
        "SPeR9Kqywui7mGB2rpii4wQMCqM1uLqTk4UdKtTfUSZgc59q0tOi3SA+9V7u1Nu+M5B5BqxZ\n" +
        "SlFUjtW2jQ2zTnzuC/wis69tTkSoOCOfrV3zhKQOhqyEXyCrYOa5JxvoXCbi7lG7ADZ9DUGe\n" +
        "gqxd8OR71WwDIabIJogCCzH5RU8V3GrbSvHrVeQHAj9OTU0USuhUiglluQkoSpypHFQzApbh\n" +
        "V4Lnn6Ulqx3tCx5HSn3OFRCxFAindW5a2weccg1mRM0Z61u8S2zAcnFYTLiTBraDtoWiykp3\n" +
        "DmtK0uPNwjHmshetOWYqcg9DVNXQ7GpfJ827HeqQI8zHvV26fcGFZu75s5wRXOxFlm5JqWKT\n" +
        "C5FUzJjP5ikWcLwTxSCxGb95L4/NtwcCp793kgXbOHYH7oNZl1JGZdyDBPU0QXBUjmspTsbx\n" +
        "pX1NrTpT5YD9e9Z9wrfaG28jJ6UG5GMhuab9oOzKH5u9Cr26FKixu8gE0m/jGaGUldx6VEzb\n" +
        "RXZCSZk0bVyTjNZUzsrZ6VqykOtZ9xDxzWDEtyo1y2eTTTIXHJpzw4qAoQeKyaZupR7CnFJ9\n" +
        "M07Y3YU9Ld3PJAFRys050Rg/UmpBuzjnPoKsG3VMKPxq1bQx5zgZqlDuZyq9jP8AmUYK9e9M\n" +
        "foc1f2bpMe9STWiOQuO3UVvBqJi5N7k21iuVYE+lRurYyw4qAmUkHPNSC4kXhsGpuKwPFG6Z\n" +
        "RsH3qubcdSwqUneM4x9KnitY3AZpoxnsz4pXCxXihQDkbjUot8HKg1pQ2KkDEkJ+ki/41YFk\n" +
        "QMb4s/8AXRf8aegWZlCDKkEZJojgkQ5ArU+xspHzw/8Af1f8anjsmIzui/7+L/jRoFjEWB1c\n" +
        "sV709twkDbTitg2LH+KP/v4v+NRy2jKCS0eP+ui/40aAAP/Z";

    public CcommitUpdate(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void initTest() throws TestEndThrowable
    {
        openContactList();
        try
        {
            Enumeration items = iContactList.items();
            while (items.hasMoreElements())
            {
                Contact contact = (Contact)items.nextElement();
                iContactList.removeContact(contact);

            }
        }
        catch (Exception e)
        {
            failTest("CcommitUpdate::initTest - unable to remove all contacts");
        }

        closeContactList();
    }

    public void runTest() throws Throwable
    {
        initTest();

        // Open the default contact list
        openContactList();
        int[] supportedFields = iContactList.getSupportedFields();

        log("CcommitUpdate::runTest - phase 1");
        // Create a new contact item
        iContact = createAndInitializeContact();
        checkContact(iContact, supportedFields, supportedFields.length);
        iContact.commit();
        checkContact(iContact, supportedFields, supportedFields.length);
        closeContactList();

        log("CcommitUpdate::runTest - phase 2");

        for (int i = 0; i < supportedFields.length; i++)
        {
            int field = supportedFields[ i ];

            // Ignore read-only fields
            if (field == Contact.REVISION ||
                    field == Contact.UID)
            {
                continue;
            }

            log("CcommitUpdate::runTest - testing field " + supportedFields[ i ]);

            openContactList();
            iContact = (Contact)iContactList.items().nextElement();
            // Change one value and test that the item data is correctly changed
            setField(iContact, supportedFields, i);

            log("CcommitUpdate::runTest - testing field before commit" + supportedFields[ i ]);
            checkFieldChange(iContact, supportedFields, i);
            // Commit the contact
            iContact.commit();

            log("CcommitUpdate::runTest - testing field after commit" + supportedFields[ i ]);
            checkFieldChange(iContact, supportedFields, i);
            resetField(iContact, supportedFields[ i ]);

            if (iContact.getFields().length != supportedFields.length)
            {
                failTest("CcommitUpdate::checkContact - unexpected number of values in the contact");
            }

            checkContact(iContact, supportedFields, supportedFields.length);
            iContact.commit();

            if (iContact.getFields().length != supportedFields.length)
            {
                failTest("CcommitUpdate::checkContact - unexpected number of values in the contact");
            }

            checkContact(iContact, supportedFields, supportedFields.length);
            closeContactList();
        }

        initTest(); // Initialize again

        log("CcommitUpdate::runTest - phase 3");

        openContactList();
        iContact = createAndInitializeContact();
        checkContact(iContact, supportedFields, supportedFields.length);
        iContact.commit();
        checkContact(iContact, supportedFields, supportedFields.length);
        closeContactList();

        for (int i = 0; i < supportedFields.length; i++)
        {
            int field = supportedFields[ i ];

            // Ignore read-only fields
            if (field == Contact.REVISION ||
                    field == Contact.UID)
            {
                continue;
            }

            log("CcommitUpdate::runTest - testing field " + supportedFields[ i ]);

            openContactList();
            iContact = (Contact)iContactList.items().nextElement();
            // Change one value and test that the item data is correctly changed
            setField(iContact, supportedFields, i);

            log("CcommitUpdate::runTest - testing field before commit" + supportedFields[ i ]);
            checkFieldChange(iContact, supportedFields, i);
            // Commit the contact
            iContact.commit();

            log("CcommitUpdate::runTest - testing field after commit" + supportedFields[ i ]);
            checkFieldChange(iContact, supportedFields, i);

            closeContactList();
        }

        initTest(); // Initialize again

        openContactList();
        iContact = createAndInitializeContact();
        checkContact(iContact, supportedFields, supportedFields.length);
        iContact.commit();
        checkContact(iContact, supportedFields, supportedFields.length);
        closeContactList();

        log("CcommitUpdate::runTest - phase 4");

        Vector skippedFields = new Vector();

        for (int i = 0; i < supportedFields.length; i++)
        {
            int field = supportedFields[ i ];

            // Ignore read-only fields
            if (field == Contact.REVISION ||
                    field == Contact.UID)
            {
                skippedFields.addElement(new Integer(field));
                continue;
            }

            log("CcommitUpdate::runTest - testing field " + supportedFields[ i ]);

            openContactList();
            iContact = (Contact)iContactList.items().nextElement();
            // Change one value and test that the item data is correctly changed
            try
            {
                String label = iContactList.getFieldLabel(field);
                addField(iContact, supportedFields[ i ], label);
            }
            catch (Exception e)
            {
                // Ignore illegal arguments and field full exceptions
                skippedFields.addElement(new Integer(field));
                continue;
            }

            log("CcommitUpdate::runTest - testing field before commit" + supportedFields[ i ]);
            if (iContact.countValues(field) != 2)
            {
                failTest("CcommitUpdate::runTest - incorrect count of values in field " + field);
            }

            // Commit the contact
            iContact.commit();

            for (int j = 0; j < i; j++)
            {
                if (!isSkipped(skippedFields, supportedFields[ j ]) &&
                        iContact.countValues(supportedFields[ j ]) != 2)
                {
                    failTest("CcommitUpdate::runTest - incorrect count of values in field " + supportedFields[ j ]);
                }
            }

            closeContactList();
        }
    }

    private boolean isSkipped(Vector aSkippedFields, int aField)
    {
        int size = aSkippedFields.size();

        for (int i = 0; i < size; i++)
        {
            if (((Integer)aSkippedFields.elementAt(i)).intValue() == aField)
            {
                return true;
            }
        }
        return false;
    }

    private void resetField(Contact aContact, int aField)
    {
        switch (iContactList.getFieldDataType(aField))
        {
        case PIMItem.STRING:
        {
            String label = iContactList.getFieldLabel(aField);
            aContact.setString(aField, 0, PIMItem.ATTR_NONE, label);
            break;
        }
        case PIMItem.STRING_ARRAY:
        {
            String label = iContactList.getFieldLabel(aField);
            String[] array = new String[ iContactList.stringArraySize(aField)];
            for (int i = 0; i < array.length; i++)
            {
                array[ i ] = label + i;
            }
            aContact.setStringArray(aField, 0, PIMItem.ATTR_NONE, array);
            log("Done");
            break;
        }
        case PIMItem.DATE:
        {
            aContact.setDate(aField, 0, PIMItem.ATTR_NONE, iCurrentDate.getTime());
            break;
        }
        case PIMItem.BOOLEAN:
        {
            aContact.setBoolean(aField, 0, PIMItem.ATTR_NONE, true);
            break;
        }
        case PIMItem.INT:
        {
            aContact.setInt(aField, 0, PIMItem.ATTR_NONE, 202);
            break;
        }
        case PIMItem.BINARY:
        {
            aContact.setBinary(aField, 0, PIMItem.ATTR_NONE, iPicture.getBytes(), 0, iPicture.getBytes().length);
            break;
        }
        default:
        {
            break;
        }
        }
    }

    private void checkFieldChange(Contact aContact, int[] aFields, int aIndex)
    {
        int field = aFields[ aIndex ];
        String label = iContactList.getFieldLabel(field);
        switch (iContactList.getFieldDataType(field))
        {
        case PIMItem.STRING:
        {
            if (!(aContact.getString(field, 0).equals(label + UPDATE)))
            {
                failTest("CcommitUpdate::checkFieldChange - field " + field + " value was not changed!");
            }
            break;
        }
        case PIMItem.STRING_ARRAY:
        {
            String[] array = new String[ iContactList.stringArraySize(field)];
            for (int j = 0; j < array.length; j++)
            {
                array[ j ] = label + j + UPDATE;
            }
            String[] temp = aContact.getStringArray(field, 0);
            for (int j = 0; j < array.length; j++)
            {
                if (!(array[ j ].equals(temp[ j ])))
                {
                    failTest("CcommitUpdate::checkFieldChange - field " + field + " value in element " + j + " was not changed!");
                }
            }
            break;
        }
        case PIMItem.BOOLEAN:
        {
            if (aContact.getBoolean(field, 0) != false)
            {
                failTest("CcommitUpdate::checkFieldChange - field " + field + " value was not changed!");
            }
            break;
        }
        case PIMItem.INT:
        {
            if (aContact.getInt(field, 0) != 200)
            {
                failTest("CcommitUpdate::checkFieldChange - field " + field + " value was not changed!");
            }
            break;
        }
        case PIMItem.BINARY:
        case PIMItem.DATE:
        default:
        {
            log("CcommitUpdate::setField - skipping field " + field);
            break;
        }
        }
    }

    private void setField(Contact aContact, int[] aFields, int aIndex)
    {
        int field = aFields[ aIndex ];
        String label = iContactList.getFieldLabel(field);
        switch (iContactList.getFieldDataType(field))
        {
        case PIMItem.STRING:
        {
            aContact.setString(field, 0, PIMItem.ATTR_NONE, label + UPDATE);
            break;
        }
        case PIMItem.STRING_ARRAY:
        {
            String[] array = new String[ iContactList.stringArraySize(field)];
            for (int j = 0; j < array.length; j++)
            {
                array[ j ] = label + j + UPDATE;
            }
            aContact.setStringArray(field, 0, PIMItem.ATTR_NONE, array);
            break;
        }
        case PIMItem.BOOLEAN:
        {
            aContact.setBoolean(field, 0, PIMItem.ATTR_NONE, false);
            break;
        }
        case PIMItem.INT:
        {
            aContact.setInt(field, 0, PIMItem.ATTR_NONE, 200);
            break;
        }
        case PIMItem.BINARY:
        case PIMItem.DATE:
        default:
        {
            log("CcommitUpdate::setField - skipping field " + field);
            break;
        }
        }
    }

    private Contact createAndInitializeContact() throws Throwable
    {
        Contact contact = iContactList.createContact();

        // Initialize the new contact with test data
        int[] supportedFields = iContactList.getSupportedFields();

        for (int i = 0; i < supportedFields.length; i++)
        {
            String label = iContactList.getFieldLabel(supportedFields[ i ]);
            try
            {
                addField(contact, supportedFields[ i ], label);
            }
            catch (Exception e)
            {
                e.printStackTrace();
                failTest("CcommitUpdate::addField - unable add value to field " + supportedFields[ i ]);
            }

        }

        return contact;
    }

    private void checkContact(
        Contact aContact,
        int[] aFields,
        int aExpectedValueCount)
    throws TestEndThrowable
    {

        // Check each field
        for (int i = 0; i < aFields.length; i++)
        {
            String label = iContactList.getFieldLabel(aFields[ i ]);
            int field = aFields[ i ];
            switch (iContactList.getFieldDataType(field))
            {
            case PIMItem.STRING:
            {
                if (field != Contact.UID &&
                        field != Contact.FORMATTED_NAME &&
                        !(aContact.getString(field, 0).equals(label)))
                {
                    failTest("CcommitUpdate::checkContact - field's " + field + " value is incorrect");
                }
                break;
            }
            case PIMItem.STRING_ARRAY:
            {
                String[] array = new String[ iContactList.stringArraySize(field)];
                for (int j = 0; j < array.length; j++)
                {
                    array[ j ] = label + j;
                }
                String[] retArray = aContact.getStringArray(field, 0);
                for (int j = 0; j < array.length; j++)
                {
                    if (!retArray[ j ].equals(array[ j ]))
                    {
                        log("CcommitUpdate::checkContact  - got value " + retArray[ j ] + " expected " + array[ j ]);
                        failTest("CcommitUpdate::checkContact - field's " + field + " array element " + j + " value is incorrect");
                    }
                }
                break;
            }
            case PIMItem.DATE:
            {
                long date = aContact.getDate(field, 0);
                if (field != Contact.REVISION &&
                        field != Contact.BIRTHDAY &&
                        field != 0x1005008 &&
                        date != iCurrentDate.getTime())
                {
                    failTest("CcommitUpdate::checkContact - field's " + field + " value is incorrect");
                }
                break;
            }
            case PIMItem.BOOLEAN:
            {
                if (aContact.getBoolean(field, 0) != true)
                {
                    failTest("CcommitUpdate::checkContact - field's " + field + " value is incorrect");
                }
                break;
            }
            case PIMItem.INT:
            {
                if (aContact.getInt(field, 0) != 202)
                {
                    failTest("CcommitUpdate::checkContact - field's " + field + " value is incorrect");
                }
                break;
            }
            case PIMItem.BINARY:
            {
                String picture = new String(aContact.getBinary(field, 0));
                System.out.println("CcommitUpdate::checkContact - binary " + picture);
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }

    private void openContactList() throws TestEndThrowable
    {
        try
        {
            iContactList = (ContactList)PIM.getInstance().openPIMList(
                               PIM.CONTACT_LIST, PIM.READ_WRITE);
        }
        catch (Exception e)
        {
            failTest("CcommitUpdate::openContactList - unable to open contact list");
        }
    }

    private void closeContactList()
    {
        try
        {
            iContactList.close();
            iContactList = null;
        }
        catch (Exception e)
        {
            failTest("CcommitUpdate::closeContactList - unable to close the list");
        }
    }

    private void addField(Contact aContact, int aField, String aLabel) throws TestEndThrowable
    {
        switch (iContactList.getFieldDataType(aField))
        {
        case PIMItem.STRING:
        {
            aContact.addString(aField, PIMItem.ATTR_NONE, aLabel);
            break;
        }
        case PIMItem.STRING_ARRAY:
        {
            String[] array = new String[ iContactList.stringArraySize(aField)];
            for (int i = 0; i < array.length; i++)
            {
                array[ i ] = aLabel + i;
            }
            aContact.addStringArray(aField, PIMItem.ATTR_NONE, array);
            break;
        }
        case PIMItem.DATE:
        {
            aContact.addDate(aField, PIMItem.ATTR_NONE, iCurrentDate.getTime());
            break;
        }
        case PIMItem.BOOLEAN:
        {
            aContact.addBoolean(aField, PIMItem.ATTR_NONE, true);
            break;
        }
        case PIMItem.INT:
        {
            aContact.addInt(aField, PIMItem.ATTR_NONE, 202);
            break;
        }
        case PIMItem.BINARY:
        {
            aContact.addBinary(aField, PIMItem.ATTR_NONE, iPicture.getBytes(), 0, iPicture.getBytes().length);
            break;
        }
        default:
        {
            break;
        }
        }
    }

}
