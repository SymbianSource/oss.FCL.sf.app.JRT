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


#include <inetprottextutils.h>
#include <AknQueryDialog.h>
#include <eswtcore.rsg>
#include "swtquerydialog.h"


// Constants for Numeric dialog
static const TInt KFloatStringMaxLength = 16; // Length for entry.

// Constants for Time dialog
// (ISO 8601 format required)
_LIT(KTimeFormat,"%H:%T:%S");
static const TInt KTimeFormatLength = 8;

// Constants for Date dialog
// (ISO 8601 format required)
_LIT(KDateFormat,"%F%Y-%M-%D");
static const TInt KDateFormatLength = 10;

// Default values for date and time conversions
_LIT(KTimeMidnight,":000000.");
_LIT(KJavaTimeMidnight,"24:00:00");

const TInt KDefaultMaxChars = 255;


// ======== MEMBER FUNCTIONS ========


HBufC* CSwtQueryDialog::RunDlgL(const TDesC& aTitle, TInt /*aStyle*/,
                                TInt aQueryType, const TDesC& aPromptText, const TDesC& aDefaultValue,
                                TInt aMinimum, TInt aMaximum)
{
    HBufC* result = NULL;
    switch (aQueryType)
    {
    case ESwtQueryDialogStyleStandard:
    {
        CSwtQueryDialogText::RunDlgL(aTitle, aPromptText, aDefaultValue,
                                     aMinimum, aMaximum, result, CSwtQueryDialogText::EStandardStyle);
        break;
    }
    case ESwtQueryDialogStyleNumeric:
    {
        CSwtQueryDialogNumeric::RunDlgL(aTitle, aPromptText, aDefaultValue,
                                        aMinimum, aMaximum, result);
    }
    break;
    case ESwtQueryDialogStylePassword:
    {
        CSwtQueryDialogText::RunDlgL(aTitle, aPromptText, aDefaultValue,
                                     aMinimum, aMaximum, result, CSwtQueryDialogText::EPasswordStyle);
    }
    break;

    case ESwtQueryDialogStyleTime:
    {
        CSwtQueryDialogDateTime::RunDlgL(aTitle, aPromptText, aDefaultValue,
                                         aMinimum, aMaximum, result, CSwtQueryDialogDateTime::ETimeStyle);
    }
    break;
    case ESwtQueryDialogStyleDate:
    {
        CSwtQueryDialogDateTime::RunDlgL(aTitle, aPromptText, aDefaultValue,
                                         aMinimum, aMaximum, result, CSwtQueryDialogDateTime::EDateStyle);
    }
    break;
    default:
        ASSERT(EFalse); // Already filtered Java side, must never occur
    }

    return result;
}

/**
* Convert from java to TTime format
* Java time Format is
* HH:MM:SS
* Required TTime format is:
* YYYYMMDD:HHMMSS.MMMMMM
*/
void CSwtQueryDialog::ConvertJavaTime(TDes& aTime, const TDesC& aJavaTime)
{
    ASSERT(aJavaTime.Length() == KTimeFormatLength);

    if (aJavaTime.Compare(KJavaTimeMidnight) == 0)
    {
        aTime.Zero();
        aTime.Append(KTimeMidnight);
    }
    else
    {
        aTime.Zero();
        aTime.Append(':');
        aTime.Append(aJavaTime.Mid(0,2));
        aTime.Append(aJavaTime.Mid(3,2));
        aTime.Append(aJavaTime.Mid(6,2));
        aTime.Append('.');
    }
}

/**
* Convert from java to TTime format
* Java date Format is
* YYYY-MM-DD
* Required TTime format is:
* YYYYMMDD:HHMMSS.MMMMMM
*/
void CSwtQueryDialog::ConvertJavaDateL(TDes& aDate, const TDesC& aJavaDate)
{
    ASSERT(aJavaDate.Length() == KDateFormatLength);

    aDate.Zero();
    aDate.Append(aJavaDate.Mid(0,4));

    TPtrC monthPtr(aJavaDate.Mid(5,2));
    TInt monthInt=0;
    InetProtTextUtils::ConvertDescriptorToInt(monthPtr, monthInt);
    monthInt--;
    if (monthInt<10)
        aDate.Append('0');
    HBufC* monthBuf;
    InetProtTextUtils::ConvertIntToDescriptorL(monthInt, monthBuf);
    aDate.Append(monthBuf->Des());
    delete monthBuf;

    TPtrC dayPtr(aJavaDate.Mid(8,2));
    TInt dayInt=0;
    InetProtTextUtils::ConvertDescriptorToInt(dayPtr, dayInt);
    dayInt--;
    if (dayInt<10)
        aDate.Append('0');
    HBufC* dayBuf;
    InetProtTextUtils::ConvertIntToDescriptorL(dayInt, dayBuf);
    aDate.Append(*dayBuf);
    delete dayBuf;

    aDate.Append(':');
    aDate.Append('.');
}


