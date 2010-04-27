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

import org.w3c.dom.DOMException;

/**
*
* This interface represents an event target, and is a subset of the EventTarget interface
* defined in the <a href="http://www.w3.org/TR/2000/REC-DOM-Level-2-Events-20001113/idl-definitions.html">
* DOM Level 2 Event model</a>.
* <br>
* This interface is implemented by an object (SVGElements) that can notify listeners about events and allows
* registration and removal of {@link org.w3c.dom.events.EventListener EventListener} objects.
*
*
*/

public interface EventTarget
{

    /**
     * This method registers the specified listener with the event target. If an EventListener is added to an EventTarget
     * while it is processing an event, it will not be triggered by the current actions. If multiple identical EventListeners
     * are registered on the same EventTarget with the same parameters the duplicate instances are discarded. They do not cause
     * the EventListener to be called twice and since they are discarded they do not need to be removed with the removeEventListener
     * method.
     *
     * @param type The type of event to listen to.
     * @param listener Will be notified when an event of the desired type happens on this target or one of its descendant.
     * @param useCapture If true, the listener will be called during the event flow capture phase. Otherwise, the listener
     * will be called during the bubble phase. If the event's target is this target, then the listener will be called during the 'at target' phase of event flow.
     *
     * @throws DOMException with error code NOT_SUPPORTED_ERR if useCapture is true since capture phase is not supported in SVG Tiny.
     * @throws NullPointerException if <code>listener</code> is null.
     * @throws NullPointerException if <code>type</code> is null.
     */
    public void addEventListener(String type, EventListener listener, boolean useCapture);

    /**
     * This method removes the specified listener from the event target. If an EventListener is removed
     * from an EventTarget while it is processing an event, it will not be triggered by the current actions. Calling removeEventListener
     * with arguments which do not identify any currently registered EventListener on the EventTarget has no effect.
     *
     * @param type The type of event that was listened to.
     * @param listener The listener that was previously registered.
     * @param useCapture If true, the listener was listening to events in the capture phase of event flow, otherwise the listener
     * was listening to events in the bubble phase.
     *
     * @throws DOMException with error code NOT_SUPPORTED_ERR if useCapture is true since capture phase is not supported in SVG Tiny.
     * @throws NullPointerException if <code>listener</code> is null.
     * @throws NullPointerException if <code>type</code> is null.
     */
    public void removeEventListener(String type, EventListener listener, boolean useCapture);

}