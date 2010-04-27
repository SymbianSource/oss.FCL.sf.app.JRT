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

/**
 * Specifies the MCC (3 digits) and MNC (2 or 3 digits) of a network
 */
public class NetworkRestriction
{
    /**
     * The MCC
     */
    protected String MCC;

    /**
     * The MNC
     */
    protected String MNC;

    public NetworkRestriction(String MCC, String MNC)
    {
        this.MCC = MCC;
        this.MNC = MNC;
    }

    public String getMCC()
    {
        return MCC;
    }

    public String getMNC()
    {
        return MNC;
    }

    public boolean equals(Object o)
    {
        if (this == o)
        {
            return true;
        }
        if (!(o instanceof NetworkRestriction))
        {
            return false;
        }
        NetworkRestriction r = (NetworkRestriction)o;
        if ((this.MCC == null && r.getMCC() != null)
                || (this.MCC != null && !this.MCC.equals(r.getMCC()))
                || (this.MNC == null && r.getMNC() != null)
                || (this.MNC != null && !this.MNC.equals(r.getMNC())))
        {
            return false;
        }
        return true;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\n MCC: " + MCC);
        sb.append("\n MNC: " + MNC);
        return sb.toString();
    }
}
