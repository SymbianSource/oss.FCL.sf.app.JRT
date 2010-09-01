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


#ifndef SWTBROWSERSCHEMEHANDLER_H
#define SWTBROWSERSCHEMEHANDLER_H


class CSchemeHandler;
class CAiwGenericParamList;
class MAknServerAppExitObserver;


/**
 * CSwtBrowserSchemeHandler
 * @lib eswtapifacade.dll
 */
class CSwtBrowserSchemeHandler
        : public CBase
{
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static  CSwtBrowserSchemeHandler* NewL(const TDesC& aUrl);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserSchemeHandler();


    IMPORT_C void SetParameterList(CAiwGenericParamList* aParamList);
    IMPORT_C void Observer(MAknServerAppExitObserver* aSchemeDoc);
    IMPORT_C void HandleUrlEmbeddedL();
    IMPORT_C void HandleUrlStandaloneL();


private:

    /**
     * Constructor.
     */
    CSwtBrowserSchemeHandler();

    /**
     * Symbian 2nd phase constructor is private.
     */
    void ConstructL(const TDesC& aUrl);



private: // data

    /**
     * Pointer to SchemeHandler
     * Own
     */
    CSchemeHandler* iSchemeHandler;
};

#endif // SWTBROWSERSCHEMEHANDLER_H



