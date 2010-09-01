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


#include <eikspane.h>
#include <eikbtgpc.h>
#include <eiklabel.h>
#include <akntitle.h>
#include <aknnavide.h>
#include <AknUtils.h>
#include <avkon.hrh>
#include <eswtcore.rsg>
#include <swtlaffacade.h>
#include "eswtwidgetscore.h"
#include "swtuiutils.h"
#include "swtmobileshell.h"


// Main pane varieties
static const TInt KSwtMainPainVarietyClassic = 3;
static const TInt KSwtMainPainVarietySmallSpPortrait = 6;
static const TInt KSwtMainPainVarietySmallSpLandscape = 9;
static const TInt KSwtMainPainVarietySmallSpLandscapePen = 4;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtMobileShell::NewL
// ---------------------------------------------------------------------------
//
CSwtMobileShell* CSwtMobileShell::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                       MSwtShell* aParent, TInt aStyle)
{
    CSwtMobileShell* self = new(ELeave) CSwtMobileShell(aDisplay, aPeer,
            aParent, aStyle);
    // Intentionally not constructed at this point. Delayed to ChangeTrimL.
    return self;
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::~CSwtMobileShell
// ---------------------------------------------------------------------------
//
CSwtMobileShell::~CSwtMobileShell()
{
    delete iStatusTextBuf;
    iKeyCodeStatus.Close();
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::CSwtMobileShell
// ---------------------------------------------------------------------------
//
inline CSwtMobileShell::CSwtMobileShell(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                        MSwtShell* aParent, TInt aStyle)
        : CSwtShell(aDisplay, aPeer, aParent, aStyle)
{
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtMobileShell::ConstructL()
{
    CSwtShell::ConstructL();
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::GetLeftAndRightKeyState
// ---------------------------------------------------------------------------
//
TBool CSwtMobileShell::GetLeftAndRightKeyState(TInt aLeftScanCode,
        TInt aRightScanCode)
{
    const TInt indexLeft = iKeyCodeStatus.FindInOrder(aLeftScanCode);
    const TInt indexRight = iKeyCodeStatus.FindInOrder(aRightScanCode);

    if (indexLeft != KErrNotFound)
    {
        iKeyCodeStatus.Remove(indexLeft);
    }
    if (indexRight != KErrNotFound)
    {
        iKeyCodeStatus.Remove(indexRight);
    }

    return (indexLeft != KErrNotFound || indexRight != KErrNotFound) ?
           ETrue : EFalse;
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtMobileShell::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    AddKeyStateL(aKeyEvent.iScanCode);
    return CSwtShell::OfferKeyEventL(aKeyEvent, aType);
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::DefaultBounds
// From CSwtShell
// ---------------------------------------------------------------------------
//
TRect CSwtMobileShell::DefaultBounds() const
{
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);

    if (FullScreenMode())
    {
        return screenRect;
    }

    TRect rect;
    TInt variety;
    if (iStatusPaneStyle == MSwtShell::EStyleSmallStatusPane)
    {
        if (CSwtLafFacade::IsLandscapeOrientation())
        {
            variety = AknLayoutUtils::PenEnabled() ?
                      KSwtMainPainVarietySmallSpLandscapePen :
                      KSwtMainPainVarietySmallSpLandscape;
        }
        else
        {
            variety = KSwtMainPainVarietySmallSpPortrait;
        }
    }
    else
    {
        if (CSwtLafFacade::IsLandscapeOrientation())
        {
            variety = AknLayoutUtils::PenEnabled() ?
                      KSwtMainPainVarietySmallSpLandscapePen :
                      KSwtMainPainVarietySmallSpLandscape;
        }
        else
        {
            variety = KSwtMainPainVarietyClassic;
        }
    }
    TAknLayoutRect layoutRect = CSwtLafFacade::GetComposeAndLayoutRect(
                                    CSwtLafFacade::EMainPaneCompose, screenRect, variety);
    rect = layoutRect.Rect();
    if (iStatusPaneStyle == MSwtShell::EStyleNoStatusPane)
    {
        rect.iTl = screenRect.iTl;
    }
    if (!HasCba())
    {
        rect.iBr = screenRect.iBr;
    }

    return rect;
}


// ---------------------------------------------------------------------------
// CSwtMobileShell::AddKeyStateL
// From MSwtMobileShell
// ---------------------------------------------------------------------------
//
void CSwtMobileShell::AddKeyStateL(TInt aScanCode)
{
    if (iKeyCodeStatus.FindInOrder(aScanCode) == KErrNotFound)
    {
        User::LeaveIfError(iKeyCodeStatus.InsertInOrder(aScanCode));
    }
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::ChangeTrimL
// From MSwtMobileShell
// ---------------------------------------------------------------------------
//
void CSwtMobileShell::ChangeTrimL(TInt aStyle, TInt aStatusStyle)
{
    MSwtShell::TSwtStatusPaneStyle statusStyle =
        static_cast<MSwtShell::TSwtStatusPaneStyle>(aStatusStyle);

    if (statusStyle == iStatusPaneStyle)
    {
        return;
    }

    iStatusPaneStyle = statusStyle;

    // Immediatelly called after construction on Java side. Status style is
    // an essential parameter for the construction of parent CSwtShell, hence
    // the delay construction.
    if (!iConstructed)
    {
        iConstructed = ETrue;
        ConstructL();
    }

    CSwtShell::SetStyle(aStyle);
    MSwtUiUtils& utils = iDisplay.UiUtils();
    if (utils.GetActiveShell() == this)
    {
        utils.UpdateStatusPaneL();
    }

    DoSetRect(DefaultBounds());
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::Shell
// From MSwtMobileShell
// ---------------------------------------------------------------------------
//
const MSwtShell* CSwtMobileShell::Shell() const
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::GetKeyState
// From MSwtMobileShell
// ---------------------------------------------------------------------------
//
TBool CSwtMobileShell::GetKeyState(TInt aSwtKeyCode)
{
    if (iDisplay.UiUtils().GetActiveShell() != Shell())
    {
        return EFalse;
    }

    // Ctrl, Shift & Alt: look for both left and right keys
    if (aSwtKeyCode == KSwtModifierControl)
    {
        return GetLeftAndRightKeyState(EStdKeyLeftCtrl, EStdKeyRightCtrl);
    }
    else if (aSwtKeyCode == KSwtModifierAlt)
    {
        return GetLeftAndRightKeyState(EStdKeyLeftAlt, EStdKeyRightAlt);
    }
    else if (aSwtKeyCode == KSwtModifierShift)
    {
        return GetLeftAndRightKeyState(EStdKeyLeftShift, EStdKeyRightShift);
    }

    TInt index = iKeyCodeStatus.FindInOrder(
                     iDisplay.GetSymbianScanCode(aSwtKeyCode));
    if (index != KErrNotFound)
    {
        iKeyCodeStatus.Remove(index);
        return ETrue;
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtMobileShell::AddKeyStateL
// ---------------------------------------------------------------------------
//
void CSwtMobileShell::SetStatusTextL(const TDesC& aText)
{
    if (iStatusTextBuf)
    {
        delete iStatusTextBuf;
        iStatusTextBuf = NULL;
    }
    iStatusTextBuf = aText.AllocL();
    if (UiUtils().GetActiveShell() == this)
    {
        UiUtils().UpdateStatusPaneL();
    }
}
