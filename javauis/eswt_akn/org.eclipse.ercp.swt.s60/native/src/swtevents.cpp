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


#include <coecntrl.h>
#include <AknUtils.h>
#include "swtevents.h"
#include "eswtwidgetscore.h"
#include "eswtwidgetsexpanded.h"
#include "javalocalref.h"
#include "methodcall.h"
#include "utils.h"
#include "swtdialogbroker.h"


// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CSwtEvent::CSwtEvent
// -----------------------------------------------------------------------------
//
inline CSwtEvent::CSwtEvent(TSwtPeer aPeer)
        : iPeer(aPeer)
{
    ASSERT(iPeer);
}

// -----------------------------------------------------------------------------
// CSwtEvent::Dispatch
// -----------------------------------------------------------------------------
//
void CSwtEvent::Dispatch(JNIEnv* aJniEnv)
{
    ASSERT(!IsDisplayDisposed());
    ASSERT(!IsPeerDisposed());
    DoDispatch(aJniEnv);
}

// -----------------------------------------------------------------------------
// CSwtCallbackEvent::CSwtCallbackEvent
// -----------------------------------------------------------------------------
//
inline CSwtCallbackEvent::CSwtCallbackEvent(TSwtPeer aPeer)
        : CSwtEvent(aPeer)
{
}

// -----------------------------------------------------------------------------
// CSwtCallbackEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtCallbackEvent::DoDispatch(JNIEnv* aJniEnv)
{
    // Dispatch the message
    jboolean doIt;
    TBool ok = SendEvent(aJniEnv, doIt);

    // Check if further execution should stop
    if (IsDisplayDisposed() || IsPeerDisposed() || !ok)
    {
        return;
    }

    // Call back the originator
    TRAPD(error, CallMethodL(this, &CSwtCallbackEvent::CallbackL, doIt));
    ThrowIfError(error, aJniEnv);
}

// -----------------------------------------------------------------------------
// CSwtPlainEvent::CSwtPlainEvent
// -----------------------------------------------------------------------------
//
CSwtPlainEvent::CSwtPlainEvent(TSwtPeer aPeer, TSwtEventType aType)
        : CSwtEvent(aPeer)
        , iType(aType)
        , iDetail(KSwtNone)
        , iItemPeer(NULL)
{
}

// -----------------------------------------------------------------------------
// CSwtPlainEvent::CSwtPlainEvent
// -----------------------------------------------------------------------------
//
CSwtPlainEvent::CSwtPlainEvent(TSwtPeer aPeer, TSwtEventType aType,
                               TInt aDetail, TSwtPeer aItemPeer)
        : CSwtEvent(aPeer)
        , iType(aType)
        , iDetail(aDetail)
        , iItemPeer(aItemPeer)
{
}

// -----------------------------------------------------------------------------
// CSwtPlainEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtPlainEvent::Type() const
{
    return iType;
}

// -----------------------------------------------------------------------------
// CSwtPlainEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtPlainEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendEvent",
                       "(IILorg/eclipse/swt/widgets/Widget;)V", static_cast<jint>(iType),
                       iDetail, iItemPeer);
}

// -----------------------------------------------------------------------------
// CSwtKeyEvent::CSwtKeyEvent
// -----------------------------------------------------------------------------
//
CSwtKeyEvent::CSwtKeyEvent(MSwtControl& aControl, const TSwtKeyEventData& aData,
                           const TKeyEvent& aKeyEvent, TEventCode aType)
        : CSwtCallbackEvent(aControl.JavaPeer())
        , iControl(aControl)
        , iSwtType(aData.iType)
        , iCharacter(static_cast<jchar>(aData.iCharacter))
        , iKeyCode(aData.iKeyCode)
        , iStateMask(aData.iStateMask)
        , iKeyEvent(aKeyEvent)
        , iType(aType)
{
    ASSERT(aType == EEventKeyDown || aType == EEventKey || aType == EEventKeyUp);
}

// -----------------------------------------------------------------------------
// CSwtKeyEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtKeyEvent::Type() const
{
    return iSwtType;
}

// -----------------------------------------------------------------------------
// CSwtKeyEvent::SendEvent
// -----------------------------------------------------------------------------
//
TBool CSwtKeyEvent::SendEvent(JNIEnv* aJniEnv, jboolean& aDoIt)
{
    TBool failed;
    aDoIt = CallBooleanJavaMethod(failed, aJniEnv, Peer(), "sendKeyEvent",
                                  "(ICII)Z", static_cast<jint>(iSwtType), iCharacter, iKeyCode,
                                  iStateMask);
    return (failed || aJniEnv->ExceptionCheck()) ? EFalse : ETrue;
}

