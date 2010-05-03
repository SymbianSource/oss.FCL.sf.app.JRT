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


#include "swtbrowserlinkresolver.h"
#include "swtbrowser.h"


// ======== MEMBER FUNCTIONS ========


CSwtBrowserLinkResolver::CSwtBrowserLinkResolver(CSwtBrowser* aBrowser, TSwtPeer aPeer)
        : iBrowser(aBrowser)
        , iPeer(aPeer)
{
}

CSwtBrowserLinkResolver::~CSwtBrowserLinkResolver()
{
}

TBool CSwtBrowserLinkResolver::ResolveEmbeddedLinkL(const TDesC& /*aEmbeddedUrl*/,
        const TDesC& /*aCurrentUrl*/, TBrCtlLoadContentType /*aLoadContentType*/,
        MBrCtlLinkContent& /*aEmbeddedLinkContent*/)
{
    return EFalse; // embededlink will be resolved by the system if return EFalse
}

TBool CSwtBrowserLinkResolver::ResolveLinkL(const TDesC& aUrl, const TDesC& /*aCurrentUrl*/,
        MBrCtlLinkContent& /*aBrCtlLinkContent*/)
{
    _LIT(KUrl, "data:");
    if (aUrl == KUrl)
    {
        return EFalse;
    }

    HBufC* url = aUrl.AllocLC();
    iBrowser->LoadObserver().SetUrlL(*url);
    iBrowser->Display().PostLocationChangingEventL(
        iPeer, *iBrowser, ETrue, ETrue, aUrl, ESwtBrLoadUrl);
    CleanupStack::PopAndDestroy(url);

    return ETrue; // Link will not resolved by system if return ETure
}

// ---------------------------------------------------------
// Virtual function, must be there
// ---------------------------------------------------------
//
void CSwtBrowserLinkResolver::CancelAll()
{
}