HBufC* CSwtQueryDialog::ConvertDefaultValueL(const TDesC& aString, TInt aQueryType)
{
    switch (aQueryType)
    {
    case ESwtQueryDialogStyleTime:
    {
        HBufC* timeBuf = HBufC::NewL(KTimeFormatLength);
        TPtr timePtr(timeBuf->Des());
        ConvertJavaTime(timePtr, aString);
        TTime time;
        TInt result = time.Set(timePtr);
        if (result == KErrNone)
        {
            return timeBuf;
        }
        delete timeBuf;
        User::Leave(ESwtErrorInvalidArgument);
    }
    break;
    case ESwtQueryDialogStyleDate:
    {
        HBufC* dateBuf = HBufC::NewL(KDateFormatLength);
        CleanupStack::PushL(dateBuf);
        TPtr datePtr(dateBuf->Des());
        ConvertJavaDateL(datePtr, aString);
        TTime time;
        TInt result = time.Set(datePtr);
        if (result == KErrNone)
        {
            CleanupStack::Pop(dateBuf);
            return dateBuf;
        }
        CleanupStack::PopAndDestroy(dateBuf);
        User::Leave(ESwtErrorInvalidArgument);
    }
    break;
    default:
        ASSERT(EFalse);
    }
    return NULL;
}


// Class CSwtQueryDialogText

CSwtQueryDialogText::CSwtQueryDialogText(TInt aMinimum, TDes& aResult)
        : CAknTextQueryDialog(aResult, CAknQueryDialog::ENoTone)
        , iMinimum(aMinimum)
{
}

TBool CSwtQueryDialogText::RunDlgL(const TDesC& aTitle, const TDesC& aPromptText,
                                   const TDesC& aDefaultValue, TInt aMinimum, TInt aMaximum, HBufC*& aResult,
                                   TTextDialogStyle aStyle)
{
    ASSERT(aResult==NULL);

    // If Minimum==Maximum, no text can be entered.
    // Moreover, the QueryDialog does not handle this situation properly
    // So we display nothing and just return NULL
    TInt minimum(0);
    TInt maximum(0);
    // If minimum is not smaller than maximum, then default minimum and maximum are set.
    if (aMinimum < aMaximum)
    {
        minimum = aMinimum;
        maximum = aMaximum;

        // Check that min and max are not higher than EMaxSecEdSecArrayLength to prevent a Panic
        // in CEikSecretEditor used by PASSWORD QueryDialog.
        if (aStyle == EPasswordStyle && maximum > CEikSecretEditor::EMaxSecEdSecArrayLength)
        {
            maximum = CEikSecretEditor::EMaxSecEdSecArrayLength;
            if (minimum >= CEikSecretEditor::EMaxSecEdSecArrayLength)
            {
                minimum = CEikSecretEditor::EMaxSecEdSecArrayLength-1;
            }
        }
    }
    else
    {
        minimum = 0;
        if (aStyle == EPasswordStyle)
        {
            maximum = CEikSecretEditor::EMaxSecEdSecArrayLength;
        }
        else
        {
            maximum = KDefaultMaxChars;
        }
    }

    // text represents the user's entry
    // set to the maximum.
    RBuf text;
    text.CleanupClosePushL();
    text.CreateMaxL(maximum);
    text.SetLength(0);

    // Create a copy of the prompt text
    RBuf prompt;
    prompt.CleanupClosePushL();
    User::LeaveIfError(prompt.Create(aPromptText));

    // Create the Dialog
    CSwtQueryDialogText* queryDialog = new(ELeave) CSwtQueryDialogText(minimum, text);

    if (aStyle==EPasswordStyle)
    {
        queryDialog->PrepareLC(R_QUERYDIALOG_TEXT_PASSWORD);
        queryDialog->SetDefaultInputMode(EAknEditorSecretAlphaInputMode);
    }
    else // EStandardStyle
    {
        queryDialog->PrepareLC(R_QUERYDIALOG_TEXT_STANDARD);
    }

    queryDialog->SetMaxLength(maximum);
    text.Append(aDefaultValue.Left(Min(maximum,aDefaultValue.Length())));

    // Set the prompt text
    queryDialog->SetPromptL(prompt);

    // Set the title.
    CAknPopupHeadingPane* controlTitle = static_cast<CAknPopupHeadingPane*>
                                         (queryDialog->ControlOrNull(EAknMessageQueryHeaderId));

    if (controlTitle == NULL)
    {
        queryDialog->SetTitleL(aTitle);
    }
    else
    {
        controlTitle->SetTextL(aTitle);
    }

    // Run the dialog
    TBool result=queryDialog->RunLD();

    // If user press ok, set the aResult string
    // (for cancel, aResult=NULL)
    if (result)
    {
        aResult = HBufC::NewL(text.Length());
        TPtr resultPtr(aResult->Des());
        resultPtr.Append(text);
    }

    CleanupStack::PopAndDestroy(2);     // prompt and text

    return result;
}


