/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java Sif plugin, ECom implementation of KUidSifPlugin.
*   Starts Java Installer to handle install, uninstall, get component info
*   requests.
*/



#ifndef JAVASIFPLUGIN_H
#define JAVASIFPLUGIN_H

#include <e32base.h>
#include <usif/sif/sifplugin.h>

#include "f32file.h"

class ResultsServer;

using namespace Usif;

/**
 * The messages known by JavaSifPlugin.
 */

const int INSTALLER_CANCEL_MESSAGE_ID = 603;
const int INSTALLER_CANCEL_RESPONSE_MESSAGE_ID = 604;

namespace java
{
namespace installer
{

/**
 * This class implements the ECom KUidSifPlugin interface for midlet
 * installation for OMJ.
 */
NONSHARABLE_CLASS(CJavaSifPlugin) : public Usif::CSifPlugin
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CJavaSifPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CJavaSifPlugin();

public:  // From base classes

    // From base class CSifPlugin

    /**
     * Returns details of a component to be installed by file name.
     *
     * @param aFileName         Component's file name
     * @param aSecurityContext  this param allows the plug-in to verify client's identity
     * @param aComponentInfo    On return, contains component's details
     * @param aStatus           Asynchronous completion status
     */
    virtual void GetComponentInfo(
        const TDesC& aFileName,
        const TSecurityContext& aSecurityContext,
        CComponentInfo& aComponentInfo,
        TRequestStatus& aStatus);

    /**
     * Returns details of a component to be installed by file handle.
     *
     * @param aFileHandle Component's file name
     * @param aSecurityContext this param allows the plug-in to verify client's identity
     * @param aComponentInfo On return, contains component's details
     * @param aStatus Asynchronous completion status
     */
    virtual void GetComponentInfo(
        RFile& aFileHandle,
        const TSecurityContext& aSecurityContext,
        CComponentInfo& aComponentInfo,
        TRequestStatus& aStatus);

    /**
     * Installs a component by file name.
     *
     * @param aFileName         Component's file name
     * @param aSecurityContext  this param allows the plug-in to verify client's identity
     * @param aArguments        The array of opaque params for the plug-in. An empty
     *   array may be passed. The following param is defined for the "SCOMO Install
     *   Inactive" operation:
     *   Name: InstallInactive, Type: Int, Value: ETrue
     *   If a plug-in receives this param, it must install a component normally but the SCOMO
     *   State should remain EDeactivated.
     * @param aResults The array of opaque params returned from the plug-in.
     *   The following return param is defined for the id of an installed component:
     *   Name: ComponentId, Type: Int
     *   Each plug-in must add this param to aResults after a successful installation.
     *   For packages containing embedded components, the returned param contains
     *   the id of the last/most embedded component.
     *  @param aStatus Asynchronous completion status
     */
    virtual void Install(
        const TDesC& aFileName,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aArguments,
        COpaqueNamedParams& aResults,
        TRequestStatus& aStatus);

    /**
     *  Installs a component by file handle.
     *
     * @param aFileHandle Component's file handle
     * @param aSecurityContext this param allows the plug-in to verify client's identity
     * @param aArguments The array of opaque params for the plug-in. An empty
     *   array may be passed.  The following param is defined for the "SCOMO Install
     *   Inactive" operation:
     *   Name: InstallInactive, Type: Int, Value: ETrue
     *   If a plug-in receives this param, it must install a component normally but the SCOMO
     *   State should remain EDeactivated.
     * @param aResults The array of opaque params returned from the plug-in.
     *   The following return param is defined for the id of an installed component:
     *   Name: ComponentId, Type: Int
     *   Each plug-in must add this param to aResults after a successful installation.
     *   For packages containing embedded components, the returned param contains
     *   the id of the last/most embedded component.
     * @param aStatus Asynchronous completion status
     */
    virtual void Install(
        RFile& aFileHandle,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aArguments,
        COpaqueNamedParams& aResults,
        TRequestStatus& aStatus);

    /**
     *  Uninstalls a component.
     *
     * @param aComponentId The id of a component to be uninstalled
     * @param aSecurityContext this param allows the plug-in to verify client's identity
     * @param aArguments The array of opaque params for the plug-in. An empty
     *   array may be passed.
     * @param aResults The array of opaque results returned from the plug-in.
     * @param aStatus The request status. KErrNone, if successful, otherwise
     *   one of the other system-wide or SIF error codes defined in usiferror.h.
     */
    virtual void Uninstall(
        TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aArguments,
        COpaqueNamedParams& aResults,
        TRequestStatus& aStatus);

