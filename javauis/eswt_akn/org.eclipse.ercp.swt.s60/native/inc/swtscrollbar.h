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


#ifndef SWTSCROLLBAR_H
#define SWTSCROLLBAR_H


#include <eiksbobs.h>
#include "eswtwidgetscore.h"



class CEikScrollBar;
class TEikScrollBarModel;


/**
 * CSwtScrollBar
 *
 * In eSWT all scrollbars must be non window owing the main reason being that
 * the scrollbars must draw at exact same time with the parent control. This
 * is especially essential in ScrolledComposite. If the scrollbars would be
 * window owing, they would draw too much ahead of the parents creating an
 * ugly visual effect when flicking the ScrolledComposite.
 * The drawback of having non window owing scrollbars is too many paint events
 * which affects on the speed of kinetic scrolling in lists.
 *
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtScrollBar)
        : public CBase
        , public MSwtScrollBar
{
public:
    CSwtScrollBar(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aStyle,
    CEikScrollBar& aScrollBar, MSwtScrollable& aScrollable, TInt aMinimum,
    TInt aMaximum, TInt aPosition, TInt aThumbSpan, TBool aDoubleSpan,
    CEikScrollBar::TOrientation aOrientation);
    inline const CEikScrollBar& EikScrollBar() const;
    void DoSetThumbPositionL(TInt aPosition);

private:
    ~CSwtScrollBar();
    void SetModelL(TInt aPosition, TInt aMinimum, TInt aMaximum, TInt aThumbSpan);

// From MSwtScrollBar
public:
    TSwtPeer Dispose();
    TSize GetSize() const;
    void SetEnabled(TBool aEnabled);
    void SetVisible(TBool aVisible);
    TInt GetThumbPosition() const;
    void SetThumbPositionL(TInt aPosition);
    TInt GetThumbSpan() const;
    void SetThumbSpanL(TInt aThumbSpan);
    TInt GetMinimum() const;
    void SetMinimumL(TInt aMinimum);
    TInt GetMaximum() const;
    void SetMaximumL(TInt aMaximum);
    TInt GetIncrement() const;
    void SetIncrement(TInt aIncrement);
    TInt GetPageIncrement() const;
    void SetPageIncrement(TInt aPageIncrement);
    void SetValuesL(TInt aPosition, TInt aMinimum, TInt aMaximum,
                    TInt aThumbSpan, TInt aIncrement, TInt aPageIncrement);

#ifdef RD_SCALABLE_UI_V2
// From MEikScrollBarObserver
public:
    void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);
#endif

// Data
private:
    MSwtDisplay&    iDisplay;
    const TSwtPeer  iPeer;
    TInt  iStyle;
    CEikScrollBar&  iScrollBar;
    MSwtScrollable& iScrollable;
    TInt iIncrement;
    TInt iPageIncrement;
    TInt iMinimum;
    TInt iMaximum;
    TInt iThumbPosition;
    TInt iThumbSpan;
    TBool iDoubleSpan;
    CEikScrollBar::TOrientation iOrientation;
};

inline const CEikScrollBar& CSwtScrollBar::EikScrollBar() const
{
    return iScrollBar;
}


#endif // SWTSCROLLBAR_H
