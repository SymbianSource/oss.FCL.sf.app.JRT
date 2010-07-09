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

import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import java.io.Serializable;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.ByteArrayOutputStream;

/**
 * The security policy object defining a set of permissions with corresponding
 * allowed access levels
 */
public final class SecurityPolicy
        implements Serializable
{
    private transient SecurityPolicyPermission[] policyPermissions = null;
    private transient String protectionDomain = null;
    public transient static int index;

    public SecurityPolicy(String protectionDomain, SecurityPolicyPermission[] policyPermissions)
    {
        this.protectionDomain = protectionDomain;
        this.policyPermissions = policyPermissions;
    }

    public PolicyBasedPermission[] getPermissions()
    {
        return policyPermissions;
    }

    public String getProtectionDomain()
    {
        return protectionDomain;
    }

    public SecurityPolicy append(SecurityPolicy otherPolicy)
    {
        if (otherPolicy == null)
        {
            return new SecurityPolicy(protectionDomain, policyPermissions);
        }
        PolicyBasedPermission[] p2 = otherPolicy.getPermissions();
        SecurityPolicyPermission[] combinedPerms = null;
        if (policyPermissions == null)
        {
            if (p2 != null)
            {
                combinedPerms = new SecurityPolicyPermission[p2.length];
                for (int i=0; i<p2.length; i++)
                {
                    combinedPerms[i] = (SecurityPolicyPermission)p2[i];
                }
            }
        }
        else
        {
            if (p2 == null)
            {
                combinedPerms = policyPermissions;
            }
            else
            {
                int len = policyPermissions.length + p2.length;
                combinedPerms = new SecurityPolicyPermission[len];
                int i = 0;
                while (i<policyPermissions.length)
                {
                    combinedPerms[i] = policyPermissions[i];
                    i++;
                }
                while (i<len)
                {
                    combinedPerms[i] = (SecurityPolicyPermission)
                                       p2[i - policyPermissions.length];
                    i++;
                }
            }
        }
        return new SecurityPolicy(protectionDomain, combinedPerms);
    }

    private void writeObject(ObjectOutputStream out) throws IOException
    {
        ByteArrayOutputStream oStream = new ByteArrayOutputStream();
        oStream.write(protectionDomain.length());
        oStream.write(protectionDomain.getBytes());
        oStream.write(policyPermissions.length);
        for (int i=0; i<policyPermissions.length; i++)
        {
            oStream.write(policyPermissions[i].getBytes());
        }
        out.write(oStream.toByteArray());
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException
    {
        // read all the bytes into a buffer
        int MAX_CHUNK_SIZE = 1024;
        ByteArrayOutputStream oStream = new ByteArrayOutputStream();
        byte[] tmp = new byte[MAX_CHUNK_SIZE];
        int len = -1;
        while ((len = in.read(tmp, 0, MAX_CHUNK_SIZE)) > 0)
        {
            oStream.write(tmp, 0, len);
        }
        if (oStream.size() <=0)
        {
            // nothing to process
            return;
        }
        byte[] buf = oStream.toByteArray();
        // process the buffer
        index = 0;
        int domainLength = buf[index];
        index++;
        protectionDomain = new String(buf, index, domainLength);
        boolean activeSettings = true;
        index+= domainLength;
        int permissions = buf[index];
        index++;
        policyPermissions = new SecurityPolicyPermission[permissions];
        for (int i=0; i<permissions; i++)
        {
            policyPermissions[i] = SecurityPolicyPermission.getFromBytes(buf, activeSettings);
        }
    }
}
