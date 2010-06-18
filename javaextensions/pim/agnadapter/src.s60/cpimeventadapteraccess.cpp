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


// INCLUDE FILES
#include "cpimeventadapteraccess.h"
#include "cpimapptadaptermanager.h"
#include "cpimmemoadaptermanager.h"
#include "cpimannivadaptermanager.h"
#include "cpimagnapptadapter.h"
#include "cpimagnmemoadapter.h"
#include "cpimagnannivadapter.h"
#include "mpimlocalizationmanager.h"
#include "mpimlocalizationdata.h"
#include "pimlocalizationids.h"
#include "logger.h"

// -----------------------------------------------------------------------------
// CPIMEventAdapterAccess::CPIMEventAdapterAccess
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMEventAdapterAccess::CPIMEventAdapterAccess(
    MPIMLocalizationManager* aLocalizationManager,
    java::util::FunctionServer* aFuncServer) // not owned
        :
        iLocalizationManager(aLocalizationManager), iFuncServer(aFuncServer)
{
    JELOG2(EPim);
}

void CPIMEventAdapterAccess::ConstructL()
{
    JELOG2(EPim);
    iEventListNames = new(ELeave) CDesCArrayFlat(KPIMNumberOfEventLists);

    // will be left empty
    iNoListNames = new(ELeave) CDesCArrayFlat(1);

    // Order of lists matters:
    // Appointments - Events (Memos) - Anniversaries
    // See OpenEventListL().

    iApptLocalizationData = iLocalizationManager->GetPIMLocalizationDataL(
                                KPIMLocalizationIdEvent, KPIMListNameIdAppointment);

    iMemoLocalizationData = iLocalizationManager->GetPIMLocalizationDataL(
                                KPIMLocalizationIdEvent, KPIMListNameIdEvent);

    iAnnivLocalizationData = iLocalizationManager->GetPIMLocalizationDataL(
                                 KPIMLocalizationIdEvent, KPIMListNameIdAnniversary);

    HBufC* listName = NULL; // reused

    // Order of lists matters, see above.

    listName = iApptLocalizationData->GetListNameL();
    CleanupStack::PushL(listName);
    iEventListNames->AppendL(*listName);
    CleanupStack::PopAndDestroy(listName);

    listName = iMemoLocalizationData->GetListNameL();
    CleanupStack::PushL(listName);
    iEventListNames->AppendL(*listName);
    CleanupStack::PopAndDestroy(listName);

    listName = iAnnivLocalizationData->GetListNameL();
    CleanupStack::PushL(listName);
    iEventListNames->AppendL(*listName);
    CleanupStack::PopAndDestroy(listName);
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterAccess::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMEventAdapterAccess* CPIMEventAdapterAccess::NewL(
    MPIMLocalizationManager* aLocalizationManager,
    java::util::FunctionServer* aFuncServer) // not owned
{
    JELOG2(EPim);
    CPIMEventAdapterAccess * self =
        new(ELeave) CPIMEventAdapterAccess(aLocalizationManager, aFuncServer);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// destructor
CPIMEventAdapterAccess::~CPIMEventAdapterAccess()
{
    JELOG2(EPim);
    delete iEventListNames;
    delete iNoListNames;
    delete iApptLocalizationData;
    delete iMemoLocalizationData;
    delete iAnnivLocalizationData;
}

const CDesCArray& CPIMEventAdapterAccess::ListNamesL(
    const TPIMListType aListType)
{
    JELOG2(EPim);
    if (aListType == EPIMEventList)
    {
        return *iEventListNames;
    }
    else
    {
        // no Event or ToDo lists
        return *iNoListNames;
    }
}

TBool CPIMEventAdapterAccess::OpenContactListL(const TDesC* /*aListName*/,
        MPIMContactAdapterManager** /*aRetAdapterManager*/,
        MPIMContactListAdapter** /*aRetListAdapter*/, MPIMLocalizationData** /*aRetLocalizationData*/)
{
    JELOG2(EPim);
    // no Contact lists
    return EFalse;
}

TBool CPIMEventAdapterAccess::OpenEventListL(const TDesC* aListName,
        MPIMEventAdapterManager** aRetAdapterManager,
        MPIMEventListAdapter** aRetListAdapter,
        MPIMLocalizationData** aRetLocalizationData)
{
    JELOG2(EPim);
    // Open named Event list or the default list if no name given
    TPIMLocalizationDataID listId = KPIMListNameIdAppointment; // default

    // Order of lists is Appointments - Memos - Anniversaries

    if (aListName)
    {
        TInt listNamePos(0);
        if (0 == iEventListNames->Find(*aListName, listNamePos))
        {
            listId = TPIMLocalizationDataID(listNamePos);
        }
        else
        {
            // List not found
            return EFalse;
        }
    }

    // OK, we have now identified the list - open it

    MPIMEventAdapterManager* adapterManager = NULL;
    MPIMEventListAdapter* listAdapter = NULL;
    MPIMLocalizationData* localizationData = NULL;

    switch (listId)
    {
    case EPIMEventListAppointment:
    {
        adapterManager = CPIMApptAdapterManager::NewL(
                             (*iEventListNames)[listId]);
        CleanupDeletePushL(adapterManager);

        CPIMAgnApptAdapter* adapter = CPIMAgnApptAdapter::NewL(iFuncServer);
        CleanupStack::PushL(adapter);

        listAdapter = CPIMEventListAdapter::NewL(CCalEntry::EAppt, adapter,
                      iFuncServer);

        CleanupStack::Pop(adapter);
        CleanupStack::Pop(adapterManager);

        localizationData = iApptLocalizationData;

        break;
    }
    case EPIMEventListEvent:
    {
        adapterManager = CPIMMemoAdapterManager::NewL(
                             (*iEventListNames)[listId]);
        CleanupDeletePushL(adapterManager);

        CPIMAgnMemoAdapter* adapter = CPIMAgnMemoAdapter::NewL(iFuncServer);
        CleanupStack::PushL(adapter);

        listAdapter = CPIMEventListAdapter::NewL(CCalEntry::EEvent, adapter,
                      iFuncServer);

        CleanupStack::Pop(adapter);
        CleanupStack::Pop(adapterManager);

        localizationData = iMemoLocalizationData;

        break;
    }
    case EPIMEventListAnniversary:
    {
        adapterManager = CPIMAnnivAdapterManager::NewL(
                             (*iEventListNames)[listId]);
        CleanupDeletePushL(adapterManager);

        CPIMAgnAnnivAdapter* adapter = CPIMAgnAnnivAdapter::NewL(iFuncServer);
        CleanupStack::PushL(adapter);

        listAdapter = CPIMEventListAdapter::NewL(CCalEntry::EAnniv, adapter,
                      iFuncServer);

        CleanupStack::Pop(adapter);
        CleanupStack::Pop(adapterManager);

        localizationData = iAnnivLocalizationData;

        break;
    }
    default:
    {
        // We should never end up here (but if we do, the list name
        // mappings are not OK)
        User::Leave(KErrCorrupt);
    }
    }

    *aRetAdapterManager = adapterManager;
    *aRetListAdapter = listAdapter;
    *aRetLocalizationData = localizationData;

    return ETrue;
}

TBool CPIMEventAdapterAccess::OpenToDoListL(const TDesC* /*aListName*/,
        MPIMToDoAdapterManager** /*aRetAdapterManager*/,
        MPIMToDoListAdapter** /*aRetListAdapter*/, MPIMLocalizationData** /*aRetLocalizationData*/)
{
    JELOG2(EPim);
    // no ToDo lists
    return EFalse;
}

//  End of File
