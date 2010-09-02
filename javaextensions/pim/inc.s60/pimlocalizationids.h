/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Localization data identifiers.
*
*/



#ifndef PIMLOCALIZATIONIDS_H
#define PIMLOCALIZATIONIDS_H

// INCLUDE FILES
#include "pimtypes.h"

/**
* @file
* This file contains constants that map the lists provided by adapters
* with label sets provided with the localization module. A constant may
* apply to one or several related lists.
*
* There are two types of IDs:
* @li Localization IDs
* @li List name IDs
* Both of them are of type TPIMLocalizationDataID.
*
* Localization IDs define a set of localized labels for fields, attributes
* and array elements and a default name for the list.
*
* List name IDs specify a name among a set of localized labels defined by
* a localization ID. The default list name can be thus overridden using
* a list name ID. A localization set may or may not define several list name
* IDs. If only single name is defined for a localization set, it may not be
* documented but it can be used using the localization operations that rely
* on the default list name id.
*
* Naming:
* @li Localization IDs are of form \c KPIMLocalizationIDXXX.
* @li List name IDs are of form \c KPIMListNameIdYYY.
*/

// CONSTANTS


//  Localization data IDs

/**
* Localization data identifier for Contacts Model contact list. The labels
* associated with this identifier apply only on that list.
*/
const TPIMLocalizationDataID KPIMLocalizationIdContacts = 1;

/**
* Localization data identifier for SIM contact list. The labels associated
* with this identifier apply only on that list.
*
* NOTE: Currently unused, defined for future purposes.
*/
const TPIMLocalizationDataID KPIMLocalizationIdSIM = 2;

/**
* Localization data identifier for all Agenda Event lists (Meeting, Memo and
* Anniversary). The labels are valid for any of those lists. Each of the lists
* uses only a subset of the labels associated with this identifier.
*/
const TPIMLocalizationDataID KPIMLocalizationIdEvent = 3;

/**
* Localization data identifier for Agenda to-do list. The labels associated
* with this identifier apply only on that list.
*/
const TPIMLocalizationDataID KPIMLocalizationIdToDo = 4;


//  List name IDs
//  NOTE! It is essential that these IDs are sequential and start from zero.

/**
* List name identifier for "Appointment" Agenda Event list.
* Applies with the localization data identifier \c KPIMLocalizationIdEvent.
*/
const TPIMLocalizationDataID KPIMListNameIdAppointment = 0;

/**
* List name identifier for "Event" Agenda Event list.
* Applies with the localization data identifier \c KPIMLocalizationIdEvent.
*/
const TPIMLocalizationDataID KPIMListNameIdEvent = 1;

/**
* List name identifier for "Anniversary" Agenda Event list.
* Applies with the localization data identifier \c KPIMLocalizationIdEvent.
*/
const TPIMLocalizationDataID KPIMListNameIdAnniversary = 2;


#endif // PIMLOCALIZATIONIDS_H


// End of File
