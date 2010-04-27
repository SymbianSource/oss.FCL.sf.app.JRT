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


#include <AknUtils.h>
#include <aknnotecontrol.h>
#include <eswtcore.rsg>
#include <swtpopupformproxy.h>

#include "eswtmobile.hrh"
#include "swtcontrolhelper.h"
#include "swttimedmessagebox.h"
#include "swtlaffacade.h"

const TInt KSwtTimedMsgBoxSecTimeout = 3;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtTimedMessageBox::RunDlgLD
// ---------------------------------------------------------------------------
//
void CSwtTimedMessageBox::RunDlgLD(const TDesC& aTitle, const TDesC& aMessage,
                                   TInt aStyle, const MSwtImage* aImage)
{
    CSwtTimedMessageBox* dlg = CSwtTimedMessageBox::NewLC(aTitle, aMessage, aStyle, aImage);
    CleanupStack::Pop(dlg);
    dlg->RunL();
    delete dlg;
}

// ---------------------------------------------------------------------------
// CSwtTimedMessageBox::NewLC
// ---------------------------------------------------------------------------
//
CSwtTimedMessageBox* CSwtTimedMessageBox::NewLC(const TDesC& aTitle, const TDesC& aMessage,
        TInt aStyle, const MSwtImage* aImage)
{
    CSwtTimedMessageBox* self = new(ELeave) CSwtTimedMessageBox(aStyle);
    CleanupStack::PushL(self);
    self->ConstructL(aTitle, aMessage, aImage);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtTimedMessageBox::~CSwtTimedMessageBox
// ---------------------------------------------------------------------------
//
CSwtTimedMessageBox::~CSwtTimedMessageBox()
{
    delete iPopupForm;
    iPopupForm = NULL;
}

// ---------------------------------------------------------------------------
// CSwtTimedMessageBox::RunL
// ---------------------------------------------------------------------------
//
void CSwtTimedMessageBox::RunL()
{
    iPopupForm->RunL();
}

// ---------------------------------------------------------------------------
// CSwtTimedMessageBox::CSwtTimedMessageBox
// ---------------------------------------------------------------------------
//
CSwtTimedMessageBox::CSwtTimedMessageBox(TInt aStyle)
        : iStyle(aStyle)
{
    // We support SWT.APPLICATION_MODAL only
    ASSERT((aStyle & KSwtModalityMask) == KSwtStyleApplicationModal);
}

// ---------------------------------------------------------------------------
// CSwtTimedMessageBox::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtTimedMessageBox::ConstructL(const TDesC& aTitle,
                                     const TDesC& aMessage, const MSwtImage* aImage)
{
    // create CEikImage
    CEikImage* image(NULL);
    if (aImage)
    {
        // Retrieve the maximum size of PopupForm image
        TSize maxImageSize = CSwtLafFacade::GetLayoutRect(
                                 CSwtLafFacade::EPopupMidpNoteAlarmWindowG1, TRect(), 0).Rect().Size();
        TSize imageSize = aImage->Bitmap().SizeInPixels();
        TSize finalSize = imageSize;
        TBool doScaling = EFalse;

        if (imageSize.iWidth > maxImageSize.iWidth ||
                imageSize.iHeight > maxImageSize.iHeight)
        {
            // If the image is bigger than the space for it in PopupForm,
            // scale the image to fit the space (preserving aspect ratio)
            doScaling = ETrue;
            finalSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                            imageSize, maxImageSize);
        }

        image = new(ELeave) CEikImage();
        CFbsBitmap* bitmap = 0;
        CFbsBitmap* mask = 0;
        if (doScaling)
        {
            bitmap = SwtControlHelper::GetCopyOfBitmapLC(
                         &aImage->Bitmap(), finalSize);
            mask = SwtControlHelper::GetInvertedCopyOfMonoBitmapLC(
                       aImage->MaskBitmap(), finalSize);
        }
        else
        {
            bitmap = SwtControlHelper::GetCopyOfBitmapLC(
                         &aImage->Bitmap());
            mask = SwtControlHelper::GetInvertedCopyOfMonoBitmapLC(
                       aImage->MaskBitmap());
        }
        image->SetNewBitmaps(bitmap, mask);
        image->SetPictureOwnedExternally(EFalse);
        CleanupStack::Pop(mask ? 2 : 1);
    }

    iPopupForm = CSwtPopupFormProxy::NewL(aMessage, aTitle, R_SWT_MSGBX_HEADING_LINE,
                                          R_SWT_MSGBX_SPACER_LINE, R_SWT_TIMEDMSGBX, 0, Animation(), iStyle,
                                          R_SWT_MSGBX_MENUBAR, 0, 0, KSwtTimedMsgBoxSecTimeout, image, R_SWT_TIMEDMSGBX_IMAGE);
}

// ---------------------------------------------------------------------------
// CSwtTimedMessageBox::Animation
// ---------------------------------------------------------------------------
//
TInt CSwtTimedMessageBox::Animation() const
{
    if (iStyle & KSwtMsgBoxStyleInformation)
    {
        return R_SWT_MSGBX_INFO_ANIM;
    }
    else if (iStyle & KSwtMsgBoxStyleError)
    {
        return R_SWT_MSGBX_ERROR_ANIM;
    }
    else if (iStyle & KSwtMsgBoxStyleWarning)
    {
        return R_SWT_MSGBX_WARNING_ANIM;
    }
    else if (iStyle & KSwtMsgBoxStyleQuestion)
    {
        return R_SWT_MSGBX_QUESTION_ANIM;
    }
    else if (iStyle & KSwtMsgBoxStyleWorking)
    {
        return R_SWT_MSGBX_WORKING_ANIM;
    }
    else
    {
        return 0;
    }
}
