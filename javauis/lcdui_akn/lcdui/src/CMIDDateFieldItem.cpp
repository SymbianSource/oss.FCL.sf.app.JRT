/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#include <eikenv.h>
#include <eiklabel.h>
#include <eikon.hrh>
#include <AknLayoutFont.h>

// LAF API in several places
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknUtils.h>
#include "CMIDDateFieldItem.h"
// API used for retrieving commands count (in IsSelectable function)
#include "CMIDCommandList.h"
// API needed for iLabelControl (inherited from CMIDControlItem)
#include "CMIDItemLabel.h"
#include "CMIDDisplayable.h"
#include "CMIDUtils.h"
#include "CMIDTicker.h"
#include "CMIDUIManager.h"

#include <j2me/jdebug.h>

#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDDateFieldItem.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);


// Must be used in TIME case due to strange definition of KAknMinimumDate
#define KCMIDDateFieldMinTime (TTime(TDateTime(0, EJanuary, 0, 0, 0, 0, 0)))

#define KCMIDDateFieldEpochDate (TTime(TDateTime(1970, EJanuary, 1-1, 0, 0, 0, 0)))
_LIT(KTimeAndDateFieldSeparator," ");

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDDateFieldItem* CMIDDateFieldItem::NewL(
    const TDesC& aLabel, MMIDDateField::TInputMode aInputMode, CMIDUIManager* aUIManager)
{
    CMIDDateFieldItem* item = new(ELeave)CMIDDateFieldItem(aUIManager);
    CleanupStack::PushL(item);
    item->ConstructL(aLabel,aInputMode);
    CleanupStack::Pop(item);
    return item;
}

CMIDDateFieldItem::~CMIDDateFieldItem()
{
    if (iAmPmToggleCommand)
    {
        iAmPmToggleCommand->SetObserver(NULL);
        iAmPmToggleCommand->Dispose();
        iAmPmToggleCommand = NULL;
    }
    if (iEditor)
    {
#ifndef RD_JAVA_S60_RELEASE_9_2
        iEditor->SetFocus(ETrue);   // needed to notify observers about editor destruction
#endif // RD_JAVA_S60_RELEASE_9_2
        delete iEditor;
        iEditor = NULL;
    }
}

void CMIDDateFieldItem::SetDate(const TTime& aTime)
{
    TTime time = aTime;
    if (iInputMode == MMIDDateField::ETime)
    {
        TDateTime dateTime = aTime.DateTime();
        time = TTime(TDateTime(0, EJanuary, 0,
                               dateTime.Hour(),
                               dateTime.Minute(),
                               dateTime.Second(),
                               dateTime.MicroSecond()));
    }
    else
    {
        TDateTime dateTime = aTime.DateTime();
        if (dateTime.Year() == 0)
        { // Date is not set or DateField type is switched
            // from ETime input mode. Date part set to "zero epoch"
            TDateTime epochDate = KCMIDDateFieldEpochDate.DateTime();
            time = TTime(TDateTime(epochDate.Year(), // set to "zero epoch"
                                   epochDate.Month(),
                                   epochDate.Day(),
                                   dateTime.Hour(),
                                   dateTime.Minute(),
                                   dateTime.Second(),
                                   dateTime.MicroSecond()));
        }
    }

    iEditor->SetTTime(time);
    SetInitialized(EFalse);
}

TTime CMIDDateFieldItem::Date() const
{
    TInt err = KErrNone;
    TTime ret = KCMIDDateFieldEpochDate;
    TBool nonEmpty = IsDateTimeNonEmpty();

    if (nonEmpty)
    {
        // GetTTime leaves if there is zero in the day, month or year fields.
        TRAP(err, ret = iEditor->GetTTime());
    }

    if ((err != KErrNone) || !nonEmpty)
    {
        TRAP(err, iEditor->PrepareForFocusLossL())
        // GetTTime should not leave after calling PrepareForFocusLossL
        ret = iEditor->GetTTime();
    }

    return ret;
}

