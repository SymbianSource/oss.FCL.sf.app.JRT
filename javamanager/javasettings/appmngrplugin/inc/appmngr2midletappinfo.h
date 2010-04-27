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
* Description:  MidletAppInfo represents installed Java applications
*
*/


#ifndef APPMNGR2MIDLETAPPINFO_H
#define APPMNGR2MIDLETAPPINFO_H
#include <appmngr2appinfo.h>            // CAppMngr2AppInfo
#include <SWInstApi.h>                  // SwiUI::RSWInstLauncher
#include <appversion.h>                 // TAppVersion
#include <downloadedcontenthandler.h>   // MDownloadedContentHandler
#include <javaregistryincludes.h>
#include "appmngr2midletinfo.h"         // MAppMngr2MidletInfo
#include <string>

class CFbsBitmap;
class CAppMngr2MidletInfoIterator;
class CAiwGenericParamList;
class CAppMngr2MidletResourceHandler;

namespace SwiUI
{
namespace CommonUI
{
class MCUIInfoIterator;
class CCUICertificateInfo;
}
}

/**
 * MIDlet Suite Application Info
 *
 * MIDlet Suite Application Info handles uinstallation of installed MIDlet suite,
 * shows MIDlet suite details and MIDlet suite settings.
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.1
 */
class CAppMngr2MidletAppInfo : public CAppMngr2AppInfo,
        public MAppMngr2MidletInfo,
        public MDownloadedContentHandler

