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
* Description:  Provides access to Agenda Model todo adapter
 *
*/


#ifndef CPIMTODOADAPTERACCESS_H
#define CPIMTODOADAPTERACCESS_H

// INCLUDES
#include "mpimadapteraccess.h"
#include "functionserver.h"

// CONSTANTS

const TInt KPIMNumberOfToDoLists = 1; // Only one ToDo list

// FORWARD DECLARATIONS
class MPIMLocalizationManager;

// CLASS DECLARATION
/**
 * Provides the elementary access the PIM Contact list provided by this
 * Contacts Model adapter module
 * Access to the adapter is through the static NewL( ) method.
 */
NONSHARABLE_CLASS(CPIMToDoAdapterAccess): public CBase,
        public MPIMAdapterAccess
{
public: // Constructors and a destructor

    /**
     * Two-phased constructor.
     *
     * @param aLocalizationManager Localization Manager.
     */
    static CPIMToDoAdapterAccess* NewL(
        MPIMLocalizationManager* aLocalizationManager,
        java::util::FunctionServer* aFuncServer);

    /**
     * destructor
     */
    ~CPIMToDoAdapterAccess();

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

        const TDesC* aCalNameArg,
        MPIMEventAdapterManager** aRetAdapterManager,
        MPIMEventListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData);

    TBool OpenToDoListL(
        CCalSession* aCalSession,

        const TDesC* aCalNameArg,
        MPIMToDoAdapterManager** aRetAdapterManager,
        MPIMToDoListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData);

private: // Constructor

    /**
     * C++ default constructor.
     */
    CPIMToDoAdapterAccess(
        MPIMLocalizationManager* aLocalizationManager,
        java::util::FunctionServer* aFuncServer);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

protected: // Data

    /** Owned. */
    CDesCArray* iToDoListNames;

    /**
     * An empty array of list names (Event and ToDo List names).
     * Owned.
     */
    CDesCArray* iNoListNames;

    /** Not owned. */
    MPIMLocalizationManager* iLocalizationManager;

    /** Owned. */
    MPIMLocalizationData* iToDoLocalizationData;

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMTODOADAPTERACCESS_H
// End of File
