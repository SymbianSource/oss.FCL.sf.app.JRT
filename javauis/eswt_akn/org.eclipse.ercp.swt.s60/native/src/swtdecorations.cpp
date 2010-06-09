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


#include <aknenv.h>
#include <eikbtgpc.h>
#include <eikspane.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <swtlaffacade.h>
#include "swtcontrolhelper.h"
#include "swtdecorations.h"


// Blank title string
_LIT(KEmptyTitle, " ");

// Space between title and contents in dialog shells.
static const TInt KSwtTitlePaneBottomMargin = 5;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtDecorations::ConstructL
// ---------------------------------------------------------------------------
//
CSwtDecorations* CSwtDecorations::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                       MSwtComposite& aParent, TInt aStyle)
{
    CCoeControl& parentCtrl = aParent.Control()->CoeControl();
    CSwtDecorations* self = new(ELeave) CSwtDecorations(aDisplay, aPeer,
            &aParent, aStyle, parentCtrl.IsVisible(), parentCtrl.IsDimmed());
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::~CSwtDecorations
// ---------------------------------------------------------------------------
//
CSwtDecorations::~CSwtDecorations()
{
    delete iTitle;
    iTitle = NULL;

    delete iBgContext;
    iBgContext = NULL;

    delete iText;
    iText = NULL;

    if (iImage)
    {
        iImage->RemoveSubRef(iImageSize);
        iImage->RemoveRef();
        iImage = NULL;
    }

    iMenuBar = NULL;
    iDefaultButton = NULL;
    iFocusedDefaultButton = NULL;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::BorderInnerRectWithoutTitle
// ---------------------------------------------------------------------------
//
TRect CSwtDecorations::BorderInnerRectWithoutTitle() const
{
    ASSERT(iParent);
    TRect borderInnerRect(Rect());
    if (HasBorderStyle())
    {
        TAknLayoutRect topLeft = CSwtLafFacade::GetLayoutRect(
                                     CSwtLafFacade::EPopupWindowsSkinPlacingFrameGeneralLine2,
                                     borderInnerRect, 0);
        TAknLayoutRect bottomRight = CSwtLafFacade::GetLayoutRect(
                                         CSwtLafFacade::EPopupWindowsSkinPlacingFrameGeneralLine5,
                                         borderInnerRect, 0);
        borderInnerRect.Move(topLeft.Rect().Width(),
                             topLeft.Rect().Height());
        borderInnerRect.Resize(-topLeft.Rect().Width(),
                               -topLeft.Rect().Height());
        borderInnerRect.Resize(-bottomRight.Rect().Width(),
                               -bottomRight.Rect().Height());
    }
    return borderInnerRect;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtDecorations::ConstructL()
{
    CSwtCanvas::ConstructL();
    CreateTitleL();
    iNormalBounds = GetBounds();
}

// ---------------------------------------------------------------------------
// CSwtDecorations::CreateTitleL
// ---------------------------------------------------------------------------
//
void CSwtDecorations::CreateTitleL()
{
    if (!iParent || !HasTitleStyle())
    {
        return;
    }

    ASSERT(!iTitle);
    iTitle = new(ELeave) CAknPopupHeadingPane();
    TPtrC extPtr(KEmptyTitle);
    iTitle->ConstructL(extPtr);
    iTitle->SetContainerWindowL(*this);
    iTitle->MakeVisible(EFalse);   // Needed for drawing trick
    UpdateFrameL();
}

// ---------------------------------------------------------------------------
// CSwtDecorations::CreateBackgroundL
// ---------------------------------------------------------------------------
//
void CSwtDecorations::CreateBackgroundL()
{
    ASSERT(!iBgContext);

    if (!iParent)
    {
        iBgContext = CAknsBasicBackgroundControlContext::NewL(
                         KAknsIIDQsnBgScreen, iDisplay.Device().Bounds(), ETrue);
    }
    else
    {
        iBgContext = CAknsFrameBackgroundControlContext::NewL(
                         KAknsIIDQsnFrPopup, Rect(), BorderInnerRectWithoutTitle(), ETrue);
        MAknsControlContext* topContext = GetTopShell().Control()->GetControlContext();
        if (topContext)
        {
            iBgContext->SetParentContext(topContext);
        }
        iBgContext->SetParentPos(PositionRelativeToScreen());
    }
}

// ---------------------------------------------------------------------------
// CSwtDecorations::UpdateTitleImageL
// ---------------------------------------------------------------------------
//
void CSwtDecorations::UpdateTitleImageL()
{
    if (!iTitle)
    {
        return;
    }

    if (!iImage)
    {
        iTitle->SetHeaderImageOwnedL(NULL);
        return;
    }

    TSize bitmapSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                           iImage->Bitmap().SizeInPixels(),
                           CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EChildShellTitleImagePane, iTitle->Rect()).Rect().Size());

    if (iImageSize == bitmapSize)
    {
        return;
    }

    CFbsBitmap* bitmap =  const_cast<CFbsBitmap*>(&iImage->SubBitmap(bitmapSize));
    CFbsBitmap* mask = const_cast<CFbsBitmap*>(iImage->SubMaskBitmap(bitmapSize, ETrue));

    if (iImageSize != TSize(-1, -1))
    {
        iImage->RemoveSubRef(iImageSize);
    }
    iImage->AddSubRef(bitmapSize);
    iImageSize = bitmapSize;

    CEikImage* image = new(ELeave) CEikImage();
    image->SetNewBitmaps(bitmap, mask);
    image->SetPictureOwnedExternally(ETrue);   // Bitmaps owned by CSwtImage
    iTitle->SetHeaderImageOwnedL(image);
}

// ---------------------------------------------------------------------------
// CSwtDecorations::SwtHandleResourceChangeL
// From CSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SwtHandleResourceChangeL(TInt aType)
{
    CSwtCanvas::SwtHandleResourceChangeL(aType);
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        UpdateTitleImageL();
    }
}