{
public:     // constructor and destructor

    /**
     * Two phase constructor
     *
     * @since S60 9.1
     * @param aRuntime Runtime
     * @param aEntry Java Registry Suite Entry. Ownership taken.
     * @param aFs Handle to a file server session
     * @return instance
     */
    static CAppMngr2MidletAppInfo* NewL(CAppMngr2Runtime& aRuntime,
                                        Java::CJavaRegistryPackageEntry* aEntry,
                                        RFs& aFs,
                                        CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Two phase constructor
     *
     * @since S60 9.1
     * @param aRuntime Runtime
     * @param aEntry Java Registry Suite Entry. Ownership taken.
     * @param aFs Handle to a file server session
     * @return instance
     */
    static CAppMngr2MidletAppInfo* NewLC(CAppMngr2Runtime& aRuntime,
                                         Java::CJavaRegistryPackageEntry* aEntry,
                                         RFs& aFs,
                                         CAppMngr2MidletResourceHandler& aResourceHandler);
    /**
     * Destructor
     *
     * @since S60 9.1
     */
    ~CAppMngr2MidletAppInfo();

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
     * Specific icon for the installed application or installation package.
     * CAppMngr2InfoBase::IconIndex() must return EAppMngr2UseSpecificIcon
     * to get CAppMngr2InfoBase::SpecificIconL() called. If the same icon is
     * used by many items, load it in CAppMngr2Runtime::LoadIconsL() and
     * use CAppMngr2InfoBase::IconIndex() to return icon indexes. This is
     * more efficient than loading the same icon bitmap many times.
     * @return CGulIcon  Item specific icon
     */
    CGulIcon* SpecificIconL() const;

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
     * If CAppMngr2InfoBase::HandleCommandL() started an asynchronous operation,
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

public:      // from MDownloadContentHandler
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
    // reserved for future use
    */
    TAny* DownloadedContentHandlerReserved1(TAny* /*aAnyParam*/);

public:      // from MAppMngr2MidletInfo
    /**
     * Path to jar file
     *
     * @since S60 9.1
     * @return Path to jar file
     */
    const TDesC& JarFileName() const;

    /**
     * Path to jad file
     *
     * @since S60 9.1
     * @return Path to jad file
     */
    const TDesC& JadFileName() const;

    /**
     * Installed application MIME type
     *
     * @since S60 9.1
     * @return Application MIME type
     */
    const TDesC8& MimeType() const;


public:     // new functions
    /**
     * Installed application version
     *
     * @since S60 9.1
     * @return Application version
     */
    const TDesC& Version() const;

    /**
     * Installed application vendor
     *
     * @since S60 9.1
     * @return Application vendor
     */
    const TDesC& Vendor() const;

    /**
     * Security domain category
     *
     * @since S60 9.1
     * @return Application security domain category
     */
    const std::wstring SecurityDomainCategory() const;

    /**
     * Security domain name
     *
     * @since S60 9.1
     * @return Application security domain name
     */
    const std::wstring SecurityDomainName() const;

    /**
     * Installed application data size
     *
     * @since S60 9.1
     * @return Application data size
     */
    const TDesC& DataSize() const;

    /**
     * Installed application description
     *
     * @since S60 9.1
     * @return Application description
     */
    const TDesC& Description() const;

    /**
     * Installed application info URL
     *
     * @since S60 9.1
     * @return Application info URL
     */
    const TDesC& InfoUrl() const;

    /**
     * Installed application update URL
     *
     * @since S60 9.1
     * @return Application update URL
     */
    const TDesC& UpdateUrl() const;

    /**
     * Installed application content
     *
     * @since S60 9.1
     * @return Application content
     */
    const TDesC& Content() const;

    /**
     * Installed application requested mandatory permissions
     *
     * @since S60 9.2
     * @return Permissions
     */
    const TDesC& Permissions() const;

protected:  // new functions

    /**
     * Constructor
     *
     * @since S60 9.1
     * @param aRuntime Runtime
     * @param aFs Handle to a file server session
     */
    CAppMngr2MidletAppInfo(CAppMngr2Runtime& aRuntime, RFs& aFsSession, CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Second phase constructor
     *
     * @since S60 9.1
     * @param aEntry Java Registry Suite Entry
     */
    void ConstructL(Java::CJavaRegistryPackageEntry* aEntry);

private:     // new functions

    /**
     * Uninstalls MIDlet suite package
     *
     * @since S60 9.1
     * @param aStatus The request status. KErrNone on completion,
     *                otherwise another of the system-wide error codes.
     */
    void UninstallL(TRequestStatus& aStatus);

    /**
    * Show installed MIDlet suite details
    *
    * @since S60 9.1
    */
    void ShowDetailsL();

    /**
    * Show installed MIDlet suite settings
    *
    * @since S60 9.1
    */
    void ShowSettingsL();

    /**
     * Send MIDlet suite installation package
     *
     * @since S60 9.1
     */
    void SendL();

    /**
     * Start update process
     *
     * @since S60 9.1
     */
    void UpdateL();

    /**
     * Starts the browser with midlet's MIDLET-Info-URL parameter
     *
     * @since S60 9.1
     */
    void ShowInfoUrlL();

    /**
     * Return Info Iterator
     *
     * @since S60 9.1
     * @return Info iterator object
     */
    CAppMngr2MidletInfoIterator& InfoIteratorL();

    /**
     * Reads MIDlet suite certificates info
     *
     * @since S60 9.1
     */
    void ReadCertificatesInfoL();


private:    // data

    /**
     * MIDlet suite entry.
     */
    Java::CJavaRegistryPackageEntry* iEntry;

    /**
     * MIDlet suite jar file path
     * Owned
     */
    HBufC* iJarPath;

    /**
     * MIDlet suite jad file path
     * Owned
     */
    HBufC* iJadPath;

    /**
     * MIDlet suite details
     * Owned
     */
    HBufC* iDetails;

    /**
     * MIDlet suite version
     * Owned
     */
    HBufC* iVersion;

    /**
     * MIDlet suite MIME type
     * Owned
     */
    HBufC8* iMimeType;

    /**
     * MIDlet suite data size
     * Owned
     */
    HBufC* iDataSize;

    /**
     * MIDlet suite description
     * Owned
     */
    HBufC* iDescription;

    /**
     * MIDlet suite info URL
     * Owned
     */
    HBufC* iInfoUrl;

    /**
     * MIDlet suite update URL
     * Owned
     */
    HBufC* iUpdateUrl;

    /**
     * Localized MIDlet suite name
     * Owned
     */
    HBufC* iLocalizedMIDletName;

    /**
     * MIDlet suite content
     * Owned
     */
    HBufC* iContent;

    /**
     * MIDlet suite mandatory permissions
     * Owned
     */
    HBufC* iPermissions;

    /**
     * MIDlet suite bitmap
     * Owned
     */
    CFbsBitmap* iAppBitmap;

    /**
     * MIDlet suite bitmap mask
     * Owned
     */
    CFbsBitmap* iAppMask;

    /**
     * MIDlet suite security domain category
     * Owned
     */
    std::wstring iSecurityDomainCategory;

    /**
     * MIDlet suite security domain name
     * Owned
     */
    std::wstring iSecurityDomainName;

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

    /**
     * Is DRM protected
     */
    TBool iIsDRMProtected;

    /**
     * Is DRM right object missing or expired
     */
    TBool iIsROMissingOrExpired;

    /**
     * Certificates info
     * Owned
     */
    RPointerArray<SwiUI::CommonUI::CCUICertificateInfo> iCertificates;

    /**
     * Certificates has been already read
     */
    TBool iCertsRead;
    CAppMngr2MidletResourceHandler& iResourceHandler;
};

#endif // APPMNGR2MIDLETAPPINFO_H

