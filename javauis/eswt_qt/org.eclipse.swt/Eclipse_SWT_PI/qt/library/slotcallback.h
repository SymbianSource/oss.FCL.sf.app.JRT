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

#ifndef SWTSLOTCALLBACK_H
#define SWT_SLOTCALLBACK_H

#include <QObject>
#ifndef QT_NO_SYSTEMTRAYICON
#include <QSystemTrayIcon>
#endif
#include "swtlog.h"
#include "jni.h"
#include "swt.h"

class QPoint;
class QItemSelection;
class QListWidgetItem;
class QDateTime;
class QUrl;
class QTreeWidgetItem;
class QModelIndex;

namespace Java { namespace eSWT {

class SlotCallback : public QObject
    {
    Q_OBJECT

public:
    /*
     * Constructor.
     * @param aJniEnv JNIEnv pointer for the GUI thread.
     * @param aPeer Local ref to the peer jobject that signals are forwarded to
     * @param aParent Source QObject of the signal
     * @param aSignalId Signal ID that will be used to identify the signal in Java code
     */
    SlotCallback( JNIEnv* aJniEnv,
                  jobject aPeer,
                  QObject* aParent,
                  const int& aSignalId );
    virtual ~SlotCallback();

private:
    SlotCallback();
    void callJava(const int& a1 = 0, const int& a2 = 0, const int& a3 = 0,
            const int& a4 = 0, const int& a5 = 0, const jstring aString = NULL);

public slots:
    void widgetSignal();
    void widgetSignal( int aInt );
    void widgetSignal( int aInt1, int aInt2 );
    void widgetSignal( int aInt1, int aInt2, int aInt3 );
    void widgetSignal( int aInt1, int aInt2, int aInt3, int aInt4 );
    void widgetSignal( int aInt1, int aInt2, int aInt3, int aInt4, int aInt5 );
    void widgetSignal( bool aBoolean );
    void widgetSignal( const QPoint& aPoint );
    void widgetSignal( const QString& aString );
    void widgetSignal( const QUrl& aString );
	void widgetSignal( const QItemSelection& aSelected, const QItemSelection& aDeSelected );
	void widgetSignal( QListWidgetItem* aItem );
    void widgetSignal( const QDateTime& aDateTime );
#ifndef QT_NO_SYSTEMTRAYICON
    void widgetSignal( const QSystemTrayIcon::ActivationReason );
#endif
    void widgetSignal( QTreeWidgetItem* aItem, int aColumn );
    void widgetSignal( QTreeWidgetItem* aItem );
    void widgetSignal( const QModelIndex& index );
    void widgetSignal( QWidget* aWidget1, QWidget* aWidget2);
    void widgetSignal(const QVariant& value);
private:
    JNIEnv*         mJniEnv;
    jobject         mPeer;
    jmethodID       mJmethod;
    const int       mSignalId;
    };

}}

#endif // SWT_SLOTCALLBACK_H

