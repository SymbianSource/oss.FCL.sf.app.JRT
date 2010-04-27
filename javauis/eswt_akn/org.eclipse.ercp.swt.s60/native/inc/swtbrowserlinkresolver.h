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


#ifndef SWTBROWSERLINKRESOLVER_H
#define SWTBROWSERLINKRESOLVER_H


#include <brctllinkresolver.h>
#include "eswt.h"


class CBrCtlInterface;
class CSwtBrowser;


/**
 * CSwtBrowserLinkResolver
 * Handles resolving embedded links
 * This class is used to provide a callback mechanism for receiving the content of an embedded
 * link or the content of a user-initiated load request.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserLinkResolver)
        : public CBase
        , public MBrCtlLinkResolver
{
public:
    /**
     * Constructor.
     * param aBrowser The pointer points to the browser
     * param aPeer The Java object associated with this C++ object
     */
    CSwtBrowserLinkResolver(CSwtBrowser* aBorwser, TSwtPeer aPeer);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserLinkResolver();

// From MBrCtlLinkResolver
    TBool ResolveEmbeddedLinkL(const TDesC& /*aEmbeddedUrl*/, const TDesC& /*aCurrentUrl*/,
                               TBrCtlLoadContentType /*aLoadContentType*/,  MBrCtlLinkContent& /*aEmbeddedLinkContent*/) ;
    TBool ResolveLinkL(const TDesC& aUrl, const TDesC& /*aCurrentUrl*/,
                       MBrCtlLinkContent& /*aBrCtlLinkContent*/) ;
    void CancelAll() ;

private:   // data
    /**
     * Pointer to browser.
     * Not own
     */
    CSwtBrowser *iBrowser;

    /**
     * Java side peer
     * Not own
     */
    TSwtPeer iPeer;
};

#endif // SWTBROWSERLINKRESOLVER_H
