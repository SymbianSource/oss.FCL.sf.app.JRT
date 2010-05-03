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
* Description:  The interface to reach the values from lcdui package
 *
*/



package com.nokia.mj.impl.properties.lcduib;

/*
 * This interface is implemented in
 * javax.microedition.lcdui.LcduiPropertyInvoker
 */
public abstract class LcduiPropertyInvoker
{
    // Statically created instace
    private static com.nokia.mj.impl.properties.lcduib.LcduiPropertyInvoker sInvoker;

    public static void setInvoker(com.nokia.mj.impl.properties.lcduib.LcduiPropertyInvoker invoker)
    {
        sInvoker = invoker;
    }

    public static String getCanvasNetworkIndicatorLocation()
    {
        // Use the static instance to get a value
        return sInvoker.getCanvasNetworkIndicatorLocationImpl();
    }

    public static String getSoftKeyLabelLocation(int softKeyId)
    {
        // Use the static instance to get a value
        return sInvoker.getSoftKeyLabelLocationImpl(softKeyId);
    }

    public static String getSoftKeyLabelAnchor(int softKeyId)
    {
        // Use the static instance to get a value
        return sInvoker.getSoftKeyLabelAnchorImpl(softKeyId);
    }

    public static String getKeyboardTypeName()
    {
        // Use the static instance to get a value
        return sInvoker.getKeyboardTypeNameImpl();
    }

    public static String getKeyScanCode()
    {
        // Use the static instance to get a value
        return sInvoker.getKeyScanCodeImpl();
    }

    public static String getKeyModifier()
    {
        // Use the static instance to get a value
        return sInvoker.getKeyModifierImpl();
    }

    // The method gets a property of network indicator location on fullscreen Canvas
    protected abstract String getCanvasNetworkIndicatorLocationImpl();
    // The method gets a property of a given softkey label location on Displayable which supports it
    protected abstract String getSoftKeyLabelLocationImpl(int softKeyId);
    // The method gets a property of a given softkey label anchor on Displayable which supports it
    protected abstract String getSoftKeyLabelAnchorImpl(int softKeyId);
    // The method gets a property of actual keyboard layout
    protected abstract String getKeyboardTypeNameImpl();
    // The method gets a property of the scan code of the latest key event
    protected abstract String getKeyScanCodeImpl();
    // The method gets a property of actual modifier keys state
    protected abstract String getKeyModifierImpl();
}
