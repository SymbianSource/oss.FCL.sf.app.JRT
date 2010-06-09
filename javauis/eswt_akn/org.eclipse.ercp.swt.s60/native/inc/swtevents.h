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


#ifndef SWTEVENTS_H
#define SWTEVENTS_H


#include <jni.h>
#include "swtdisplay.h"
#include "swteventqueue.h"
#include "eswtgraphics.h"


class CEikMenuPane;
class MSwtControl;
class CSwtDialogBroker;


/**
 * CSwtEvent
 * Base class for all events
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtEvent)
        : public CBase
{
// Own methods
public:
    /**
     * Dispatch event to Java peer. Callback events must not initiate the
     * callback if either the default display or their peer was disposed.
     * @param aJniEnv jni env.
     */
    void Dispatch(JNIEnv* aJniEnv);

    /**
     * Peer getter. Returns the Java object this event is targeted to.
     * @return peer jobject.
     */
    inline jobject Peer() const;

    /**
     * Event type getter.
     * @return TSwtEventType.
     */
    virtual TSwtEventType Type() const =0;

    /**
     * Peer disposed notifier. Informs this event that the Java object
     * it is targeted to has been disposed.
     */
    inline void PeerDisposed();

    /**
     * Pushed to queue notifier. Called by the processing event queue
     * when the event is pushed into the queue.
     * @aEventQueue the queue.
     */
    inline void PushedToQueue(CSwtEventQueue* aQueue);

protected:
    /**
     * Default constructor.
     * @param aPeer Java object this event is targeted to.
     */
    inline CSwtEvent(TSwtPeer aPeer);

    /**
     * Test if the Java object this event is targeted to has been disposed.
     * @reuturn ETrue if peer disposed.
     */
    inline TBool IsPeerDisposed() const;

    /**
     * Test if display is disposed.
     * @reuturn ETrue if display disposed.
     */
    inline TBool IsDisplayDisposed() const;

    /**
     * Actual dispatch.
     * @param aJniEnv jni env.
     */
    virtual void DoDispatch(JNIEnv* aJniEnv) =0;

// Data
private:
    /**
     * Java peer this event is targeted to.
     */
    jobject iPeer;

    /**
     * The event queue in which the event is being processed.
     * Not own.
     */
    CSwtEventQueue* iQueue;
};


// -----------------------------------------------------------------------------
// CSwtEvent::Peer
// -----------------------------------------------------------------------------
//
inline jobject CSwtEvent::Peer() const
{
    return iPeer;
}

// -----------------------------------------------------------------------------
// CSwtEvent::PeerDisposed
// -----------------------------------------------------------------------------
//
inline void CSwtEvent::PeerDisposed()
{
    iPeer = NULL;
}

// -----------------------------------------------------------------------------
// CSwtEvent::IsPeerDisposed
// -----------------------------------------------------------------------------
//
inline TBool CSwtEvent::IsPeerDisposed() const
{
    return (iPeer == NULL);
}

// -----------------------------------------------------------------------------
// CSwtEvent::PushedToQueue
// -----------------------------------------------------------------------------
//
inline void CSwtEvent::PushedToQueue(CSwtEventQueue* aQueue)
{
    iQueue = aQueue;
}

// -----------------------------------------------------------------------------
// CSwtEvent::IsDisplayDisposed
// -----------------------------------------------------------------------------
//
inline TBool CSwtEvent::IsDisplayDisposed() const
{
    // In case of calling Display.dispose() from java-side-event-listener the
    // display and consequently also event queue with all events are already
    // deleted. Therefore it is needed to check this situation from Tls in order
    // not to access freed memory.
    if (!CSwtDisplay::CurrentOrNull())
    {
        return ETrue;
    }

    // check state of queue
    ASSERT(iQueue);
    return iQueue->IsDestroying();
}


