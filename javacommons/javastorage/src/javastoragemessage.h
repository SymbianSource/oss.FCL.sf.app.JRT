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
* Description:  JavaStorageMessage
*
*/


#ifndef JAVASTORAGEMESSAGE_H
#define JAVASTORAGEMESSAGE_H

#include "comms.h"

namespace java
{
namespace storage
{

/**
 * JavaStorageMessage contains storage related communication constants.
 */
class JavaStorageMessage
{
public:
    enum MessageIdentifier
    {
        EOpen = 1,
        EClose,
        EStartTransaction,
        ECommit,
        ERollback,
        ERead,
        EWrite,
        ESearch,
        ERemove,
        ECreateTable,
        EAppendTable,
        EUpdate,
        EShutdown,
        EGetAvailableDrives
    };

private:
};

/**
 * Connect and disconnect message identifier.
 * ServerMessage: string headers, string dbAddress
 * ResponseMessage: string headers
 */
const int IPC_JS_CONN_REG =
    java::comms::JAVASTORAGE_MESSAGE_ID_RANGE_START_C + 1;

} // end namespace storage
} // end namespace java

#endif // JAVASTORAGEMESSAGE_H

