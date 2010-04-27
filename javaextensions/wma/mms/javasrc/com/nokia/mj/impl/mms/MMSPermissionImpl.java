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

package com.nokia.mj.impl.mms;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class MMSPermissionImpl extends PermissionBase
{

    public String iAction = null;
    private int iRecipientsCount = 0;

    public MMSPermissionImpl(String aUri,String aAction)
    {
        super(aUri);
        iAction = aAction;
    }
    public MMSPermissionImpl(String aUri,String aAction,int aRecipientsCount)
    {
        super(aUri);
        iAction = aAction;
        iRecipientsCount = aRecipientsCount;
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
                       SecurityPromptMessage.QUESTION_ID_SEND_RECEIVE_MESSAGES,
                       null);
        }
        // either provide own localized text
        // or use the pre-defined texts
        // Note: some permissions use the same text always (case in which the
        // ID and the params for the text could be chosen at this phase) and
        // some permissions use a more dynamic text, depending on the context
        // in which they are used, case in which the ID and the params could
        // come from the constructor
        if (iAction.compareTo("receive") == 0)
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_MMS_RECEIVING, null);
        }
        else if (iAction.compareTo("open") == 0)
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_MMS_OPENING_CONN, null);
        }
        else if (iAction.compareTo("send") == 0)
        {
            if (iRecipientsCount == 1)
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_MMS_SINGLE_SENDING,
                           null);
            }
            else
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_MMS_SINGLE_SENDING_MULTIPLE_DESTINATIONS,
                           new Object[] {new Integer(iRecipientsCount)});
            }
        }
        return null;
    }

    public String toString()
    {
        if (iAction.compareTo("open") == 0)
        {
            return "javax.microedition.io.Connector.mms";
        }
        else if (iAction.compareTo("send") == 0)
        {
            return "javax.wireless.messaging.mms.send";
        }
        else if (iAction.compareTo("receive") == 0)
        {
            return "javax.wireless.messaging.mms.receive";
        }
        return null;
    }

    public boolean implies(Permission permission)
    {
        if (permission instanceof MMSPermissionImpl)
        {
            MMSPermissionImpl perm = (MMSPermissionImpl)permission;
            if (matchActions(iAction,perm.iAction))
            {
                return true;
            }
        }
        return false;
    }

    public int hashCode()
    {
        return 0;
    }

    public boolean equals(Object obj)
    {
        return true;
    }

    public String getActions()
    {
        return "";
    }

    public PermissionCollection newPermissionCollection()
    {
        return null;
    }

}
