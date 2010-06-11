/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <QEvent>
#include <QPaintEvent>
#include <QRect>

#include <org_eclipse_swt_internal_qt_OS.h>
#include "eventcallback.h"
#include "swtlog.h"
#include "jniutils.h"
#include "swtapplication.h"

using namespace Java::eSWT;

EventCallback::EventCallback()
    : mEventFilterCallsActive(0),
      mDestroying(false), 
      mJniUtils(swtApp->jniUtils())
    {
    SWT_LOG_FUNC_CALL();
    }

EventCallback::~EventCallback()
    {
    SWT_LOG_FUNC_CALL();
    Q_ASSERT( mEventFilterCallsActive == 0 );
    }

void EventCallback::Destroy()
    {
    SWT_LOG_FUNC_CALL();
    SWT_LOG_DATA_1("Active event filter calls=%d", mEventFilterCallsActive);
    mDestroying = true;
    if( mEventFilterCallsActive == 0 )
        {
        delete this;
        }
    }

bool EventCallback::eventFilter(QObject* aObj, QEvent* aEvent)
    {
    return eventFilter(aObj, aEvent, -1);
    }

bool EventCallback::eventFilter(QObject* aObj, QEvent* aEvent, const int& aSwtEventType)
    {
    SWT_LOG_FUNC_CALL();
    SWT_LOG_DATA_1("Active event filter calls=%d", mEventFilterCallsActive + 1);

    ++mEventFilterCallsActive;

    bool returnCode = eventToJava( aObj, aEvent, aSwtEventType );

    if( --mEventFilterCallsActive == 0 && mDestroying )
        {
        delete this;
        }

    return returnCode;
    }

/*
 * Note that if event is cancelled here then it won't be passed to object event
 * filters or the QObject::event. If you need to hook into event loop after
 * event has been fully processed then it should be done in notify().
 */
