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


#ifndef SWTFACTORY_H
#define SWTFACTORY_H


#include "eswtwidgetscore.h"
#include "eswtmobileextensions.h"
#include "eswtwidgetsexpanded.h"
#include "eswtgraphics.h"


/**
 * CSwtFactory
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtFactory)
        : public CBase
        , public MSwtFactory
{
// Own methods
public:
    static CSwtFactory* NewL();
    CSwtFactory();
    ~CSwtFactory();
    void ConstructL();

// From MSwtFactory
public:
    // Core widgets
    MSwtUiUtils* NewUiUtilsL(MSwtDisplay& aDisplay) const;
    MSwtCanvas* NewCanvasL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtComposite& aParent, TInt aStyle) const;
    MSwtComposite* NewCompositeL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                 MSwtComposite& aParent, TInt aStyle) const;
    MSwtDecorations* NewDecorationsL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                     MSwtComposite& aParent, TInt aStyle) const;
    MSwtShell* NewShellL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                         MSwtShell* aParent, TInt aStyle) const;
    MSwtMenu* NewMenuL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                       TInt aStyle) const;
    MSwtMenuItem* NewMenuItemL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                               MSwtMenu& aParent, TInt aIndex, TInt aStyle) const;
    MSwtMenuArranger* NewMenuArrangerL(MSwtDisplay& aDisplay) const;
    MSwtLink* NewLinkL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                       MSwtComposite& aParent, TInt aStyle) const;
    MSwtList* NewListL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                       MSwtComposite& aParent, TInt aStyle) const;
    MSwtLabel* NewLabelL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                         MSwtComposite& aParent, TInt aStyle) const;
    MSwtButton* NewButtonL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtComposite& aParent, TInt aStyle) const;
    MSwtText* NewTextL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                       MSwtComposite& aParent, TInt aStyle) const;
    MSwtProgressBar*  NewProgressBarL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                      MSwtComposite& aParent, TInt aStyle) const;
    MSwtSlider* NewSliderL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtComposite& aParent, TInt aStyle) const;
    MSwtCombo* NewComboL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                         MSwtComposite& aParent, TInt aStyle) const;
    MSwtFileDialog* NewFileDialogL(const TDesC& aInitPath,
                                   const TDesC& aTitle, TInt aStyle) const;
    MSwtCommand* NewCommandL(MSwtDisplay& aDisplay, TSwtPeer aPeerRef,
                             MSwtControl& aParentControl, MSwtCommand* aParentCommand, TInt aType,
                             TInt aPriority) const;
    MSwtCommandArranger* NewCommandArrangerL(MSwtDisplay& aDisplay) const;

    // Mobile widgets
    MSwtCaptionedControl* NewCaptionedControlL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const;
    MSwtConstrainedText* NewConstrainedTextL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const;
    MSwtDateEditor* NewDateEditorL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                   MSwtComposite& aParent, TInt aStyle) const;
    MSwtMobileShell* NewMobileShellL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                     MSwtShell* aParent, TInt aStyle) const;
    MSwtMobileDevice* NewMobileDeviceL(MSwtDisplay& aDisplay,
                                       TSwtPeer aPeer) const;
    MSwtInput* NewInputL(TSwtPeer aPeer, TInt aInputId) const;
    MSwtScreen* NewScreenL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           TInt aScreenId) const;
    MSwtMultiPagePager* NewMultiPagePagerL(MSwtDisplay& aDisplay,
                                           TSwtPeer aPeer, MSwtShell& aParent) const;
    MSwtMultiPageTabGroup* NewMultiPageTabGroupL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtShell& aParent) const;
    MSwtMultiPageDialog* NewMultiPageDialogL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtShell& aParent) const;
    MSwtHyperLink* NewHyperLinkL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                 MSwtComposite& aParent, TInt aStyle) const;
    MSwtListBox* NewListBoxL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                             MSwtComposite& aParent, TInt aStyle) const;
    MSwtListView* NewListViewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                               MSwtComposite& aParent, TInt aStyle) const;
    MSwtSortedList* NewSortedListL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                   MSwtComposite& aParent, TInt aStyle) const;
    MSwtTextExtension* NewTextExtensionL(MSwtDisplay& aDisplay,
                                         TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const;
    void RunTimedMessageBoxDlgLD(const TDesC& aTitle, const TDesC& aMessage,
                                 TInt aStyle, const MSwtImage* aImage) const;
    HBufC* RunQueryDialogL(const TDesC& aTitle, TInt aStyle,
                           TInt aQueryType, const TDesC& aPromptText, const TDesC& aDefaultValue,
                           TInt aMinimum, TInt aMaximum) const;
    virtual HBufC* RunQueryDialogConvertDefaultValueL(const TDesC& aString,
            TInt aQueryType) const;

    // Expanded widgets
    MSwtClipboard* NewClipboardL(TSwtPeer aPeer) const;
    MSwtTable* NewTableL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                         MSwtComposite& aParent, TInt aStyle) const;
    MSwtTableColumn* NewTableColumnL(TSwtPeer aPeer, MSwtTable& aParent,
                                     TInt aStyle, TInt aColumnIndex) const;
    MSwtTableItem* NewTableItemL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                 MSwtTable& aParent, TInt aRowIndex) const;
    MSwtTree* NewTreeL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                       MSwtComposite& aParent, TInt aStyle) const;
    TRgb* RunColorDialogL(TRgb* aDefaultColor) const;
    MSwtDirectoryDialog* NewDirectoryDialogL(const TDesC& aFilterPath,
            const TDesC& aTitle) const;
    MSwtBrowser* NewBrowserL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                             MSwtComposite& aParent, TInt aStyle) const;

    // Graphics
    MSwtColor* NewColorL(const MSwtDevice& aDevice, const TRgb& aRgb) const;
    MSwtGc* NewBitmapGcL(MSwtBitmapDrawable& aDrawable, CFbsBitGc* aGc,
                         MSwtGc::TDestructor aDestructor, const TRgb& aPenCol,
                         const TRgb& aBrushCol, const MSwtFont& aFont) const;
    MSwtGc* NewWindowGcL(MSwtWindowDrawable& aDrawable, CWindowGc* aGc,
                         MSwtGc::TDestructor aDestructor, const TRgb& aPenCol,
                         const TRgb& aBrushCol, const MSwtFont& aFont, const TRect& aBoundaryRect,
                         const TRect& aGcScreenRect) const;
    MSwtFont* NewFontL(const MSwtDevice& aDevice, const TDesC& aTypefaceName,
                       TInt aHeight, TInt aStyle) const;
};

#endif // SWTFACTORY_H
