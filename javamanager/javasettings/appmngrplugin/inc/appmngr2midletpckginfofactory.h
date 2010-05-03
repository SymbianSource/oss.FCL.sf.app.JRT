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
* Description:  Midlet PackageInfo factory
*
*/


#ifndef APPMNGR2MIDLETPCKGINFOFACTORY_H
#define APPMNGR2MIDLETPCKGINFOFACTORY_H

#include "appmngr2midletpackageinfo.h"  // CAppMngr2MidletPackageInfo
#include "jcfjadjarmatcher.h"           // CJcfJadJarMatcher

class CAppMngr2Runtime;
class CAppMngr2MidletUtils;
class CAppMngr2RecognizedFile;
class CAppMngr2MidletResourceHandler;

/**
 *  Midlet PackageInfo factory implementation
 *
 *  This class matches provided JAD and JAR files to array
 *  of PackageInfo objects.
 *
 *  @lib appmngr2midletplugin.dll
 *  @since S60 v9.1
 */
class CAppMngr2MidletPckgInfoFactory : public CActive,
        public MJcfJadJarMatcherObserver
{
public:
    /**
     * Two phase constructor
     *
     * @since S60 9.1
     * @param aRuntime Runtime
     * @param aFs Handle to a file server session
     * @return instance
     */
    static CAppMngr2MidletPckgInfoFactory* NewL(CAppMngr2Runtime& aRuntime, RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Destructor
     *
     * @since S60 9.1
     */
    ~CAppMngr2MidletPckgInfoFactory();

    /**
     * Compose array of PackageInfo object of installable
     * packages.
     *
     * @since S60 9.1
     * @param aPackageInfo Returned array of installable packages
     * @param aFileList Array of recognized files
     * @param aStatus The request status. KErrNone on completion,
     *                otherwise another of the system-wide error codes.
     *
     */
    void GetInstallationFilesL(RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
                               const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
                               TRequestStatus& aStatus);

    /**
     * Cancel pending asynchronous GetInstallationFilesL() request.
     *
     * @since S60 9.1
     */
    void CancelGetInstallationFiles();

public: // MJcfJadJarMatcherObserver
    /**
     * Callback from CJcfJadJarMatcher
     */
    void Update(TInt aArg1);

protected: // from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    /**
     * Constructor
     *
     * @since S60 9.1
     * @param aRuntime Runtime
     * @param aFs Handle to a file server session
     */
    CAppMngr2MidletPckgInfoFactory(CAppMngr2Runtime& aRuntime, RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Second phase constructor
     *
     * @since S60 9.1
     */
    void ConstructL();

    /**
     * Creates PackageInfo object of installable package and
     * append it to array of inst pckgs.
     *
     * @since S60 9.1
     * @param aFileName File name to installable package
     * @param aDataType Corresponding data type
     * @param aStatus Request statys
     */
    void GetInstallationFileL(const TDesC& aFileName,
                              const TDataType& aDataType);

    /**
     * Completes asynchronous request
     *
     * @since S60 9.1
     * @param aRequestStatus Request request
     * @param aReason completation reason
     */
    void CompleteRequest(TRequestStatus& aRequestStatus, TInt aStatus);

    /**
     * Match JAD and JAR file
     *
     * @since S60 9.1
     * @param aMatchingFileName Path file name to matching JAD or JAR file.
     * @param aMatchingMimeType - MIME type of matching JAD or JAR file
     * @param aMatchedFileName - Path file name of the matched JAR or JAD file
     * @param aStatus The request status. KErrNone on completion,
     *                otherwise another of the system-wide error codes.
     */
    void MatchL(const TDesC& aMatchingFileName,
                const TDesC8& aMatchingMimeType,
                TDes& aMatchedFileName,
                TRequestStatus &aStatus);

    /**
     * Cancel matching JAD or JAR file
     * The Jad Jar Matcher API does not provide cancel method.
     * The cancelation is provided by deletation of matcher instance.
     *
     * @since S60 9.1
     */
    void CancelMatching();

private:    // data

    /**
     * Handle to a file server session
     * Not owned
     */
    RFs& iFs;

    /**
     * Path to JAD file
     */
    TFileName iJadFile;

    /**
     * Path to JAR file
     */
    TFileName iJarFile;

    /**
     * Parsed installation files
     */
    TInt iCounter;

    /**
     * Next state
     */
    TInt iState;

    /**
     * External request status
     * Not owned
     */
    TRequestStatus* iExtStatus;

    /**
     * Array of installation packages
     * Not owned
     */
    RPointerArray<CAppMngr2PackageInfo>* iPackageInfos;

    /**
     * Array of recognized installation packages
     * Not owned
     */
    const RPointerArray<CAppMngr2RecognizedFile>* iFileList; // not own

    /**
     * Runtime
     * Not owned
     */
    CAppMngr2Runtime& iRuntime;

    /**
     * JAD JAR Matcher
     * Owned
     */
    CJcfJadJarMatcher* iMatcher;

    /**
     * State machine states
     */
    enum TMatchingStates
    {
        ENextPckg = 0,
        EMatchedJar,
        EMatchedJad,
        EAppendPckg
    };
    CAppMngr2MidletResourceHandler& iResourceHandler;
};

#endif // APPMNGR2MIDLETPCKGINFOFACTORY_H