void CMIDDateFieldItem::SetUninitialized()
{
    iInitialised = EFalse;

    iEditor->SetTTime(KCMIDDateFieldEpochDate);

    if (iInputMode == MMIDDateField::ETime)
    {
        iEditor->SetTTime(KCMIDDateFieldMinTime);
        reinterpret_cast<CEikTimeEditor*>(iEditor)->SetUninitialised(ETrue);
    }
    else if (iInputMode == MMIDDateField::EDate)
    {
        reinterpret_cast<CEikDateEditor*>(iEditor)->SetUninitialised(ETrue);
    }
    else if (iInputMode == MMIDDateField::EDateTime)
    {
        reinterpret_cast<CEikTimeAndDateEditor*>(iEditor)->SetUninitialised(ETrue);
    }
    DoSafeDraw();
}

void CMIDDateFieldItem::SetInputModeL(MMIDDateField::TInputMode aInputMode)
{
    CEikTTimeEditor* editor = NULL;

    TBool validOldTime = IsDateTimeNonEmpty();
    TTime oldTime;
    if (validOldTime)
    {
        // GetTTime leaves if there is 0 in day, month or year fields
        TRAPD(err, oldTime = iEditor->GetTTime());
        if (err != KErrNone)
        {
            validOldTime = EFalse;
        }
    }

    switch (aInputMode)
    {
    case MMIDDateField::EDate:
    {
        CEikDateEditor* dateEditor = new(ELeave) CEikDateEditor();
        CleanupStack::PushL(dateEditor);
        dateEditor->ConstructL(KAknMinimumDate,KAknMaximumDate,KCMIDDateFieldEpochDate,ETrue);
        CleanupStack::Pop(dateEditor);
        dateEditor->SetUninitialised(ETrue);
        editor = dateEditor;
        break;
    }
    case MMIDDateField::ETime:
    {
        CEikTimeEditor* timeEditor = new(ELeave) CEikTimeEditor();
        CleanupStack::PushL(timeEditor);
        timeEditor->ConstructL(KCMIDDateFieldMinTime,
                               KAknMaximumDate,
                               KCMIDDateFieldMinTime,
                               EEikTimeWithoutSecondsField);
        CleanupStack::Pop(timeEditor);
        timeEditor->SetUninitialised(ETrue);
        editor = timeEditor;
        break;
    }
    case MMIDDateField::EDateTime:
    {
        CEikTimeAndDateEditor* timeDateEditor = new(ELeave) CEikTimeAndDateEditor();
        CleanupStack::PushL(timeDateEditor);
        TUint32 flags = EEikTimeWithoutSecondsField + EEikDateWithoutPopoutCalendar;
        TPtrC separatorPtr(KTimeAndDateFieldSeparator);
        HBufC* separator = separatorPtr.AllocLC();
        timeDateEditor->ConstructL(KAknMinimumDate,
                                   KAknMaximumDate,
                                   KCMIDDateFieldEpochDate,flags,separator);
        CleanupStack::Pop(separator); // Do not destroy separator as
        // CEikTimeAndDateEditor takes control of it
        CleanupStack::Pop(timeDateEditor);
        timeDateEditor->SetUninitialised(ETrue);
        editor = timeDateEditor;
        break;
    }
    default:
        ASSERT(EFalse);
    }

    delete iEditor;
    iEditor = editor;

    iInputMode = aInputMode;

    // Set font from LAF to the editor, so that MinimumSize() will return correct values.
    // AknLayoutUtils::LayoutMfne() will also set the font from LAF to the editor
    iEditor->SetFont(AknLayoutUtils::FontFromId(
                         AknLayoutScalable_Avkon::form2_midp_time_pane_t1().Font()));

    iEditor->SetBorder(TGulBorder::ENone);


    if (iForm)
    {
        iEditor->SetContainerWindowL(*this);
        iEditor->ActivateL();
    }

    if (validOldTime)
    {
        SetDate(oldTime);
        DoSafeDraw();
    }
    else
    {
        iInitialised = EFalse;
    }

    // Text colour from skin
    iEditor->SetSkinTextColorL(EAknsCIQsnTextColorsCG6);
}

