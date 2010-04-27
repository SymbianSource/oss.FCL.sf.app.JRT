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


#include <barsread.h>
#include <AknUtils.h>
#include "swteditindicator.h"


// ======== MEMBER FUNCTIONS ========


// Class CSwtEditIndicatorRedictor
CSwtEditIndicatorRedictor::CSwtEditIndicatorRedictor()
{
}

CSwtEditIndicatorRedictor::~CSwtEditIndicatorRedictor()
{
}


// Class SwtEditIndicator
CSwtEditIndicator* CSwtEditIndicator::NewL(CCoeControl& aControl)
{
    CSwtEditIndicator* self = new(ELeave) CSwtEditIndicator();
    CleanupStack::PushL(self);
    self->ConstructL(aControl);
    CleanupStack::Pop();    //self
    return self;
}

CSwtEditIndicator::CSwtEditIndicator()
{}

CSwtEditIndicator::~CSwtEditIndicator()
{
    delete iIndicatorContainer;
}

void CSwtEditIndicator::ConstructL(CCoeControl& aControl)
{
    iIndicatorContainer = new(ELeave) CAknIndicatorContainer(CAknIndicatorContainer::EQueryEditorIndicators);

    iIndicatorContainer->SetContainerWindowL(aControl);

    TResourceReader reader;
    aControl.ControlEnv()->CreateResourceReaderLC(reader, R_AVKON_NAVI_PANE_EDITOR_INDICATORS);
    iIndicatorContainer->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy();  // resource reader

    iIndicatorContainer->ActivateL();
}

void CSwtEditIndicator::SetState(TAknEditingState aState)
{
    // Copied from aknqueryeditorindicator.cpp

    if (AknLayoutUtils::Variant() == EEuropeanVariant)
    {
        TBool t9IsOn = ((aState >= ET9Upper) && (aState <= ET9Shifted) ||
                        aState == ET9Arabic || aState == ET9Hebrew);
        TBool upper = ((aState == ET9Upper) || (aState == EMultitapUpper));
        TBool lower = ((aState == ET9Lower) || (aState == EMultitapLower));
        TBool shifted = ((aState == ET9Shifted) || (aState == EMultitapShifted));
        TBool arabic = ((aState == ET9Arabic) || (aState == EMultitapArabic));
        TBool hebrew = ((aState == ET9Hebrew) || (aState == EMultitapHebrew));
        TBool farsiandurdu = ((aState == ET9FarsiAndUrdu) || (aState == EMultitapFarsiAndUrdu));

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
                     farsiandurdu
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorNumberCase,
                     (aState == ENumeric) ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorArabicIndicNumberCase,
                     (aState == EArabicIndicNumeric) ? EAknIndicatorStateOn : EAknIndicatorStateOff,
                     EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorQuery,
                     (t9IsOn || aState == EStateNone)  ?
                     EAknIndicatorStateOff : EAknIndicatorStateOn, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorT9,
                     t9IsOn ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
    }
    else    // APAC
    {
        TBool query = !(aState == EIndicatorStatePinyin ||
                        aState == EIndicatorStateZhuyin ||
                        aState == EIndicatorStateZhuyinFind ||
                        aState == EIndicatorStateStrokeFind ||
                        aState == EIndicatorStateStroke ||
                        aState == EIndicatorStateCangJie ||
                        aState == EIndicatorStateStrokeTrad ||
                        aState == EIndicatorStateJapanesePredictive);
        TBool thai = ((aState == ET9Thai) || (aState == EMultitapThai));

        SetIndicator(EAknNaviPaneEditorIndicatorTextCase,
                     (aState == EMultitapShifted)
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorLowerCase,
                     (aState == EMultitapLower)
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorUpperCase,
                     (aState == EMultitapUpper)
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorNumberCase,
                     (aState == ENumeric)
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff,  EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorQuery,
                     query && (aState != EStateNone)
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorT9,
                     !query && (aState != EStateNone)
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorThai,
                     thai
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorPinyin,
                     aState == EIndicatorStatePinyin
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorZhuyin,
                     aState == EIndicatorStateZhuyin
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        // for japanese
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseHiraganaKanji,
                     (aState == EIndicatorStateHiraganaKanji || aState == EIndicatorStateJapanesePredictive)
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullKatakana,
                     aState == EIndicatorStateFullKatakana
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseHalfKatakana,
                     aState == EIndicatorStateHalfKatakana
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullTextAlphabet,
                     aState == EIndicatorStateFullLatinText
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullUpperAlphabet,
                     aState == EIndicatorStateFullLatinUpper
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullLowerAlphabet,
                     aState == EIndicatorStateFullLatinLower
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseFullNumeric,
                     aState == EIndicatorStateFullNumeric
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorJapaneseHiragana,
                     aState == EIndicatorStateHiragana
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorStroke,
                     aState == EIndicatorStateStroke
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorCangJie,
                     aState == EIndicatorStateCangJie
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorStrokeTrad,
                     aState == EIndicatorStateStrokeTrad
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        SetIndicator(EAknNaviPaneEditorIndicatorZhuyinFind,
                     aState == EIndicatorStateZhuyinFind
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorStrokeFind,
                     aState == EIndicatorStateStrokeFind
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        TBool predictiveMode =
            ((aState == EIndicatorStatePinyin) ||
             (aState == EIndicatorStateZhuyin) ||
             (aState == EIndicatorStateZhuyinFind) ||
             (aState == EIndicatorStateStrokeFind) ||
             (aState == EIndicatorStateStroke) ||
             (aState == EIndicatorStateCangJie) ||
             (aState == EIndicatorStateStrokeTrad) ||
             (aState == ET9Upper) ||
             (aState == ET9Shifted) ||
             (aState == ET9Lower) ||
             (aState == ET9Arabic) ||
             (aState == ET9Hebrew) ||
             (aState == ET9Thai) ||
             (aState == EIndicatorStateJapanesePredictive) ||
             (aState == ET9UpperVietnamese) ||
             (aState == ET9LowerVietnamese) ||
             (aState == EMultitapShiftedVietnamese));

        SetIndicator(EAknNaviPaneEditorIndicatorT9, predictiveMode
                     ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

        // for Vietnamese
        TBool upperVietnamese = ((aState == ET9UpperVietnamese) || (aState == EMultitapUpperVietnamese));
        TBool lowerVietnamese = ((aState == ET9LowerVietnamese) || (aState == EMultitapLowerVietnamese));
        TBool shiftedVietnamese = ((aState == ET9ShiftedVietnamese) || (aState == EMultitapShiftedVietnamese));

        SetIndicator(EAknNaviPaneEditorIndicatorVietnameseTextCase,
                     shiftedVietnamese ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorVietnameseLowerCase,
                     lowerVietnamese ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
        SetIndicator(EAknNaviPaneEditorIndicatorVietnameseUpperCase,
                     upperVietnamese ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
    }
    SetIndicator(EAknNaviPaneEditorIndicatorQwertyShift,
                 aState == EQwertyShift ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);

    iIndicatorContainer->DrawDeferred();

}

CAknIndicatorContainer* CSwtEditIndicator::IndicatorContainer()
{
    return iIndicatorContainer;
}

void CSwtEditIndicator::SetIndicator(TInt aIndicator, TInt aState, TBool aDrawNow)
{
    iIndicatorContainer->SetIndicatorState(TUid::Uid(aIndicator), aState, aDrawNow);
}
