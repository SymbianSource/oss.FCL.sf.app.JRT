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

package com.nokia.mj.impl.pki;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class SignaturePermission extends PermissionBase
{

    public String iAction = null;

    public SignaturePermission(String aUri,String aAction)
    {
        super(aUri);
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
        if ((iAction.compareTo("authenticate") == 0))
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_AUTH_SERVICES_USAGE, null);
        }
        return null;
    }

    public String toString()
    {
        if (iAction.compareTo("authenticate") == 0)
        {
            return "javax.microedition.securityservice.CMSMessageSignatureService";
        }
        return null;
    }

    public boolean implies(Permission permission)
    {
        if (permission instanceof SignaturePermission)
        {
            SignaturePermission perm = (SignaturePermission)permission;
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
