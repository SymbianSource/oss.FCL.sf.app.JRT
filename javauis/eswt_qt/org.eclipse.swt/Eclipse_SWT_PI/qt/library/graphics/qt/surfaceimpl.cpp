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
#include <QMainWindow>
#include "surfaceimpl.h"
#include "gfxlog.h"

namespace Java { namespace GFX {

SurfaceImpl::SurfaceImpl()
{
    GFX_LOG_FUNC_CALL();
    mTargetType = ETypeNone;
    mImageType = ENone;
    mTarget = NULL;
}

SurfaceImpl::~SurfaceImpl()
{
    GFX_LOG_FUNC_CALL();
    mTarget = NULL;
}

void SurfaceImpl::clearTarget()
{
    GFX_LOG_FUNC_CALL();
    mTarget = NULL;
    mTargetType = ETypeNone;
    mImageType = ENone;
}

QPaintDevice* SurfaceImpl::getBindable()
{
    GFX_LOG_FUNC_CALL();
    return static_cast<QPaintDevice*>(mTarget);
}

QPaintDevice* SurfaceImpl::getBufferFlushTarget()
{
    return mBufferFlushTarget;
}

int SurfaceImpl::getHeight()
{
    GFX_LOG_FUNC_CALL();
    return mTarget->height();
}

QImage* SurfaceImpl::getImage()
{
    GFX_LOG_FUNC_CALL();
    if((mTargetType == ETypeImage) && (mImageType == EImage))
    {
        return (QImage*)mTarget;
    }
    return NULL;
}

TImageType SurfaceImpl::getImageType()
{
    // Returns ENone in case of not image
    return mImageType;
}

TTargetType SurfaceImpl::getType()
{
    GFX_LOG_FUNC_CALL();
    return mTargetType;
}

QPixmap* SurfaceImpl::getPixmap()
{
    GFX_LOG_FUNC_CALL();
    if((mTargetType == ETypeImage) && (mImageType == EPixmap))
    {
        return (QPixmap*)mTarget;
    }
    return NULL;
}

QWidget* SurfaceImpl::getWidget()
{
    GFX_LOG_FUNC_CALL();
    if(mTargetType == ETypeWidget)
    {
        return (QWidget*)mTarget;
    }
    return NULL;
}

int SurfaceImpl::getWidth()
{
    GFX_LOG_FUNC_CALL();
    return mTarget->width();
}

void SurfaceImpl::setTarget(int aTarget, TTargetType aType)
{
    GFX_LOG_FUNC_CALL();

    // reset ImageType
    mImageType = ENone;

    switch (aType)
    {
        case ETypeWidget:
        {
            mTarget = static_cast<QWidget*>(reinterpret_cast<QObject*>(aTarget));
            break;
        }
        case ETypeImage:
        {
            // Handling for both QImage and QPixmap
            Image* image = reinterpret_cast<Image*>(aTarget);
            mTarget = image->getBindable();
            // Store native image type
            mImageType = image->type();
            break;
        }
        case ETypeBuffer:
        {
            Buffer* buffer = reinterpret_cast<Buffer*>(aTarget);
            mTarget = buffer->getBindable();
            break;
        }
        case ETypeWindowSurface:
        {
            WindowSurface* surface = reinterpret_cast<WindowSurface*>(aTarget);
            mTarget = surface->getDevice();
            break;
        }
        default:
        {
            Q_ASSERT(false && "TargetType not recognized");
        }
    }
    mTargetType = aType;
}

void SurfaceImpl::setBufferFlushTarget(QPaintDevice* aBufferFlushTarget)
    {
    mBufferFlushTarget = aBufferFlushTarget;
    }

} // namespace GFX
} // namespace Java