// -----------------------------------------------------------------------------
// CSwtKeyEvent::CSwtKeyEvent
// -----------------------------------------------------------------------------
//
void CSwtKeyEvent::CallbackL(TBool aDoIt)
{
    if (aDoIt)
    {
        // There is no difference between key-down and key event in SWT. Therefore,
        // those events are merged. If this is such an event, let's un-merge.
        // EEventKeyDown can be posted only if the key does not generate EEventKey!
        if (iType == EEventKey && iKeyEvent.iRepeats == 0)
        {
            TKeyEvent keyDownEvent(iKeyEvent);
            keyDownEvent.iCode = 0;
            iControl.ProcessKeyEventL(keyDownEvent, EEventKeyDown);
        }
        iControl.ProcessKeyEventL(iKeyEvent, iType);

#ifndef RD_JAVA_S60_RELEASE_9_2
        if (AknLayoutUtils::MSKEnabled())
        {
            // Cba consumes the MSK event key, we have to create a simulated
            // EEventKey for EStdKeyDevice3 key down event.
            if (iType == EEventKeyDown && iKeyEvent.iScanCode == EStdKeyDevice3 &&
                    iKeyEvent.iRepeats == 0)
            {
                TKeyEvent keyEvent(iKeyEvent);
                keyEvent.iCode = EKeyOK;
                iControl.ProcessKeyEventL(keyEvent, EEventKey);
            }
        }
#endif // RD_JAVA_S60_RELEASE_9_2
    }
}

// -----------------------------------------------------------------------------
// CSwtTraverseEvent::CSwtTraverseEvent
// -----------------------------------------------------------------------------
//
CSwtTraverseEvent::CSwtTraverseEvent(MSwtControl& aControl, TSwtTraversal aDetail,
                                     TBool aDoIt, const TSwtKeyEventData& aData, const TKeyEvent& aKeyEvent,
                                     TEventCode aType)
        : CSwtKeyEvent(aControl, aData, aKeyEvent, aType)
        , iDetail(aDetail)
        , iDoItDefault(ConvertBoolean(aDoIt))
{
}


// -----------------------------------------------------------------------------
// CSwtTraverseEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtTraverseEvent::Type() const
{
    return ESwtEventTraverse;
}


// -----------------------------------------------------------------------------
// CSwtTraverseEvent::DoDispatch
// Dispatches the traverse event, and depending on the event's "doit" field may
// also dispatch the original key event.
// -----------------------------------------------------------------------------
//
void CSwtTraverseEvent::DoDispatch(JNIEnv* aJniEnv)
{
    // Dispatch the message
    jboolean doIt;
    TBool ok = SendTraverseEvent(aJniEnv, doIt);

    // Check if further execution should stop
    if (IsDisplayDisposed() || IsPeerDisposed() || !ok)
    {
        return;
    }

    // Perform traversal or key event
    if (doIt)
    {
        if (iDetail != ESwtTraverseNone)
        {
            TRAPD(error, CallMethodL(this, &CSwtTraverseEvent::TraverseCallbackL));
            ThrowIfError(error, aJniEnv);
        }
    }
    else
    {
        CSwtKeyEvent::DoDispatch(aJniEnv);
    }
}

// -----------------------------------------------------------------------------
// CSwtTraverseEvent::SendTraverseEvent
// -----------------------------------------------------------------------------
//
TBool CSwtTraverseEvent::SendTraverseEvent(JNIEnv* aJniEnv, jboolean& aDoIt)
{
    TBool failed;
    const char* signature = "(IZCII)Lorg/eclipse/swt/widgets/Event;";
    jobject event = CallObjectJavaMethod(failed, aJniEnv, Peer(),
                                         "sendTraverseEvent", signature, iDetail, iDoItDefault,
                                         iCharacter, iKeyCode, iStateMask);

    // Check if further execution should stop
    if (IsDisplayDisposed() || aJniEnv->ExceptionCheck() || failed)
    {
        return EFalse;
    }

    ASSERT(event);
    jclass clazz = aJniEnv->FindClass("org/eclipse/swt/widgets/Event");
    if (!clazz)
    {
        failed = ETrue;
    }
    else
    {
        aDoIt = GetBooleanField(aJniEnv, event, clazz, "doit", failed);
        iDetail = GetIntField(aJniEnv, event, clazz, "detail", failed);
        aJniEnv->DeleteLocalRef(clazz);
    }
    aJniEnv->DeleteLocalRef(event);

    return !failed;
}

// -----------------------------------------------------------------------------
// CSwtTraverseEvent::TraverseCallbackL
// -----------------------------------------------------------------------------
//
void CSwtTraverseEvent::TraverseCallbackL()
{
    // Ensure the control still has the focus, it may not always be so if
    // too many events have been generated before being processed
    // Must call here IsFocusControl() instead of CoeControl().IsFocused()
    // because Shells can never get Coe focus. Shell focus is defined to be
    // the state when the Shell is active and has no focused children.
    if (iControl.IsFocusControl())
    {
        // Find the Control's Shell
        MSwtShell* shell;
        MSwtControl* ctrl = &iControl;
        while ((shell = ctrl->ShellInterface()) == NULL)
        {
            ctrl = ctrl->GetParent()->Control();
        }

        // Find next focusable control
        MSwtControl* newFocus = shell->FindTraversalTargetL(iDetail, iControl);
        if (newFocus)
        {
            newFocus->CoeControl().SetFocus(ETrue, ENoDrawNow);
        }
    }
}

