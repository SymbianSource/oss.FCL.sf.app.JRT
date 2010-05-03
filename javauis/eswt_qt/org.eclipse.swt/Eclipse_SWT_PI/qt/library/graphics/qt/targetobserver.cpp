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
#include "targetobserver.h"
#include "gfxlog.h"

namespace Java { namespace GFX {

TargetObserver::TargetObserver()
{
    mActive = false;
    mTarget = NULL;
}

TargetObserver::~TargetObserver()
{
    if(mActive)
    {
        disconnectTarget();
    }
    mGC = NULL;
}

void TargetObserver::connectTarget(GraphicsContext* aGc, QObject* aTarget)
{
    GFX_LOG_FUNC_CALL();
    mGC = aGc;
    connect(aTarget, SIGNAL(destroyed( QObject* )), SLOT(widgetDestroyed()), Qt::DirectConnection);
    mActive = true;
    mTarget = aTarget;
}

void TargetObserver::disconnectTarget()
{
    GFX_LOG_FUNC_CALL();
    mGC = NULL;
    disconnect(mTarget, SIGNAL(destroyed()), 0, 0);
    mActive = false;
}

bool TargetObserver::isObserving()
{
    GFX_LOG_FUNC_CALL();
    return mActive;
}

void TargetObserver::widgetDestroyed()
{
    GFX_LOG_FUNC_CALL();
    if(mActive) {
        mGC->releaseTarget();
    } else {
        // not active anymore so no need to call releaseTarget
    }
}

} // namespace GFX
} // namespace Java

