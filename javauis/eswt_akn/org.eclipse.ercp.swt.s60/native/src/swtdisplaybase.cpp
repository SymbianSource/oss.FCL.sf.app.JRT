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


#include <eikbtgpc.h>
#include <fbs.h>
#include <gulutil.h>
#include <eikon.hrh>
#include <AknUtils.h>
#include <bautils.h>
#include <swtlaffacade.h>
#include "s60commonutils.h"
#include "swtfactory.h"
#include "swtdisplaybase.h"
#include "eswtwidgetsexpanded.h"
#include "eswtmobileextensions.h"
#include "swtevents.h"
#include "swtkeymapper.h"
#include "swttimer.h"
#include "swtimage.h"
#include "swtmidremconobserver.h"
#include "swtjavabufferdrawer.h"
#include "swtfont.h"
#include "swtuiutils.h"


#define ASSERT_JAVAUITHREAD() ASSERT(IsCurrentThreadJavaUi())
#define ASSERT_NATIVEUITHREAD() ASSERT(IsCurrentThreadNativeUi())


// Assuming ResourceLanguageFileNameL inserts DRIVE:/KDC_RESOURCE_FILES_DIR/java/
// Assuming KDC_RESOURCE_FILES_DIR = /resource/java/
_LIT(KSwtResFile, "eswtcore.rsc");


// ======== MEMBER FUNCTIONS ========


/**
 * 2nd phase constructor
 */
void ASwtDisplayBase::ConstructInJavaUiThreadL()
{
    iEventQueue = CSwtEventQueue::NewL(*this);
}

/**
 * UI thread's side constructor
 */
void ASwtDisplayBase::ConstructInNativeUiThreadL()
{
    ASSERT_NATIVEUITHREAD();

    iCoeEnv = CEikonEnv::Static(); // codescanner::eikonenvstatic
    ASSERT(iCoeEnv);
    iFactory = CSwtFactory::NewL();
    iResId = LoadResourceFileL();
    iUiUtils = iFactory->NewUiUtilsL(*this);
    iMenuArranger = iFactory->NewMenuArrangerL(*this);
    iCommandArranger = iFactory->NewCommandArrangerL(*this);
    iKeyMapper = CSwtKeyMapper::NewL();
    iRemConObserver = CSwtMIDRemConObserver::NewL();
    if (iRemConObserver)
    {
        iRemConObserver->AddMediaKeysListenerL(this);
    }
    iJavaBufferDrawer = new(ELeave) CSwtJavaBufferDrawer(*this);
#ifdef RD_SCALABLE_UI_V2
    iLongTapDetector = CAknLongTapDetector::NewL(this);
#endif
}


/**
 * Destructor
 */
ASwtDisplayBase::~ASwtDisplayBase()
{
}


/**
 * Destroys the Display part that runs in the UI thread
 */
void ASwtDisplayBase::DestroyInNativeUiThread()
{
    ASSERT_NATIVEUITHREAD();

    if (iRemConObserver)
    {
        iRemConObserver->RemoveMediaKeysListener(this);
    }
    delete iRemConObserver;
    iRemConObserver = NULL;

    if (iSystemFont)
    {
        iSystemFont->RemoveRef();
        iSystemFont = NULL;
    }

    delete iKeyMapper;
    iKeyMapper = NULL;

    delete iCommandArranger;  // Mobile Extensions
    iCommandArranger = NULL;

    delete iMenuArranger;
    iMenuArranger = NULL;

    delete iUiUtils;          // Core
    iUiUtils = NULL;

    delete iFactory;
    iFactory = NULL;

    iTimers.ResetAndDestroy();

    if (iEventQueue)
        iEventQueue->Flush();

    iResourceChangeObservers.Close();
    iAppFocusObservers.Close();

    delete iJavaBufferDrawer;
    iJavaBufferDrawer = NULL;

#ifdef RD_SCALABLE_UI_V2
    delete iLongTapDetector;
    iLongTapDetector = NULL;
#endif //RD_SCALABLE_UI_V2   

    if (iResId != KErrNotFound)
    {
        iCoeEnv->DeleteResourceFile(iResId);
        iResId = KErrNotFound;
    }

    iCoeEnv = NULL;
}


/**
 * Sleeps until an event is received
 *
 * Must be executed in the main thread.
 *
 * @return <code>ETrue</code> if the reason for awakening is reception of an
 *         event; <code>EFalse</code> otherwise.
 */
TBool ASwtDisplayBase::Sleep()
{
    ASSERT_JAVAUITHREAD();

    // Must be set before checking if the queue is empty so that if an event
    // arrives between the moment we checked the queue and when we call
    // WaitForRequest(), we won't go to sleep.
    iRequestStatus = KRequestPending;

    if (!iEventQueue->IsEmpty()) //lint !e613
        return ETrue;

    User::WaitForRequest(iRequestStatus);
    return ETrue;
}

/**
 * Creates a new timer that will auto-destruct. It will also register with
 * <code>RegisterTimerL()</code>, in case we need to exit and the timer is
 * still running.
 */
