/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implements MID edit indicator container.
*
*/


// INTERNAL INCLUDES
#include "CMIDEditingStateIndicator.h"

// EXTERNAL INCLUDES
#include <MMIDCustomComponentContainer.h>
#include <AknUtils.h>
#include <barsread.h>
#include <j2me/jdebug.h>

namespace
{
const TInt KDefIndicatorHeight  = 25;
const TInt KColorOpaque         = 255;
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::NewL
// ---------------------------------------------------------------------------
//
CMIDEditingStateIndicator* CMIDEditingStateIndicator::NewL()
{
    CMIDEditingStateIndicator* self =
        new(ELeave) CMIDEditingStateIndicator();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::~CMIDEditingStateIndicator
// ---------------------------------------------------------------------------
//
CMIDEditingStateIndicator::~CMIDEditingStateIndicator()
{
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::SetState
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::SetState(TAknEditingState aState)
{
    DEBUG_INT("CMIDEditingStateIndicator::SetState +, aState=%d", aState);

    // Implementation from aknqueryeditorindicator.cpp

    // Report state change event if the actual editing state changes.
    if (aState != iEditingState)
    {
        // Leaves are ignored intentionally.
        TRAP_IGNORE(ReportEventL(
                        MCoeControlObserver::EEventStateChanged));
    }

    if (AknLayoutUtils::Variant() == EEuropeanVariant)
    {
        TBool t9IsOn = ((aState >= ET9Upper) && (aState <= ET9Shifted)
                        || aState == ET9FarsiAndUrdu || aState == ET9Arabic || aState
                        == ET9Hebrew || aState == ET9Thai || aState
                        == EIndicatorStatePredictiveHindi
#ifdef RD_MARATHI
                        || aState == EIndicatorStatePredictiveMarathi
#endif
                       );
        TBool upper = ((aState == ET9Upper) ||
                       (aState == EMultitapUpper));

        TBool lower = ((aState == ET9Lower) ||
                       (aState == EMultitapLower));

        TBool shifted = ((aState == ET9Shifted) ||
                         (aState == EMultitapShifted));

        TBool arabic = ((aState == ET9Arabic) ||
                        (aState == EMultitapArabic));

        TBool hebrew = ((aState == ET9Hebrew) ||
                        (aState == EMultitapHebrew));

        TBool farsiandurdu = ((aState == ET9FarsiAndUrdu) ||
                              (aState == EMultitapFarsiAndUrdu));

        TBool hindi = ((aState == EIndicatorStatePredictiveHindi) ||
                       (aState == EIndicatorStateMultitapHindi));

        TBool thai = ((aState == ET9Thai) || (aState == EMultitapThai));
#ifdef RD_MARATHI
        TBool marathi = ((aState == EIndicatorStatePredictiveMarathi) ||
                         (aState == EIndicatorStateMultitapMarathi));
#endif

        SetIndicator(EAknNaviPaneEditorIndicatorTextCase,
                     shifted ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorLowerCase,
                     lower ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorUpperCase,
                     upper ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorArabicCase,
                     arabic ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorHebrewCase,
                     hebrew ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorFarsiAndUrduCase,
                     farsiandurdu ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        // for Hindi ( One of a call for setindicator for Hindi Language needs
        // to be removed)
        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariCase,
                     hindi ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariIndicNumberCase,
                     (aState == EDevanagariIndicNumeric) ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorThai,
                     thai ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

#ifdef RD_MARATHI
        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariCase,
                     marathi ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariIndicNumberCase,
                     (aState == EDevanagariIndicNumeric) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);
#endif

#ifdef RD_HINDI_PHONETIC_INPUT
        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariPhoneticUpperCase,
                     (aState == EIndicatorStateHindiPhoneticUpper) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariPhoneticLowerCase,
                     (aState == EIndicatorStateHindiPhoneticLower) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);
#endif
        SetIndicator(EAknNaviPaneEditorIndicatorNumberCase,
                     (aState == ENumeric) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorArabicIndicNumberCase,
                     (aState == EArabicIndicNumeric) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorQuery,
                     (t9IsOn || aState == EStateNone) ?
                     EAknIndicatorStateOff : EAknIndicatorStateOn,
                     EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorT9,
                     t9IsOn ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
    }
    else // APAC
    {
        TBool query = !(aState == EIndicatorStatePinyin ||
                        aState == EIndicatorStateZhuyin ||
                        aState == EIndicatorStateZhuyinFind ||
                        aState == EIndicatorStateStrokeFind ||
                        aState == EIndicatorStateStroke ||
                        aState == EIndicatorStateCangJie ||
                        aState == EIndicatorStateStrokeTrad ||
                        aState == EIndicatorStatePinyinPhrase ||
                        aState == EIndicatorStateZhuyinPhrase ||
                        aState == EIndicatorStateStrokePhrase ||
                        aState == EIndicatorStateStrokeTradPhrase ||
                        aState == EIndicatorStateJapanesePredictive);

        TBool thai = ((aState == ET9Thai) || (aState == EMultitapThai));

        TBool hindi = ((aState == EIndicatorStatePredictiveHindi) ||
                       (aState == EIndicatorStateMultitapHindi));

#ifdef RD_MARATHI
        TBool marathi = ((aState == EIndicatorStatePredictiveMarathi) ||
                         (aState == EIndicatorStateMultitapMarathi));
#endif

        SetIndicator(EAknNaviPaneEditorIndicatorNumberCase,
                     (aState == ENumeric) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        TBool upper = ((aState == ET9Upper) ||
                       (aState == EMultitapUpper));

        TBool lower = ((aState == ET9Lower) ||
                       (aState == EMultitapLower));

        TBool shifted = ((aState == ET9Shifted) ||
                         (aState == EMultitapShifted));

        SetIndicator(EAknNaviPaneEditorIndicatorLowerCase,
                     lower ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorUpperCase,
                     upper ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorTextCase,
                     shifted ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        TBool predictiveMode = (
                                   (aState == EIndicatorStatePinyin) ||
                                   (aState == EIndicatorStateZhuyin) ||
                                   (aState == EIndicatorStateZhuyinFind) ||
                                   (aState == EIndicatorStateStrokeFind) ||
                                   (aState == EIndicatorStateStroke) ||
                                   (aState == EIndicatorStateCangJie) ||
                                   (aState == EIndicatorStateStrokeTrad) ||
                                   (aState == EIndicatorStatePinyinPhrase) ||
                                   (aState == EIndicatorStateZhuyinPhrase) ||
                                   (aState == EIndicatorStateStrokePhrase) ||
                                   (aState == EIndicatorStateStrokeTradPhrase) ||
                                   (aState == ET9Upper) ||
                                   (aState == ET9Shifted) ||
                                   (aState == ET9Lower) ||
                                   (aState == ET9Arabic) ||
                                   (aState == ET9Hebrew) ||
                                   (aState == ET9Thai) ||
                                   (aState == EIndicatorStatePredictiveHindi) ||
#ifdef RD_MARATHI
                                   (aState == EIndicatorStatePredictiveMarathi) ||
#endif
                                   (aState == EIndicatorStateJapanesePredictive) ||
                                   (aState == ET9UpperVietnamese) ||
                                   (aState == ET9LowerVietnamese) ||
                                   (aState == EMultitapShiftedVietnamese));

        SetIndicator(EAknNaviPaneEditorIndicatorQuery,
                     (!predictiveMode) && query && (aState != EStateNone) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorT9,
                     predictiveMode ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorThai,
                     thai ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorPinyin,
                     aState == EIndicatorStatePinyin ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorPinyinPhrase,
                     aState == EIndicatorStatePinyinPhrase ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorZhuyin,
                     aState == EIndicatorStateZhuyin ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorZhuyinPhrase,
                     aState == EIndicatorStateZhuyinPhrase ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        // for japanese
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseHiraganaKanji,
                     (aState == EIndicatorStateHiraganaKanji ||
                      aState == EIndicatorStateJapanesePredictive) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullKatakana,
                     aState == EIndicatorStateFullKatakana ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseHalfKatakana,
                     aState == EIndicatorStateHalfKatakana ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullTextAlphabet,
                     aState == EIndicatorStateFullLatinText ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullUpperAlphabet,
                     aState == EIndicatorStateFullLatinUpper ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullLowerAlphabet,
                     aState == EIndicatorStateFullLatinLower ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullNumeric,
                     aState == EIndicatorStateFullNumeric ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseHiragana,
                     aState == EIndicatorStateHiragana ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorStroke,
                     aState == EIndicatorStateStroke ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorStrokeTradPhrase,
                     aState == EIndicatorStateStrokeTradPhrase ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorStrokePhrase,
                     aState == EIndicatorStateStrokePhrase ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorCangJie,
                     aState == EIndicatorStateCangJie ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorStrokeTrad,
                     aState == EIndicatorStateStrokeTrad ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorZhuyinFind,
                     aState == EIndicatorStateZhuyinFind ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorStrokeFind,
                     aState == EIndicatorStateStrokeFind ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

        // for Hindi ( One of a call for setindicator for Hindi Language needs
        // to be removed)
        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariCase,
                     hindi ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariIndicNumberCase,
                     (aState == EDevanagariIndicNumeric) ? EAknIndicatorStateOn
                     : EAknIndicatorStateOff, EFalse);

#ifdef RD_MARATHI
        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariCase,
                     marathi ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorDevanagariIndicNumberCase,
                     (aState == EDevanagariIndicNumeric) ?
                     EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);
#endif
    }

    SetIndicator(EAknNaviPaneEditorIndicatorQwertyShift,
                 aState == EQwertyShift ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                 EFalse);

#ifdef RD_INTELLIGENT_TEXT_INPUT
    SetIndicator(EAknNaviPaneEditorIndicatorFnKeyPressed,
                 (aState == EFnKeyPressed) ?
                 EAknIndicatorStateOn : EAknIndicatorStateOff,
                 EFalse);

    SetIndicator(EAknNaviPaneEditorIndicatorFnKeyLocked,
                 (aState == EFnKeyLocked) ?
                 EAknIndicatorStateOn : EAknIndicatorStateOff,
                 EFalse);
#endif

    // for Vietnamese
    TBool upperVietnamese = ((aState == ET9UpperVietnamese) ||
                             (aState == EMultitapUpperVietnamese));

    TBool lowerVietnamese = ((aState == ET9LowerVietnamese) ||
                             (aState == EMultitapLowerVietnamese));

    TBool shiftedVietnamese = ((aState == ET9ShiftedVietnamese) ||
                               (aState == EMultitapShiftedVietnamese));

    SetIndicator(EAknNaviPaneEditorIndicatorVietnameseTextCase,
                 shiftedVietnamese ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                 EFalse);

    SetIndicator(EAknNaviPaneEditorIndicatorVietnameseLowerCase,
                 lowerVietnamese ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                 EFalse);

    SetIndicator(EAknNaviPaneEditorIndicatorVietnameseUpperCase,
                 upperVietnamese ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                 EFalse);

    if (CAknEnv::Static()->TransparencyEnabled())
    {
        CWindowGc& controlGc = SystemGc();
        CWindowGc& systemGc = iCoeEnv->SystemGc();

        // If Gc's do not match there is ongoing transition effect.
        // Drawing indicator will cause background flicker in that
        // situation.
        if (&controlGc == &systemGc)
        {
            Redraw();
        }
    }
    else
    {
        Redraw();
    }

    iEditingState = aState;

    DEBUG("CMIDEditingStateIndicator::SetState -");
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::IndicatorContainer
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
CAknIndicatorContainer* CMIDEditingStateIndicator::IndicatorContainer()
{
    DEBUG("CMIDEditingStateIndicator::IndicatorContainer");

    return this;
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::SupplyIndicatorMopObject
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CMIDEditingStateIndicator::SupplyIndicatorMopObject(
    TTypeUid aId)
{
    if (iState != EIndicatorStateDisabled)
    {
        return SupplyMopObject(aId, this);
    }

    return TTypeUid::Null();
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::SetPosition
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::SetPosition(TInt aX, TInt aY)
{
    DEBUG_INT2("CMIDEditingStateIndicator::SetPosition +, aX=%d, aY=%d",
               aX, aY);

    CCoeControl::SetPosition(TPoint(aX, aY));

    // Issue redraw.
    Redraw();

    DEBUG("CMIDEditingStateIndicator::SetPosition -");
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::SetVisibleL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::SetVisibleL(TBool aVisible)
{
    DEBUG_INT("CMIDEditingStateIndicator::SetVisibleL +, aVisible=%d",
              aVisible);

    // Make visible only when enabled (i.e. custom position set or
    // relative with the text editor component.
    if (iState != EIndicatorStateDisabled)
    {
        MakeVisible(aVisible);
    }

    // Issue redraw.
    Redraw();

    DEBUG("CMIDEditingStateIndicator::SetVisibleL -");
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::EnabledState
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
CMIDEditingStateIndicator::TIndicatorState
CMIDEditingStateIndicator::EnabledState() const
{
    return iState;
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::SetEnabledState
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::SetEnabledState(
    CMIDEditingStateIndicator::TIndicatorState aState)
{
    iState = aState;

    // Hide the indicator container if it is set to disabled.
    if (iState == EIndicatorStateDisabled)
    {
        MakeVisible(EFalse);
    }
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::Redraw
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::Redraw() const
{
    if (!iParent)
    {
        return;
    }

    // If the indicator has a transaprent background, the parent
    // needs to be redrawn in that case.
    if (iBackgroundColor.Alpha() < KColorOpaque)
    {
        iParent->DrawDeferred();
    }
    else
    {
        // No transparency. Draw this control only.
        DrawDeferred();
    }
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::PositionChanged
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::OverrideColorL(
    TInt aLogicalColor, TRgb aColor)
{
    CCoeControl::OverrideColorL(aLogicalColor, aColor);

    if (aLogicalColor == EColorControlBackground)
    {
        iBackgroundColor = aColor;
    }
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::SetSize
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::SetSize(TInt aWidth, TInt /*aHeight*/)
{
    DEBUG_INT("CMIDEditingStateIndicator::SetSize +, aWidth=%d", aWidth);

    CAknIndicatorContainer::SetSize(
        TSize(aWidth, KDefIndicatorHeight));

    DEBUG("CMIDEditingStateIndicator::SetSize -");
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::Draw
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::Draw(const TRect& aRect) const
{
    DEBUG("CMIDEditingStateIndicator::Draw +");

    CWindowGc& gc = SystemGc();

    // If scaling is on, clip to canvas rect is needed.
    if (iIsScalingOn)
    {
        gc.SetClippingRect(iOnScreenCanvasRect);
    }

    // Draw background with background color
    TRgb backgroundColor;
    GetColor(EColorControlBackground, backgroundColor);
    gc.SetBrushColor(backgroundColor);
    gc.Clear(aRect);

    // cancel of clip
    if (iIsScalingOn)
    {
        gc.CancelClippingRect();
    }

    DEBUG("CMIDEditingStateIndicator::Draw -");
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::Reserved_1
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::Reserved_1()
{
    // Needed due to linking issues.
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::CMIDEditingStateIndicator
// ---------------------------------------------------------------------------
//
CMIDEditingStateIndicator::CMIDEditingStateIndicator() :
        CAknIndicatorContainer(CAknIndicatorContainer::EQueryEditorIndicators)
{
    DEBUG("CMIDEditingStateIndicator::CMIDEditingStateIndicator");
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::SetContainerWindowL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::SetContainerWindowL(CCoeControl* aParent)
{
    DEBUG("CMIDEditingStateIndicator::SetContainerWindowL +");

    // Implementation from aknqueryeditorindicator.cpp ConstructL

    if (CAknEnv::Static()->TransparencyEnabled())
    {
        DEBUG("CMIDEditingStateIndicator::SetContainerWindowL, transparent");

        CAknIndicatorContainer::SetContainerWindowL(*aParent);
    }
    else
    {
        CreateWindowL(aParent);
    }

    ActivateL();
    iParent = aParent;

    DEBUG("CMIDEditingStateIndicator::SetContainerWindowL -");
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::SetIndicator
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::SetIndicator(
    TInt aIndicator,
    TInt aState,
    TBool aDrawNow)
{
    SetIndicatorState(TUid::Uid(aIndicator), aState, aDrawNow);
}

// ---------------------------------------------------------------------------
// CMIDEditingStateIndicator::ConstructL
// (other items were commented in a header
// ---------------------------------------------------------------------------
//
void CMIDEditingStateIndicator::ConstructL()
{
    DEBUG("CMIDEditingStateIndicator::ConstructL +");

    // Implementation from aknqueryeditorindicator.cpp
    TResourceReader reader;

    iCoeEnv->CreateResourceReaderLC(
        reader, R_AVKON_NAVI_PANE_EDITOR_INDICATORS);

    ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader

    DEBUG("CMIDEditingStateIndicator::ConstructL -");
}

// End of file
