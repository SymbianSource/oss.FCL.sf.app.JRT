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
* Description:  Supported Event fields for different platforms
 *
*/


#ifndef AGNEVENTSUPPORT_H
#define AGNEVENTSUPPORT_H

#include "pimcommon.h"
#include "pimevent.h"

/** @file
 * This file contains the list of supported Event fields for different platforms.
 */

/**
 * Appointment fields.
 */
const TPIMField KPIMSupportedApptFields[] =
{
    EPIMEventSummary,
    EPIMEventNote,
    EPIMEventLocation,
    EPIMEventStart,
    EPIMEventEnd,
    EPIMEventAlarm,
    EPIMEventClass,
    EPIMEventUid,
    EPIMEventRevision
};

const TInt KPIMSupportedApptFieldsCount = sizeof(KPIMSupportedApptFields)
        / sizeof(TPIMField);

/**
 * Memo (Event) fields.
 */
const TPIMField KPIMSupportedMemoFields[] =
{
    EPIMEventSummary,
    EPIMEventNote,
    EPIMEventStart,
    EPIMEventEnd,
    EPIMEventAlarm,
    EPIMEventClass,
    EPIMEventUid,
    EPIMEventRevision
};

const TInt KPIMSupportedMemoFieldsCount = sizeof(KPIMSupportedMemoFields)
        / sizeof(TPIMField);

/**
 * Anniversary fields.
 */
const TPIMField KPIMSupportedAnnivFields[] =
{
    EPIMEventSummary,
    EPIMEventNote,
    EPIMEventStart,
    EPIMEventAlarm,
    EPIMEventClass,
    EPIMEventUid,
    EPIMEventRevision
};

const TInt KPIMSupportedAnnivFieldsCount = sizeof(KPIMSupportedAnnivFields)
        / sizeof(TPIMField);

const TInt KPIMAgnStringFieldMaxLength = 160;

#endif // AGNEVENTSUPPORT_H