void ASwtDisplayBase::AddTimerL(TInt aDelayInMilliSeconds, TInt aTimerHandle)
{
    CSwtTimer* timer = CSwtTimer::NewL(*this);
    timer->ExecuteAfterLD(aDelayInMilliSeconds, aTimerHandle);
}


/**
 * Adds a new timer to the java side timer list
 */
void ASwtDisplayBase::RegisterTimerL(const CSwtTimer* aTimer)
{
    UnRegisterTimer(aTimer); //prevents duplicating timers
    User::LeaveIfError(iTimers.Append(aTimer));
}


/**
 * Removes the timer from the java side timer list
 */
void ASwtDisplayBase::UnRegisterTimer(const CSwtTimer* aTimer)
{
    TInt pos = iTimers.Find(aTimer);
    if (pos >= 0)
    {
        iTimers.Remove(pos);
    }
}


/**
 * Awakens the display because of reception of a new event.
 *
 * Must be executed from within the App UI's thread.
 */
void ASwtDisplayBase::HandleNewEvent()
{
    ASSERT_NATIVEUITHREAD();
    DoWake(ETrue);
}


/**
 * Awakens the display (i.e. the main thread)
 */
void ASwtDisplayBase::DoWake(TBool aBecauseOfEvent)
{
    if (iRequestStatus == KRequestPending)
    {
        TRequestStatus* statusPtr = &iRequestStatus;
        iJavaUiThread.RequestComplete(statusPtr, aBecauseOfEvent);
    }
}


/**
 * Returns the maximum allowed depth of icons
 *
 * Must be executed from within the App UI's thread.
 */
TInt ASwtDisplayBase::GetIconDepth() const
{
    ASSERT_NATIVEUITHREAD();
    return TDisplayModeUtils::NumDisplayModeBitsPerPixel(iCoeEnv->ScreenDevice()->DisplayMode());
}


/**
 * Returns the double click time
 *
 * Must be executed from within the App UI's thread.
 */
TInt ASwtDisplayBase::GetDoubleClickTime() const
{
    ASSERT_NATIVEUITHREAD();
    TTimeIntervalMicroSeconds32 interval(0);
    TInt distance(0);
    iCoeEnv->WsSession().GetDoubleClickSettings(interval, distance);
    return interval.Int();
}


/**
 * Emits a beep
 *
 * Must be executed from within the App UI's thread.
 */
void ASwtDisplayBase::Beep() const
{
    ASSERT_NATIVEUITHREAD();
    iCoeEnv->Beep();
}


/**
 * Posts an asynchronous event.
 *
 * @param aData  SWT data of the key event.
 *
 * This method should be able to post events of type KeyDown or KeyUp.
 */
void ASwtDisplayBase::PostL(const TSwtKeyEventData& aData)
{
    ASSERT_NATIVEUITHREAD();

    if ((aData.iType == ESwtEventKeyDown) || (aData.iType == ESwtEventKeyUp))
    {
        MSwtShell* shell = iUiUtils->GetActiveShell();
        if (shell == NULL)
        {
            return;
        }
        MSwtControl* control = shell->FocusControl();
        if (control == NULL)
        {
            return;
        }

        TKeyEvent keyEvent;
        keyEvent.iCode      = 0;
        keyEvent.iRepeats   = 0;
        keyEvent.iModifiers = iKeyMapper->GetSymbianModifier(aData);

        if (aData.iType == ESwtEventKeyUp)
        {
            // Key up event: keyEvent.iCode is set to 0
            keyEvent.iScanCode  = iKeyMapper->GetSymbianScanCode(aData.iKeyCode);
            control->CoeControl().OfferKeyEventL(keyEvent, EEventKeyUp);
        }
        else
        {
            // Key down event in SWT = key down event follows with key event in Symbian
            // If several SWT ESwtEventKeyDown are following,
            // we should post first one Symbian EEventKeyDown
            // and then as many Symbian EEventKey as SWT ESwtEventKeyDown.
            // But posting EEventKeyDown and EEventKey at each time should not cause any problem.

            // Key down event: keyEvent.iCode is set to 0
            TUint code = aData.iCharacter;
            iKeyMapper->GetSymbianCodes(aData.iKeyCode, keyEvent.iScanCode, &code);
            control->CoeControl().OfferKeyEventL(keyEvent, EEventKeyDown);

            // Key event: keyEvent.iCode is not 0.
            keyEvent.iCode = code;
            control->CoeControl().OfferKeyEventL(keyEvent, EEventKey);
        }
    }
}

/**
 * Posts an asynchronous event.
 * @param aData  SWT data of the mouse event.
 * This method should be able to post events of type MouseDown, MouseUp, MouseMove.
 */
