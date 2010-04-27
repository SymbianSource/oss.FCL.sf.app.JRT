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


#ifndef SWTBROWSERLAYOUTOBSERVER_H
#define SWTBROWSERLAYOUTOBSERVER_H


#include <brctllayoutobserver.h>
#include <eiksbfrm.h>


class CSwtBrowser;
class CEikScrollBarFrame;
class TEikScrollBarModel;


/**
 * CSwtBrowserContainer
 * Handle scrollbar events.
 * This class implements MBrCtlLayoutObserver, which registers for loyout events.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserLayoutObserver)
        : public CBase
        , public MBrCtlLayoutObserver
{
public:
    /**
     * Constructor.
     * @param aContainer The pointer points to the scrollbar frame
     */
    CSwtBrowserLayoutObserver(CEikScrollBarFrame* aSbFrame);

    /**
     * Destructor.
     */
    ~CSwtBrowserLayoutObserver();

// From MBrCtlLayoutObserver
    void UpdateBrowserVScrollBarL(TInt aDocumentHeight,
                                  TInt aDisplayHeight, TInt aDisplayPosY) ;
    void UpdateBrowserHScrollBarL(TInt aDocumentWidth,
                                  TInt aDisplayWidth, TInt aDisplayPosX) ;
    void NotifyLayoutChange(TBrCtlLayout aNewLayout) ;
    void UpdateTitleL(const TDesC& aTitle);

private:   // data
    /**
     * Model of horizontal scroll bar.
     */
    TEikScrollBarModel iHModel;

    /**
     * Model of vertical scroll bar.
     */
    TEikScrollBarModel iVModel;

    /**
     * Horizontal scroll bar.
     * Not own
     */
    CEikScrollBar* iHScrollBar;
    /**
     * Vertical scroll bar.
     * Not own
     */
    CEikScrollBar* iVScrollBar;

    /**
     * The scroll bar frame.
     * Not own
     */
    CEikScrollBarFrame* iSbFrame;
};

#endif // SWTBROWSERLAYOUTOBSERVER_H



