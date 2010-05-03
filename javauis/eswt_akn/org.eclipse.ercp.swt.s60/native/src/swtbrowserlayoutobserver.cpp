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


#include "swtbrowserlayoutobserver.h"
#include "swtbrowser.h"


// ======== MEMBER FUNCTIONS ========


CSwtBrowserLayoutObserver::CSwtBrowserLayoutObserver(CEikScrollBarFrame* aSbFrame)
        : iSbFrame(aSbFrame)
{
    if (iSbFrame)
    {
        iHScrollBar = iSbFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal);
        iVScrollBar = iSbFrame->GetScrollBarHandle(CEikScrollBar::EVertical);
    }
    else
    {
        iHScrollBar = NULL;
        iVScrollBar = NULL;
    }
    iHModel.iScrollSpan = 0;
    iHModel.iThumbPosition = 0;
    iHModel.iThumbSpan = 0;
    iVModel.iScrollSpan = 0;
    iVModel.iThumbPosition = 0;
    iVModel.iThumbSpan = 0;
}

CSwtBrowserLayoutObserver::~CSwtBrowserLayoutObserver()
{
}

// ---------------------------------------------------------
// The function updates vertical custom scrollbar
// ---------------------------------------------------------
//
void CSwtBrowserLayoutObserver::UpdateBrowserVScrollBarL(TInt aDocumentHeight,
        TInt aDisplayHeight, TInt aDisplayPosY)
{

    iVModel.iScrollSpan = aDocumentHeight;
    iVModel.iThumbPosition = aDisplayPosY;
    iVModel.iThumbSpan = aDisplayHeight;

    if (!iVScrollBar) return;
    iVScrollBar->SetModelL(&iVModel);
}

// ---------------------------------------------------------
// The function updates horizontal custom scrollbar
// ---------------------------------------------------------
//
void CSwtBrowserLayoutObserver::UpdateBrowserHScrollBarL(TInt aDocumentWidth,
        TInt aDisplayWidth, TInt aDisplayPosX)
{
    iHModel.iScrollSpan = aDocumentWidth;
    iHModel.iThumbPosition =  aDisplayPosX;
    iHModel.iThumbSpan = aDisplayWidth;

    if (!iHScrollBar) return;
    iHScrollBar->SetModelL(&iHModel);
}


// ---------------------------------------------------------
// Virtual function, must be there
// ---------------------------------------------------------
//
void CSwtBrowserLayoutObserver::NotifyLayoutChange(TBrCtlLayout /*aNewLayout*/)
{
}

// ---------------------------------------------------------
// Virtual function, must be there
// ---------------------------------------------------------
//

void CSwtBrowserLayoutObserver::UpdateTitleL(const TDesC& /*aTitle*/)
{
}