TBool ASwtDisplayBase::MousePostL(const TSwtKeyEventData& aData, TPoint point)
{
    ASSERT_NATIVEUITHREAD();

    if (AknLayoutUtils::PenEnabled())
    {
        TRawEvent event;
        if (aData.iType == ESwtEventMouseDown)
        {
            event.Set(TRawEvent::EButton1Down, point.iX, point.iY);
            iCoeEnv->WsSession().SimulateRawEvent(event);
        }
        else if (aData.iType == ESwtEventMouseUp)
        {
            event.Set(TRawEvent::EButton1Up, point.iX, point.iY);
            iCoeEnv->WsSession().SimulateRawEvent(event);
        }
        else if (aData.iType == ESwtEventMouseMove)
        {
            event.Set(TRawEvent::EPointerMove, point.iX, point.iY);
            iCoeEnv->WsSession().SimulateRawEvent(event);
        }
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}

/**
 * Constructs an uninitialised Image
 *
 * @param aSize  The new image's size
 */
MSwtImage* ASwtDisplayBase::NewImageL(const TSize& aSize)
{
    return CSwtImage::NewL(*this, aSize, *this);
}


/**
 * Constructs an Image with initial data
 *
 * @param aData  The data to construct the image from
 */
MSwtImage* ASwtDisplayBase::NewImageFromDataL(const MSwtImageData& aData)
{
    return CSwtImage::NewL(this, aData, this);
}


//
// Own internal event methods
//

void ASwtDisplayBase::PostForegroundEventL(TSwtPeer aPeer, const TBool& aForeground)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtForegroundEvent(aPeer, aForeground)); //lint !e613
}


//
// Virtual methods from MSwtDisplay
//

MSwtDevice& ASwtDisplayBase::Device()
{
    return *this;
}

const MSwtFactory& ASwtDisplayBase::Factory() const
{
    ASSERT(iFactory);
    return *iFactory;
}

MSwtUiUtils& ASwtDisplayBase::UiUtils()
{
    ASSERT(iUiUtils!=NULL);
    return *iUiUtils;         //lint !e613
}

MSwtMenuArranger& ASwtDisplayBase::MenuArranger()
{
    ASSERT(iMenuArranger!=NULL);
    return *iMenuArranger;
}

MSwtCommandArranger* ASwtDisplayBase::CommandArranger()
{
    return iCommandArranger;
}

void ASwtDisplayBase::AddResourceChangeObserverL(MSwtResourceChangeObserver* aObserver)
{
    User::LeaveIfError(iResourceChangeObservers.Append(aObserver));
}

void ASwtDisplayBase::RemoveResourceChangeObserver(MSwtResourceChangeObserver* aObserver)
{
    TInt pos = iResourceChangeObservers.Find(aObserver);
    if (pos != KErrNotFound)
    {
        iResourceChangeObservers.Remove(pos);
    }
}

void ASwtDisplayBase::AddAppFocusObserverL(MSwtAppFocusObserver* aObserver)
{
    User::LeaveIfError(iAppFocusObservers.Append(aObserver));
}

void ASwtDisplayBase::RemoveAppFocusObserver(MSwtAppFocusObserver* aObserver)
{
    TInt pos = iAppFocusObservers.Find(aObserver);
    if (pos != KErrNotFound)
    {
        iAppFocusObservers.Remove(pos);
    }
}

TInt ASwtDisplayBase::GetSymbianScanCode(const TInt aSwtKeyCode) const
{
    return CSwtKeyMapper::GetSymbianScanCode(aSwtKeyCode);
}

TUint ASwtDisplayBase::GetSymbianKeyCode(const TInt aSwtKeyCode) const
{
    TUint keyCode = aSwtKeyCode;
    TInt scanCodeNotNeeded;
    CSwtKeyMapper::GetSymbianCodes(aSwtKeyCode, scanCodeNotNeeded, &keyCode);
    return keyCode;
}

void ASwtDisplayBase::ForceTraverseEventL(MSwtControl& aControl, TSwtTraversal aDetail, TBool aDoIt)
{
    ASSERT_NATIVEUITHREAD();

    TSwtKeyEventData data;
    data.iType      = ESwtEventTraverse;
    data.iCharacter = 0;
    data.iKeyCode   = 0;
    data.iStateMask = 0;

    TKeyEvent keyEvent;
    keyEvent.iCode      = 0;
    keyEvent.iModifiers = 0;
    keyEvent.iRepeats   = 0;
    keyEvent.iScanCode  = 0;

    iEventQueue->PushL(new(ELeave) CSwtTraverseEvent(aControl, aDetail, aDoIt, data, keyEvent, EEventKey)); //lint !e613
}


void ASwtDisplayBase::PostTraverseEventL(MSwtControl& aControl, TSwtTraversal aDetail, const TKeyEvent& aKeyEvent,
        TEventCode aType, TBool aDoIt)
{
    ASSERT_NATIVEUITHREAD();
    ASSERT(iKeyMapper!=NULL);

    TSwtKeyEventData data(iKeyMapper->MapKeyEventL(aKeyEvent, aType)); //lint !e613
    if (data.iType != ESwtEventNone)
        iEventQueue->PushL(new(ELeave) CSwtTraverseEvent(aControl, aDetail, aDoIt, data, aKeyEvent, aType)); //lint !e613
}


