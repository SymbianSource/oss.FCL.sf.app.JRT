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

package com.nokia.mj.impl.security.midp.authorization;

import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

/**
 * Variant of UserSecuritySettings correlated to the security policy
 */
public class SecurityPolicyPermissionSettings
        implements UserSecuritySettings
{
    private String name;
    private int currentInteractionMode;
    private int[] allowedInteractionModes;

    public SecurityPolicyPermissionSettings(
        String name,
        int currentInteractionMode,
        int[] allowedInteractionModes)
    {
        this.name = name;
        this.currentInteractionMode = currentInteractionMode;
        this.allowedInteractionModes = allowedInteractionModes;
    }

    public String getName()
    {
        return name;
    }

    public int getCurrentInteractionMode()
    {
        return currentInteractionMode;
    }

    public int[] getAllowedInteractionModes()
    {
        return allowedInteractionModes;
    }

    public boolean getBlanketPrompt()
    {
        return false;
    }

    public boolean isInteractionModeAllowed(int interactionMode)
    {
        for (int i=0; allowedInteractionModes != null
                && i<allowedInteractionModes.length; i++)
        {
            if (allowedInteractionModes[i] == interactionMode)
            {
                return true;
            }
        }
        return false;
    }

    public boolean equals(Object o)
    {
        if (this == o)
        {
            return true;
        }
        if (!(o instanceof SecurityPolicyPermissionSettings))
        {
            return false;
        }
        SecurityPolicyPermissionSettings settings =
            (SecurityPolicyPermissionSettings)o;
        if (this.getCurrentInteractionMode()
                == settings.getCurrentInteractionMode()
                && arraysEqual(this.getAllowedInteractionModes(),
                               settings.getAllowedInteractionModes()))
        {
            return true;
        }
        return false;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\nSetting name:\"" + name + "\"");
        sb.append(" Settings current interaction mode:" + currentInteractionMode);
        sb.append(" Settings allowed interaction modes:");
        if (allowedInteractionModes != null)
        {
            for (int i=0; i<allowedInteractionModes.length; i++)
            {
                sb.append(allowedInteractionModes[i]);
                if (i < allowedInteractionModes.length - 1)
                {
                    sb.append(",");
                }
            }
        }
        else
        {
            sb.append("NULL");
        }
        return sb.toString();
    }

    private boolean arraysEqual(int[] array1, int[] array2)
    {
        if (array1 == null && array2 == null)
        {
            return true;
        }
        if ((array1 == null && array2 != null) ||
                (array1 != null && array2 == null) ||
                (array1.length != array2.length))
        {
            return false;
        }
        for (int i=0; i<array1.length; i++)
        {
            boolean found = false;
            for (int j=0; j<array2.length; j++)
            {
                if (array1[i] == array2[j])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
            return true;
        }
        return true;
    }

    public byte[] getBytes() throws IOException
    {
        ByteArrayOutputStream oStream = new ByteArrayOutputStream();
        oStream.write(name.length());
        oStream.write(name.getBytes());
        oStream.write(currentInteractionMode);
        oStream.write(allowedInteractionModes.length);
        for (int i=0; i<allowedInteractionModes.length; i++)
        {
            oStream.write(allowedInteractionModes[i]);
        }
        return oStream.toByteArray();
    }

    public static SecurityPolicyPermissionSettings getFromBytes(byte[] buf)
    {
        int nameLen = buf[SecurityPolicy.index];
        SecurityPolicy.index++;
        String name = new String(buf, SecurityPolicy.index, nameLen);
        SecurityPolicy.index += nameLen;
        int currentInteractionMode = buf[SecurityPolicy.index];
        SecurityPolicy.index++;
        int[] allowedInteractionModes = new int[buf[SecurityPolicy.index]];
        SecurityPolicy.index++;
        for (int i=0; i<allowedInteractionModes.length; i++)
        {
            allowedInteractionModes[i] = buf[SecurityPolicy.index];
            SecurityPolicy.index++;
        }
        return new SecurityPolicyPermissionSettings(name, currentInteractionMode, allowedInteractionModes);
    }

}