void CMIDDateFieldItem::SetInitialized(TInt aSetCurrentTime /* = ETrue */)
{
    iInitialised = ETrue;

    if (iInputMode == MMIDDateField::ETime)
    {
        reinterpret_cast<CEikTimeEditor*>(iEditor)->SetUninitialised(EFalse);
    }
    else if (iInputMode == MMIDDateField::EDate)
    {
        reinterpret_cast<CEikDateEditor*>(iEditor)->SetUninitialised(EFalse);
    }
    else if (iInputMode == MMIDDateField::EDateTime)
    {
        reinterpret_cast<CEikTimeAndDateEditor*>(iEditor)->SetUninitialised(EFalse);
    }

    if (aSetCurrentTime)
    {
        TTime time;
        time.HomeTime();
        iEditor->SetTTime(time);
    }

    iEditor->DrawDeferred();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDDateFieldItem::CMIDDateFieldItem(CMIDUIManager* aUIManager)
        :CMIDControlItem(EDefault, aUIManager),
        iInitialised(EFalse),
        iAmPmToggleCommand(0)
{
    iMMidItem = this;
}

void CMIDDateFieldItem::ConstructL(const TDesC& aLabel,MMIDDateField::TInputMode aInputMode)
{
    CMIDControlItem::ConstructL();
    SetLabelL(aLabel);
    SetFocusing(ETrue);

    UpdateMemberVariables();

    SetInputModeL(aInputMode);
    // Create built-in command to toggle am/pm value
    HBufC* label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_CHANGE);
    CleanupStack::PushL(label);
    iAmPmToggleCommand = CMIDCommand::NewBuiltInCommandL(label->Des(),
                         MMIDCommand::EItem,
                         CMIDCommand::EAmPmToggleCommandId);
    CleanupStack::PopAndDestroy(label); // command creates a copy of the label
    iAmPmToggleCommand->SetObserver(this);
}

void CMIDDateFieldItem::SetLabelL(const TDesC& aLabel)
{
    CMIDControlItem::SetLabelL(aLabel);
}

/** Recalculate the preferred size.*/
TSize CMIDDateFieldItem::ResetPreferredSize() const
{
    CMIDDateFieldItem* self = const_cast<CMIDDateFieldItem*>(this);
    TRAP_IGNORE(self->SetPreferredSizeL(iRequestedPreferredSize));
    return iPreferredSize;
}

void CMIDDateFieldItem::SetPreferredSizeL(const TSize& aSize)
{
    iRequestedPreferredSize = CheckRequestedSize(aSize);
    iPreferredSize = iRequestedPreferredSize;

    if (iPreferredSize.iWidth < 0)
    { // height is specified but width isn't
        iPreferredSize.iWidth = Max(iLabelControl->PreferredWidth(),
                                    iEditor->Size().iWidth + iMargins.iLeft + iMargins.iRight);
    }

    // make sure the width doesn't get bigger than the form width
    iPreferredSize.iWidth = Min(iPreferredSize.iWidth, FormClientAreaWidth());

    if (iPreferredSize.iHeight < 0)
    {  // width is specified but height isn't, do text wrapping (call SetWidthL)
        // so that we can then calculate the height
        if (HasLabel())
        {
            iLabelControl->SetWidthL(iPreferredSize.iWidth);
        }

        iPreferredSize.iHeight = LabelHeight() + ItemPreferredHeightWithoutLabel();
    }

    TSize minimumSize = MinimumSize();

    if ((minimumSize.iWidth == 0) && (minimumSize.iHeight == 0))
    { //it means there is no control and no label, so set our size to null
        iPreferredSize.iWidth = 0;
        iPreferredSize.iHeight = 0;
    }
    else
    { //make sure the preferred size is not smaller than the minimum size
        iPreferredSize.iWidth = Max(iPreferredSize.iWidth, minimumSize.iWidth);
        iPreferredSize.iHeight = Max(iPreferredSize.iHeight, minimumSize.iHeight);
    }

}

TBool CMIDDateFieldItem::IsSelectable() const
{
    return (!IsNonFocusing() || CommandList()->Count() > 0);
}

