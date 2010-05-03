/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Controls the native soft notifications.
*
*/


// INCLUDE FILES
#include "CSoftNotification.h"
#include "CSoftNotificationEvent.h"
#include "CSoftNotificationEventSource.h"

#include <AknDynamicSoftNotifier.h>
#include <AknDynamicSoftNotificationParams.h>
#include <jdebug.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <vwsdefpartner.h>
#endif


// CONSTANTS
namespace
{
/// Default soft notification priority (range: 1000...3000)
const TInt KSoftNotificationPriority = 1000;
/// Event id when user accepted notification
const TInt KEventNoteAccepted = 1;
/// Event id when user canceled notification
const TInt KEventNoteCanceled = 2;
}

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CSoftNotification::NewLC
// Static constructor
// -----------------------------------------------------------------------------
//
CSoftNotification* CSoftNotification::NewL(
    TInt aAppId,
    TInt aNotificationId,
    CSoftNotificationEventSource& aEventSource)
{
    DEBUG("CSoftNotification::NewL");

    CSoftNotification* self =
        new(ELeave) CSoftNotification(aAppId,
                                      aNotificationId,
                                      aEventSource);
    CleanupStack::PushL(self);

    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSoftNotification::~CSoftNotification()
{
    DEBUG("CSoftNotification::~CSoftNotification +");

    delete iSoftkey1;
    delete iSoftkey2;

    delete iLabel;
    delete iGroupLabel;

    delete iImageData;

    delete iNotifier;

    DEBUG("CSoftNotification::~CSoftNotification -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::NotificationAccepted
// Dynamic soft notification was accepted by user.
// -----------------------------------------------------------------------------
//
void CSoftNotification::NotificationAccepted(TInt aIdentifier)
{
    DEBUG_INT("CSoftNotification::NotificationAccepted +, id=%d", aIdentifier);

    if (iNotificationId == aIdentifier && iPeer)
    {
        // Error ignored since there is no other way to get the error to
        // the Java side than posting.
        TRAP_IGNORE(PostEventL(KEventNoteAccepted));
    }

    DEBUG("CSoftNotification::NotificationAccepted -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::NotificationCanceled
// Dynamic soft notification was canceled by user.
// -----------------------------------------------------------------------------
//
void CSoftNotification::NotificationCanceled(TInt aIdentifier)
{
    DEBUG_INT("CSoftNotification::NotificationCanceled +, id=%d", aIdentifier);

    if (iNotificationId == aIdentifier && iPeer)
    {
        // Error ignored since there is no other way to get the error to
        // the Java side than posting.
        TRAP_IGNORE(PostEventL(KEventNoteCanceled));
    }

    DEBUG("CSoftNotification::NotificationCanceled -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::ShowSoftNotificationL
// Displays a soft notification
// -----------------------------------------------------------------------------
//
void CSoftNotification::ShowSoftNotificationL()
{
    DEBUG("CSoftNotification::ShowSoftNotificationL +");

    TAknDynamicSoftNotificationParams param(KSoftNotificationPriority);
    FillNotificationParams(param);

    TInt oldId = iNotificationId;
    iNotificationId =
        iNotifier->SetDynamicNotificationCountL(param, iNotificationId, 1);

    // Stop observing the removed note (=id changed).
    if (oldId != iNotificationId)
    {
        iNotifier->StopObserving(oldId);
        iNotifier->StartObservingL(iNotificationId, this);
    }

    DEBUG("CSoftNotification::ShowSoftNotificationL -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::RemoveSoftNotificationL
// Cancels and removes the soft notification
// -----------------------------------------------------------------------------
//
void CSoftNotification::RemoveSoftNotificationL()
{
    DEBUG("CSoftNotification::RemoveSoftNotificationL +");

    iNotifier->CancelDynamicNotificationL(iNotificationId);

    DEBUG("CSoftNotification::RemoveSoftNotificationL -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::SetTextL
// Sets a text for a soft notification
// -----------------------------------------------------------------------------
//
void CSoftNotification::SetTextL(const TDesC& aText, const TDesC& aGroupText)
{
    DEBUG("CSoftNotification::SetTextL +");

    HBufC* txt = aText.AllocL();
    delete iLabel;
    iLabel = txt;

    txt = aGroupText.AllocL();
    delete iGroupLabel;
    iGroupLabel = txt;

    DEBUG("CSoftNotification::SetTextL -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::SetSoftkeyLabelsL
// Sets new labels for softkeys
// -----------------------------------------------------------------------------
//
void CSoftNotification::SetSoftkeyLabelsL(
    const TDesC& aSoftkey1Label,
    const TDesC& aSoftkey2Label)
{
    DEBUG("CSoftNotification::SetSoftkeyLabelsL +");

    HBufC* txt = aSoftkey1Label.AllocL();
    delete iSoftkey1;
    iSoftkey1 = txt;

    txt = aSoftkey2Label.AllocL();
    delete iSoftkey2;
    iSoftkey2 = txt;

    DEBUG("CSoftNotification::SetSoftkeyLabelsL -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::SetImageL
// Sets an image for a soft notification
// -----------------------------------------------------------------------------
//
void CSoftNotification::SetImageL(
    const TDesC8& aImage)
{
    DEBUG("CSoftNotification::SetImageL +");

    HBufC8* image = aImage.AllocL();
    delete iImageData;
    iImageData = image;

    DEBUG("CSoftNotification::SetImageL -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::Id
// Notification Id
// -----------------------------------------------------------------------------
//
TInt CSoftNotification::Id()
{
    DEBUG("CSoftNotification::Id");

    return iNotificationId;
}

// -----------------------------------------------------------------------------
// CSoftNotification::SetPeerObserverL
// -----------------------------------------------------------------------------
//
void CSoftNotification::SetPeerObserver(jobject aPeer, jmethodID aMethodId)
{
    DEBUG("CSoftNotification::SetPeerObserver");

    iPeer = aPeer;
    iMethodId = aMethodId;
}

// -----------------------------------------------------------------------------
// CSoftNotification::RemovePeerObserverL
// -----------------------------------------------------------------------------
//
void CSoftNotification::RemovePeerObserver(JNIEnv& aJniEnv)
{
    DEBUG("CSoftNotification::RemovePeerObserver +");

    // The stored global reference has to be deleted since it is not
    // used anymore.
    if (iPeer)
    {
        aJniEnv.DeleteWeakGlobalRef((jweak)iPeer);
    }

    iPeer = NULL;
    iMethodId = NULL;

    DEBUG("CSoftNotification::RemovePeerObserver -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSoftNotification::ConstructL()
{
    DEBUG("CSoftNotification::ConstructL +");

    iNotifier = CAknDynamicSoftNotifier::NewL();

    // Try to start observing. Ignore the leave, it means that the note is
    // already observed by another instance of this class or the note is not
    // valid anymore.
    // The observing has to be started after creation so that notifications
    // are observed in every case (use case: an instance of this class is
    // created by using an id to a notification already visible on the screen).
    TRAP_IGNORE(iNotifier->StartObservingL(iNotificationId, this));

    DEBUG("CSoftNotification::ConstructL -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::PostEventL
// Posts an event to the Java side.
// -----------------------------------------------------------------------------
//
void CSoftNotification::PostEventL(TInt aEventId)
{
    DEBUG("CSoftNotification::PostEventL +");

    CSoftNotificationEvent* javaEvent =
        new(ELeave) CSoftNotificationEvent(iPeer, iMethodId, aEventId);

    // Event source takes the ownership to the event.
    iEventSource.PostEvent(javaEvent, CJavaEventBase::EEventPriority);

    DEBUG("CSoftNotification::PostEventL -");
}

// -----------------------------------------------------------------------------
// CSoftNotification::CSoftNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSoftNotification::CSoftNotification(
    TInt aAppId,
    TInt aNotificationId,
    CSoftNotificationEventSource& aEventSource) :
        iNotificationId(aNotificationId),
        iEventSource(aEventSource)
{
    DEBUG("CSoftNotification::CSoftNotification");

    iAppId = TUid::Uid(aAppId);
}

// -----------------------------------------------------------------------------
// CSoftNotification::FillNotificationParams
// -----------------------------------------------------------------------------
//
void CSoftNotification::FillNotificationParams(
    TAknDynamicSoftNotificationParams& aParam)
{
    DEBUG("CSoftNotification::FillNotificationParams +");

    if (iSoftkey1 && iSoftkey2)
    {
        aParam.SetSoftkeys(*iSoftkey1, *iSoftkey2);
    }

    if (iLabel)
    {
        aParam.SetNoteLabels(*iLabel, *iLabel);
    }

    if (iGroupLabel)
    {
        aParam.SetGroupLabels(*iGroupLabel, *iGroupLabel);
    }

    if (iImageData)
    {
        aParam.SetImageData(*iImageData);
    }

    TVwsViewId view(iAppId, iAppId);
    aParam.SetViewActivationParams(view);
    aParam.EnableObserver();

    DEBUG("CSoftNotification::FillNotificationParams -");
}

//  End of File
