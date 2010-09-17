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

#ifndef JAVASOFTINDICATORPLUGIN_H
#define JAVASOFTINDICATORPLUGIN_H

#include <QObject>
#include <QStringList>
#include <hbindicatorplugininterface.h>

/**
 * Main class of java indicator plugin. 
 */
class JavaSoftIndicatorPlugin : public QObject,
        public HbIndicatorPluginInterface
    {
    Q_OBJECT
    Q_INTERFACES(HbIndicatorPluginInterface)

public:
    /**
     * Constructor
     */
    JavaSoftIndicatorPlugin();

    /**
     * Destructor
     */
    virtual ~JavaSoftIndicatorPlugin();

    /**
     * @see HbIndicatorPluginInterface
     */
    QStringList indicatorTypes() const;

    /**
     * @see HbIndicatorPluginInterface
     */
    bool accessAllowed(const QString &indicatorType,
            const QVariantMap &securityInfo) const;
    
    /**
     * @see HbIndicatorPluginInterface
     */
    HbIndicatorInterface* createIndicator(const QString &indicatorType);

    /**
     * @see HbIndicatorPluginInterface
     */
    int error() const;

private:
    Q_DISABLE_COPY( JavaSoftIndicatorPlugin)

    /**
     * Error value
     */
    int iError;

    /**
     * Indicator types 
     * supported indicator types. 
     */
    //QStringList iIndicatorTypes;
    };

#endif // JAVASOFTINDICATORPLUGIN_H