TSize CMIDDateFieldItem::MinimumSize()
{
    TBool hasLabel = HasLabel();
    TInt height = ItemPreferredHeightWithoutLabel();

    if (hasLabel)
    {
        height += OneLineLabelHeight();
    }

    TInt formWidth = FormClientAreaWidth();
    TInt width     = formWidth;

    if (Layout() & ELayout2)
    {
        TInt labelMinWidth = hasLabel ? iLabelControl->MinimumSize().iWidth : 0;

        width = Max(iEditor->MinimumSize().iWidth + iMargins.iLeft + iMargins.iRight,
                    labelMinWidth);
        width = Min(width, formWidth);
    }

    return TSize(width, height);
}

TInt CMIDDateFieldItem::CountComponentControls() const
{
    return 2;
}

CCoeControl* CMIDDateFieldItem::ComponentControl(TInt aIndex) const
{

    switch (aIndex)
    {
    case 0:
        return iLabelControl;

    case 1:
        return iEditor;
    }

    ASSERT(NULL);
    return NULL;
}

void CMIDDateFieldItem::Draw(const TRect& aRect) const
{
    if (!iForm)
    {
        return; // if we don't have a form we shouldn't try to draw
    }

    CMIDControlItem::Draw(aRect);
}

void CMIDDateFieldItem::SizeChanged()
{
    TRect rect = Rect();
    TPoint topLeft = Position();
    TInt labelHeight = LabelHeight();

    iLabelControl->SetExtent(topLeft, TSize(rect.Width(), labelHeight));

    rect.iTl.iY += labelHeight;
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(rect, AknLayoutScalable_Avkon::form2_midp_time_pane().LayoutLine());

    AknLayoutUtils::LayoutMfne(iEditor, layoutRect.Rect(),
                               AknLayoutScalable_Avkon::form2_midp_time_pane_t1().LayoutLine());

    CMIDControlItem::SizeChanged();
}

TKeyResponse CMIDDateFieldItem::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TInt oldCurrentField = iEditor->CurrentField();
    TInt code = aKeyEvent.iCode;
    TBool initialised = EFalse;
    //
    // Enter Key
    //
    if (aType == EEventKey)
    {
        usedKeyEvent=&aKeyEvent;
        prevInitialised=iInitialised;
    }
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iInitialised && ((aType == EEventKey && (aKeyEvent.iScanCode == EStdKeyEnter || aKeyEvent.iScanCode == EStdKeyDevice3)) ||
                         (aType == EEventKey && (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyDevice3) && (usedKeyEvent != &aKeyEvent) ||
                          (aType == EEventKeyUp && (aKeyEvent.iScanCode == EStdKeyEnter || aKeyEvent.iScanCode == EStdKeyDevice3) &&  prevInitialised == iInitialised && (usedKeyEvent != &aKeyEvent)))))
#else
    if (iInitialised && ((aType == EEventKey &&  aKeyEvent.iScanCode == EStdKeyEnter) ||
                         (aType == EEventKey && aKeyEvent.iCode == EKeyEnter && (usedKeyEvent != &aKeyEvent) ||
                          (aType == EEventKeyUp && aKeyEvent.iScanCode == EStdKeyEnter &&  prevInitialised == iInitialised && (usedKeyEvent != &aKeyEvent)))))
