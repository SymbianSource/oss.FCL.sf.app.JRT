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
* Description:  Class to store destination network related information
 *
*/


package com.nokia.mid.iapinfo;

import java.util.Vector;

/**
 * Please refer IapInfo Spec.
 */
public class DestinationNetwork
{

    // variable to store the ID of the destination network
    private int iDestinationNetworkID;

    // variable to store the name of the destination network
    private String iDestinationNetworkName = null;

    /*
     * variable to store the access points ID associated with this network
     * destination
     */
    private Vector iAccessPointsID = null;

    /** Parameter nokia_netid string */
    static final String PARAM_NOKIA_NETID = "nokia_netid";

    /**
     * Constructor for class com.nokia.mid.iapinfo.DestinationNetwork, which
     * creates a new DestinationNetwork object with the passed parameters.
     *
     * @param aDestinationNetworkID the ID of the destination network
     * @param aDestinationNetworkName the name of the destination network
     * @param aAccessPoints the included access points
     */
    DestinationNetwork(int aDestinationNetworkID,
                       String aDestinationNetworkName, int[] aAccessPoints)
    {
        this.iDestinationNetworkID = aDestinationNetworkID;
        this.iDestinationNetworkName = aDestinationNetworkName;
        this.iAccessPointsID = new Vector();
        for (int i = 0; i < aAccessPoints.length; i++)
        {
            this.iAccessPointsID.addElement(new Integer(aAccessPoints[i]));
        }
    }

    /**
     * Please refer IapInfo Spec.
     */
    public AccessPoint[] getAccessPoints() throws IAPInfoException
    {
        AccessPoint[] _array = new AccessPoint[this.iAccessPointsID.size()];
        IAPInfo iap = IAPInfo.getIAPInfo();
        for (int i = 0; i < _array.length; i++)
        {
            _array[i] = iap.getAccessPoint(((Integer) this.iAccessPointsID
                                            .elementAt(i)).intValue());
        }
        return _array;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public int getID()
    {
        return iDestinationNetworkID;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public String getURL(String aURL)
    {
        return aURL + ";" + PARAM_NOKIA_NETID + "=" + iDestinationNetworkID;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public String getName()
    {
        return iDestinationNetworkName;
    }
}