// -----------------------------------------------------------------------------
// CSwtMouseEvent::CSwtMouseEvent
// -----------------------------------------------------------------------------
//
CSwtMouseEvent::CSwtMouseEvent(TSwtPeer aPeer, TSwtEventType aType,
                               TInt aButton, const TPoint& aPos, TInt aStateMask)
        : CSwtEvent(aPeer)
        , iType(aType)
        , iButton(aButton)
        , iX(aPos.iX)
        , iY(aPos.iY)
        , iStateMask(aStateMask)
{
}

// -----------------------------------------------------------------------------
// CSwtMouseEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtMouseEvent::Type() const
{
    return iType;
}

// -----------------------------------------------------------------------------
// CSwtMouseEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtMouseEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "sendMouseEvent", "(IIIII)V",
                       static_cast<jint>(iType), iButton, iX, iY, iStateMask);
}

// -----------------------------------------------------------------------------
// CSwtScrollEvent::CSwtScrollEvent
// -----------------------------------------------------------------------------
//
CSwtScrollEvent::CSwtScrollEvent(TSwtPeer aPeer, TInt aDetail)
        : CSwtEvent(aPeer)
        , iDetail(aDetail)
{
    ASSERT((aDetail >= ESwtKeyArrowUp && aDetail <= ESwtKeyEnd)
           || aDetail == KSwtDrag || aDetail == 0);
}

// -----------------------------------------------------------------------------
// CSwtScrollEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtScrollEvent::Type() const
{
    return ESwtEventSelection;
}

// -----------------------------------------------------------------------------
// CSwtScrollEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtScrollEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "sendScrollEvent", "(I)V", iDetail);
}

// -----------------------------------------------------------------------------
// CSwtPaintEvent::CSwtPaintEvent
// -----------------------------------------------------------------------------
//
CSwtPaintEvent::CSwtPaintEvent(TSwtPeer aSenderPeer, TSwtPeer aShellPeer,
                               const TRect& aRect, TBool aMergeable)
        : CSwtEvent(aShellPeer)
        , iMergeable(aMergeable)
        , iRect(aRect)
        , iSenderPeer(aSenderPeer)
{
}

// -----------------------------------------------------------------------------
// CSwtPaintEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtPaintEvent::Type() const
{
    return ESwtEventPaint;
}

// -----------------------------------------------------------------------------
// CSwtPaintEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtPaintEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallObjectJavaMethod(failed, aJniEnv, Peer(), "paint",
                         "(Lorg/eclipse/swt/graphics/Rectangle;ZZ)Lorg/eclipse/swt/graphics/Rectangle;",
                         NewJavaRectangle(aJniEnv, iRect), ConvertBoolean(ETrue),
                         ConvertBoolean(ETrue));
}

// -----------------------------------------------------------------------------
// CSwtResizeEvent::CSwtResizeEvent
// -----------------------------------------------------------------------------
//
CSwtResizeEvent::CSwtResizeEvent(TSwtPeer aPeer)
        : CSwtEvent(aPeer)
{
}

// -----------------------------------------------------------------------------
// CSwtResizeEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtResizeEvent::Type() const
{
    return ESwtEventResize;
}

// -----------------------------------------------------------------------------
// CSwtResizeEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtResizeEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "sendResizeEvent", "()V");
}

// -----------------------------------------------------------------------------
// CSwtTimerCallback::CSwtTimerCallback
// -----------------------------------------------------------------------------
//
CSwtTimerCallback::CSwtTimerCallback(TSwtPeer aPeer, TInt aTimerHandle)
        : CSwtEvent(aPeer)
        , iTimerHandle(aTimerHandle)
{
}

// -----------------------------------------------------------------------------
// CSwtTimerCallback::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtTimerCallback::Type() const
{
    return ESwtEventNone;
}

// -----------------------------------------------------------------------------
// CSwtTimerCallback::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtTimerCallback::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "timerCallback", "(I)V",
                       iTimerHandle);
}

// -----------------------------------------------------------------------------
// CSwtVerifyEvent::CSwtVerifyEvent
// -----------------------------------------------------------------------------
//
CSwtVerifyEvent::CSwtVerifyEvent(MSwtVerifyEventObserver& aWidget,
                                 TSwtPeer aPeer, TInt aStart, TInt aEnd)
        : CSwtEvent(aPeer)
        , iWidget(aWidget)
        , iStart(aStart)
        , iEnd(aEnd)
{
}

// -----------------------------------------------------------------------------
// CSwtVerifyEvent::~CSwtVerifyEvent
// -----------------------------------------------------------------------------
//
CSwtVerifyEvent::~CSwtVerifyEvent()
{
    delete iOriginalText;
}

// -----------------------------------------------------------------------------
// CSwtVerifyEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CSwtVerifyEvent::ConstructL(const TDesC& aText)
{
    iOriginalText = aText.AllocL();
}

// -----------------------------------------------------------------------------
// CSwtVerifyEvent::NewL
// -----------------------------------------------------------------------------
//
CSwtVerifyEvent* CSwtVerifyEvent::NewL(MSwtVerifyEventObserver& aWidget,
                                       TSwtPeer aPeer, TInt aStart, TInt aEnd, const TDesC& aText)
{
    CSwtVerifyEvent* self = new(ELeave) CSwtVerifyEvent(aWidget, aPeer,
            aStart, aEnd);
    CleanupStack::PushL(self);
    self->ConstructL(aText);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSwtVerifyEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtVerifyEvent::Type() const
{
    return ESwtEventVerify;
}

// -----------------------------------------------------------------------------
// CSwtVerifyEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtVerifyEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TRAPD(error, DoDispatchL(aJniEnv));
    ThrowIfError(error, aJniEnv);
}

