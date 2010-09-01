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


#ifndef SWTBROWSERAPSETTINGSHANDLER_H
#define SWTBROWSERAPSETTINGSHANDLER_H

class CApSettingsHandler;

/**
 * CSwtBrowserApSettingsHandler
 * @lib eswtapifacade.dll
 */


class CSwtBrowserApSettingsHandler
        : public CBase
{
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CSwtBrowserApSettingsHandler* NewLC();

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserApSettingsHandler();

    IMPORT_C TInt RunSettingsL(TUint32 aHighLight, TUint32& aSelected);

private:

    /**
     * Constructor.
     */
    CSwtBrowserApSettingsHandler();

    /**
     * Symbian 2nd phase constructor is private.
     */
    void ConstructL();


private: // data

    /**
     * Pointer to CApUtils
     * Own
     */
    CApSettingsHandler* iSettingsHandler;

};

#endif // SWTBROWSERAPSETTINGSHANDLER_H
