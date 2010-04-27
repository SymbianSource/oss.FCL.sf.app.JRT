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


#ifndef SWTBROWSERSOFTKEYSOBSERVER_H
#define SWTBROWSERSOFTKEYSOBSERVER_H


#include <brctlsoftkeysobserver.h>


class CSwtBrowser;


/**
 * CSwtBrowserSoftkeysObserver
 * This observer is notified when the browser requests softkeys changes
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserSoftkeysObserver)
        : public CBase
        , public MBrCtlSoftkeysObserver
{
public:
    /**
     * Default C++ constructor.
     */
    CSwtBrowserSoftkeysObserver(CSwtBrowser* iBrowser);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserSoftkeysObserver();

public:
// From MBrCtlSoftkeysObserver
    virtual void UpdateSoftkeyL(TBrCtlKeySoftkey aKeySoftkey,
                                const TDesC& aLabel,
                                TUint32 aCommandId,
                                TBrCtlSoftkeyChangeReason aReason);

private: // Data
    /**
     * Pointer to browser API provider.
     * Not own
     */
    CSwtBrowser *iBrowser;
};

#endif // SWTBROWSERSOFTKEYSOBSERVER_H
