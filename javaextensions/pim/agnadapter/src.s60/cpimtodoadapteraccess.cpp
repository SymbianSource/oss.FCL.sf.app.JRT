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


// INCLUDE FILES
#include "cpimtodoadapteraccess.h"
#include "cpimtodoadaptermanager.h"
#include "cpimtodolistadapter.h"
#include "mpimlocalizationmanager.h"
#include "mpimlocalizationdata.h"
#include "pimlocalizationids.h"
#include "logger.h"

// -----------------------------------------------------------------------------
// CPIMToDoAdapterAccess::CPIMToDoAdapterAccess
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMToDoAdapterAccess::CPIMToDoAdapterAccess(
    MPIMLocalizationManager* aLocalizationManager,
    java::util::FunctionServer* aFuncServer) :
        iLocalizationManager(aLocalizationManager), iFuncServer(aFuncServer)
{
    JELOG2(EPim);
}

void CPIMToDoAdapterAccess::ConstructL()
{
    JELOG2(EPim);
    iToDoLocalizationData = iLocalizationManager->GetPIMLocalizationDataL(
                                KPIMLocalizationIdToDo);

    iToDoListNames = new(ELeave) CDesCArrayFlat(KPIMNumberOfToDoLists);
    iNoListNames = new(ELeave) CDesCArrayFlat(1);  // will be left empty

    HBufC* listName = iToDoLocalizationData->GetListNameL();
    CleanupStack::PushL(listName);
    iToDoListNames->AppendL(*listName);
    CleanupStack::PopAndDestroy(listName);
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterAccess::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMToDoAdapterAccess* CPIMToDoAdapterAccess::NewL(
    MPIMLocalizationManager* aLocalizationManager,
    java::util::FunctionServer* aFuncServer) // not owned
{
    JELOG2(EPim);
    CPIMToDoAdapterAccess * self =
        new(ELeave) CPIMToDoAdapterAccess(aLocalizationManager, aFuncServer);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// destructor
CPIMToDoAdapterAccess::~CPIMToDoAdapterAccess()
{
    JELOG2(EPim);
    delete iToDoLocalizationData;
    delete iToDoListNames;
    delete iNoListNames;
}

const CDesCArray& CPIMToDoAdapterAccess::ListNamesL(
    const TPIMListType aListType)
{
    JELOG2(EPim);
    if (aListType == EPIMToDoList)
    {
        return *iToDoListNames;
    }
    else
    {
        // no Contact or Event lists
        return *iNoListNames;
    }
}

TBool CPIMToDoAdapterAccess::OpenContactListL(const TDesC* /*aListName*/,
        MPIMContactAdapterManager** /*aRetAdapterManager*/,
        MPIMContactListAdapter** /*aRetListAdapter*/, MPIMLocalizationData** /*aRetLocalizationData*/)
{
    JELOG2(EPim);
    // no Contact lists
    return EFalse;
}

TBool CPIMToDoAdapterAccess::OpenEventListL(CCalSession* /*aCalSession*/, 
        const TDesC* /*aListName*/,
        MPIMEventAdapterManager** /*aRetAdapterManager*/,
        MPIMEventListAdapter** /*aRetListAdapter*/, 
        MPIMLocalizationData** /*aRetLocalizationData*/
        )
{
    JELOG2(EPim);
    // no Event lists
    return EFalse;
}

TBool CPIMToDoAdapterAccess::OpenToDoListL(CCalSession* aCalSession, 
        const TDesC* aListName,
        MPIMToDoAdapterManager** aRetAdapterManager,
        MPIMToDoListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData)
{
    JELOG2(EPim);
    // There is exactly one ToDo list, which is the default ToDo list.

    // If name is given, check that it matches the existing list name
    if (aListName && (*aListName != (*iToDoListNames)[0]))
    {
        // Not found
        return EFalse;
    }

    // OK, got this far - Open the Todo list

    MPIMToDoAdapterManager* adapterManager = CPIMToDoAdapterManager::NewL(
                (*iToDoListNames)[0]);
    CleanupDeletePushL(adapterManager);

    MPIMToDoListAdapter* listAdapter = CPIMToDoListAdapter::NewL(iFuncServer, aCalSession);
    CleanupStack::Pop(adapterManager);

    MPIMLocalizationData* localizationData = iToDoLocalizationData;

    // Return values
    *aRetAdapterManager = adapterManager;
    *aRetListAdapter = listAdapter;
    *aRetLocalizationData = localizationData;

    return ETrue;
}

//  End of File
