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

/**
* This interface represents an event listener, and is a subset of the EventListener interface
* defined in the <a href="http://www.w3.org/TR/2000/REC-DOM-Level-2-Events-20001113/idl-definitions.html">
* DOM Level 2 Event model</a>.
* <p>
* This interface must be implemented and registered on an EventTarget using the
* {@link org.w3c.dom.events.EventTarget#addEventListener addEventListener} method to be notified
* about events that occur on or bubble through the event target.
*
*
*/

public interface EventListener
{

    /**
     * This method is called whenever an event occurs of the type for which the
     * EventListener interface was registered.. The Event object contains the
     * necessary information pertaining to the event, such as its
     * target and type.
     *
     * @param evt the Event object containing necessary event information.
     */
    public void handleEvent(Event evt);

}