void ASwtDisplayBase::PostKeyEventL(MSwtControl& aControl, const TKeyEvent& aKeyEvent, TEventCode aType)
{
    ASSERT_NATIVEUITHREAD();
    ASSERT(iKeyMapper!=NULL);
    ASSERT(aType!=EEventNull);

    // EEventKeyDown can be posted only if the key does not generate EEventKey!
    TSwtKeyEventData data(iKeyMapper->MapKeyEventL(aKeyEvent, aType)); //lint !e613
    if (data.iType != ESwtEventNone)
        iEventQueue->PushL(new(ELeave) CSwtKeyEvent(aControl, data, aKeyEvent, aType)); //lint !e613
}


void ASwtDisplayBase::PostPaintEventL(TSwtPeer aSenderPeer, TSwtPeer aShellPeer, const TRect& aRect, TBool aMergeable)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPaintEvent(aSenderPeer, aShellPeer, aRect, aMergeable)); //lint !e613
}


void ASwtDisplayBase::PostMouseEventL(TSwtPeer aPeer, TSwtEventType aType, TInt aButton, const TPoint& aPos, TInt aStateMask)
{
    ASSERT_NATIVEUITHREAD();
    ASSERT(aType>=ESwtEventMouseDown && aType<=ESwtEventMouseDoubleClick);
    ASSERT(aButton==0 || (aType==ESwtEventMouseDown || aType==ESwtEventMouseUp));

#ifdef RD_SCALABLE_UI_V2
    // After the long tap we send mouse up event for button2, see also HandleLongTapEventL().
    if (iLongTapDetected && aType == ESwtEventMouseUp && aButton == KSwtMouseButton1)
    {
        aButton = KSwtMouseButton2;
    }
#endif //RD_SCALABLE_UI_V2        

    iEventQueue->PushL(new(ELeave) CSwtMouseEvent(aPeer, aType, aButton, aPos, aStateMask)); //lint !e613
}

void ASwtDisplayBase::PostMoveEventL(TSwtPeer aPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, ESwtEventMove)); //lint !e613
}

void ASwtDisplayBase::PostResizeEventL(TSwtPeer aPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtResizeEvent(aPeer)); //lint !e613
}

void ASwtDisplayBase::PostScrollEventL(TSwtPeer aPeer, TInt aDetail)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtScrollEvent(aPeer, aDetail)); //lint !e613
}

void ASwtDisplayBase::PostSelectionEventL(TSwtPeer aPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, ESwtEventSelection)); //lint !e613
}

void ASwtDisplayBase::PostSelectionEventL(TSwtPeer aPeer, const TDesC& aText)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(CSwtTextSelectionEvent::NewL(aPeer, aText));  //lint !e613
}

void ASwtDisplayBase::PostSelectionEventL(TSwtPeer aPeer, TInt aDetail, TSwtPeer aItemPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, ESwtEventSelection, aDetail, aItemPeer)); //lint !e613
}

void ASwtDisplayBase::PostDefaultSelectionEventL(TSwtPeer aPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, ESwtEventDefaultSelection,KSwtNone,NULL)); //lint !e613
}

void ASwtDisplayBase::PostDefaultSelectionEventL(TSwtPeer aPeer, TInt aDetail, TSwtPeer aItemPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, ESwtEventDefaultSelection, aDetail, aItemPeer)); //lint !e613
}

void ASwtDisplayBase::PostFocusEventL(TSwtPeer aPeer, TSwtEventType aType)
{
    ASSERT_NATIVEUITHREAD();
    ASSERT(aType==ESwtEventFocusIn || aType==ESwtEventFocusOut);
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, aType)); //lint !e613
}

void ASwtDisplayBase::PostShellEventL(TSwtPeer aPeer, TSwtEventType aType)
{
    ASSERT_NATIVEUITHREAD();
    ASSERT(aType==ESwtEventActivate || aType==ESwtEventDeactivate);
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, aType)); //lint !e613
}

void ASwtDisplayBase::PostModifyEventL(TSwtPeer aPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, ESwtEventModify, KSwtNone, NULL)); //lint !e613
}

void ASwtDisplayBase::PostVerifyEventL(MSwtVerifyEventObserver& aWidget, TSwtPeer aPeer, TInt aStart, TInt aEnd, const TDesC& aText)
{
    ASSERT_NATIVEUITHREAD();
    ASSERT(aStart>=0 && aStart<=aEnd);
    iEventQueue->PushL(CSwtVerifyEvent::NewL(aWidget, aPeer, aStart, aEnd, aText)); //lint !e613
}

void ASwtDisplayBase::PostShowEventL(TSwtPeer aPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, ESwtEventShow)); //lint !e613
}

void ASwtDisplayBase::PostHideEventL(TSwtPeer aPeer)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtPlainEvent(aPeer, ESwtEventHide)); //lint !e613
}

void ASwtDisplayBase::PostScreenEventL(TSwtPeer aPeer, TInt aType)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtScreenEvent(aPeer, aType)); //lint !e613
}

void ASwtDisplayBase::PostMobileDeviceEventL(TSwtPeer aPeer, TInt aEventType)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtMobileDeviceEvent(aPeer, aEventType)); //lint !e613
}

