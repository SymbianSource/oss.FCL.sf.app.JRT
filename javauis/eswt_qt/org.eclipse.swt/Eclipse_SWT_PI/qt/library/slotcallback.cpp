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

#include <QPoint>
#include <QString>
#include <QUrl>
#include <QModelIndex>
#ifdef __SYMBIAN32__
#include <cntservicescontact.h>
#endif

#include "QItemSelection"
#include "QListWidgetItem"

#include "slotcallback.h"
#include "swt.h"
#include "jniutils.h"
#include "swtapplication.h"

using namespace Java::eSWT;

SlotCallback::SlotCallback(JNIEnv* aJniEnv, jobject aPeer, QObject* aParent, const int& aSignalId)
      : QObject(aParent),
        mJniEnv(aJniEnv),
        mPeer(NULL),
        mJmethod(NULL),
        mSignalId(aSignalId)
{
    SWT_LOG_FUNC_CALL();

    // If Java peer is not Display then jobject ref and methodID are needed.
    // Otherwise they are managed by JniUtils.
    if (!swtApp->jniUtils().isDisplay(aPeer))
    {
        mPeer = aJniEnv->NewGlobalRef(aPeer);
        if(!mPeer)
        {
            throw std::bad_alloc();
        }
        mJmethod = swtApp->jniUtils().FindJavaMethodID(aJniEnv, mPeer, "eventProcess",
                "(IIIIIIIILjava/lang/String;)Z");
        if(!mJmethod)
        {
            throw std::bad_alloc();
        }
    }
}

SlotCallback::~SlotCallback()
{
    SWT_LOG_FUNC_CALL();
    if (mPeer)
    {
        mJniEnv->DeleteGlobalRef(mPeer);
    }
}

void SlotCallback::callJava(const int& a1, const int& a2, const int& a3,
        const int& a4, const int& a5, const jstring aString)
{
    JniUtils& jniUtils = swtApp->jniUtils();
    if (mPeer)
    {
        // Call non-Display peer using our own jobject ref and jmethodID
        jniUtils.eventProcess(mPeer, mJmethod, reinterpret_cast<int>(parent()), mSignalId, a1, a2, a3, a4, a5, aString);
    }
    else
    {
        // Call Display peer, JniUtils manages the jobject ref and jmethodID
        jniUtils.eventProcess(parent(), mSignalId, a1, a2, a3, a4, a5, aString);
    }
}

void SlotCallback::widgetSignal()
{
    SWT_LOG_FUNC_CALL();
    callJava();
}

void SlotCallback::widgetSignal(int aInt)
{
    SWT_LOG_FUNC_CALL();
    callJava(aInt);
}
#ifndef QT_NO_SYSTEMTRAYICON
void SlotCallback::widgetSignal(QSystemTrayIcon::ActivationReason aReason)
{
    SWT_LOG_FUNC_CALL();
    callJava(aReason);
}
#endif
void SlotCallback::widgetSignal(int aInt1, int aInt2)
{
    SWT_LOG_FUNC_CALL();
    callJava(aInt1, aInt2);
}

void SlotCallback::widgetSignal(int aInt1, int aInt2, int aInt3)
{
    SWT_LOG_FUNC_CALL();
    callJava(aInt1, aInt2, aInt3);
}

void SlotCallback::widgetSignal(int aInt1, int aInt2, int aInt3, int aInt4)
{
    SWT_LOG_FUNC_CALL();
    callJava(aInt1, aInt2, aInt3, aInt4);
}

void SlotCallback::widgetSignal(int aInt1, int aInt2, int aInt3, int aInt4,
        int aInt5)
{
    SWT_LOG_FUNC_CALL();
    callJava(aInt1, aInt2, aInt3, aInt4, aInt5);
}

void SlotCallback::widgetSignal(bool aBoolean)
{
    SWT_LOG_FUNC_CALL();
    callJava(aBoolean ? 1 : 0);
}

void SlotCallback::widgetSignal(const QPoint& aPoint)
{
    SWT_LOG_FUNC_CALL();
    callJava(aPoint.x(), aPoint.y());
}

void SlotCallback::widgetSignal(const QString& aString)
{
    SWT_LOG_FUNC_CALL();
    callJava(0, 0, 0, 0, 0, swtApp->jniUtils().QStringToJavaString(mJniEnv, aString));
}

void SlotCallback::widgetSignal(const QUrl& aUrl)
{
    widgetSignal(aUrl.toString());
}

void SlotCallback::widgetSignal(const QDateTime& /*aDateTime*/)
{
    // QDateTime parameter is not needed, just ignore it
    widgetSignal();
}

void SlotCallback::widgetSignal(const QItemSelection& aSelected,
        const QItemSelection& aDeSelected)
{
    SWT_LOG_FUNC_CALL();
    const jint selectionHandle = reinterpret_cast<jint> (&aSelected);
    const jint deSelectionHandle = reinterpret_cast<jint> (&aDeSelected);
    callJava(selectionHandle, deSelectionHandle);
}

void SlotCallback::widgetSignal(QListWidgetItem* aSelected)
{
    SWT_LOG_FUNC_CALL();
    jint selectionHandle = reinterpret_cast<jint> (aSelected);
    callJava(selectionHandle);
}

void SlotCallback::widgetSignal(QTreeWidgetItem* aSelected, int aColumn)
{
    SWT_LOG_FUNC_CALL();
    jint selectionHandle = reinterpret_cast<jint> (aSelected);
    callJava(selectionHandle, aColumn);
}

void SlotCallback::widgetSignal(QTreeWidgetItem* aSelected)
{
    SWT_LOG_FUNC_CALL();
    jint selectionHandle = reinterpret_cast<jint> (aSelected);
    callJava(selectionHandle);
}

void SlotCallback::widgetSignal( const QModelIndex& index )
{
    SWT_LOG_FUNC_CALL();
    callJava(index.row(), index.column());
}

void SlotCallback::widgetSignal( QWidget* aWidget1, QWidget* aWidget2 )
{
    SWT_LOG_FUNC_CALL();
    callJava(reinterpret_cast<jint>(aWidget1), reinterpret_cast<jint>(aWidget2));
}

void SlotCallback::widgetSignal(const QVariant& value)
{
    SWT_LOG_FUNC_CALL();
    callJava(reinterpret_cast<jint>(&value));
}

#ifdef __SYMBIAN32__
Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)
#endif

