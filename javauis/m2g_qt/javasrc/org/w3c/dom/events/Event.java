/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package org.w3c.dom.events;

import java.lang.String;

/**
 * The Event interface is used to provide contextual information about an event to the handler
 * processing the event. An object which implements the Event interface is passed as the first
 * parameter to the {@link org.w3c.dom.events.EventListener#handleEvent handleEvent} call. If
 * an event target is an element instance (see <a href="http://www.w3.org/TR/SVG11/struct.html#InterfaceSVGElementInstance">SVGElementInstance</a>), the currentTarget is an implementation of EventTarget
 * that does not implement the Node interface.
 */

public interface Event
{
    /**
     * This method is used to get the current target of this event.
     * In SVG Tiny, this is always an object to which event listener was attached.
     *
     * @return the event's <code>EventTarget</code>.
     */
    public EventTarget getCurrentTarget();

    /**
     * This method returns the event type information. The name of the event is case-sensitive.
     * The following event types are supported:
     * <br>
     * <b>click</b>, <b>DOMActivate</b>, <b>DOMFocusIn</b>, <b>DOMFocusOut</b> (defined DOM Events);
     * @return the event's type.
     */
    public String getType();


}
