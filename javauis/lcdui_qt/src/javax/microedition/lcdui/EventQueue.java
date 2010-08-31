/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package javax.microedition.lcdui;

import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;

final class EventQueue
{

    LCDUIEvent first;
    LCDUIEvent last;

    EventQueue()
    {
    }

    synchronized void push(LCDUIEvent event)
    {
        event.next = null;
        if(isEmpty())
        {
            first = last = event;
        }
        else
        {
            last.next = event;
            last = event;
        }
    }

    synchronized LCDUIEvent pop()
    {
        if(isEmpty())
        {
            return null;
        }
        LCDUIEvent event = first;
        first = first.next;
        event.next = null;
        if(isEmpty())
        {
            last = null;
        }
        return event;
    }

    synchronized boolean isEmpty()
    {
        return first == null;
    }
}
