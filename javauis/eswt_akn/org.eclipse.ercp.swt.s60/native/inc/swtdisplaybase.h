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


#ifndef SWTDISPLAYBASE_H
#define SWTDISPLAYBASE_H

#ifdef RD_SCALABLE_UI_V2
#include <aknlongtapdetector.h>
#endif

#include "eswtgraphics.h"
#include "swteventqueue.h"
#include "swtinstancecounts.h"
#include "swtmidmediakeyslistener.h"


class CSwtEventQueue;
class CSwtKeyMapper;
class CSwtTimer;
class MSwtFactory;
class MSwtUiUtils;
class MSwtMenuArranger;
class MSwtCommandArranger;
class CSwtMIDRemConObserver;
class CSwtJavaBufferDrawer;
class CSwtDialogBroker;

/**
 * ASwtDisplayBase
 * Base class for implementing the C++ counterpart to the
 * org.eclipse.swt.widgets.Display Java class.
 * @lib eswt
 */
NONSHARABLE_CLASS(ASwtDisplayBase)
        : public MSwtDisplay
        , public MSwtDevice
#ifdef RD_SCALABLE_UI_V2
        , public MAknLongTapDetectorCallBack
#endif
        , public MSwtPopupMenuCallBack
        , public MSwtMediaKeysListener
{
protected:
    ASwtDisplayBase();
    void ConstructInJavaUiThreadL();
    void ConstructInNativeUiThreadL();
    ~ASwtDisplayBase();
    void DestroyInNativeUiThread();

// Methods to be used by the java thread
public:
    TBool           Sleep();
    inline void     Wake();
    inline TBool    ReadAndDispatch(JNIEnv& aJniEnv);
    inline void     PopAndDispatchPaintEvents(JNIEnv& aJniEnv, MSwtControl* aSwtControl);
    inline void     PeerDisposed(TSwtPeer aPeer);
    void AddTimerL(TInt aDelayInMilliSeconds, TInt aTimerHandle);
    void RegisterTimerL(const CSwtTimer* aTimer);
    void UnRegisterTimer(const CSwtTimer* aTimer);

// Methods to be used by the native thread
public:
    void HandleNewEvent();
    TInt GetIconDepth() const;
    TInt GetDoubleClickTime() const;
    void Beep() const;
    void PostL(const TSwtKeyEventData& aData);
    TBool MousePostL(const TSwtKeyEventData& aData, TPoint point);
    MSwtImage* NewImageL(const TSize& aSize);
    MSwtImage* NewImageFromDataL(const MSwtImageData& aData);
    MSwtImage* NewImageFromThemeL(const TInt aId);
    MSwtImage* NewImageFromSvgBufL(const TDesC8& aBuf, const TSize& aSize);
    MSwtImage* NewImageFromSvgFileL(const TDesC& aFile, const TSize& aSize);
    MSwtImage* ScaleImageL(const MSwtImage& aSrcImage,
                           const TSize& aDestSize, TBool aKeepAspectRatio);
    inline CSwtJavaBufferDrawer& JavaBufferDrawer();

// Methods to be used by either thread
public:
    inline TBool IsCurrentThreadJavaUi() const;
#ifdef _DEBUG
    virtual TBool IsCurrentThreadNativeUi() const =0;
#endif
    virtual TSwtPeer JavaPeer() =0;
    inline CSwtEventQueue* EventQueue() const;

private:
    void DoWake(TBool aBecauseOfEvent);

// Own internal event methods
protected:
    void PostForegroundEventL(TSwtPeer aPeer, const TBool& aForeground);

// From MSwtDisplay (mostly intended to be used by the App UI's thread)
public:
    MSwtDevice& Device();
    const MSwtFactory& Factory() const;
    MSwtUiUtils& UiUtils();
    MSwtMenuArranger& MenuArranger();
    MSwtCommandArranger* CommandArranger();
    void AddResourceChangeObserverL(MSwtResourceChangeObserver* aObserver);
    void RemoveResourceChangeObserver(MSwtResourceChangeObserver* aObserver);
    void AddAppFocusObserverL(MSwtAppFocusObserver* aObserver);
    void RemoveAppFocusObserver(MSwtAppFocusObserver* aObserver);
    TInt GetSymbianScanCode(const TInt aSwtKeyCode) const;
    TUint GetSymbianKeyCode(const TInt aSwtKeyCode) const;
    void ForceTraverseEventL(MSwtControl& aControl,
                             TSwtTraversal aDetail, TBool aDoIt);
    void PostTraverseEventL(MSwtControl& aControl, TSwtTraversal aDetail,
                            const TKeyEvent& aKeyEvent, TEventCode aType, TBool aDoIt);
    void PostKeyEventL(MSwtControl& aControl, const TKeyEvent& aKeyEvent,
                       TEventCode aType);
    void PostPaintEventL(TSwtPeer aSenderPeer, TSwtPeer aShellPeer,
                         const TRect& aRect, TBool aMergeable);
    void PostMouseEventL(TSwtPeer aPeer, TSwtEventType aType,
                         TInt aButton, const TPoint& aPos, TInt aStateMask);
    void PostMoveEventL(TSwtPeer aPeer);
    void PostResizeEventL(TSwtPeer aPeer);
    void PostScrollEventL(TSwtPeer aPeer, TInt aDetail);
    void PostSelectionEventL(TSwtPeer aPeer);
    void PostSelectionEventL(TSwtPeer aPeer, const TDesC& aText);
    void PostSelectionEventL(TSwtPeer aPeer, TInt aDetail, TSwtPeer aItemPeer);
    void PostDefaultSelectionEventL(TSwtPeer aPeer);
    void PostDefaultSelectionEventL(TSwtPeer aPeer, TInt aDetail, TSwtPeer aItemPeer);
    void PostFocusEventL(TSwtPeer aPeer, TSwtEventType aType);
    void PostShellEventL(TSwtPeer aPeer, TSwtEventType aType);
    void PostModifyEventL(TSwtPeer aPeer);
    void PostVerifyEventL(MSwtVerifyEventObserver& aWidget, TSwtPeer aPeer,
                          TInt aStart, TInt aEnd, const TDesC& aText);
    void PostShowEventL(TSwtPeer aPeer);
    void PostHideEventL(TSwtPeer aPeer);
    void PostScreenEventL(TSwtPeer aPeer, TInt aType);
    void PostMobileDeviceEventL(TSwtPeer aPeer, TInt aEventType);
    void PostTreeEventL(TSwtPeer aPeer, TSwtEventType aType, TInt aItemHandle);
    void PostLocationChangingEventL(TSwtPeer aPeer, MSwtBrowser& aBrowser,
                                    TBool aDoIt, TBool aTop, const TDesC& aLocation,
                                    TSwtBrCallBackOperationType aCallBackOperationType);
    void PostLocationChangedEventL(TSwtPeer aPeer, TBool aDoIt,
                                   TBool aTop, const TDesC& aLocation);
    void PostProgressEventL(TSwtPeer aPeer, TInt current, TInt total);
    void PostProgressCompletedEventL(TSwtPeer aPeer, TInt current, TInt total);
    void PostStatusTextEventL(TSwtPeer aPeer, const TDesC& aText);
    void PostTitleEventL(TSwtPeer aPeer, const TDesC& aTitle);
    void PostDialogResultEventL(CSwtDialogBroker* aBroker, TSwtPeer aPeer, const TDesC& aDialogResult);
    void PostDialogResultEventL(CSwtDialogBroker* aBroker, TSwtPeer aPeer, TInt aDialogResult);
    void PostDialogResultEventL(CSwtDialogBroker* aBroker, TSwtPeer aPeer, TInt aDialogResult1, TInt aDialogResult2, TInt aDialogResult3);
    void PostCloseEventL(TSwtPeer aPeer, TBool& aDispatched);
    void PostShowFocusedControlEventL(TSwtPeer aPeer);
    TInt ApplicationUid();
    void SetMobileDevice(MSwtMobileDevice* aMobileDevice);
    MSwtMobileDevice* MobileDevice();
#ifdef RD_SCALABLE_UI_V2
    void TryDetectLongTapL(const TPointerEvent& aPointerEvent);
    MSwtControl* LongTapAnimationControl() const;
    void CancelLongTapAnimation();
#endif // RD_SCALABLE_UI_V2
    CEikonEnv* CoeEnv() const;
    CDesC16ArrayFlat* GetFontNamesL(TBool aScalable) const;
    TBool RevertPointerEvent() const;
    void SetRevertPointerEvent(TBool aStatus);

// From MSwtDevice
public:
    MSwtColor* CreateColorL(const TRgb& aRgbValue) const;
    TRect Bounds() const;
    TRect ClientArea() const;
    TInt  Depth() const;
    TSize Dpi() const;
    CArrayFixFlat<TSwtFontData>* GetFontListL(const TDesC& aFaceName,
            TBool aScalable) const;
    TRgb  GetSystemColor(TSwtColorId aId) const;
    const MSwtFont* GetSystemFont() const;
    TInt  GetDefaultFontHeightL();

// From MSwtBitmapDrawable
public:
    MSwtGc* NewGcL();
    CBitmapDevice& GraphicsDevice();
    void HandleUpdate();
private:
    static void DestroyNativeGc(CBitmapContext* aGc);

#ifdef RD_SCALABLE_UI_V2
// From MAknLongTapDetectorCallBack
public:
    void HandleLongTapEventL(const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation);
#endif

// From MSwtPopupMenuCallBack
public:
    void HandlePopupMenuClosedL();

// From MSwtMediaKeysListener
public:
    void HandleMediaKeyEvent(TKeyEvent& aKeyEvent, TInt aEventType);

// Own private methods
private:
    TInt LoadResourceFileL();
    void StoreSvgAsMifL(const TDesC8& aSvgBuf, TFileName& aGeneratedFile);
    CFbsBitmap* RasterizeL(const CFbsBitmap& aMifBmp);
    HBufC8* LoadFileL(const TDesC& aFileName);

// Java thread's data
protected:
    // The thread of the Java VM
    RThread iJavaUiThread;

    // The event queue, owned, cannot be NULL
    CSwtEventQueue* iEventQueue;

    // Our status object used for notification of events
    TRequestStatus iRequestStatus;

// Native thread's data
protected:
    // DLL's factory, owned, cannot be NULL
    MSwtFactory* iFactory;

    // The UI utility object, owned, cannot be NULL
    MSwtUiUtils* iUiUtils;

    // The Menu arranger, owned, cannot be NULL
    MSwtMenuArranger* iMenuArranger;

    // The Command arranger, owned, may be NULL
    MSwtCommandArranger* iCommandArranger;

    // The key mapper, owned, cannot be NULL
    CSwtKeyMapper* iKeyMapper;

    // The system font, object owned. Null at construction,
    // allocated at the first SystemFontL() method call.
    MSwtFont* iSystemFont;

    // The java side timers that are currently active
    RPointerArray<CSwtTimer> iTimers;

    // Registered resource change observers
    RPointerArray<MSwtResourceChangeObserver> iResourceChangeObservers;

    // Registered app focus observers
    RPointerArray<MSwtAppFocusObserver> iAppFocusObservers;

    // The unique instance of the mobile device
    MSwtMobileDevice* iMobileDevice;

    // The listener object for media keys
    CSwtMIDRemConObserver* iRemConObserver;

    // The shared object taking care of drawing the Java
    // content for all controls, owned, cannot be NULL
    CSwtJavaBufferDrawer* iJavaBufferDrawer;

#ifdef RD_SCALABLE_UI_V2
    // The long tap detector, cannot be NULL
    CAknLongTapDetector* iLongTapDetector;

    // Indicates if a long tap has been detected after the previous EButton1Down event.
    TBool iLongTapDetected;

    // The pointer event of type EButton1Down which initiates the long tap
    TPointerEvent iLongTapPointerEvent;

    // The control upon which long tap started
    MSwtControl* iLongTapControl;
#endif

    // Id of the loaded resource file
    TInt iResId;

    // Cached Coe env reference.
    CEikonEnv* iCoeEnv;

    // Flag for state reverting pointer events.
    TBool iRevertPointerEvent;

#if defined(_DEBUG) && defined(__WINS__)
public:
    TSwtInstanceCounts iInstanceCounts;
#endif // defined(_DEBUG) && defined(__WINS__)

public:
    TInt iApplicationUid;
};

