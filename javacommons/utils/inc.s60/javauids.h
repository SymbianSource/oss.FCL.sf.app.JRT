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
* Description:  Common header for OMJ S60 UIDs. All
*                components should use the S60 UID constants defined here.
*
*/


#include <javadomainpskeys.h>

#ifndef JAVAUIDS_H
#define JAVAUIDS_H


// Note that many of the ECOM relates UIDs are needed also in OMJ S60 version
// because OMJ eclipses many of the ECOM plugin dlls. Thats why they have been
// moved to this OMJ section of the header file.

// Symbian Uid for MIDlet application type
#define KMidletApplicationTypeUid 0x10210E26

// Secure id for the OMJ midp
//#define KJavaMidpSecureId 0x200211D9
#define KJavaMidpSecureId 0x102033E6 //Using legacy secureId

// javacaptain.exe S60 Uid
#define KJavaCaptainUid 0x200211DC

// javainstaller.exe S60 Uid
#define KJavaInstallerUid 0x2001843A

// This Uid is not used any more by preinstaller. Can be taken into use
// by some other component if needed.
// javapreinstaller.exe S60 Uid
// #define KJavaPreinstallerUid 0x2001E261

// javalauncher.exe (old stubmidp2recogexe) - Symbian AppArc starts
// this 'launcher' application to start Java applications
#define KJavaLauncherUid 0x2001E262

// javacleaner.exe is executed automatically if OMJ is uninstalled.
// It restores AppArc non native app type registrations so that
// Java applications are launched by old S60 stubmidp2recogexe
#define KOmjCleanerUid 0x2002119A

// javaappconverter.exe is executed automatically once after OMJ has
// been installed as a SIS package. It uses javainstaller to convert 
// S60 MIDlets to OMJ
#define KJavaAppConverterUid 0x2002121C

// javaafterflashconverter.exe is executed by Java Captain after 
// ROM image that contains OMJ has been flashed to a device that 
// earlier had Java 1.x. It uses javainstaller to convert 
// S60 MIDlets to OMJ
#define KJavaAfterFlashConverterUid 0x20034612

// javarestoreconverter.exe is executed in if old Java 1.X MIDlet
// have been restored and must be converted and installed to OMJ environment
#define KJavaRestoreConverterUid 0x2002B3EA

// javapostupdater.exe is executed automatically once after OMJ has
// been installed. It finalizes OMJ installation.
#define KJavaPostUpdaterUid 0x20022EF6

// javamidp.exe - midp starter for DRM capable MIDlets
//#define KJavaMidp 0x200211DA
#define KJavaMidp 0x102033E6 //Using legacy secureId

// javamidpdrm.exe - midp starter for DRM capable MIDlets
//#define KJavaMidpDrm 0x200211DB

// tckrunner.exe S60 Uid
#define KJavaTckRunner 0x2002121E


// ifeui.dll S60 uid (InstallationFrontEnd dll) - ECOM plugin for S60 AppInstUI
// that handles Java installations by starting OMJ Java Installer executable
#define KInstallerFrontEndDllUid 0x101F875F
// InstallationFrontEnd ECOM Interface
#define KInstallerFrontEndEcomIfUid 0x101F875C
// InstallationFrontEnd ECOM implementation
#define KInstallerFrontEndEcomImplUid 0x101F8760

// javasifplugin.dll S60 uid - ECOM plugin for S60 USIF
// that handles Java installations by starting OMJ Java Installer executable
#define KJavaSifPluginDllUid 0x2002BC6F
// Sif Plugin ECOM Interface  // KUidSifPlugin
#define KSifPluginEcomIfUid 0x10285BC2
// Sif Plugin ECOM implementation
#define KJavaSifPluginEcomImplUid 0x2002BC70

// recjar.dll (Java Recognizer) - MIME type recognizer ECOM plugin for Symbian
// application framework for .jad and .jar
#define KJavaRecognizerDllUid 0x102031FB
// Java Recognizer ECOM interface
#define KRecognizerEcomIfUid 0x101F7D87
// Java Recognizer ECOM implementation
#define KRecognizerEcomImplUid 0x1000967A

// javasidchecker.dll (Java Sid Checker) - ECOM plugin for Symbian AppArc
// that tells AppArc whether an installed Java application is present
#define KJavaSidCheckerDllUid 0x10281FBE
// Java Sid Checker ECOM interface
#define KSidCheckerEcomIfUid 0x20007D8C
// Java Sid Checker ECOM implementation
#define KSidCheckerEcomImplUid 0x10281FBF

// javaappschemeplugin.dll ('javaapp:' Scheme handler) - ECOM plugin for
// Scheme Handler that handles 'javaapp:' URI scheme by starting
// the specified midlet
#define KJavaAppHandlerDllUid 0x2002875F
// javaapp: handler ECOM Interface
#define KJavaAppHandlerEcomIfUid 0x101f8534
// javaapp: handler ECOM implementation
#define KJavaAppHandlerEcomImplUid 0x20028767


// Java Registry Client Domain API - javaregistryclient.dll
#define KRegistryClientUid 0x10282476
// Java Registry Legacy - installedappsregistry.dll
#define KRegistryInstAppLegacyUid 0x101f8a24
// Java Registry Legacy - javaregistry.dll
#define KRegistryLegacyUid 0x101Faaaa

// Java Registry Server - javaregistry.exe
#define KRegistryServerUid 0x10282475


// Java Backup Core - javabackup.exe
#define KBackupCoreUid 0x1028246F
// Java Backup ECOM interface
#define KBackupEcomIfUid 0x10282470
// Java Backup ECOM implementation
#define KBackupEcomImplUid 0x10282471
// Java Backup MIDP2 dll - midp2backupplugin.dll
#define KBackupMidp2DllUid 0x10282474


// Native application installer
#define KAppInstUiUid 0x101F875A
// When .sis package is installed, sis launcher starts
// the processes executed during installation, for example javacleaner.exe
#define KAppSisLauncherUid 0x1020473F


// Random Uid from an illegal Uid range. No real Symbian application
// may have this Uid so it can be used e.g. in dummy uninstall requests.
#define KRandomIllegalAppUid 0xC06E8321


#endif // JAVAUIDS_H
