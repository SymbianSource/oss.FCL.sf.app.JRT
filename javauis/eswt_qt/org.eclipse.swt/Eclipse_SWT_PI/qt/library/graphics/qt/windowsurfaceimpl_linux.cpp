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

#include <QWidget>
#include <QPainter>
#include <QPaintEngine>
#include "windowsurfaceimpl_linux.h"
#include "gfxlog.h"

namespace Java { namespace GFX {

WindowSurfaceImpl::WindowSurfaceImpl(QPaintDevice* aSurface, WindowSurfaceType aType)
: mIsBound(false),
  mBufferedRendering(false),
  mAutoRefresh(false),
  mPaintingStarted(false)
{
    GFX_LOG_FUNC_CALL();
    mMainSurface.device = aSurface;
    mMainSurface.type = aType;
}

WindowSurfaceImpl::WindowSurfaceImpl(QWidget* aWidget, bool aAutoRefresh)
: mIsBound(false),
  mBufferedRendering(false),
  mAutoRefresh(false),
  mPaintingStarted(false)
{
    GFX_LOG_FUNC_CALL();
    if(aWidget == NULL) 
    {
        throw GfxException(EGfxErrorIllegalArgument, "Target widget is NULL");
    }
    mMainSurface.widget = aWidget;
    mAutoRefresh = aAutoRefresh;
    updateSurfaceData();   
}


WindowSurfaceImpl::~WindowSurfaceImpl()
{
    GFX_LOG_FUNC_CALL();
    if(mMainSurface.localSurface != NULL)
    {
        delete mMainSurface.localSurface;
        mMainSurface.localSurface = NULL;
    }
}

void WindowSurfaceImpl::beginPaint(int aX, int aY, int aWidth, int aHeight)
{
    if(mAutoRefresh && (mMainSurface.widget != NULL)) 
    {
        updateSurfaceData();
    }
    mPaintingStarted = true;
}

void WindowSurfaceImpl::endPaint()
{
    // nothing to do
}

void WindowSurfaceImpl::flush()
{
    // nothing to do
}

void WindowSurfaceImpl::bind(int aCapabilies)
{
    GFX_LOG_FUNC_CALL();
    // Bind is not allowed if beginPaint has not been called
    if(!mPaintingStarted)
    {
        throw GfxException(EGfxErrorIllegalState, "beginPaint() not called before bind()");
    }
    mIsBound = true;
}

int WindowSurfaceImpl::getType()
{
    GFX_LOG_FUNC_CALL();
    return mMainSurface.type;
}

QPaintDevice* WindowSurfaceImpl::getDevice()
{
    if(mMainSurface.localSurfaceInUse) 
    {
        return mMainSurface.localSurface;
    }
    else 
    {
        return mMainSurface.device;
    }
}

QImage* WindowSurfaceImpl::getQtImage()
{
    return mMainSurface.localSurface;
}

void WindowSurfaceImpl::refresh()
{
    updateSurfaceData();
}

void WindowSurfaceImpl::release()
{
    GFX_LOG_FUNC_CALL();
    if (!mIsBound)
    {
        return;
    }
    // nothing to do
    mIsBound = false;
}

void WindowSurfaceImpl::dispose()
{
    GFX_LOG_FUNC_CALL();
    delete this;
}

void WindowSurfaceImpl::handleSymbianWindowVisibilityChange(bool /*aVisible*/)
{
    // Do nothing
}

// Private methods
void WindowSurfaceImpl::createLocalSurface(int aWidth, int aHeight)
{
    mMainSurface.localSurface = new QImage(aWidth, aHeight, QImage::Format_ARGB32);
    if(mMainSurface.localSurface->isNull()) 
    {   
        throw GfxException(EGfxErrorNoMemory, "Local Surface creation failed");
    }   
}

void WindowSurfaceImpl::deleteLocalSurface()
{
    if(mMainSurface.localSurface != NULL)
    {
        delete mMainSurface.localSurface;
        mMainSurface.localSurface = NULL;
    }
    
}

bool WindowSurfaceImpl::isLocalSurfaceValid()
{
    if(mMainSurface.localSurface == NULL)
    {
        return false;
    }
    
    if((mMainSurface.localSurfaceInUse) && 
       (mMainSurface.localSurface->width() == mMainSurface.widget->width()) && 
       (mMainSurface.localSurface->height() == mMainSurface.widget->height()))
    {
        return true;
    }
    return false;
}

void WindowSurfaceImpl::updateSurfaceData()
{   
    // If painting is active, i.e. beginPaint has been called 
    // surface data is not updated
    if(mPaintingStarted)
    {
        return;
    }   
    // check if we already have local surface with valid size
    if(!isLocalSurfaceValid()) 
    {
    	// incase we have invalid surface delete the current one
        // and create new
        if(mMainSurface.localSurfaceInUse) 
        {
            deleteLocalSurface();
        }
        createLocalSurface(mMainSurface.widget->width(), mMainSurface.widget->height());
        // set info
        mMainSurface.qSurface = NULL;
        mMainSurface.device = mMainSurface.localSurface;
        mMainSurface.type = WsTypeQtImage;
        mMainSurface.localSurfaceInUse = true;
        return;
    }
    else 
    {
        // We have valid local surface so just return
        return;
    }
}


} // namespace GFX
} // namespace Java
