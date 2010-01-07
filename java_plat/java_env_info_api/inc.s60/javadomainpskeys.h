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
* Description:  javautils pskey definitions
*
*/

#ifndef JAVADOMAINPSKEY_H
#define JAVADOMAINPSKEY_H

/**
 * This header provides info about Java environment PS keys.
 *
 * During Java application installation the JavaInstaller updates the
 * installation state to P&S keys. The states are changed as follows:
 *
 *   Installation (or upgrade) begins: iUid = 0, iProgress = 0, iState = INSTALLING
 *   Installation proceeds: iUid = no_change, iProgress = xx%, iState = no_change
 *   Installation completes: iUid = suiteUid, iProgress = 100, iState = INSTALL_OK
 *   Installation fails: iUid = suiteUid, iProgress = 0, iState = INSTALL_FAIL
 *
 *   Uninstallation begins: iUid = 0, iProgress = 0, iState = UNINSTALLING
 *   Uninstallation proceeds: iUid = no_change, iProgress = xx%, iState = no_change
 *   Uninstallation completes: iUid = suiteUid, iProgress = 100, iState = UNINSTALL_OK
 *   Uninstallation fails: iUid = suiteUid, iProgress = 0, iState = UNINSTALL_FAIL
 *
 *
 *
 * P&S keys that are updated:
 *
 *   iUid ==> KPSUidJavaLatestInstallation
 *   iProgress ==> KPSUidJavaLatestInstallationProgress
 *   iState ==> KPSUidJavaLatestInstallationState
 *
 * Values for the P&S keys:
 *
 *   INSTALLING = 0x100;
 *   INSTALL_OK = 0x101;
 *   INSTALL_FAIL = 0x102;
 *   UNINSTALLING = 0x200;
 *   UNINSTALL_OK = 0x201;
 *   UNINSTALL_FAIL = 0x202;
 */

#ifndef KPSUidJavaLatestInstallation
/** PS flag for the most recent installed Java application. */
#define KPSUidJavaLatestInstallation 0x10282567
#endif

#ifndef KPSUidJavaLatestInstallationProgress
/** PS flag for indicating JavaInstaller installation progress. */
#define KPSUidJavaLatestInstallationProgress 0x20019546
#endif

#ifndef KPSUidJavaLatestInstallationState
/** PS flag for JavaInstaller latest installation state. */
#define KPSUidJavaLatestInstallationState 0x20019547
#endif

#endif //JAVADOMAINPSKEY_H

