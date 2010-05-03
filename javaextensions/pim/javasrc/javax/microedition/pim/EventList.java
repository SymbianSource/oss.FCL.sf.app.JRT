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
* Description:  Event list interface.
 *
*/


// PACKAGE
package javax.microedition.pim;

// IMPORTS
import java.util.Enumeration;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant PIMList interface.
 */
public interface EventList extends PIMList
{
    public static final int STARTING = 0;

    public static final int ENDING = 1;

    public static final int OCCURRING = 2;

    public Event createEvent();

    public Event importEvent(Event aItem);

    public void removeEvent(Event aItem) throws PIMException;

    public Enumeration items(int aSearchType, long aStartDate, long aEndDate,
                             boolean aInitialEventOnly) throws PIMException;

    public int[] getSupportedRepeatRuleFields(int aFrequency);
}