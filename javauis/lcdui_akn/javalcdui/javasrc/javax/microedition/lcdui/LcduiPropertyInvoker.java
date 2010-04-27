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
* Description:  The implementation of invoker to reach the lcdui package
 *
*/


package javax.microedition.lcdui;

final class LcduiPropertyInvoker
        extends com.nokia.mj.impl.properties.lcduib.LcduiPropertyInvoker
{

    /*
         * Static instance creator
     * This should be called from the static constructor within class Toolkit
     */
    static void createInvoker()
    {
        LcduiPropertyInvoker.setInvoker(
            new javax.microedition.lcdui.LcduiPropertyInvoker());
    }

    /*
     * This method checks if the current Displayable is instance of Canvas
     * If yes, then the Canvas is asked for the property
     * If no, the null is returned
     */
    protected String getCanvasNetworkIndicatorLocationImpl()
    {
        Toolkit toolkit = Toolkit.getToolkit();
        Display display = toolkit.getDisplay();
        String locationString = null;
        synchronized (toolkit)
        {
            Displayable disp = display.getCurrent();

            if (disp instanceof Canvas)
            {
                locationString = ((Canvas)disp).getNetworkIndicatorLocation();
            }
        }
        return locationString;
    }

    /*
    * This method checks if the current Displayable exists
    * If yes, then the current Displayable is asked for the property
    * If no, the null is returned
    */
    protected String getSoftKeyLabelLocationImpl(int softKeyId)
    {
        Toolkit toolkit = Toolkit.getToolkit();
        Display display = toolkit.getDisplay();
        String SoftKeyLabelLocation = null;
        synchronized (toolkit)
        {
            Displayable disp = display.getCurrent();

            if (disp != null)
            {
                SoftKeyLabelLocation = disp.getSoftKeyLabelLocation(softKeyId);
            }
        }
        return SoftKeyLabelLocation;
    }

    /*
    * This method checks if the current Displayable exists
    * If yes, then the current Displayable is asked for the property
    * If no, the null is returned
    */
    protected String getSoftKeyLabelAnchorImpl(int softKeyId)
    {
        Toolkit toolkit = Toolkit.getToolkit();
        Display display = toolkit.getDisplay();
        String SoftKeyLabelAnchor = null;
        synchronized (toolkit)
        {
            Displayable disp = display.getCurrent();

            if (disp != null)
            {
                SoftKeyLabelAnchor = disp.getSoftKeyLabelAnchor(softKeyId);
            }
        }
        return SoftKeyLabelAnchor;
    }

    /*
     * Method returns String value of actual keyboard layout
     */
    protected String getKeyboardTypeNameImpl()
    {
        String returnValue = null;
        returnValue = Displayable.getKeyboardTypeName();
        return returnValue;
    }

    /*
     * Method returns String value of the scan code of the latest key event
     */
    protected String getKeyScanCodeImpl()
    {
        String returnValue=null;
        returnValue = Displayable.getKeyScanCode();
        return returnValue;
    }
    /*
     * Method returns String value of the modifier key state
     */
    protected String getKeyModifierImpl()
    {
        String returnValue=null;
        returnValue = Displayable.getKeyModifier();
        return returnValue;
    }
}
