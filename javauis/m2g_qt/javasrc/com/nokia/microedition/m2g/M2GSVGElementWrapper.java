/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g;

import org.w3c.dom.events.*;
import org.w3c.dom.DOMException;
import com.nokia.mj.impl.utils.Logger;

public class M2GSVGElementWrapper extends M2GObject implements EventTarget
{
    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private M2GSVGElement iElement;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     * @param aElement Element to be wrapped
     */
    M2GSVGElementWrapper(M2GSVGElement aElement)
    {
        super();
        iElement = aElement;
    }

    /**
     * Cleanup operations.
     */
    void doCleanup()
    {
    }

    /**
     * Construct operations.
     */
    void doConstruct()
    {
        if (iElement != null)
        {
            setHandle(iElement.getHandle());
        }
    }

    /**
     * @see org.w3c.dom.events.EventTarget#addEventListener()
     */
    public void addEventListener(String type, EventListener listener, boolean useCapture)
    {
        if (type == null || type.equals(""))
        {
            throw new NullPointerException(M2GSVGElement.TYPE_IS_NULL_ESTR);
        }
        if (listener == null)
        {
            throw new NullPointerException(M2GSVGElement.LISTENER_IS_NULL_ESTR);
        }
        if (useCapture == true)
        {
            throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
                                   M2GSVGElement.CAPTURE_NOT_SUPPORTED_ESTR);
        }
        try
        {
            iElement.getDocument().register(iElement, type, listener);
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaUI, "addEventListener() exception:" + e.toString());
        }
    }

    /**
     * Get wrapped target element
     * @return Event target
     */
    public EventTarget getWrappedTarget()
    {
        return iElement;
    }

    /**
     * @see org.w3c.dom.events.EventTarget#removeEventListener()
     */
    public void removeEventListener(String type, EventListener listener, boolean useCapture)
    {
        if (type == null || type.equals(""))
        {
            throw new NullPointerException(M2GSVGElement.TYPE_IS_NULL_ESTR);
        }
        if (listener == null)
        {
            throw new NullPointerException(M2GSVGElement.LISTENER_IS_NULL_ESTR);
        }
        if (useCapture == true)
        {
            throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
                                   M2GSVGElement.CAPTURE_NOT_SUPPORTED_ESTR);
        }
        try
        {
            iElement.getDocument().unregister(iElement, type, listener);
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaUI, "removeEventListener() exception:" + e.toString());
        }
    }
}
