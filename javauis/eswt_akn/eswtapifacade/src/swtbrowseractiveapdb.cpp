/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/


#include <ApSelect.h>
#include <ApListItemList.h>
#include <ActiveApDb.h>
#include <ApListItem.h>


#include "swtbrowseractiveapdb.h"



class CSwtBrowserApDbObserver
        : public CBase
        , public MActiveApDbObserver
{
public:

    CSwtBrowserApDbObserver(CSwtBrowserActiveApDb* aApDb);

    ~CSwtBrowserApDbObserver();
    // From MActiveApDbObserver

    /**
     * From MActiveApDbObserver
     * Handle database event (refresh cached AP list on change).
     *
     * @param aEvent Database-related event.
     * @return void.
     */
    void HandleApDbEventL(MActiveApDbObserver::TEvent aEvent);


private:
    /*
     * Pointer to avtive ap database
     * Not own
     */
    CSwtBrowserActiveApDb* iApDb;
};


CSwtBrowserApDbObserver::CSwtBrowserApDbObserver(CSwtBrowserActiveApDb* aApDb)
        : iApDb(aApDb)
{

}

CSwtBrowserApDbObserver::~CSwtBrowserApDbObserver()
{

}


void CSwtBrowserApDbObserver::HandleApDbEventL(
    MActiveApDbObserver::TEvent aEvent)
{
    iApDb->HandleApDbEventL(aEvent);
}



EXPORT_C CSwtBrowserActiveApDb* CSwtBrowserActiveApDb::NewL()
{
    CSwtBrowserActiveApDb* activeAp = new(ELeave) CSwtBrowserActiveApDb();
    CleanupStack::PushL(activeAp);
    activeAp->ConstructL();
    CleanupStack::Pop(activeAp);
    return activeAp;
}

CSwtBrowserActiveApDb::~CSwtBrowserActiveApDb()
{
    if (iDb)
    {
        iDb->RemoveObserver(iObserver);
    }
    delete iObserver;
    delete iCachedApList;
    delete iDb;
}


CSwtBrowserActiveApDb::CSwtBrowserActiveApDb(): iUpdatePending(EFalse)
{
}


void CSwtBrowserActiveApDb::ConstructL()
{
    iDb = CActiveApDb::NewL(EDatabaseTypeIAP);
    iObserver = new(ELeave) CSwtBrowserApDbObserver(this);
    iDb->AddObserverL(iObserver);
    iCachedApList = new(ELeave) CApListItemList();
    iDelayedRefresh = ETrue;
}

void CSwtBrowserActiveApDb::HandleApDbEventL(const TInt aEvent)
{
    // Refreshing AP list when:
    // 1. Database changed;
    // 2. Database unlocked and there is a pending update
    if (aEvent == MActiveApDbObserver::EDbChanged ||
            (aEvent == MActiveApDbObserver::EDbAvailable && iUpdatePending))
    {
        // Reefresh cached AP list.
        iDelayedRefresh = ETrue;
    }
}

EXPORT_C CActiveApDb& CSwtBrowserActiveApDb::ApDb() const
{
    return *iDb;
}


EXPORT_C CCommsDatabase& CSwtBrowserActiveApDb::CommsDb() const
{
    return *(iDb->Database());
}

EXPORT_C TUint32 CSwtBrowserActiveApDb::FirstAccessPointUidL()
{
    if (iDelayedRefresh)
    {
        GetAccessPointsL();
        iDelayedRefresh = EFalse;
    }

    if (iCachedApList->Count() > 0)
    {
        return (*iCachedApList)[0]->Uid();
    }
    else
    {
        return  0;
    }
}


void CSwtBrowserActiveApDb::GetAccessPointsL()
{
    CApListItemList* apList = new(ELeave) CApListItemList();
    CleanupStack::PushL(apList);
    TRAPD(err, DoGetAccessPointsL(*apList));
    switch (err)
    {
    case KErrNone:
    {
        // Got new data.
        delete iCachedApList;
        CleanupStack::Pop(apList);
        iCachedApList = apList;
        iUpdatePending = EFalse;
        break;
    }

    case KErrLocked:
    case KErrAccessDenied:
    case KErrPermissionDenied:
    {
        // Could not access database. Update now pending, old data remains.
        CleanupStack::PopAndDestroy(apList);
        iUpdatePending = ETrue;
        break;
    }

    default:
    {
        // Other error. Propagate.
        User::Leave(err);
        break;
    }
    }
}


void CSwtBrowserActiveApDb::DoGetAccessPointsL(CApListItemList& aList) const
{
    CApSelect* apSelect = CApSelect::NewLC
                          (
                              *(iDb->Database()),
                              KEApIspTypeAll,
                              EApBearerTypeAll,
                              KEApSortNameAscending,
                              EIPv4 | EIPv6
                          );
    apSelect->AllListItemDataL(aList);
    CleanupStack::PopAndDestroy(apSelect);
}