void  CSwtQueryDialogText::UpdateLeftSoftKeyL()
{
    // The method has been overrided to handle the minimum value
    CAknQueryControl* control = QueryControl();
    if (control)
    {
        if (control->GetTextLength()<iMinimum)
        {
            MakeLeftSoftkeyVisible(EFalse);
        }
        else
        {
            CAknQueryDialog::UpdateLeftSoftKeyL();
        }
    }
}

// This method needs to be redefined because of a crash if "delete" or "backspace"
// is pressed when the string is empty.
TBool CSwtQueryDialogText::NeedToDismissQueryL(const TKeyEvent& aKeyEvent)
{
    CAknQueryControl* control = QueryControl();
    if (control && control->GetTextLength() == 0
            && (aKeyEvent.iCode == EKeyDelete || aKeyEvent.iCode == EKeyBackspace))
    {
        return ETrue;
    }

    return CAknTextQueryDialog::NeedToDismissQueryL(aKeyEvent);
}


// Class CSwtQueryDialogNumeric

CSwtQueryDialogNumeric::CSwtQueryDialogNumeric(TReal& aResult,
        const TDesC& aDefaultValue, TInt aMinimum, TInt aMaximum)
        : CAknFloatingPointQueryDialog(aResult, CAknQueryDialog::ENoTone)
        , iDefaultValue(aDefaultValue), iTextMinLength(aMinimum), iTextMaxLength(aMaximum)
{
}

TBool CSwtQueryDialogNumeric::RunDlgL(const TDesC& aTitle, const TDesC& aPromptText,
                                      const TDesC& aDefaultValue, TInt aMinimum, TInt aMaximum, HBufC*& aResult)
{
    ASSERT(aResult==NULL);

    // Copy prompt text
    RBuf prompt;
    prompt.CleanupClosePushL();
    User::LeaveIfError(prompt.Create(aPromptText));

    // Create the Dialog
    TReal realvalue=0;
    CSwtQueryDialogNumeric* queryDialog = new(ELeave) CSwtQueryDialogNumeric(
        realvalue, aDefaultValue, aMinimum, aMaximum);
    queryDialog->PrepareLC(R_QUERYDIALOG_NUMBER);

    // Set the prompt text
    queryDialog->SetPromptL(prompt);

    // Set the title.
    CAknPopupHeadingPane* controlTitle = static_cast<CAknPopupHeadingPane*>
                                         (queryDialog->ControlOrNull(EAknMessageQueryHeaderId));

    if (controlTitle == NULL)
    {
        queryDialog->SetTitleL(aTitle);
    }
    else
    {
        controlTitle->SetTextL(aTitle);
    }

    // Default value is set in the PostLayoutDynInitL

    // Run the dialog
    TBool result=queryDialog->RunLD();

    // If user press ok, set the aResult string
    // (for cancel, aResult=NULL)
    if (result)
    {
        aResult = HBufC::NewLC(KFloatStringMaxLength);
        TPtr resultPtr(aResult->Des());
        // There's no specification about the format to return for a number
        // We return in the the string with no triads and a dot character for
        // decimal separator.
        TRealFormat realformat;
        realformat.iWidth=KFloatStringMaxLength;    // so no overflow can happen
        realformat.iTriad=KDoNotUseTriads;          // no triads
        realformat.iPoint='.';
        resultPtr.AppendNum(realvalue,realformat);
        CleanupStack::Pop();     // aResult
    }

    CleanupStack::PopAndDestroy();

    return result;
}

