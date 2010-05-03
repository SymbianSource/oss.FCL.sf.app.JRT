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
* Description:  Java MIDlet runtime type for AppMngr2
*
*/


#ifndef APPMNGR2MIDLETRUNTIME_H
#define APPMNGR2MIDLETRUNTIME_H

#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <javaregistryincludes.h>

class MAppMngr2RuntimeObserver;
class CAppMngr2MidletPckgInfoFactory;
class CAppMngr2RecognizedFile;
class CAppMngr2PackageInfo;
class CAknIconArray;
class CAppMngr2MidletResourceHandler;

/**
 *  ECom interface UID for AppMngr2 MIDlet Runtime Plugin
 */
const TInt KAppMngr2MidletUidValue = 0x20016BF8;
const TUid KAppMngr2MidletUid = { KAppMngr2MidletUidValue };

/**
 * MIDlet Runtime Plug-in
 *
 * The MIDlet Runtime Plug-in provides to AppMngr:
 * - Array of MIDlet suite Installation Packages Info objects
 *   based on provided file name list.
 * - Array of installed MIDlets Application Info objects.
 * - MIDlet default icons
 * - Data types supported for creating installation
 * - Additional directories for installation file scanning.
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.1
 */
class CAppMngr2MidletRuntime : public CAppMngr2Runtime
{
public:     // constructor and destructor

    /**
     * Two phase constructor
     * @param aObserver Observer implementing MAppMngr2RuntimeObserver functions
     *
     * @since S60 9.1
     * @return instance
     */
    static CAppMngr2MidletRuntime* NewL(MAppMngr2RuntimeObserver& aObserver);

    /**
     * Destructor
     *
     * @since S60 9.1
     */
    ~CAppMngr2MidletRuntime();

public:     // from CAppMngr2Runtime

    /**
     * Load plug-in specific icons. Functions CAppMngr2InfoBase::IconIndex()
     * and CAppMngr2InfoBase::IndicatorIconIndex() must return indexes to the
     * plug-in specific icons appended to aIconArray.
     * @param aIconArray  Array where to append the loaded icons
     */
    void LoadIconsL(CAknIconArray& aIconArray);

    /**
     * Get data types that this Runtime plug-in supports for creating installation
     * file (CAppMngr2PackageInfo) objects. When installation files are scanned,
     * recognized files are provided to the plug-in via GetPackageInfosL() function.
     * @param aDataTypeArray  Array where to append supported data types
     */
    void GetSupportedDataTypesL(CDataTypeArray& aDataTypeArray);

    /**
     * Create package info objects to represent installation files in "Installation
     * Files" view. Each package info object is displayed as a separate item in the
     * "Installation Files" view. Package info objects should be based on recognized
     * installation files, provided in aFileList array. All data types match to the
     * ones defined in function CAppMngr2Runtime::GetSupportedDataTypesL().
     * @param aPackageInfos  Array where to append package info objects
     * @param aFileList  List of recognized files
     * @param aFsSession  File server session
     * @param aStatus  The request status. KErrNone on completion,
     *                otherwise another of the system-wide error codes.
     */
    void GetInstallationFilesL(RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
                               const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
                               RFs& aFsSession,
                               TRequestStatus& aStatus);

    /**
     * Cancel pending asynchronous GetInstallationFilesL() request.
     */
    void CancelGetInstallationFiles();

    /**
     * Create application info objects to represent installed applications in
     * "Installed" view. Each application info object is displayed as separate
     * item in "Installed" view.
     * @param aApps  Array where to add application info objects
     * @param aFsSession  File server session
     * @param aStatus  Request status for the asynchronous request
     */
    void GetInstalledAppsL(RPointerArray<CAppMngr2AppInfo>& aApps,
                           RFs& aFsSession, TRequestStatus& aStatus);

    /**
     * Cancel pending asynchronous GetInstalledAppsL() request.
     */
    void CancelGetInstalledApps();


private:    // new functions

    /**
     * Constructor
     * @param aObserver Observer implementing MAppMngr2RuntimeObserver functions
     *
     * @since S60 9.1
     */
    CAppMngr2MidletRuntime(MAppMngr2RuntimeObserver &aObserver);

    /**
     * Second phase constructor
     *
     * @since S60 9.1
     */
    void ConstructL();

    /**
     * Create Installed MIDlet suite application info object.
     * @param aApps  Array where to add application info object
     * @param aFsSession  File server session
     * @param aEntry  Java registry MIDlet suite entry
     */
    void GetInstalledAppL(RPointerArray<CAppMngr2AppInfo>& aApps,
                          RFs& aFsSession,
                          Java::CJavaRegistryPackageEntry* aEntry);

private:    // data

    /**
     * Java Registry for getting installed Java applications
     * Owned
     */
    Java::CJavaRegistry* iJavaRegistry;

    /**
     * Package Info factory for getting installation packages
     * Owned
     */
    CAppMngr2MidletPckgInfoFactory* iPckgFactory;

    /**
     * Resource handler
     * Owned
     */
    CAppMngr2MidletResourceHandler* iResourceHandler;
};

#endif // APPMNGR2MIDLETRUNTIME_H

