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


#include "swtfactory.h"
#include "swtbutton.h"
#include "swtcombo.h"
#include "swtfiledialog.h"
#include "swtlink.h"
#include "swtlist.h"
#include "swtlabel.h"
#include "swtmenu.h"
#include "swtmenuitem.h"
#include "swtmenuarranger.h"
#include "swtscrollbar.h"
#include "swtshell.h"
#include "swttext.h"
#include "swtprogressbar.h"
#include "swtslider.h"
#include "swtuiutils.h"
#include "swtcommand.h"
#include "swtcommandarranger.h"
#include "swtcolordialog.h"
#include "swtclipboard.h"
#include "swttable.h"
#include "swttablecolumn.h"
#include "swttableitem.h"
#include "swttree.h"
#include "swtdirectorydialog.h"
#include "swtuiutils.h"
#include "swtbrowser.h"
#include "swtcaptionedcontrol.h"
#include "swtconstrainedtext.h"
#include "swtdateeditor.h"
#include "swthyperlink.h"
#include "swtinput.h"
#include "swtlistbox.h"
#include "swtlistview.h"
#include "swtmobiledevice.h"
#include "swtmobileshell.h"
#include "swtmultipagedialog.h"
#include "swtscreen.h"
#include "swtsortedlist.h"
#include "swttasktip.h"
#include "swttextextension.h"
#include "swttimedmessagebox.h"
#include "swtquerydialog.h"
#include "swtuiutils.h"
#include "swtbitmapgc.h"
#include "swtcolor.h"
#include "swtfont.h"
#include "swtwindowgc.h"


// ======== MEMBER FUNCTIONS ========


CSwtFactory* CSwtFactory::NewL()
{
    CSwtFactory* self = new(ELeave) CSwtFactory;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CSwtFactory::CSwtFactory()
{
}

CSwtFactory::~CSwtFactory()
{
}

void CSwtFactory::ConstructL()
{
}

MSwtUiUtils* CSwtFactory::NewUiUtilsL(MSwtDisplay& aDisplay) const
{
    return CSwtUiUtils::NewL(aDisplay);
}

MSwtCanvas* CSwtFactory::NewCanvasL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtCanvas::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtComposite* CSwtFactory::NewCompositeL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtComposite::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtDecorations* CSwtFactory::NewDecorationsL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtDecorations::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtShell* CSwtFactory::NewShellL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtShell* aParent,
    TInt aStyle) const
{
    return CSwtShell::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtMenu* CSwtFactory::NewMenuL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    TInt aStyle) const
{
    return CSwtMenu::NewL(aDisplay, aPeer, aStyle);
}

MSwtMenuItem* CSwtFactory::NewMenuItemL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtMenu& aParent,
    TInt aIndex,
    TInt aStyle) const
{
    return CSwtMenuItem::NewL(aDisplay, aPeer, aParent, aIndex, aStyle);
}

MSwtMenuArranger* CSwtFactory::NewMenuArrangerL(
    MSwtDisplay& aDisplay) const
{
    return CSwtMenuArranger::NewL(aDisplay);
}

MSwtLink* CSwtFactory::NewLinkL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtLink::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtList* CSwtFactory::NewListL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtList::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtLabel* CSwtFactory::NewLabelL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtLabel::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtButton* CSwtFactory::NewButtonL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtButton::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtText* CSwtFactory::NewTextL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtText::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtProgressBar* CSwtFactory::NewProgressBarL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtProgressBar::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtSlider* CSwtFactory::NewSliderL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtSlider::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtCombo* CSwtFactory::NewComboL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtCombo::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtFileDialog* CSwtFactory::NewFileDialogL(const TDesC& aInitPath,
        const TDesC& aTitle, TInt aStyle) const
{
    return CSwtFileDialog::NewL(aInitPath, aTitle, aStyle);
}

MSwtCommand* CSwtFactory::NewCommandL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtControl& aParentControl,
    MSwtCommand* aParentCommand,
    TInt aType,
    TInt aPriority) const
{
    return CSwtCommand::NewL(aDisplay, aPeer, aParentControl,
                             aParentCommand, aType, aPriority);
}

MSwtCommandArranger* CSwtFactory::NewCommandArrangerL(
    MSwtDisplay& aDisplay) const
{
    return CSwtCommandArranger::NewL(aDisplay);
}

MSwtMobileShell* CSwtFactory::NewMobileShellL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtShell* aParent,
    TInt aStyle) const
{
    return CSwtMobileShell::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtMobileDevice* CSwtFactory::NewMobileDeviceL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer) const
{
    return CSwtMobileDevice::NewL(aDisplay, aPeer);
}

MSwtInput* CSwtFactory::NewInputL(
    TSwtPeer aPeer,
    TInt aInputId) const
{
    return CSwtInput::NewL(aPeer, aInputId);
}

MSwtScreen* CSwtFactory::NewScreenL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    TInt aScreenId) const
{
    return CSwtScreen::NewL(aDisplay, aPeer, aScreenId);
}

MSwtMultiPagePager* CSwtFactory::NewMultiPagePagerL(
    MSwtDisplay& /*aDisplay*/,
    TSwtPeer /*aPeer*/,
    MSwtShell& /*aParent*/) const
{
    return NULL;
}

MSwtMultiPageTabGroup*  CSwtFactory::NewMultiPageTabGroupL(
    MSwtDisplay& /*aDisplay*/,
    TSwtPeer /*aPeer*/,
    MSwtShell& /*aParent*/) const
{
    return NULL;
}

MSwtMultiPageDialog* CSwtFactory::NewMultiPageDialogL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtShell& aParent) const
{
    return CSwtMultiPageDialog::NewL(aDisplay, aPeer, aParent);
}

