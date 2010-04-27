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

package com.nokia.mj.impl.properties.bluetooth;

import java.util.Hashtable;
import java.io.IOException;
import javax.bluetooth.BluetoothStateException;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.SystemPropertyProvider;

/**
 * Class defines the System Properties that need to be exposed by JSR-82.
 */
public final class BtDynamicPropertyHandler implements SystemPropertyProvider
{
    // Value as a string in Base10 Digits
    private static String BT_L2CAP_RECEIVEMTU_MAX = "bluetooth.l2cap.receiveMTU.max";

    private static String BT_CONNECTED_DEVICES_MAX = "bluetooth.connected.devices.max";

    // Following return True/False
    private static String BT_CONNECTED_INQUIRY = "bluetooth.connected.inquiry";

    private static String BT_CONNECTED_PAGE = "bluetooth.connected.page";

    private static String BT_CONNECTED_INQUIRY_SCAN = "bluetooth.connected.inquiry.scan";

    private static String BT_CONNECTED_PAGE_SCAN = "bluetooth.connected.page.scan";

    private static String BT_MASTER_SWITCH = "bluetooth.master.switch";

    // Value as a string in Base10 Digits
    private static String BT_SD_TRANS_MAX = "bluetooth.sd.trans.max";

    private static String BT_SD_ATTR_RETRIEVE_MAX = "bluetooth.sd.attr.retrievable.max";

    // Constants corresponding to the Property values
    private static final int L2CAP_RECEIVEMTU_MAX = 1;

    private static final int CONNECTED_DEVICES_MAX = 2;

    private static final int CONNECTED_INQUIRY = 3;

    private static final int CONNECTED_PAGE = 4;

    private static final int CONNECTED_INQUIRY_SCAN = 5;

    private static final int CONNECTED_PAGE_SCAN = 6;

    private static final int MASTER_SWITCH = 7;

    private static final int SD_TRANS_MAX = 8;

    private static final int SD_ATTR_RETRIEVE_MAX = 9;

    private static Hashtable iPropertyKeys;

    // Initialize the hashtables when the class is first loaded.
    static
    {
        iPropertyKeys = new Hashtable();
        iPropertyKeys.put(BT_L2CAP_RECEIVEMTU_MAX, new Integer(
                              L2CAP_RECEIVEMTU_MAX));
        iPropertyKeys.put(BT_CONNECTED_DEVICES_MAX, new Integer(
                              CONNECTED_DEVICES_MAX));
        iPropertyKeys.put(BT_CONNECTED_INQUIRY, new Integer(CONNECTED_INQUIRY));
        iPropertyKeys.put(BT_CONNECTED_PAGE, new Integer(CONNECTED_PAGE));
        iPropertyKeys.put(BT_CONNECTED_INQUIRY_SCAN, new Integer(
                              CONNECTED_INQUIRY_SCAN));
        iPropertyKeys.put(BT_CONNECTED_PAGE_SCAN, new Integer(
                              CONNECTED_PAGE_SCAN));
        iPropertyKeys.put(BT_MASTER_SWITCH, new Integer(MASTER_SWITCH));
        iPropertyKeys.put(BT_SD_TRANS_MAX, new Integer(SD_TRANS_MAX));
        iPropertyKeys.put(BT_SD_ATTR_RETRIEVE_MAX, new Integer(
                              SD_ATTR_RETRIEVE_MAX));
    }

    /**
     * Retrieves Bluetooth system properties.
     *
     * @param aKey
     *            The property to retrieve as defined in this class.
     * @return The value of the property specified; null if the property is not
     *         defined
     */
    public String getProperty(String aKey)
    {
        String propertyValue = null;
        String propertyName = aKey;

        Object property = iPropertyKeys.get(propertyName);
        if (null == property)
        {
            return null;
        }

        int propertyType = ((Integer) property).intValue();

        propertyValue = com.nokia.mj.impl.bluetooth.BTPlatformControl
                        ._getProperty(propertyType);

        return propertyValue;
    }

    public boolean isStatic(String aKey)
    {
        // If there is any property that can change during application's
        // runtime,
        // then we return false in that case.
        return true;
    }

    // Native methods for BTDynamicPropertyHandler
}