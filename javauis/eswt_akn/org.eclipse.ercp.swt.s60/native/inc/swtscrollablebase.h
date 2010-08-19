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


#ifndef SWTSCROLLABLEBASE_H
#define SWTSCROLLABLEBASE_H


#include <eikscrlb.h>
#include <eiksbobs.h>
#include <swtlaffacade.h>
#include "swtcontrolbase.h"


class CSwtScrollBar;


/**
 * ASwtScrollableBase
 * Base (abstract) class of all scrollable controls
 * @lib eswt
 */
NONSHARABLE_CLASS(ASwtScrollableBase)
        : public ASwtControlBase
        , public MSwtScrollable
        , public MEikScrollBarObserver
{
// Own methods
protected:
    /**
     * Default constructor
     * @param aDisplay parent display
     * @param aPeer java peer of this class
     * @param aParent composite parent
     * @param aStyle control style
     */
    inline ASwtScrollableBase(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite* aParent, TInt aStyle);

    /**
     * Destructor
     */
    inline ~ASwtScrollableBase();

    /**
     * Scrollbar frame getter
     * @return pointer to frame
     */
    virtual CEikScrollBarFrame* SbFrame() const =0;

    /**
     * Default scrollbar model getter
     * @return model
     */
    inline static TEikScrollBarModel DefaultSbModel(TInt aStyle);

    /**
     * Compute the inner rectangle of a scrollbar frame
     * @param aRect scrollbar frame container rectangle
     * @return rectangle contained within the scrollbar frame
     */
    TRect SbInnerRect(const TRect& aRect) const;

    /**
     * Compute the outer rectangle of a scrollbar frame
     * @param aRect rectangle contained within the scrollbar frame
     * @return scrollbar frame container rectangle
     */
    TRect SbOuterRect(const TRect& aRect) const;

// From MSwtScrollable
public:
    MSwtControl* Control();
    MSwtScrollBar* CreateScrollBarL(TSwtPeer aPeer, TInt aStyle);
    virtual void SetVisibleScrollBar(TInt aStyle, TBool aVisible);
    virtual void ScrollBarDestroyed(const MSwtScrollBar* aScrollbar);
    void ComputeTrim(TRect& aRect) const;
    TBool HasScrollBars() const;

// From ASwtControlBase
public:
    virtual TRect ClientRect() const;
    virtual MSwtScrollable* ScrollableInterface();

// From MEikScrollBarObserver
public:
    virtual void HandleScrollEventL(CEikScrollBar* aScrollBar,
                                    TEikScrollEvent aEventType);

// Data
protected:
    /**
     * Horizontal scrollbar, may be NULL.
     * Not own.
     */
    CSwtScrollBar* iHScroll;

    /**
     * Vertical scrollbar, may be NULL.
     * Not own.
     */
    CSwtScrollBar* iVScroll;
};

inline ASwtScrollableBase::ASwtScrollableBase(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite* aParent,
    TInt aStyle)
        : ASwtControlBase(aDisplay, aPeer, aParent, aStyle)
{
}

inline ASwtScrollableBase::~ASwtScrollableBase()
{
    iHScroll = NULL;
    iVScroll = NULL;
}

inline TEikScrollBarModel ASwtScrollableBase::DefaultSbModel(TInt aStyle)
{
    TInt thumbPos(CSwtLafFacade::KSwtDefaultScrollBarModelThumbposition);

    if ((aStyle & KSwtStyleHScroll) && AknLayoutUtils::LayoutMirrored())
    {
        thumbPos = Max((CSwtLafFacade::KSwtDefaultScrollBarModelScrollSpan
                        - CSwtLafFacade::KSwtDefaultScrollBarModelThumbposition
                        - CSwtLafFacade::KSwtDefaultScrollBarModelThumbSpan), 0);
    }


    return TEikScrollBarModel(CSwtLafFacade::KSwtDefaultScrollBarModelScrollSpan,
                              CSwtLafFacade::KSwtDefaultScrollBarModelThumbSpan,
                              thumbPos);
}

#endif // SWTSCROLLABLEBASE_H