// -----------------------------------------------------------------------------
// CSwtVerifyEvent::DoDispatchL
// -----------------------------------------------------------------------------
//
void CSwtVerifyEvent::DoDispatchL(JNIEnv* aJniEnv)
{
    // Convert the original string to a Java string
    jstring text = NewJavaString(aJniEnv, *iOriginalText);
    DeleteInUiHeap(iOriginalText);
    iOriginalText = NULL;
    if (!text)
    {
        return;
    }

    // Dispatch the message
    TBool failed;
    const char* signature = "(IILjava/lang/String;)Lorg/eclipse/swt/widgets/Event;";
    RJavaLocalRef<jobject> event(aJniEnv, CallObjectJavaMethod(failed, aJniEnv,
                                 Peer(), "sendVerifyEvent", signature, iStart, iEnd, text));
    aJniEnv->DeleteLocalRef(text);

    // Check if further execution should stop
    if (IsDisplayDisposed() || failed || aJniEnv->ExceptionCheck())
    {
        return;
    }

    ASSERT(event);

    // Retrieve "doit" and "text"
    jboolean doIt = JNI_FALSE;
    HBufC* verifiedText = NULL;
    CleanupClosePushL(event);
    RJavaLocalRef<jclass> clazz(aJniEnv, aJniEnv->FindClass(
                                    "org/eclipse/swt/widgets/Event"));
    if (!clazz)
    {
        failed = ETrue;
    }
    else
    {
        CleanupClosePushL(clazz);
        doIt = GetBooleanField(aJniEnv, event, clazz, "doit", failed);
        if (doIt)
        {
            verifiedText = GetStringFieldL(aJniEnv, event, clazz, "text", failed);
        }
        CleanupStack::PopAndDestroy(&clazz);
    }
    CleanupStack::PopAndDestroy(&event);

    // Callback the originator
    if (doIt && !failed && !IsPeerDisposed())
    {
        TPtr ptr(NULL, 0, 0);
        if (verifiedText)
        {
            CleanupStack::PushL(verifiedText);
            ptr.Set(verifiedText->Des());
        }
        CallMethodL(&iWidget, &MSwtVerifyEventObserver::ProcessVerifiedTextL,
                    iStart, iEnd, ptr);
        if (verifiedText)
        {
            CleanupStack::Pop(verifiedText);
        }
    }
    delete verifiedText;
}

// -----------------------------------------------------------------------------
// CSwtScreenEvent::CSwtScreenEvent
// -----------------------------------------------------------------------------
//
CSwtScreenEvent::CSwtScreenEvent(TSwtPeer aPeer, TInt aType)
        : CSwtEvent(aPeer)
        , iType(aType)
{
}

// -----------------------------------------------------------------------------
// CSwtScreenEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtScreenEvent::Type() const
{
    return ESwtEventNone;
}

// -----------------------------------------------------------------------------
// CSwtScreenEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtScreenEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;

    const char* methodName;
    switch (iType)
    {
    case MSwtScreen::KEventScreenActivated:
        methodName = "internal_sendScreenEventActivated";
        break;
    case MSwtScreen::KEventScreenDeactivated:
        methodName = "internal_sendScreenEventDeactivated";
        break;
    case MSwtScreen::KEventScreenOrientationChanged:
        methodName = "internal_sendOrientationScreenChanged";
        break;
    default:
        return;
    }

    CallVoidJavaMethod(failed, aJniEnv, Peer(), methodName, "()V");
}

// -----------------------------------------------------------------------------
// CSwtMobileDeviceEvent::CSwtMobileDeviceEvent
// -----------------------------------------------------------------------------
//
CSwtMobileDeviceEvent::CSwtMobileDeviceEvent(TSwtPeer aPeer, TInt aEventType)
        : CSwtEvent(aPeer)
        , iEventType(aEventType)
{
}

// -----------------------------------------------------------------------------
// CSwtMobileDeviceEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtMobileDeviceEvent::Type() const
{
    return ESwtEventNone;
}

// -----------------------------------------------------------------------------
// CSwtMobileDeviceEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtMobileDeviceEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(),
                       "internal_sendMobileDeviceEvent", "(I)V", iEventType);
}

// -----------------------------------------------------------------------------
// CSwtTreeEvent::CSwtTreeEvent
// -----------------------------------------------------------------------------
//
CSwtTreeEvent::CSwtTreeEvent(TSwtPeer aPeer, TSwtEventType aType, TInt aItemHandle)
        : CSwtEvent(aPeer)
        , iType(aType)
        , iItemHandle(aItemHandle)
{
}

// -----------------------------------------------------------------------------
// CSwtTreeEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtTreeEvent::Type() const
{
    return iType;
}

// -----------------------------------------------------------------------------
// CSwtTreeEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtTreeEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendEvent",
                       "(II)V", static_cast< jint >(iType), static_cast< jint >(iItemHandle));
}

