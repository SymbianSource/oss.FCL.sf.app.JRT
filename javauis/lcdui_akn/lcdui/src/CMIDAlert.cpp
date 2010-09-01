/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Alert implementation for Series 60
*
*/



#include "CMIDAlert.h"
#include "CMIDAlertDialog.h"
#include "CMIDDisplayable.h"
#include "CMIDGaugeItem.h"
#include "CMIDUtils.h"

// used for layouting
// LAF - AknLayoutScalable_Avkon::popup_midp_note_alarm_window_g1
#include <aknlayoutscalable_avkon.cdl.h>

// using MMIDImage and MMIDBitmapImage
#include <lcdgr.h>
// macros definitions for resources
#include <lcdui.rsg>

// CEikCaptionedControl API used when setting image or animation
// in SetImageOrAnimToDialogL function
#include <eikcapc.h>
// using CEikImage when setting image or animation
// in SetImageOrAnimToDialogL function
#include <eikimage.h>
// macros for resources related to alert
#include <avkon.rsg>
#include <avkon.hrh>

// macros definitions for outputing logs
#include <j2me/jdebug.h>

// sound files
#include <aknappui.h>
#include <aknsoundsystem.h>

const TInt KMillToMicroSeconds = 1000;
const TInt KAlertDefaultTimeout = 3000;
const TInt KConfirmationNoteTimeout = 1500;

CMIDAlert* CMIDAlert::NewL(
    MMIDEnv& aEnv,
    TAlertType aType,
    MMIDDisplayable& aDisplayable,
    const TDesC& aString,
    MMIDImage* aImage,
    MMIDUtils* aUtils)
{
    CMIDAlert* alert = new(ELeave) CMIDAlert(aEnv, aType, aUtils);
    CleanupStack::PushL(alert);
    alert->ConstructL(aDisplayable,aString,aImage);
    CleanupStack::Pop(alert);
    return alert;
}

CMIDAlert::CMIDAlert(MMIDEnv& aEnv, TAlertType aType, MMIDUtils* aUtils)
        : iEnv(&aEnv), iAlertType(aType), iUtils(aUtils)
{
}

CMIDAlert::~CMIDAlert()
{
    if (iBitmapImage)
    {
        iBitmapImage->RemoveRef();
        iBitmapImage = NULL;
    }

    delete iUserText; //lint !1551
    delete iDefaultText;

    delete iDialog;

    delete iDialogTimer;

    // Displayable is notified about content control deletion
    if (iDisplayable)
    {
        iDisplayable->NotifyContentDestroyed();
    }

}


void CMIDAlert::ConstructL(MMIDDisplayable& aDisplayable,const TDesC& aString,MMIDImage* aImage)
{
    CMIDDisplayable* dsp = (CMIDDisplayable*)(&aDisplayable);
    aDisplayable.SetComponentL(*this);
    iDisplayable = dsp;

    //Hide menu if visible
    CMIDMenuHandler* menuHandler = iDisplayable->MenuHandler();

    if (menuHandler)
    {
        menuHandler->HideMenuIfVisible();
    }


    SetStringL(aString);
    SetImageL(aImage);

    CreateDialogL();

    SetContainerWindowL(*dsp);

    iAlertTone = ENoTone;
    ASSERT(iUtils);
}

/** Play a sound. Called when the dialog is shown. */
TInt CMIDAlert::PlaySound()
{
    return iUtils->PlaySound(iAlertType);
}

/** Sets the dialog text to a user given text
or to the default text. If the text is so long
that it requires scrolling then the dialog must
become modal, hence we call SetModalL() if that's
the case. This method is also called by ConstructL()
before creating the dialog so we must check there
is a dialog before calling SetTextL() on the dialog.*/
void CMIDAlert::SetStringL(const TDesC& aString)
{
    DEBUG("CMIDAlert::SetStringL <");

    delete iUserText;
    iUserText = NULL;

    if (aString.Length() > 0)
    {
        iUserText = aString.AllocL();
    }
    if (iDialog)
    {
        iDialog->SetTextL(iUserText ? iUserText : iDefaultText);
        iDialog->DrawDeferred();
        iDialog->SizeChanged();
    }

    if (IsModal())
    {
        SetModalL();
    }


    DEBUG("CMIDAlert::SetStringL >");
}

