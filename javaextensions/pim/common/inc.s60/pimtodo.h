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
* Description:  ToDo types and constants
 *
*/


#ifndef PIMTODO_H
#define PIMTODO_H

/** @file
 * This file contains the PIM API constants and definitions for ToDos.
 */

/**
 * ToDo field constants.
 */
enum TPIMToDoField
{
    EPIMToDoClass = 100,
    /**< Integer, values from \ref TPIMToDoClassValue. */
    EPIMToDoCompleted = 101,
    /**< Boolean. */
    EPIMToDoCompletionDate = 102,
    /**< Date. */
    EPIMToDoDue = 103,
    /**< Date. */
    EPIMToDoNote = 104,
    /**< String. */
    EPIMToDoPriority = 105,
    /**< Integer, values from [0,9]. */
    EPIMToDoRevision = 106,
    /**< Date. */
    EPIMToDoSummary = 107,
    /**< String. */
    EPIMToDoUid = 108,
    /**< String, read-only when persisted (converted from the Item ID). */
    EPIMToDoExtAlarm = 0x1005001
    /**< Extended field "Alarm" */
};

/**
 * ToDo \ref EPIMToDoClass field values.
 * [From javax.microedition.pim.ToDo]
 */
enum TPIMToDoClassValue
{
    EPIMToDoClassConfidential = 200,
    EPIMToDoClassPrivate = 201,
    EPIMToDoClassPublic = 202
};

/**
 * ToDo priority constants for native ToDos.
 */
// Based on empirical testing
enum TPIMToDoNativePriority
{
    EPIMToDoNativePriorityHigh = 1,
    EPIMToDoNativePriorityMedium = 2,
    EPIMToDoNativePriorityLow = 3
};

/**
 * ToDo priority constants for PIM API ToDos.
 */
enum TPIMToDoPriority
{
    EPIMToDoPriorityMinValue = 0,
    EPIMToDoPriorityHigh = 1,
    EPIMToDoPriorityMedium = 4,
    EPIMToDoPriorityLow = 7,
    EPIMToDoPriorityMaxValue = 9
};

#endif // PIMTODO_H
// End of file
