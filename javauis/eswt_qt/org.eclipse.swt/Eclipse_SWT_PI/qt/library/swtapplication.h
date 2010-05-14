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

#ifndef SWTAPPLICATION_H_
#define SWTAPPLICATION_H_

#include <QObject>
#include <QApplication>
#include <QTime>
#include <QPointer>
#include <jni.h>
#include "jniutils.h"

#ifdef __SYMBIAN32__
#include "swtmobiledevice.h"
#endif
// Convenience macro for getting the singleton application object instance
#define swtApp static_cast<SwtApplication*>(qApp)

#ifdef __SYMBIAN32__
class TWsEvent;
#endif

namespace Java { namespace eSWT {

class SwtFontCache;

/**
 * SwtApplication class. Overrides parts of QApplication to hook into the event
 * loop in correct locations. Serves as a singleton object to store things so
 * that writable static data (not available on all platforms) is not needed in
 * the shared library.
 */
class SwtApplication: public QApplication
{
Q_OBJECT

public:
    /**
     * Constructor.
     * @param argc To be passed to QApplication constructor
     * @param argv To be passed to QApplication constructor
     * @param aJniUtils JNI utility instance, will not take ownership
     * @param version To be passed to QApplication constructor
     */
    SwtApplication(int &argc, char **argv, JniUtils* aJniUtils, 
            int version = QT_VERSION);

    /**
     * Destructor.
     */
    virtual ~SwtApplication();

    /**
     * From QApplication.
     */
    bool notify(QObject *, QEvent *);

    /**
     * Returns time-stamp to be used in the events.
     * @return Milliseconds since SwtApplication was created, the counter
     * wraps to zero after 24h.
     */
    int eventTime();

    /**
     * Returns the JNI-utilities helper class instance.
     */
    inline JniUtils& jniUtils() { return *(mJniUtils.data()); }

    /**
     * Returns the font cache helper class instance.
     */
    inline SwtFontCache& fontCache() { return *mFontCache; }

    /**
     * Like QCoreApplication::postEvent except that the event won't be actually
     * posted until postDeferredEvents is called. This was added to be able to 
     * repost canceled deferred deletion events. 
     */
    void addDeferredEvent(QObject* receiver, QEvent* event);
    
    /**
     * Will post all the deferred events that have been added by calling 
     * addDeferredEvent. After calling this the events will be in Qt queue
     * and will be sent on the next call to sendPostedEvents according to 
     * the normal event processing rules. 
     */
    void postDeferredEvents();
    
    int initializeMobileDevice();
    
    void destroyMobileDevice();
    
#ifdef __SYMBIAN32__
    /**
     * From QApplication.
     */
#if QT_VERSION >= 0x040600
    bool symbianEventFilter(const QSymbianEvent* aEvent);
#else
    bool s60EventFilter(TWsEvent* aEvent);
#endif
#endif

private:

    // Internal helper class that encapsulates a QEvent and its receiver. 
    class SwtDeferredQEvent 
        {
    public:
        SwtDeferredQEvent(QObject* aReceiver, QEvent* aEvent) 
            : mReceiver(aReceiver), mEvent(aEvent) {}
        virtual ~SwtDeferredQEvent()
            {
            mReceiver = NULL;
            mEvent = NULL;
            }
        QObject* mReceiver;
        QEvent* mEvent;
        };
    
    QTime mStartTime;
    QPointer<JniUtils> mJniUtils;
    SwtFontCache* mFontCache;
    QVector<SwtDeferredQEvent*> mDeferredEvents;
#ifdef __SYMBIAN32__
    CSwtMobileDevice* iMobileDevice;
#endif
};

}
}

#endif
