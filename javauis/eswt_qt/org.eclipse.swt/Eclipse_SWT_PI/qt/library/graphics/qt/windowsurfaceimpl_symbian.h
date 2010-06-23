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
#ifndef WINDOWSURFACEQT_H_
#define WINDOWSURFACEQT_H_

#include "graphics.h"
#include <QtGui/private/qwindowsurface_p.h>

namespace Java { namespace GFX {

class WindowSurfaceImpl: public WindowSurface
{
public:
   /**
    * Ctor for paint device
    * @param surface The paindevice of Qt render surface
    * @param type The type of the surface 
    */
    WindowSurfaceImpl(QPaintDevice* aSurface, WindowSurfaceType aType);
    
    /**
     * Ctor for window owning widgets
     * @param aWidget The target widget that this window surface is created on
     * @param aAutoRefresh If true the Qt window surface pointer is updated each time
     *                     the bind is called, otherwise not 
     */
    WindowSurfaceImpl(QWidget* aWidget, bool aAutoRefresh);

   /**
    * Dtor
    */
    virtual ~WindowSurfaceImpl();
    
    /**
     * From WindowSurface
     * @see WindowSurface
     */
    virtual void bind(int aCapabilies = WsTypeQtImage) ;
    virtual void beginPaint(int aX, int aY, int aWidth, int aHeight);
    virtual void endPaint();
    virtual void flush();
    virtual int getType();
    virtual QPaintDevice* getDevice();
    virtual EGLSurface getEglSurface();
    virtual EGLenum getEglApi();
    virtual QImage* getQtImage();
    virtual void refresh();
    virtual void release();
    virtual void dispose();
    virtual CFbsBitmap* getSymbianBitmap();

private:
    
    /**
     * Creates and activates local off-screen buffer for rendering 
     * in background mode
     */
    void createLocalSurface(int aWidth, int aHeight);
    
    /**
     * Deletes local surface
     */
    void deleteLocalSurface();
    
    /**
     * Checks that local surface size is correct
     */
    bool isLocalSurfaceValid();
    
    /**
     * Updates window surface info, i.e. fetches info from Qt
     */
    void updateSurfaceData();
    
    /**
     * Saves current EGL state
     */
    void saveEglState();
    
    /**
     * Restores previously saved EGL state
     */
    void restoreEglState();
    
 
    struct surfaceData
    {
        // The window surface that Qt has created
        QWindowSurface* qSurface;
        // The paint device QWindowSurface has
        QPaintDevice* device;
        // The Target widget which owns the QWindowSurface
        QWidget* widget;
        // Local surface used when widget is in background
        // and QWindowSurface is not available
        QImage* localSurface;
        // The symbian bitmap used in a case where
        // the widget is parially visible and Qt switches 
        // to software rendering
        CFbsBitmap* symbianBitmap;
        // The current type of the target surface
        WindowSurfaceType type;
        // Indicates if local surface is used
        bool localSurfaceInUse;
        surfaceData() : qSurface(NULL),
                        device(NULL), 
                        widget(NULL),
                        localSurface(NULL),
                        symbianBitmap(NULL),
                        type(WsTypeUnknown),
                        localSurfaceInUse(false)
        {}
    };
    
    /**
     * Data collection for EGL surface
     */
    struct eglData
    {
        EGLSurface drawSurface;
        EGLSurface readSurface;
        EGLContext context;
        EGLenum api;
        EGLDisplay display;
        eglData() : drawSurface(EGL_NO_SURFACE),
                    readSurface(EGL_NO_SURFACE),
                    context(EGL_NO_CONTEXT),
                    api(EGL_OPENGL_ES_API),
                    display(EGL_DEFAULT_DISPLAY)
        {}
    };
    
    surfaceData mMainSurface;
    eglData mEgl;
    QPainter mPainter;
    bool mIsBound;
    bool mBufferedRendering;
    bool mAutoRefresh;
    bool mPaintingStarted;
};

} // namespace GFX
} // namespace Java

#endif /*WINDOWSURFACEQT_H_*/
