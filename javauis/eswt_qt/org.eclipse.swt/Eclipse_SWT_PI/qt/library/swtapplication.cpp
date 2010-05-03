/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <QPaintEvent>

#ifdef __SYMBIAN32__
#if QT_VERSION >= 0x040600
#include <QSymbianEvent>
#endif
#endif

#include <org_eclipse_swt_internal_qt_OS.h>
#include "swtapplication.h"
#include "jniutils.h"
#include "swtfontcache.h"
#include "swt.h"
#include "swtlog.h"

#ifdef __SYMBIAN32__
#include "swts60.h"
#endif




using namespace Java::eSWT;

SwtApplication::SwtApplication(int &argc, char **argv, JniUtils* aJniUtils, int version)
    : 
#ifdef __SYMBIAN32__
        QApplication(SymbianUtils::NewApplication, argc, argv, version), iMobileDevice(NULL)
#else
        QApplication(argc, argv, version)
#endif
        , mJniUtils(aJniUtils)
    {
    SWT_LOG_FUNC_CALL();
    mFontCache = new SwtFontCache();
    mStartTime = QTime::currentTime();
    }

SwtApplication::~SwtApplication()
    {
    SWT_LOG_FUNC_CALL();
    delete mFontCache;
    mFontCache = NULL;
    }

bool SwtApplication::notify(QObject* aObj, QEvent* aEvent)
    {
    SWT_LOG_FUNC_CALL();

    // QApplication::notify will offer the event to the application event filters.
    // If they don't consume the event then it will be offered to the object event
    // filters. If they don't consume the event then it will be offered to the
    // QObject::event.
    bool consumed = QApplication::notify(aObj, aEvent);

    // Note that consumed == true might mean any of these:
    // 1. Event was cancelled in the application event filters
    // 2. Event was cancelled in the object event filters
    // 3. Event was passed to the QObject which consumed it normally
    // We don't check what happened here but call Java paint listeners always.
    // This means that if the event is cancelled by the application filter then
    // object filters and passing to the QWidget are skipped but Java paint
    // event is still sent.
    switch ( aEvent->type() )
        {
        // Some widgets (at least QTextEdit) are not painting fully when
        // receiving a paint event. Some have a weird system of object
        // event filters on other widgets that trigger the painting. For that
        // reason it's not enough to offer the paint event to QWidget::event()
        // but all paint events must be offered also to the object event
        // filters. Thus, here is the correct place where all painting has
        // been done by QWidget and Java content can be painted on top.
        case QEvent::Paint:
            {
            QPaintEvent* paintEvent = static_cast<QPaintEvent*>(aEvent);
            const QRect rect = paintEvent->rect();
            const QRegion* region = &(paintEvent->region());
            // Made up event after QWidget has painted, Java paints here
            mJniUtils->eventProcess(aObj, org_eclipse_swt_internal_qt_OS_QSWTEVENT_WIDGETPAINTED, rect.x(),
                rect.y(), rect.width(), rect.height(),
                reinterpret_cast<int> (region));
            break;
            }
        case QEvent::Resize:
            {
            QResizeEvent* event = static_cast<QResizeEvent*>(aEvent);
            const QSize& size = event->size();
            const QSize& oldSize = event->oldSize();
            mJniUtils->eventProcess(aObj, org_eclipse_swt_internal_qt_OS_QSWTEVENT_WIDGETRESIZED, oldSize.width(), oldSize.height(), size.width(), size.height());
            break;
            }
        case QEvent::Move:
            {
            mJniUtils->eventProcess(aObj, org_eclipse_swt_internal_qt_OS_QSWTEVENT_WIDGETMOVED);
            break;
            }
        case QEvent::ApplicationActivate:
            {
#ifdef __SYMBIAN32__
            if(iMobileDevice)
                {
                iMobileDevice->AppForeground();
                }
#endif
            break;
            }
        default:
            break;
        }
        
    return consumed;
    }

int SwtApplication::initializeMobileDevice()
    {
    SWT_LOG_FUNC_CALL();
    int handle = 0;
#ifdef __SYMBIAN32__
   handle = reinterpret_cast<int>(CSwtMobileDevice::NewL());
#endif
    return handle;
    }

void SwtApplication::destroyMobileDevice()
    {
    SWT_LOG_FUNC_CALL();
#ifdef __SYMBIAN32__
    if(iMobileDevice)
        {
        delete iMobileDevice;
        iMobileDevice = NULL;
        }
#endif    
    }

int SwtApplication::eventTime()
    {
    SWT_LOG_FUNC_CALL();
    return mStartTime.elapsed();
    }

void SwtApplication::addDeferredEvent(QObject* aReceiver, QEvent* aEvent)
    {
    SWT_LOG_FUNC_CALL();
    mDeferredEvents.append(new SwtDeferredQEvent(aReceiver, aEvent));
    }

void SwtApplication::postDeferredEvents()
    {
    SWT_LOG_FUNC_CALL();
    const int size = mDeferredEvents.size();
    for(int i = 0; i < size; ++i)
        {
        SwtDeferredQEvent* const& eventContainer = mDeferredEvents.at(i);
        postEvent(eventContainer->mReceiver, eventContainer->mEvent);
        delete eventContainer;
        }
    if(size)
        {
        mDeferredEvents.erase(mDeferredEvents.begin(), mDeferredEvents.end());
        }
    }

#ifdef __SYMBIAN32__
#if QT_VERSION >= 0x040600
bool SwtApplication::symbianEventFilter(const QSymbianEvent* aEvent)
    {
    if(aEvent->type()== QSymbianEvent::ResourceChangeEvent) 
        {
        return SymbianUtils::eventFilter(this, aEvent->resourceChangeType(), -1);
        }
    const TWsEvent* wsEvent = aEvent->windowServerEvent(); 
    if(wsEvent)
        {
        return SymbianUtils::eventFilter(this, wsEvent);
        }
    return false;
    }
#else
bool SwtApplication::s60EventFilter(TWsEvent* aEvent)
    {
    return SymbianUtils::eventFilter(this, aEvent);
    }
#endif
#endif
