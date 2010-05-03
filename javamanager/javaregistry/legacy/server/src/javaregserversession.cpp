/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregserversession implementation
*
*/


#include "javaregserversession.h"
#include "javaregdef.h"
#include "javaregproperty.h"
#include "javapropertyarray.h"
#include "javareguidarrayconv.h"
#include "javaregserver.h"
#include "logger.h"

using namespace Java::Manager::Registry;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CJavaRegServerSession::CJavaRegServerSession() :
        iBackupSession(EFalse),
        iLegacy(EFalse)
{
}


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CJavaRegServerSession::~CJavaRegServerSession()
{
    // if it was a backup session move registry back to NoBackup state
    if (iBackupSession)
    {
        Server()->SetBackupState(ENoBackup);
    }
}

// ---------------------------------------------------------------------------
// Returns the iLegacy member
// ---------------------------------------------------------------------------
TBool CJavaRegServerSession::IsLegacy() const
{
    return iLegacy;
}


// ---------------------------------------------------------------------------
// Returns server object
// ---------------------------------------------------------------------------
CJavaRegServer* CJavaRegServerSession::Server() const
{
    return const_cast< CJavaRegServer* >(
               static_cast< const CJavaRegServer* >(CSession2::Server()));
}


// ---------------------------------------------------------------------------
// ServiceL
// ---------------------------------------------------------------------------
void CJavaRegServerSession::ServiceL(const RMessage2& aMessage)
{
    TRAPD(err, DispatchMessageL(aMessage));
    aMessage.Complete(err);
}


// ---------------------------------------------------------------------------
// DispatchMessageL
// ---------------------------------------------------------------------------
void CJavaRegServerSession::DispatchMessageL(const RMessage2& aMessage)
{
    JELOG(EJavaStorage, "CJavaRegServerSession::DispatchMessageL()");

    TInt function = aMessage.Function();

    switch (function)
    {
    case EInitializeServerUseIntegr:
        InitializeServerUseIntegrL(aMessage);
        break;

    case EEntryExists:
        EntryExistsL(aMessage);
        break;

    case EGetEntryUids:
        GetEntryUidsL(aMessage);
        break;

    case EGetEntry:
    {
        GetEntryL(aMessage);
        break;
    }

    case EGetEntryFromAll:
    {
        GetEntryL(aMessage, ETrue);
    }
    break;

    case EFindEntries:
        FindEntriesL(aMessage);
        break;

    case EFindAllEntries:
        FindAllEntriesL(aMessage);
        break;

    case ESetBackupState:
        SetBackupState(aMessage);
        break;

    default:
        User::Leave(KErrNotSupported);
    }
}

// ---------------------------------------------------------------------------
// InitializeServerUseIntegrL
// ---------------------------------------------------------------------------
void CJavaRegServerSession::InitializeServerUseIntegrL(
    const RMessage2& aMessage)
{
    JELOG(EJavaStorage, "CJavaRegServerSession::InitializeServerUseIntegrL()");

    TBool useIntegrity = aMessage.Int0();
    iLegacy = aMessage.Int1();
    if (useIntegrity)
    {
        TTime currentTime;
        currentTime.UniversalTime();
        TInt64 transactionId = currentTime.Int64();
    }
}


// ---------------------------------------------------------------------------
// EntryExistsL
// ---------------------------------------------------------------------------
void CJavaRegServerSession::EntryExistsL(const RMessage2& aMessage)
{
    JELOG(EJavaStorage, "CJavaRegServerSession::EntryExistsL()");

    Server()->LeaveIfRestoringL();

    // obtain uid
    TUid uid = UidFromMessageL(0, aMessage);

    // check if entry exists for uid
    TBool result = Server()->EntryExistsL(uid);

    // write back result
    TPckgBuf<TBool> resultBuf(result);
    aMessage.WriteL(1, resultBuf) ;
}


// ---------------------------------------------------------------------------
// GetEntryUidsL
// ---------------------------------------------------------------------------
void CJavaRegServerSession::GetEntryUidsL(const RMessage2& aMessage)
{
    JELOG(EJavaStorage, "CJavaRegServerSession::GetEntryUidsL()");

    Server()->LeaveIfRestoringL();

    TInt drive = aMessage.Int2();
    RArray<TUid> uids;
    CleanupClosePushL(uids);

    Server()->GetEntryUidsL(uids, drive);
    WriteBackUidArrayL(uids, aMessage) ;
    CleanupStack::PopAndDestroy(&uids);
}


