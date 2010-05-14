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


package com.nokia.mj.impl.media;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class PlayerPermission extends PermissionBase
{
    String iAction;

    public PlayerPermission(String aUri, String aAction)
    {
        super(aUri);
        iAction = aAction ;
    }

    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @return the localized text associated with the security prompt
     */
    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        return (SecurityPromptMessage.getInstance()).getText(
                   SecurityPromptMessage.QUESTION_ID_AUDIO_VIDEO_RECORDING,
                   null);
    }


    public String toString()
    {
        if (iAction.compareTo("record") == 0)
        {
            return "javax.microedition.media.control.RecordControl";
        }
        else if (iAction.compareTo("snapshot") == 0)
        {
            return "javax.microedition.media.control.VideoControl.getSnapshot";
        }
        return "";
    }

    public boolean implies(Permission p)
    {
        if (p instanceof PlayerPermission)
        {
            PlayerPermission per = (PlayerPermission)p;
            return matchActions(iAction, per.iAction);
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