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
package com.nokia.mj.test.pim.automatic.PIM.PopenPIMlist;
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
import com.nokia.mj.test.pim.adaptation.*;


// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.openPIMlist(int, int)
 */
public class PopenPIMlist
        extends MIDPTest
{

    public PopenPIMlist(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        PIM p = PIM.getInstance();

        PIMList readContactList =
            openAndCheck(p, PIM.CONTACT_LIST, PIM.READ_ONLY);
        PIMList writeContactList =
            openAndCheck(p, PIM.CONTACT_LIST, PIM.WRITE_ONLY);
        PIMList readWriteContactList =
            openAndCheck(p, PIM.CONTACT_LIST, PIM.READ_WRITE);

        testRead(writeContactList);
        boolean exceptionthrown = false;
        try
        {
            ContactList cList = (ContactList)readContactList;
            Contact contact = cList.createContact();
            contact.commit();
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



        PIMList readEventList =
            openAndCheck(p, PIM.EVENT_LIST, PIM.READ_ONLY);
        PIMList writeEventList =
            openAndCheck(p, PIM.EVENT_LIST, PIM.WRITE_ONLY);
        PIMList readWriteEventList =
            openAndCheck(p, PIM.EVENT_LIST, PIM.READ_WRITE);

        testRead(writeEventList);
        testRead(writeContactList);
        exceptionthrown = false;
        try
        {
            EventList cList = (EventList)readEventList;
            Event event = cList.createEvent();
            event.commit();
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
            failTest("Write to READ_ONLY Event List succeeded");
        } // end of if (!exceptionthrown)

        PIMList readToDoList =
            openAndCheck(p, PIM.TODO_LIST, PIM.READ_ONLY);
        PIMList writeToDoList =
            openAndCheck(p, PIM.TODO_LIST, PIM.WRITE_ONLY);
        PIMList readWriteToDoList =
            openAndCheck(p, PIM.TODO_LIST, PIM.READ_WRITE);

        testRead(writeToDoList);
        exceptionthrown = false;
        try
        {
            ToDoList cList = (ToDoList)readToDoList;
            ToDo todo = cList.createToDo();
            todo.commit();
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
            failTest("Write to READ_ONLY ToDo List succeeded");
        } // end of if (!exceptionthrown)

        TestException(p, -1, PIM.READ_ONLY);
        TestException(p, 0 , PIM.READ_ONLY);
        TestException(p, 4 , PIM.READ_ONLY);
        TestException(p, PIM.CONTACT_LIST, -1);
        TestException(p, PIM.CONTACT_LIST, 0);
        TestException(p, PIM.CONTACT_LIST, 4);

        passTest();
    }

    private void testRead(PIMList aList) throws TestEndThrowable
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

    private PIMList openAndCheck(PIM aP, int aListType, int aMode)
    throws TestEndThrowable
    {
        PIMList list = null;
        try
        {
            list = aP.openPIMList(aListType, aMode);
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


    private void TestException(PIM aP, int aListType, int aMode)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aP.openPIMList(aListType, aMode);
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
            failTest("exception not thrown( listtype -1 )");
        } // end of if (!exceptionThrown)
    }

}
