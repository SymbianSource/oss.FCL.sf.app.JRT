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
* Description:  Handler for the lcdui java system properties
 *
*/

package com.nokia.mj.impl.properties.lcduib;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;

import java.util.Hashtable;

/**
 * This gets the available lcdui properties
 * using the names
 *   "com.nokia.canvas.net.indicator.location"
 *   "com.nokia.softkey1.label.location"
 *   "com.nokia.softkey2.label.location"
 *   "com.nokia.softkey3.label.location"
 *   "com.nokia.softkey1.label.anchor"
 *   "com.nokia.softkey2.label.anchor"
 *   "com.nokia.softkey3.label.anchor"
 *   "com.nokia.keyboard.type"
 *   "com.nokia.key.scancode"
 *   "com.nokia.key.modifier"
 * The properties are then got using com.nokia.mj.impl.properties.lcduib.LcduiPropertyInvoker
 * (see also javax.microedition.lcdui.LcduiPropertyInvoker)
 */


public final class DynamicPropertyHandler implements SystemPropertyProvider
{

    /*
     * Constants for identifying the asked property.
     */
    private static String COM_NOKIA_CANVAS_NET_INDICATOR_LOCATION = "com.nokia.canvas.net.indicator.location";
    private static String COM_NOKIA_SOFTKEY1_LABEL_LOCATION       = "com.nokia.softkey1.label.location";
    private static String COM_NOKIA_SOFTKEY2_LABEL_LOCATION       = "com.nokia.softkey2.label.location";
    private static String COM_NOKIA_SOFTKEY3_LABEL_LOCATION       = "com.nokia.softkey3.label.location";
    private static String COM_NOKIA_SOFTKEY1_LABEL_ANCHOR         = "com.nokia.softkey1.label.anchor";
    private static String COM_NOKIA_SOFTKEY2_LABEL_ANCHOR         = "com.nokia.softkey2.label.anchor";
    private static String COM_NOKIA_SOFTKEY3_LABEL_ANCHOR         = "com.nokia.softkey3.label.anchor";
    private static String COM_NOKIA_KEYBOARD_TYPE = "com.nokia.keyboard.type";
    private static String COM_NOKIA_KEY_SCANCODE = "com.nokia.key.scancode";
    private static String COM_NOKIA_KEY_MODIFIER = "com.nokia.key.modifier";

    /*
     * Constants used by property handler implementations.
     */
    public static final int CANVAS_NET_INDICATOR_LOCATION = 0;
    public static final int SOFTKEY1_LABEL_LOCATION = 1;
    public static final int SOFTKEY2_LABEL_LOCATION = 2;
    public static final int SOFTKEY3_LABEL_LOCATION = 3;
    public static final int SOFTKEY1_LABEL_ANCHOR = 4;
    public static final int SOFTKEY2_LABEL_ANCHOR = 5;
    public static final int SOFTKEY3_LABEL_ANCHOR = 6;
    public static final int KEYBOARD_TYPE = 7;
    public static final int KEY_SCANCODE = 8;
    public static final int KEY_MODIFIER = 9;

    private static Hashtable iPropertyKeys;

    //Initialize the Hashtables when this class is first loaded.
    static
    {
        iPropertyKeys = new Hashtable();
        iPropertyKeys.put(COM_NOKIA_CANVAS_NET_INDICATOR_LOCATION, new Integer(CANVAS_NET_INDICATOR_LOCATION));
        iPropertyKeys.put(COM_NOKIA_SOFTKEY1_LABEL_LOCATION, new Integer(SOFTKEY1_LABEL_LOCATION));
        iPropertyKeys.put(COM_NOKIA_SOFTKEY2_LABEL_LOCATION, new Integer(SOFTKEY2_LABEL_LOCATION));
        iPropertyKeys.put(COM_NOKIA_SOFTKEY3_LABEL_LOCATION, new Integer(SOFTKEY3_LABEL_LOCATION));
        iPropertyKeys.put(COM_NOKIA_SOFTKEY1_LABEL_ANCHOR, new Integer(SOFTKEY1_LABEL_ANCHOR));
        iPropertyKeys.put(COM_NOKIA_SOFTKEY2_LABEL_ANCHOR, new Integer(SOFTKEY2_LABEL_ANCHOR));
        iPropertyKeys.put(COM_NOKIA_SOFTKEY3_LABEL_ANCHOR, new Integer(SOFTKEY3_LABEL_ANCHOR));
        iPropertyKeys.put(COM_NOKIA_KEYBOARD_TYPE, new Integer(KEYBOARD_TYPE));
        iPropertyKeys.put(COM_NOKIA_KEY_SCANCODE, new Integer(KEY_SCANCODE));
        iPropertyKeys.put(COM_NOKIA_KEY_MODIFIER, new Integer(KEY_MODIFIER));
    }

    /* (non-Javadoc)
     * @see com.nokia.mj.impl.rt.support.SystemPropertyProvider#getProperty(java.lang.String)
     */
    public String getProperty(String propertyName)
    {
        String propertyValue = null;

        //Look for the property id from the Hashtable
        Integer propertyType = (Integer)iPropertyKeys.get(propertyName);

        if (propertyType != null)
        {
            int propertyTypeInt = propertyType.intValue();

            switch (propertyTypeInt)
            {
            case CANVAS_NET_INDICATOR_LOCATION:
                propertyValue = LcduiPropertyInvoker.getCanvasNetworkIndicatorLocation();
                break;
            case SOFTKEY1_LABEL_LOCATION:
                propertyValue = LcduiPropertyInvoker.getSoftKeyLabelLocation(1);
                break;
            case SOFTKEY2_LABEL_LOCATION:
                propertyValue = LcduiPropertyInvoker.getSoftKeyLabelLocation(2);
                break;
            case SOFTKEY3_LABEL_LOCATION:
                propertyValue = LcduiPropertyInvoker.getSoftKeyLabelLocation(3);
                break;
            case SOFTKEY1_LABEL_ANCHOR:
                propertyValue = LcduiPropertyInvoker.getSoftKeyLabelAnchor(1);
                break;
            case SOFTKEY2_LABEL_ANCHOR:
                propertyValue = LcduiPropertyInvoker.getSoftKeyLabelAnchor(2);
                break;
            case SOFTKEY3_LABEL_ANCHOR:
                propertyValue = LcduiPropertyInvoker.getSoftKeyLabelAnchor(3);
                break;
            case KEYBOARD_TYPE:
                propertyValue = LcduiPropertyInvoker.getKeyboardTypeName();
                break;
            case KEY_SCANCODE:
                propertyValue = LcduiPropertyInvoker.getKeyScanCode();
                break;
            case KEY_MODIFIER:
                propertyValue = LcduiPropertyInvoker.getKeyModifier();
                break;
            }
        }

        return propertyValue;
    }

    public boolean isStatic(String key)
    {
        // All the properties are not static
        return false;
    }
}