/**
 * C++ consructor
 */
inline ASwtDisplayBase::ASwtDisplayBase()
        : iResId(KErrNotFound)
{
}

/**
 * Checks if the current thread is the main thread.
 */
inline TBool ASwtDisplayBase::IsCurrentThreadJavaUi() const
{
    return (RThread().Id() == iJavaUiThread.Id());
}

/**
 * Awakens the display
 * Must not be executed from within the AppUi thread, but can be called from
 * within any Java thread.
 */
inline void ASwtDisplayBase::Wake()
{
    ASSERT(!IsCurrentThreadNativeUi());
    DoWake(EFalse);
}

/**
 * Reads the oldest event in the event queue and dispatches it.
 * Must be executed from within the main thread.
 * @return EFalse if the caller can sleep upon return; ETrue otherwise.
 */
inline TBool ASwtDisplayBase::ReadAndDispatch(JNIEnv& aJniEnv)
{
    ASSERT(IsCurrentThreadJavaUi());
    return iEventQueue->PopAndDispatch(aJniEnv);
}

/**
 * Reads paint events in the event queue and dispatches it.
 * Must be executed from within the main thread.
 * @param aPeer If equals NULL then all paint events of all control are dispatch
 *        otherwise only the paint event of the Control correspondent to Peer was dispatched.
 */
inline void ASwtDisplayBase::PopAndDispatchPaintEvents(JNIEnv& aJniEnv, MSwtControl* aSwtControl)
{
    ASSERT(IsCurrentThreadJavaUi());
    iEventQueue->PopAndDispatchPaintEvents(aJniEnv, aSwtControl);
}

/**
 * Informs the Display that a Java object has been disposed of (i.e. its
 * C++ counterpart has been destroyed).
 * Must be executed from within the main thread.
 * @param aPeer The Java object that has been disposed of.
 */
inline void ASwtDisplayBase::PeerDisposed(TSwtPeer aPeer)
{
    ASSERT(IsCurrentThreadJavaUi());
    iEventQueue->PeerDisposed(aPeer);
}

/**
 * Returns the event queue
 */
inline CSwtEventQueue* ASwtDisplayBase::EventQueue() const
{
    return iEventQueue;
}

/**
 * Returns the shared object taking care of drawing the Java content for all controls.
 */
inline CSwtJavaBufferDrawer& ASwtDisplayBase::JavaBufferDrawer()
{
    return *iJavaBufferDrawer;
}

#endif // SWTDISPLAYBASE_H