    /**
     * Activates a component.
     *
     * @param aComponentId The id of a component to be activated
     * @param aSecurityContext this param allows the plug-in to verify client's identity
     * @param aStatus The request status. KErrNone, if successful, otherwise
     *   one of the other system-wide or SIF error codes defined in usiferror.h.
     */
    virtual void Activate(
        TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        TRequestStatus& aStatus);

    /**
     * Deactivates a component.
     *
     * @param aComponentId The id of a component to be deactivated
     * @param aSecurityContext this param allows the plug-in to verify client's identity
     * @param aStatus The request status. KErrNone, if successful, otherwise
     *   one of the other system-wide or SIF error codes defined in usiferror.h.
     */
    virtual void Deactivate(
        TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        TRequestStatus& aStatus);

    /**
     *  Cancels an ongoing asynchronous request.
     *
     *  This is a synchronous call. When it returns the original asynchronous call is completed.
     */
    virtual void CancelOperation();


private:

    /**
     * Constructor.
     */
    CJavaSifPlugin();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Java Installer does not have AllFiles capability.
     * So if the .jad/.jar file is in the private data cage of
     * some other process, it must be copied
     * to the private data cage of Java Installer in this
     * plugin which is executed in a prorcess which hopefully has
     * AllFiles capability.
     *
     * @param[in][out]  aFileName  The file to installed. The file
     *  will be copied to tmp directory in Java Installer data cage
     *  if needed. In this case aFileName is changed also.
     */
    void CopyFilesIfNeededL(TFileName &aFileName);

    /**
     * Check if the file is Jad file. Argument KSifInParam_MimeType is
     * used for check if it exists.
     *
     * @param[in] aFileHandle the file to be checked
     * @param[in] aArguments Install request arguments
     * @param[out] aIsJad set to ETrue if the file is Jad file
     * @return KErrNone or Symbian error code
     */
    TInt IsJadFile(RFile& aFileHandle, const COpaqueNamedParams& aArguments, TBool& aIsJad);

    /**
     * Check if the file is Jad file.
     *
     * @param[in] aFileHandle the file to be checked
     * @param[out] aIsJad set to ETrue if the file is Jad file
     * @return KErrNone or Symbian error code
     */
    TInt IsJadFile(RFile& aFileHandle, TBool& aIsJad);

    /**
     * Uses the information in aArguments to create the correct command line for
     * Java Installer.
     *
     * @param[in][out] aCommandLine the buffer to be filled with command line arguments
     * @param[in]      aArguments The array of opaque params for the plug-in. An empty
     *   array may be passed.  The following param is defined for the "SCOMO Install
     *   Inactive" operation:
     *   Name: InstallInactive, Type: Int, Value: ETrue
     *   If a plug-in receives this param, it must install a component normally but the SCOMO
     *   State should remain EDeactivated.
     */
    void BuildInstallCommandLine(
        TPtr& aCommandLine,
        const COpaqueNamedParams& aArguments);

    /**
     * Returns the value of int param found from aArguments or -1 if
     * the param is not found. Must be called only for int params
     * that have positive value range!
     *
     * @param[in] aName The name of the integer param to be obtained.
     * @param[in] aArguments The array of opaque params for the plug-in.
     * @return -1 if the named param if not found, otherwise the value of the
     */
    TInt GetPositiveIntParam(
        const TDesC& aName,
        const COpaqueNamedParams& aArguments);

    /**
     * Start ResultsServer for receiving Comms message(s) from
     * Java Installer
     *
     * @param[in][out] aResults If Comms message contains installation or
     *  uninstallation operation results, they will be stored in this variable
     * @param[in][out] aComponentInfo If Comms message contains contains details of
     *  a component, they will be stored into this variable
     * @return
     */
    TInt StartResultsServer(
        COpaqueNamedParams& aResults,
        CComponentInfo& aComponentInfo);

    /**
     * If Java Installer is already running, set error category EInstallerBusy etc
     * to aResults, set aStatus to KErrAlreadyExists and return ETrue
     * @param[in][out] aResults
     * @param[in][out] aStatus
     * @return ETrue if Java Installer is running
     */
    TBool ExitIfJavaInstallerRunning(
        COpaqueNamedParams& aResults,
        TRequestStatus& aStatus);

private: //  Data

    RFs mRFs;
    RArray<RProcess> mHandlesToClose;
    ResultsServer*   mResultsServer;

    COpaqueNamedParams* mDummyResults;
    CComponentInfo*     mDummyInfo;
};

} // Installer
} // Java

#endif // JAVASIFPLUGIN_H
