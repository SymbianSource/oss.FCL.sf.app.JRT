/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  APNSettings implementation.
*
*/


#include "apncenreptable.h"
#include "connectionmanager.h"
#include "logger.h"
#include "javacommonutils.h" // Required by logger.h
#include "javasymbianoslayer.h"
#include "javauid.h"

namespace CENREP
{

CAPNCenRepTable::~CAPNCenRepTable()
{
}

CAPNCenRepTable* CAPNCenRepTable::NewLC()
{
    CAPNCenRepTable* table = new(ELeave) CAPNCenRepTable;
    CleanupStack::PushL(table);
    return table;
}

void CAPNCenRepTable::CreateL()
{
    // noop
}

void CAPNCenRepTable::OpenL()
{
    // noop
}

void CAPNCenRepTable::Close(void)
{
    // noop
}

TUint32 CAPNCenRepTable::GetAPNIdL(TUint32 aMSId)
{
    TUid uid = TUid::Uid(aMSId);
    java::util::Uid javaUid;
    TUidToUid(uid, javaUid);

    return ConnectionManager::getApnIdL(javaUid);
}

void CAPNCenRepTable::SetAPNL(TUint32 aMSId, TUint32  aAPNId)
{
    TUid uid = TUid::Uid(aMSId);
    java::util::Uid javaUid;
    TUidToUid(uid, javaUid);
    ConnectionManager::setApnIdL(javaUid, aAPNId);
}

void CAPNCenRepTable::RemoveL(TUint32 aMSId)
{
    TUid uid = TUid::Uid(aMSId);
    java::util::Uid javaUid;
    TUidToUid(uid, javaUid);
    ConnectionManager::setApnIdL(javaUid, KJavaNetworkAccessNotSpecified);
}

void CAPNCenRepTable::SetUseSystemDefaultAPNL(TUint32 /*aMSId*/)
{
}

void CAPNCenRepTable::SetUseJavaDefaultAPNL(TUint32 /*aMSId*/)
{
}

TBool CAPNCenRepTable::IsUseSystemDefaultAPNL(TUint32 /*aMSId*/)
{
    return false;
}

TBool CAPNCenRepTable::IsUseJavaDefaultAPNL(TUint32 /*aMSId*/)
{
    return false;
}

TUint32 CAPNCenRepTable::GetJavaDefaultAPNL()
{
    return KMIDletSuiteAPNNotSpecified;
}

void CAPNCenRepTable::SetJavaDefaultAPNL(TUint32 /*aAPNId*/)
{
}
} // end of namespace CENREP


