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

package com.nokia.mj.impl.security.midp.storage;

import java.util.Vector;
import com.nokia.mj.impl.security.midp.common.GeneralSecuritySettings;

public final class AuthenticationStorageData
{
    private String protectionDomain;
    private String protectionDomainCategory;
    private String jarHashValue;
    private String rootHashValue;
    private int[] validatedChainIndexes;
    private String jarPath;
    private int securityWarningsMode;

    public AuthenticationStorageData(String protectionDomain, String protectionDomainCategory, String jarHashValue, String rootHashValue, String jarPath)
    {
        this(protectionDomain,
             protectionDomainCategory,
             jarHashValue,
             rootHashValue,
             null /* validatedChainIndexes */,
             jarPath,
             GeneralSecuritySettings.UNINITIALIZED_SECURITY_MODE);
    }

    public AuthenticationStorageData(String protectionDomain, String protectionDomainCategory, String jarHashValue, String rootHashValue, Vector validatedChainIndexes)
    {
        this(protectionDomain,
             protectionDomainCategory,
             jarHashValue,
             rootHashValue,
             validatedChainIndexes,
             null /* jar path */,
             GeneralSecuritySettings.UNINITIALIZED_SECURITY_MODE);
    }

    public AuthenticationStorageData(String protectionDomain, String protectionDomainCategory, String jarHashValue, String rootHashValue, Vector validatedChainIndexes, int securityWarningsMode)
    {
        this(protectionDomain,
             protectionDomainCategory,
             jarHashValue,
             rootHashValue,
             validatedChainIndexes,
             null /* jar path */,
             securityWarningsMode);
    }

    public AuthenticationStorageData(String protectionDomain, String protectionDomainCategory, String jarHashValue, String rootHashValue, Vector validatedChainIndexes, String jarPath)
    {
        this(protectionDomain,
             protectionDomainCategory,
             jarHashValue,
             rootHashValue,
             validatedChainIndexes,
             jarPath,
             GeneralSecuritySettings.UNINITIALIZED_SECURITY_MODE);
    }

    public AuthenticationStorageData(String protectionDomain, String protectionDomainCategory, String jarHashValue, String rootHashValue, Vector validatedChainIndexes, String jarPath, int securityWarningsMode)
    {
        this.protectionDomain = protectionDomain;
        this.protectionDomainCategory = protectionDomainCategory;
        this.jarHashValue = jarHashValue;
        this.rootHashValue = rootHashValue;
        this.jarPath = jarPath;
        setValidatedChainIndexes(validatedChainIndexes);
        this.securityWarningsMode = securityWarningsMode;
    }


    public String getProtectionDomain()
    {
        return protectionDomain;
    }

    public String getProtectionDomainCategory()
    {
        return protectionDomainCategory;
    }

    public String getJarHashValue()
    {
        return jarHashValue;
    }

    public String getRootHashValue()
    {
        return rootHashValue;
    }

    public String getJarPath()
    {
        return jarPath;
    }

    public int getSecurityWarningsMode()
    {
        return securityWarningsMode;
    }

    public int[] getValidatedChainIndexes()
    {
        return validatedChainIndexes;
    }

    public void setJarHashValue(String jarHashValue)
    {
        this.jarHashValue = jarHashValue;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\nDomain name:" + protectionDomain);
        sb.append("\nDomain category:" +protectionDomainCategory);
        sb.append("\nJar hash:" + jarHashValue);
        sb.append("\nRoot hash:" + rootHashValue);
        if (validatedChainIndexes != null && validatedChainIndexes.length > 0)
        {
            sb.append("\nValidated chain indexes:");
            for (int i=0; i<validatedChainIndexes.length; i++)
            {
                sb.append(validatedChainIndexes[i]);
                if (i < validatedChainIndexes.length - 1)
                {
                    sb.append(",");
                }
            }
        }
        return sb.toString();
    }

    private void setValidatedChainIndexes(Vector validatedChainIndexes)
    {
        if (validatedChainIndexes != null && validatedChainIndexes.size() > 0)
        {
            // remove duplicates
            Vector vIndexes = new Vector();
            for (int i=0; i<validatedChainIndexes.size(); i++)
            {
                Integer index = (Integer)validatedChainIndexes.elementAt(i);
                if (!vIndexes.contains(index))
                {
                    vIndexes.addElement(index);
                }
            }
            this.validatedChainIndexes = new int[vIndexes.size()];
            for (int i=0; i<vIndexes.size(); i++)
            {
                this.validatedChainIndexes[i] = ((Integer)vIndexes
                                                 .elementAt(i)).intValue();
            }
        }
    }
}