void ASwtDisplayBase::PostTreeEventL(TSwtPeer aPeer, TSwtEventType aType, TInt aItemHandle)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtTreeEvent(aPeer, aType, aItemHandle)); //lint !e613
}

void ASwtDisplayBase::PostLocationChangingEventL(TSwtPeer aPeer
        , MSwtBrowser& aBrowser, TBool aDoIt, TBool aTop
        , const TDesC& aLocation, TSwtBrCallBackOperationType aCallBackOperationType)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(CSwtLocationChangingEvent::NewL(aPeer, aBrowser, aDoIt, aTop
                       , aLocation, aCallBackOperationType));
}

void ASwtDisplayBase::PostLocationChangedEventL(TSwtPeer aPeer,
        TBool aDoIt, TBool aTop, const TDesC& aLocation)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(CSwtLocationChangedEvent::NewL(aPeer, aDoIt, aTop, aLocation));
}

void ASwtDisplayBase::PostProgressEventL(TSwtPeer aPeer, TInt aCurrent, TInt aTotal)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtProgressEvent(aPeer, aCurrent, aTotal));
}

void ASwtDisplayBase::PostProgressCompletedEventL(TSwtPeer aPeer, TInt aCurrent, TInt aTotal)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtProgressCompletedEvent(aPeer, aCurrent, aTotal));
}

void ASwtDisplayBase::PostStatusTextEventL(TSwtPeer aPeer, const TDesC& aStatusText)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(CSwtStatusTextEvent::NewL(aPeer, aStatusText));
}

void ASwtDisplayBase::PostTitleEventL(TSwtPeer aPeer, const TDesC& aTitle)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(CSwtTitleEvent::NewL(aPeer, aTitle));
}

void ASwtDisplayBase::PostDialogResultEventL(CSwtDialogBroker* aBroker, TSwtPeer aPeer, const TDesC& aDialogResult)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(CSwtDialogStringResultEvent::NewL(aBroker, aPeer, aDialogResult));
}

void ASwtDisplayBase::PostDialogResultEventL(CSwtDialogBroker* aBroker, TSwtPeer aPeer, TInt aDialogResult)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtDialogIntegerResultEvent(aBroker, aPeer, aDialogResult));
}

void ASwtDisplayBase::PostDialogResultEventL(CSwtDialogBroker* aBroker, TSwtPeer aPeer,  TInt aDialogResult1, TInt aDialogResult2, TInt aDialogResult3)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtDialogRgbResultEvent(aBroker, aPeer, aDialogResult1, aDialogResult2, aDialogResult3));
}

void ASwtDisplayBase::PostCloseEventL(TSwtPeer aPeer, TBool& aDispatched)
{
    ASSERT_NATIVEUITHREAD();
    iEventQueue->PushL(new(ELeave) CSwtCloseEvent(aPeer, aDispatched));
}

TInt ASwtDisplayBase::ApplicationUid()
{
    return iApplicationUid;
}

CEikonEnv* ASwtDisplayBase::CoeEnv() const
{
    ASSERT(iCoeEnv);
    return iCoeEnv;
}

CDesC16ArrayFlat* ASwtDisplayBase::GetFontNamesL(TBool aScalable) const
{
    ASSERT_NATIVEUITHREAD();

    CEikonEnv* eikEnv = iCoeEnv;
    CWsScreenDevice* screenDevice = NULL;
    if (eikEnv)
    {
        screenDevice = eikEnv->ScreenDevice();
    }
    if (!eikEnv || !screenDevice)
    {
        return NULL;
    }

    // Look for available fonts
    const TInt KArrayGranularity = 4;
    //Caller method is responsible for deleting the fontNameList
    CDesC16ArrayFlat* fontNameList = new(ELeave) CDesC16ArrayFlat(KArrayGranularity);
    CleanupStack::PushL(fontNameList);
    FontUtils::GetAvailableFontsL(*screenDevice, *fontNameList, EGulAllFonts);

    TFontSpec fontSpec;
    CFont* font = NULL;
    TInt  fontNameListCount = fontNameList->Count();
    for (TInt nameIndex=0; nameIndex<fontNameListCount; ++nameIndex)
    {
        // Look for the nearest font from the current typeface name.
        // This is needed only to check if the font correspond to the
        // aScalable parameter
        fontSpec.iTypeface.iName = (*fontNameList)[nameIndex];
        fontSpec.iHeight = 0;
        fontSpec.iFontStyle = TFontStyle(EPostureUpright, EStrokeWeightNormal, EPrintPosNormal);
        User::LeaveIfError(screenDevice->GetNearestFontInTwips(font, fontSpec) == KErrNone);

        // Manage scalable attribute
        // CFont::TypeUid() returns the UId of the type of the font:
        //  - KCFbsFontUid for a CFbsFont
        //  - KCBitmapFontUid or KCBitmapFontUidVal for a CBitmapFont
        // On Series60 SDK, no Uid  is available for CBitmapFont.
        // So scalable management is processed only for CFbsFont.
        // For the other font types, the font is always added to the list.
        if (font->TypeUid() == KCFbsFontUid)
        {
            if (aScalable !=(static_cast<CFbsFont*>(font))->IsOpenFont())
            {
                // Delete the font because its scalable attribute
                // doesn't correspond to aScalable parameter.
                fontNameList->Delete(nameIndex);
                fontNameList->Compress();
                fontNameListCount--;
                nameIndex--;
            }
        }
#ifdef _DEBUG
        else
        {
            // Non-recognized font type Uid.
            __DEBUGGER();
        }
#endif
        screenDevice->ReleaseFont(font);
        font = NULL;
    }

    CleanupStack::Pop(fontNameList);
    return fontNameList;
}