/**
 * CSwtCallbackEvent
 * Utility base class for events which must call back the sender after the event
 * has been dispatched.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCallbackEvent)
        : public CSwtEvent
{
// Own methods
protected:
    inline CSwtCallbackEvent(TSwtPeer aPeer);
    virtual TBool SendEvent(JNIEnv* aJniEnv, jboolean& aDoIt) =0;
    virtual void CallbackL(TBool aDoIt) =0;

// From CSwtEvent
public:
    void DoDispatch(JNIEnv* aJniEnv);
};


/**
 * CSwtPlainEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtPlainEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtPlainEvent(TSwtPeer aPeer, TSwtEventType aType);
    CSwtPlainEvent(TSwtPeer aPeer, TSwtEventType aType, TInt aDetail,
                   TSwtPeer aItemPeer);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    const TSwtEventType iType;
    const jint iDetail;
    const jobject iItemPeer;
};


/**
 * CSwtKeyEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtKeyEvent)
        : public CSwtCallbackEvent
{
// Own methods
public:
    CSwtKeyEvent(MSwtControl& aControl, const TSwtKeyEventData& aData,
    const TKeyEvent& aKeyEvent, TEventCode aType);

// From CSwtEvent
public:
    TSwtEventType Type() const;

// From CSwtCallbackEvent
protected:
    TBool SendEvent(JNIEnv* aJniEnv, jboolean& aDoIt);
    void  CallbackL(TBool aDoIt);

// Data
protected:
    MSwtControl& iControl;

    // Java event data
    const TSwtEventType iSwtType;
    const jchar iCharacter;
    const jint iKeyCode;
    const jint iStateMask;

    // Native event data
    TKeyEvent iKeyEvent;
    TEventCode iType;
};


/**
 * CSwtTraverseEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTraverseEvent)
        : public CSwtKeyEvent
{
// Own methods
public:
    CSwtTraverseEvent(MSwtControl& aControl, TSwtTraversal aDetail, TBool aDoIt,
    const TSwtKeyEventData& aData, const TKeyEvent& aKeyEvent, TEventCode aType);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Own methods
private:
    TBool SendTraverseEvent(JNIEnv* aJniEnv, jboolean& aDoIt);
    void TraverseCallbackL();

// Data
private:
    /**
     * The traversal detail
     */
    jint iDetail;

    /**
     * The original value for the "doit" field
     */
    const jboolean iDoItDefault;
};


/**
 * CSwtMouseEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtMouseEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtMouseEvent(TSwtPeer aPeer, TSwtEventType aType, TInt aButton,
    const TPoint& aPos, TInt aStateMask);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    const TSwtEventType iType;
    const jint iButton;
    const jint iX;
    const jint iY;
    const jint iStateMask;
};


/**
 * CSwtScrollEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtScrollEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtScrollEvent(TSwtPeer aPeer, TInt aDetail);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    const jint iDetail;
};


/**
 * CSwtPaintEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtPaintEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtPaintEvent(TSwtPeer aSenderPeer, TSwtPeer aShellPeer,
    const TRect& aRect, TBool aMergeable);
    inline TBool IsMergeable() const;
    inline TRect& Rect();

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
public:
    /**
     * Whether this event can be merged with another paint event.
     */
    const TBool iMergeable;

    /**
     * The rectangle to paint.
     */
    TRect iRect;

    /**
     * Peer of the sender.
     */
    TSwtPeer iSenderPeer;
};


