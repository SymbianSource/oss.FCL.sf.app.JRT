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


package com.nokia.mj.impl.properties.pim;

import com.nokia.mj.impl.pim.PIMManager;
import com.nokia.mj.impl.pim.Calendar;
import com.nokia.mj.impl.rt.support.SystemPropertyProvider;

/**
 * This class implements SystemPropertyProvider and provides getProperty to
 * retrieve dynamic system property.
 */
public final class DynamicPropertyHandler implements SystemPropertyProvider
{

    public String getProperty(String key)
    {

        //Code for list calendars - Call Open PIM List
        // MIDlet should know about its own calendars, but we could offer also system property (e.g.) com.nokia.mid.calendars, which lists comma-separated local calendarnames:
        // String calendarNames = System.getProperty("com.nokia.mid.calendars");

        PIMManager pim = new PIMManager();

        Calendar calendarList[] = pim.listCalendars();

        StringBuffer calendarListString = new StringBuffer();;

        if (calendarList != null)
        {
            for (int i = 0; i < calendarList.length; i++)
            {
                calendarListString.append(calendarList[i].getCalendarName()+",");

            }
        }
        else
        {
            return null;
        }
        pim = null;
        calendarListString.deleteCharAt(calendarListString.length() - 1);
        return calendarListString.toString();
    }

    public boolean isStatic(String key)
    {
        return false;
    }

}