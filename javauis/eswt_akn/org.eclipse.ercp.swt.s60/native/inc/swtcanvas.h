/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTCANVAS_H
#define SWTCANVAS_H


#include "swtcomposite.h"


/**
 * CSwtCanvas
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCanvas)
        : public CSwtComposite
        , public MSwtCanvas
{
// Own methods
public:
    static CSwtCanvas* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);
protected:
    inline CSwtCanvas(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                      MSwtComposite* aParent, TInt aStyle, TBool aVisibility,
                      TBool aDimmed);

// From CSwtComposite
public:
    TBool IsFocusable(TInt aReason = KSwtFocusByApi) const;

// From MSwtCanvas
public:
    inline MSwtComposite* Composite();
    void Scroll(const TPoint& aDest, const TRect& aRect, TBool aAll);

// Form CCoeControl
public:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                TEventCode aType);

};

// ---------------------------------------------------------------------------
// CSwtCanvas::CSwtCanvas
// ---------------------------------------------------------------------------
//
inline CSwtCanvas::CSwtCanvas(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                              MSwtComposite* aParent, TInt aStyle, TBool aVisibility, TBool aDimmed)
        : CSwtComposite(aDisplay, aPeer, aParent, aStyle, aVisibility, aDimmed)
{
}

// ---------------------------------------------------------------------------
// CSwtCanvas::Composite
// From MSwtCanvas
// ---------------------------------------------------------------------------
//
inline MSwtComposite* CSwtCanvas::Composite()
{
    return this;
}

#endif // SWTCANVAS_H
