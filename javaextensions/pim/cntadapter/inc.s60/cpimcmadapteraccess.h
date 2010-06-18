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
* Description:  Provides access to Contacts Model adapter
 *
*/


#ifndef CPIMCMADAPTERACCESS_H
#define CPIMCMADAPTERACCESS_H

// INCLUDES
#include "mpimadapteraccess.h"
#include "functionserver.h"

// FORWARD DECLARATIONS
class MPIMLocalizationManager;
class MPIMLocalizationData;

// CLASS DECLARATION
/**
 * Provides the elementary access the PIM Contact list provided by this
 * Contacts Model adapter module.
 */
NONSHARABLE_CLASS(CPIMCMAdapterAccess): public CBase,
        public MPIMAdapterAccess
{
public: // Constructors and a destructor

    /**
     * Two-phased constructor.
     *
     * @param aLocalizationManager Localization Manager.
     */
    static CPIMCMAdapterAccess* NewL(
        MPIMLocalizationManager* aLocalizationManager,
        java::util::FunctionServer* aFuncServer);

    /**
     * destructor
     */
    ~CPIMCMAdapterAccess();

public: // MPIMAdapterAccess

    const CDesCArray& ListNamesL(
        const TPIMListType aListType);

    TBool OpenContactListL(
        const TDesC* aListName,
        MPIMContactAdapterManager** aRetAdapterManager,
        MPIMContactListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData);

    TBool OpenEventListL(
        const TDesC* aListName,
        MPIMEventAdapterManager** aRetAdapterManager,
        MPIMEventListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData);

    TBool OpenToDoListL(
        const TDesC* aListName,
        MPIMToDoAdapterManager** aRetAdapterManager,
        MPIMToDoListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData);

protected:
    /**
     * C++ default constructor.
     */
    CPIMCMAdapterAccess(
        MPIMLocalizationManager* aLocalizationManager,
        java::util::FunctionServer* aFuncServer);

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();

protected: // Data

    /** Owned. */
    CDesCArray* iContactListNames;

    /**
     * An empty array of list names (Event and ToDo List names).
     * Owned.
     */
    CDesCArray* iNoListNames;

    /**
     * Contact localization data, owned.
     */
    MPIMLocalizationData* iContactLocalizationData;

    /** Not owned. */
    MPIMLocalizationManager* iLocalizationManager;

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMCMADAPTERACCESS_H
