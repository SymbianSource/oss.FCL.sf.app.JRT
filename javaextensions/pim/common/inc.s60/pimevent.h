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
* Description:  Event types and constants
 *
*/


#ifndef PIMEVENT_H
#define PIMEVENT_H

/** @file
 * This file contains the PIM API constants and definitions for Events.
 */

/**
 * Event search type.
 * [From javax.microedition.pim.EventList]
 */
enum TPIMEventSearchType
{
    EPIMEventSearchStarting = 0,
    EPIMEventSearchEnding = 1,
    EPIMEventSearchOccurring = 2
};

/**
 * Event list type constants
 */
enum TPIMEventListType
{
    EPIMEventListAppointment = 0,
    EPIMEventListEvent = 1,
    EPIMEventListAnniversary = 2
};

/**
 * Event field constants.
 */
enum TPIMEventField
{
    EPIMEventAlarm = 100,
    /**< Integer, seconds. */

    EPIMEventClass = 101,
    /**< Integer, values from \ref TPIMEventClassValue. */

    EPIMEventEnd = 102,
    /**< Date. */

    EPIMEventLocation = 103,
    /**< String. */

    EPIMEventNote = 104,
    /**< String. */

    EPIMEventRevision = 105,
    /**< Date, read-only when persisted (the time of last modification). */

    EPIMEventStart = 106,
    /**< Date. */

    EPIMEventSummary = 107,
    /**< String. */

    EPIMEventUid = 108
    /**< String, read-only when persisted (converted from the Item ID). */
};

/**
 * Event \ref EPIMEventClass field values.
 * [From javax.microedition.pim.Event]
 */
enum TPIMEventClassValue
{
    EPIMEventClassConfidential = 200,
    EPIMEventClassPrivate = 201,
    EPIMEventClassPublic = 202
};

#endif // PIMEVENT_H
