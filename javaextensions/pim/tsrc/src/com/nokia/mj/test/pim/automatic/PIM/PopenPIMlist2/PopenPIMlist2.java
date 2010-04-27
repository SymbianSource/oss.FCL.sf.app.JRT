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
package com.nokia.mj.test.pim.automatic.PIM.PopenPIMlist2;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.ContactList;
import javax.microedition.pim.EventList;
import javax.microedition.pim.ToDoList;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.Contact;
import javax.microedition.pim.Event;
import javax.microedition.pim.ToDo;
import javax.microedition.pim.PIMException;
import com.nokia.mj.test.pim.adaptation.*;


// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.openPIMlist(int, int, String)
 */
public class PopenPIMlist2
        extends MIDPTest
{

    public PopenPIMlist2(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // Preconditions
        PIM p = PIM.getInstance();

        String[] contactLists = p.listPIMLists(PIM.CONTACT_LIST);
        String[] eventLists = p.listPIMLists(PIM.EVENT_LIST);
        String[] todoLists = p.listPIMLists(PIM.TODO_LIST);

        // Test steps
        PIMList[] readContactLists =
            openAndCheckAllNames(p, PIM.CONTACT_LIST, PIM.READ_ONLY,
                                 contactLists);
        PIMList[] writeContactLists =
            openAndCheckAllNames(p, PIM.CONTACT_LIST, PIM.WRITE_ONLY,
                                 contactLists);
        PIMList[] readWriteContactLists =
            openAndCheckAllNames(p, PIM.CONTACT_LIST, PIM.READ_WRITE,
                                 contactLists);

        testRead(writeContactLists);
        testWrite(readContactLists);

        PIMList[] readEventLists =
            openAndCheckAllNames(p, PIM.EVENT_LIST, PIM.READ_ONLY,
                                 eventLists);
        PIMList[] writeEventLists =
            openAndCheckAllNames(p, PIM.EVENT_LIST, PIM.WRITE_ONLY,
                                 eventLists);
        PIMList[] readWriteEventLists =
            openAndCheckAllNames(p, PIM.EVENT_LIST, PIM.READ_WRITE,
                                 eventLists);

        testRead(writeEventLists);
        testWrite(readEventLists);

        PIMList[] readToDoLists =
            openAndCheckAllNames(p, PIM.TODO_LIST, PIM.READ_ONLY,
                                 todoLists);
        PIMList[] writeToDoLists =
            openAndCheckAllNames(p, PIM.TODO_LIST, PIM.WRITE_ONLY,
                                 todoLists);
        PIMList[] readWriteToDoLists =
            openAndCheckAllNames(p, PIM.TODO_LIST, PIM.READ_WRITE,
                                 todoLists);

        testRead(writeToDoLists);
        testWrite(readToDoLists);

        TestIAException(p, -1, PIM.READ_ONLY, contactLists[ 0 ]);
        TestIAException(p, 0 , PIM.READ_ONLY, contactLists[ 0 ]);
        TestIAException(p, 4 , PIM.READ_ONLY, contactLists[ 0 ]);
        TestIAException(p, PIM.CONTACT_LIST, -1, contactLists[ 0 ]);
        TestIAException(p, PIM.CONTACT_LIST, 0, contactLists[ 0 ]);
        TestIAException(p, PIM.CONTACT_LIST, 4, contactLists[ 0 ]);

        TestPIMException(p, PIM.CONTACT_LIST, PIM.READ_ONLY, "notalistname");
        TestPIMException(p, PIM.EVENT_LIST, PIM.READ_ONLY,
                         contactLists[ 0 ]);

        TestNPException(p, PIM.CONTACT_LIST, PIM.READ_ONLY, null);

        passTest();
    }

    private void testRead(PIMList[] aLists) throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testSingleRead(aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testSingleRead(PIMList aList) throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aList.items();
        }
        catch (SecurityException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not SE: " + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            failTest("Reading from WRITE_ONLY list succeeded");
        } // end of if (!exceptionThrown)
    }

    private void testWrite(PIMList[] aLists) throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testSingleWrite(aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testSingleWrite(PIMList aList) throws TestEndThrowable
    {
        boolean exceptionthrown = false;

        try
        {
            if (aList instanceof ContactList)
            {
                ContactList cList = (ContactList)aList;
                Contact contact = cList.createContact();
                contact.commit();
            } // end of if (aList instanceof ContactList)
            if (aList instanceof EventList)
            {
                EventList eList = (EventList)aList;
                Event event = eList.createEvent();
                event.commit();
            } // end of if (aList instanceof EventList)
            if (aList instanceof ToDoList)
            {
                ToDoList tList = (ToDoList)aList;
                ToDo todo = tList.createToDo();
                todo.commit();
            } // end of if (aList instanceof ToDoList)
        }
        catch (SecurityException e)
        {
            exceptionthrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not SE: " + e.toString());
        } // end of catch

        if (!exceptionthrown)
        {
            failTest("Write to READ_ONLY Contact List succeeded");
        } // end of if (!exceptionthrown)
    }

    private PIMList[] openAndCheckAllNames(PIM aP, int aListType, int aMode,
                                           String[] aNames)
    throws TestEndThrowable
    {
        int listCount = aNames.length;
        PIMList[] pimLists = new PIMList[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            pimLists[ i ] = openAndCheck(aP, aListType, aMode, aNames[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
        return pimLists;
    }

    private PIMList openAndCheck(PIM aP, int aListType, int aMode,
                                 String aName)
    throws TestEndThrowable
    {
        PIMList list = null;
        try
        {
            list = aP.openPIMList(aListType, aMode, aName);
        }
        catch (SecurityException e)
        {
            failTest("SecurityException: " + e.getMessage());
        }
        catch (Throwable e)
        {
            failTest("Throwable:" + e.toString());
        } // end of catch

        switch (aListType)
        {
        case PIM.CONTACT_LIST:
        {
            if (!(list instanceof javax.microedition.pim.ContactList))
            {
                failTest("Not a contact list.");
            }
            break;
        }
        case PIM.EVENT_LIST:
        {
            if (!(list instanceof javax.microedition.pim.EventList))
            {
                failTest("Not an event list.");
            }
            break;
        }
        case PIM.TODO_LIST:
        {
            if (!(list instanceof javax.microedition.pim.ToDoList))
            {
                failTest("Not a todo list.");
            }
            break;
        }
        default:
        {
            failTest("Test internal error");
            break;
        }
        } // end of switch (aListType)
        return list;
    }


    private void TestIAException(PIM aP, int aListType, int aMode,
                                 String aName)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aP.openPIMList(aListType, aMode, aName);
        }
        catch (IllegalArgumentException e)
        {
            exceptionThrown = true;
        }
        catch (Throwable e)
        {
            failTest("Not IAE: " + e.toString());
        } // end of catch

        if (!exceptionThrown)
        {
            failTest("exception not thrown");
        } // end of if (!exceptionThrown)
    }

    private void TestPIMException(PIM aP, int aListType, int aMode,
                                  String aName)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aP.openPIMList(aListType, aMode, aName);
        }
        catch (PIMException e)
        {
            exceptionThrown = true;
        }
        catch (Throwable e)
        {
            failTest("Not PIMException: " + e.toString());
        } // end of catch

        if (!exceptionThrown)
        {
            failTest("exception not thrown");
        } // end of if (!exceptionThrown)
    }

    private void TestNPException(PIM aP, int aListType, int aMode,
                                 String aName)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aP.openPIMList(aListType, aMode, aName);
        }
        catch (NullPointerException e)
        {
            exceptionThrown = true;
        }
        catch (Throwable e)
        {
            failTest("Not NPException: " + e.toString());
        } // end of catch

        if (!exceptionThrown)
        {
            failTest("exception not thrown");
        } // end of if (!exceptionThrown)
    }


}