// -----------------------------------------------------------------------------
// CSwtForegroundEvent::CSwtForegroundEvent
// -----------------------------------------------------------------------------
//
CSwtForegroundEvent::CSwtForegroundEvent(TSwtPeer aPeer,
        const TBool& aForeground)
        : CSwtEvent(aPeer)
        , iForeground(aForeground)
{
}

// -----------------------------------------------------------------------------
// CSwtForegroundEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtForegroundEvent::Type() const
{
    return ESwtEventForeground;
}

// -----------------------------------------------------------------------------
// CSwtForegroundEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtForegroundEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    jboolean jForeground = JNI_TRUE;
    if (!iForeground)
    {
        jForeground = JNI_FALSE;
    }
    CallStaticVoidJavaMethod(failed, aJniEnv,
                             "org/eclipse/swt/internal/symbian/ForegroundNotifyWrapper",
                             "setForeground", "(Z)V", jForeground);
}

// -----------------------------------------------------------------------------
// CSwtLocationChangingEvent::CSwtLocationChangingEvent
// -----------------------------------------------------------------------------
//
CSwtLocationChangingEvent::CSwtLocationChangingEvent(TSwtPeer aPeer,
        MSwtBrowser& aBrowser, TBool aDoIt, TBool aTop,
        TSwtBrCallBackOperationType aCallBackOperationType)
        : CSwtCallbackEvent(aPeer)
        , iBrowser(&aBrowser)
        , iDoIt(aDoIt)
        , iTop(aTop)
        , iCallBackOperationType(aCallBackOperationType)
{
}

// -----------------------------------------------------------------------------
// CSwtLocationChangingEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CSwtLocationChangingEvent::ConstructL(const TDesC& aLocation)
{
    iLocation = aLocation.AllocL();
}

// -----------------------------------------------------------------------------
// CSwtLocationChangingEvent::NewL
// -----------------------------------------------------------------------------
//
CSwtLocationChangingEvent* CSwtLocationChangingEvent::NewL(TSwtPeer aPeer,
        MSwtBrowser& aBrowser, TBool aDoIt, TBool aTop,
        const TDesC& aLocation, TSwtBrCallBackOperationType aCallBackOperationType)
{
    CSwtLocationChangingEvent* self = new(ELeave) CSwtLocationChangingEvent(
        aPeer, aBrowser, aDoIt, aTop, aCallBackOperationType);
    CleanupStack::PushL(self);
    self->ConstructL(aLocation);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSwtLocationChangingEvent::~CSwtLocationChangingEvent
// -----------------------------------------------------------------------------
//
CSwtLocationChangingEvent::~CSwtLocationChangingEvent()
{
    delete iLocation;
}

// -----------------------------------------------------------------------------
// CSwtLocationChangingEvent::SendEvent
// -----------------------------------------------------------------------------
//
TBool CSwtLocationChangingEvent::SendEvent(JNIEnv* aJniEnv, jboolean& aDoIt)
{
    TBool failed;
    jstring location = NewJavaString(aJniEnv, *iLocation);
    aDoIt = CallBooleanJavaMethod(failed, aJniEnv, Peer(),
                                  "internal_sendLocationChangingEvent", "(ZZLjava/lang/String;)Z",
                                  static_cast<jboolean>(iDoIt), static_cast<jboolean>(iTop),
                                  location);
    // Don't delete iLocation here, CallBack function still needs it.
    return (failed || aJniEnv->ExceptionCheck()) ? EFalse : ETrue;
}

// -----------------------------------------------------------------------------
// CSwtLocationChangingEvent::CallbackL
// This function is always called in UI thread
// -----------------------------------------------------------------------------
//
void CSwtLocationChangingEvent::CallbackL(TBool aDoIt)
{
    if (aDoIt)
    {
        // Load URL based on iCallBackOperationType
        TRAP_IGNORE(iBrowser->DoSetUrlL(*iLocation, iCallBackOperationType));
    }
    delete iLocation;
    iLocation = NULL;
}

// -----------------------------------------------------------------------------
// CSwtLocationChangedEvent::CSwtLocationChangedEvent
// -----------------------------------------------------------------------------
//
CSwtLocationChangedEvent::CSwtLocationChangedEvent(TSwtPeer aPeer, TBool aDoIt,
        TBool aTop)
        : CSwtEvent(aPeer)
        , iDoIt(aDoIt)
        , iTop(aTop)
{
}

// -----------------------------------------------------------------------------
// CSwtLocationChangedEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CSwtLocationChangedEvent::ConstructL(const TDesC& aLocation)
{
    iLocation = aLocation.AllocL();
}

// -----------------------------------------------------------------------------
// CSwtLocationChangedEvent::NewL
// -----------------------------------------------------------------------------
//
CSwtLocationChangedEvent* CSwtLocationChangedEvent::NewL(TSwtPeer aPeer,
        TBool aDoIt, TBool aTop, const TDesC&  aLocation)
{
    CSwtLocationChangedEvent* self = new(ELeave) CSwtLocationChangedEvent(
        aPeer, aDoIt, aTop);
    CleanupStack::PushL(self);
    self->ConstructL(aLocation);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSwtLocationChangedEvent::~CSwtLocationChangedEvent
// -----------------------------------------------------------------------------
//
CSwtLocationChangedEvent::~CSwtLocationChangedEvent()
{
    delete iLocation;
}

// -----------------------------------------------------------------------------
// CSwtLocationChangedEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtLocationChangedEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    jstring location = NewJavaString(aJniEnv, *iLocation);

    DeleteInUiHeap(iLocation);
    iLocation = NULL;

    CallVoidJavaMethod(failed, aJniEnv, Peer(),
                       "internal_sendLocationChangedEvent", "(ZZLjava/lang/String;)V",
                       static_cast< jboolean >(iDoIt), static_cast< jboolean >(iTop),
                       location);
}

// -----------------------------------------------------------------------------
// CSwtProgressEvent::CSwtProgressEvent
// -----------------------------------------------------------------------------
//
CSwtProgressEvent::CSwtProgressEvent(TSwtPeer aPeer, TInt aCurrent,TInt aTotal)
        : CSwtEvent(aPeer)
        , iCurrent(aCurrent)
        , iTotal(aTotal)
{
}

// -----------------------------------------------------------------------------
// CSwtProgressEvent::~CSwtProgressEvent
// -----------------------------------------------------------------------------
//
CSwtProgressEvent::~CSwtProgressEvent()
{
}

// -----------------------------------------------------------------------------
// CSwtProgressEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtProgressEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendProgressEvent",
                       "(II)V", static_cast< jint >(iCurrent),static_cast< jint >(iTotal));
}

