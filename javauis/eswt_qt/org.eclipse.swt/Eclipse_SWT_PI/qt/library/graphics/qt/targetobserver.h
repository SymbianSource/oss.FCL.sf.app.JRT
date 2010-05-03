/*******************************************************************************
 * Copyright (c) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial API and implementation
 *******************************************************************************/
#ifndef TARGETOBSERVER_H_
#define TARGETOBSERVER_H_

#include <QObject>
#include "graphics.h"

namespace Java { namespace GFX {

class TargetObserver: public QObject
{
Q_OBJECT

public:
    /**
     * Ctor
     */
    TargetObserver();

    /**
     * Dtor
     */
    ~TargetObserver();

    /**
     * Creates a connection to listen targets state
     *
     * @param aGc The calling GraphicsContext about to bind the <code>aTarget</code>
     * @param aTarget The object to monitor
     */
    void connectTarget(GraphicsContext* aGc, QObject* aTarget);

    /**
     * Disconnects monitoring
     */
    void disconnectTarget();

    /**
     * Checks the state of this observer
     * @return True if this instance is monitoring an object, otherwise false
     */
    bool isObserving();

public slots:

    /**
     * Callback slot for widget destroyed signal
     */
    void widgetDestroyed();

private:
    GraphicsContext* mGC;
    QObject* mTarget;
    bool mActive;
};

} // namespace GFX
} // namespace Java

#endif // TARGETOBSERVER_H