#endif // RD_JAVA_S60_RELEASE_9_2        
    {

        // Toggle AmPm
        if (IsCurrentFieldAmPmField())
        {
            ToggleAmPmFieldValue();
            return EKeyWasConsumed;
        }
        else
        {
            CMIDDisplayable& displayable = iForm->CurrentDisplayable();
            TInt cntOpt = displayable.NumCommandsForOkOptionsMenu();

            // set first command from command list,
            // if command list <= 0 then command sets NULL
            const CMIDCommand* command = (CommandList()->Count() > 0 ?
                                          CommandList()->At(0).iCommand :
                                          NULL);

            // Activate Default command
            // For default command will run ProcessCommandL( KItemCommandIdBase + 1 )
            if (DefaultCommand())
            {
                displayable.ProcessCommandL(CommandList()->CommandOffset() +
                                            CommandList()->FindCommandIndex(DefaultCommand()));
                return EKeyWasConsumed;
            }
            // Show Menu or activate one command
            else
            {
                // Active Command Show Menu
                // if ( cntOpt > 1 ) will run menu, else execute ProcessCommandL( CommandOffset )
                if (cntOpt > 1)
                {
                    displayable.MenuHandler()->ShowMenuL(CMIDMenuHandler::EOkMenu);
                    return EKeyWasConsumed;
                }
                else if (command && command->CommandType() != MMIDCommand::EBack &&
                         command->CommandType() != MMIDCommand::ECancel)
                {
                    displayable.ProcessCommandL(CommandList()->CommandOffset());
                    return EKeyWasConsumed;
                }
                else
                {
                    displayable.ProcessCommandL(displayable.MainCommandList()->CommandOffset());
                    return EKeyWasConsumed;
                }
            }
        }
    }

    //
    // End Enter Key
    //
    if (code == EKeyDownArrow || code == EKeyUpArrow || code == EKeyApplication0)
        return EKeyWasNotConsumed;

    if (!iInitialised && aType == EEventKey && !CMIDUtils::IgnoreKeyEvent(aKeyEvent.iCode))
    {
        if (code == EKeyBackspace)
        {
            return EKeyWasNotConsumed;
        }
        // Use any key to set an uninitialised DateField to an initialised one
        else if (code != EKeyLeftArrow && code != EKeyRightArrow && code != EKeyDevice3)
        {
            SetInitialized();
            initialised = ETrue;
        }
    }

    if ((aType == EEventKey) && (code == EKeyLeftArrow || code == EKeyRightArrow))
    {
        // is opened VKB
        CMIDDisplayable& displayable = iForm->CurrentDisplayable();
        if (!displayable.IsVKBOnScreen())
        {
            // Check if focus moving is possible inside the editor, if not, do nothing
            if ((code == EKeyLeftArrow) && (iEditor->CurrentField() == 0))
            {
                return EKeyWasNotConsumed;
            }
            else if ((code == EKeyRightArrow) && (iEditor->CurrentField() == (iEditor->NumFields()-1)))
            {
                return EKeyWasNotConsumed;
            }
        }
        // Redraw is needed here for properly text drawing.
        iEditor->DrawDeferred();
    }

    TUint scanCode = aKeyEvent.iScanCode;
    TBool losingFocus = EFalse;
    //
    // keyboard menu
    //
    switch (scanCode)
    {
    case EStdKeyF1:
    case EStdKeyMenu:
        losingFocus = ETrue;
    }
    //
    // Non keyboard zoom,menu,etc.
    //
    if (scanCode>=ESpecialKeyBase && scanCode<(ESpecialKeyBase+ESpecialKeyCount))
        losingFocus=ETrue;
    //
    // CBA buttons
    //
    if (scanCode>=EStdKeyDevice0 && scanCode<=EStdKeyDeviceF)
        losingFocus=ETrue;
    //
    if (iInitialised && losingFocus)
    {
        TRAP_IGNORE(iEditor->PrepareForFocusLossL());
    }

    CEikMfneField* currentField = iEditor->Field(iEditor->CurrentField());
    TBool notEmpty = currentField->IsValid();

    TKeyResponse ret =  iEditor->OfferKeyEventL(aKeyEvent,aType);

    if (oldCurrentField != iEditor->CurrentField())
    { // focus was moved -> update commands (toggle command may be possible)
        UpdateCommands();
    }

    if (aType == EEventKey && iEditor->Field(iEditor->CurrentField())->IsValid())
    {
        // Check if change event should be fired. The contents of the field is changed if:
        // - a number key is pressed (always changes the contents of the field) and zero is not the first number
        // - a backspace key is pressed when the the field was not empty
        // - the AM/PM field is changed
        // - the field was empty but the native component set value to it (arrow key was pressed)
        // - it was initialised
        TBool amPmField = IsCurrentFieldAmPmField();
        TChar ch = TChar(code);
        if (ch.IsDigit() ||
                (code == EKeyBackspace && notEmpty) ||
                (amPmField && code != EKeyLeftArrow && code != EKeyRightArrow) ||
                (!notEmpty && currentField->IsValid()) || initialised)
        {
            ReportEventL(MCoeControlObserver::EEventStateChanged);
        }
    }
    return ret;
}

