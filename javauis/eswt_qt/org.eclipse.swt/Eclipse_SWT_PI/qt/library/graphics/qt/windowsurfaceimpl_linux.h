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
    virtual QImage* getQtImage();
    virtual void refresh();
    virtual void release();
    virtual void dispose();
    virtual void handleSymbianWindowVisibilityChange(bool aVisible);

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

    struct surfaceData
    {
        QWindowSurface* qSurface;
        QPaintDevice* device;
        QWidget* widget;
        QImage* localSurface;
        WindowSurfaceType type;  
        bool localSurfaceInUse;
        surfaceData() : qSurface(NULL),
                        device(NULL), 
                        widget(NULL),
                        localSurface(NULL),
                        type(WsTypeUnknown),
                        localSurfaceInUse(false)
        {}
    };

    
    surfaceData mMainSurface;
    QPainter mPainter;
    bool mIsBound;
    bool mBufferedRendering;
    bool mAutoRefresh;
    bool mPaintingStarted;
};

} // namespace GFX
} // namespace Java

#endif /*WINDOWSURFACEQT_H_*/
