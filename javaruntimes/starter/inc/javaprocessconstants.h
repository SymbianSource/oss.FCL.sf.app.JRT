/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains Java process constants and dll names,
*                module and message ids.
*
*/

#ifndef JAVAPROCESSCONSTANTS_H
#define JAVAPROCESSCONSTANTS_H

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

/**
 * Defines the Java process name. When the Java is eclipsed on top of legacy
 * Java the process name must be the same as in legacy. This is because we
 * are using same data cage
 */
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD // codescanner::ifpreprocessor
const char* const JAVA_PROCESS = "j9midps60";
#else // RD_JAVA_S60_RELEASE_5_0_IAD
const char* const JAVA_PROCESS = "javamidp";
#endif // RD_JAVA_S60_RELEASE_5_0_IAD

// DLL responsible for starting the midp runtime
const char* const JAVA_MIDP_STARTER_DLL = "javamidpstarter";

// DLL responsible for starting the installer runtime
const char* const JAVA_INSTALLER_STARTER_DLL = "javainstallerstarter";

// DLL responsible for starting the preinstaller
const char* const JAVA_PREINSTALLER_STARTER_DLL = "javapreinstaller";

// DLL responsible for starting the javasizehelperserver
const char* const JAVA_SIZE_HELPER_SERVER_DLL = "javasizehelperserver";

} // namespace runtime
} // namespace java

#endif // JAVAPROCESSCONSTANTS_H