bool EventCallback::eventToJava(QObject* aObj, QEvent* aEvent, const int& aSwtEventType)
    {
    if( aSwtEventType > -1 )
        {
        return doSendEvent(aObj, aSwtEventType);
        }

    int eventType = aEvent->type();

    switch ( eventType )
        {
        case QEvent::Paint:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (paint event) object=%x", eventType, aObj);
            QPaintEvent *paintEvent = static_cast<QPaintEvent*>(aEvent);
            const QRect rect = paintEvent->rect();
            const QRegion *region = &(paintEvent->region());
            return doSendEvent(aObj, eventType, rect.x(),
                rect.y(), rect.width(), rect.height(),
                reinterpret_cast<int> (region));
            }
        case QEvent::Close:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (close event) object=%x", eventType, aObj);
            return doSendEvent(aObj, eventType);
            }
        case QEvent::KeyPress: //intentional fall-through
        case QEvent::KeyRelease:
        case QEvent::ShortcutOverride:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (key event) object=%x", eventType, aObj);
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(aEvent);
            // QString ensures that the character at position size() is always '\0'
            int character = keyEvent->text()[0].unicode();
            
            // In cases where modifier keys (alt, ctrl etc) are involved the 
            // keyEvent->text() will return an empty string. In such cases 
            // keyEvent->key() will contain a valid value.
            if (character == 0)
                {
                character = keyEvent->key();
                }
            
            return doSendEvent(aObj, eventType,
                keyEvent->key(), static_cast<int> (keyEvent->modifiers()),
                character, 
// This parameter is used for handling modifier keys. On Linux use the native virtual key,
// on Symbian this returns 0 so use native scan code instead.
#ifdef __SYMBIAN32__
                keyEvent->nativeScanCode()
#else
                keyEvent->nativeVirtualKey()
#endif
                );
            }
        case QEvent::MouseButtonPress: //intentional fall-through
        case QEvent::MouseButtonRelease: //intentional fall-through
        case QEvent::MouseButtonDblClick: //intentional fall-through
        case QEvent::MouseMove:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (mouse event) object=%x", eventType, aObj);
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(aEvent);
            return doSendEvent(aObj, eventType,
                static_cast<int> (mouseEvent->button()), mouseEvent->x(),
                mouseEvent->y(), static_cast<int> (mouseEvent->modifiers()),
                static_cast<int> (mouseEvent->buttons()));
            }
        case QEvent::FocusIn: // Sent before gaining focus
            {
            QFocusEvent* focusEvent = static_cast<QFocusEvent*>(aEvent);
            SWT_LOG_EVENT_3("QEvent type=%d (FocusIn) reason=%d object=%x", eventType, focusEvent->reason(), aObj);
            return doSendEvent(aObj, eventType, focusEvent->reason());
            }
        case QEvent::FocusOut: // Sent before losing focus
            {
            QFocusEvent* focusEvent = static_cast<QFocusEvent*>(aEvent);
            SWT_LOG_EVENT_3("QEvent type=%d (FocusOut) reason=%d object=%x", eventType, focusEvent->reason(), aObj);
            return doSendEvent(aObj, eventType, focusEvent->reason());
            }
        case QEvent::ContextMenu:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (context menu event) object=%x", eventType, aObj);
            QContextMenuEvent* contextEvent = static_cast<QContextMenuEvent*>(aEvent);
            return doSendEvent(aObj, eventType, contextEvent->globalX(), contextEvent->globalY(), contextEvent->reason());
            }
        case QEvent::Shortcut:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (ShortcutEvent) object=%x", eventType, aObj);
            QShortcutEvent* scEvent = static_cast<QShortcutEvent*>(aEvent);
            const QKeySequence& sequence = scEvent->key();
            const int sequenceCount( sequence.count() );
            const int keyCount( 4 ); // QKeySequence supports only maximum of 4 keys
            int keys[keyCount];
            for( int i = 0; i < keyCount; ++i )
                {
                if( i < sequenceCount )
                    {
                    keys[i] = sequence[i];
                    }
                else
                    {
                    keys[i] = 0;
                    }
                }
            return doSendEvent(aObj, eventType, keys[0], keys[1], keys[2], keys[3] );
            }
        case QEvent::WindowActivate:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (WindowActivate) object=%x", eventType, aObj);
            return doSendEvent(aObj, eventType);
            }
        case QEvent::WindowDeactivate:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (WindowDeactivate) object=%x", eventType, aObj);
            return doSendEvent(aObj, eventType);
            }
        case QEvent::WindowStateChange:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (WindowStateChange) object=%x", eventType, aObj);
            QWindowStateChangeEvent* scEvent = static_cast<QWindowStateChangeEvent*>(aEvent);
            return doSendEvent(aObj, eventType, (int)(scEvent->oldState())); // QFlags implements operator int()
            }
        case QEvent::DeferredDelete:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (DeferredDelete) object=%x", eventType, aObj);
            const bool cancel = !swtApp->jniUtils().safeToDelete(aObj); 
            if(cancel)
                {
                // Repost a new event for trying again later. Note that this 
                // new event might have different internal parameters like
                // eventloop level it was posted from etc. 
                swtApp->addDeferredEvent(aObj, new QEvent(QEvent::DeferredDelete));
                }
            return cancel;
            }
        case QEvent::ApplicationFontChange:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (ApplicationFontChange) object=%x", eventType, aObj);
            return doSendEvent(aObj, eventType);
            }
        case QEvent::ApplicationLayoutDirectionChange:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (ApplicationLayoutDirectionChange) object=%x", eventType, aObj);
            return doSendEvent(aObj, eventType);
            }
        case QEvent::ApplicationPaletteChange:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (ApplicationPaletteChange) object=%x", eventType, aObj);
            return doSendEvent(aObj, eventType);
            }
        case org_eclipse_swt_internal_qt_OS_QSWTEVENT_BUFFERFLUSH:
            {
            SWT_LOG_EVENT_2("QEvent type=%d (SwtBufferFlush) object=%x", eventType, aObj);
            return doSendEvent(aObj, eventType);
            }
        default:
            {
            SWT_LOG_EVENT_2("Unknown QEvent type=%d object=%x", eventType, aObj);
            return false;
            }
        }
    }

bool EventCallback::doSendEvent(const QObject* aQObject, const int& aQEventType,
        const int& a1, const int& a2, const int& a3,
        const int& a4, const int& a5)
    {
    mEventTargetObjectAlive = const_cast<QObject*>(aQObject);
    bool cancelInFilter = mJniUtils.eventProcess (aQObject,
            aQEventType, a1, a2, a3, a4, a5); 
    if(!mEventTargetObjectAlive)
        {
        // Safety check:
        // If the object was destroyed by the listener then drop the event 
        // being processed to avoid invalid pointer access. Shouldn't ever
        // happen. 
        cancelInFilter = true;
        }
    mEventTargetObjectAlive = NULL;
    return cancelInFilter;
    }
