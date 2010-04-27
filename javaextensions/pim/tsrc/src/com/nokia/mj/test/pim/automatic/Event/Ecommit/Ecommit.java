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
package com.nokia.mj.test.pim.automatic.Event.Ecommit;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests that Event.commit() persists values correctly
 */
public class Ecommit
        extends MIDPTest
{
    private String[] iEventLists;
    private String iItemSubject;
    private Date iStartDate;
    private Date iEndDate;
    private int iAlarm = 900;
    private int iDefaultAlarm = -43200;

    public Ecommit(String sTestName,
                   j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void runTest() throws TestEndThrowable
    {
        // Init test data
        init();
        // Do not close lists between commit
        doTest(false);
        // Close lists between commit
        doTest(true);
    }

    public void doTest(boolean aCloseList) throws TestEndThrowable
    {
        try
        {
            debug("Ecommit::doTest - testing appointments");
            EventList appts = openList(iEventLists[ 0 ]);
            // Test first event list (appointments)

            // Create an appointment and initialize data to it
            Event appt = createAndCommitItem(appts);

            if (aCloseList)
            {
                appts.close();
                appts = openList(iEventLists[ 0 ]);
                appt  = getSpecificEventItem(appts, iItemSubject);
            }

            // Check commited appointment item that data was scaled correctly
            testField(appts, appt, Event.SUMMARY, iItemSubject);
            testField(appts, appt, Event.START, iStartDate);
            testField(appts, appt, Event.END, iEndDate);
            testField(appts, appt, Event.ALARM, new Integer(iAlarm));
            // Change alarm to future
            appt.setInt(Event.ALARM, 0, PIMItem.ATTR_NONE, -90000);
            appt.commit();

            if (aCloseList)
            {
                appts.close();
                appts = openList(iEventLists[ 0 ]);
                appt  = getSpecificEventItem(appts, iItemSubject);
            }

            // Alarm occurs at 12:00 o'clock but the offset must be calculated from the start time
            Date alarmDate = new Date((getStartOfDay(iStartDate).getTime() + (43200000)));
            int alarmInt = getDifferenceInSeconds(iStartDate, alarmDate);
            testField(appts, appt, Event.ALARM, new Integer(alarmInt));
            appts.removeEvent(appt);
            appts.close();

            debug("Ecommit::doTest - testing memos");

            // Test second event list (memos)
            EventList memos = openList(iEventLists[ 1 ]);
            // Create a memo and initialize data to it
            Event memo = createAndCommitItem(memos);

            if (aCloseList)
            {
                memos.close();
                memos = openList(iEventLists[ 1 ]);
                memo  = getSpecificEventItem(memos, iItemSubject);
            }

            // Check commited memo item that data was scaled correctly
            testField(memos, memo, Event.SUMMARY, iItemSubject);
            testField(memos, memo, Event.START, iStartDate);
            testField(memos, memo, Event.END, iEndDate);

            testField(memos, memo, Event.ALARM, new Integer(iAlarm));
            // Change alarm to future
            memo.setInt(Event.ALARM, 0, PIMItem.ATTR_NONE, -90000);
            memo.commit();

            if (aCloseList)
            {
                memos.close();
                memos = openList(iEventLists[ 1 ]);
                memo  = getSpecificEventItem(memos, iItemSubject);
            }

            // Alarm occurs at 12:00 o'clock but the offset must be calculated from the start time
            alarmDate = new Date((getStartOfDay(iStartDate).getTime() + (43200000)));
            alarmInt = getDifferenceInSeconds(iStartDate, alarmDate);
            testField(memos, memo, Event.ALARM, new Integer(alarmInt));

            memos.removeEvent(memo);
            memos.close();

            debug("Ecommit::doTest - testing annivs");

            // Test third event list (anniversaries)
            EventList annivs = openList(iEventLists[ 2 ]);
            // Create an appointment and initialize data to it
            Event anniv = createAndCommitItem(annivs);

            if (aCloseList)
            {
                annivs.close();
                annivs = openList(iEventLists[ 2 ]);
                anniv  = getSpecificEventItem(annivs, iItemSubject);
            }

            // Check commited appointment item that data was scaled correctly
            testField(annivs, anniv, Event.SUMMARY, iItemSubject);
            testField(annivs, anniv, Event.START, getStartOfDay(iStartDate));
            alarmDate = new Date((iStartDate.getTime() - iAlarm*1000));
            // Date has been converted to start of day
            alarmInt = getDifferenceInSeconds(getStartOfDay(iStartDate), alarmDate);
            testField(annivs, anniv, Event.ALARM, new Integer(alarmInt));
            // Change alarm to future
            anniv.setInt(Event.ALARM, 0, PIMItem.ATTR_NONE, -90000);
            anniv.commit();

            if (aCloseList)
            {
                annivs.close();
                annivs = openList(iEventLists[ 2 ]);
                anniv  = getSpecificEventItem(annivs, iItemSubject);
            }

            // test default value since start time is set to 00:00
            testField(annivs, anniv, Event.ALARM, new Integer(iDefaultAlarm));
            annivs.removeEvent(anniv);
            annivs.close();
        }
        catch (Throwable e)
        {
            failTest("Event.commit() test failed: " + e.toString());
        }

        passTest();
    }

    private EventList openList(String aListName)
    {
        try
        {
            PIM pim = PIM.getInstance();
            return (EventList)pim.openPIMList(PIM.EVENT_LIST,
                                              PIM.READ_WRITE,
                                              aListName);
        }
        catch (Throwable e)
        {
            failTest("Failed to open PIM list");
        }
        return null;
    }

    private Event getSpecificEventItem(EventList aList, String aSubject) throws Exception
    {
        Event matchingItem = aList.createEvent();
        matchingItem.addString(Event.SUMMARY, PIMItem.ATTR_NONE, aSubject);
        Enumeration items = aList.items(matchingItem);
        // There should be at least one element
        return (Event)items.nextElement();
    }

    private Event createAndCommitItem(EventList aList)
    {
        Event event = aList.createEvent();
        // Initialize data
        if (aList.isSupportedField(Event.SUMMARY))
        {
            event.addString(Event.SUMMARY, PIMItem.ATTR_NONE, iItemSubject);
        }
        if (aList.isSupportedField(Event.START))
        {
            event.addDate(Event.START, PIMItem.ATTR_NONE, iStartDate.getTime());
        }
        if (aList.isSupportedField(Event.END))
        {
            event.addDate(Event.END, PIMItem.ATTR_NONE, iEndDate.getTime());
        }
        if (aList.isSupportedField(Event.ALARM))
        {
            event.addInt(Event.ALARM, PIMItem.ATTR_NONE, iAlarm);
        }
        try
        {
            event.commit();
        }
        catch (Exception e)
        {
            failTest("Ecommit::createAndCommitItem() - Exception when creating item");
        }
        return event;
    }

    private void init()
    {
        // Clean lists
        try
        {
            PIM pim = PIM.getInstance();
            iEventLists = pim.listPIMLists(PIM.EVENT_LIST);

            for (int i = 0; i < iEventLists.length; i++)
            {
                EventList list = openList(iEventLists[ i ]);
                Enumeration items = list.items();
                while (items.hasMoreElements())
                {
                    Event event = (Event)items.nextElement();
                    list.removeEvent(event);
                }
            }
        }
        catch (Exception e)
        {
            failTest("Ecommit::init() - Exception when cleaning lists");
        }

        iStartDate = new Date(System.currentTimeMillis());
        // End date to next day
        iEndDate = new Date(iStartDate.getTime() + 86400000);
        iItemSubject = "TestEventItem" + iStartDate.toString();
    }

    private Date getStartOfDay(Date aDate)
    {
        Calendar cal = Calendar.getInstance();

        cal.setTime(aDate);
        cal.set(Calendar.HOUR_OF_DAY, 0);
        cal.set(Calendar.MINUTE, 0);
        cal.set(Calendar.SECOND, 0);
        cal.set(Calendar.MILLISECOND, 0);

        return cal.getTime();
    }

    private int getDifferenceInSeconds(Date aFirstDate, Date aSecondDate)
    {
        double diff = (aFirstDate.getTime() - aSecondDate.getTime());
        int diffSecs = (((int)diff) / 1000);
        // Round full seconds
        diffSecs /= 60;
        return diffSecs*60;
    }

    private void testField(EventList aList, Event aItem, int aField, Object aExpectedResult)
    throws TestEndThrowable
    {
        boolean result = false;
        int type = aList.getFieldDataType(aField);
        debug("Ecommit::testField - testing field: " + aField + " + with value: " + aExpectedResult);
        switch (type)
        {
        case PIMItem.STRING:
        {
            String value = ((String)aExpectedResult);
            result = value.equals(aItem.getString(aField, 0));
            debug("Ecommit::testField - returned value: " + aItem.getString(aField, 0));
            break;
        }
        case PIMItem.INT:
        {
            Integer value = ((Integer)aExpectedResult);
            result = value.intValue() == aItem.getInt(aField, 0);
            debug("Ecommit::testField - returned value: " + aItem.getInt(aField, 0));
            debug("ATTENTION!!! Note that Java may have problems handling daylight saving times. Check that when this case fails");
            break;
        }
        case PIMItem.DATE:
        {
            Date value = ((Date)aExpectedResult);
            result = value.getTime() == aItem.getDate(aField, 0);
            debug("Ecommit::testField - returned value: " + new Date(aItem.getDate(aField, 0)).toString());
            debug("ATTENTION!!! Note that Java may have problems handling daylight saving times. Check that when this case fails");
            break;
        }
        case PIMItem.BOOLEAN:
        {
            Boolean value = ((Boolean)aExpectedResult);
            result = value.booleanValue() == aItem.getBoolean(aField, 0);
            debug("Ecommit::testField - returned value: " + aItem.getBoolean(aField, 0));
            break;
        }
        case PIMItem.STRING_ARRAY:
        default:
            assertString(false, "Ecommit::testField - test does not support field type: " + type);
            break;
        }
        assertString(result, "Ecommit::testField - incorrect value returned from field: " + aField);
    }
}
