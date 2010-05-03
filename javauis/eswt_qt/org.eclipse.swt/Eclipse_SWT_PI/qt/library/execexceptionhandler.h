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
#ifndef _EXECEXCEPTIONHANDLER_H_
#define _EXECEXCEPTIONHANDLER_H_

#include <QDialog>
#include <QMenu>

namespace Java { namespace eSWT {

/**
 * Abstract base class for event loop termination handling. Subclass instances
 * of these can  be used when exec() has been called and needs to be forced to
 * terminate. This is needed e.g. in the case of Java exceptions during exec().
 */
class ExecExceptionHandler
{
protected:
    ExecExceptionHandler(QObject* aObject) : mExecutor( aObject ), mTerminated( false ) {}
public:
    virtual ~ExecExceptionHandler() {}
    /**
     * Returns the object that exec() has been called on. E.g. a QMenu or
     * a QDialog.
     * @return QObject pointer
     */
    QObject* executor() { return mExecutor; }

    /**
     * Terminates the exec event loop.
     */
    void terminate() { doTerminate(); mTerminated = true; }

    /**
     * Query if terminate() has already been called.
     * @return true if already terminated.
     */
    bool isTerminated() { return mTerminated; }
private:
    ExecExceptionHandler() {}
protected:
    virtual void doTerminate() = 0;
    QObject* mExecutor;
    bool mTerminated;
};

/**
 * Handles how to terminate QDialog::exec().
 */
class DialogExecExceptionHandler : public ExecExceptionHandler
{
public:
    DialogExecExceptionHandler(QDialog* aObject) : ExecExceptionHandler( aObject ) {}
    void doTerminate()
    {
        QDialog* dlg = qobject_cast<QDialog*>(mExecutor);
        if(dlg) dlg->reject();
    }
};

/**
 * Handles how to terminate QMenu::exec().
 */
class MenuExecExceptionHandler : public ExecExceptionHandler
{
public:
    MenuExecExceptionHandler(QMenu* aObject) : ExecExceptionHandler( aObject ) {}
    void doTerminate()
    {
        QMenu* menu = qobject_cast<QMenu*>(mExecutor);
        if(menu) menu->close();
    }
};

}}

#endif //_EXECEXCEPTIONHANDLER_H_