/** Sets the dialog title or removes it. This is
done by inserting or removing the heading and separator
controls as well as calling CAknPopupForm::SetTitleL().
See also DisableVisibilityChange(), which we must call
to avoid the dialog disappearing and re-appearing each
time we insert or remove a control. */
void CMIDAlert::SetTitleL(const TDesC* aString)
{
    TBitFlags flags = iDialog->Flags();
    iDialog->DisableVisibilityChange(ETrue);

    if (aString && aString->Length() > 0)
    { // Title is updated to dialog

        if (!flags.IsSet(EPopupFormHasHeading))
        {// Add headerline and extra empty line
            iDialog->InsertControlL(0, R_MIDP_ALERT_HEADING_LINE, EFalse);
            iDialog->InsertControlL(0, R_MIDP_ALERT_SPACER_LINE, EFalse);
        }

        iDialog->SetTitleL(*aString);
    }
    else // Title is removed from dialog
    {// Remove headerline and extra empty line

        if (flags.IsSet(EPopupFormHasHeading))
        {
            iDialog->DeleteControl(EEikCtLabel);
            iDialog->DeleteControl(EEikCtSpacer);
        }
    }

    iDialog->DisableVisibilityChange(EFalse);
    iDialog->DrawDeferred();
}

/** Either set or remove an image.  This
method is also called in ConstructL() before creating the dialog
and hence we must make sure we do not call SetImageToDialogL
if there is not dialog. @see SetImageToDialogL(). */
void CMIDAlert::SetImageL(MMIDImage* aImage)
{
    if (iBitmapImage)
    {
        iBitmapImage->RemoveRef();
        iBitmapImage = NULL;
    }

    if (aImage)
    {
        iBitmapImage = aImage->BitmapImage();
    }

    if (iDialog)
    {
        SetImageOrAnimToDialogL();
    }
}

/** If we have an image, this image is set into the dialog. If the dialog had an
animation then the animation is hidden. If we have no image we make sure the dialog
does not have an image either (by calling SetImage(NULL). If the dialog has an
animation we then make the animation visible again. */
void CMIDAlert::SetImageOrAnimToDialogL()
{
    ASSERT(iDialog);
    iDialog->DisableVisibilityChange(ETrue);

    if (iBitmapImage)
    { //remove animation if any and add image
        if (iDialog->Flags().IsSet(EPopupFormHasAnimation))
        {
            iDialog->DeleteControl(EPopupFormAnimation);
        }

        if (!iDialog->Flags().IsSet(EPopupFormHasImage))
        {
            iDialog->InsertControlL(0, R_MIDP_ALERT_IMAGE_LINE, EFalse);
        }

        CEikImage* newImage = CreateImageLC();
        CEikImage* dlgImage = (CEikImage*)(iDialog->GetControlByControlType(EEikCtImage)->iControl);
        newImage->SetPictureOwnedExternally(ETrue);
        dlgImage->SetBitmap(newImage->Bitmap());
        dlgImage->SetMask(newImage->Mask());
        CleanupStack::PopAndDestroy(newImage);
    }
    else if (iAlertAnimation > 0)
    { //or else remove image if any and add  animation
        if (iDialog->Flags().IsSet(EPopupFormHasImage))
        {
            iDialog->DeleteControl(EPopupFormImage);
        }

        if (iDialog->Flags().IsSet(EPopupFormHasAnimation))
        { //because animation may have changed
            iDialog->DeleteControl(EPopupFormAnimation);
        }
        iDialog->DisableVisibilityChange(EFalse);
        iDialog->InsertControlL(0, iAlertAnimation, EFalse);
        iDialog->DisableVisibilityChange(ETrue);
    }
    else
    { //remove any animation or image that should not be there anymore
        if (iDialog->Flags().IsSet(EPopupFormHasAnimation))
        {
            iDialog->DeleteControl(EPopupFormAnimation);
        }

        if (iDialog->Flags().IsSet(EPopupFormHasImage))
        {
            iDialog->DeleteControl(EPopupFormImage);
        }
    }

    iDialog->DisableVisibilityChange(EFalse);
    iDialog->DrawDeferred();
}

