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


#include "browserapiprovider.h"
#include "swtbrowserstatechangeobserver.h"


_LIT(KSchemaIdentifier, "://");
const TInt KSchemaIdentifierLength = 3;


// ======== MEMBER FUNCTIONS ========


CSwtBrowserStateChangeObserver::CSwtBrowserStateChangeObserver(MBrowserApiProvider* aApiProvider
        , TSwtPeer aPeer)
        : iApiProvider(aApiProvider)
        , iPeer(aPeer)
{
}

CSwtBrowserStateChangeObserver::~CSwtBrowserStateChangeObserver()
{
}

void CSwtBrowserStateChangeObserver::StateChanged(TBrCtlDefs::TBrCtlState aState
        , TInt /*aValue*/)
{
    switch (aState)
    {
        /*
        case TBrCtlDefs::EStateImageMapView:
            {
            break;
            }
        */
    case TBrCtlDefs::EStateHistoryView:
    {
        TRAP_IGNORE(NotifyTitleChangeL(*iApiProvider));
        break;
    }
    case TBrCtlDefs::EStateThumbnailView:
    {
        TRAP_IGNORE(NotifyTitleChangeL(*iApiProvider));
        break;
    }
    default:
        break;
    }
}

void CSwtBrowserStateChangeObserver::NotifyTitleChangeL(MBrowserApiProvider& aApiProvider)
{
    HBufC* title = aApiProvider.BrCtlInterface().PageInfoLC(
                       TBrCtlDefs::EPageInfoTitle);
    if (title && title->Length())
    {
        HBufC* textbuf = iApiProvider->BrCtlInterface().PageInfoLC(TBrCtlDefs::EPageInfoTitle);
        if (textbuf)
        {
            iApiProvider->Display().PostTitleEventL(iPeer, *textbuf);
        }
        CleanupStack::PopAndDestroy(textbuf);
    }
    else
    {
        TInt offset = 0;
        HBufC* titleUrl = aApiProvider.BrCtlInterface().PageInfoLC(
                              TBrCtlDefs::EPageInfoUrl);
        if (!titleUrl)
        {
            CleanupStack::PopAndDestroy(titleUrl);
            titleUrl = KNullDesC().AllocLC();
        }
        offset = titleUrl->Find(KSchemaIdentifier);
        if (offset != KErrNotFound)
        {
            titleUrl->Des().Delete(0,offset + KSchemaIdentifierLength);
        }
        HBufC* textbuf = iApiProvider->BrCtlInterface().PageInfoLC(TBrCtlDefs::EPageInfoTitle);
        if (textbuf)
        {
            iApiProvider->Display().PostTitleEventL(iPeer, *textbuf);
        }
        CleanupStack::PopAndDestroy(textbuf);
        CleanupStack::PopAndDestroy(titleUrl);
    }

    CleanupStack::PopAndDestroy(title);
}