// -----------------------------------------------------------------------------
// CSwtProgressCompletedEvent::DoDispatch
// -----------------------------------------------------------------------------
//
CSwtProgressCompletedEvent::CSwtProgressCompletedEvent(TSwtPeer aPeer,
        TInt aCurrent, TInt aTotal)
        : CSwtEvent(aPeer)
        , iCurrent(aCurrent)
        , iTotal(aTotal)
{
}

// -----------------------------------------------------------------------------
// CSwtProgressCompletedEvent::~CSwtProgressCompletedEvent
// -----------------------------------------------------------------------------
//
CSwtProgressCompletedEvent::~CSwtProgressCompletedEvent()
{
}

// -----------------------------------------------------------------------------
// CSwtProgressCompletedEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtProgressCompletedEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(),
                       "internal_sendProgressCompletedEvent", "(II)V",
                       static_cast< jint >(iCurrent),static_cast< jint >(iTotal));
}

// -----------------------------------------------------------------------------
// CSwtStatusTextEvent::CSwtStatusTextEvent
// -----------------------------------------------------------------------------
//
CSwtStatusTextEvent::CSwtStatusTextEvent(TSwtPeer aPeer)
        : CSwtEvent(aPeer)
{
}

// -----------------------------------------------------------------------------
// CSwtStatusTextEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CSwtStatusTextEvent::ConstructL(const TDesC& aStatusText)
{
    iStatusText = aStatusText.AllocL();
}

// -----------------------------------------------------------------------------
// CSwtStatusTextEvent::NewL
// -----------------------------------------------------------------------------
//
CSwtStatusTextEvent* CSwtStatusTextEvent::NewL(TSwtPeer aPeer,
        const TDesC& aStatusText)
{
    CSwtStatusTextEvent* self = new(ELeave) CSwtStatusTextEvent(aPeer);
    CleanupStack::PushL(self);
    self->ConstructL(aStatusText);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSwtStatusTextEvent::~CSwtStatusTextEvent
// -----------------------------------------------------------------------------
//
CSwtStatusTextEvent::~CSwtStatusTextEvent()
{
    delete iStatusText;
}

// -----------------------------------------------------------------------------
// CSwtStatusTextEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtStatusTextEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    jstring statusText = NewJavaString(aJniEnv, *iStatusText);
    DeleteInUiHeap(iStatusText);
    iStatusText = NULL;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendStatusTextEvent",
                       "(Ljava/lang/String;)V", statusText);
}

// -----------------------------------------------------------------------------
// CSwtTitleEvent::CSwtTitleEvent
// -----------------------------------------------------------------------------
//
CSwtTitleEvent::CSwtTitleEvent(TSwtPeer aPeer)
        : CSwtEvent(aPeer)
{
}

// -----------------------------------------------------------------------------
// CSwtTitleEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CSwtTitleEvent::ConstructL(const TDesC& aTitle)
{
    iTitle = aTitle.AllocL();
}

// -----------------------------------------------------------------------------
// CSwtTitleEvent::NewL
// -----------------------------------------------------------------------------
//
CSwtTitleEvent* CSwtTitleEvent::NewL(TSwtPeer aPeer, const TDesC&  aTitle)
{

    CSwtTitleEvent* self = new(ELeave) CSwtTitleEvent(aPeer);
    CleanupStack::PushL(self);
    self->ConstructL(aTitle);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSwtTitleEvent::~CSwtTitleEvent
// -----------------------------------------------------------------------------
//
CSwtTitleEvent::~CSwtTitleEvent()
{
    delete iTitle;
}

// -----------------------------------------------------------------------------
// CSwtTitleEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtTitleEvent::DoDispatch(JNIEnv* aJniEnv)
{

    TBool failed;
    jstring title = NewJavaString(aJniEnv, *iTitle);
    DeleteInUiHeap(iTitle);
    iTitle = NULL;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendTitleEvent",
                       "(Ljava/lang/String;)V", title);
}

