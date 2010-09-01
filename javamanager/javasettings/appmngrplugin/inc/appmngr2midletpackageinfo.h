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
* Description:  MIDlet suite Package Info represents installable JAR/JAD files
*
*/


#ifndef APPMNGR2MIDLETPACKAGEINFO_H
#define APPMNGR2MIDLETPACKAGEINFO_H

#include <appmngr2packageinfo.h>        // CAppMngr2PackageInfo
#include <SWInstApi.h>                  // SwiUI::RSWInstLauncher
#include <downloadedcontenthandler.h>   // MDownloadedContentHandler
#include "appmngr2midletinfo.h"         // MAppMngr2MidletInfo

class CAppMngr2MidletInfoIterator;
class CAppMngr2MidletResourceHandler;

void CleanUpResetAndDestroy(TAny *aArray);

/**
 * MIDlet Suite Installation Package Info
 *
 * MIDlet Suite Installation Package Info represents installable
 * MIDlet suite packages (JAD/JAR files).
 * It handles installation, removal, sending and shows details of
 * installable JAD/JAR files.
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.1
 */
class CAppMngr2MidletPackageInfo : public CAppMngr2PackageInfo,
        public MAppMngr2MidletInfo,
        public MDownloadedContentHandler
{
public:     // constructor and destructor

    /**
     * Two phase constructor
     *
     * @since S60 9.1
     * @return instance
     * @param aRuntime Runtime
     * @param aJadFileName MIDlet suite JAD file name, KNullDesC if not available
     * @param aJarFileName MIDlet suite JAR file name, KNullDesC if not available
     * @param aFs Handle to a file server session
     */
    static CAppMngr2MidletPackageInfo* NewL(
        CAppMngr2Runtime& aRuntime,
        const TDesC& aJadFileName,
        const TDesC& aJarFileName,
        RFs& aFs,
        CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Two phase constructor
     *
     * @since S60 9.1
     * @return instance
     * @param aRuntime Runtime
     * @param aJadFileName MIDlet suite JAD file name, KNullDesC if not available
     * @param aJarFileName MIDlet suite JAR file name, KNullDesC if not available
     * @param aFs Handle to a file server session
     */
    static CAppMngr2MidletPackageInfo* NewLC(
        CAppMngr2Runtime& aRuntime,
        const TDesC& aJadFileName,
        const TDesC& aJarFileName,
        RFs& aFs,
        CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Destructor
     *
     * @since S60 9.1
     */
    ~CAppMngr2MidletPackageInfo();

public:     // from CAppMngr2InfoBase

    /**
     * Icon index for the installed application or installation package.
     * Returns icon index referring to the icon array loaded in function
     * CAppMngr2Runtime::LoadIconsL(). If an item has a specific icon that
     * no other items use, return EAppMngr2UseSpecificIcon and load the
     * specific icon via CAppMngr2InfoBase::SpecificIconL().
     * @return TInt  Index to the icon array
     */
    TInt IconIndex() const;

    /**
     * Name of the list item, first line
     * @return const TDesC&  Displayable name
     */
    const TDesC& Name() const;

    /**
     * Additional info, second line (usually size)
     * @returns const TDesC&  Displayable additional information line
     */
    const TDesC& Details() const;

    /**
     * Indicator icon index. Returns indexes to the icon array returned by
     * CAppMngr2Runtime::LoadIconsL(). Also default icon indexes defined in
     * AppMngr2Common.hrh (TAppMngr2IconIndex) can be used. Return
     * EAppMngr2NoIndicatorIcon if no indicator icon should be displayed, or
     * EAppMngr2UseSpecificIcon if item specific indicator icon is returned
     * via CAppMngr2InfoBase::SpecificIndicatorIconL().
     * @return TInt  Index to the icon array, or default icon number
     */
    TInt IndicatorIconIndex() const;

    /**
     * Defines item specific menu items.
     * Empty default implementation is used, if plug-in does not override it.
     * Menu commands are run via CAppMngr2InfoBase::HandleCommandL()
     * @param aMenuCmds Array where plug-in specific menu items are added
     */
    void GetMenuItemsL(RPointerArray<CEikMenuPaneItem::SData>& aMenuCmds);

    /**
     * Can be used to enable generic menu commands. Generic menu commands are
     * defined in TAppMngr2GenericCommands enum in AppMngr2Common.hrh. Generic
     * commands are hidden from the menu if the currently selected item does not
     * support them. Return ETrue for those command ids that are supported by
     * this item.
     * @param aCmdId Generic command id
     * @return TBool ETrue if command aCmdId is supported
     */
    TBool SupportsGenericCommand(TInt aCmdId);

    /**
     * Starts user invoked commands asynchronously. Command id may be a generic
     * command id or a plug-in specific command id. Generic command ids are defined
     * inAppMngr2Common.hrh (see TAppMngr2GenericCommand enum). Plug-in specific
     * commands are defined via functions CAppMngr2InfoBase::GetMenuItemsL() and
     * CAppMngr2InfoBase::GetMiddleSoftkeyCommandL(). If the command is synchronous,
     * the plug-in must complete the aStatus by calling User::RequestComplete().
     * @param aCommandId  Command id to be run
     * @param aStatus  Active object request status
     */
    void HandleCommandL(TInt aCommandId, TRequestStatus& aStatus);

    /**
     * If CAppMngr2InfoBase::HandleCommandResultL() started an asynchronous operation,
     * this function is called when the operation completes.
     * @param aStatus  Completion code, KErrNone or some error code
     */
    void HandleCommandResultL(TInt aStatus);

    /**
     * Cancels asynchronously started HandleCommandL() command. This function
     * may be called any time when an asynchronous command is running and it
     * must cancel the running command as quickly as possible.
     */
    void CancelCommand();

public:      // from MDownloadContentHandler for browser integration
    /**
     * Handle downloaded content that has been saved to a file,
     * and return ETrue if it was handled successfully.
     *
     * @since S60 9.1
     * @param aFileName File name with path.
     * @param aParamList Generic parameter list.
     * @param aContinue Output parameter: It must be ETrue if the caller wants
     *                  Browser not to terminate after the content is handled.
     * @return ETrue if the implementer has processed the content. If it is
     *                  EFalse, BrowserLauncher opens that content in an
     *                  appropriate viewer application.
     */
    TBool HandleContentL(const TDesC& aFileName,
                         const CAiwGenericParamList& aParamList,
                         TBool& aContinue);

    /**
     * From MDownloadContentHandler for browser integration
     * @since S60 9.1
     * reserved for future use
     */
    TAny* DownloadedContentHandlerReserved1(TAny* /*aAnyParam*/);

public:     // from MAppMngr2MidletInfo
    /**
     * Path to jad file
     *
     * @since S60 9.1
     * @return Path to jad file, return KNullDesC if not available
     */
    const TDesC& JadFileName() const;

    /**
     * Path to jar file
     *
     * @since S60 9.1
     * @return Path to jar file, return KNullDesC if not available
     */
    const TDesC& JarFileName() const;

    /**
     * Package MIME type
     *
     * @since S60 9.1
     * @return Package MIME type
     */
    const TDesC8& MimeType() const;

public:     // new functions
    /**
     * Path to package file (Jar if no Jad file, else Jad)
     *
     * @since S60 9.1
     * @return path to package file
     */
    const TDesC& FileName() const;

    /**
     * Return true if package is DRM protected
     *
     * @since S60 9.1
     * @return package is DRM protected
     */
    TBool IsDRMProtected() const;

    /**
     * DRM protected package file path
     *
     * @since S60 9.1
     * @return Path to protected package if available, else return KNullDesC
     */
    const TDesC& ProtectedFileName() const;


private:    // new functions

    /**
     * Constructor
     *
     * @since S60 9.1
     * @param aRuntime Runtime
     * @param aFs Handle to a file server session
     */
    CAppMngr2MidletPackageInfo(CAppMngr2Runtime& aRuntime, RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Second phase constructor
     *
     * @since S60 9.1
     * @param aJadFileName MIDlet suite JAD file name, KNullDesC if not available
     * @param aJarFileName MIDlet suite JAR file name, KNullDesC if not available
     */
    void ConstructL(const TDesC& aJadFileName, const TDesC& aJarFileName);

    /**
     * Removes MIDlet suite installation package
     *
     * @since S60 9.1
     */
    void RemoveL();

    /**
     * Installs MIDlet suite installation package
     *
     * @since S60 9.1
     * @param aStatus The request status. KErrNone on completion,
     *                otherwise another of the system-wide error codes.
     */
    void InstallL(TRequestStatus& aStatus);

    /**
     * Cancel MIDlet suite installation package installation
     *
     * @since S60 9.1
     */
    void CancelInstallationL();

    /**
     * Show MIDlet suite installation package details
     *
     * @since S60 9.1
     */
    void ShowDetailsL();

    /**
     * Send MIDlet suite installation package
     *
     * @since S60 9.1
     */
    void SendL();

    /**
     * Starts the browser with midlet's MIDLET-Info-URL parameter
     *
     * @since S60 9.1
     * @param aStatus The request status. KErrNone on completion,
     *                otherwise another of the system-wide error codes.
     */
    void ShowInfoUrlL();

    /**
     * Return Info Iterator which implements MCUIInfoIterator API for
     * CCUIDetailsDialog
     *
     * @since S60 9.1
     * @return Info iterator object
     */
    CAppMngr2MidletInfoIterator& InfoIteratorL();

private:    // data

    /**
     * MIDlet suite is DRM protected
     */
    TBool iIsDRMProtected;

    /**
     * MIDlet suite name
     * Owned
     */
    HBufC* iName;

    /**
     * MIDlet suite details
     * Owned
     */
    HBufC* iDetails;

    /**
     * MIDlet suite file name (jad or jar)
     * Owned
     */
    HBufC* iFileName;

    /**
     * MIDlet suite JAD file name
     * Owned
     */
    HBufC* iJadFileName;

    /**
     * MIDlet suite JAR
     * Owned
     */
    HBufC* iJarFileName;

    /**
     * MIDlet suite MIME type
     * Owned
     */
    HBufC8* iMimeType;

    /**
     * Installer launcher
     * Owned
     */
    SwiUI::RSWInstLauncher* iSWInstLauncher;

    /**
     * Info iterator
     * Owned
     */
    CAppMngr2MidletInfoIterator* iInfoIterator;

    CAppMngr2MidletResourceHandler& iResourceHandler;
};

#endif // APPMNGR2MIDLETPACKAGEINFO_H

