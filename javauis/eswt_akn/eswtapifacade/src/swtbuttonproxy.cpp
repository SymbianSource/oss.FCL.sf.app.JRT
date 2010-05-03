/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/



#include "swtcustombutton.h"
#include "swtbuttonproxy.h"

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

const TInt KSwtAknButtonCtrlsCount = 1;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtButtonStateProxy* CSwtButtonStateProxy::NewL(
    CAknButtonState& aAknButtonState)
{
    CSwtButtonStateProxy* self = new(ELeave) CSwtButtonStateProxy(aAknButtonState);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::~CSwtButtonStateProxy
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtButtonStateProxy::~CSwtButtonStateProxy()
{
}

// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::Text
// From CAknButtonState
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSwtButtonStateProxy::Text() const
{
    return iDelegate.Text();
}

// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::SetTextL
// From CAknButtonState
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonStateProxy::SetTextL(const TDesC& aText)
{
    iDelegate.SetTextL(aText);
}

// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::Icon
// From CAknButtonState
// ---------------------------------------------------------------------------
//
EXPORT_C const CGulIcon* CSwtButtonStateProxy::Icon() const
{
    return iDelegate.Icon();
}

// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::SetIcon
// From CAknButtonState
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonStateProxy::SetIcon(CGulIcon* aIcon)
{
    iDelegate.SetIcon(aIcon);
}

// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::Flags
// From CAknButtonState
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtButtonStateProxy::Flags() const
{
    return iDelegate.Flags();
}

// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::SetFlags
// From CAknButtonState
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonStateProxy::SetFlags(const TInt aFlags)
{
    iDelegate.SetFlags(aFlags);
}

// ---------------------------------------------------------------------------
// CSwtButtonStateProxy::CSwtButtonStateProxy
// ---------------------------------------------------------------------------
//
CSwtButtonStateProxy::CSwtButtonStateProxy(CAknButtonState& aAknButtonState)
        : CBase()
        , iDelegate(aAknButtonState)
{
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtButtonProxy* CSwtButtonProxy::NewL(
    CCoeControl* aParent,
    CGulIcon* aIcon,
    CGulIcon* aDimmedIcon,
    CGulIcon* aPressedIcon,
    CGulIcon* aHoverIcon,
    const TDesC& aText,
    const TDesC& aHelpText,
    const TInt aButtonFlags,
    const TInt aStateFlags)
{
    CSwtButtonProxy* self = new(ELeave) CSwtButtonProxy();
    CleanupStack::PushL(self);
    self->ConstructL(
        aParent,
        aIcon,
        aDimmedIcon,
        aPressedIcon,
        aHoverIcon,
        aText,
        aHelpText,
        aButtonFlags,
        aStateFlags);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtButtonProxy* CSwtButtonProxy::NewL(
    CCoeControl* aParent,
    const TDesC& aFilePath,
    const TInt aBmpId,
    const TInt aMaskId,
    const TInt aDimmedBmpId,
    const TInt aDimmedMaskId,
    const TInt aPressedBmpId,
    const TInt aPressedMaskId,
    const TInt aHoverBmpId,
    const TInt aHoverMaskId,
    const TDesC& aText,
    const TDesC& aHelpText,
    const TInt aButtonFlags,
    const TInt aStateFlags,
    const TAknsItemID& aId /*= KAknsIIDNone*/,
    const TAknsItemID& aDimmedId /*= KAknsIIDNone*/,
    const TAknsItemID& aPressedId /*= KAknsIIDNone*/,
    const TAknsItemID& aHoverId /*= KAknsIIDNone*/)
{
    CSwtButtonProxy* self = new(ELeave) CSwtButtonProxy();
    CleanupStack::PushL(self);
    self->ConstructL(
        aParent,
        aFilePath,
        aBmpId,
        aMaskId,
        aDimmedBmpId,
        aDimmedMaskId,
        aPressedBmpId,
        aPressedMaskId,
        aHoverBmpId,
        aHoverMaskId,
        aText,
        aHelpText,
        aButtonFlags,
        aStateFlags,
        aId,
        aDimmedId,
        aPressedId,
        aHoverId);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtButtonProxy* CSwtButtonProxy::NewL(CCoeControl* aParent,
        CGulIcon* aIcon, const TDesC& aText, const TInt aFlags)
{
    CSwtButtonProxy* self = new(ELeave) CSwtButtonProxy();
    CleanupStack::PushL(self);
    self->ConstructL(aParent, aIcon, aText, aFlags);
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------------------------
// CSwtButtonProxy::~CSwtButtonProxy
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtButtonProxy::~CSwtButtonProxy()
{
    delete iDelegate;
    iDelegate = NULL;
    delete iDelegateState;
    iDelegateState = NULL;
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::Text
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSwtButtonProxy::Text() const
{
    CSwtButtonStateProxy* state = State();
    if (state)
    {
        return state->Text();
    }
    return KNullDesC();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetTextL
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetTextL(const TDesC& aText)
{
    CSwtButtonStateProxy* state = State();
    if (state)
    {
        state->SetTextL(aText);
    }
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::Icon
// ---------------------------------------------------------------------------
//
EXPORT_C const CGulIcon* CSwtButtonProxy::Icon(void) const
{
    CSwtButtonStateProxy* state = State();
    if (state)
    {
        return state->Icon();
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetIcon
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetIcon(CGulIcon* aIcon)
{
    CSwtButtonStateProxy* state = State();
    if (state)
    {
        state->SetIcon(aIcon);
        SizeChanged();
    }
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::Flags
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtButtonProxy::Flags() const
{
    CSwtButtonStateProxy* state = State();
    if (state)
    {
        return state->Flags();
    }

    return 0;
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetFlags
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetFlags(const TInt aFlags)
{
    CSwtButtonStateProxy* state = State();
    if (state)
    {
        return state->SetFlags(aFlags);
    }
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetTextFont
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetTextFont(const CFont* aFont)
{
    iDelegate->SetTextFont(aFont);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetTextHorizontalAlignment
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetTextHorizontalAlignment(
    const TInt aHorizontalAlignment)
{
    iDelegate->SetTextHorizontalAlignment(
        CGraphicsContext::TTextAlign(aHorizontalAlignment));
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetTextUnderlineStyle
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetTextUnderlineStyle(
    TFontUnderline aUnderlineStyle)
{
    iDelegate->SetTextUnderlineStyle(aUnderlineStyle);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetIconScaleMode
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetIconScaleMode(const TScaleMode aScaleMode)
{
    iDelegate->SetIconScaleMode(aScaleMode);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetIconHorizontalAlignment
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetIconHorizontalAlignment(
    const TInt aHorizontalAlignment)
{
    iDelegate->SetIconHorizontalAlignment(
        CAknButton::TAlignment(aHorizontalAlignment));
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetHighlightRect
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetHighlightRect(const TRect& aRect)
{
    iDelegate->SetHighlightRect(aRect);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::HighlightRect
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CSwtButtonProxy::HighlightRect() const
{
    return iDelegate->HighlightRect();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::IsDimmed
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtButtonProxy::IsDimmed() const
{
    return iDelegate->IsDimmed();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::State
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtButtonStateProxy* CSwtButtonProxy::State() const
{
    return iDelegateState;
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::ButtonFlags
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtButtonProxy::ButtonFlags() const
{
    return iDelegate->ButtonFlags();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetIconSize
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtButtonProxy::SetIconSize(const TSize& aSize)
{
    return iDelegate->SetIconSize(aSize);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::ResetState
// From CAknButton
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::ResetState()
{
    iDelegate->ResetState();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CSwtButtonProxy::ComponentControl(TInt /*aIdx*/) const
{
    return iDelegate;
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtButtonProxy::CountComponentControls() const
{
    return KSwtAknButtonCtrlsCount;
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::MinimumSize
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CSwtButtonProxy::MinimumSize()
{
    return iDelegate->MinimumSize();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::ActivateL
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::ActivateL()
{
    if (iDelegate)
    {
        iDelegate->ActivateL();
    }
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetDimmed(TBool aDimmed)
{
    iDelegate->SetDimmed(aDimmed);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::MakeVisible(TBool aVisible)
{
    iDelegate->MakeVisible(aVisible);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::PrepareForFocusLossL
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::PrepareForFocusLossL()
{
    iDelegate->PrepareForFocusLossL();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::PrepareForFocusGainL
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::PrepareForFocusGainL()
{
    iDelegate->PrepareForFocusGainL();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CSwtButtonProxy::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    return iDelegate->OfferKeyEventL(aKeyEvent, aType);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::HandlePointerEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    iDelegate->HandlePointerEventL(aPointerEvent);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::HandleResourceChange(TInt aType)
{
    iDelegate->HandleResourceChange(aType);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SetBackground
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetBackground(const MCoeControlBackground* aBackground)
{
    iDelegate->SetBackground(aBackground);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::IsFocused
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtButtonProxy::IsFocused() const
{
    return iDelegate->IsFocused();
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::IsFocused
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SetFocus(TBool aFocus, TDrawNow aDrawNow /*=ENoDrawNow*/)
{
    iDelegate->SetFocus(aFocus, aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::SizeChanged()
{
    iDelegate->SetRect(Rect());
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::HandleControlEventL
// From MCoeControlObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::HandleControlEventL(CCoeControl* aControl,
        TCoeEvent aEventType)
{
    if (aControl == iDelegate)
    {
        if (Observer())
        {
            Observer()->HandleControlEventL(this, aEventType);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::CSwtButtonProxy
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtButtonProxy::CSwtButtonProxy()
        : CAknControl()
{
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::ConstructL(
    CCoeControl* aParent,
    CGulIcon* aIcon,
    CGulIcon* aDimmedIcon,
    CGulIcon* aPressedIcon,
    CGulIcon* aHoverIcon,
    const TDesC& aText,
    const TDesC& aHelpText,
    const TInt aButtonFlags,
    const TInt aStateFlags)
{
    if (!aParent)
    {
        User::Leave(KErrArgument);
    }

    SetContainerWindowL(*aParent);
    SetMopParent(aParent);

    iDelegate = CSwtCustomButton::NewL(
                    aIcon,
                    aDimmedIcon,
                    aPressedIcon,
                    aHoverIcon,
                    aText,
                    aHelpText,
                    aButtonFlags,
                    aStateFlags);
    iDelegate->SetContainerWindowL(*this);
    iDelegate->SetMopParent(this);
    iDelegate->SetObserver(this);

    CAknButtonState* aknState = iDelegate->State();
    if (aknState)
    {
        iDelegateState = CSwtButtonStateProxy::NewL(*aknState);
    }

    SetComponentsToInheritVisibility(ETrue);

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    // CAknButton generates more tactile feedbacks than needed.
    // So disabling all native feedback and only eSWT will generate
    // all the events.
    MTouchFeedback *feedback = MTouchFeedback::Instance();
    if (feedback)
    {
        feedback->EnableFeedbackForControl(iDelegate, EFalse);
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtButtonProxy::ConstructL(
    CCoeControl* aParent,
    const TDesC& aFilePath,
    const TInt aBmpId,
    const TInt aMaskId,
    const TInt aDimmedBmpId,
    const TInt aDimmedMaskId,
    const TInt aPressedBmpId,
    const TInt aPressedMaskId,
    const TInt aHoverBmpId,
    const TInt aHoverMaskId,
    const TDesC& aText,
    const TDesC& aHelpText,
    const TInt aButtonFlags,
    const TInt aStateFlags,
    const TAknsItemID& aId /*= KAknsIIDNone*/,
    const TAknsItemID& aDimmedId /*= KAknsIIDNone*/,
    const TAknsItemID& aPressedId /*= KAknsIIDNone*/,
    const TAknsItemID& aHoverId /*= KAknsIIDNone*/)
{
    if (!aParent)
    {
        User::Leave(KErrArgument);
    }

    SetContainerWindowL(*aParent);
    SetMopParent(aParent);

    iDelegate = CSwtCustomButton::NewL(
                    aFilePath,
                    aBmpId,
                    aMaskId,
                    aDimmedBmpId,
                    aDimmedMaskId,
                    aPressedBmpId,
                    aPressedMaskId,
                    aHoverBmpId,
                    aHoverMaskId,
                    aText,
                    aHelpText,
                    aButtonFlags,
                    aStateFlags,
                    aId,
                    aDimmedId,
                    aPressedId,
                    aHoverId);
    iDelegate->SetContainerWindowL(*this);
    iDelegate->SetMopParent(this);
    iDelegate->SetObserver(this);

    CAknButtonState* aknState = iDelegate->State();
    if (aknState)
    {
        iDelegateState = CSwtButtonStateProxy::NewL(*aknState);
    }

    SetComponentsToInheritVisibility(ETrue);

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    // CAknButton generates more tactile feedbacks than needed.
    // So disabling all native feedback and only eSWT will generate
    // all the events.
    MTouchFeedback *feedback = MTouchFeedback::Instance();
    if (feedback)
    {
        feedback->EnableFeedbackForControl(iDelegate, EFalse);
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
}

// ---------------------------------------------------------------------------
// CSwtButtonProxy::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtButtonProxy::ConstructL(CCoeControl* aParent, CGulIcon* aIcon,
                                 const TDesC& aText, const TInt aFlags)
{
    ConstructL(
        aParent,
        aIcon,
        NULL,       //aDimmedIcon,
        NULL,       //aPressedIcon
        NULL,       //aHoverIcon
        aText,
        KNullDesC,  //aHelpText
        aFlags,
        0);         //aStateFlags
}