TBool ASwtDisplayBase::RevertPointerEvent() const
{
    return iRevertPointerEvent || iLongTapDetected;
}

void ASwtDisplayBase::SetRevertPointerEvent(TBool aStatus) 
{
    iRevertPointerEvent = aStatus;
}

//
// Virtual methods from MSwtDevice
//

MSwtColor* ASwtDisplayBase::CreateColorL(const TRgb& aRgbValue) const
{
    return iFactory->NewColorL(*this, aRgbValue); //lint !e613
}

TRect ASwtDisplayBase::Bounds() const
{
    ASSERT_NATIVEUITHREAD();
    return TRect(iCoeEnv->EikAppUi()->ApplicationRect());
}

TRect ASwtDisplayBase::ClientArea() const
{
    ASSERT_NATIVEUITHREAD();
    TRect appUiCltRect(iCoeEnv->EikAppUi()->ApplicationRect());
    iUiUtils->Cba().ReduceRect(appUiCltRect);
    return appUiCltRect;
}

TInt ASwtDisplayBase::Depth() const
{
    ASSERT_NATIVEUITHREAD();
    switch (iCoeEnv->ScreenDevice()->DisplayMode())
    {
    case EGray2:
        return 1;
    case EGray4:
        return 2;
    case EGray16:
        return 4;
    case EGray256:
        return 8;
    case EColor16:
        return 4;
    case EColor256:
        return 8;
    case EColor64K:
        return 16;
    case EColor16M:
        return 24;
    case EColor16MU:
        return 32;
    case EColor16MA:
        return 32;
    case EColor16MAP:
        return 32;
    case EColor4K:
        return 12;
    default:
        ASSERT(EFalse);
        return 0;
    }
}

TSize ASwtDisplayBase::Dpi() const
{
    ASSERT_NATIVEUITHREAD();

    // Convert a 1440x1440 twips (i.e. 1x1 inch) square to pixels, this yelds the
    // DPI resolution.
    return iCoeEnv->ScreenDevice()->TwipsToPixels(TPoint(KTwipsPerInch,KTwipsPerInch)).AsSize();
}

static TInt getTheHeightInTWipsL(CEikonEnv* aEnv, const MSwtDevice& aDevice,
                                 const TFontSpec& aFontSpec, TInt aIndexInArray, CArrayFixFlat<TInt>* aArrayHeight)
{
    TFontSpec fontSpec(aFontSpec);
    CFont*    iFont                = NULL;
    TInt      heightInTWips        = aArrayHeight->At(aIndexInArray);
    TInt      heightOfObtainFontPt = 0;
    TInt      heightOfAskedFontPt  = FontUtils::PointsFromTwips(heightInTWips);
    TInt      maxHeightPt          = FontUtils::PointsFromTwips(aArrayHeight->At(aArrayHeight->Count()-1));

    CBitmapDevice& bmpDevice = *(aEnv->ScreenDevice());

    // Look for available heights for the current font.
    fontSpec.iHeight = FontUtils::TwipsFromPoints(heightOfAskedFontPt);
    User::LeaveIfError(bmpDevice.GetNearestFontInTwips(iFont, fontSpec));
    heightOfObtainFontPt = FontUtils::PointsFromTwips(iFont->FontSpecInTwips().iHeight);

    while (((heightInTWips != iFont->FontSpecInTwips().iHeight)
            && (heightOfAskedFontPt < maxHeightPt))
            && (heightOfAskedFontPt >  heightOfObtainFontPt))
    {
        heightOfAskedFontPt++;
        fontSpec.iHeight = FontUtils::TwipsFromPoints(heightOfAskedFontPt);

        const_cast<MSwtDevice&>(aDevice).GraphicsDevice().ReleaseFont(iFont);
        heightOfObtainFontPt = FontUtils::PointsFromTwips(iFont->FontSpecInTwips().iHeight);
        User::LeaveIfError(bmpDevice.GetNearestFontInTwips(iFont, fontSpec));
    }

    const_cast<MSwtDevice&>(aDevice).GraphicsDevice().ReleaseFont(iFont);
    return fontSpec.iHeight;
}