/**
 * CSwtResizeEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtResizeEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtResizeEvent(TSwtPeer aPeer);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);
};


/**
 * CSwtTimerCallback
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTimerCallback)
        : public CSwtEvent
{
// Own methods
public:
    CSwtTimerCallback(TSwtPeer aPeer, TInt aTimerHandle);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    TInt iTimerHandle;
};


/**
 * CSwtEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtVerifyEvent)
        : public CSwtEvent
{
// Own methods
public:
    static CSwtVerifyEvent* NewL(MSwtVerifyEventObserver& aWidget,
    TSwtPeer aPeer, TInt aStart, TInt aEnd, const TDesC& aText);
    virtual ~CSwtVerifyEvent();

private:
    inline CSwtVerifyEvent(MSwtVerifyEventObserver& aWidget,
                           TSwtPeer aPeer, TInt aStart, TInt aEnd);
    void ConstructL(const TDesC& aText);
    void DoDispatchL(JNIEnv* aJniEnv);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    MSwtVerifyEventObserver& iWidget;
    jint iStart;
    jint iEnd;
    HBufC* iOriginalText;
};

/**
 * CSwtEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtScreenEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtScreenEvent(TSwtPeer aPeer, TInt aType);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    const jint iType;
};


/**
 * CSwtMobileDeviceEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtMobileDeviceEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtMobileDeviceEvent(TSwtPeer aPeer, TInt aEventType);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

public:
    const jint iEventType;
};


/**
 * CSwtTreeEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTreeEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtTreeEvent(TSwtPeer aPeer, TSwtEventType aType, TInt aItemHandle);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    const TSwtEventType iType;
    const TInt iItemHandle;
};


/**
 * CSwtForegroundEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtForegroundEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtForegroundEvent(TSwtPeer aPeer, const TBool& aForeground);

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    TBool iForeground;
};


/**
 * CSwtEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtLocationChangingEvent)
        : public CSwtCallbackEvent
{
// Own methods
public:
    /**
     * The 1st and 2nd phase constructor wrapper
     * @param aPeer The targeted java peer
     * @param aBrowser The browser which initiates this event
     * @param aDoIt A flag indicating whether the location loading should be allowed
     * @param aTop A flag indicating whether the location opens in the top frame or not.
     * @param aLocation Current location
     * @param aCallBackOperationType The loading operation type
     */
    static CSwtLocationChangingEvent* NewL(TSwtPeer aPeer, MSwtBrowser& aBrowser,
    TBool aDoIt, TBool aTop, const TDesC& aLocation,
    TSwtBrCallBackOperationType aCallBackOperationType);

private:
    /**
     * Constructor
     * @param aPeer The targeted java peer
     * @param aBrowser The browser which initiates this event
     * @param aDoIt A flag indicating whether the location loading should be allowed
     * @param aTop A flag indicating whether the location opens in the top frame or not.
     * @param aCallBackOperationType The loading operation type
     */
    CSwtLocationChangingEvent(TSwtPeer aPeer, MSwtBrowser& aBrowser,
                              TBool aDoIt, TBool aTop,
                              TSwtBrCallBackOperationType aCallBackOperationType);

    /**
     * Second phase constructor*
     * @param aLocation Current location
     * @return void
     */
    void ConstructL(const TDesC& aLcation);

    /**
     * Destructor
     */
    virtual ~CSwtLocationChangingEvent();


    // From CSwtEvent

    /**
    * From CSwtEvent
    * Get type of the event
    *
    * @return ESwtEventLocation
    */
    TSwtEventType Type() const
    {
        return ESwtEventLocation;
    };

// From CSwtCallbackEvent
protected:
    TBool SendEvent(JNIEnv* aJniEnv, jboolean& aDoIt);
    void CallbackL(TBool aDoIt);

// Data
private:
    /**
     * A pointer to the browser which initiates this event
     * Not own
     */
    MSwtBrowser*  iBrowser;

    /**
     * A flag indicating whether the location loading should be allowed
     */
    const TBool iDoIt;

    /**
     * A flag indicating whether the location opens in the top frame or not.
     */
    const TBool iTop;

    /**
     * Current location
     * Own
     */
    HBufC*  iLocation;

    /**
     * The loading operation type
     */
    TSwtBrCallBackOperationType iCallBackOperationType;
};


/**
 * CSwtLocationChangedEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtLocationChangedEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * The 1st and 2nd phase constructor wrapper
     *
     * @param aPeer The targeted java peer
     * @param aDoIt A flag indicating whether the location loading should be allowed
     * @param aTop aTop A flag indicating whether the location opens in the top frame or not
     * @param aLocation The current location
     */
    static CSwtLocationChangedEvent* NewL(TSwtPeer aPeer, TBool aDoIt
    , TBool aTop, const TDesC&  aLocation);

