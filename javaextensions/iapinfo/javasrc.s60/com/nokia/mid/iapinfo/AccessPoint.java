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
* Description:  Class to store all access point related  information
 *
*/


package com.nokia.mid.iapinfo;

/**
 * Please refer to IapInfo Spec.
 */
public class AccessPoint
{

    // ID of the access point
    private int iAccessPointID;

    // Name of the access point
    private String iAccessPointName = null;

    // Type of the access point
    private String iAccessPointBearerType = null;
    private String iAccessPointServiceType = "";

    /** Parameter nokia_apnid string */
    static final String PARAM_NOKIA_APNID = "nokia_apnid";

    /**
     * Constructor for class com.nokia.mid.iapinfo.AccessPoint, which creates a
     * new AccessPoint object with the received parameters.
     *
     * @param aAccessPointID the ID of the AccessPoint
     * @param aAccessPointName the name of the AccessPoint
     * @param aAccessPointBearerType the bearer type of the AccessPoint
     * @param aAccessPointServiceType the service type of the AccessPoint
     */
    AccessPoint(int aAccessPointID, String aAccessPointName,
                String aAccessPointBearerType, String aAccessPointServiceType)
    {
        iAccessPointID = aAccessPointID;
        iAccessPointName = aAccessPointName;
        iAccessPointBearerType = aAccessPointBearerType;
        iAccessPointServiceType = aAccessPointServiceType;
    }

    /**
     * Please refer to IapInfo Spec.
     */
    public String getServiceType()
    {
        return iAccessPointServiceType;
    }

    /**
     * Please refer to IapInfo Spec.
     */
    public String getBearerType()
    {
        return iAccessPointBearerType;
    }

    /**
     * Please refer to IapInfo Spec.
     */
    public int getID()
    {
        return iAccessPointID;
    }

    /**
     * Please refer to IapInfo Spec.
     */
    public String getName()
    {
        return iAccessPointName;
    }

    /**
     * Please refer to IapInfo Spec.
     */
    public String getURL(String aURL)
    {
        return aURL + ";" + PARAM_NOKIA_APNID + "=" + iAccessPointID;
    }
}
