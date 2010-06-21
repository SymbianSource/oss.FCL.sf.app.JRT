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


#include <eswtcore.rsg>
#include <AknLayoutFont.h>
#include <AknsControlContext.h>
#include <AknsDrawUtils.h>
#include <avkon.rsg>
#include <AknUtils.h>
#include <AknDef.h>
#ifdef RD_JAVA_S60_RELEASE_9_2
#include <AknPriv.hrh>
#endif
#include <swtlaffacade.h>
#if defined( RD_SCALABLE_UI_V2 )
#include <swtbuttonproxy.h>
#include <aknconsts.h>
#include <avkon.mbg>
#include <fepbase.h>
#endif // RD_SCALABLE_UI_V2
#include "swteditindicator.h"
#include "swtfont.h"
#include "swtdateeditor.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::NewL
// ---------------------------------------------------------------------------------------------
//
CSwtDateEditor* CSwtDateEditor::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                     MSwtComposite& aParent, TInt aStyle)
{
    CSwtDateEditor* self = new(ELeave) CSwtDateEditor(aDisplay, aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::CSwtDateEditor
// ---------------------------------------------------------------------------------------------
//
CSwtDateEditor::CSwtDateEditor(MSwtDisplay& aDisplay,
                               TSwtPeer aPeer,
                               MSwtComposite& aParent,
                               TInt aStyle)
        : ASwtControlBase(aDisplay, aPeer, &aParent, aStyle)
        , iCurrentFieldLength(0)
        , iCurrentFieldDecimalPlacesCount(0)
        , iLastField(0)
{
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::~CSwtDateEditor
// ---------------------------------------------------------------------------------------------
//
CSwtDateEditor::~CSwtDateEditor()
{
    delete iEditor;
    delete iIndicator;
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
    }

#ifdef _lint
    iEditor      = NULL;
    iDefaultFont = NULL;
#endif
}


// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::ConstructL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::ConstructL()
{
    ASSERT(iParent);
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);
    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());
    SetComponentsToInheritVisibility(ETrue);

    CSwtEditIndicatorRedictor* redirector = MopGetObject(redirector);
    if (redirector)
    {
        iIndicator = CSwtEditIndicator::NewL(CoeControl());
    }

    // Read the Java Epoc date in the resource file
    HBufC* minDateString = iEikonEnv->AllocReadResourceLC(R_SWT_DATEEDITOR_MIN_DATE);
    iEpocDateInMicroSec = TTime(minDateString->Des()).Int64();
    CleanupStack::PopAndDestroy(minDateString);

    SetBackground(this);   // Back will be drawn by ASwtControlBase::Draw
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::RetrieveDefaultFontL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::RetrieveDefaultFontL()
{
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }
    // Not same but similar to Text's font. Height most probably the same.
    const CFont* font = AknLayoutUtils::FontFromId(CSwtLafFacade::GetFontId(
                            CSwtLafFacade::EFont2MidpTimePaneT1Font));
    ASSERT(font);
    iDefaultFont = CSwtFont::NewL(iDisplay.Device(), *font);

    if (!iFont)
    {
        // to avoid the default font of CEikEdwin
        ProcessFontUpdate();
    }
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::ProcessFontUpdate
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::ProcessFontUpdate()
{
    const MSwtFont* swtFont=GetFont();
    ASSERT(swtFont);
    const CFont& font = (swtFont->Font());
    iEditor->SetFont(&font);
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::PrepareForFocusChangeL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::PrepareForFocusChangeL()
{
    if (IsFocused())
    {
        iEditor->PrepareForFocusGainL();
    }
    else
    {
        iEditor->PrepareForFocusLossL();
        if (iIndicator)
        {
            iIndicator->SetState(EStateNone);
        }
    }
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::CountComponentControls
// ---------------------------------------------------------------------------------------------
//
TInt CSwtDateEditor::CountComponentControls() const
{
    TInt count(0);
    if (iEditor)
    {
        count = 1;
        if (iIndicator)
        {
            count++;
        }
    }
    return count;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::ComponentControl
// ---------------------------------------------------------------------------------------------
//
CCoeControl* CSwtDateEditor::ComponentControl(TInt aIndex) const
{
    ASSERT(iEditor);
    if (aIndex == 0)
    {
        return iEditor;
    }
    return iIndicator->IndicatorContainer();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::FocusChanged
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::FocusChanged(TDrawNow aDrawNow)
{
    if (iEditor)
    {
        TRAP_IGNORE(PrepareForFocusChangeL());
        iEditor->SetFocus(IsFocused());
    }

    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::MakeVisible
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::MakeVisible(TBool aVisible)
{
    // Close VKB. Do it here instead of FocusChange to avoid split input flicker.
    if (iEditor && iEditor->IsFocused() && !aVisible)
    {
        CCoeFep* fep = iDisplay.CoeEnv()->Fep();
        if (fep)
        {
            fep->HandleDestructionOfFocusedItem();
        }
    }
    CCoeControl::MakeVisible(aVisible);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SetBackgroundL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SetBackgroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetBackgroundL(aColor);
    TRgb color((aColor) ? aColor->RgbValue() : iEikonEnv->Color(EColorControlBackground));
    iEditor->OverrideColorL(EColorControlBackground, color);

    // While background color is set, the editor's control context must be null,
    // otherwise there will be ugly background drawing problems while typing.
    // It looks like there is no need to override FocusBackgroundPolicy,
    // SetUseOverrideColors causes the editor to ignore the skin back.
    // If a foreground color has been set, cannot go back to skin colors yet.
    if (!iFgColorIsCustom)
    {
        if (aColor)
        {
            iEditor->SetUseOverrideColors(ETrue);
        }
        else
        {
            iEditor->SetUseOverrideColors(EFalse);
        }
    }

    if (aColor)
    {
        iBgColorIsCustom = ETrue;
    }
    else
    {
        iBgColorIsCustom = EFalse;
    }

    Redraw();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SetForegroundL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SetForegroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetForegroundL(aColor);

    TRgb color((aColor) ? aColor->RgbValue() : iEikonEnv->Color(EColorControlText));
    iEditor->OverrideColorL(EColorControlText, color);

    // If a back color has been set, cannot go back to skin colors yet.
    if (!iBgColorIsCustom)
    {
        if (aColor)
        {
            iEditor->SetUseOverrideColors(ETrue);
        }
        else
        {
            iEditor->SetUseOverrideColors(EFalse);
        }
    }

    if (aColor)
    {
        iFgColorIsCustom = ETrue;
    }
    else
    {
        iFgColorIsCustom = EFalse;
    }

    Redraw();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::OfferKeyEventL
// ---------------------------------------------------------------------------------------------
//
TKeyResponse CSwtDateEditor::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    TBool traversalDoIt = ETrue;

    if (iEditor)
    {
        switch (aKeyEvent.iCode)
        {
        case EKeyUpArrow:
            traversalDoIt = ETrue;
            break;
        case EKeyDownArrow:
            traversalDoIt = ETrue;
            break;
        case EKeyRightArrow:
            traversalDoIt = ((iEditor->CurrentField() == iEditor->NumFields()-1) &&
                             (GetShell().FindTraversalTargetL(ESwtTraverseArrowNext, *this)));
            break;
        case EKeyLeftArrow:
            traversalDoIt = (iEditor->CurrentField() == 0 &&
                             GetShell().FindTraversalTargetL(ESwtTraverseArrowNext, *this));
            break;
        default:
            traversalDoIt = ETrue;
        }
    }

    return ASwtControlBase::HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::ProcessKeyEventL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (!iEditor)
    {
        return;
    }

    if (aType != EEventKey)
        return;

    TInt code = aKeyEvent.iCode;
    TBool amPmField = IsFieldAmPmField(iEditor->CurrentField());
    TBool offsetStyleField = IsFieldOffsetStyleField(iEditor->CurrentField());

    TBool isFieldNumeric = !amPmField && !offsetStyleField;
    TBool wasFieldChangedAlready = IsCurrentFieldChanged(isFieldNumeric);
    UpdateFieldCounters(isFieldNumeric, wasFieldChangedAlready);

    iEditor->OfferKeyEventL(aKeyEvent, aType);
    //MSK label update may be needed.
    if (AknLayoutUtils::MSKEnabled())
    {
        MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
        if (commandArranger)
        {
            commandArranger->UpdateMSKLabelL();
        }
    }
    // End MSK
    if (aKeyEvent.iCode == EKeyEnter)
    {
        iDisplay.PostDefaultSelectionEventL(iPeer);
    }

    TBool isModifyEventAllowed = IsModifyEventAllowed(code, wasFieldChangedAlready);

    if (isModifyEventAllowed || amPmField || offsetStyleField)
    {
        iDisplay.PostModifyEventL(iPeer);
    }
}


// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::IsFieldChanged
// ---------------------------------------------------------------------------------------------
//

TBool CSwtDateEditor::IsCurrentFieldChanged(TBool aIsFieldNumeric)
{
    TBool isFieldChangedAlready = ETrue;
    if (aIsFieldNumeric)
    {
        isFieldChangedAlready = CEikMfneField::EInverseVideo !=
                                iEditor->Field(iEditor->CurrentField())->HighlightType();
    }
    return isFieldChangedAlready;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::IsFieldChanged
// ---------------------------------------------------------------------------------------------
//

void CSwtDateEditor::UpdateFieldCounters(TBool aIsFieldNumeric, TBool aIsFieldChanged)
{
    if (aIsFieldNumeric)
    {
        if (!aIsFieldChanged)
        {
            TBuf<16> actualText;
            actualText.AppendNum(
                static_cast<CEikMfneNumber*>(iEditor->Field(iEditor->CurrentField()))->Value());
            iCurrentFieldDecimalPlacesCount = actualText.Length();
            iCurrentFieldLength = 0;
        }
    }
    else
    {
        iCurrentFieldLength = 0;
        iCurrentFieldDecimalPlacesCount = 0;
    }
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::IsModifyEventAllowed
// ---------------------------------------------------------------------------------------------
//
TBool CSwtDateEditor::IsModifyEventAllowed(TInt aKeyCode, TBool aWasFieldChangedAlready)
{
    TInt fieldMinimumValue = 0;
    TInt fieldMaximumValue = 0;
    CEikMfneNumber* currentField = static_cast<CEikMfneNumber*>(iEditor->Field(iEditor->CurrentField()));
    currentField->GetMinimumAndMaximum(fieldMaximumValue, fieldMaximumValue);
    TBuf<16> minText;
    minText.AppendNum(fieldMinimumValue);
    TBuf<16> maxText;
    maxText.AppendNum(fieldMaximumValue);
    TInt maxDigits = Max(2, Max(minText.Length(), maxText.Length()));
    TBool isZeroChar = 0 == (aKeyCode - TText(currentField->DigitType()));
    TBool raiseEvent = ETrue;
    TBool isArrowKey = (aKeyCode == EKeyLeftArrow) || (aKeyCode == EKeyRightArrow);
    TChar ch = TChar(aKeyCode);

    if (iEditor->CurrentField() != iLastField || isArrowKey)
    {
        iCurrentFieldLength = 0;
        iCurrentFieldDecimalPlacesCount = 0;
        if (isArrowKey && !aWasFieldChangedAlready)
        {
            raiseEvent = EFalse;
        }
    }
    else if (EKeyBackspace == aKeyCode)
    {
        if (!aWasFieldChangedAlready)
        {
            iCurrentFieldLength = maxDigits;
        }

        iCurrentFieldLength--;
        iCurrentFieldDecimalPlacesCount--;
        if (iCurrentFieldDecimalPlacesCount < 1)
        {
            raiseEvent = EFalse;
        }
    }
    else if (ch.IsDigit())
    {
        iCurrentFieldLength++;
        if (iCurrentFieldLength >= maxDigits)
        {
            iCurrentFieldLength = 0;
            iCurrentFieldDecimalPlacesCount = 0;
        }
        else
        {
            raiseEvent = !isZeroChar || (iCurrentFieldDecimalPlacesCount > 0);
        }
        if (!aWasFieldChangedAlready)
        {
            iCurrentFieldDecimalPlacesCount = 0;
        }
        if (!isZeroChar || (iCurrentFieldDecimalPlacesCount > 0 && isZeroChar))
        {
            iCurrentFieldDecimalPlacesCount++;
        }
    }
    else
    {
        raiseEvent = false;
    }

    iLastField = iEditor->CurrentField();

    return raiseEvent;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::IsFieldAmPmField
// ---------------------------------------------------------------------------------------------
//
TBool CSwtDateEditor::IsFieldAmPmField(TInt fieldIndex) const
{
    // current field is am/pm field if it accepts something else than positive integers
    TBool amPmField = (iDateEditorStyle == MSwtDateEditor::ESwtTime ||
                       iDateEditorStyle == MSwtDateEditor::ESwtDateTime)  &&
                      (iEditor->Field(fieldIndex)->InputCapabilities().Capabilities() &
                       ~TCoeInputCapabilities::EWesternNumericIntegerPositive);
    return amPmField;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::IsFieldOffsetStyleField
// ---------------------------------------------------------------------------------------------
//
TBool CSwtDateEditor::IsFieldOffsetStyleField(TInt fieldIndex) const
{
    // current field is +/- field if it accepts something else than positive integers
    TBool offsetStyleField = (iDateEditorStyle == MSwtDateEditor::ESwtOffset)  &&
                             (iEditor->Field(fieldIndex)->InputCapabilities().Capabilities() &
                              ~TCoeInputCapabilities::EWesternNumericIntegerPositive);
    return offsetStyleField;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SetDimmed
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SetDimmed(TBool aDimmed)
{
    ASSERT(iEditor);

    // Close VKB. Do it here instead of FocusChange to avoid split input flicker.
    if (iEditor && iEditor->IsFocused() && aDimmed)
    {
        CCoeFep* fep = iDisplay.CoeEnv()->Fep();
        if (fep)
        {
            fep->HandleDestructionOfFocusedItem();
        }
    }

    CCoeControl::SetDimmed(aDimmed);
    iEditor->SetDimmed(aDimmed);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::HandleResourceChange
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SwtHandleResourceChangeL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SwtHandleResourceChangeL(TInt aType)
{
    CAknControl::HandleResourceChange(aType);

    if (aType == KAknsMessageSkinChange
            || aType == KEikDynamicLayoutVariantSwitch)
    {
        if (!iFont)
        {
            RetrieveDefaultFontL();
        }
        else
        {
            ProcessFontUpdate();
        }
        SizeChanged();
    }
#ifdef RD_JAVA_S60_RELEASE_9_2
    else if (aType == KAknSplitInputEnabled)
    {
        const MSwtShell* activeShell = iDisplay.UiUtils().GetActiveShell();
        if (activeShell && activeShell->FocusControl() == this)
        {
            iDisplay.UiUtils().SetSplitInputEditor(this);
        }
    }
#endif
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SizeChanged
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SizeChanged()
{
    if (!iEditor)
    {
        return;
    }

    // Update component size
    // Place the Editor at the center of the SwtDateEditor
    TSize swtSize(BorderInnerRect().Size());
    TPoint swtPos(Position());

    if (iIndicator)
    {
        const TPoint indPos(swtPos.iX + GetBorderWidth(),
                            swtPos.iY + GetBorderWidth());
        iIndicator->IndicatorContainer()->SetPosition(indPos);

        const TSize indicatorSize(swtSize.iWidth, KSWTEditIndicatorHeight);
        iIndicator->IndicatorContainer()->SetSize(indicatorSize);
        swtSize.iHeight -= indicatorSize.iHeight;
        swtPos.iY += indicatorSize.iHeight;
    }

    // The native editor adds these margins, so we have to consider them also.
    TInt topMargin(0);
    TInt bottomMargin(0);
    TAknTextDecorationMetrics metrics(iEditor->Font());
    metrics.GetTopAndBottomMargins(topMargin, bottomMargin);

    // MinimumSize returns the actual size of the control, so we need to set its size,
    // otherwise we will always receive 0.
    iEditor->SetSize(TSize(iEditor->Size().iWidth,
                           iEditor->Font()->FontMaxHeight() + topMargin + bottomMargin));
    TSize editorSize(iEditor->MinimumSize());

    TSize buttonEdSiz(editorSize);
    TPoint editorPos(0, 0);

    // calculates horizontal alignment of date in the editor
    if (swtSize.iWidth > buttonEdSiz.iWidth)
    {
        editorPos.iX += swtPos.iX + static_cast<TInt>((swtSize.iWidth-
                        buttonEdSiz.iWidth)/2.0) ;
    }
    else
    {
        editorPos.iX += swtPos.iX;
    }

    // calculates vertical alignment of date in the editor
    if (swtSize.iHeight > buttonEdSiz.iHeight)
    {
        editorPos.iY = swtPos.iY + static_cast<TInt>((swtSize.iHeight-
                       buttonEdSiz.iHeight)/2.0) ;
    }
    else
    {
        editorPos.iY = swtPos.iY;
    }

    iEditor->SetPosition(TPoint(editorPos.iX + GetBorderWidth(), editorPos.iY + GetBorderWidth()));

    if (iEditor->SupportsFeature(CEikMfne::EClipGcToRect))
    {
        iEditor->SetFeature(CEikMfne::EClipGcToRect, ETrue);
    }

    // places the editor on the center
    TSize newEditorSize(
        (swtSize.iWidth > editorSize.iWidth) ? editorSize.iWidth : swtSize.iWidth,
        (swtSize.iHeight > editorSize.iHeight) ? editorSize.iHeight : swtSize.iHeight);

    iEditor->SetSize(newEditorSize);

    HandleSizeChanged();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::PositionChanged
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::PositionChanged()
{
    HandlePositionChanged();
    // Notify change to UiUtils if this is an editor open for split view editing.
    MSwtUiUtils& utils = iDisplay.UiUtils();
    if (utils.SplitInputView() == this)
    {
        utils.AdjustSplitInputShellPos();
    }
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::MopSupplyObject
// ---------------------------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtDateEditor::MopSupplyObject(TTypeUid aId)
{
    if (iIndicator && aId.iUid == MAknEditingStateIndicator::ETypeId)
    {
        return aId.MakePtr(static_cast<MAknEditingStateIndicator*>(iIndicator));
    }
    TTypeUid::Ptr id = ASwtControlBase::SwtMopSupplyObject(aId);
    if (!id.Pointer())
    {
        return CCoeControl::MopSupplyObject(aId);
    }
    return id;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::CoeControl
// ---------------------------------------------------------------------------------------------
//
CCoeControl& CSwtDateEditor::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::CoeControl
// ---------------------------------------------------------------------------------------------
//
const CCoeControl& CSwtDateEditor::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::ComputeSizeL
// ---------------------------------------------------------------------------------------------
//
TSize CSwtDateEditor::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    if (!iEditor)
    {
        // iEditor could be null if the CSwtDateEditor is contained in a CaptionedControl.
        // When the method CaptionedControl::RegisterChildL() is called,
        // a SetSize() is made on its children, hence a call to ComputeSizeL().
        return TSize(0, 0);
    }

    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return TSize(aWHint, aHHint);
    }

    TSize size(iEditor->MinimumSize());
    if (iIndicator)
    {
        size.iHeight += KSWTEditIndicatorHeight;
    }
    size.iHeight = Max(iEditor->Font()->FontMaxHeight(), size.iHeight);
    if (GetFont() == &DefaultFont())
    {
        size.iHeight = Max(iDisplay.UiUtils().InlineReferenceFontHeight(), size.iHeight);
    }
    size += iDisplay.UiUtils().InlineReferencePadding().SizeDelta();
    size += BorderSizeDelta();

    if (aWHint != KSwtDefault)
    {
        size.iWidth = aWHint;
    }
    if (aHHint != KSwtDefault)
    {
        size.iHeight = aHHint;
    }

    return size;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SetFontL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SetFontL(const MSwtFont* aFont)
{
    // Does nothing. The font of the native editor is managed by the system.
    // It can't be modified.
    ASwtControlBase::DoSetFontL(aFont);
    if (iEditor)
    {
        ProcessFontUpdate();
    }
    SizeChanged();
    Redraw();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::DefaultFont
// ---------------------------------------------------------------------------------------------
//
const MSwtFont& CSwtDateEditor::DefaultFont() const
{
    return *iDefaultFont;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::Control
// ---------------------------------------------------------------------------------------------
//
MSwtControl* CSwtDateEditor::Control()
{
    return static_cast<MSwtControl*>(this);
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::GetDate
// ---------------------------------------------------------------------------------------------
//
TInt64 CSwtDateEditor::GetDate()
{
    ASSERT(iEditor);
    TInt64 result(0);

    switch (iDateEditorStyle)
    {
    case ESwtDate:
    {
        // The Date style contains only day infos. No time infos.
        // So we get the original time set in Java, and update only the day.
        // Thus, no need to take TimeZone into account.
        TTime getDate(static_cast<CEikDateEditor*>(iEditor)->Date());
        TDateTime javaDateTime(TTime(iJavaDateInMicroSec).DateTime());
        TDateTime onlyTime(0, EJanuary, 0, javaDateTime.Hour(), javaDateTime.Minute(),
                           javaDateTime.Second(), javaDateTime.MicroSecond());
        TTime clockInTTime(onlyTime);
        result = clockInTTime.Int64() + getDate.Int64() - iEpocDateInMicroSec;
    }
    break;

    case ESwtTime:
    {
        TTime getTime(static_cast<CEikTimeEditor*>(iEditor)->Time());
        TDateTime javaDateTime(TTime(iJavaDateInMicroSec).DateTime());
        TDateTime onlyDay(javaDateTime.Year(), javaDateTime.Month(),
                          javaDateTime.Day(), 0, 0, 0, 0);
        TTime dayInTTime(onlyDay);
        result = dayInTTime.Int64() + getTime.Int64();
    }
    break;

    case ESwtDateTime:
        result = static_cast<CEikTimeAndDateEditor*>(iEditor)->TimeAndDate().Int64()
                 - iEpocDateInMicroSec;
        break;

    default:
        break;
    }

    return result;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::GetOffset
// ---------------------------------------------------------------------------------------------
//
TInt CSwtDateEditor::GetOffset() const
{
    ASSERT(iEditor);

    switch (iDateEditorStyle)
    {
    case ESwtDuration:
        return static_cast<CEikDurationEditor*>(iEditor)->Duration().Int();

    case ESwtOffset:
        return static_cast<CEikTimeOffsetEditor*>(iEditor)->TimeOffset().Int();

    case ESwtTime:
        return static_cast<TInt64>(static_cast<CEikTimeEditor*>(
                                       iEditor)->Time().Int64() / 1000000);

    default:
        break;
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SetDate
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SetDate(const TInt64& aJavaDate)
{
    ASSERT(iEditor);

    iJavaDateInMicroSec = aJavaDate;
    TTime date(iEpocDateInMicroSec + iJavaDateInMicroSec);

    switch (iDateEditorStyle)
    {
    case ESwtDate:
        static_cast<CEikDateEditor*>(iEditor)->SetDate(date);
        break;

    case ESwtTime:
    {
        TDateTime dateTime(date.DateTime());
        TInt64 onlyTimeInMicroSec(dateTime.Hour() * 3600
                                  + dateTime.Minute() * 60
                                  + dateTime.Second());
        onlyTimeInMicroSec = onlyTimeInMicroSec * 1000000;
        onlyTimeInMicroSec += dateTime.MicroSecond();
        static_cast<CEikTimeEditor*>(iEditor)->SetTime(TTime(onlyTimeInMicroSec));
    }
    break;

    case ESwtDateTime:
        static_cast<CEikTimeAndDateEditor*>(iEditor)->SetTimeAndDate(date);

        break;

    default:
        break;
    }
    Redraw();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SetDateEditorStyleL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SetDateEditorStyleL(TInt aDateEditorStyle)
{
    TTime initialDate;
    initialDate.HomeTime();
    iJavaDateInMicroSec = initialDate.Int64() - iEpocDateInMicroSec;

    // Create the editor, according to the style
    iDateEditorStyle  = aDateEditorStyle & 0xf;
    iDateEditorFormat = aDateEditorStyle & 0xf0;

    //Clear field counters
    iCurrentFieldLength = 0;
    iLastField = 0;
    iCurrentFieldDecimalPlacesCount = 0;

    TUint32 editorFlags = 0;
    if (iDateEditorFormat == ESwtCompact)
    {
        editorFlags = EEikTimeWithoutSecondsField;
    }

    switch (iDateEditorStyle)
    {
    case ESwtDate:
    {
        TBool withoutPopoutCalendar = EFalse;
        if (iDateEditorFormat == ESwtCompact)
        {
            withoutPopoutCalendar = ETrue;
        }
        iEditor = new(ELeave) CEikDateEditor();
        static_cast<CEikDateEditor*>(iEditor)->ConstructL(
            KAknMinimumDate, KAknMaximumDate, initialDate, withoutPopoutCalendar);
    }
    break;

    case ESwtTime:
    {
        iEditor = new(ELeave) CEikTimeEditor();
        static_cast<CEikTimeEditor*>(iEditor)->ConstructL(
            TTime(TInt64(0)),
            TTime(TInt64(KSwtSecondsInOneDay)*1000000), initialDate, editorFlags);
    }
    break;

    case ESwtDateTime:
    {
        iEditor = new(ELeave) CEikTimeAndDateEditor();
        static_cast<CEikTimeAndDateEditor*>(iEditor)->ConstructL(
            KAknMinimumDate, KAknMaximumDate, initialDate, editorFlags);
    }
    break;

    case ESwtDuration:
    {
        iEditor = new(ELeave) CEikDurationEditor();
        static_cast<CEikDurationEditor*>(iEditor)->ConstructL(
            TTimeIntervalSeconds(0),
            TTimeIntervalSeconds(KSwtSecondsInOneDay),
            TTimeIntervalSeconds(0), editorFlags);
    }
    break;

    case ESwtOffset:
    {
        iEditor = new(ELeave) CEikTimeOffsetEditor();
        static_cast<CEikTimeOffsetEditor*>(iEditor)->ConstructL(
            TTimeIntervalSeconds(-KSwtSecondsInOneDay),
            TTimeIntervalSeconds(KSwtSecondsInOneDay),
            TTimeIntervalSeconds(0), editorFlags);
    }
    break;

    default:
        break;
    }

    if (iEditor)
    {
        iEditor->SetContainerWindowL(*this);
        iEditor->SetFocusing(ETrue);
        iEditor->MakeVisible(IsVisible());
        iEditor->SetFocus(IsFocused());
        iEditor->SetDimmed(IsDimmed());
        RetrieveDefaultFontL();
        iEditor->SetSkinTextColorL(EAknsCIQsnTextColorsCG6);

#ifdef RD_JAVA_S60_RELEASE_9_2
        if (iEditor->SupportsFeature(CEikMfne::EPartialScreenInput))
        {
            iEditor->SetFeature(CEikMfne::EPartialScreenInput, ETrue);
        }
#endif
    }
    ActivateL();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SetOffset
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SetOffset(TInt aSeconds)
{
    switch (iDateEditorStyle)
    {
    case ESwtDuration:
        static_cast<CEikDurationEditor*>(iEditor)->SetDuration(
            TTimeIntervalSeconds(aSeconds));
        break;

    case ESwtOffset:
        static_cast<CEikTimeOffsetEditor*>(iEditor)->SetTimeOffset(
            TTimeIntervalSeconds(aSeconds));
        break;

    case ESwtTime:
    {
        aSeconds = aSeconds % 86400; // 24 hours.
        TInt hour = aSeconds / 3600;
        TInt minute = (aSeconds - hour * 3600) / 60;
        TInt second = aSeconds - 3600*hour - 60*minute;
        TDateTime datetime(0, EJanuary, 0, hour, minute, second, 0);
        TTime time(datetime);
        static_cast<CEikTimeEditor*>(iEditor)->SetTime(time);
    }
    break;

    default:
        break;
    }
    Redraw();

}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::GetMinimumAndMaximumAndInitialDatesForCalendarL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::GetMinimumAndMaximumAndInitialDatesForCalendarL(
    TTime &aMinimumDate,
    TTime &aMaximumDate,
    TTime &aInitialDate) const
{
    ASSERT(iEditor);

    switch (iDateEditorStyle)
    {
    case ESwtDateTime:
        static_cast<CEikTimeAndDateEditor*>(iEditor)->GetMinimumAndMaximum(
            aMinimumDate, aMaximumDate);
        aInitialDate = static_cast<CEikTimeAndDateEditor*>(iEditor)->TimeAndDate();
        break;

    default:
        break;
    }
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::SetDateFromCalendarAndDrawNow
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::SetDateFromCalendarAndDrawNow(const TTime &aDate)
{
    ASSERT(iEditor);

    switch (iDateEditorStyle)
    {
    case ESwtDateTime:
    {
        TDateTime current(static_cast<CEikTimeAndDateEditor*>(
                              iEditor)->TimeAndDate().DateTime());
        TDateTime setDateTime(aDate.DateTime());
        setDateTime.SetHour(current.Hour());
        setDateTime.SetMinute(current.Minute());
        setDateTime.SetSecond(current.Second());
        setDateTime.SetMicroSecond(current.MicroSecond());
        static_cast<CEikTimeAndDateEditor*>(iEditor)->SetTimeAndDate(
            TTime(setDateTime));
    }
    break;

    default:
        break;
    }
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::MSKLabelL
// ---------------------------------------------------------------------------------------------
//
HBufC* CSwtDateEditor::MSKLabelL() const
{
    if (iEditor && (IsFieldAmPmField(iEditor->CurrentField()) ||
                    IsFieldOffsetStyleField(iEditor->CurrentField())))
    {
        HBufC* label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_CHANGE);
        return label ;
    }
    return ASwtControlBase::MSKLabelL();
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::IsKeyUsed
// ---------------------------------------------------------------------------------------------
//
TBool CSwtDateEditor::IsKeyUsed(TUint aKeyCode) const
{
    if (iEditor && (IsFieldAmPmField(iEditor->CurrentField()) ||
                    IsFieldOffsetStyleField(iEditor->CurrentField())) && aKeyCode == EKeyOK)
    {
        return ETrue;
    }
    return EFalse;
}


void CSwtDateEditor::SetBounds(const TRect& aRect)
{
    // Divert the job to UiUtils if this is an editor open for split view editing.
    MSwtUiUtils& utils = iDisplay.UiUtils();
    if (utils.SplitInputView() == this)
    {
        utils.SetSplitInputViewSize(aRect.Size());
        SetLocation(aRect.iTl);
    }
    else
    {
        ASwtControlBase::SetBounds(aRect);
    }
}

void CSwtDateEditor::SetWidgetSize(const TSize& aSize)
{
    // Divert the job to UiUtils if this is an editor open for split view editing.
    MSwtUiUtils& utils = iDisplay.UiUtils();
    if (utils.SplitInputView() == this)
    {
        utils.SetSplitInputViewSize(aSize);
    }
    else
    {
        ASwtControlBase::SetWidgetSize(aSize);
    }
}

TSwtPeer CSwtDateEditor::Dispose()
{
    // Close VKB.
    if (iEditor && iEditor->IsFocused())
    {
        CCoeFep* fep = iDisplay.CoeEnv()->Fep();
        if (fep)
        {
            fep->HandleDestructionOfFocusedItem();
        }
    }
    return ASwtControlBase::Dispose();
}


// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::HandlePointerEventL
// ---------------------------------------------------------------------------------------------
//
#if defined( RD_SCALABLE_UI_V2 )
void CSwtDateEditor::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    TBool hit = ETrue;

#ifdef RD_JAVA_S60_RELEASE_9_2
    TBool isActiveSplitEditor = iDisplay.UiUtils().SplitInputEditor() == this;
    TBool pressed = iPressed;
    hit = Rect().Contains(aPointerEvent.iPosition);

    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        iPressed = !isActiveSplitEditor;
    else if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        iPressed = EFalse;
    else if (aPointerEvent.iType == TPointerEvent::EDrag)
        iPressed = hit && !isActiveSplitEditor;
#endif

    if (!(aPointerEvent.iType == TPointerEvent::EButton1Up
            && (iDisplay.RevertPointerEvent() || !hit)))
    {
        iEditor->HandlePointerEventL(aPointerEvent);

        //MSK label update may be needed.
        if (AknLayoutUtils::MSKEnabled())
        {
            MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
            if (commandArranger)
            {
                commandArranger->UpdateMSKLabelL();
            }
        }
        // End MSK
    }

    //Selected field might changed
    iCurrentFieldLength = 0;
    iCurrentFieldDecimalPlacesCount = 0;
    iLastField = iEditor->CurrentField();

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (pressed != iPressed)
        Redraw();
#endif

    PostMouseEventL(aPointerEvent);
}

// ---------------------------------------------------------------------------------------------
// CSwtDateEditor::HandleControlEventL
// ---------------------------------------------------------------------------------------------
//
void CSwtDateEditor::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}
#endif // RD_SCALABLE_UI_V2
