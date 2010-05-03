/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial API and implementation
 *******************************************************************************/

#include "graphics.h"
#include "bufferimpl.h"
#include "graphicscontextimpl.h"
#include "pixmap.h"
#include "imageloaderimpl.h"
#include "imagedataimpl.h"
#include "palettedataimpl.h"
#include "windowsurfaceimpl.h"
#include "gfxlog.h"

namespace Java { namespace GFX {

/*static*/ WindowSurface* GraphicsFactory::createWindowSurface(QPaintDevice* aSurface, WindowSurfaceType aType)
{
   GFX_LOG_FUNC_CALL();
   WindowSurfaceImpl* wsurf = new WindowSurfaceImpl(aSurface, aType);
   return static_cast<WindowSurface*>(wsurf);
}

/*static*/ WindowSurface* GraphicsFactory::createWindowSurface(QWidget* aWidget, bool aAutoRefresh)
{
   GFX_LOG_FUNC_CALL();
   WindowSurfaceImpl* wsurf = new WindowSurfaceImpl(aWidget, aAutoRefresh);
   return static_cast<WindowSurface*>(wsurf);
}

/*static*/ Buffer* GraphicsFactory::createBuffer()
{
    GFX_LOG_FUNC_CALL();
    BufferImpl* buffer = new BufferImpl();
    return static_cast<Buffer*>(buffer);
}

/*static*/ GraphicsContext* GraphicsFactory::createGraphicsContext()
{
    GFX_LOG_FUNC_CALL();
    GraphicsContextImpl* gc = new GraphicsContextImpl();
    return static_cast<GraphicsContext*>(gc);
}

/*static*/ Image* GraphicsFactory::createImage(int aWidth, int aHeight, int aFillColor)
{
    GFX_LOG_FUNC_CALL();
    Pixmap* pixmap = new Pixmap();
    pixmap->createBySize(aWidth, aHeight, aFillColor ,EFormatNone /*format decided by framework*/);
    return static_cast<Image*>(pixmap);
}

/*static*/ Image* GraphicsFactory::createImage(Image* aImage, int aX, int aY, int aWidth, int aHeight)
{
    GFX_LOG_FUNC_CALL();
    Pixmap* pixmap = new Pixmap();
    pixmap->createFromImage(aImage, aX, aY, aWidth, aHeight);
    return static_cast<Image*>(pixmap);
}

/*static*/ Image* GraphicsFactory::createImage(const QImage& aImage)
{
    GFX_LOG_FUNC_CALL();
    Pixmap* pixmap = new Pixmap();
    pixmap->createFromQImage(aImage);
    return static_cast<Image*>(pixmap);
}

/*static*/ Image* GraphicsFactory::createImage(int* aRgbData, int aWidth, int aHeight, bool aHasAlpha)
{
    Pixmap* pixmap = new Pixmap();
    pixmap->createFromRGB(aRgbData, aWidth, aHeight, aHasAlpha);
    return static_cast<Image*>(pixmap);
}

/*static*/ ImageLoader* GraphicsFactory::createImageLoader()
{
    GFX_LOG_FUNC_CALL();
    ImageloaderImpl* loader = new ImageloaderImpl();
    return static_cast<ImageLoader*>(loader);
}

/*static*/ Image* GraphicsFactory::createImage(ImageDataWrapper* aImageDataPtr)
{
    GFX_LOG_FUNC_CALL();
    Pixmap* pixmap = new Pixmap();
    pixmap->createFromImageData(aImageDataPtr);
    return static_cast<Image*>(pixmap);
}

/*static*/ ImageDataWrapper* GraphicsFactory::createImageData(Image* aImage)
{
    GFX_LOG_FUNC_CALL();
    ImageDataWrapperImpl* wrapper = new ImageDataWrapperImpl();

    if(aImage != NULL)
    {
        wrapper->setData(aImage);
    }
    return wrapper;
}

/*static*/ PaletteDataWrapper* GraphicsFactory::createPaletteData(PaletteDataWrapper::TDirectData& aDirectData)
{
    GFX_LOG_FUNC_CALL();
    return new DirectPaletteDataWrapper(aDirectData);
}

/*static*/ PaletteDataWrapper* GraphicsFactory::createPaletteData(QVector<QRgb>& aIndexedData)
{
    GFX_LOG_FUNC_CALL();
    return new IndexedPaletteDataWrapper(aIndexedData);
}

} // namespace GFX
} // namespace Java
