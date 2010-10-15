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
#include "imageimpl.h"
#include "imageloaderimpl.h"
#include "imagedataimpl.h"
#include "palettedataimpl.h"
#include "gfxlog.h"

#ifdef __SYMBIAN32__
#include "windowsurfaceimpl_symbian.h"
#else
#include "windowsurfaceimpl_linux.h"
#endif

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

/*static*/ Image* GraphicsFactory::createImage(int aWidth, int aHeight, int aFillColor, TImageType aType)
{
    GFX_LOG_FUNC_CALL();
    if(aType == EImage)
        {
        ImageImpl* image = new ImageImpl();
        // create ARGB32 image
        image->createBySize(aWidth, aHeight, aFillColor, EFormatARGB32); 
        return static_cast<Image*>(image);
        }
    else if(aType == EPixmap)
        {
        Pixmap* pixmap = new Pixmap();
        pixmap->createBySize(aWidth, aHeight, aFillColor ,EFormatNone /*format decided by framework*/);
        return static_cast<Image*>(pixmap);
        }
    return NULL;
}

/*static*/ Image* GraphicsFactory::createImage(Image* aImage, int aX, int aY, int aWidth, int aHeight)
{
    GFX_LOG_FUNC_CALL();
    if(aImage->type() == EImage)
        {
        ImageImpl* image = new ImageImpl();
        image->createFromImage(aImage, aX, aY, aWidth, aHeight); 
        return static_cast<Image*>(image);    
        }
    else if(aImage->type() == EPixmap)
        {
        Pixmap* pixmap = new Pixmap();
        pixmap->createFromImage(aImage, aX, aY, aWidth, aHeight);
        return static_cast<Image*>(pixmap);       
        }
    return NULL;
}

/*static*/ Image* GraphicsFactory::createImage(Image* aImage, int aX, int aY, int aWidth, 
                                                  int aHeight, TImageType aTypeOfCopy)
{
    GFX_LOG_FUNC_CALL();
    if(aTypeOfCopy == EImage)
        {
        ImageImpl* image = new ImageImpl();
        image->createFromImage(aImage, aX, aY, aWidth, aHeight); 
        return static_cast<Image*>(image);    
        }
    else if(aTypeOfCopy == EPixmap)
        {
        Pixmap* pixmap = new Pixmap();
        pixmap->createFromImage(aImage, aX, aY, aWidth, aHeight);
        return static_cast<Image*>(pixmap);       
        }
    return NULL;
}

/*static*/ Image* GraphicsFactory::createImage(const QImage& aImage, TImageType aType)
{
    GFX_LOG_FUNC_CALL();
    if(aType == EImage)
         {
         ImageImpl* image = new ImageImpl();
         // create ARGB32 image
         image->createFromQImage(aImage);
         return static_cast<Image*>(image);
         }
     else if(aType == EPixmap)
         {
         Pixmap* pixmap = new Pixmap();
         pixmap->createFromQImage(aImage);
         return static_cast<Image*>(pixmap);
         }
    return NULL;
}

/*static*/ Image* GraphicsFactory::createImage(const QPixmap& aPixmap, TImageType aType)
{
    GFX_LOG_FUNC_CALL();
    if(aType == EImage)
         {
         ImageImpl* image = new ImageImpl();
         // create ARGB32 image
         image->createFromQPixmap(aPixmap);
         return static_cast<Image*>(image);
         }
     else if(aType == EPixmap)
         {
         Pixmap* pixmap = new Pixmap();
         pixmap->createFromQPixmap(aPixmap);
         return static_cast<Image*>(pixmap);
         }
    return NULL;
}

/*static*/ Image* GraphicsFactory::createImage(int* aRgbData, int aWidth, int aHeight, bool aHasAlpha, TImageType aType)
{
    GFX_LOG_FUNC_CALL();
    if(aType == EImage)
        {
        ImageImpl* image = new ImageImpl();
        image->createFromRGB(aRgbData, aWidth, aHeight, aHasAlpha); 
        return static_cast<Image*>(image);    
        }
    else if(aType == EPixmap)
        {
        Pixmap* pixmap = new Pixmap();
        pixmap->createFromRGB(aRgbData, aWidth, aHeight, aHasAlpha);
        return static_cast<Image*>(pixmap);       
        }
    return NULL; 
}

/*static*/ ImageLoader* GraphicsFactory::createImageLoader(TImageType aType)
{
    GFX_LOG_FUNC_CALL();
    ImageloaderImpl* loader = new ImageloaderImpl(aType);
    return static_cast<ImageLoader*>(loader);
}

/*static*/ Image* GraphicsFactory::createImage(ImageDataWrapper* aImageDataPtr, TImageType aType)
{
    GFX_LOG_FUNC_CALL();
    
    if(aType == EImage)
         {
         ImageImpl* image = new ImageImpl();
         image->createFromImageData(aImageDataPtr); 
         return static_cast<Image*>(image);    
         }
    else if(aType == EPixmap)
         {
         Pixmap* pixmap = new Pixmap();
         pixmap->createFromImageData(aImageDataPtr);
         return static_cast<Image*>(pixmap);       
         }
    return NULL;
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