CArrayFixFlat<TSwtFontData>* ASwtDisplayBase::GetFontListL(const TDesC& aFaceName, TBool aScalable) const
{
    ASSERT_NATIVEUITHREAD();

    CEikonEnv& eikEnv = *iCoeEnv;

    // Look for available fonts
    const TInt        KArrayGranularity = 4;
    CDesC16ArrayFlat* fontNameList = new(ELeave) CDesC16ArrayFlat(KArrayGranularity);
    CleanupStack::PushL(fontNameList);
    if (aFaceName.Length() == 0)
    {
        FontUtils::GetAvailableFontsL(*(eikEnv.ScreenDevice()), *fontNameList, EGulAllFonts);
    }
    else
    {
        fontNameList->AppendL(aFaceName);
    }

    CArrayFixFlat<TSwtFontData>* arrayFontData = new(ELeave) CArrayFixFlat<TSwtFontData>(KArrayGranularity);
    CleanupStack::PushL(arrayFontData);

    CArrayFixFlat<TInt>* heightList = new(ELeave)CArrayFixFlat<TInt>(KArrayGranularity);
    CleanupStack::PushL(heightList);

    TFontSpec    fontSpec;
    TSwtFontData fontData;
    CFont*       font = NULL;
    TInt  heightIndex;
    TInt  fontNameListCount = fontNameList->Count();
    TInt  heightListCount;
    for (TInt nameIndex=0; nameIndex<fontNameListCount; ++nameIndex)
    {
        // Look for the nearest font from the current typeface name.
        fontSpec.iTypeface.iName = (*fontNameList)[nameIndex];
        fontSpec.iHeight = 0;
        fontSpec.iFontStyle = TFontStyle(EPostureUpright, EStrokeWeightNormal, EPrintPosNormal);
        User::LeaveIfError(eikEnv.ScreenDevice()->GetNearestFontInTwips(font, fontSpec) == KErrNone);

        // Manage scalable attribute
        // CFont::TypeUid() returns the UId of the type of the font:
        //  - KCFbsFontUid for a CFbsFont
        //  - KCBitmapFontUid or KCBitmapFontUidVal for a CBitmapFont
        // On Series60 SDK, no Uid  is available for CBitmapFont.
        // So scalable management is processed only for CFbsFont.
        // For the other font types, the font is always added to the list.
        if (font->TypeUid() == KCFbsFontUid)
        {
            if (aScalable != (static_cast<CFbsFont*>(font))->IsOpenFont())
            {
                // Ignore the font because its scalable attribute
                // doesn't correspond to aScalable parameter.
                continue;
            }
        }
#ifdef _DEBUG
        else
        {
            // Non-recognized font type Uid.
            __DEBUGGER();
        }
#endif


        fontSpec = font->FontSpecInTwips();

        // Look for available heights for the current font
        if (FontUtils::GetAvailableHeightsInTwipsL(*(eikEnv.ScreenDevice()),
                fontSpec.iTypeface.iName, *heightList) == KErrNone)
        {
            fontData.iFontSpec  = font->FontSpecInTwips();
            heightListCount     = heightList->Count();
            for (heightIndex = 0; heightIndex<heightListCount; heightIndex++)
            {
                fontData.iFontSpec.iHeight = getTheHeightInTWipsL(iCoeEnv, *this,
                                             fontData.iFontSpec, heightIndex, heightList);
                arrayFontData->AppendL(fontData);
            }
        }
        heightList->Reset();
        eikEnv.ScreenDevice()->ReleaseFont(font);
        font = NULL;
    }

    CleanupStack::PopAndDestroy(heightList);
    CleanupStack::Pop(arrayFontData);
    CleanupStack::PopAndDestroy(fontNameList);
    return arrayFontData;
}

TRgb ASwtDisplayBase::GetSystemColor(TSwtColorId aId) const
{
    ASSERT_NATIVEUITHREAD();
    return iUiUtils->GetSystemColor(aId); //lint !e613
}

const MSwtFont* ASwtDisplayBase::GetSystemFont() const
{
    ASSERT_NATIVEUITHREAD();
    if (!iSystemFont)
    {
        ASwtDisplayBase* self = const_cast<ASwtDisplayBase*>(this);
        TRAP_IGNORE(self->iSystemFont = CSwtFont::NewL(*self, *AknLayoutUtils::FontFromId(
                                            CSwtLafFacade::GetFontId(CSwtLafFacade::EForm2MidpLabelPaneT1Font, 0))));
    }
    return iSystemFont;
}

TInt ASwtDisplayBase::GetDefaultFontHeightL()
{
    TFontSpec fontSpec(KNullDesC, 0);
    CFont* font = NULL;
    CBitmapDevice& device = GraphicsDevice();
    User::LeaveIfError(device.GetNearestFontInTwips(font, fontSpec));

    TInt result = 0;
    if (font!=NULL)
    {
        fontSpec = font->FontSpecInTwips();
        result = FontUtils::PointsFromTwips(fontSpec.iHeight);
        device.ReleaseFont(font);
        font = NULL;
    }

    return result;
}


//
// Virtual methods from MSwtDrawable
//

MSwtGc* ASwtDisplayBase::NewGcL()
{
    _LIT(KLibName, "scdv");

    ASSERT_NATIVEUITHREAD();

    CFbsScreenDevice* device = CFbsScreenDevice::NewL(KLibName, iCoeEnv->ScreenDevice()->DisplayMode());
    device->SetAutoUpdate(ETrue);
    CleanupStack::PushL(device);
    CFbsBitGc* nativeGc;
    User::LeaveIfError(device->CreateContext(nativeGc));
    CleanupStack::Pop(device);

    return iFactory->NewBitmapGcL(*this, nativeGc, DestroyNativeGc, KRgbBlack, KRgbWhite, *iSystemFont); //lint !e613
}

