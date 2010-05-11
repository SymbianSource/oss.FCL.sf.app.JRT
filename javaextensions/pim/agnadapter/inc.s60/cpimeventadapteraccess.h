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
* Description:  Provides access to Agenda Model event adapters
 *
*/


#ifndef CPIMEVENTADAPTERACCESS_H
#define CPIMEVENTADAPTERACCESS_H

// INCLUDES
#include "mpimadapteraccess.h"
#include "functionserver.h"

// CONSTANTS

/**
 * Points to the Calendar in the Agendar Server data cage on C: drive
 * (Platform Security).
 */
_LIT(KPIMAgendaFileName, "c:Calendar");

const TInt KPIMNumberOfEventLists = 3; // 3 lists are: Appt, Anniv and Event

// FORWARD DECLARATIONS
class MPIMLocalizationManager;

// CLASS DECLARATION
/**
 * Provides the elementary access the PIM Contact list provided by this
 * Contacts Model adapter module
 * Access to the adapter is through the static NewL( ) method.
 */
NONSHARABLE_CLASS(CPIMEventAdapterAccess): public CBase,
        public MPIMAdapterAccess
{
public: // Constructors and a destructor

    /**
     * Two-phased constructor.
     *
     * @param aLocalizationManager Localization Manager.
     */
    static CPIMEventAdapterAccess* NewL(
        MPIMLocalizationManager* aLocalizationManager,
        java::util::FunctionServer* aFuncServer);  // not owned

    /**
     * destructor
     */
    ~CPIMEventAdapterAccess();

public: // MPIMAdapterAccess

    const CDesCArray& ListNamesL(
        const TPIMListType aListType);

    TBool OpenContactListL(
        const TDesC* aListName,
        MPIMContactAdapterManager** aRetAdapterManager,
        MPIMContactListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData);

    TBool OpenEventListL(
        CCalSession* aCalSession,

        const TDesC* aListName, 
        MPIMEventAdapterManager** aRetAdapterManager,
        MPIMEventListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData);

    TBool OpenToDoListL(
        CCalSession* aCalSession,

        const TDesC* aListName, 
        MPIMToDoAdapterManager** aRetAdapterManager,
        MPIMToDoListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData);

protected: // Constructors

    /**
     * C++ default constructor.
     */
    CPIMEventAdapterAccess(
        MPIMLocalizationManager* aLocalizationManager,
        java::util::FunctionServer* aFuncServer);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

protected: // Data

    /** Owned. */
    CDesCArray* iEventListNames;

    /**
     * An empty array of list names (Contact and ToDo List names).
     * Owned.
     */
    CDesCArray* iNoListNames;

    /** Appointment Event localization data, owned. */
    MPIMLocalizationData* iApptLocalizationData;

    /** Memo Event localization data, owned. */
    MPIMLocalizationData* iMemoLocalizationData;

    /** Anniversary Event localization data, owned. */
    MPIMLocalizationData* iAnnivLocalizationData;

    /** Not owned. */
    MPIMLocalizationManager* iLocalizationManager;

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMEVENTADAPTERACCESS_H
// End of File