/** Creates a CEikImage starting from iBitmapImage. The CEikImage
can then be inserted into the dialog. @see ResizeImage(), SetImageToDialogL() */
CEikImage* CMIDAlert::CreateImageLC() const
{
    ASSERT(iBitmapImage);

    CFbsBitmap* bmp = CMIDUtils::CopyBitmapL(iBitmapImage->ColorBitmap());
    CleanupStack::PushL(bmp);

    CFbsBitmap* mask = NULL;
    if (iBitmapImage->AlphaBitmap())
    {
        //alert fails to display transparent images of type EGray2 so we
        //force a EGray256, which we have verified to be working.
        mask = iBitmapImage->CreateAlphaBitmapL(EGray256, EFalse);
    }

    CleanupStack::PushL(mask);

    CEikImage* eikImage = new(ELeave) CEikImage;
    CleanupStack::Pop(mask);
    CleanupStack::Pop(bmp);

    ResizeImage(bmp,mask);
    eikImage->SetPicture(bmp, mask);

    CleanupStack::PushL(eikImage);
    return eikImage;
}

/** Resize the image (bitmap and mask if applicable). In theory the
AVKON dialog should take care of this but there is panic in CEikCaptionedControl
if we don't do this. If the image is smaller it ends up at the top left. If it is
bigger it is clipped starting at top left. */
void CMIDAlert::ResizeImage(CFbsBitmap* aImage, CFbsBitmap* aMask) const
{
    CMIDMenuHandler* menuHandler = iDisplayable->MenuHandler();


    TAknWindowLineLayout lineLayout;
    lineLayout = AknLayoutScalable_Avkon::popup_midp_note_alarm_window_g1(0).LayoutLine();

    TSize goodSize = TSize(lineLayout.iW, lineLayout.iH);
    TSize imageSize = aImage->SizeInPixels();

    if (imageSize.iWidth > goodSize.iWidth ||
            imageSize.iHeight > goodSize.iHeight)
    {
        TSize newSize(Min(goodSize.iWidth, imageSize.iWidth),Min(goodSize.iHeight, imageSize.iHeight));
        aImage->Resize(newSize);

        if (aMask)
        {
            aMask->Resize(newSize);
        }
    }
}

/** Set a progress bar indicator. */
void CMIDAlert::SetIndicatorL(MMIDGauge* aGauge)
{
    DEBUG("CMIDAlert::SetIndicatorL <");
    iDialog->SetGaugeL(static_cast< CMIDGaugeItem* >(aGauge));
    DEBUG("CMIDAlert::SetIndicatorL >");
}

/** Deletes an existing dialog and creates a new one.

    Note: this should be called in ConstructL()
    or when the alert type changes. For everything else
    an existing dialog is updated.
 */
void CMIDAlert::CreateDialogL()
{
    DEBUG("CMIDAlert::CreateDialogL <");
    ASSERT(!iDialog);

    iDialog = CMIDAlertDialog::NewL(this, iDisplayable);

    TInt resource = SetAlertResourceIdsL();
    iDialog->SetTone(iAlertTone);
    iDialog->SetPopupFormType(EAknNote);

    iDialog->PrepareLC(resource);
    CleanupStack::Pop(iDialog);
    iEikonEnv->EikAppUi()->RemoveFromStack(iDialog);

    iDialog->ButtonGroupContainer().MakeVisible(EFalse);

    DEBUG("CMIDAlert::CreateDialogL - SetImageOrAnimToDialogL <");
    SetImageOrAnimToDialogL();
    DEBUG("CMIDAlert::CreateDialogL - SetImageOrAnimToDialogL <");

    iDialog->SetTextL(iUserText ? iUserText : iDefaultText);
    iDialog->DrawDeferred();
    iDialog->SizeChanged();

    if (IsModal())
    {
        SetModalL();
    }
    if (iDisplayable->HasTitle())
    {
        SetTitleL(iDisplayable->Title());
    }

    DEBUG("CMIDAlert::CreateDialogL >");
}

