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
* Description:  Implements MCUIInfoIterator for MIDlets to use CCUIDetailsDialog
*
*/


#ifndef APPMNGR2MIDLETINFOITERATOR_H
#define APPMNGR2MIDLETINFOITERATOR_H

#include <appmngr2infoiterator.h>       // CAppMngr2InfoIterator

#include <javaregistryincludes.h>
#include <string>

class CAppMngr2MidletAppInfo;
class CAppMngr2MidletPackageInfo;
class TAppVersion;
class CAppMngr2MidletResourceHandler;

/**
 * MIDlet suite Info Iterator
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.1
 */
class CAppMngr2MidletInfoIterator : public CAppMngr2InfoIterator
{
public:     // constructors and destructor
    /**
     * Constructor
     *
     * @since S60 9.1
     * @param aMidlet MIDlet suite package or app info
     * @param aInfoType Info type
     * @param aFs Handle to a file server session
     */
    static CAppMngr2MidletInfoIterator* NewL(CAppMngr2InfoBase& aMidlet,
            TAppMngr2InfoType aInfoType, RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Destructor
     *
     * @since S60 9.1
     */
    ~CAppMngr2MidletInfoIterator();

public:     // new functions

    /**
     * Get info URL
     *
     * @since S60 9.1
     */
    const TDesC& GetInfoUrlL();

    /**
     * Get update URL
     *
     * @since S60 9.1
     */
    const TDesC& GetUpdateUrlL();

protected:  // new functions

    /**
     * Second phase constructor
     *
     * @since S60 9.1
     */
    void ConstructL();

    /**
     * Set all fields
     *
     * @since S60 9.1
     */
    void SetAllFieldsL();

    /**
     * Set installed apps info
     *
     * @since S60 9.1
     * @param MIDlet suite app info
     */
    void SetInstalledAppsInfoL(CAppMngr2MidletAppInfo& aMidlet);

    /**
     * Set not installed app info
     *
     * @since S60 9.1
     * @param MIDlet suite package info
     */
    void SetNotInstalledAppsInfoL(CAppMngr2MidletPackageInfo& aPackage);

    /**
     * Sets status of installed or not installed midlet based on aInfoType
     * defined in constructor.
     *
     * @since S60 9.1
     */
    void SetStatusL();

private:    // new functions

    /**
     * Constructor
     *
     * @since S60 9.1
     * @param
     */
    CAppMngr2MidletInfoIterator(CAppMngr2InfoBase& aMidlet,
                                TAppMngr2InfoType aInfoType, RFs& aFs,
                                CAppMngr2MidletResourceHandler& aResourceHandler);

    /**
     * Set version
     *
     * @since S60 9.1
     * @param aVersion MIDlet suite version
     */
    void SetVersionL(const TDesC& aVersion);

    /**
     * Set vendor name
     *
     * @since S60 9.1
     * @param aVendor MIDlet suite vendor name
     */
    void SetVendorNameL(const TDesC& aMidletSuiteName);

    /**
     * Set domain
     *
     * @since S60 9.1
     * @param aSecurityDomainCategory MIDlet suite security
     *        domain category
     * @param aSecurityDomainName MIDlet suite security
     *        domain name
     */
    void SetDomainL(const std::wstring aSecurityDomainCategory, const std::wstring aSecurityDomainName);

    /**
     * Set User data size
     *
     * @since S60 9.1
     * @param aDataSize User data size
     */
    void SetDataL(const TDesC& aDataSize);

    /**
     * Set info URL
     *
     * @since S60 9.1
     * @param aAttributes MIDlet suite info URL
     */
    void SetInfoUrlL(const TDesC& aInfoUrl);

    /**
     * Set update URL
     *
     * @since S60 9.1
     * @param aUpdateUrl MIDlet suite update URL
     */
    void SetUpdateUrlL(const TDesC& aUpdateUrl);

    /**
     * Set content
     *
     * @since S60 9.1
     * @param aContent MIDlet suite content
     */
    void SetContentL(const TDesC& aContent);

    /**
     * Set mandatory permissions
     *
     * @since S60 9.2
     * @param aPermissions MIDlet suite mandatory permissions
     */
    void SetPermissionsL(const TDesC& aPermissions);

    /**
     * Set content
     *
     * @since S60 9.1
     * @param aAttributes MIDlet suite manifest attributes
     */
    void SetContentL(const RPointerArray<Java::MJavaAttribute>& aAttributes);

    /*
     * Set type
     *
     * @since S60 9.1
     * @param aDescription MIDlet suite description
     */
    void SetTypeL(const TDesC& aDescription);

    TInt AttributeValueIndex(const RPointerArray<Java::MJavaAttribute>& aAttrs,
                             const TDesC& aName);


private:    // data

    /**
     * Handle to file system
     * Not owned
     */
    RFs& iFs;

    /**
     * Info URL
     * Owned
     */
    HBufC* iInfoUrl;

    /**
     * Update URL
     * Owned
     */
    HBufC* iUpdateUrl;

    CAppMngr2MidletResourceHandler& iResourceHandler;
};

#endif // APPMNGR2MIDLETINFOITERATOR_H

