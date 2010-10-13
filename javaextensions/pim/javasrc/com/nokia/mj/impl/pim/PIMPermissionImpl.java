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
* Description:  PIM permission check wrapper.
 *
*/



// PACKAGE
package com.nokia.mj.impl.pim;

import javax.microedition.pim.PIM;
import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class PIMPermissionImpl extends PermissionBase
{
    protected static final int OPERATION_ITEM_DELETION     = 1;
    protected static final int OPERATION_ITEM_COMMIT       = 2;
    protected static final int OPERATION_CATEGORY_DELETION = 3;

    protected static final String ACTION_READ_CONTACTS  = "read_contacts";
    protected static final String ACTION_WRITE_CONTACTS = "write_contacts";
    protected static final String ACTION_READ_TODOS     = "read_todos";
    protected static final String ACTION_WRITE_TODOS    = "write_todos";
    protected static final String ACTION_READ_EVENTS    = "read_events";
    protected static final String ACTION_WRITE_EVENTS   = "write_events";

    int iOperation ;

    String iListName = null;
    String iItemInfo = null;
    String iAction;


    PIMPermissionImpl(String aUri, String aAction)
    {
        super(aUri);
        iAction = aAction ;
    }

    PIMPermissionImpl(String aAction, String aItemInfo, String aListName, int aOperation)
    {
        super("pim://*");
        iOperation = aOperation;
        iListName = aListName;
        iItemInfo = aItemInfo;
        iAction = aAction;
    }

    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @return the localized text associated with the security prompt
     */
    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        if (aInteractionMode == SESSION_INTERACTION_MODE ||
                aInteractionMode == BLANKET_INTERACTION_MODE)
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_MANAGE_USER_DATA,
                       null);
        }

        // either provide own localized text
        // or use the pre-defined texts
        // Note: some permissions use the same text always (case in which the
        // ID and the params for the text could be chosen at this phase) and
        // some permissions use a more dynamic text, depending on the context
        // in which they are used, case in which the ID and the params could
        // come from the constructor
        if (iOperation == OPERATION_ITEM_DELETION)
        {
            if (iItemInfo == null)
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_DELETING_UNNAMED_ITEM,
                           new String[] {iListName});
            }
            else
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_DELETING_ITEM,
                           new String[] {iListName});
            }
        }
        else if (iOperation == OPERATION_ITEM_COMMIT)
        {
            if (iItemInfo == null)
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_UPDATING_UNNAMED_ITEM,
                           new String[] {iListName});
            }
            else
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_UPDATING_ITEM,
                           new String[] {iListName});
            }
        }
        else if (iOperation == OPERATION_CATEGORY_DELETION)
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_DELETING_CATEGORY,
                       new String[] {iItemInfo, iListName});
        }
        else if (ACTION_READ_CONTACTS.equals(iAction))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_READING_CONTACTS,null);
        }
        else if (ACTION_READ_EVENTS.equals(iAction))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_READING_EVENTS,null);
        }
        else if (ACTION_READ_TODOS.equals(iAction))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_READING_TODOS,null);
        }
        else if (ACTION_WRITE_CONTACTS.equals(iAction))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_MODIFYING_CONTACTS,null);
        }
        else if (ACTION_WRITE_EVENTS.equals(iAction))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_MODIFYING_EVENTS,null);
        }
        else if (ACTION_WRITE_TODOS.equals(iAction))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_MODIFYING_TODOS,null);
        }
        else if (PermissionBase.matchActions(iAction, ACTION_READ_EVENTS + "," + ACTION_READ_TODOS))
        {
            // list a calendar
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_LISTING_CALENDARS,null);
        }
        else if (PermissionBase.matchActions(iAction, ACTION_WRITE_EVENTS + "," + ACTION_WRITE_TODOS))
        {
            if (iItemInfo == null)
            {
                // create calendar
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_ADDING_CALENDAR,null);
            }
            else
            {
                // delete calendar
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_DELETING_CALENDAR,
                           new String[] {iItemInfo});
            }
        }
        return null;
    }


    public String toString()
    {
        if (ACTION_READ_CONTACTS.equals(iAction))
        {
            return "javax.microedition.pim.ContactList.read";
        }
        else if (ACTION_READ_EVENTS.equals(iAction))
        {
            return "javax.microedition.pim.EventList.read";
        }
        else if (ACTION_READ_TODOS.equals(iAction))
        {
            return "javax.microedition.pim.ToDoList.read";
        }
        else if (ACTION_WRITE_CONTACTS.equals(iAction))
        {
            return "javax.microedition.pim.ContactList.write";
        }
        else if (ACTION_WRITE_EVENTS.equals(iAction))
        {
            return "javax.microedition.pim.EventList.write";
        }
        else if (ACTION_WRITE_TODOS.equals(iAction))
        {
            return "javax.microedition.pim.ToDoList.write";
        }
        else if (PermissionBase.matchActions(iAction, ACTION_READ_EVENTS + "," + ACTION_READ_TODOS))
        {
            return "javax.microedition.pim.EventList.read, javax.microedition.pim.ToDoList.read";
        }
        else if (PermissionBase.matchActions(iAction, ACTION_WRITE_EVENTS + "," + ACTION_WRITE_TODOS))
        {
            return "javax.microedition.pim.EventList.write, javax.microedition.pim.ToDoList.write";
        }
        return null;
    }

    public boolean implies(Permission p)
    {
        if (p instanceof PIMPermissionImpl)
        {
            PIMPermissionImpl per = (PIMPermissionImpl)p;
            if (PermissionBase.matchActions(iAction,per.iAction))
            {
                return true;
            }
        }
        return false;
    }

    public boolean equals(Object obj)
    {
        return true;
    }

    public int hashCode()
    {
        return 0;
    }

    public String getActions()
    {
        return iAction;
    }

    public PermissionCollection newPermissionCollection()
    {
        return null;
    }


}