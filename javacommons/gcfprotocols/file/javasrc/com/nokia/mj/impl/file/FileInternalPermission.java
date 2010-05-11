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

package com.nokia.mj.impl.file;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

/**
 * FileInternalPermission acts as an interface between Security and File API
 * specifics.
 * <p>
 * All paths on device are divided into categories:<br/> PUBLIC_DIRS:
 * Accessible by all domains in any mode (read/write) <br/> PRIVATE_USER_FILES:
 * Direcotries other than those part of PUBLIC_DIRS and not part of other
 * categories<br/> HOME_DIR: Application's private directory.<br/>
 * RESTRICTED_PUBLIC_FILES: Those paths that are part of PUBLIC_DIRS but no
 * write operation can be performed on these.<br/>
 * <p>
 * Security's access policy file gives access to these categories based on
 * domains and decides on the prompting as well. It does so by creating
 * instances of FileInternalPermission with these categories as target and
 * read/write as the intent.
 * <p>
 * FileInternalPermission objects are passed to implies() method to check for a
 * match. It now becomes the responsibility of implies() to check if the
 * category of the offered permission is the same as the path that we are trying
 * to check access to.
 * <p>
 * Usage should be something like this in File API:
 *
 * <pre>
 * FileInternalPermission per = new FileInternalPermission(
 *      &quot;C:/Data/Images/someFile&quot;, &quot;read&quot;);
 * ApplicationUtils.getInstance().checkPermission(per);
 * </pre>
 *
 * This per, is checked against pre-created permission. Implies method returns
 * true in case the categories match.
 *
 */
public class FileInternalPermission extends PermissionBase
{
    // In case it is created by Security, it will be one of the
    // categories. If File API creates it, it will contain the absolute path.
    private String targetName;

    // Intent can be read or write. Composite actions are not considered.
    private String intent;

    public FileInternalPermission(String aUri)
    {
        // By default, give read permissions.
        this(aUri, "read");
    }

    public FileInternalPermission(String aUri, String aActions)
    {
        super(aUri);
        targetName = aUri;
        intent = aActions;
    }

    public boolean implies(Permission aPermission)
    {
        if (aPermission instanceof FileInternalPermission)
        {
            // This is in case the permission was pregranted
            FileInternalPermission per = (FileInternalPermission) aPermission;

            if (PermissionBase.matchActions(per.getActions(), intent))
            {
                return true;
            }
        }

        return false;
    }

    public String toString()
    {
        return "javax.microedition.io.FileProtocolPermission";
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

        if (intent.equals("read"))
        {
            if (targetName.equals(""))
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_READING_USER_DATA,
                           new String[] { targetName });
            }

            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_READING_FILE,
                       new String[] { targetName });
        }

        if (intent.equals("write"))
        {
            if (targetName.equals(""))
            {
                return (SecurityPromptMessage.getInstance()).getText(
                           SecurityPromptMessage.QUESTION_ID_WRITING_USER_DATA,
                           new String[] { targetName });
            }

            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_MODIFYING_FILE,
                       new String[] { targetName });
        }

        FileLogger.ELog("FileInternalPermission: getSecurityPromptQuestion: "
                        + "Invalid security intent presented: " + intent
                        + " for target: " + targetName);
        return null;
    }

    public String getTarget()
    {
        return targetName;
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
        return intent;
    }

    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}
