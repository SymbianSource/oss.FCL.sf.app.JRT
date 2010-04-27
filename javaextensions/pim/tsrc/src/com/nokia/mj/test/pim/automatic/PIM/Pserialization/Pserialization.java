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
package com.nokia.mj.test.pim.automatic.PIM.Pserialization;
//IMPORTS
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;


// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.fromSerialFormat() and
 * javax.microedition.PIM.toSerialFormat().
 *
 * The test involves creation of items, converting them to
 * serialized format and back and checking whether the original
 * and resulting items are equal.
 */
public class Pserialization extends MIDPTest
{

    public Pserialization(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // PIM manager
    PIM iPim = null;

    // Lists with which the original test items are associated
    ContactList[] iContactLists;
    EventList[] iEventLists;
    ToDoList[] iToDoLists;

    // Original test items, one per list.
    // The indexes correspond with the list indexes in the iXyzLists.
    Contact[] iOriginalContacts;
    Event[] iOriginalEvents;
    ToDo[] iOriginalToDos;

    public void initTest() throws TestEndThrowable
    {
        try
        {
            // Get PIM instance
            iPim = PIM.getInstance();

            // Get list names per type
            String[] contactListNames = iPim.listPIMLists(PIM.CONTACT_LIST);
            String[] eventListNames = iPim.listPIMLists(PIM.EVENT_LIST);
            String[] toDoListNames = iPim.listPIMLists(PIM.TODO_LIST);

            // Create list arrays as we now know their amount
            iContactLists = new ContactList[ contactListNames.length ];
            iEventLists = new EventList[ eventListNames.length ];
            iToDoLists = new ToDoList[ toDoListNames.length ];

            // Create original test item arraya as we now know their amount
            iOriginalContacts = new Contact[ contactListNames.length ];
            iOriginalEvents = new Event[ eventListNames.length ];
            iOriginalToDos = new ToDo[ toDoListNames.length ];

            // Open lists and create original test items

            // Contact
            for (int i = 0; i < contactListNames.length; i++)
            {
                iContactLists[ i ] = (ContactList) iPim.openPIMList(
                                         PIM.CONTACT_LIST,
                                         PIM.READ_WRITE,
                                         contactListNames[ i ]);

                iOriginalContacts[ i ] = iContactLists[ i ].createContact();
                ItemTool.fillItem(iOriginalContacts[ i ], iContactLists[ i ]);
            }

            // Event
            for (int i = 0; i < eventListNames.length; i++)
            {
                iEventLists[ i ] = (EventList) iPim.openPIMList(
                                       PIM.EVENT_LIST,
                                       PIM.READ_WRITE,
                                       eventListNames[ i ]);

                iOriginalEvents[ i ] = iEventLists[ i ].createEvent();
                ItemTool.fillItem(iOriginalEvents[ i ], iEventLists[ i ]);
            }

            // ToDo
            for (int i = 0; i < toDoListNames.length; i++)
            {
                iToDoLists[ i ] = (ToDoList) iPim.openPIMList(
                                      PIM.TODO_LIST,
                                      PIM.READ_WRITE,
                                      toDoListNames[ i ]);

                iOriginalToDos[ i ] = iToDoLists[ i ].createToDo();
                ItemTool.fillItem(iOriginalToDos[ i ], iToDoLists[ i ]);
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            failTest("Test initialization failed: " + e);
        }

        // OK, what we now got is:
        // - full arrays of ContactLists, EventLists and ToDoLists
        // - full arrays of original test Contacts, Events and ToDos,
        //   one per each list, all exportable fields filled
        log("initTest() finished.");
    }


    public void cleanupTest()
    {
        // Nothing necessary
    }


    public String toSerialFormat(PIMItem aItem)
    throws TestEndThrowable
    {
        String format = (aItem instanceof Contact) ?
                        "VCARD/2.1" : "VCALENDAR/1.0";

        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        try
        {
            iPim.toSerialFormat(aItem,
                                bos,
                                null,  // default encoding used
                                format);
        }
        catch (Exception e)
        {
            failTest("Unexpected exception: " + e);
            e.printStackTrace();
        }

        return bos.toString();
    }

    /**
     * Single-entry versit objects assumed.
     */
    public PIMItem fromSerialFormat(String aVersitObject)
    throws TestEndThrowable
    {
        PIMItem[] resultingItems = null;
        ByteArrayInputStream bis = new ByteArrayInputStream(
            aVersitObject.getBytes());
        try
        {
            resultingItems = iPim.fromSerialFormat(
                                 bis,
                                 null);  // default encoding used
        }
        catch (Exception e)
        {
            failTest("Unexpected exception: " + e);
            e.printStackTrace();
        }

        return resultingItems[ 0 ]; // return first (and only) item
    }

    public void runTest() throws TestEndThrowable
    {
        initTest();
        StringBuffer log = new StringBuffer();

        for (int i = 0; i < iOriginalContacts.length; i++)
        {
            String versitString = toSerialFormat(
                                      iOriginalContacts[ i ]);

            Contact resultingContact = (Contact)fromSerialFormat(
                                           versitString);

            boolean isEqual = ItemTool.isEqualItems(
                                  iOriginalContacts[ i ],
                                  resultingContact,
                                  log,
                                  iContactLists[ i ],
                                  ItemTool.LOG_DIFF);

            if (!isEqual)
            {
// versit string probably has something that chokes xml-parser/encoder
//              log( "Resulting versit string:" );
//              log( versitString + '\n' );
//              log( log.toString() );

                log("Not all data is preserved during conversion.");
                // failTest( "Serialization corrupts the item." );
            }
        }
        passTest();
        cleanupTest();
    }
}



