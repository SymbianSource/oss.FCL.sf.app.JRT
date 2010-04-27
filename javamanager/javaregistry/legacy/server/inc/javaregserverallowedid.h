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
* Description:  javaregserverallowedid definition
*
*/


#ifndef JAVAREGSERVERALLOWEDID_H
#define JAVAREGSERVERALLOWEDID_H

#include <javauids.h>


namespace Java
{
namespace Manager
{
namespace Registry
{

/**
 * These are the allowed secureIds who can access the JavaRegistryServer.
 */
const TSecureId AllowedClientSecureId_Backup = KBackupCoreUid;
const TSecureId AllowedClientSecureId_Installer = 0x101F875A;   // KSWInstSvrUid in AppinstUI
const TSecureId AllowedClientSecureId_OsgiVm = 0x10275084;      // OsgiVm secure ID
const TSecureId AllowedClientSecureId_JavaHelperServer = 0x1020329F;   // JavaHelperServer.exe
// secure ID - for legacy reasons
const TSecureId AllowedClientSecureId_AppInstUi = 0x101F875A;   // AppInstUi (SWInstSvrUI.exe)
// secure ID - for legacy reasons
const TSecureId AllowedClientSecureId_SDKDebugAgent = 0x10208DDD; // SDK debug agent

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGSERVERALLOWEDID_H
