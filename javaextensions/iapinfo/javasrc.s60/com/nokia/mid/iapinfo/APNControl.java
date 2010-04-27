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
* Description:  Implements Java APNControl related functions
 *
*/

package com.nokia.mid.iapinfo;

import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;

/**
 * Implements Java APNControl related functions. Currently the only featue is
 * the retrieval of the last used IAP ID.
 *
 */
class APNControl
{

    /**
     * Retrieves the last used IAP ID. Typically this function is used when in
     * the socket connection URI the NETId parameter is used. (see Java
     * APNControl documentation)
     *
     * @return IAP ID used for the last network connection open
     */
    public int getLastIAP()
    {
        ConnectionManager cminstance = ConnectionManager.getInstance();
        if (cminstance != null)
        {
            AccessPoint  lastUsedApn = cminstance.iSessionNetworkAccessPoint;
            if (lastUsedApn != null)
            {
                if (lastUsedApn.getType() == AccessPoint.NAP_IAP) // IAP ID
                {
                    return  lastUsedApn.getNapId();
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
        //return _getLastIAP();
    }

    /*----------------------------------------------------------------*/
    /* Native funcions */
    /*----------------------------------------------------------------*/
    /*
     * Retrieves the last used IAP ID.
     *
     * @return the IAP ID used for the last network connection open
     */
    private static native int _getLastIAP();
}
