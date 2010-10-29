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
import org.w3c.dom.svg.SVGElement;

/**
 * The Event interface is used to provide contextual information about an event to the handler
 * processing the event. An object which implements the Event interface is passed as the first
 * parameter to the {@link org.w3c.dom.events.EventListener#handleEvent handleEvent} call.
 */
public class M2GEvent implements Event
{
    public static final boolean EVENT_TARGET = false;
    public static final boolean WRAPPED_EVENT_TARGET = true;
    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private String iEventType;
    private EventTarget iTarget;
    private boolean iWrappedTarget;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     * @param aEventType Type
     * @param aTarget Target
     * @param aWrappedTarget If WRAPPED_EVENT_TARGET then <code>getCurrentTarget</code>
     * method returns a wrapped event target (which base class is NOT <code>Node</code>.
     * Otherwise normal event target (which base class is <code>Node</code>) is returned.
     */
    public M2GEvent(String aEventType, EventTarget aTarget, boolean aWrappedTarget)
    {
        iEventType = aEventType;
        iTarget = aTarget;
        iWrappedTarget = aWrappedTarget;
    }

    /**
     * @see org.w3c.dom.events.Event#getCurrentTarget()
     * @see M2GEvent()
     */
    public EventTarget getCurrentTarget()
    {
        if (iWrappedTarget)
        {
            if ((iTarget != null) && (iTarget instanceof M2GSVGElement))
            {
                return new M2GSVGElementWrapper((M2GSVGElement)iTarget);
            }
            return null;
        }
        return iTarget;
    }

    /**
     * @see org.w3c.dom.events.Event#getType()
     */
    public String getType()
    {
        return iEventType;
    }
}