#ifdef RD_SCALABLE_UI_V2
void CMIDDateFieldItem::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled() && iForm)
    {
        TInt nOldCurrent = iEditor->CurrentField();
        TBool editorArea = iEditor->Rect().Contains(aPointerEvent.iPosition);
        TBool longTapArea = Rect().Contains(aPointerEvent.iPosition);
        TBool longTapDetected = EFalse;
        // On EButton1Down event check that event is on the allowed long tap area
        if (aPointerEvent.iType != TPointerEvent::EButton1Down || longTapArea)
        {
            longTapDetected = iForm->TryDetectLongTapL(aPointerEvent);
        }

        TPointerEvent pEvent = aPointerEvent;
        //if label area of area right from editor area is tapped the coordinates are transferred
        //to editor area. VKB is launched if tapped anywhere else than AM/PM field.
        if (aPointerEvent.iPosition.iY <= iLabelControl->Rect().iBr.iY)
        {
            pEvent.iPosition.iY = iEditor->Rect().iTl.iY;
        }
        if (aPointerEvent.iPosition.iX > iEditor->Rect().iBr.iX)
        {
            pEvent.iPosition.iX = iEditor->Rect().iBr.iX - 1;
        }

        // Keep track of the pointer grabbing control
        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            iGrabbingControl = NULL;

            const TInt count = CountComponentControls();
            for (TInt i = (count - 1); i >= 0; --i)
            {
                CCoeControl* ctrl = ComponentControl(i);
                if (ctrl->Rect().Contains(pEvent.iPosition))
                {
                    iGrabbingControl = ctrl;
                }
            }
        }
#ifdef RD_JAVA_S60_RELEASE_9_2
        // In single click UI VKB is opened with first tap.
        if (!longTapDetected && !iForm->PhysicsScrolling())
        {
            if (aPointerEvent.iType == TPointerEvent::EButton1Up
                    && iGrabbingControl == iEditor
                    && IsCurrentFieldAmPmField()
                    && editorArea)
            {
                // Toggle AM/PM value
                iEditor->HandleMfneCommandL(
                    MAknMfneCommandObserver::EMfneIncrementCurrentFieldValue);
                ReportEventL(MCoeControlObserver::EEventStateChanged);
            }
            else if (iGrabbingControl)
            {
                iGrabbingControl->HandlePointerEventL(aPointerEvent);

                // Send PointerDown and PointerUp events again to the editor.
                // This enables VKB to open with first tap even if internal
                // focus is in another field.
                if (aPointerEvent.iType == TPointerEvent::EButton1Up)
                {
                    pEvent.iType = TPointerEvent::EButton1Down;
                    iGrabbingControl->HandlePointerEventL(pEvent);
                    pEvent.iType = TPointerEvent::EButton1Up;
                    iGrabbingControl->HandlePointerEventL(pEvent);
                }
            }
        }
        // else do not forward pointer event to CCoeControl as
        // there is a long tap or physics scrolling going on
#else
        // In non-single click UI,
        // VKB is opened when tapping already focused item.
        if (!longTapDetected && !iForm->IsFocusChangingWithPen() && !iForm->PhysicsScrolling())
        {
            if (aPointerEvent.iType == TPointerEvent::EButton1Up
                    && iGrabbingControl == iEditor
                    && IsCurrentFieldAmPmField()
                    && editorArea)
            {
                // Toggle AM/PM value
                iEditor->HandleMfneCommandL(
                    MAknMfneCommandObserver::EMfneIncrementCurrentFieldValue);
                ReportEventL(MCoeControlObserver::EEventStateChanged);
            }
            else if (IsFocused() && iGrabbingControl)
            {
                iGrabbingControl->HandlePointerEventL(aPointerEvent);

                // Send PointerDown and PointerUp events again to the editor.
                // This enables VKB to open with first tap even if internal
                // focus is in another field.
                if (aPointerEvent.iType == TPointerEvent::EButton1Up)
                {
                    pEvent.iType = TPointerEvent::EButton1Down;
                    iGrabbingControl->HandlePointerEventL(pEvent);
                    pEvent.iType = TPointerEvent::EButton1Up;
                    iGrabbingControl->HandlePointerEventL(pEvent);
                }
            }
        }
        // else do not forward pointer event to CCoeControl as
        // there is a long tap or physics scrolling going on

