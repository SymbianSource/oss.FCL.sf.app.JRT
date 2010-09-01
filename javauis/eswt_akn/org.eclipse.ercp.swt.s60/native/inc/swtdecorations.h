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


#ifndef SWTDECORATIONS_H
#define SWTDECORATIONS_H


#include <AknsControlContext.h>
#include <aknPopupHeadingPane.h>
#include "swtcanvas.h"


class CAknsBasicBackgroundControlContext;


/**
 * CSwtCanvas
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtDecorations)
        : public CSwtCanvas
        , public MSwtDecorations
{
// Own methods
public:
    static CSwtDecorations* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);
    virtual ~CSwtDecorations();
    virtual TRect BorderInnerRectWithoutTitle() const;

protected:
    inline CSwtDecorations(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtComposite* aParent, TInt aStyle, TBool aVisibility, TBool aDimmed);
    void ConstructL();
    inline TBool HasTitleStyle() const;
    virtual void LayoutTitle();
    virtual void CreateTitleL();
    virtual void CreateBackgroundL();
    void UpdateTitleImageL();

// From CSwtComposite
protected:
    void SwtHandleResourceChangeL(TInt aType);

// From ASwtControlBase
protected:
    virtual void DrawBorder(const TRect& aRect) const;
    virtual TRect BorderInnerRect() const;
    virtual TRect BorderOuterRect(const TRect& aRect) const;

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;

protected:
    void SizeChanged();
    void PositionChanged();
    void FocusChanged(TDrawNow aDrawNow);
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

// From MSwtControl
public :
    inline MSwtDecorations* DecorationsInterface();
    MAknsControlContext* GetControlContext() const;

// From MSwtDecorations
public:
    inline MSwtCanvas* Canvas();
    const TDesC* GetText() const;
    const MSwtMenu* MenuBar();
    const MSwtImage* GetImage() const;
    void SetImageL(const MSwtImage* aImage);
    void SetMaximized(TBool aMaximized);
    TBool GetMaximized() const;
    void SetMinimized(TBool aMinimized);
    TBool GetMinimized() const;
    void SetMenuBar(const MSwtMenu* aMenuBar);
    void SetTextL(const TDesC& aText);
    void SetDefaultButton(MSwtButton* aButton);
    void SetFocusedDefaultButton(MSwtButton* aButton);
    const MSwtButton* OriginalDefaultButton() const;
    const MSwtButton* DefaultButton() const;
    TSwtPeer GetDefaultButton() const;

// Data
protected:
    /**
     * True if the mobile shell is in fullscreen.
     */
    TBool iIsMaximized;

    /**
     * Store our coordinates for when we are maximized
     */
    TRect iNormalBounds;

    /**
     * The title bar, cannot be NULL.
     * Own.
     */
    CAknPopupHeadingPane* iTitle;

    /**
     * Skin data: Background
     */
    CAknsBasicBackgroundControlContext* iBgContext;

private:
    /**
     * Current menu bar, may be null.
     * Not own.
     */
    const MSwtMenu* iMenuBar;

    /**
     * The default button, may be NULL.
     * Not own.
     */
    MSwtButton* iDefaultButton;

    /**
     * The default button which is focused, may be NULL.
     * Not own.
     */
    MSwtButton* iFocusedDefaultButton;

    /**
     * The decorations Image, can be NULL.
     * Not own.
     */
    const MSwtImage* iImage;

    /**
     * Title text.
     * Own.
     */
    HBufC* iText;

    /**
     * Current image size.
     */
    TSize iImageSize;
};

// ---------------------------------------------------------------------------
// CSwtDecorations::CSwtDecorations
// ---------------------------------------------------------------------------
//
inline CSwtDecorations::CSwtDecorations(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                        MSwtComposite* aParent, TInt aStyle, TBool aVisibility, TBool aDimmed)
        : CSwtCanvas(aDisplay, aPeer, aParent, aStyle, aVisibility, aDimmed)
        , iNormalBounds(TRect::EUninitialized), iText(NULL)
{
}

// ---------------------------------------------------------------------------
// CSwtDecorations::HasTitleStyle
// ---------------------------------------------------------------------------
//
inline TBool CSwtDecorations::HasTitleStyle() const
{
    return ((iStyle & (KSwtStyleTitle | KSwtStyleNoTrim)) == KSwtStyleTitle);
}

// ---------------------------------------------------------------------------
// CSwtDecorations::DecorationsInterface
// From MSwtControl
// ---------------------------------------------------------------------------
//
MSwtDecorations* CSwtDecorations::DecorationsInterface()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::Canvas
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
MSwtCanvas* CSwtDecorations::Canvas()
{
    return this;
}


#endif // SWTDECORATIONS_H
