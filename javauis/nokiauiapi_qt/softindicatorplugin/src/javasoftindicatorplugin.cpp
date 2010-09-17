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

#include <QtPlugin>

#include "javasoftindicatorplugin.h"
#include "javasoftindicator.h"


Q_EXPORT_PLUGIN( JavaSoftIndicatorPlugin)

// ----------------------------------------------------------------------------
// JavaSoftIndicatorPlugin::JavaSoftIndicatorPlugin
// ----------------------------------------------------------------------------
JavaSoftIndicatorPlugin::JavaSoftIndicatorPlugin() :
    iError(0)
    {
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicatorPlugin::~JavaSoftIndicatorPlugin
// ----------------------------------------------------------------------------
JavaSoftIndicatorPlugin::~JavaSoftIndicatorPlugin()
    {
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicatorPlugin::indicatorTypes
// Return notification types this plugin implements
// ----------------------------------------------------------------------------
QStringList JavaSoftIndicatorPlugin::indicatorTypes() const
    {
    QStringList iIndicatorTypes;
    iIndicatorTypes
            << "com.nokia.javasoftnotification.indicatorplugin/1.0_269487546_203";
    iIndicatorTypes
            << "com.nokia.javasoftnotification.indicatorplugin/1.0_269487546_202";
    iIndicatorTypes
                << "com.nokia.javasoftnotification.indicatorplugin/1.0_269487546_201";
    iIndicatorTypes
                << "com.nokia.javasoftnotification.indicatorplugin/1.0_270212593_203";
        iIndicatorTypes
                << "com.nokia.javasoftnotification.indicatorplugin/1.0_270212593_202";
       
    return iIndicatorTypes;
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicatorPlugin::accessAllowed
// Check if client is allowed to use notification widget
// ----------------------------------------------------------------------------
bool JavaSoftIndicatorPlugin::accessAllowed(const QString &indicatorType,
        const QVariantMap &securityInfo) const
    {
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)
    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicatorPlugin::createIndicator
// ----------------------------------------------------------------------------
HbIndicatorInterface* JavaSoftIndicatorPlugin::createIndicator(
        const QString &indicatorType)
    {
    HbIndicatorInterface *indicator = new JavaSoftIndicator(indicatorType);
    return indicator;
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicatorPlugin::error
// ----------------------------------------------------------------------------
int JavaSoftIndicatorPlugin::error() const
    {
    return iError;
    }

