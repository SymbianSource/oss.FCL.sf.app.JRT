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
* Description: Java Debug API
*
*/


#ifndef JAVADEBUGAPI_H
#define JAVADEBUGAPI_H

#include <e32cmn.h>

/**
 * Java Debug API allows it's users to install, remove, start and
 * stop Java applications.
 *
 * @lib javadebugapi.lib
 * @since S60 v9.2
 */

namespace java
{
namespace debug
{

/**
 * Installs Java application.
 * Java application is installed silently i.e. no prompts are shown to the user.
 * @param[in] aFilename shall point to local JAD or JAR file
 * @param[out] aSuiteUid returns the suite uid assigned to the application
 * @param[out] aApplicationUid returns a list of application uids assigned to applications inside suite uid
 * @capability TrustedUI
 * @return true if installation is successful
 */
IMPORT_C TBool installApp(HBufC& aFilename, TUid& aSuiteUid, RArray<TUid>& aApplicationUid);

/**
 * Uninstalls Java application.
 * Java application is removed silently i.e. no prompts are shown to the user.
 * If application uid is given then the application suite it belongs is removed too.
 * @param[in] aUid application suite uid (or application uid)
 * @capability TrustedUI
 * @return true if application is uninstalled successfully
 */
IMPORT_C TBool uninstallApp(TUid aSuiteUid);

/**
 * Starts Java application.
 * @param[in] aApplicationUid application uid
 * @param[in] aUeiParameters includes arguments defined in Unified Emulator Interface (UEI) specification.
 * If aUeiParameters contains unsupported options then those options are discarded silently.
 * Supported arguments:
 *   -Xverbose[:options]
 *   -Xdebug -Xrunjdwp:name=value[,name=value[...]]
 * For example, to start Java application in debug mode and set full tracing on
 * following UIE arguments could be given:
 * "-Xdebug -Xrunjdwp:server=y,address=localhost:8000 -Xverbose:all"
 * @capability NetworkControl
 * @return true if application is started
 */
IMPORT_C TBool startApp(TUid aApplicationUid, HBufC& aUeiParameters);

/**
 * Stops Java application.
 * @param[in] aApplicationUid application uid
 * @capability PowerMgmt
 * @return true if application is stopped
 */
IMPORT_C TBool stopApp(TUid aApplicationUid);

} // end namespace debug
} // end namespace java

#endif // JAVADEBUGAPI_H