#endif // RD_JAVA_S60_RELEASE_9_2
        // msk: notify if the current field has changed
        if (nOldCurrent != iEditor->CurrentField())
        {
            UpdateCommands();
        }

        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            iGrabbingControl = NULL;
        }
    }
}
#endif // RD_SCALABLE_UI_V2

// msk
TBool CMIDDateFieldItem::ProcessCommandL(CMIDCommand* aCommand)
{
    if (aCommand->Id() == iAmPmToggleCommand->Id() && IsCurrentFieldAmPmField())
    {
        ToggleAmPmFieldValue();
    }
    return ETrue;
}

// msk
/**
 * Updates the items commands according to the currently focused field. Needs to be
 * called when ever focused field changes.
 */
void CMIDDateFieldItem::UpdateCommands()
{
    // If the current field is the AM/PM field, provide toggle command
    SetBuiltInMSKCommand(IsCurrentFieldAmPmField() ? iAmPmToggleCommand : NULL);
}

// msk
TBool CMIDDateFieldItem::IsCurrentFieldAmPmField()
{
    return IsFieldAmPmField(iEditor->CurrentField());
}

TBool CMIDDateFieldItem::IsFieldAmPmField(TInt fieldIndex)
{
    // current field is am/pm field if it accepts something else than positive integers
    TBool amPmField =
        (iInputMode == MMIDDateField::ETime || iInputMode == MMIDDateField::EDateTime)  &&
        (iEditor->Field(fieldIndex)->InputCapabilities().Capabilities() &
         ~TCoeInputCapabilities::EWesternNumericIntegerPositive);
    return amPmField;
}

void CMIDDateFieldItem::ToggleAmPmFieldValue()
{
    TInt numFields = iEditor->NumFields();
    for (TInt i = 0; i < numFields; i++)
    { // find am/pm field
        if (IsFieldAmPmField(i))
        { // toggle the field value
            CEikMfneSymbol* ampmField = static_cast<CEikMfneSymbol*>(iEditor->Field(i));
            TAmPm ampm = static_cast<TAmPm>(ampmField->IdOfCurrentSymbolicItem());
            if (ampm == EAm)
            {
                ampm = EPm;
            }
            else
            {
                ampm = EAm;
            }
            ampmField->SetCurrentSymbolicItemToId(ampm);
            iEditor->DrawDeferred();
            break; // assumes that there is only one am/pm field
        }
    }
}

void CMIDDateFieldItem::SetContainerWindowL(const CCoeControl& aContainer)
{
    CMIDControlItem::SetContainerWindowL(aContainer);
    iEditor->SetContainerWindowL(*this);
    SetObserver(iForm);
    ActivateL();
}

void CMIDDateFieldItem::DoSafeDraw()
{
    if (DrawableWindow() && iForm)
    {
        DrawDeferred();
    }
}

void CMIDDateFieldItem::FocusChanged(TDrawNow aDrawNow)
{
    TBool focus = IsFocused();

    iEditor->SetFocus(focus);
    if (focus)
    {
        TRAP_IGNORE(iUIManager->OpenNaviPaneControllerL()->PauseTickerL(
                        TICKER_PAUSE_INTERVAL, this));
    }
    else
    {
        TRAP_IGNORE(iUIManager->OpenNaviPaneControllerL()->PauseTickerL(
                        0, this));
#ifdef RD_SCALABLE_UI_V2
        iGrabbingControl = NULL;
#endif
    }

    TRAP_IGNORE(UpdateTextColorsL());

    CMIDControlItem::FocusChanged(aDrawNow);

    DoSafeDraw();
}

