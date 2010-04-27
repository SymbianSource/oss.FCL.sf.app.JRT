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
* Description:  Java Install Launcher, ECom implementation of SWInstUIPluginAPI.
*   Will be executed in native installer process.
*/



#ifndef JAVAINSTALLLAUNCHER_H
#define JAVAINSTALLLAUNCHER_H

#include <e32base.h>

#include "f32file.h"
#include "CUIDialogs.h"
#include "SWInstUIPluginAPI.h"
#include "asyncwaitcallback.h"

class CEikonEnv;

namespace Java
{
namespace Installer
{

/**
 * This class implements the ECom SWInstUIPluginAPI interface for midlet
 * installation for OMJ.
 *
 * @lib IfeUi
 * @since S60 v3.2
 */
NONSHARABLE_CLASS(CJavaInstallLauncher) : public SwiUI::CSWInstUIPluginAPI,
        public SwiUI::CommonUI::MCUIWaitDialogCallback
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     * @since S60 v3.2
     */
    static CJavaInstallLauncher* NewL();

    /**
     * Destructor.
     * @since S60 v3.2
     */
    virtual ~CJavaInstallLauncher();

public:  // From base classes

    // From base class CSWInstUIPluginAPI

    /**
     * From CSWInstUIPluginAPI, Perform installation.
     * @since 3.0
     * @param[in] aFile - Handle to the installation package.
     *   If the file is in the private data cage of the caller or in some other
     *   place that can be accessed only with AllFiles capability it must be
     *   copied to the private data cage of Java Installer and the sharing mode of
     *   the handle is changed to EFileShareReadersOnly.
     *   Otherwise the copy operation would fail.
     * @param[in] aInstallParams - Object containing the installation parameters.
     * @param[in] aIsDRM - Indicates if installation pkg has DRM.
     * @param[out] aStatus -  asynchronous call status indicator,
     *   possible error codes returned through this
     */
    virtual void InstallL(
        RFile& aFile,
        SwiUI::TInstallReq& aInstallParams,
        TBool aIsDRM,
        TRequestStatus& aStatus);

    /**
     * From CSWInstUIPluginAPI, Perform silent installation.
     * @since 3.2
     * @param[in] aFile - Handle to the installation package.
     *   If the file is in the private data cage of the caller or in some other
     *   place that can be accessed only with AllFiles capability it must be
     *   copied to the private data cage of Java Installer and the sharing mode of
     *   the handle is changed to EFileShareReadersOnly.
     *   Otherwise the copy operation would fail.
     * @param[in] aInstallParams - Object containing the installation parameters.
     * @param[in] aIsDRM - Indicates if installation pkg has DRM.
     * @param[in] aOptions - Values for installation options.
     * @param[out] aStatus -  asynchronous call status indicator,
     *   possible error codes returned through this
     */
    virtual void SilentInstallL(
        RFile& aFile,
        TInstallReq& aInstallParams,
        TBool aIsDRM,
        SwiUI::TInstallOptions& aOptions,
        TRequestStatus& aStatus);

    /**
     * From CSWInstUIPluginAPI, Perform uninstallation.
     * @since 3.0
     * @param[in] aUid - Uid of the package to be uninstalled.
     * @param[in] aMIME - Mime type of the package.
     * @param[out] aStatus -  asynchronous call status indicator,
     *   possible error codes returned through this
     */
    virtual void UninstallL(
        const TUid& aUid,
        const TDesC8& aMIME,
        TRequestStatus& aStatus);

    /**
     * From CSWInstUIPluginAPI, Perform silent uninstallation.
     * @since 3.2
     * @param aUid[in] - Uid of the package to be uninstalled.
     * @param aMIME[in] - Mime type of the package.
     * @param aOptions[in] - Values for uninstallation options.
     * @param[out] aStatus -  asynchronous call status indicator,
     *   possible error codes returned through this
     */
    virtual void SilentUninstallL(
        const TUid& aUid,
        const TDesC8& aMIME,
        SwiUI::TUninstallOptions& aOptions,
        TRequestStatus& aStatus);