// -----------------------------------------------------------------------------
// CSwtDialogStringResultEvent::CSwtDialogStringResultEvent
// -----------------------------------------------------------------------------
//
CSwtDialogStringResultEvent::CSwtDialogStringResultEvent(CSwtDialogBroker* aBroker, TSwtPeer aPeer)
        : CSwtEvent(aPeer), iDialogBroker(aBroker)
{
}

// -----------------------------------------------------------------------------
// CSwtDialogStringResultEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CSwtDialogStringResultEvent::ConstructL(const TDesC& aDialogResult)
{
    iDialogResult = aDialogResult.AllocL();
}

// -----------------------------------------------------------------------------
// CSwtDialogStringResultEvent::NewL
// -----------------------------------------------------------------------------
//
CSwtDialogStringResultEvent* CSwtDialogStringResultEvent
::NewL(CSwtDialogBroker* aBroker, TSwtPeer aPeer, const TDesC&  aDialogResult)
{

    CSwtDialogStringResultEvent* self = new(ELeave) CSwtDialogStringResultEvent(aBroker, aPeer);
    CleanupStack::PushL(self);
    self->ConstructL(aDialogResult);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSwtDialogStringResultEvent::~CSwtDialogStringResultEvent
// -----------------------------------------------------------------------------
//
CSwtDialogStringResultEvent::~CSwtDialogStringResultEvent()
{
    delete iDialogBroker;
    delete iDialogResult;
}

// -----------------------------------------------------------------------------
// CSwtDialogStringResultEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtDialogStringResultEvent::Type() const
{
    return ESwtEventDialogStringResult;
}


// -----------------------------------------------------------------------------
// CSwtDialogStringResultEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtDialogStringResultEvent::DoDispatch(JNIEnv* aJniEnv)
{

    CSwtDisplay* display = CSwtDisplay::CurrentOrNull();
    ASSERT(display);
    // active object have to be deleted in UI thread
    CallMethod(display, &CSwtDisplay::RemoveDialogBroker, iDialogBroker);
    iDialogBroker = NULL;

    TBool failed;
    jstring string = NewJavaString(aJniEnv, *iDialogResult);
    DeleteInUiHeap(iDialogResult);
    iDialogResult = NULL;

    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendResult",
                       "(Ljava/lang/String;)V", string);
}


// -----------------------------------------------------------------------------
// CSwtDialogIntegerResultEvent::CSwtDialogIntegerResultEvent
// -----------------------------------------------------------------------------
//
CSwtDialogIntegerResultEvent::CSwtDialogIntegerResultEvent(CSwtDialogBroker* aBroker,
        TSwtPeer aPeer, TInt aDialogResult)
        : CSwtEvent(aPeer), iDialogBroker(aBroker)
        , iDialogResult(aDialogResult)
{
}

// -----------------------------------------------------------------------------
// CSwtDialogIntegerResultEvent::~CSwtDialogIntegerResultEvent
// -----------------------------------------------------------------------------
//
CSwtDialogIntegerResultEvent::~CSwtDialogIntegerResultEvent()
{
    delete iDialogBroker;
}

// -----------------------------------------------------------------------------
// CSwtDialogIntegerResultEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtDialogIntegerResultEvent::Type() const
{
    return ESwtEventDialogIntegerResult;
}


// -----------------------------------------------------------------------------
// CSwtDialogIntegerResultEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtDialogIntegerResultEvent::DoDispatch(JNIEnv* aJniEnv)
{

    CSwtDisplay* display = CSwtDisplay::CurrentOrNull();
    ASSERT(display);
    // active object have to be deleted in UI thread
    CallMethod(display, &CSwtDisplay::RemoveDialogBroker, iDialogBroker);
    iDialogBroker = NULL;

    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendResult",
                       "(I)V", static_cast< jint >(iDialogResult));
}
// -----------------------------------------------------------------------------
// CSwtDialogRgbResultEvent::CSwtDialogRgbResultEvent
// -----------------------------------------------------------------------------
//
CSwtDialogRgbResultEvent::CSwtDialogRgbResultEvent(CSwtDialogBroker* aBroker, TSwtPeer aPeer, TInt aRed, TInt aGreen, TInt aBlue)
        : CSwtEvent(aPeer)
        , iDialogBroker(aBroker)
        , iRed(aRed)
        , iGreen(aGreen)
        , iBlue(aBlue)
{
}

// -----------------------------------------------------------------------------
// CSwtDialogRgbResultEvent::~CSwtDialogRgbResultEvent
// -----------------------------------------------------------------------------
//
CSwtDialogRgbResultEvent::~CSwtDialogRgbResultEvent()
{
    delete iDialogBroker;
}

// -----------------------------------------------------------------------------
// CSwtDialogRgbResultEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtDialogRgbResultEvent::Type() const
{
    return ESwtEventDialogRgbResult;
}