void CSwtQueryDialogNumeric::PostLayoutDynInitL()
{
    CAknQueryControl* querycontrol=QueryControl();
    ASSERT(querycontrol!=NULL);

    TInt textEntryLength = querycontrol->GetTextEntryLength();

    if (iTextMaxLength == 0 || iTextMaxLength > textEntryLength)
    {
        iTextMaxLength = textEntryLength;
    }

    // Check min and max length values
    if (iTextMinLength >= iTextMaxLength)
    {
        iTextMinLength = 0;
        iTextMaxLength = textEntryLength;
    }

    querycontrol->SetTextEntryLength(iTextMaxLength);

    // Set the default value
    HBufC* defaultValueString = HBufC::NewLC(iDefaultValue.Length());
    TPtr defaultValuePtr(defaultValueString->Des());
    defaultValuePtr.AppendFormat(iDefaultValue);
    querycontrol->SetTextL(defaultValuePtr.Left(Min(iTextMaxLength, defaultValuePtr.Length())));
    CleanupStack::PopAndDestroy();   // defaultValueString

    UpdateLeftSoftKeyL();
}

void  CSwtQueryDialogNumeric::UpdateLeftSoftKeyL()
{
    // The method has been overrided to handle the minimum value
    CAknQueryControl* control = QueryControl();
    if (control)
    {
        if (control->GetTextLength() < iTextMinLength)
        {
            MakeLeftSoftkeyVisible(EFalse);
        }
        else
        {
            CAknQueryDialog::UpdateLeftSoftKeyL();
        }
    }
}


// This method needs to be redefined because of a crash if "delete" or "backspace"
// is pressed when the string is empty.
TBool CSwtQueryDialogNumeric::NeedToDismissQueryL(const TKeyEvent& aKeyEvent)
{
    CAknQueryControl* control = QueryControl();
    if (control && control->GetTextLength() == 0
            && (aKeyEvent.iCode == EKeyDelete || aKeyEvent.iCode == EKeyBackspace))
    {
        return ETrue;
    }

    return CAknFloatingPointQueryDialog::NeedToDismissQueryL(aKeyEvent);
}



// Class CSwtQueryDialogTime

CSwtQueryDialogDateTime::CSwtQueryDialogDateTime(TTime& aTime)
        : CAknTimeQueryDialog(aTime, CAknQueryDialog::ENoTone)
{
}

TBool CSwtQueryDialogDateTime::RunDlgL(const TDesC& aTitle, const TDesC& aPromptText,
                                       const TDesC& aDefaultValue, TInt /*aMinimum*/, TInt /*aMaximum*/, HBufC*& aResult,
                                       TDateTimeDialogStyle aDialogStyle)
{
    ASSERT(aResult==NULL);

    // set timevalue to default.
    // aDefaultValue must have been converted to the right format before.
    // If the format is wrong, this will panic
    // Format is:
    // YYYYMMDD:HHMMSS.MMMMMM
    TTime timevalue;
    // If no default value is set, then th current home time is set as default value.
    if (aDefaultValue.Length() == 0)
    {
        timevalue.HomeTime();
    }
    else
    {
        timevalue.Set(aDefaultValue);
    }

    // Copy prompt text
    RBuf prompt;
    prompt.CleanupClosePushL();
    User::LeaveIfError(prompt.Create(aPromptText));

    // Create the Dialog
    CSwtQueryDialogDateTime* queryDialog = new(ELeave) CSwtQueryDialogDateTime(timevalue);

    if (aDialogStyle==ETimeStyle)
    {
        queryDialog->PrepareLC(R_QUERYDIALOG_TIME);
    }
    else if (aDialogStyle==EDateStyle)
    {
        queryDialog->PrepareLC(R_QUERYDIALOG_DATE);
    }


    // Set the prompt text
    queryDialog->SetPromptL(prompt);

    // Set the title.
    CAknPopupHeadingPane* controlTitle = static_cast<CAknPopupHeadingPane*>
                                         (queryDialog->ControlOrNull(EAknMessageQueryHeaderId));

    if (controlTitle == NULL)
    {
        queryDialog->SetTitleL(aTitle);
    }
    else
    {
        controlTitle->SetTextL(aTitle);
    }

    // Run the dialog
    TBool result=queryDialog->RunLD();

    // If user press ok, set the aResult string
    // (for cancel, aResult=NULL)
    if (result)
    {
        if (aDialogStyle==ETimeStyle)
        {
            aResult = HBufC::NewLC(KTimeFormatLength);
            TPtr resultPtr(aResult->Des());
            timevalue.FormatL(resultPtr,KTimeFormat);
            CleanupStack::Pop();     // aResult
        }
        else if (aDialogStyle==EDateStyle)
        {
            aResult = HBufC::NewLC(KDateFormatLength);
            TPtr resultPtr(aResult->Des());
            timevalue.FormatL(resultPtr,KDateFormat);
            CleanupStack::Pop();     // aResult
        }
    }

    CleanupStack::PopAndDestroy();

    return result;
}
