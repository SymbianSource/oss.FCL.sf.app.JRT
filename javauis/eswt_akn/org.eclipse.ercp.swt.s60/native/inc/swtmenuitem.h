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


#ifndef SWTMENUITEM_H
#define SWTMENUITEM_H


#include "eswtwidgetscore.h"


/**
 * CSwtMenuItem
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtMenuItem)
        : public CBase
        , public MSwtMenuItem
{
// Own methods
public:
    /**
     * Two phase constructor.
     * @param aDisplay
     * @param aPeer
     * @param aParent
     * @param aIndex
     * @param aStyle
     * @return CSwtMenuItem*
     */
    static CSwtMenuItem* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtMenu& aParent, TInt aIndex, TInt aStyle, TInt aCommandId = 0);

protected:
    /**
     * First phase constructor.
     * @param aDisplay
     * @param aPeer
     * @param aParent
     * @param aIndex
     * @param aStyle
     */
    inline CSwtMenuItem(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                        MSwtMenu& aParent, TInt aIndex, TInt aStyle, TInt aCommandId);

    /**
     * Destructor.
     */
    virtual ~CSwtMenuItem();

    /**
     * Second phase constructor.
     */
    void ConstructL();

// From MSwtMenuItem
public:
    TSwtPeer Dispose();
    TSwtPeer JavaPeer() const;
    TBool GetSelection() const;
    void SetAcceleratorL(TInt aAccelerator);
    void SetEnabled(TBool aEnabled);
    void SetImageL(const MSwtImage* aImage);
    void SetMenuL(const MSwtMenu* aMenu);
    void SetSelection(TBool aSelected);
    void SetTextL(const TDesC& aText);
    TInt Accelerator() const;
    TBool IsEnabled() const;
    const MSwtImage* Image() const;
    void GetImageBitmapsL(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask, const TSize& aSize);
    const MSwtMenu* Menu() const;
    const TDesC& Text() const;
    TInt CommandId() const;
    TInt Style() const;
    const MSwtMenu& Parent() const;
    void UpdatePaneL(CEikMenuPane& aPane, TSwtMenuItemData aDetail);

private:
    /**
     * The current pane that displays the menu item.
     * NULL if not displayed.
     */
    CEikMenuPane* Pane() const;

    /**
     * Search for radio sibblings in parent menu.
     */
    TInt FirstRadioSibbling() const;
    TInt LastRadioSibbling() const;

    /**
     * Get ordinal index of this
     */
    TInt Index() const;

    /**
     * Remove item from pane.
     */
    void CleanPane() const;

// Data
private:
    /**
     * Current display.
     */
    MSwtDisplay& iDisplay;

    /**
     * Java peer.
     */
    const TSwtPeer iPeer;

    /**
     * Parent menu.
     */
    MSwtMenu& iParent;

    /**
     * Index of the item.
     */
    TInt iIndexOfCreation;

    /**
     * Style flags.
     */
    TInt iStyle;

    /**
     * For radio and checkbox.
     */
    TBool iSelected;

    /**
     * Accelerator.
     */
    TInt iAccelerator;

    /**
     * Enablement flag.
     */
    TBool iEnabled;

    /**
     * Item title.
     * Own.
     */
    HBufC* iText;

    /**
     * Command id.
     */
    TInt iCommandId;

    /**
     * Item image. Ref counted, may be NULL.
     * Not own.
     */
    const MSwtImage* iImage;

    /**
     * Contained menu. May be NULL.
     * Not own.
     */
    const MSwtMenu* iMenu;

    /**
     * Contained dummy image mask. May be NULL.
     */
    CFbsBitmap* iDummyMask;

    /**
     * Used to keep track of image subreferences. The reson for using an array
     * here is not knowing when the menu panes get closed and further not knowing
     * on what screen those menu panes are open. So we must cache all the sizes
     * needed and remove the references only when the image is changed or the
     * menu item is destroyed.
     */
    RArray<TSize> iImageSizes;
};

#endif // SWTMENUITEM_H
