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

package com.nokia.mj.impl.properties.mobinfo;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class MobileInfoPermission extends PermissionBase
{
    // the known target names
    private static final String IMSI_TARGET_NAME = "mobinfo.imsi";
    private static final String MSISDN_TARGET_NAME = "mobinfo.msisdn";
    private static final String CELLID_TARGET_NAME = "mobinfo.cellid";
    private static final String COUNTRYCODE_TARGET_NAME = "mobinfo.countrycode";
    private static final String NETWORKID_TARGET_NAME = "mobinfo.networkid";
    private static final String PUBLIC_INFO_TARGET_NAME = "mobinfo.publicinfo";
    private static final String ONS_TARGET_NAME = "mobinfo.ons";
    private static final String SPN_TARGET_NAME = "mobinfo.spn";

    private String iTarget = null;
    private String iAction = null;

    public MobileInfoPermission(String aUri,String aAction)
    {
        super(aUri);
        // figure out the target
        if (IMSI_TARGET_NAME.equals(aUri)
                || MSISDN_TARGET_NAME.equals(aUri)
                || CELLID_TARGET_NAME.equals(aUri)
                || PUBLIC_INFO_TARGET_NAME.equals(aUri)
                || COUNTRYCODE_TARGET_NAME.equals(aUri)
                || NETWORKID_TARGET_NAME.equals(aUri)
                || SPN_TARGET_NAME.equals(aUri)
                || ONS_TARGET_NAME.equals(aUri))
        {
            // aUri contains a known target name -> save it as such
            iTarget = aUri;
        }
        else
        {
            // the aUri contains the property name
            // -> do the mapping between the property name and the known
            //    target names
            if (MobileInfoProperties.IMSI.equals(aUri))
            {
                iTarget = IMSI_TARGET_NAME;
            }
            else if (MobileInfoProperties.MSISDN.equals(aUri))
            {
                iTarget = MSISDN_TARGET_NAME;
            }
            else if (MobileInfoProperties.CELLID.equals(aUri))
            {
                iTarget = CELLID_TARGET_NAME;
            }
            else if (MobileInfoProperties.COUNTRY_CODE.equals(aUri))
            {
                iTarget = COUNTRYCODE_TARGET_NAME;
            }
            else if (MobileInfoProperties.NETWORK_ID.equals(aUri))
            {
                iTarget = NETWORKID_TARGET_NAME;
            }
            else if (MobileInfoProperties.SERVICE_PROVIDER_NAME.equals(aUri))
            {
                iTarget = SPN_TARGET_NAME;
            }
            else if (MobileInfoProperties.OPERATOR_NAME.equals(aUri))
            {
                iTarget = ONS_TARGET_NAME;
            }
            else
            {
                iTarget = PUBLIC_INFO_TARGET_NAME;
            }
        }
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
        if (CELLID_TARGET_NAME.equals(iTarget)
            || COUNTRYCODE_TARGET_NAME.equals(iTarget)
            || NETWORKID_TARGET_NAME.equals(iTarget))
        {
            return SecurityPromptMessage.getInstance().getText(
                SecurityPromptMessage.QUESTION_ID_READING_LOCATION_DATA,
                null);
        }
        if (SPN_TARGET_NAME.equals(iTarget)
            || ONS_TARGET_NAME.equals(iTarget))
        {
            return SecurityPromptMessage.getInstance().getText(
                SecurityPromptMessage.QUESTION_ID_READING_USER_DATA,
                null);
        }

        return null;
    }

    public String toString()
    {
        if (IMSI_TARGET_NAME.equals(iTarget))
        {
            return "com.nokia.mid.Mobinfo.IMSI";
        }
        else if (MSISDN_TARGET_NAME.equals(iTarget))
        {
            return "com.nokia.mid.Mobinfo.MSISDN";
        }
        else if (CELLID_TARGET_NAME.equals(iTarget))
        {
            return "com.nokia.mid.Mobinfo.cellid";
        }
        else if (COUNTRYCODE_TARGET_NAME.equals(iTarget))
        {
            return "com.nokia.mid.Mobinfo.countrycode";
        }
        else if (NETWORKID_TARGET_NAME.equals(iTarget))
        {
            return "com.nokia.mid.Mobinfo.networkid";
        }
        else if (SPN_TARGET_NAME.equals(iTarget))
        {
            return "com.nokia.mid.Mobinfo.SPN";
        }
        else if (ONS_TARGET_NAME.equals(iTarget))
        {
            return "com.nokia.mid.Mobinfo.ONS";
        }
        return null;
    }

    public boolean implies(Permission permission)
    {
        if (permission instanceof MobileInfoPermission)
        {
            MobileInfoPermission perm = (MobileInfoPermission)permission;
            // the target and action must be identical
            if (((iTarget != null && iTarget.equalsIgnoreCase(perm.iTarget))
                    || (iTarget == null && perm.iTarget == null)) &&
                    ((iAction != null && iAction.equalsIgnoreCase(perm.iAction))
                     || (iAction == null && perm.iAction == null)))
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
        return iAction;
    }

    public PermissionCollection newPermissionCollection()
    {
        return null;
    }

}