private:
    /**
     * Constructor
     *
     * @param aPeer The targeted java peer
     * @param aDoIt A flag indicating whether the location loading should be allowed
     * @param aTop aTop A flag indicating whether the location opens in the top frame or not
     */
    CSwtLocationChangedEvent(TSwtPeer aPeer, TBool aDoIt, TBool aTop);

    /**
     * Second phase constructor
     *
     * @param aLocation Current location
     * @return void
     */
    void ConstructL(const TDesC&  aLocation);

    /**
     * Destructor
     */
    virtual ~CSwtLocationChangedEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const
    {
        return ESwtEventLocation;
    };
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     *  A flag indicating whether the location loading should be allowed
     */
    const TBool iDoIt;

    /**
     * A flag indicating whether the location opens in the top frame or not.
     */
    const TBool iTop;

    /**
     * Current location
     * Own
     */
    HBufC* iLocation;
};


/**
 * CSwtProgressEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtProgressEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * Constructor
     * @param aPeer The targeted java peer
     * @param aCurrent The current value
     * @param aTotal The total value
     */
    CSwtProgressEvent(TSwtPeer aPeer, TInt aCurrent, TInt aTotal);

    /**
     * Destructor
     */
    virtual ~CSwtProgressEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const
    {
        return ESwtEventProgress;
    };
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     * The current value
     */
    const TInt iCurrent;

    /**
     * The total value
     */
    const TInt iTotal;
};


/**
 * CSwtProgressCompletedEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtProgressCompletedEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * Constructor
     * @param aPeer The targeted java peer
     * @param aCurrent The current value
     * @param aTotal The total value
     */
    CSwtProgressCompletedEvent(TSwtPeer aPeer, TInt aCurrent, TInt aTotal);

    /**
     * Destructor
     */
    virtual ~CSwtProgressCompletedEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const
    {
        return ESwtEventProgress;
    };
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     * The current value
     */
    const TInt iCurrent;

    /**
     * The total value
     */
    const TInt iTotal;
};


/**
 * CSwtStatusTextEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtStatusTextEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * The 1st and 2nd phase constructor wrapper
     * @param aPeer The targeted java peer
     * @param aStatusText The status text
     */
    static CSwtStatusTextEvent* NewL(TSwtPeer aPeer, const TDesC& aStatusText);

// Own methods
private:
    /**
     * Constructor
     * @param aPeer The targeted java peer
     */
    CSwtStatusTextEvent(TSwtPeer aPeer);

    /**
     * Second phase constructor
     * @param aStatusText The status text
     */
    void ConstructL(const TDesC&  aStatusText);

    /**
     * Destructor
     */
    virtual ~CSwtStatusTextEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const
    {
        return ESwtEventStatusText;
    };
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     * The status text
     * Own
     */
    HBufC* iStatusText;
};


/**
 * CSwtTitleEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTitleEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * The 1st and 2nd phase constructor wrapper
     * @param aPeer The targeted java peer
     * @param aTitle The title of the current document
     */
    static CSwtTitleEvent* NewL(TSwtPeer aPeer, const TDesC&  aTitle);

private:
    /**
     * Constructor
     * @param aPeer The targeted java peer
     */
    CSwtTitleEvent(TSwtPeer aPeer);

    /**
     * Second phase constructor
     * @param aTitle The title of the current document
     */
    void ConstructL(const TDesC&  aTitle);

    /**
     * Destructor
     */
    virtual ~CSwtTitleEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const
    {
        return ESwtEventTitle;
    };
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     * The title of the current document
     * Own
     */
    HBufC* iTitle;
};

/**
 * CSwtDialogStringResult
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtDialogStringResultEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * The 1st and 2nd phase constructor wrapper
     * @param aBroker The dialog broker to be deleted
     * @param aPeer The targeted java peer
     * @param aDialogResult The dialog string result
     */
    static CSwtDialogStringResultEvent* NewL(CSwtDialogBroker* aBroker, TSwtPeer aPeer, const TDesC&  aDialogResult);

