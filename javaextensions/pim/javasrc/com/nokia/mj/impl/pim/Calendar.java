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
* Description:  Abstract PIMExtension class.
 *
*/

// PACKAGE
package com.nokia.mj.impl.pim;

// CLASS DEFINITION
/**
 * <p>
 * Nokia propritery class for holding the calendar file name and calendar display name.
 * </p>

 * <h3>General</h3>
 * The object of the class will be returned to user when listcalendars is called from the MIDlet.
 * Using the object user can obtain either calendar file name or calendar display name. These names should be used in appropriate places.
 * <ul>
 * <li>
 * <h3>Sample MIDlet code snippet</h3>
 *
 * <h4>Opening a Memo List from a calendar name Personal available on C drive</h4>
 *
 * <code>
 * import javax.microedition.pim.*; <br>
 * import com.nokia.mid.pimextension.PIMExtension; <br>
 * import com.nokia.mid.pimextension.calendar;<br>
 * <pre>
 *     ...
 *     // Gets a PIMExtended class instance
 *     PIM pim = PIM.getInstance();
 *     try {
 *     PIMExtension pimExtension = (PIMExtension) pim;
 *     }
 *     catch (ClassCastException e){
 *     // extension is not avaialable
 *     }
 *
 *     // Gets the list of calendars available on the device
 *     calendar[] calList = pimExtension.listCalendars();
 *
 *     // Opens the memo list from the "Personal" calendar
 *     EventList memoList = ( EventList )
 *         calPIM.openPIMList(PIM.EVENT_LIST, PIM.READ_WRITE, "Memos", calList.getCalendarFileName);
 *  ...
 *  </pre>
 * </code>
 */
public final class Calendar
{
    /*
    *
    * two strings to hold calendar File Name and Calendar name. These should not be accesible, hence they are private.
    *
    */
    private String calendarFileName;
    private String calendarName;
    /*
    *
    *constructor accessed by jrt implementation to construct the object with the 2 different names of the calendar.
    *
    */
    public Calendar(String fileName, String userName)
    {
        calendarFileName = fileName;
        calendarName = userName;
    }
    /*
    *
    *
    * This functions getCalendarFileName() returns the calendar file name in the database.
    * This string should be passed to all the operations on calendar.
    *
    */
    public String getCalendarFileName()
    {
        return calendarFileName;
    }

    /*
    *
    *
    * This functions getCalendarName() returns the display name of the calendar.
    * This should be used for user display purposes.
    * Note: When user creates a calendar from java side, both the names will be same.
    * The file created will be named as the string passed by user.
    * The calendar will also be named with the string.
    *
    */
    public String getCalendarName()
    {
        return calendarName;
    }
}