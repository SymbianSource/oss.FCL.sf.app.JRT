/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: RmsName
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import com.nokia.mj.impl.rt.support.*;

/**
 * RmsName is helper class for record store name, vendor and suite.
 * This simplifies record store name handling in various places as
 * sometimes simple name is enough but sometimes full name containing
 * vendor and suite information is needed.
 */
public class RmsName
{
    private String iName;
    private String iVendorName;
    private String iSuiteName;

    public RmsName(String aName)
    {
        iName = aName;
        iVendorName = ApplicationInfo.getInstance().getVendor();
        iSuiteName = ApplicationInfo.getInstance().getSuiteName();
    }

    public RmsName(String aName, String aVendorName, String aSuiteName)
    {
        iName = aName;
        iVendorName = aVendorName;
        iSuiteName = aSuiteName;
    }

    public String getName()
    {
        return iName;
    }

    public String getVendorName()
    {
        return iVendorName;
    }

    public String getSuiteName()
    {
        return iSuiteName;
    }

    public boolean isHomeSuite()
    {
        if (getVendorName().equals(ApplicationInfo.getInstance().getVendor()) &&
                getSuiteName().equals(ApplicationInfo.getInstance().getSuiteName()))
        {
            return true;
        }
        return false;
    }

    public boolean equals(RmsName aName)
    {
        if (getName().equals(aName.getName()) &&
                getVendorName().equals(aName.getVendorName()) &&
                getSuiteName().equals(aName.getSuiteName()))
        {
            return true;
        }
        return false;
    }

    public String toString()
    {
        return getName() + " " + getVendorName() + " " + getSuiteName();
    }

}

