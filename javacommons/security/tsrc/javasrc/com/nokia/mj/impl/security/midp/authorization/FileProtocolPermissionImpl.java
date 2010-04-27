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
import javax.microedition.io.*;

public class FileProtocolPermissionImpl
        extends PermissionBase
{
    private FileProtocolPermission iHelper;
    private SecurityPromptMessage pm = SecurityPromptMessage.getInstance();
    private String targetName;
    private String promptText;

    public FileProtocolPermissionImpl(String uri)
    {
        this(uri, null, null);
    }

    public FileProtocolPermissionImpl(String uri, String actions, String promptText)
    {
        super(uri);
        iHelper = new FileProtocolPermission(uri,actions);
        targetName = "myFile200.txt";
        this.promptText = promptText;
    }

    public FileProtocolPermissionImpl(String uri, String actions)
    {
        this(uri, actions, null);
    }

    public boolean implies(Permission p)
    {
        if (p instanceof FileProtocolPermissionImpl)
        {
            return true;
        }
        return iHelper.implies(p);
    }

    public String getActions()
    {
        return iHelper.getActions();
    }

    public int hashCode()
    {
        return iHelper.hashCode();
    }

    public boolean equals(Object obj)
    {
        return iHelper.equals(obj);
    }

    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        String q = "";
        if (promptText != null)
        {
            q = q + " " + promptText;
        }
        return "File access prompt " + q;
    }

    public String getSecurityPromptAnswerOption(int aAnswerOption)
    {
        switch (aAnswerOption)
        {
        case ONESHOT_ANSWER_OPTION:
            return pm.getText(
                       SecurityPromptMessage.ANSWER_ID_ALLOW, null);
        case SESSION_ANSWER_OPTION:
            return pm.getText(
                       SecurityPromptMessage.ANSWER_ID_ALLOW, null);
        case BLANKET_ANSWER_OPTION:
            return "Allow for all files";
        }
        return null;
    }

    public String toString()
    {
        return "(FileProtocolPermission" + " " + getName() + " " + getActions() + ")";
    }

}