// ---------------------------------------------------------------------------
// CSwtDecorations::DrawBorder
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
void CSwtDecorations::DrawBorder(const TRect& /*aRect*/) const
{
    CWindowGc& gc = SystemGc();
    AknsDrawUtils::BackgroundBetweenRects(AknsUtils::SkinInstance(),
                                          iBgContext, this, gc, Rect(), BorderInnerRectWithoutTitle());
}

// ---------------------------------------------------------------------------
// CSwtDecorations::BorderInnerRect
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
TRect CSwtDecorations::BorderInnerRect() const
{
    ASSERT(iParent);
    TRect innerRect(BorderInnerRectWithoutTitle());
    if (iTitle && HasTitleStyle())
    {
        innerRect.iTl.iY = iTitle->Rect().iBr.iY + KSwtTitlePaneBottomMargin;
    }
    return innerRect;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::BorderOuterRect
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
TRect CSwtDecorations::BorderOuterRect(const TRect& aRect) const
{
    ASSERT(iParent);
    TRect borderOuterRect(aRect);
    if (iTitle && HasTitleStyle())
    {
        borderOuterRect.iTl.iY -= iTitle->MinimumSize().iHeight;
    }
    if (HasBorderStyle())
    {
        TRect windowRect(Rect());
        TAknLayoutRect topLeft = CSwtLafFacade::GetLayoutRect(
                                     CSwtLafFacade::EPopupWindowsSkinPlacingFrameGeneralLine2,
                                     windowRect, 0);
        TAknLayoutRect bottomRight = CSwtLafFacade::GetLayoutRect(
                                         CSwtLafFacade::EPopupWindowsSkinPlacingFrameGeneralLine5,
                                         windowRect, 0);
        borderOuterRect.Move(-topLeft.Rect().Width(),
                             -topLeft.Rect().Height());
        borderOuterRect.Resize(topLeft.Rect().Width(),
                               topLeft.Rect().Height());
        borderOuterRect.Resize(bottomRight.Rect().Width(),
                               bottomRight.Rect().Height());
    }

    return borderOuterRect;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtDecorations::CountComponentControls() const
{
    TInt count = CSwtCanvas::CountComponentControls();
    if (iParent && iTitle && HasTitleStyle())
    {
        ++count;
    }
    return count;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtDecorations::ComponentControl(TInt aIndex) const
{
    if (iParent && iTitle && HasTitleStyle())
    {
        if (aIndex == 0)
        {
            return iTitle;
        }
        --aIndex;
    }
    return CSwtCanvas::ComponentControl(aIndex);
}

// ---------------------------------------------------------------------------
// CSwtDecorations::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SizeChanged()
{
    if (iParent && iTitle && HasTitleStyle())
    {
        //Can be called during ConstructL() before title is created... :-|
        LayoutTitle();
    }
    CSwtCanvas::SizeChanged();
    if (iParent && iBgContext)
    {
        // Update the Background Skin Size of Child Shell.
        // The size of the top level background doesn't need to be updated
        // because it used the display bounds. So we just update it on a
        // "HandleResourceChange" when the screen size change.
        (static_cast<CAknsFrameBackgroundControlContext*>(iBgContext))->SetFrameRects(
            Rect(), BorderInnerRectWithoutTitle());
    }
}

// ---------------------------------------------------------------------------
// CSwtDecorations::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtDecorations::PositionChanged()
{
    if (iParent && iTitle && HasTitleStyle())
    {
        LayoutTitle();
    }
    CSwtComposite::PositionChanged();
    if (iParent && iBgContext)
    {
        iBgContext->SetParentPos(PositionRelativeToScreen());
    }
}

// ---------------------------------------------------------------------------
// CSwtDecorations::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtDecorations::FocusChanged(TDrawNow aDrawNow)
{
    if (iParent && iTitle && HasTitleStyle())
    {
        TBool focused = IsFocused();
        iTitle->SetFocus(focused, ENoDrawNow);
        if (focused)
        {
            LayoutTitle();
        }
    }
    CSwtCanvas::FocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtDecorations::MopSupplyObject
// From CCoeControl
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtDecorations::MopSupplyObject(TTypeUid aId)
{
    if (aId.iUid == CEikMenuBar:: ETypeId)
    {
        // The menu bar of the menu arranger is supplied for instance when
        // a Edwin asks for a menu bar to display its fep editing menu.
        return aId.MakePtr(iDisplay.MenuArranger().EikMenuBar());
    }
    if (aId.iUid == MAknsControlContext::ETypeId && iTitle)
    {
        return MAknsControlContext::SupplyMopObject(aId, GetControlContext());
    }
    return CSwtCanvas::MopSupplyObject(aId);
}

// ---------------------------------------------------------------------------
// CSwtDecorations::GetControlContext
// From MSwtControl
// ---------------------------------------------------------------------------
//
MAknsControlContext* CSwtDecorations::GetControlContext() const
{
    return iBgContext;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::GetText
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
const TDesC* CSwtDecorations::GetText() const
{
    ASSERT(iParent);
    return iText;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::ConstructL
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SetTextL(const TDesC& aText)
{
    ASSERT(iParent);
    if (iText)
    {
        delete iText;
        iText = NULL;
    }
    iText = aText.AllocL();
    if (iTitle)
    {
        iTitle->SetTextL(aText);
    }
    if (iTitle)
    {
        RedrawRegion(iTitle->Rect());
    }
}

// ---------------------------------------------------------------------------
// CSwtDecorations::SetDefaultButton
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SetDefaultButton(MSwtButton* aButton)
{
    if (iDefaultButton == aButton)
    {
        return;
    }
    if (iDefaultButton)
    {
        MSwtControl* focusControl = GetShell().FocusControl();

        // If the current default button is the current focus control, it keeps
        // its default status. If the current focus control is not a button,
        // the current default button will be changed and should lose its
        // default status now. The case where there is no focus control could
        // probably not happen, but we even so remove the default status
        // of the current default button for secure.
        if (focusControl)
        {
            if (focusControl->ButtonInterface() != iDefaultButton)
            {
                const_cast<MSwtButton*>(iDefaultButton)->SetDefault(EFalse);
            }
        }
        else
        {
            const_cast<MSwtButton*>(iDefaultButton)->SetDefault(EFalse);
        }
    }
    iDefaultButton = aButton;
    if (iDefaultButton)
    {
        MSwtControl* focusControl = GetShell().FocusControl();
        if (focusControl)
        {
            // If the current focused control is not a Button
            // then the new current default button gain the default status.
            if (focusControl->ButtonInterface() == NULL)
            {
                const_cast<MSwtButton*>(iDefaultButton)->SetDefault(ETrue);
            }
            else if ((focusControl->Style() & KSwtStylePush) == 0)
            {
                const_cast<MSwtButton*>(iDefaultButton)->SetDefault(ETrue);
            }
        }
        else
        {
            const_cast<MSwtButton*>(iDefaultButton)->SetDefault(ETrue);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtDecorations::SetFocusedDefaultButton
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SetFocusedDefaultButton(MSwtButton* aButton)
{
    if (aButton)
    {
        ASSERT(aButton->Control()->Style() & KSwtStylePush);
        aButton->SetDefault(ETrue);
        // If the defaultButton of the CSwtDecorations is not the Focused Button,
        // so remove its DefaultButton Status.
        if ((iDefaultButton != aButton) && iDefaultButton)
        {
            iDefaultButton->SetDefault(EFalse);
        }
    }
    else
    {
        iFocusedDefaultButton->SetDefault(EFalse);
        // So set the DefaultButton status on the CSwtDecorations to DefaultButton.
        if (iDefaultButton)
        {
            iDefaultButton->SetDefault(ETrue);
        }
    }
    iFocusedDefaultButton = aButton;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::OriginalDefaultButton
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
const MSwtButton* CSwtDecorations::OriginalDefaultButton() const
{
    return iDefaultButton;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::DefaultButton
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
const MSwtButton* CSwtDecorations::DefaultButton() const
{
    return iFocusedDefaultButton ? iFocusedDefaultButton : iDefaultButton;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::GetDefaultButton
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtDecorations::GetDefaultButton() const
{
    if (DefaultButton())
    {
        return (const_cast<MSwtButton*>(DefaultButton()))->Control()->JavaPeer();
    }
    else
    {
        return NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtDecorations::LayoutTitle
// The position of the title depends on whether we are a top level shell (no
// parent) or a dialog shell (there is a parent). For top level shells (no
// parent), the title is positioned using the outer rectangle so that it
// appears on the application title bar. For child shells (dialogs), the title
// is posistioned using the border inner rect so that it appears immediately
// underneath the top border and it doesn't overlap the dialog frame.
// ---------------------------------------------------------------------------
//
void CSwtDecorations::LayoutTitle()
{
    ASSERT(iParent);
    ASSERT(iTitle);

    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EPopupParent, mainPane);
    TInt h(CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EHeadingPane, mainPane).Rect().Height());
    iTitle->SetExtent(TPoint(0, 0), TSize(Rect().Width(), h));

    TRAP_IGNORE(UpdateTitleImageL());
}

// ---------------------------------------------------------------------------
// CSwtDecorations::GetImage
// ---------------------------------------------------------------------------
//
const MSwtImage* CSwtDecorations::GetImage() const
{
    return iImage;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::SetImageL
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SetImageL(const MSwtImage* aImage)
{
    if (iImage)
    {
        iImage->RemoveSubRef(iImageSize);
        iImage->RemoveRef();
    }
    iImage = aImage;
    iImageSize = TSize(-1, -1);
    if (iImage)
    {
        iImage->AddRef();
    }
    if (iTitle)
    {
        UpdateTitleImageL();
        RedrawRegion(iTitle->Rect());
    }
}

// ---------------------------------------------------------------------------
// CSwtDecorations::SetMaximized
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SetMaximized(TBool /*aMaximized*/)
{
    // Nothing to do, unsupported feature
}

// ---------------------------------------------------------------------------
// CSwtDecorations::GetMaximized
// ---------------------------------------------------------------------------
//
TBool CSwtDecorations::GetMaximized() const
{
    return iIsMaximized;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::SetMinimized
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SetMinimized(TBool /*aMinimized*/)
{
    // Nothing to do, unsupported feature
}

// ---------------------------------------------------------------------------
// CSwtDecorations::GetMinimized
// ---------------------------------------------------------------------------
//
TBool CSwtDecorations::GetMinimized() const
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::MenuBar
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtDecorations::MenuBar()
{
    return iMenuBar;
}

// ---------------------------------------------------------------------------
// CSwtDecorations::SetMenuBar
// ---------------------------------------------------------------------------
//
void CSwtDecorations::SetMenuBar(const MSwtMenu* aMenuBar)
{
    iMenuBar = aMenuBar;
    iDisplay.MenuArranger().MenuBarSetOnDecorations();
}