// ---------------------------------------------------------------------------
// GetEntryL
// ---------------------------------------------------------------------------
void CJavaRegServerSession::GetEntryL(const RMessage2& aMessage,
                                      TBool aAllEntries)
{
    JELOG(EJavaStorage, "CJavaRegServerSession::GetEntryL()");

    Server()->LeaveIfRestoringL();

    // read uid
    TUid uid = UidFromMessageL(2, aMessage);

    // get property
    CJavaPropertyArray* props = NULL;
    Server()->GetEntryL(uid, props, aAllEntries);

    TInt neededLength = 0;

    // if there is property write it back
    if (props)
    {
        CleanupStack::PushL(props);
        TInt allocatedLength = aMessage.GetDesMaxLengthL(0);
        neededLength = props->Size();

        if (allocatedLength >= neededLength)
        {
            HBufC8* propBuffer = props->SerializedPropertiesL();
            CleanupStack::PushL(propBuffer);
            aMessage.WriteL(0, propBuffer->Des());
            CleanupStack::PopAndDestroy(propBuffer);
        }
        else
        {
            WLOG(EJavaStorage,
                 "Allocated length is to small. Send back the needed length.");
        }

        // Delete because not cached.
        props->DeleteProperties();
        CleanupStack::PopAndDestroy(props);
    }

    // write back needed length
    TPckgBuf<TInt> lengthPckg(neededLength);
    aMessage.WriteL(1, lengthPckg);
}


// ---------------------------------------------------------------------------
// FindEntries
// ---------------------------------------------------------------------------
void CJavaRegServerSession::FindEntriesL(const RMessage2& aMessage)
{
    JELOG(EJavaStorage, "CJavaRegServerSession::FindEntriesL()");

    Server()->LeaveIfRestoringL();

    RArray<TUid> uids;
    CleanupClosePushL(uids);

    // read properties
    CJavaPropertyArray* props = PropertiesFromMessageL(2, aMessage);
    CleanupStack::PushL(props);

    Server()->FindEntriesL(*props, uids);

    CleanupStack::PopAndDestroy(props);

    WriteBackUidArrayL(uids, aMessage) ;

    CleanupStack::PopAndDestroy(&uids);
}


// ---------------------------------------------------------------------------
// FindAllEntries
// ---------------------------------------------------------------------------
void CJavaRegServerSession::FindAllEntriesL(const RMessage2& aMessage)
{
    JELOG(EJavaStorage, "CJavaRegServerSession::FindEntriesL()");

    Server()->LeaveIfRestoringL();

    RArray<TUid> uids;
    CleanupClosePushL(uids);

    // read properties
    CJavaPropertyArray* props = PropertiesFromMessageL(2, aMessage);
    CleanupStack::PushL(props);

    Server()->FindAllEntriesL(*props, uids);

    CleanupStack::PopAndDestroy(props);

    WriteBackUidArrayL(uids, aMessage) ;

    CleanupStack::PopAndDestroy(&uids);
}


// ---------------------------------------------------------------------------
// SetBackupState
// ---------------------------------------------------------------------------
void CJavaRegServerSession::SetBackupState(const RMessage2& aMessage)
{
    JELOG(EJavaStorage, "CJavaRegServerSession::SetBackupState()");

    TBackupState state = (TBackupState) aMessage.Int0();
    Server()->SetBackupState(state);
    iBackupSession = ETrue;
}


// ---------------------------------------------------------------------------
// UidFromMessageL
// ---------------------------------------------------------------------------
TUid CJavaRegServerSession::UidFromMessageL(TInt aIndex,
        const RMessage2&  aMessage)
{
    TUid uid = TUid::Uid(0);
    TPckg<TUid> uidPack(uid);
    aMessage.ReadL(aIndex, uidPack);
    return uid;
}


// ---------------------------------------------------------------------------
// PropertiesFromMessageL
// ---------------------------------------------------------------------------
CJavaPropertyArray* CJavaRegServerSession::PropertiesFromMessageL(
    TInt aIndex,
    const RMessage2&  aMessage)
{
    TInt length = aMessage.GetDesLengthL(aIndex);
    HBufC8* propBuffer = HBufC8::NewLC(length);
    TPtr8 ptr = propBuffer->Des();
    aMessage.ReadL(aIndex, ptr);
    CJavaPropertyArray* props = CJavaPropertyArray::NewL(propBuffer);
    CleanupStack::PopAndDestroy(propBuffer);

    return props;
}


// ---------------------------------------------------------------------------
// WriteBackUidArrayL
// ---------------------------------------------------------------------------
void CJavaRegServerSession::WriteBackUidArrayL(RArray<TUid>& aUids,
        const RMessage2&  aMessage)
{
    HBufC16* uidBuffer = NULL;
    JavaRegUidArrayConverter::StoreTUidsL(aUids, uidBuffer);
    CleanupStack::PushL(uidBuffer);

    TInt allocatedLength = aMessage.GetDesMaxLengthL(0);
    TInt neededLength = uidBuffer->Length();

    if (allocatedLength >= neededLength)
    {
        aMessage.WriteL(0, uidBuffer->Des());
    }
    else
    {
        WLOG(EJavaStorage,
             "Allocated length is to small. Send back the needed length.");
    }

    CleanupStack::PopAndDestroy(uidBuffer);

    // write back needed length
    TPckgBuf<TInt> lengthPckg(neededLength);
    aMessage.WriteL(1, lengthPckg);
}