    /**
     * From CSWInstUIPluginAPI, Indicates if the application shell must
     * be updated.
     * Obsolete since 3.1.
     * @deprecated
     * @since 3.0
     * @return ETrue, if yes, EFalse otherwise.
     */
    virtual TBool IsAppShellUpdate();

    /**
     * From CSWInstUIPluginAPI, Cancel the current operation.
     * @since 3.0
     */
    virtual void Cancel();

    /**
     * From CSWInstUIPluginAPI, Informs the plugin about completed
     * operation. Called when all tasks are completed.
     * @since 3.0
     */
    virtual void CompleteL();

    /**
     * From CSWInstUIPluginAPI, but not used
     * @since 3.0
     * @since 3.0
     * @param[in] aOperation - The uninstall operation to be performed
     * @param[in] aParams - Parameters for the operation

     */
    virtual CErrDetails* GetErrorDetails();

    /**
     * From CSWInstUIPluginAPI, Perform a custom uninstallation.
     * @since S60 v3.2
     * @param[in] aOperation - The uninstall operation to be performed
     * @param[in] aParams - Parameters for the operation
     * @param[out] aStatus -  asynchronous call status indicator,
     *   possible error codes returned through this
     */
    virtual void CustomUninstallL(
        TOperation aOperation,
        const TDesC8& aParams,
        TRequestStatus& aStatus);

    /**
     * From CSWInstUIPluginAPI, Perform a silent custom uninstallation.
     * @since 3.0
     * @param[in] aOperation - The uninstall operation to be performed
     * @param[in] aParams - Parameters for the operation
     * @param[out] aStatus -  asynchronous call status indicator,
     *   possible error codes returned through this
     */
    virtual void SilentCustomUninstallL(
        TOperation aOperation,
        SwiUI::TUninstallOptions& aOptions,
        const TDesC8& aParams,
        TRequestStatus& aStatus);

    // From MCUIWaitDialogCallback

    /**
     * This is called when the dialog is about to be closed.
     * @since 3.0
     * @param aButtonId - Id of the button, which was used to cancel the dialog.
     * @return ETrue, if it's ok to close the dialog, EFalse otherwise.
     */
    TBool HandleDialogExitL(TInt aButtonId);

    /**
     * Hides preparing installation dialog.
     */
    void HidePreparingInstallationDialogL();

private:

    /**
     * Constructor.
     * @since S60 v3.2
     */
    CJavaInstallLauncher();

    /**
     * 2nd phase constructor.
     * @since S60 v3.2
     */
    void ConstructL();

    /**
     * Non-leaving function to convert 8bit to 16bit by padding the data.
     * Returns null if conversion fails or parameter is empty string
     *
     * @param[in] aSrc - the 7 bit US-ASCII string to be converted
     * @return HBufC16 pointer or null
     */
    HBufC16* Convert(const TDesC8& aSrc);


    /**
     * Opens UI resource file.
     */
    void OpenUiResourcesL();

    /**
     * Shows preparing installation dialog.
     */
    void ShowPreparingInstallationDialogL();

    /**
     * Java Installer does not have AllFiles capability.
     * So if the .jad/.jar file is in the private data cage of
     * Remove Application Management system, it must be copied
     * to the private data cage of Java Installer in this
     * plugin which is executed in native installer which has
     * AllFiles capability.
     *
     * @param[in][out]  aFileName  The file to installed. The file
     *  will be copied to tmp directory in Java Installer data cage
     *  if needed. In this case aFileName is changed also.
     * @param[in][out] aFile The original open file handle to the file
     *  to be installed. If the file must be copied, this handle
     *  is closed. Otherwise the copy operation would fail.
     */
    void CopyFilesIfNeededL(TFileName &aFileName, RFile& aFile);


private: //  Data

    TRequestStatus* iStatus;
    RFs iRFs;

    // Data members for wait dialog
    CEikonEnv* iEik;
    TInt iResourceFileOffset;
    SwiUI::CommonUI::CCUIDialogs* iCommonDialogs;
    CAsyncWaitCallBack* iWaitToHideDialog;

    RArray<RProcess> iHandlesToClose;

};

} // Installer
} // Java

#endif // JAVAINSTALLLAUNCHER_H

// End of File