void ASwtDisplayBase::DestroyNativeGc(CBitmapContext* aGc)
{
    delete aGc->Device();
    delete aGc;
}

CBitmapDevice& ASwtDisplayBase::GraphicsDevice()
{
    ASSERT_NATIVEUITHREAD();
    return *(iCoeEnv->ScreenDevice());
}

void ASwtDisplayBase::HandleUpdate()
{
    // Nothing to do
}

void ASwtDisplayBase::SetMobileDevice(MSwtMobileDevice* aMobileDevice)
{
    // Not to be called by anyone except CSwtMobileDevice and only once
    ASSERT(!iMobileDevice);
    iMobileDevice = aMobileDevice;
}

MSwtMobileDevice* ASwtDisplayBase::MobileDevice()
{
    return iMobileDevice;
}

#ifdef RD_SCALABLE_UI_V2
void ASwtDisplayBase::TryDetectLongTapL(const TPointerEvent& aPointerEvent)
{
    ASSERT_NATIVEUITHREAD();
    ASSERT(iCommandArranger);

    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        iLongTapControl = NULL;
        iLongTapDetected = EFalse;
        MSwtControl* ctrl = iUiUtils->GetPointerGrabbingControl();

        // No long tap animation on scrollbars or trimings.
        // Long tap animation only if there is a popup menu.
        if (ctrl && ctrl->IsLongTapAnimationCandidate(aPointerEvent))
        {
            iLongTapControl = ctrl;
        }
    }
    
    iLongTapPointerEvent = aPointerEvent;

    if (iLongTapControl)
    {
        iLongTapDetector->PointerEventL(aPointerEvent);
    }
}

MSwtControl* ASwtDisplayBase::LongTapAnimationControl() const
{
    ASSERT_NATIVEUITHREAD();
    return iLongTapControl;
}

void ASwtDisplayBase::CancelLongTapAnimation()
{
    ASSERT_NATIVEUITHREAD();
    // IAD WARNING: Do not call CAknLongTapDetector::CancelAnimationL()
    iLongTapControl = NULL;
    iLongTapDetected = EFalse;
}

void ASwtDisplayBase::HandleLongTapEventL(const TPoint& /*aPenEventLocation*/,
        const TPoint& aPenEventScreenLocation)
{
    ASSERT_NATIVEUITHREAD();
    ASSERT(iMenuArranger);

    MSwtControl* ctrl = iLongTapControl;
    if (!ctrl)
    {
        // Control disposed or pen lifted just before menu was about to open.
        iLongTapDetected = EFalse;
        return;
    }

    // Will be switched back after the popup menu closes 
    // and the late pointer up event was delivered.
    iLongTapDetected = ETrue;

    // Open menu on the longTap cursor position
    iMenuArranger->OpenStylusPopupMenuL(*ctrl, aPenEventScreenLocation, this);
}
#endif //RD_SCALABLE_UI_V2    

// From MSwtPopupMenuCallBack
// Native Ui Thread
void ASwtDisplayBase::HandlePopupMenuClosedL()
{
    if (iLongTapControl)
    {
        // Forward delayed pointer event
        TPointerEvent event(iLongTapPointerEvent);
        event.iType = TPointerEvent::EButton1Up;
        iLongTapControl->HandlePointerEventL(event); // revert
    }
    
    // Just to clear the flags.
    CancelLongTapAnimation();
}

// From MSwtMediaKeysListener
// Native Ui Thread
void ASwtDisplayBase::HandleMediaKeyEvent(TKeyEvent& aKeyEvent, TInt aEventType)
{
    ASSERT(iUiUtils);

    MSwtControl* focusControl = 0;
    MSwtShell* activeShell = NULL;

    if (iUiUtils->IsAppFocused())
    {
        activeShell = iUiUtils->GetActiveShell();

        if (activeShell)
        {
            focusControl = activeShell->FocusControl();
        }
    }
    else
    {
        CSwtUiUtils* uiUtils = static_cast<CSwtUiUtils*>(iUiUtils);
        focusControl = uiUtils->NextFocusedControl();
    }

    if (focusControl)
    {
        TRAP_IGNORE(focusControl->CoeControl().OfferKeyEventL(aKeyEvent, (TEventCode)aEventType));
    }
    else
    {
        if (activeShell)
        {
            TRAP_IGNORE(activeShell->Control()->CoeControl().OfferKeyEventL(aKeyEvent, (TEventCode)aEventType));
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::LoadResourceFileL
// ---------------------------------------------------------------------------
//
TInt ASwtDisplayBase::LoadResourceFileL()
{
    TFileName langFile = java::util::S60CommonUtils::ResourceLanguageFileNameL(KSwtResFile);
    return iCoeEnv->AddResourceFileL(langFile);
}