TCoeInputCapabilities CMIDDateFieldItem::InputCapabilities() const
{
    TCoeInputCapabilities inputCapabilities(TCoeInputCapabilities::ENone, NULL,
                                            const_cast<CMIDDateFieldItem*>(this));
    inputCapabilities.MergeWith(CMIDControlItem::InputCapabilities());
    return inputCapabilities;
}

TInt CMIDDateFieldItem::ItemPreferredHeightWithoutLabel()
{
    return iMargins.iTop + iEditorHeight + iMargins.iBottom;
}

/* ResolutionChange
 *
 * This method is called after dynamic resolution change
 */
void CMIDDateFieldItem::ResolutionChange(TInt /*aType*/)
{
    UpdateMemberVariables();
    // Set font from LAF to the editor, so that MinimumSize() will return correct values.
    // AknLayoutUtils::LayoutMfne() will also set the font from LAF to the editor
    iEditor->SetFont(AknLayoutUtils::FontFromId(
                         AknLayoutScalable_Avkon::form2_midp_time_pane_t1().Font()));
}

void CMIDDateFieldItem::AdjustToSizeL(const TSize& aSize)
{
    UpdateMemberVariables();
    if (HasLabel())
    {
        iLabelControl->AdjustToSizeL(
            TSize(aSize.iWidth, aSize.iHeight - ItemPreferredHeightWithoutLabel()));
    }
}

void CMIDDateFieldItem::Dispose()
{
    delete this;
}

void CMIDDateFieldItem::AddCommandL(MMIDCommand* aCommand)
{
    CMIDItem::AddCommandL(aCommand);
}

void CMIDDateFieldItem::RemoveCommand(MMIDCommand* aCommand)
{
    CMIDItem::RemoveCommand(aCommand);
}

void CMIDDateFieldItem::SetDefaultCommand(MMIDCommand* aCommand)
{
    CMIDItem::SetDefaultCommand(aCommand);
}

TSize CMIDDateFieldItem::PreferredSize() const
{
    CMIDDateFieldItem* self = const_cast<CMIDDateFieldItem*>(this);
    CMIDItem* item = static_cast<CMIDItem*>(self);
    return item->PreferredSize();
}

TSize CMIDDateFieldItem::MinimumSize() const
{
    CCoeControl* control = const_cast<CMIDDateFieldItem*>(this);
    return control->MinimumSize();
}

void CMIDDateFieldItem::SetLayoutL(TLayout aLayout)
{
    CMIDItem::SetLayoutL(aLayout);
}

TBool CMIDDateFieldItem::IsDateTimeNonEmpty() const
{
    if (iEditor)
    {
        TInt count = iEditor->NumFields();
        for (TInt i = 0; i < count; ++i)
        {
            if (!iEditor->Field(i)->IsValid())
            {
                return EFalse;
            }
        }

        return ETrue;
    }
    else
    {
        return EFalse;
    }
}

void CMIDDateFieldItem::UpdateMemberVariables()
{
    TAknWindowLineLayout layout = AknLayoutScalable_Avkon::form2_midp_time_pane().LayoutLine();
    iMargins.iTop    = layout.it;
    iMargins.iBottom = ItemContentBottomMargin();
    iMargins.iLeft   = layout.il;
    iMargins.iRight  = layout.ir;
    iEditorHeight    = layout.iH;
}

// Update right cursor position on Datefiled
void CMIDDateFieldItem::CursorUpdate()
{
    if (IsFocused())
    {
        iEditor->SetFocus(ETrue);
    }
}

void CMIDDateFieldItem::UpdateTextColorsL()
{
    // Set color for content text according to item highlight
    // (logical color constants are defined in lcdui.h)
    if (iHighlighted)
    {
        // Text colour from skin - highlighted
        iEditor->SetSkinTextColorL(KHighlightedItemTextColor);
    }
    else
    {
        // Text colour from skin - unfocused
        iEditor->SetSkinTextColorL(KNonHighlightedItemTextColor);
    }
}

// End of File