// -----------------------------------------------------------------------------
// CSwtDialogRgbResultEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtDialogRgbResultEvent::DoDispatch(JNIEnv* aJniEnv)
{
    CSwtDisplay* display = CSwtDisplay::CurrentOrNull();
    ASSERT(display);
    // active object have to be deleted in UI thread
    CallMethod(display, &CSwtDisplay::RemoveDialogBroker, iDialogBroker);
    iDialogBroker = NULL;

    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendResult",
                       "(III)V", static_cast< jint >(iRed),
                       static_cast< jint >(iGreen),
                       static_cast< jint >(iBlue));
}



// -----------------------------------------------------------------------------
// CSwtSettingsEvent::CSwtSettingsEvent
// -----------------------------------------------------------------------------
//
CSwtSettingsEvent::CSwtSettingsEvent(TSwtPeer aPeer, TBool aThemeChanged)
        :CSwtEvent(aPeer)
        ,iThemeChanged(aThemeChanged)
{
}

TSwtEventType CSwtSettingsEvent::Type() const
{
    return ESwtEventSettings;
}

void CSwtSettingsEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "runSettings",
                       "(Z)V",  static_cast< jboolean >(iThemeChanged));
}


// -----------------------------------------------------------------------------
// CSwtCloseEvent::CSwtCloseEvent
// -----------------------------------------------------------------------------
//
CSwtCloseEvent::CSwtCloseEvent(TSwtPeer aPeer, TBool& aDispatched)
        : CSwtCallbackEvent(aPeer), iDispatched(aDispatched)
{
}


// -----------------------------------------------------------------------------
// CSwtCloseEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtCloseEvent::Type() const
{
    return ESwtEventClose;
}

// -----------------------------------------------------------------------------
// CSwtCloseEvent::SendEvent
// -----------------------------------------------------------------------------
//
TBool CSwtCloseEvent::SendEvent(JNIEnv* aJniEnv, jboolean &aDoIt)
{
    TBool failed;
    aDoIt = CallBooleanJavaMethod(failed, aJniEnv, Peer(),
                                  "sendCloseEvent", "()Z");

    TBool fine = !failed && !aJniEnv->ExceptionCheck();
    if (fine && aDoIt)
    {
        // CallbackL needs the display. Get it here as we are in the Java ui thread.
        iDisplay = CSwtDisplay::CurrentOrNull();
    }

    return fine;
}

// -----------------------------------------------------------------------------
// CSwtCloseEvent::SendEvent
// -----------------------------------------------------------------------------
//
void CSwtCloseEvent::CallbackL(TBool aDoIt)
{
    iDispatched = ETrue;
    if (aDoIt)
    {
        if (iDisplay)
        {
            iDisplay->OfferWsEventL(SwtWsEventShutdown);
        }
    }
}

// -----------------------------------------------------------------------------
// CSwtTextSelectionEvent::CSwtTextSelectionEvent
// -----------------------------------------------------------------------------
//
CSwtTextSelectionEvent::CSwtTextSelectionEvent(TSwtPeer aPeer)
        : CSwtEvent(aPeer)
{
}

// -----------------------------------------------------------------------------
// CSwtTextSelectionEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CSwtTextSelectionEvent::ConstructL(const TDesC& aSelectedText)
{
    iSelectedText = aSelectedText.AllocL();
}

// -----------------------------------------------------------------------------
// CSwtTextSelectionEvent::NewL
// -----------------------------------------------------------------------------
//
CSwtTextSelectionEvent* CSwtTextSelectionEvent::NewL(TSwtPeer aPeer,
        const TDesC& aSelectedText)
{
    CSwtTextSelectionEvent* self = new(ELeave) CSwtTextSelectionEvent(aPeer);
    CleanupStack::PushL(self);
    self->ConstructL(aSelectedText);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSwtTextSelectionEvent::~CSwtTextSelectionEvent
// -----------------------------------------------------------------------------
//
CSwtTextSelectionEvent::~CSwtTextSelectionEvent()
{
    delete iSelectedText;
}

// -----------------------------------------------------------------------------
// CSwtTextSelectionEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtTextSelectionEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    jstring selectedText = NewJavaString(aJniEnv, *iSelectedText);
    DeleteInUiHeap(iSelectedText);
    iSelectedText = NULL;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "internal_sendEvent",
                       "(ILjava/lang/String;)V", static_cast<jint>(Type()), selectedText);
}

// -----------------------------------------------------------------------------
// CSwtShowFocusedControlEvent::CSwtShowFocusedControlEvent
// -----------------------------------------------------------------------------
//
CSwtShowFocusedControlEvent::CSwtShowFocusedControlEvent(TSwtPeer aPeer)
        : CSwtEvent(aPeer)
{
}

// -----------------------------------------------------------------------------
// CSwtShowFocusedControlEvent::Type
// -----------------------------------------------------------------------------
//
TSwtEventType CSwtShowFocusedControlEvent::Type() const
{
    return ESwtEventShowFocusedControl;
}

// -----------------------------------------------------------------------------
// CSwtShowFocusedControlEvent::DoDispatch
// -----------------------------------------------------------------------------
//
void CSwtShowFocusedControlEvent::DoDispatch(JNIEnv* aJniEnv)
{
    TBool failed;
    CallVoidJavaMethod(failed, aJniEnv, Peer(), "handleShowFocusedControlEvent", "()V");
}
