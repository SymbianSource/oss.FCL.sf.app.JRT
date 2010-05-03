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

//#include <QtOpenGL>
#include <QWidget>
#include <QPainter>
#include <QPaintEngine>
#include "windowsurfaceimpl.h"
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
#ifdef __SYMBIAN32__
    if(!mMainSurface.localSurfaceInUse)
    {
        QRegion region(aX, aY, aWidth, aHeight);
        mMainSurface.qSurface->beginPaint(region);
        // In case local surface was used last round
        // and we now have Qt's window surface again
        // delete the local surface to save memory
        if(mMainSurface.localSurface != NULL)
        {
            delete mMainSurface.localSurface;
            mMainSurface.localSurface = NULL;
        }
    }
#endif
    mPaintingStarted = true;
}

void WindowSurfaceImpl::endPaint()
{
#ifdef __SYMBIAN32__
    if(!mMainSurface.localSurfaceInUse)
    {
        mMainSurface.qSurface->endPaint(QRegion());
    }
#endif
    mPaintingStarted = false;
}

void WindowSurfaceImpl::flush()
{
#ifdef __SYMBIAN32__
    if(!mMainSurface.localSurfaceInUse)
    { 
        QRegion region(0,0,mMainSurface.device->width(), mMainSurface.device->height());  
        mMainSurface.qSurface->flush(mMainSurface.widget, region, QPoint());
    }
#endif
}

void WindowSurfaceImpl::bind(int aCapabilies)
{
    GFX_LOG_FUNC_CALL();
    // Bind is not allowed if beginPaint has not been called
    if(!mPaintingStarted)
    {
        throw GfxException(EGfxErrorIllegalState, "beginPaint() not called before bind()");
    }
    
    switch (mMainSurface.type)
    {
        case WsTypeQtImage:
        {
            mBufferedRendering = true;
            break;
        }
        
        case WsTypeEglSurface:
        {
            // If caller does not support EGL surface
            // create temp buffer to be used as target and
            // copy pixels from window surface to temp buffer
            if ((aCapabilies & WsTypeEglSurface) == 0) {
                // TODO copy pixels from EGL surface to 
                // QImage created here
                
            } else {
                // Caller supports EGL surface, so just 
                // save EGL state here
                saveEglState();
            }
            break;
        }
        case WsTypeSymbianBitmap:
        {
            break;
        }
        default:
        {
            // we should newer get here
        }
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
    return mMainSurface.device;
}

#ifdef __SYMBIAN32__
EGLSurface WindowSurfaceImpl::getEglSurface()
{
    return mEgl.drawSurface;
}

EGLenum WindowSurfaceImpl::getEglApi()
{
    return mEgl.api;
}

CFbsBitmap* WindowSurfaceImpl::getSymbianBitmap()
{
    return mMainSurface.symbianBitmap;
}
#endif

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

    if (mBufferedRendering)
    {
        // TODO draw QImage with painter to actual target
    }
    else
    {
        if(mMainSurface.type == WsTypeEglSurface)
        {
            restoreEglState();
        }
        // pixels are already rendered to the target
        // no post blitter hook needed
    }
    mIsBound = false;
}

void WindowSurfaceImpl::dispose()
{
    GFX_LOG_FUNC_CALL();
    delete this;
}

void WindowSurfaceImpl::saveEglState()
{
#ifdef __SYMBIAN32__
    // Some painter needs to be active on the device
    // in order to get correct data from EGL, so if 
    // there's no active painter, activate one here
    if(!mMainSurface.device->paintingActive())
    {
        mPainter.begin(mMainSurface.device);
    }
    mEgl.drawSurface = eglGetCurrentSurface(EGL_DRAW);
    mEgl.readSurface = eglGetCurrentSurface(EGL_READ);
    mEgl.context = eglGetCurrentContext();
    mEgl.api = eglQueryAPI();
#endif
}
    
void WindowSurfaceImpl::restoreEglState()
{
#ifdef __SYMBIAN32__
    eglBindAPI(mEgl.api);
    eglMakeCurrent(EGL_DEFAULT_DISPLAY, mEgl.drawSurface, mEgl.readSurface, mEgl.context);
    mEgl.drawSurface = EGL_NO_SURFACE;
    mEgl.readSurface = EGL_NO_SURFACE;
    mEgl.context = EGL_NO_CONTEXT;
    
    // if owned painter was activate to the device 
    // release it
    if(mPainter.isActive())
    {
        mPainter.end();
    }
#endif
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
#ifdef __SYMBIAN32__
    // If painting is active, i.e. beginPaint has been called 
    // surface data is not updated
    if(mPaintingStarted)
    {
        return;
    }   
    QWindowSurface* surface = mMainSurface.widget->windowSurface();
    
    // If window surface is null it means that the widget has been 
    // sent to background and widget's window surface has been deleted, 
    // in such case create own QImage as local surface in order to support 
    // rendering in background
    if(surface == NULL || surface == 0)
    {
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
    else 
    {
        // We got Qt's window surface, so in case we had local surface in use
        // delete it as it's not used anymore
        if(mMainSurface.localSurfaceInUse)
        {
            deleteLocalSurface();
        }
    }
    
    // We got window surface so extract information
    QPaintDevice* device = surface->paintDevice();
    if(device->paintingActive())
    {
        throw GfxException(EGfxErrorIllegalState, "Internal error: Device active when refreshing data");  
    }
    
    // Attach painter to device in oder to see which type 
    // of device it is working on
    mPainter.begin(device);
    QPaintEngine* engine = mPainter.paintEngine();
    
    // determine the surface type based on the engine used
    // as Qt does not provide exact info of the surface type
    switch (engine->type())
    {
        case QPaintEngine::OpenVG:
            // surface is EGL window surface
            mMainSurface.type = WsTypeEglSurface;
            break;
        case QPaintEngine::Raster:
            mMainSurface.type = WsTypeSymbianBitmap;
            if(device->devType() == QInternal::Pixmap)
            {
                QPixmap* pixmap = static_cast<QPixmap*>(device);
                mMainSurface.symbianBitmap = pixmap->toSymbianCFbsBitmap();
            }
            else 
            {
                throw GfxException(EGfxErrorIllegalArgument, "Unsupported device type");
            }
            break;
        default:
            throw GfxException(EGfxErrorIllegalArgument, "Unsupported widget window surface type");
            break;
    }
    
    // release painter
    mPainter.end();
    mMainSurface.qSurface = surface;
    mMainSurface.device = device;
    mMainSurface.localSurfaceInUse = false;
#endif
}


} // namespace GFX
} // namespace Java