/** This is called when the owning displayable is asked to
come to the foreground or to go to the background. If coming
to the foreground, we set the timeout, play a sound and display
the dialog. If going to the background we hide the dialog. */
void CMIDAlert::HandleCurrentL(TBool aCurrent)
{
    DEBUG_INT("CMIDAlert::HandleCurrentL < (%d)", aCurrent);

    if (aCurrent)
    {
        if (IsModal())
        {
            iTimeout = MMIDAlert::EForever;
        }

        SetTimeoutL(iTimeout);
        PlaySound();

        iEikonEnv->EikAppUi()->AddToStackL(iDialog,ECoeStackPriorityDialog);

        // The flag  flags |= CEikButtonGroupContainer::EDelayActivation
        // is set in CEikDialog::CreateButtonGroupContainerL , It means that button
        // will not be activeated automaticaly after it is create, it need to be activated
        iDialog->ButtonGroupContainer().ActivateL();
        iDialog->ShowL(ETrue);
    }
    else
    {
        iDialog->ShowL(EFalse);
        iEikonEnv->EikAppUi()->RemoveFromStack(iDialog);
    }

    DEBUG("CMIDAlert::HandleCurrentL >");
}

/** The alert type is used to present a
visually different kind of alert to the user.
If a diferent type is set we must reset the animation
(unless an image has already been set and this is taken
care of by SetImageOrAnimtoDialogL()),
the tone and the text (unless a user text was given).
Finally we draw the dialog.
*/
void CMIDAlert::SetTypeL(TAlertType aType)
{
    if (iAlertType != aType)
    {
        iAlertType = aType;
        SetAlertResourceIdsL();

        iDialog->SetTone(iAlertTone);
        SetImageOrAnimToDialogL();

        iDialog->SetTextL(iUserText ? iUserText : iDefaultText);

        iDialog->DrawDeferred();
    }
}

/** Return the alert default timeout. */
TInt CMIDAlert::DefaultTimeout()
{
    if (iAlertType == EConfirmation)
    {
        return KConfirmationNoteTimeout;
    }
    else
    {
        return KAlertDefaultTimeout;
    }
}

/** Sets the timeout for a timed alert. */
void CMIDAlert::SetTimeoutL(TInt aTime)
{
    iTimeout = aTime;

    if (!iDialog)
    {//no need to start the t.o. if dialog is not there yet
        return;
    }

    delete iDialogTimer;
    iDialogTimer = NULL;

    TTimeIntervalMicroSeconds32 interval = aTime * KMillToMicroSeconds;
    if (interval.Int() >= 0)
    {
        iDialogTimer = CPeriodic::NewL(CActive::EPriorityStandard);
        iDialogTimer->Start(interval, interval, TCallBack(TimerCallBack,this));
    }
}

TInt CMIDAlert::TimerCallBack(TAny* aThis)
{
    CMIDAlert* self = STATIC_CAST(CMIDAlert*, aThis);
    if (self)
    {
        self->DoTimerCallBack();
    }

    return KErrNone;
}

/** When the timeout expires, try to dismiss the dialog.
We cannot delete the dialog here because we need java
side to set another Displayable first. If java side does
not do this - because of a badly behaved midlet - we keep
the dialog showing. */
void CMIDAlert::DoTimerCallBack()
{
    if (!IsModal())
    {// The dialog may have become modal whilst
        // the timer was running
        TryToDismissDialog();
    }

    if (iDialogTimer)
    {
        iDialogTimer->Cancel();
    }
}

/**
* Handles a change to the control's resources.
*/
void CMIDAlert::HandleResourceChange(TInt aType)
{
    if (aType == KAknsMessageSkinChange)
    {
        TRAP_IGNORE(SetImageOrAnimToDialogL(); iDialog->InsertGaugeIntoDialogL(););
    }
}

/** Post a request java side to try and dismiss the dialog */
void CMIDAlert::TryToDismissDialog()
{
    if (iDialog)
    {
        iEnv->PostJavaEvent(*this,EDisplayable);
    }
}

#ifdef RD_SCALABLE_UI_V2
TBool CMIDAlert::TryDetectLongTapL(const TPointerEvent& aPointerEvent)
{
    return iDisplayable->TryDetectLongTapL(aPointerEvent);
}
#endif

