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

package com.nokia.mj.impl.security.common;

import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import com.nokia.mj.impl.utils.Tokenizer;

/**
 * Base class for internal permissions, extending the public
 * base permission (java.security.Permission) with the concept
 * of security prompt
 */
public abstract class PermissionBase
        extends java.security.Permission
{

    // constants for the answer options
    public static final int BLANKET_ANSWER_OPTION = 1;
    public static final int SESSION_ANSWER_OPTION = 2;
    public static final int ONESHOT_ANSWER_OPTION = 3;

    // constants for the interaction modes
    public static final int ONESHOT_INTERACTION_MODE = 1;
    public static final int SESSION_INTERACTION_MODE = 2;
    public static final int BLANKET_INTERACTION_MODE = 3;

    /**
     * Constructs a permission with the specified name.
     *
     * @param name name of the Permission object being created.
     */
    public PermissionBase(String name)
    {
        super(name);
    }

    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @param aInteractionMode ONESHOT_INTERACTION_MODE or
     *                         SESSION_INTERACTION_MODE or
     *                         BLANKET_INTERACTION_MODE
     * @return the localized text associated with the security prompt
     */
    public abstract String getSecurityPromptQuestion(int aInteractionMode);

    /**
     * Returns the string representation of this permission. The returned
     * string must contain at least the name of the permission class and the
     * name of the target. Optionally the returned string can contain the
     * actions (if applicable).
     * The returned permission class name MUST NOT be the name of the
     * internal class, instead it MUST be the name of the corresponding
     * public permission class
     * (e.g. javax.microedition.io.FileProtocolPermission for the case
     * of File API)
     *
     * @return the string representation of the permission
     */
    public abstract String toString();

    /**
     * Returns the localized text associated with certain answer option
     * displayed in the security prompt
     *
     * @param aAnswerOption BLANKET_ANSWER_OPTION or
     *                      SESSION_ANSWER_OPTION or
     *                      ONESHOT_ANSWER_OPTION
     * @return              the localized text associated with the queried
     *                      answer option
     */
    public String getSecurityPromptAnswerOption(int aAnswerOption)
    {
        SecurityPromptMessage securityPromptMessage = SecurityPromptMessage
                .getInstance();
        switch (aAnswerOption)
        {
        case ONESHOT_ANSWER_OPTION:
            return securityPromptMessage.getText(
                       SecurityPromptMessage.ANSWER_ID_ALLOW, null);
        case SESSION_ANSWER_OPTION:
            return securityPromptMessage.getText(
                       SecurityPromptMessage.ANSWER_ID_ALLOW, null);
        case BLANKET_ANSWER_OPTION:
            return securityPromptMessage.getText(
                       SecurityPromptMessage.ANSWER_ID_ALLOW, null);
        }
        return null;
    }

    /**
     * Returns a boolean indicating if the security prompt is needed for
     * a certain interaction mode.
     * Most of the subclasses should be fine with the default implementation.
     *
     * @param aInteractionMode ONESHOT_INTERACTION_MODE or
     *                         SESSION_INTERACTION_MODE or
     *                         BLANKET_INTERACTION_MODE
     * @return                 true if the security prompt is needed,
     *                         false otherwise
     */
    public boolean isSecurityPromptNeeed(int aInteractionMode)
    {
        return true;
    }

    /**
     * Utility function for matching two actions. Matching is case insensitive.
     * Both single and composite actions are supported. Composite action is
     * defined as a comma separated list of single actions (e.g. "read,write"
     * is a composite action made of two single actions "read" and "write").
     *
     * Usage example:
     *    matchActions("read","read") returns true
     *    matchActions("read,write","read") returns true
     *    matchActions("read,open,write","write,read") returns true
     *
     * @param aAction1 the action against which the match is done
     * @param aAction2 the action which is matched
     * @return         true if aAction2 matches aAction1; false otherwise
     */
    public static boolean matchActions(String aAction1, String aAction2)
    {
        if (aAction1 == null
                && aAction2 == null)
        {
            return true;
        }
        if (aAction1 == null
                || aAction2 == null)
        {
            return false;
        }
        String[] actions1 = Tokenizer.split(aAction1,",");
        String[] actions2 = Tokenizer.split(aAction2,",");
        for (int i=0; i<actions2.length; i++)
        {
            boolean found = false;
            for (int j=0; j<actions1.length; j++)
            {
                if (actions2[i].equalsIgnoreCase(actions1[j]))
                {
                    found = true;
                }
            }
            if (!found)
            {
                return false;
            }
        }
        return true;
    }
}