private:
    /**
     * Constructor
     * @param aBroker The dialog broker to be deleted
     * @param aPeer The targeted java peer
     */
    CSwtDialogStringResultEvent(CSwtDialogBroker* aBroker, TSwtPeer aPeer);

    /**
     * Second phase constructor
     * @param aResultString The dialog result string
     */
    void ConstructL(const TDesC&  aDialogResult);

    /**
     * Destructor
     */
    virtual ~CSwtDialogStringResultEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     * The DialogBroker to be deleted
     */
    CSwtDialogBroker* iDialogBroker;

    /**
     * The title of the current document
     * Own
     */
    HBufC* iDialogResult;
};

/**
 * CSwtDialogIntegerResultEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtDialogIntegerResultEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * Constructor
     * @param aBroker The dialog broker to be deleted
     * @param aPeer The targeted java peer
     * @param aCurrent The dialog integer resulte
     */
    CSwtDialogIntegerResultEvent(CSwtDialogBroker* aBroker, TSwtPeer aPeer, TInt aDialogResult);

    /**
     * Destructor
     */
    virtual ~CSwtDialogIntegerResultEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     * The DialogBroker to be deleted
     */
    CSwtDialogBroker* iDialogBroker;

    /**
     * The current value
     */
    const TInt iDialogResult;
};

/**
 * CSwtDialogRgbResultEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtDialogRgbResultEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * Constructor
     * @param aBroker The dialog broker to be deleted
     * @param aPeer The targeted java peer
     * @param aCurrent The dialog integer resulte
     */
    CSwtDialogRgbResultEvent(CSwtDialogBroker* aBroker, TSwtPeer aPeer, TInt aRed, TInt aGreen, TInt aBlue);

    /**
     * Destructor
     */
    virtual ~CSwtDialogRgbResultEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const;
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:

    /**
     * The DialogBroker to be deleted
     */
    CSwtDialogBroker* iDialogBroker;

    /**
     * The current value
     */
    const TInt iRed;
    const TInt iGreen;
    const TInt iBlue;
};


/**
 * CSwtCloseEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCloseEvent)
        : public CSwtCallbackEvent
{
// Own methods
public:
    CSwtCloseEvent(TSwtPeer aPeer, TBool& aDispatched);

// From CSwtEvent
public:
    TSwtEventType Type() const;

// From CSwtCallbackEvent
protected:
    TBool SendEvent(JNIEnv* aJniEnv, jboolean& aDoIt);
    void CallbackL(TBool aDoIt);

// Data
private:
    TBool& iDispatched;
    CSwtDisplay* iDisplay;
};

/**
 * CSwtSettingsEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtSettingsEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtSettingsEvent(TSwtPeer aPeer, TBool aThemeChanged);

// From CSwtEvent
public:
    TSwtEventType Type() const;

protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     * True if the event is due to a theme change
     **/
    const TBool iThemeChanged;
};

/**
 * CSwtTextSelectionEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTextSelectionEvent)
        : public CSwtEvent
{
// Own methods
public:
    /**
     * The 1st and 2nd phase constructor wrapper
     * @param aPeer The targeted java peer
     * @param aSelectedText The selected text
     */
    static CSwtTextSelectionEvent* NewL(TSwtPeer aPeer, const TDesC& aSelectedText);

// Own methods
private:
    /**
     * Constructor
     * @param aPeer The targeted java peer
     */
    CSwtTextSelectionEvent(TSwtPeer aPeer);

    /**
     * Second phase constructor
     * @param aSelectedText The status text
     */
    void ConstructL(const TDesC&  aSelectedText);

    /**
     * Destructor
     */
    virtual ~CSwtTextSelectionEvent();

// From CSwtEvent
public:
    TSwtEventType Type() const
    {
        return ESwtEventSelection;
    };
protected:
    void DoDispatch(JNIEnv* aJniEnv);

// Data
private:
    /**
     * The status text
     * Own
     */
    HBufC* iSelectedText;
};

/**
 * CSwtShowFocusedControlEvent
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtShowFocusedControlEvent)
        : public CSwtEvent
{
// Own methods
public:
    CSwtShowFocusedControlEvent(TSwtPeer aPeer);

// From CSwtEvent
public:
    TSwtEventType Type() const;

protected:
    void DoDispatch(JNIEnv* aJniEnv);
};
#endif // SWTEVENTS_H
