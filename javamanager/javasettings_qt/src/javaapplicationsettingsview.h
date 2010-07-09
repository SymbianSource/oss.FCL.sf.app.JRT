/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

namespace java
{
namespace settings
{

/**
 * Realization of the p-impl pattern: settings view consists of a public part (
 * implementing the ApplicationSettingsView interface) and the private part
 * (containing the private details). The public part simply maintains a pointer
 * to the private part thus enabling further changes into the private part without
 * any risk of breaking the public part
 */
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
    Q_PRIVATE_SLOT(d_func(), void _q_settingsChanged(const QString & aNewValue))
    Q_PRIVATE_SLOT(d_func(), void _q_settingsChanged(bool aNewValue))
    Q_PRIVATE_SLOT(d_func(), void _q_dataItemDisplayed(const QModelIndex aDataItemIndex))

private:
    // the pointer to the private counterpart
    JavaApplicationSettingsViewPrivate * d_ptr;
};

} // namespace settings
} // namespace java


#endif // JAVAAPPLICATIONSETTINGSVIEW_H
