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

package com.nokia.mj.impl.security.midp.common;

import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * Class which bound together certificate info with protection
 * domain info.
 */
public final class ProtectionDomain
{
    private String name;
    private String category;

    public ProtectionDomain(String name,
                            String category)
    {
        this.name = name;
        this.category = category;
    }

    public static ProtectionDomain getManufacturerDomain()
    {
        return new ProtectionDomain("Manufacturer", ApplicationInfo.MANUFACTURER_DOMAIN);
    }

    public static ProtectionDomain getOperatorDomain()
    {
        return new ProtectionDomain("Operator", ApplicationInfo.OPERATOR_DOMAIN);
    }

    public static ProtectionDomain getIdentifiedThirdPartyDomain()
    {
        return new ProtectionDomain("IdentifiedThirdParty", ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN);
    }

    public static ProtectionDomain getUnidentifiedThirdPartyDomain()
    {
        return new ProtectionDomain("UnidentifiedThirdParty", ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN);
    }

    /**
     * Getter for the name of the protection domain
     *
     * @return The name of protection domain
     */
    public String getName()
    {
        return name;
    }

    /**
     * Getter for the category of the protection domain
     *
     * @return One of the following constants:
     *         com.nokia.mj.impl.rt.support.ApplicationInfo.MANUFACTURER_DOMAIN
     *         com.nokia.mj.impl.rt.support.ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN
     *         com.nokia.mj.impl.rt.support.ApplicationInfo.OPERATOR_DOMAIN
     *         com.nokia.mj.impl.rt.support.ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN
     */
    public String getCategory()
    {
        return category;
    }

    public boolean equals(Object o)
    {
        if (o instanceof ProtectionDomain)
        {
            ProtectionDomain otherDomain = (ProtectionDomain)o;
            if (((this.name != null
                    && this.name.equals(
                        otherDomain.getName()))
                    || (this.name == null
                        && otherDomain.getName() == null))
                    && ((this.category != null
                         && this.category.equals(
                             otherDomain.getCategory()))
                        || (this.category == null
                            && otherDomain.getCategory() == null)))
            {
                return true;
            }
        }
        return false;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("ProtectionDomain name: ").append(getName())
        .append(", category: ").append(getCategory());
        return sb.toString();
    }
}
