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
* Description:  javasizehelpcommon definitions
*
*/


#ifndef JAVASIZEHELPCOMMON_H
#define JAVASIZEHELPCOMMON_H

// INCLUDES
#include <e32base.h>

//JavaSizeHelpServer names
_LIT(KJavaSizeHelpServerName, "javasizehelperserver");
_LIT(KJavaSizeHelpServerExe, "javasizehelperserver.exe");

//JavaSizeHelpServer UID
const TUid KUidJavaSizeHelpServer = { 0x2000BF08 };

//Server services
enum TService
{
    EGetUserUsedDiskSpace,
    EMaxService
};

#endif // JAVASIZEHELPCOMMON_H
