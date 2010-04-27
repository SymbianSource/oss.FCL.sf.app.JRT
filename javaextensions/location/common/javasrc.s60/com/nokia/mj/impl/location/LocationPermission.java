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

package com.nokia.mj.impl.location;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class LocationPermission extends PermissionBase
{
    public String iAction = null;

    public LocationPermission(String aName, String aAction)
    {
        super(aName);
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
        // either provide own localized text
        // or use the pre-defined texts
        // Note: some permissions use the same text always (case in which the
        // ID and the params for the text could be chosen at this phase) and
        // some permissions use a more dynamic text, depending on the context
        // in which they are used, case in which the ID and the params could
        // come from the constructor
        if (matchActions(iAction, "position"))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_POSITIONING_DATA_USAGE,
                       null);
        }
        else if (matchActions(iAction, "orientation"))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_ORIENTATION_DATA_USAGE,
                       null);
        }
        else if ((matchActions(iAction, "read"))
                 || (matchActions(iAction, "write"))
                 || (matchActions(iAction, "category"))
                 || (matchActions(iAction, "management")))
        {
            return (SecurityPromptMessage.getInstance())
                   .getText(
                       SecurityPromptMessage.QUESTION_ID_LANDMARK_DATA_USAGE,
                       null);
        }
        else if (matchActions(iAction, "registerlistener"))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_PROXIMITY_LISTENER_REGISTRATION,
                       null);
        }
        return null;
    }

    public String toString()
    {
        String result = null;
        if (matchActions(iAction, "position"))
        {
            result = "javax.microedition.location.Location";
        }
        else if (matchActions(iAction, "orientation"))
        {
            result = "javax.microedition.location.Orientation";
        }
        else if (matchActions(iAction, "read"))
        {
            result = "javax.microedition.location.LandmarkStore.read";
        }
        else if (matchActions(iAction, "write"))
        {
            result = "javax.microedition.location.LandmarkStore.write";
        }
        else if (matchActions(iAction, "category"))
        {
            result = "javax.microedition.location.LandmarkStore.category";
        }
        else if (matchActions(iAction, "management"))
        {
            result = "javax.microedition.location.LandmarkStore.management";
        }
        else if (matchActions(iAction, "registerlistener"))
        {
            result = "javax.microedition.location.ProximityListener";
        }
        return result;
    }

    public boolean implies(Permission perm)
    {
        if (perm instanceof LocationPermission)
        {
            LocationPermission permission = (LocationPermission) perm;
            if (matchActions(iAction, permission.iAction))
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
