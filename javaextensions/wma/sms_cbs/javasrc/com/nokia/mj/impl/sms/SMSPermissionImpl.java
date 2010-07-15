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

package com.nokia.mj.impl.sms;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import com.nokia.mj.impl.utils.Formatter;

public class SMSPermissionImpl extends PermissionBase
{
    public String iAction = null;

    String iDestination = null;

    int iNumberOfSegments = 0;

    public SMSPermissionImpl(String aUri,String aAction)
    {
        super(aUri);
        iAction = aAction;
    }
    public SMSPermissionImpl(String aUri,String aAction,int aNumberOfSegments,String aDestNumber)
    {
        super(aUri);
        iAction = aAction;
        iNumberOfSegments = aNumberOfSegments;
        iDestination = aDestNumber;
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
                       SecurityPromptMessage.QUESTION_ID_SMS_RECEIVING, null);
        }
        else if (iAction.compareTo("open") == 0)
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_SMS_OPENING_CONN, null);
        }
        else if (iAction.compareTo("send") == 0)
        {
            String prefix = "";
            String formattedDestination = iDestination;
            if (formattedDestination.startsWith("+"))
            {
                formattedDestination = iDestination.substring(1);
                prefix = "+";
            }
            formattedDestination = prefix + Formatter.formatDigits(formattedDestination);
            if (iNumberOfSegments > 1)
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_SMS_SENDING,
                           new Object[] {new Integer(iNumberOfSegments),
                           formattedDestination});
            }
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_SMS_SINGLE_SENDING,
                       new Object[] {formattedDestination});
        }
        return null;
    }

    public String toString()
    {
        if (iAction.compareTo("open") == 0)
        {
            return "javax.microedition.io.Connector.sms";
        }
        else if (iAction.compareTo("send") == 0)
        {
            return "javax.wireless.messaging.sms.send";
        }
        else if (iAction.compareTo("receive") == 0)
        {
            return "javax.wireless.messaging.sms.receive";
        }
        return null;
    }

    public boolean implies(Permission perm)
    {
        if (perm instanceof SMSPermissionImpl)
        {
            SMSPermissionImpl permission = (SMSPermissionImpl) perm;
            if (matchActions(iAction,permission.iAction))
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
