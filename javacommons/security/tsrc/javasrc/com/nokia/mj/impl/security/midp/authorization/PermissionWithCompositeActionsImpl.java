/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.security.midp.authorization;

import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import java.security.*;

public class PermissionWithCompositeActionsImpl
        extends PermissionBase
{
    private SecurityPromptMessage pm = SecurityPromptMessage.getInstance();
    private String promptText;
    String actions;
    protected static final String ACTION1  = "action1";
    protected static final String ACTION2  = "action2";


    public PermissionWithCompositeActionsImpl(String uri, String actions)
    {
        this(uri, null, actions);
    }

    public PermissionWithCompositeActionsImpl(String uri, String promptText, String actions)
    {
        super(uri);
        this.promptText = promptText;
        this.actions = actions;
    }

    public boolean implies(Permission p)
    {
        if (p instanceof PermissionWithCompositeActionsImpl)
        {
            PermissionWithCompositeActionsImpl per = (PermissionWithCompositeActionsImpl)p;
            if (PermissionBase.matchActions(actions,per.actions))
            {
                return true;
            }
        }
        return false;
    }

    public String getActions()
    {
        return actions;
    }
    public int hashCode()
    {
        return 0;
    }

    public boolean equals(Object obj)
    {
        return true;
    }

    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        String q = null;
        if (ACTION1.equals(actions))
        {
            q = "Prompt for action1";
        }
        else if (ACTION2.equals(actions))
        {
            q = "Prompt for action2";
        }
        else if (PermissionBase.matchActions(actions, ACTION1 + "," + ACTION2))
        {
            q = "Prompt for action1 and action2";
        }

        if (promptText != null)
        {
            q = q + promptText;
        }
        return q;
    }

    public String toString()
    {
        return "(PermissionWithCompositeActionsImpl" + " " + getName() + " " + getActions() + ")";
    }

    public PermissionCollection newPermissionCollection()
    {
        return null;
    }

}
