/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregserver implementation
*
*/


#include "javaregserver.h"
#include "javaregserversession.h"
#include "javaregserverscheduler.h"
#include "javapropertyarray.h"
#include "javaregstore.h"
#include "logger.h"

namespace Java
{
namespace Manager
{
namespace Registry
{

/**
 * Input params to set up CPolicyServer's security policy
 */
const TInt KRangeCnt = 3;
const TInt Ranges[KRangeCnt] =
{
    // range for EInitializeServerUseIntegr, EEntryExists,
    // EGetEntryUids, EGetEntry, EFindEntries
    0,
    // range for EInitializeServerTransId, ESetEntry,
    // ERemoveEntry, EAllocateUids, ESetBackupState
    EInitializeServerTransId,
    // rest of requests, that not exist
    ESetBackupState + 1
};

const TUint8 PolicyInds[] =
{
    // range for EInitializeServerUseIntegr, EEntryExists,
    // EGetEntryUids, EGetEntry, EFindEntries
    CPolicyServer::EAlwaysPass,
    // range for EInitializeServerTransId, ESetEntry,
    // ERemoveEntry, EAllocateUids, ESetBackupState
    CPolicyServer::ECustomCheck,
    // rest of functions, that not exist
    CPolicyServer::ENotSupported
};

const CPolicyServer::TPolicy Policy =
{
    CPolicyServer::EAlwaysPass,    // on connect
    KRangeCnt,
    Ranges,
    PolicyInds,
    0 // Mandatory spare
};

} // namespace Registry
} // namespace Manager
} // namespace Java


using namespace Java::Manager::Registry;

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CJavaRegServer* CJavaRegServer::NewL()
{
    CJavaRegServer* server = new(ELeave) CJavaRegServer();

    CleanupStack::PushL(server);
    server->ConstructL();
    CleanupStack::Pop(server);

    return server;
}


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CJavaRegServer::CJavaRegServer() :
        CPolicyServer(CActive::EPriorityStandard, Policy, ESharableSessions),
        iBackupState(ENoBackup),
        iStore(NULL)
{
}


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CJavaRegServer::ConstructL()
{
    JELOG(EJavaStorage, "CJavaRegServer::ConstructL()");
    // Start server.
    StartL(KJavaRegServerName);

    // create store object
    iStore = CJavaRegStore::NewL();
}


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CJavaRegServer::~CJavaRegServer()
{
    JELOG(EJavaStorage, "CJavaRegServer::~CJavaRegServer()");
    delete iStore;
    iStore = NULL;
}


// ---------------------------------------------------------------------------
// Creates new session
// ---------------------------------------------------------------------------
CSession2* CJavaRegServer::NewSessionL(const TVersion & /*aVersion*/,
                                       const RMessage2& /*aMessage*/
                                      ) const
{
    return new(ELeave) CJavaRegServerSession();
}


// ---------------------------------------------------------------------------
// Panics client
// ---------------------------------------------------------------------------
void CJavaRegServer::PanicClient(const RMessage2&  aMessage,
                                 TInt aPanic) const
{
    WLOG1(EJavaStorage, "Panic Client: %d", aPanic);
    aMessage.Panic(KJavaRegServerName, aPanic);
}


// ---------------------------------------------------------------------------
// EntryExistsL
// ---------------------------------------------------------------------------
TBool CJavaRegServer::EntryExistsL(TUid aUid)
{
    RArray<TUid> allUids;
    CleanupClosePushL(allUids);

    iStore->GetUidsL(allUids);

    TInt res = allUids.Find(aUid);

    CleanupStack::PopAndDestroy(&allUids);

    return (res != KErrNotFound);
}


// ---------------------------------------------------------------------------
// GetEntryUidsL
// ---------------------------------------------------------------------------
void CJavaRegServer::GetEntryUidsL(RArray<TUid>& aUids, TInt aDrive)
{
    if (aDrive < 0)
    {
        iStore->GetUidsL(aUids);
    }
    else
    {
        ELOG1(
            EJavaStorage,
            "GetEntryUidsL from drive: %d is not supported",
            aDrive);
        User::Leave(KErrNotSupported);
    }
}


// ---------------------------------------------------------------------------
// GetEntryL
// ---------------------------------------------------------------------------
void CJavaRegServer::GetEntryL(TUid aUid,
                               CJavaPropertyArray*& aProperties, TBool aAllEntries)
{
    if (aUid != TUid::Null())
    {
        iStore->ReadPropertiesL(aUid, aProperties, aAllEntries);
    }
}


// ---------------------------------------------------------------------------
// FindEntriesL
// ---------------------------------------------------------------------------
void CJavaRegServer::FindEntriesL(CJavaPropertyArray& aProperties,
                                  RArray<TUid>& aFoundUids)
{
    iStore->FindEntriesL(aProperties, aFoundUids);
}

// ---------------------------------------------------------------------------
// FindAllEntriesL
// ---------------------------------------------------------------------------
void CJavaRegServer::FindAllEntriesL(CJavaPropertyArray& aProperties,
                                     RArray<TUid>& aFoundUids)
{
    iStore->FindEntriesL(aProperties, aFoundUids, ETrue);
}


// ---------------------------------------------------------------------------
// SetBackupState
// ---------------------------------------------------------------------------
void CJavaRegServer::SetBackupState(TBackupState aState)
{
    iBackupState = aState;
}


// ---------------------------------------------------------------------------
// LeaveIfBackingUpOrRestoringL
// ---------------------------------------------------------------------------
void CJavaRegServer::LeaveIfBackingUpOrRestoringL()
{
    if ((iBackupState == EBackuping) || (iBackupState == ERestoring))
    {
        User::Leave(KErrServerBusy);
    }
}


// ---------------------------------------------------------------------------
// LeaveIfRestoringL
// ---------------------------------------------------------------------------
void CJavaRegServer::LeaveIfRestoringL()
{
    if (iBackupState == ERestoring)
    {
        User::Leave(KErrServerBusy);
    }
}


// ---------------------------------------------------------------------------
// Server Loader Part
// ---------------------------------------------------------------------------
GLDEF_C TInt E32Main()
{
    TInt err = CJavaRegServerScheduler::ThreadStart();
    return err;
}
