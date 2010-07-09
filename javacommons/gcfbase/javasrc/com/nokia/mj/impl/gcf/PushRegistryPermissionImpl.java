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


package com.nokia.mj.impl.gcf;

import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import java.security.Permission;

/**
 * This class is used by security framework to get text for the security prompt.
 */
public final class PushRegistryPermissionImpl extends PermissionBase
{

    public String mAction;
    public String mBtDeviceName = null;

    /**
     * This constructor is needed for security framework.
     */
    public PushRegistryPermissionImpl(String aName)
    {
        super(aName);
        mAction = "";
        mBtDeviceName = null;
    }

    /**
     * This constructor is needed for security framework.
     */
    public PushRegistryPermissionImpl(String aName, String action)
    {
        super(aName);
        mAction = action;
        //mBtDeviceName = aBtDeviceName;
        mBtDeviceName = null;
    }

    /**
     *
     */
    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        if (mAction.equals("staticregistration"))
        {
            return SecurityPromptMessage.getInstance().getText
                   (SecurityPromptMessage.QUESTION_ID_PUSH_STATIC_REGISTRATION,null);
        }
        else if (mAction.equals("dynamicregistration"))
        {
            return SecurityPromptMessage.getInstance().getText
                   (SecurityPromptMessage.QUESTION_ID_PUSH_DYNAMIC_REGISTRATION,null);
        }
        else if (mAction.equals("autoinvocation"))
        {
            if (null == mBtDeviceName)
            {
                return SecurityPromptMessage.getInstance().getText
                       (SecurityPromptMessage.QUESTION_ID_AUTO_INVOCATION,null);
            }
            else if (mBtDeviceName.equals(""))
            {
                return SecurityPromptMessage.getInstance().getText
                       (SecurityPromptMessage.QUESTION_ID_AUTO_INVOCATION,null);
            }
            else
            {
                return SecurityPromptMessage.getInstance().getText
                       (SecurityPromptMessage.QUESTION_ID_BLUETOOTH_USAGE,
                        new String[] {mBtDeviceName});
            }
        }
        return null;

    }
    public String getSecurityPromptAnswerOption(int aAnswerOption)
    {
        switch (aAnswerOption)
        {
        case ONESHOT_ANSWER_OPTION:
        case SESSION_ANSWER_OPTION:
        case BLANKET_ANSWER_OPTION:
            return SecurityPromptMessage.getInstance().getText(
                       SecurityPromptMessage.ANSWER_ID_ALLOW, null);
        }
        return null;
    }

    public boolean isSecurityPromptNeeed(int aInteractionMode)
    {
        if (aInteractionMode == SESSION_INTERACTION_MODE
                && mAction.equals("autoinvocation"))
            return false;
        return true;
    }

    /**
     *
     */
    public String toString()
    {
        return "javax.microedition.io.PushRegistry";
    }

    /**
     *
     */
    public boolean equals(Object obj)
    {
        if (!(obj instanceof PushRegistryPermissionImpl))
            return false;

        PushRegistryPermissionImpl tmpObj = (PushRegistryPermissionImpl) obj;
        if (getName().equals(tmpObj.getName()) && matchActions(mAction,tmpObj.mAction))
            return true;
        return false;
    }

    /**
     *
     */
    public int hashCode()
    {
        return 0;
    }

    /**
     *
     */
    public String getActions()
    {
        return "";
    }

    /**
     *
     */
    public boolean implies(Permission p)
    {
        if (p instanceof PushRegistryPermissionImpl)
        {
            return true;
        }
        return false;
    }

}//end PermissionBase

