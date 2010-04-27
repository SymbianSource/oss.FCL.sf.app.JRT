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


#include <eikbtgpc.h>
#include <avkon.rsg>
#include "swtbrowsersoftkeysobserver.h"
#include "swtbrowser.h"


// ======== MEMBER FUNCTIONS ========


CSwtBrowserSoftkeysObserver::CSwtBrowserSoftkeysObserver(CSwtBrowser* aBrowser)
{
    iBrowser = aBrowser;
}

CSwtBrowserSoftkeysObserver::~CSwtBrowserSoftkeysObserver()
{
}

void CSwtBrowserSoftkeysObserver::UpdateSoftkeyL(TBrCtlKeySoftkey  /*aKeySoftkey*/,
        const TDesC&  /*aLabel*/, TUint32  /*aCommandId*/, TBrCtlSoftkeyChangeReason /* aReason */)
{
    iBrowser->UpdateCbaL();
}
