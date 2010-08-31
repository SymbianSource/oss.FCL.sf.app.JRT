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

#ifndef EVENTCALLBACK_H
#define EVENTCALLBACK_H

#include <QObject>
#include "swtapplication.h"

/**
 * A class defining an object which intercepts events from the Qt event dispatcher
 * and offers them to the Java peer object (the Display instance).
 */
namespace Java { namespace eSWT {

class JniUtils;

class EventCallback : public QObject {
    Q_OBJECT
public:
    /**
     * The constructor.
     */
    EventCallback();

    /**
     * Marks the instance for deletion. If there are active eventFilter calls then
     * the deletion will be deferred and carried out when the last eventFilter returns.
     */
    void Destroy();

    bool eventFilter(QObject* aObj, QEvent* aEvent, const int& aSwtEventType);

private:
    /**
     * The destructor. Private because the instance must be deleted only after
     * it has been verified that all eventFilter calls have returned.
     */
    virtual ~EventCallback();

protected:
    bool eventFilter(QObject* aObj, QEvent* aEvent);
    bool eventToJava(QObject* aObj, QEvent* aEvent);
    bool eventToJava(QObject* aObj, QEvent* aEvent, const int& aSwtEventType);
    bool doSendEvent(const QObject* aQObject, const int& aQEventType,
                      const int& a1 = 0, const int& a2 = 0, const int& a3 = 0,
                      const int& a4 = 0, const int& a5 = 0);

private:
    int  mEventFilterCallsActive;
    bool mDestroying;
    QObjectPtr mEventTargetObjectAlive;
    JniUtils& mJniUtils;
};

}}

#endif //EVENTCALLBACK_H

