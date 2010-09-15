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


// INCLUDE files
#include "cpimcmadapteraccess.h"
#include "cpimcmadaptermanager.h"
#include "cpimcontactlistadapter.h"
#include "mpimlocalizationmanager.h"
#include "mpimlocalizationdata.h"
#include "pimlocalizationids.h"
#include "logger.h"

// ============================ MEMBER FUNCTIONS ===============================

const CDesCArray& CPIMCMAdapterAccess::ListNamesL(const TPIMListType aListType)
{
    JELOG2(EPim);
    if (aListType == EPIMContactList)
    {
        return *iContactListNames;
    }
    else
    {
        // no Event or ToDo lists
        return *iNoListNames;
    }
}

TBool CPIMCMAdapterAccess::OpenContactListL(const TDesC* aListName,
        MPIMContactAdapterManager** aRetAdapterManager,
        MPIMContactListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData)
{
    JELOG2(EPim);
    // There is exactly one Contact list, which is the default contact list.

    // If name is given, check that it matches the existing list name
    if (aListName && (*aListName != (*iContactListNames)[0]))
    {
        // Not found
        return EFalse;
    }

    // OK, got this far - Open the Contact list

    CPIMCMAdapterManager* adapterManager = CPIMCMAdapterManager::NewL(
                                               (*iContactListNames)[0]);

    CleanupStack::PushL(adapterManager);

    CPIMContactListAdapter* listAdapter = CPIMContactListAdapter::NewL(
                                              iFuncServer);

    CleanupStack::Pop(adapterManager);

    *aRetAdapterManager = adapterManager;
    *aRetListAdapter = listAdapter;
    *aRetLocalizationData = iContactLocalizationData;

    return ETrue;
}

TBool CPIMCMAdapterAccess::OpenEventListL(CCalSession* /*aCalSession*/, const TDesC* /*aListName*/,
        MPIMEventAdapterManager** /*aRetAdapterManager*/,
        MPIMEventListAdapter** /*aRetListAdapter*/, MPIMLocalizationData** /*aRetLocalizationData*/)
{
    JELOG2(EPim);
    // no Event lists
    return EFalse;
}

TBool CPIMCMAdapterAccess::OpenToDoListL(CCalSession* /*aCalSession*/, const TDesC* /*aListName*/,
        MPIMToDoAdapterManager** /*aRetAdapterManager*/,
        MPIMToDoListAdapter** /*aRetListAdapter*/, MPIMLocalizationData** /*aRetLocalizationData */)
{
    JELOG2(EPim);
    // no ToDo lists
    return EFalse;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterAccess::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMCMAdapterAccess* CPIMCMAdapterAccess::NewL(
    MPIMLocalizationManager* aLocalizationManager,
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMCMAdapterAccess* self =
        new(ELeave) CPIMCMAdapterAccess(aLocalizationManager, aFuncServer);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// destructor
CPIMCMAdapterAccess::~CPIMCMAdapterAccess()
{
    JELOG2(EPim);
    delete iContactListNames;
    delete iNoListNames;
    delete iContactLocalizationData;
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterAccess::CPIMCMAdapterAccess
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMCMAdapterAccess::CPIMCMAdapterAccess(
    MPIMLocalizationManager* aLocalizationManager,
    java::util::FunctionServer* aFuncServer) :
        iLocalizationManager(aLocalizationManager), iFuncServer(aFuncServer)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMCMAdapterAccess::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMCMAdapterAccess::ConstructL()
{
    JELOG2(EPim);
    // there will be one Contact list name
    iContactListNames = new(ELeave) CDesCArrayFlat(1);

    // will be left empty
    iNoListNames = new(ELeave) CDesCArrayFlat(1);

    iContactLocalizationData = iLocalizationManager->GetPIMLocalizationDataL(
                                   KPIMLocalizationIdContacts);

    HBufC* listName = iContactLocalizationData->GetListNameL();
    CleanupStack::PushL(listName);
    iContactListNames->AppendL(*listName);
    CleanupStack::PopAndDestroy(listName);
}
