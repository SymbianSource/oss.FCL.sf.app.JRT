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
* Description:  Interface for APNInfo implementation
 *
*/


package com.nokia.mid.iapinfo;

/**
 * Please refer IapInfo Spec.
 */
public abstract class IAPInfo
{

    IAPInfo()
    {
    }

    private static IAPInfo instance = null;

    /**
     * Please refer IapInfo Spec.
     */

    public synchronized static IAPInfo getIAPInfo() throws IAPInfoException
    {
        if (instance == null)
            instance = new IAPInfoImpl();
        return instance;
    }

    /**
     * Please refer IapInfo Spec.
     */

    abstract public AccessPoint[] getAccessPoints() throws IAPInfoException;

    /**
     * Please refer IapInfo Spec.
     */

    abstract public AccessPoint getAccessPoint(int aID) throws IAPInfoException;

    /**
     * Please refer IapInfo Spec.
     */

    abstract public AccessPoint getAccessPoint(String aName)
    throws IAPInfoException;

    /**
     * Please refer IapInfo Spec.
     */

    abstract public AccessPoint getLastUsedAccessPoint()
    throws IAPInfoException;

    /**
     * Please refer IapInfo Spec.
     */

    abstract public DestinationNetwork[] getDestinationNetworks()
    throws IAPInfoException;

    /**
     * Please refer IapInfo Spec.
     */

    abstract public DestinationNetwork getDestinationNetwork(int aID)
    throws IAPInfoException;

    /**
     * Please refer IapInfo Spec.
     */

    abstract public DestinationNetwork getDestinationNetwork(String aName)
    throws IAPInfoException;

    /**
     * Please refer IapInfo Spec.
     */

    abstract public AccessPoint[] getConnectionPreferences()
    throws IAPInfoException;

}
