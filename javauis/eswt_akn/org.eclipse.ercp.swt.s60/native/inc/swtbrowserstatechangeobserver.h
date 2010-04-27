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


#ifndef SWTBROWSERSTATECHANGEOBSERVER_H
#define SWTBROWSERSTATECHANGEOBSERVER_H


#include <brctlinterface.h>
#include "eswt.h"


class MBrowserApiProvider;


/**
 * CSwtBrowserSpecialLoadObserver
 * Handle state changed events
 * This class implements MBrCtlStateChangeObserver, which registers for state change events.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserStateChangeObserver)
        : public CBase
        , public MBrCtlStateChangeObserver
{
public:
    /**
     * Constructor.
     * @param aContainer The pointer points to the browser API provider.
     * @param aPeer The Java object associated with this C++ object.
     */
    CSwtBrowserStateChangeObserver(MBrowserApiProvider* aApiProvider, TSwtPeer aPeer);

    /**
     * Destructor.
     */
    ~CSwtBrowserStateChangeObserver();

// From MBrCtlStateChangeObserver
    void StateChanged(TBrCtlDefs::TBrCtlState aState, TInt /*aValue*/);

private:
    /**
     * Update the title.
     * @param aApiProvider The browser API provider.
     * @return void.
     */
    void NotifyTitleChangeL(MBrowserApiProvider& aApiProvider);

private:   // data
    /**
     * Pointer to the browser API provider.
     * Not own
     */
    MBrowserApiProvider* iApiProvider;

    /**
     * Java side peer
     * Not own
     */
    TSwtPeer iPeer;
};

#endif // SWTBROWSERSTATECHANGEOBSERVER_H
