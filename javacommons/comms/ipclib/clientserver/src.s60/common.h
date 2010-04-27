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
* Description: Comms helper functions and constants
*
*/

#ifndef COMMON_H
#define COMMON_H

#include "transport.h"

namespace java
{
namespace comms
{

const TInt KCommsStackSize = 0x5000;

enum ECommsOperations
{
    ESend,
    EReceive,
    ECancelReceive
};

// delete must be used to free allocated memory
HBufC8* messageToDes(const ipcMessage_t& aMsg);

// delete[] must be used to free allocated memory
char* desToMessage(const TDesC8& aDes);

} // namespace comms
} // namespace java

#endif // COMMON_H