MSwtCaptionedControl* CSwtFactory::NewCaptionedControlL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtCaptionedControl::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtConstrainedText* CSwtFactory::NewConstrainedTextL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtConstrainedText::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtDateEditor* CSwtFactory::NewDateEditorL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtDateEditor::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtListBox* CSwtFactory::NewListBoxL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtListBox::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtHyperLink* CSwtFactory::NewHyperLinkL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtHyperLink::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtListView* CSwtFactory::NewListViewL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtListView::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtSortedList* CSwtFactory::NewSortedListL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtSortedList::NewL(aDisplay, aPeer, aParent, aStyle);
}

void CSwtFactory::RunTimedMessageBoxDlgLD(
    const TDesC& aTitle,
    const TDesC& aMessage,
    TInt aStyle,
    const MSwtImage* aImage) const
{
    CSwtTimedMessageBox::RunDlgLD(aTitle, aMessage, aStyle, aImage);
}

HBufC* CSwtFactory::RunQueryDialogL(
    const TDesC& aTitle,
    TInt aStyle,
    TInt aQueryType,
    const TDesC& aPromptText,
    const TDesC& aDefaultValue,
    TInt aMinimum,
    TInt aMaximum) const
{
    return CSwtQueryDialog::RunDlgL(aTitle, aStyle, aQueryType,
                                    aPromptText, aDefaultValue, aMinimum, aMaximum);
}

HBufC* CSwtFactory::RunQueryDialogConvertDefaultValueL(
    const TDesC& aString,
    TInt aQueryType) const
{
    return CSwtQueryDialog::ConvertDefaultValueL(aString, aQueryType);
}

MSwtTaskTip* CSwtFactory::NewTaskTipL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    TInt aStyle) const
{
    return CSwtTaskTip::NewL(aDisplay, aPeer, aStyle);
}

MSwtTextExtension* CSwtFactory::NewTextExtensionL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    return CSwtTextExtension::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtClipboard* CSwtFactory::NewClipboardL(TSwtPeer aPeer) const
{
    //( void )aPeer;
    //return NULL;
    return CSwtClipboard::NewL(aPeer);
}

MSwtTable* CSwtFactory::NewTableL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    (void)aDisplay;
    (void)aPeer;
    (void)aParent;
    (void)aStyle;
    return CSwtTable::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtTableColumn* CSwtFactory::NewTableColumnL(
    TSwtPeer aPeer,
    MSwtTable& aParent,
    TInt aStyle,
    TInt aColumnIndex) const
{
    (void)aPeer;
    (void)aParent;
    (void)aStyle;
    (void)aColumnIndex;
    return CSwtTableColumn::NewL(aPeer, aParent, aStyle, aColumnIndex);
}

MSwtTableItem* CSwtFactory::NewTableItemL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtTable& aParent,
    TInt aRowIndex) const
{
    (void)aDisplay;
    (void)aPeer;
    (void)aParent;
    (void)aRowIndex;
    return CSwtTableItem::NewL(aDisplay, aPeer, static_cast<CSwtTable&>(aParent), aRowIndex);
}

MSwtTree* CSwtFactory::NewTreeL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle) const
{
    (void)aDisplay;
    (void)aPeer;
    (void)aParent;
    (void)aStyle;
    return CSwtTree::NewL(aDisplay, aPeer, aParent, aStyle);
}

MSwtFontDialog* CSwtFactory::NewFontDialogL() const
{
    User::Leave(ESwtErrorNotImplemented);
    return NULL;
    //return new( ELeave ) CSwtFontDialog();
}

TRgb* CSwtFactory::RunColorDialogL(TRgb* aDefaultColor) const
{
    return CSwtColorDialog::RunDlgL(aDefaultColor);
}

MSwtDirectoryDialog* CSwtFactory::NewDirectoryDialogL(const TDesC& aFilterPath,
        const TDesC& aTitle) const
{
    return CSwtDirectoryDialog::NewL(aFilterPath, aTitle);
}

MSwtBrowser* CSwtFactory::NewBrowserL(MSwtDisplay& aDisplay, TSwtPeer aPeer
                                      , MSwtComposite& aParent, TInt aStyle) const
{
    return CSwtBrowser::NewL(aDisplay, aPeer, aParent, aStyle);
    //return NULL;
}

MSwtColor* CSwtFactory::NewColorL(const MSwtDevice& aDevice,
                                  const TRgb& aRgb) const
{
    return new(ELeave) CSwtColor(aDevice, aRgb);
}

MSwtGc* CSwtFactory::NewBitmapGcL(MSwtBitmapDrawable& aDrawable,
                                  CFbsBitGc* aGc, MSwtGc::TDestructor aDestructor, const TRgb& aPenCol,
                                  const TRgb& aBrushCol, const MSwtFont& aFont) const
{
    return CSwtBitmapGc::NewL(aDrawable, aGc, aDestructor, aPenCol, aBrushCol, aFont);
}

MSwtGc* CSwtFactory::NewWindowGcL(MSwtWindowDrawable& aDrawable,
                                  CWindowGc* aGc, MSwtGc::TDestructor aDestructor, const TRgb& aPenCol,
                                  const TRgb& aBrushCol, const MSwtFont& aFont, const TRect& aBoundaryRect,
                                  const TRect& aGcScreenRect) const
{
    return CSwtWindowGc::NewL(aDrawable, aGc, aDestructor, aPenCol,
                              aBrushCol, aFont, aBoundaryRect, aGcScreenRect);
}

MSwtFont* CSwtFactory::NewFontL(const MSwtDevice& aDevice,
                                const TDesC& aTypefaceName, TInt aHeight, TInt aStyle) const
{
    return CSwtFont::NewL(aDevice, aTypefaceName, aHeight, aStyle);
}
