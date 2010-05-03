/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef JAVAAPPLICATIONSETTINGSVIEW_H
#define JAVAAPPLICATIONSETTINGSVIEW_H

#include "applicationsettingsview.h"

class JavaApplicationSettingsViewPrivate;

class JavaApplicationSettingsView: public ApplicationSettingsView
{
    Q_OBJECT
    Q_INTERFACES(ApplicationSettingsView)

public:
    explicit JavaApplicationSettingsView();
    virtual ~JavaApplicationSettingsView();

public slots:
    void initialize(const QString&);

signals:
    void aboutToClose();

private:
    Q_DISABLE_COPY(JavaApplicationSettingsView)
    Q_DECLARE_PRIVATE_D(d_ptr, JavaApplicationSettingsView)
    Q_PRIVATE_SLOT(d_func(), void _q_settingsChanged(const QString &newValue))
    Q_PRIVATE_SLOT(d_func(), void _q_dataItemDisplayed(const QModelIndex))

private:
    JavaApplicationSettingsViewPrivate * d_ptr;
};


#endif // JAVAAPPLICATIONSETTINGSVIEW_H
