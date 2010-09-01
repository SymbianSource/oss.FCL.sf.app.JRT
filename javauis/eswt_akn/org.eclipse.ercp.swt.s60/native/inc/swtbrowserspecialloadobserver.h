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


#ifndef SWTBROWSERSPECIALLOADOBSERVER_H
#define SWTBROWSERSPECIALLOADOBSERVER_H


#include <brctlspecialloadobserver.h>
#include <AknServerApp.h>


class CDocumentHandler;
class CSwtBrowserSchemeHandler;
class CSwtBrowser;
class CAiwGenericParamList;


/**
 * CSwtBrowserSpecialLoadObserver
 * This observer handles special load events such as network connection,
 * deal with non-http(s) or non-html requests.
 * This class mainly implements MBrCtlSpecialLoadObserver,
 * which registers for special load events.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserSpecialLoadObserver)
        : public CBase
        , public MBrCtlSpecialLoadObserver
        , public MAknServerAppExitObserver
{
public:
    /**
     * Two-phased constructor.
     * param aContainer The pointer points to the browser Api provider
     */
    static CSwtBrowserSpecialLoadObserver* NewL(CSwtBrowser* aBrowser);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserSpecialLoadObserver();

// From MBrCtlSpecialLoadObserver
    virtual void NetworkConnectionNeededL(TInt* aConnectionPtr, TInt* aSockSvrHandle,
                                          TBool* aNewConn, TApBearerType* aBearerType);
    TBool HandleRequestL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray);
    TBool HandleDownloadL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray);

// From MAknServerAppExitObserver
    void HandleServerAppExit(TInt aReason);

private:
    /**
     * Constructor.
     * param aContainer The pointer points to the browser Api provider
     */
    CSwtBrowserSpecialLoadObserver(CSwtBrowser* aBrowser);

    /**
     * Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Request the host applicaion to handle non-http request.
     * @param aTypeArray The non-http(s) or file URL
     * @param aDesArray Parameters to pass to the host application. Contain referer header.
     * @return ETrue is handled by the host application. EFlase if not
     */
    virtual TBool DoHandleRequestL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray);

    /**
     * Make generic parameter list from BrCtl parameter list.
     * @param aTypeArray array of download parameter types
     * @param aDesArray array of values associated with the types in the type array
     * @return The dynamically allocated list.
     */
    CAiwGenericParamList* BrCtlParamList2GenericParamListL(RArray<TUint>* aTypeArray,
            CDesCArrayFlat* aDesArray) const;

    /**
     * Extract the given BrCtl parameter from the list.
     * @param aParamTypeToFind Extract this parameter.
     * @param aTypeArray array of download parameter types.
     * @param aDesArray array of values associated with the types in the type array.
     * @param aParamFound Output: ETrue if the parameter was found.
     * @return A TPtrC pointer for the value.
     */
    TPtrC ExtractBrCtlParam(TUint aParamTypeToFind, RArray<TUint>* aTypeArray,
                            CDesCArrayFlat* aDesArray, TBool& aParamFound) const;

private: //Data
    /**
     * Scheme processing.
     */
    TBool iSchemeProcessing;

    /**
     * Used to send the content to the proper application.
     * Own
     */
    CDocumentHandler* iHandler;

    /**
     * scheme handler
     * Own
     */
    CSwtBrowserSchemeHandler *iSchemeHandler;

    /**
     * Pointer to browser API provider.
     * Not own
     */
    CSwtBrowser *iBrowser;
};

#endif // SWTBROWSERSPECIALLOADOBSERVER_H