/** Return true if the alert is scrollable. */
TBool CMIDAlert::IsScrollable()
{
    if (iDialog)
    {
        return iDialog->TextIsScrollable();
    }
    else
    {
        return EFalse;
    }
}

/** -----------------------------------------------------------------------------
// CMIDAlert::SetModalL
//
// A modal Alert has the timeout set to FOREVER and must be dismissed using a Command.
// All Alerts have either the DISMISS_COMMAND or a command added by the application.
//
// An Alert becomes modal when:
// - The timeout is set to FOREVER
// - There are two or more commands
// - The content becomes so large that it must scroll
//
// The java framework will call SetModal() when two or more commands are added.
// The other two conditions are tested in CMIDAlert.
//
// When the reverse of all the above is true, the Alert reverts to being non modal.
// In this case, the timer will only be re started when the Alert is made current
// -----------------------------------------------------------------------------
*/
void CMIDAlert::SetModalL()
{
    iTimeout = MMIDAlert::EForever;
    SetTimeoutL(iTimeout);

    iDialog->UpdateCbasL();
}

/** Returns true if the alert is modal (not timed, scrollable or with more than one
command). Otherwise it returns false.*/
TBool CMIDAlert::IsModal()
{
    return (IsScrollable()
            || (iTimeout == MMIDAlert::EForever)
            || (iDisplayable->CommandCount() >= KAlertModalCommandCount));
}

/** Return the resource ID depending on the dialog type. Also
set the alert animation, tone, and default text according to
the alert type. */
TInt CMIDAlert::SetAlertResourceIdsL()
{
    // More resource types might be needed if an alert wants to show a
    // gauge with correct layout....
    delete iDefaultText;
    iDefaultText = NULL;

    switch (iAlertType)
    {
    case EAlarm:
        iDefaultText = iEikonEnv->AllocReadResourceL(R_MIDP_ALERT_ALARM_DEFAULT_TEXT);
        iAlertAnimation = R_MIDP_ALARM_NOTE_ANIMATION;
        iAlertTone =  EInformationTone;
        return R_MIDP_ALARM_NOTE_DIALOG;
    case EConfirmation:
        iDefaultText = iEikonEnv->AllocReadResourceL(R_MIDP_ALERT_CONF_DEFAULT_TEXT);
        iAlertAnimation = R_MIDP_CONFIRMATION_NOTE_ANIMATION;
        iAlertTone =  EConfirmationTone;
        return R_MIDP_CONFIRMATION_NOTE_DIALOG;
    case EError:
        iDefaultText = iEikonEnv->AllocReadResourceL(R_MIDP_ALERT_ERROR_DEFAULT_TEXT);
        iAlertAnimation = R_MIDP_ERROR_NOTE_ANIMATION;
        iAlertTone =  EErrorTone;
        return R_MIDP_ERROR_NOTE_DIALOG;
    case EWarning:
        iDefaultText = iEikonEnv->AllocReadResourceL(R_MIDP_ALERT_WARNING_DEFAULT_TEXT);
        iAlertAnimation = R_MIDP_WARNING_NOTE_ANIMATION;
        iAlertTone =  EWarningTone;
        return R_MIDP_WARNING_NOTE_DIALOG;
    case EInfo:
        iDefaultText = iEikonEnv->AllocReadResourceL(R_MIDP_ALERT_INFO_DEFAULT_TEXT);
        iAlertAnimation = R_MIDP_INFORMATION_NOTE_ANIMATION;
        iAlertTone =  EInformationTone;
        return R_MIDP_INFORMATION_NOTE_DIALOG;
    case ENone: // No Alerttype set, use information note
        iDefaultText = iEikonEnv->AllocReadResourceL(R_MIDP_ALERT_NULL_DEFAULT_TEXT);
        iAlertAnimation = 0;
        iAlertTone = ENoTone;
        return R_MIDP_NONE_ALERTTYPE_DIALOG;
    default:
        ASSERT(EFalse);
        return R_MIDP_NONE_ALERTTYPE_DIALOG;
    }
}

/** Called java side to release the native implementation of the dialog, i.e. us */
void CMIDAlert::Dispose()
{
    delete this;
}

// End of File
