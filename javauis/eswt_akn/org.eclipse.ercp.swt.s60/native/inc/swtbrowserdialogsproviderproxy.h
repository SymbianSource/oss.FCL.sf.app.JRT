/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifndef SWTBROWSERDIALOGSPROVIDERPROXY_H
#define SWTBROWSERDIALOGSPROVIDERPROXY_H


#include <AknUtils.h>
#include <brctlinterface.h>


class MSwtBrowserDialogsProviderObserver;
class CSwtBrowserDialogsProviderObserverProxyPrivate;
class CBrowserDialogsProvider;


/**
 * CSwtBrowserDialogsProvider
 * @lib eswtapifacade.dll
 */
class CSwtBrowserDialogsProviderProxy
        : public CBase
        , public MBrCtlDialogsProvider
{
// Own methods
public:
    IMPORT_C static CSwtBrowserDialogsProviderProxy* NewL(
        MSwtBrowserDialogsProviderObserver* aObserver);
    IMPORT_C virtual ~CSwtBrowserDialogsProviderProxy();

// From MBrCtlDialogsProvider
public:
    IMPORT_C void DialogNotifyErrorL(TInt aErrCode);
    IMPORT_C void DialogNotifyHttpErrorL(TInt aErrCode, const TDesC& aUri);
    IMPORT_C TBool DialogFileSelectLC(const TDesC& aStartPath,
                                      const TDesC& aRootPath, HBufC*& aSelectedFileName);
    IMPORT_C TBool DialogSelectOptionL(const TDesC& aTitle,
                                       TBrCtlSelectOptionType aSelectOptionType,
                                       CArrayFix<TBrCtlSelectOptionData>& aOptions);
    IMPORT_C TBool DialogUserAuthenticationLC(const TDesC& aUrl, const TDesC& aRealm,
            const TDesC& aDefaultUserName, HBufC*& aReturnedUserName,
            HBufC*& aReturnedPasswd, TBool aBasicAuthentication = EFalse);
    IMPORT_C void DialogNoteL(const TDesC& aMessage);
    IMPORT_C void DialogAlertL(const TDesC& aTitle, const TDesC& aMessage);
    IMPORT_C TBool DialogConfirmL(const TDesC& aTitle, const TDesC& aMessage,
                                  const TDesC& aYesMessage, const TDesC& aNoMessage);
    IMPORT_C TBool DialogPromptLC(const TDesC& aTitle, const TDesC& aMessage,
                                  const TDesC& aDefaultInput, HBufC*& aReturnedInput);
    IMPORT_C TBool DialogDownloadObjectL(CBrCtlObjectInfo* aBrCtlObjectInfo);
    IMPORT_C void DialogDisplayPageImagesL(CArrayFixFlat<TBrCtlImageCarrier>& aPageImages);
    IMPORT_C void CancelAll();
    IMPORT_C void DialogFindL();

// Own methods
private:
    CSwtBrowserDialogsProviderProxy(MSwtBrowserDialogsProviderObserver* aObserver);
    void ConstructL();

// Data
private:
    /**
     * The native browser dialogs provider to which all the requests are delegated.
     * Own.
     */
    CBrowserDialogsProvider* iDelegate;

    /**
     * The native browser dialogs provider observer to which iDelegate reports back.
     * This observer of the delegate forwards all the reports to iObserver.
     * Own.
     */
    CSwtBrowserDialogsProviderObserverProxyPrivate* iDelegateObserver;

    /**
     * Optional observer to which this class reports to.
     * The reports are forwarded from iDelegateObserver.
     * Not Own.
     */
    MSwtBrowserDialogsProviderObserver* iObserver;
};

#endif // SWTBROWSERDIALOGSPROVIDERPROXY_H


