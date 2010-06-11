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
#include <QBrush>
#include <QGradient>
#include <QImage>
#include <QLine>
#include <QLinearGradient>
#include <QPainterPath>
#include <QPolygon>
#include <QColor>
#include <QFont>
#include <QStyleOptionFocusRect>
#include <QApplication>
#include <QFrame>
#include <QPaintEngine>
//#ifdef __SYMBIAN32__
//#include <QtGui/private/qpixmapdata_p.h>
//#endif
#include <memory>
#include "graphicscontextimpl.h"
#include "gfxlog.h"
#include "autorelease.h"
#include "swtbufferflushevent.h"
#include "iostream"
using namespace std;


namespace Java { namespace GFX {

GraphicsContextImpl::GraphicsContextImpl()
: mPainter(NULL),
  mPen(NULL),
  mBackground(Qt::white),
  mForeground(Qt::black)
{
    GFX_LOG_FUNC_CALL();
}

GraphicsContextImpl::~GraphicsContextImpl()
{
    GFX_LOG_FUNC_CALL();

    // Disconnect observer in case it is active
    if(mObserver.isObserving())
    {
        mObserver.disconnectTarget();
    }
    doCleanup();
}

WindowSurface* GraphicsContextImpl::getWindowSurface()
{
    GFX_LOG_FUNC_CALL();

    QPaintEngine* engine = mPainter->paintEngine();
    QPaintDevice* device = engine->paintDevice();

    switch (device->devType())
    {
        case QInternal::Image:
        {
            return GraphicsFactory::createWindowSurface(device, WsTypeQtImage);
        }
        case QInternal::Pixmap:
        {
            // this is a quick fix for linux build break
//#ifdef __SYMBIAN32__
//            QPixmapData* data = mSurface.getPixmap()->pixmapData();
//            return GraphicsFactory::createWindowSurface(static_cast<QPaintDevice*>(data->buffer()), QtImage);
//#else
            return NULL;
//#endif
        }
        default:
        {
            return 0;
        }
    }

}

void GraphicsContextImpl::bindTarget(int aTarget, TTargetType aType, const int& aBufferFlushTargetHandle)
{
    GFX_LOG_FUNC_CALL();

    // If not created yet, create fresh painter & pen
    if (mPainter == NULL || mPen == NULL ) {
        mPainter = new(std::nothrow) QPainter();
        mPen     = new(std::nothrow) QPen();
    }

    // Validate allocations
    if (mPainter == NULL || mPen == NULL )
    {
        doCleanup();
        throw GfxException(EGfxErrorNoMemory, "mPainter or mPen - alloc failed");
    }

    // color are clean at this point
    mBgColorDirty = false;
    mFgColorDirty = false;

    // Reset local color
    mForeground = mPen->color();

    // Set target for surface
    mSurface.setTarget(aTarget, aType);
    if(aType == ETypeBuffer)
        {
        mSurface.setBufferFlushTarget(reinterpret_cast<QPaintDevice*>(aBufferFlushTargetHandle));
        }

    QPaintDevice* paintDevice = mSurface.getBindable();
    if(paintDevice->paintingActive())
    {
        doCleanup();
        throw GfxException(EGfxErrorIllegalState, "Target already bound by other gc instance");
    }
    
    // Start painter on target and check success of begin()
    // This might fail if there's already painter associated with
    // current target, the target is null or if target is widget and
    // paintevent hasn't been sent by window server before trying to paint
    if (!(mPainter->begin(paintDevice)))
    {
        doCleanup();
        throw GfxException(EGfxErrorIllegalArgument, "Painter->begin() - failed");
    }

    // If target is type of Widget or MainWindow, set observer to monitor
    // rendering targets state, i.e. when it's destroyed.
    // Is target is deleted before releaseTarget is called, the observer gets notification
    // of that and automatically calls releaseTarget of this GC.
    if(mSurface.getType() == ETypeWidget)
    {
        mObserver.connectTarget((GraphicsContext*)this, mSurface.getWidget());
    }

    // set default pen width to 1 isntead of Qt default 0 (cosmetic pen)
    mPen->setWidth(1);
    mPainter->setPen(*mPen);
}

void GraphicsContextImpl::render(Buffer* aBuffer)
{
    GFX_LOG_FUNC_CALL();
    mPainter->drawPicture(0,0, *aBuffer->getPicture());
}

void GraphicsContextImpl::releaseTarget()
{
    GFX_LOG_FUNC_CALL();

    // End painting, i.e. flush if painter is active,
    // otherwise end should not be called.
    // mPainter might be null at this point if binding failed,
    // so check that before checking state.
    if(mPainter != NULL) {
        if(mPainter->isActive())
        {
            mPainter->end();
        }
    }

    // Disconnect observer in case it is active
    if(mObserver.isObserving())
    {
        mObserver.disconnectTarget();
    }

    // Do cleanup
    mSurface.clearTarget();

    // saveSettings/restoreSettings need mPainter, so do not do cleanup for now
    // doCleanup();
}

/*
 *  Copy area from currently bound target to given image target
 *  NOTE: This covers currently only Images
 */
void GraphicsContextImpl::copyArea(Image* aImage, int aX, int aY)
{
    GFX_LOG_FUNC_CALL();

    switch (mSurface.getType()) {
        case ETypeWidget:
        case ETypeBuffer:
        {
            QWidget* widget = 0;
            if (mSurface.getType() == ETypeWidget)
            {
                widget = mSurface.getWidget();
            }
            else // ETypeBuffer
            {
                widget = static_cast<QWidget*>(reinterpret_cast<QObject*>(mSurface.getBufferFlushTarget()));
            }

            if (widget)
            {
                // Determine the smallest area needed to grab, don't grab anything we don't need
                const int widgetGrabWidth = widget->width() - aX;
                const int widgetGrabHeight = widget->height() - aY;
                const int imageGrabWidth = aImage->getWidth();
                const int imageGrabHeight = aImage->getHeight();
                const int minGrabWidth = widgetGrabWidth < imageGrabWidth ? widgetGrabWidth : imageGrabWidth;
                const int minGrabHeight = widgetGrabHeight < imageGrabHeight ? widgetGrabHeight : imageGrabHeight;

                // Source coordinates need to be adjusted by the widget's
                // location in the parent that owns the window.
                QPoint offset = widgetOffsetInWindow(widget);
                QRect source(aX+offset.x(), aY+offset.y(), minGrabWidth, minGrabHeight);

                QRect target(0, 0, minGrabWidth, minGrabHeight);

                // Get the paint device that the widget is using
                int devType = QInternal::UnknownDevice;
                QPaintEngine* engine = mPainter->paintEngine();
                QPaintDevice* device = NULL;
                if(engine)
                    {
                    device = engine->paintDevice();
                    if(device)
                        {
                        devType = device->devType();
                        }
                    }

                switch(devType)
                    {
                    case QInternal::Image:
                        {
                        QImage& pixels = static_cast<QImage&>(*device);
                        QPainter p(aImage->getBindable());
                        drawImageCheckOverlap(p, *aImage->getBindable(), target, pixels, source);
                        }
                        break;
                    case QInternal::Pixmap:
                        {
                        QPixmap& pixmap = static_cast<QPixmap&>(*device);
                        QPainter p(aImage->getBindable());
                        drawPixmapCheckOverlap(p, *aImage->getBindable(), target, pixmap, source);
                        }
                        break;
                    case QInternal::Picture:
                        {
                        // Grab area from the window and draw it to the Image.
                        // Don't grab from outside of the window but grab a smaller area
                        // if necessary and adjust target coordinates accordingly.
                        QRect widgetRect(offset, widget->size());
                        QRect grabRect = source.intersected(widgetRect);
                        QPixmap winGrab = QPixmap::grabWindow(widget->window()->winId(),
                                grabRect.x(), grabRect.y(), grabRect.width(), grabRect.height());
                        QPainter p(aImage->getBindable());
                        p.drawPixmap(target.x() + grabRect.x() - source.x(),
                                target.y() + grabRect.y() - source.y(), winGrab);
                        }
                        break;
                    default: // QInternal::UnknownDevice or something else
                        {
                        // Grab area from the window and draw it to the widget.
                        QRect grabRect = source.intersected(QRect(QPoint(0, 0), widget->size()));
                        QPixmap winGrab = QPixmap::grabWindow(widget->window()->winId(),
                                grabRect.x(), grabRect.y(), grabRect.width(), grabRect.height());
                        QPainter p(aImage->getBindable());
                        p.drawPixmap(target.x() + grabRect.x() - source.x(),
                                target.y() + grabRect.y() - source.y(), winGrab);
                        }
                        break;
                    }
            }
            break;
        }
        case ETypeImage:
        {
            TImageType type = mSurface.getImageType();

            if (type == EPixmap)
            {
                QPixmap* source = mSurface.getPixmap();
                QPixmap* target = aImage->getPixmap();
                QRect sourceRect(aX, aY, target->width(), target->height());
                QRect targetRect(0, 0, target->width(), target->height());

                // If copying on itself then don't try to create two QPainters
                // for the same paint device
                if(target == source)
                {
                    drawPixmapCheckOverlap(*mPainter, *target, targetRect, *source, sourceRect);
                }
                else
                {
                    QPainter painter(target);
                    drawPixmapCheckOverlap(painter, *target, targetRect, *source, sourceRect);
                }
            }
            else
            {
                Q_ASSERT(false && "CopyArea image type not recognized");
            }
            break;
        }
        default:
            Q_ASSERT(false && "Surface type not recognized");
            break;
    }
}

/*
 *  Copy a rectangular area within the currently bound target
 */
void GraphicsContextImpl::copyArea(int aSrcX, int aSrcY, int aWidth, int aHeight, int aDestX, int aDestY, bool aPaint)
{
    GFX_LOG_FUNC_CALL();

    TTargetType painterTarget = mSurface.getType();
    switch(painterTarget)
    {
        case ETypeWidget:
        case ETypeBuffer:
        {
            QWidget* widget = 0;
            if (painterTarget == ETypeWidget)
            {
                widget = mSurface.getWidget();
            }
            else // ETypeBuffer
            {
                widget = static_cast<QWidget*>(reinterpret_cast<QObject*>(mSurface.getBufferFlushTarget()));
            }
            if (widget)
            {
                // Source coordinates need to be adjusted by the widget's
                // location in the parent that owns the window. Target
                // coordinates are already correctly in the widget's coordinate
                // system.
                QPoint offset = widgetOffsetInWindow(widget);
                QRect target(aDestX, aDestY, aWidth, aHeight);
                QRect source(aSrcX+offset.x(), aSrcY+offset.y(), aWidth, aHeight);

                // Get the paint device that the widget is using
                int devType = QInternal::UnknownDevice;
                QPaintEngine* engine = mPainter->paintEngine();
                QPaintDevice* device = NULL;
                if(engine)
                    {
                    device = engine->paintDevice();
                    if(device)
                        {
                        devType = device->devType();
                        }
                    }

                switch(devType)
                    {
                    case QInternal::Image:
                        {
                        QImage& pixels = static_cast<QImage&>(*device);
                        drawImageCheckOverlap(*mPainter, pixels, target, pixels, source);
                        }
                        break;
                    case QInternal::Pixmap:
                        {
                        QPixmap& pixmap = static_cast<QPixmap&>(*device);
                        drawPixmapCheckOverlap(*mPainter, pixmap, target, pixmap, source);
                        }
                        break;
                    case QInternal::Picture:
                        {
                        // Grab area from the window and draw it to the QPicture buffer.
                        // Don't grab from outside of the window but grab a smaller area
                        // if necessary and adjust target coordinates accordingly.
                        QRect widgetRect(offset, widget->size());
                        QRect grabRect = source.intersected(widgetRect);
                        QPixmap winGrab = QPixmap::grabWindow(widget->window()->winId(),
                                grabRect.x(), grabRect.y(), grabRect.width(), grabRect.height());
                        mPainter->drawPixmap(target.x() + grabRect.x() - source.x(),
                                target.y() + grabRect.y() - source.y(), winGrab);

                        // The copied area needs to be painted now. In the end
                        // result it needs to appear on top so the area needs
                        // to be first painted to the widget then a paint event
                        // is sent to the application where the area is
                        // clipped. All commands that have been collected
                        // outside of the paint listener are flushed here. If
                        // there are more commands outside the paint listener
                        // after this flush then it might cause flicker.
                        flushBufferToSwtWidget(widget, target);
                        }
                        break;
                    default: // QInternal::UnknownDevice or something else
                        {
                        // Grab area from the window and draw it to the widget.
                        QRect grabRect = source.intersected(QRect(QPoint(0, 0), widget->size()));
                        QPixmap winGrab = QPixmap::grabWindow(widget->window()->winId(),
                                grabRect.x(), grabRect.y(), grabRect.width(), grabRect.height());
                        mPainter->drawPixmap(target.x() + grabRect.x() - source.x(),
                                target.y() + grabRect.y() - source.y(), winGrab);
                        }
                        break;
                    }

                if(aPaint)
                    {
                    // Calculate region to invalidate

                    QRegion sourceArea(aSrcX, aSrcY, aWidth, aHeight);
                    QRegion targetArea(aDestX, aDestY, aWidth, aHeight);
                    QRegion clientArea(0, 0, widget->size().width(), widget->size().height());
                    QRegion sourceAreaInsideWidget = clientArea.intersected(sourceArea);

                    // Start with the whole source area
                    QRegion invalidArea(sourceArea);
                    // Remove possibly overlapping target area
                    invalidArea = invalidArea.subtracted(targetArea);
                    // Add the areas that were scrolled in from outside of the widget
                    invalidArea = invalidArea.united(targetArea);
                    sourceAreaInsideWidget.translate(aDestX-aSrcX, aDestY-aSrcY);
                    invalidArea = invalidArea.subtracted(sourceAreaInsideWidget);

                    widget->update(invalidArea);
                    }
            }
            break;
        }
        case ETypeImage:
        {
            // first obtain native image type of current target (QPixmap/QImage)
            TImageType type = mSurface.getImageType();


            if (type == EPixmap)
            {
                QRect sourceRect = QRect(aSrcX, aSrcY, aWidth, aHeight);
                QRect targetRect = QRect(aDestX, aDestY, aWidth, aHeight);
                drawPixmapCheckOverlap(*mPainter, *mSurface.getBindable(), targetRect, *mSurface.getPixmap(), sourceRect);
            }
            else
            {
                Q_ASSERT(false && "CopyArea image type not recognized");
            }
            break;
        }
        default:
        {
            Q_ASSERT(false && "Surface type not recognized");
        }
    }
}

void GraphicsContextImpl::drawArc(int aX, int aY, int aWidth, int aHeight, int aStartAngle, int aArcAngle)
{
    GFX_LOG_FUNC_CALL();
    checkFgColor();
    QRect arcRect(aX, aY, aWidth, aHeight);
    mPainter->drawArc(arcRect, aStartAngle*16, aArcAngle*16);
}

void GraphicsContextImpl::drawEllipse(int aX, int aY, int aWidth, int aHeight)
{
    GFX_LOG_FUNC_CALL();
    checkFgColor();
    mPainter->drawEllipse(aX, aY, aWidth, aHeight);
}

void GraphicsContextImpl::drawFocus(int aX, int aY, int aWidth, int aHeight)
{
    GFX_LOG_FUNC_CALL();
    QStyleOptionFocusRect option;
    option.rect.setRect(aX, aY, aWidth, aHeight);
    QApplication::style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, mPainter);
}

void GraphicsContextImpl::drawImage(Image* aImage, int x, int y)
{
    TImageType type = aImage->type();
    switch (type)
    {
        case EPixmap:
            mPainter->drawPixmap(x, y, *(aImage->getPixmap()));
            break;
        default:
            Q_ASSERT(false && "Image type not recognized");
            break;
    }
}

void GraphicsContextImpl::drawImage(Image* aImage, int aManipulation, int aTx, int aTy, int aTw, int aTh, int aSx, int aSy, int aSw, int aSh)
{
    GFX_LOG_FUNC_CALL();

    // Check if we need to do manipulation
    if(aManipulation != ETransNone)
    {
        // create a copy of the image and transform it, i.e. don't modify the original
        QImage transformed = doTransform(aImage->toImage(), aManipulation);
        mPainter->drawImage(QRect(aTx ,aTy, aTw, aTh), transformed, QRect(aSx, aSy, aSw, aSh));
        return;
    }
    TImageType type = aImage->type();
    switch (type)
    {
        case EPixmap:
            mPainter->drawPixmap(QRect(aTx ,aTy, aTw, aTh), *(aImage->getPixmap()), QRect(aSx, aSy, aSw, aSh));
            break;
        default:
            Q_ASSERT(false && "Image type not recognized");
            break;
    }
}

void GraphicsContextImpl::drawLine(int aX1, int aY1, int aX2, int aY2)
{
    GFX_LOG_FUNC_CALL();
    checkFgColor();
    mPainter->drawLine(aX1, aY1, aX2, aY2);
}

void GraphicsContextImpl::drawPoint(int aX, int aY)
{
    GFX_LOG_FUNC_CALL();
    checkFgColor();
    mPainter->drawPoint(QPoint(aX,aY));
}

void GraphicsContextImpl::drawPolygon(int aPointArray[], int aLength)
{
    GFX_LOG_FUNC_CALL();
    checkFgColor();
    QPolygon polygon;
    polygon.setPoints(aLength/2, aPointArray);
    mPainter->drawPolygon(polygon, Qt::OddEvenFill);
}
void GraphicsContextImpl::drawPolyline(int aPointArray[], int aLength)
{
    GFX_LOG_FUNC_CALL();
    checkFgColor();
    QPolygon polygon;
    polygon.setPoints(aLength/2, aPointArray);
    mPainter->drawPolyline(polygon);
}

void GraphicsContextImpl::drawRect(int aX, int aY, int aWidth, int aHeight)
{
    GFX_LOG_FUNC_CALL();
    checkFgColor();
    mPainter->drawRect(aX, aY, aWidth, aHeight);
}

void GraphicsContextImpl::drawRGB(int aRgbData[], int aRgbDataLength, int aOffset, int aScanlength,
                                    int aX, int aY, int aWidth, int aHeight, bool aProcessAlpha, int aManipulation)
{
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(aRgbData != NULL);

    // source holds pixels in 32bpp, i.e. 4 bytes per pixel
    const int bytesPerPixel = 4;

    // setup cleaners, note that buffer must be set stack before image
    // to ensure that image is deleted before buffer when going out of scope
    AutoRelease<int> buffer(NULL, true);
    std::auto_ptr<QImage> image(NULL);

    // with positive scanlength buffer can be used as such,
    // only startpointer needs to be shifted by offset
    if(aScanlength > 0)
    {
        if(aRgbDataLength < (aOffset+(aScanlength*(aHeight-1)) + aWidth ))
        {
            throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
        }
        // set rgb data starting point and create image;
        image.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*>(aRgbData + aOffset), aWidth, aHeight, (bytesPerPixel*aScanlength),
        (aProcessAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32)));

        // validate alloc
        if(image.get() == NULL || image->isNull())
        {
            throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
        }
    }

    // With negative scanlength rgb data needs to be reorganized, i.e. scanlines are read in reverse order
    // which means that image will be vertically mirrored
    else
    {
        // check that we will be within bounds of rgbData before creating buffer
        if((aOffset + ((aHeight-1)*aScanlength)) < 0)
        {
            throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
        }

        // create buffer for reorganized rgb data
        const int dataSize = aWidth*aHeight;
        buffer.reset(new(std::nothrow) int[dataSize]);

        // validate creation
        if(buffer.get() == NULL) {
            throw GfxException(EGfxErrorNoMemory, "rgbBuffer creation failed");
        }

        // source and destinations pointers
        int* srcPtr = aRgbData + aOffset;
        int* dstPtr = buffer.get();

        // setup boundaries for both src and dst arrays
        const int* srcMemStart = aRgbData;
        const int* srcMemEnd = aRgbData + aRgbDataLength;
        const int* dstMemStart = buffer.get();
        const int* dstMemEnd = buffer.get() + dataSize;

        // copy height number of image lines to buffer
        for(int i = 0; i < aHeight; i++)
        {
            // validate that both pointers are within array ranges
            if(!isPtrWithinRange(srcMemStart, srcMemEnd, (srcPtr + aWidth)) ||
                !isPtrWithinRange(srcMemStart, srcMemEnd, srcPtr) ||
                !isPtrWithinRange(dstMemStart, dstMemEnd, dstPtr) ||
                !isPtrWithinRange(dstMemStart, dstMemEnd, (dstPtr + aWidth)))
            {
                // report error
                throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
            }

            // copy line to buffer
            memcpy(dstPtr, srcPtr, aWidth*bytesPerPixel);

            // move to next line, as scanlength is negative it means backwards
            srcPtr = srcPtr + aScanlength;
            dstPtr = dstPtr + aWidth;
        }

        // finally create image
        image.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*>(buffer.get()), aWidth, aHeight,
        (aProcessAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32)));

        // validate alloc
        if(image.get() == NULL || image->isNull())
        {
            throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
        }
    }

    if(aManipulation != ETransNone)
    {
        QImage transformed = doTransform(*image, aManipulation);
        mPainter->drawImage(QPoint(aX,aY), transformed);
    } else {
        mPainter->drawImage(QPoint(aX,aY), *image);
    }
}

void GraphicsContextImpl::drawRGB(char aRgbData[], char aTransparenceMask[],  int aRgbDataLength,
                                                        int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight,
                                                        int aTransform, int aFormat) {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(aRgbData != NULL);


    // setup cleaners, note that buffer must be set stack before image
    // to ensure that image is deleted before buffer when going out of scope
    // AutoReleaseArray is needed for array type as auto_prt does not handle arrays

    std::auto_ptr<QImage> image(NULL);
    std::auto_ptr<QImage> mask (NULL);

    // only startpointer needs to be shifted by offset
    if(aScanlength > 0)
    {
        if(aRgbDataLength < (aOffset+(aScanlength*(aHeight-1)) + aWidth ))
        {
            throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
        }
        // set rgb data starting point and create image;
        image.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*>(aRgbData + aOffset), aWidth, aHeight, (aScanlength/8) + 1,
                 QImage::Format_Mono));

        // validate alloc
        if(image.get() == NULL || image->isNull())
        {
            throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
        }
        //it seems that currently QImage uses wrong Color table for Mono images
        //to ensure that color table is right, we reset it to correct colors
        image->setNumColors(2);
        image->setColor( 0, qRgb(255,255,255)); //white
        image->setColor( 1, qRgb(0,0,0) ); //black

        // set rgb data starting point and create image mask which is also image;
        if (!aTransparenceMask == 0) {
            mask.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*>(aTransparenceMask + aOffset), aWidth, aHeight, aScanlength,
                     QImage::Format_Mono)); //TODO: which formats are supported here?

            // validate alloc
            if(mask.get() == NULL || mask->isNull())
            {
                throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
            }
            //apply mask
            image->setAlphaChannel(*mask);
        }
    }

    // With negative scanlength scanlines are read in reverse order
    // which means that image will be vertically mirrored
    else
    {
        std::auto_ptr<QImage> negative (NULL);
        // check that we will be within bounds of rgbData before creating buffer
        if((aOffset + ((aHeight-1)*aScanlength)) < 0)
        {
            throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
        }

        // set rgb data starting point and create image;
        image.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*> (aRgbData + aOffset + ((aHeight-1)*aScanlength)), aWidth, aHeight, aScanlength,
                 QImage::Format_Mono)); //TODO: which formats are supoprted here?
        // validate alloc
        if(image.get() == NULL || image->isNull())
        {
            throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
        }
        //it seems that currently QImage uses wrong Color table for Mono images
        //to ensure that color table is right, we reset it to correct colors
        image->setNumColors(2);
        image->setColor( 0, qRgb(255,255,255));
        image->setColor( 1, qRgb(0,0,0) );
        if(aTransparenceMask) {
            mask.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*>(aTransparenceMask + aOffset + ((aHeight-1)*aScanlength)), aWidth, aHeight, aScanlength,
                     QImage::Format_Mono));

            // validate alloc
            if(mask.get() == NULL || mask->isNull())
            {
                throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
            }
            //apply mask
            image->setAlphaChannel(*mask);
         }
        //vertically mirror the image
        negative.reset(new(std::nothrow) QImage (image->mirrored(false,true)));
        image.reset(negative.get());
        negative.release();

    }
    //char c = reinterpret_cast<char >(*imageStart);
    QImage transformed = doTransform(*image, aTransform) ;
    mPainter->drawImage(QPoint(aX,aY), transformed);
}


void GraphicsContextImpl::drawRGB(short aRgbData[], int aRgbDataLength, int aOffset, int aScanlength, int aX, int aY,
                                  int aWidth, int aHeight, bool aProcessAlpha, int aManipulation, int aFormat) {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(aRgbData != NULL);

    // source holds pixels in 32bpp, i.e. 4 bytes per pixel
    const int bytesPerPixel = 2;
    //TODO:change formats to enumeration.
    // setup cleaners,
    std::auto_ptr<QImage> image(NULL);
    if (aFormat == EFormatRGB16 || aFormat == EFormatRGB555) {
        //in this case ignore process Alpha
        if(aScanlength > 0) {
            if(aRgbDataLength < (aOffset+(aScanlength*(aHeight-1)) + aWidth ))
            {
                throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
            }
            // set rgb data starting point and create image;
            image.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*>(aRgbData + aOffset), aWidth, aHeight,
                                                 (bytesPerPixel*aScanlength), (aFormat == EFormatRGB555 ? QImage::Format_RGB555 : QImage::Format_RGB16)));
            // validate alloc
            if(image.get() == NULL || image->isNull())
            {
                throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
            }
        }
        //scanlength < 0
        else {
            std::auto_ptr<QImage> negative (NULL);

            if((aOffset + ((aHeight-1)*aScanlength)) < 0)
            {
                throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
            }
            // set rgb data starting point and create image;
            image.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*> (aRgbData + aOffset + ((aHeight-1)*aScanlength)),
                                                 aWidth, aHeight, aScanlength*bytesPerPixel,
                                                (aFormat == EFormatRGB555 ? QImage::Format_RGB555 : QImage::Format_RGB16)));
            // validate alloc
            if(image.get() == NULL || image->isNull())
            {
                throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
            }
            //vertically mirror the image
            negative.reset(new(std::nothrow) QImage (image->mirrored(false,true)));
            image.reset(negative.get());
            negative.release();
        }

    }
    else {
        if(aScanlength > 0) {
            if(aRgbDataLength < (aOffset+(aScanlength*(aHeight-1)) + aWidth ))
            {
                throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
            }
            // set rgb data starting point and create image;
            image.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*>(aRgbData + aOffset), aWidth, aHeight,
                                                 (bytesPerPixel*aScanlength), (aProcessAlpha ? QImage::Format_ARGB4444_Premultiplied : QImage::Format_RGB444)));
            // validate alloc
            if(image.get() == NULL || image->isNull())
            {
                throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
            }
        }
        //scanlength < 0
        else {
            std::auto_ptr<QImage> negative (NULL);

            if((aOffset + ((aHeight-1)*aScanlength)) < 0)
            {
                throw GfxException(EGfxErrorArrayIndexOutOfBounds, "");
            }

            // set rgb data starting point and create image;
            image.reset(new(std::nothrow) QImage(reinterpret_cast<unsigned char*> (aRgbData + aOffset + ((aHeight-1)*aScanlength)),
                                                 aWidth, aHeight, aScanlength*bytesPerPixel,
                                                (aProcessAlpha ? QImage::Format_ARGB4444_Premultiplied : QImage::Format_RGB444)));
            // validate alloc
            if(image.get() == NULL || image->isNull())
            {
                throw GfxException(EGfxErrorNoMemory, "Image creation from rgbdata failed");
            }
            //vertically mirror the image
            negative.reset(new(std::nothrow) QImage (image->mirrored(false,true)));
            image.reset(negative.get());
            negative.release();
        }
    }
    QImage transformed = doTransform(*image, aManipulation) ;
    mPainter->drawImage(QPoint(aX,aY), transformed);

}

void GraphicsContextImpl::drawRoundRect(int aX, int aY, int aWidth, int aHeight, int aArcWidth, int aArcHeight)
{
    GFX_LOG_FUNC_CALL();
    checkFgColor();
    int xRnd = (int)((((float)aArcWidth * 2) / (float)aWidth) * 100);
    int yRnd = (int)((((float)aArcHeight * 2) / (float)aHeight) * 100);
    mPainter->drawRoundRect(aX, aY, aWidth, aHeight, xRnd, yRnd);
}

void GraphicsContextImpl::drawString(
        const unsigned short* aText, int aX, int aY, int aWidth, int aHeight,
        int aLength, int aAlignments, int aFlags, bool aIsTransparent)
{
    GFX_LOG_FUNC_CALL();

    checkFgColor();

    // Convert char array
    QString textToDraw = QString::fromUtf16(aText, aLength);

    // Save painter's context
    mPainter->save();

    // Set background opaque or transparent
    mPainter->setBackgroundMode(aIsTransparent ? Qt::TransparentMode : Qt::OpaqueMode);
    mPainter->setBackground(mBackground);

    // Combine alignmen and text flags
    int flagsAndAlignments = (aFlags | aAlignments);

    // Initialize bounding box for the text to be drawn
    QRect br(aX, aY, aWidth, aHeight);

    // If bounding box width or height is not set then it's calculated
    if(aWidth <= 0 || aHeight <= 0)
    {
        /* TODO use device's width and area?
        QRect r(aX, aY, mPainter->device()->width(), mPainter->device()->height());
        if(aWidth <= 0)
        {
            br.setWidth(r.width() - x);
        }
        if(aHeight <= 0)
        {
            br.setHeight(r.height() - y);
        }
        */
        QRect r(mPainter->boundingRect(QRect(), flagsAndAlignments, textToDraw));
        if(aWidth <= 0)
        {
            br.setWidth(r.width());
        }
        if(aHeight <= 0)
        {
            br.setHeight(r.height());
        }
    }
    // Draw text
    mPainter->drawText(br, flagsAndAlignments, textToDraw);
    // Painter context is restored
    mPainter->restore();
}

void GraphicsContextImpl::drawWindowSurface(WindowSurface* aSurface, int aX, int aY, int aWidth, int aHeight)
{
    GFX_LOG_FUNC_CALL();
    QRect area(aX, aY, aWidth, aHeight);
    switch(aSurface->getType())
    {
        case WsTypeQtImage:
        {  
            mPainter->drawImage(area, *aSurface->getQtImage(), area);
            break;
        }
        default:
            return;   
    }
}

void GraphicsContextImpl::fillArc(int aX, int aY, int aWidth, int aHeight, int aStartAngle, int aArcAngle)
{
    GFX_LOG_FUNC_CALL();
    QPainterPath path(QPointF(aX+(aWidth/2),aY+(aHeight/2)));
    path.arcTo( QRectF(aX, aY, aWidth, aHeight), aStartAngle, aArcAngle );
    mPainter->fillPath(path, mBackground);
}

void GraphicsContextImpl::fillEllipse(int aX, int aY, int aWidth, int aHeight)
{
    GFX_LOG_FUNC_CALL();
    mPainter->save();
    setBrushForFill();
    mPainter->drawEllipse(QRect(aX, aY, aWidth, aHeight));
    mPainter->restore();
}

void GraphicsContextImpl::fillGradientRect(int aX, int aY, int aWidth, int aHeight, bool aVertical, bool aSwapColors)
{
    GFX_LOG_FUNC_CALL();

    // create linear gardient and set start at origo of the rectangle
    QLinearGradient linearGrad;
    linearGrad.setStart(QPointF(aX,aY));

    // set gradient vector at the same position as the rect to be drawn
    // gradient sweeping from top to bottom
    if(aVertical)
    {
        linearGrad.setFinalStop(QPointF(aX, aY+(aHeight-1)));
    }
    // gradient sweeping from left to right
    else {
        linearGrad.setFinalStop(QPointF(aX+(aWidth-1), aY));
    }

    // Set start and end colors depending on swapColors
    if(aSwapColors)
    {
        linearGrad.setColorAt(0, mBackground);
        linearGrad.setColorAt(1, mForeground);
    }
    else
    {
        linearGrad.setColorAt(0, mForeground);
        linearGrad.setColorAt(1, mBackground);
    }
    mPainter->fillRect(aX, aY, aWidth, aHeight, QBrush(linearGrad));
}

void GraphicsContextImpl::fillPolygon(int aPointArray[], int aLength)
{
    GFX_LOG_FUNC_CALL();
    QPolygon polygon;
    polygon.setPoints(aLength/2, aPointArray);
    mPainter->save();
    setBrushForFill();
    mPainter->drawPolygon(polygon, Qt::OddEvenFill);
    mPainter->restore();
}

void GraphicsContextImpl::fillRect(int aX, int aY, int aWidth, int aHeight)
{
    GFX_LOG_FUNC_CALL();
    mPainter->fillRect(aX, aY, aWidth, aHeight, mBackground);
}

void GraphicsContextImpl::fillRoundRect(int aX, int aY, int aWidth, int aHeight, int aArcWidth, int aArcHeight)
{
    GFX_LOG_FUNC_CALL();
    int xRnd = (int)((((float)aArcWidth * 2) / (float)aWidth) * 100);
    int yRnd = (int)((((float)aArcHeight * 2) / (float)aHeight) * 100);
    int pWidth = mPen->width();

    // Zero pen width means cosmetic pen, which equals to 1 pixel width
    if(pWidth == 0)
    {
        aWidth = aWidth - 1;
        aHeight = aHeight -1;
    }
    else
    {
        aWidth = aWidth - pWidth;
        aHeight = aHeight - pWidth;
    }

    mPainter->save();
    setBrushForFill();
    mPainter->drawRoundRect(aX, aY, aWidth, aHeight, xRnd, yRnd);
    mPainter->restore();
}

int GraphicsContextImpl::getCharacterWidth(char aCh, bool aIsAdvanced)
{
    if(aIsAdvanced)
    {
        return mPainter->fontMetrics().width(aCh);
    }
    return mPainter->fontMetrics().boundingRect(aCh).width();
}

void GraphicsContextImpl::getFontMetricsData(int aData[], const QFont& aFont)
{
    GFX_LOG_FUNC_CALL();
    QFontMetrics fm(aFont);
    aData[EFmAscent] = fm.ascent();
    aData[EFmAverageCharWidth] = fm.averageCharWidth();
    aData[EFmDescent] = fm.descent();
    aData[EFmHeight] = fm.height();
    aData[EFmLeading] = fm.leading();
}

void GraphicsContextImpl::getFontMetricsData(int aData[])
{
    GFX_LOG_FUNC_CALL();
    getFontMetricsData(aData, mPainter->font());
}

int GraphicsContextImpl::getBackgroundAlpha()
{
    GFX_LOG_FUNC_CALL();
    return mBackground.alpha();
}

int GraphicsContextImpl::getBackgroundColor()
{
    GFX_LOG_FUNC_CALL();
    return mBackground.rgba();
}

int GraphicsContextImpl::getBlendingMode()
{
    GFX_LOG_FUNC_CALL();
    int mode = mPainter->compositionMode();
    switch (mode)
    {
        case QPainter::CompositionMode_Source:
            return ESrc;
        case QPainter::CompositionMode_SourceOver:
            return ESrcOver;
        case QPainter::CompositionMode_Xor:
            return EXor;
        default:
            Q_ASSERT(false && "Blendingmode not recognized");
            return 0;
    }
}

void GraphicsContextImpl::getClip(int aClipArray[])
{
    GFX_LOG_FUNC_CALL();

    // if clip has not been set painter return 0,0,0,0
    // so in such case return whole target rect
    if(!mPainter->hasClipping())
    {
        aClipArray[ERectX] = 0;
        aClipArray[ERectY] = 0;
        aClipArray[ERectWidth] = mSurface.getWidth();
        aClipArray[ERectHeight] = mSurface.getHeight();
    }
    else
    {
        QRegion region = mPainter->clipRegion();
        QRect rect = region.boundingRect();
        aClipArray[ERectX] = rect.left();
        aClipArray[ERectY] = rect.top();
        aClipArray[ERectWidth] = rect.width();
        aClipArray[ERectHeight] = rect.height();
    }
}

int GraphicsContextImpl::getForegroundAlpha()
{
    GFX_LOG_FUNC_CALL();
    return mForeground.alpha();
}

int  GraphicsContextImpl::getForegroundColor()
{
    GFX_LOG_FUNC_CALL();
    return mForeground.rgba();
}

void GraphicsContextImpl::getTextBoundingBox(
        int aBoundingBox[], const unsigned short* aText, int aTextLength, int aAlignments,
        int aFlags, int aRectX, int aRectY, int aRectWidth, int aRectHeight)
{
    GFX_LOG_FUNC_CALL();

    // Convert the char array
    QString textToDraw(QString::fromUtf16(aText, aTextLength));

    // Combine alignmen and text flags
    int flagsAndAlignments = (aFlags | aAlignments);

    // Get the bounding box
    QRect br(mPainter->boundingRect(
        QRect(aRectX, aRectY, aRectWidth, aRectHeight), flagsAndAlignments, textToDraw));

    // Set the bounding box data to the array
    aBoundingBox[ERectX] = br.x();
    aBoundingBox[ERectY] = br.y();
    aBoundingBox[ERectWidth] = br.width();
    aBoundingBox[ERectHeight] = br.height();
}

int GraphicsContextImpl::getStrokeWidth()
{
    GFX_LOG_FUNC_CALL();
    return mPen->width();
}

int GraphicsContextImpl::getStrokeStyle()
{
    GFX_LOG_FUNC_CALL();
    const int style = mPen->style();

    switch (style)
    {
        case Qt::SolidLine:
            return EStrokeSolid;
        case Qt::DotLine:
            return EStrokeDot;
        case Qt::DashLine:
            return EStrokeDash;
        case Qt::DashDotLine:
            return EStrokeDashDot;
        case Qt::DashDotDotLine:
            return EStrokeDashDotDot;
        default:
            Q_ASSERT(false && "The stroke style is not recognized");
            return 0;
    }
}

int GraphicsContextImpl::getTranslateX()
{
    GFX_LOG_FUNC_CALL();
    QMatrix m = mPainter->worldMatrix();
    return (int)m.dx();
}

int GraphicsContextImpl::getTranslateY()
{
    GFX_LOG_FUNC_CALL();
    QMatrix m = mPainter->worldMatrix();
    return (int)m.dy();
}

void GraphicsContextImpl::setBackgroundAlpha(int aAlpha)
{
    GFX_LOG_FUNC_CALL();
    mBackground.setAlpha(aAlpha);
    mBgColorDirty = true;
}

void GraphicsContextImpl::setBackgroundColor(int aArgb, bool aUpdateAlpha)
{
    GFX_LOG_FUNC_CALL();
    if (aUpdateAlpha)
    {
        mBackground.setRgba(aArgb);
    }
    else
    {
        mBackground.setRgb(aArgb);
    }
    mBgColorDirty = true;

}

void GraphicsContextImpl::setBlendingMode(TBlendingMode aMode)
{
    GFX_LOG_FUNC_CALL();
    switch (aMode)
    {
        case ESrc:
            mPainter->setCompositionMode(QPainter::CompositionMode_Source);
            break;
        case ESrcOver:
            mPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            break;
        case EXor:
            mPainter->setCompositionMode(QPainter::CompositionMode_Xor);
            break;
        default:
            Q_ASSERT(false && "Blendingmode not recognized");
            return;
    }
}

bool GraphicsContextImpl::hasClipping()
{
    GFX_LOG_FUNC_CALL();
    return mPainter->hasClipping();
}

void GraphicsContextImpl::cancelClipping()
{
    GFX_LOG_FUNC_CALL();
    mPainter->setClipping(false);
}

void GraphicsContextImpl::setClip(int aX, int aY, int aWidth, int aHeight, bool aIntersects)
{
    GFX_LOG_FUNC_CALL();
    if (aIntersects) {
        mPainter->setClipRect(QRect(aX, aY, aWidth, aHeight), Qt::IntersectClip);
    } else {
        mPainter->setClipRect(QRect(aX, aY, aWidth, aHeight), Qt::ReplaceClip);
    }
}

void GraphicsContextImpl::setFont(int aFontHandle)
{
    QFont* font = reinterpret_cast<QFont*>( aFontHandle );
    mPainter->setFont(*font);
}

void GraphicsContextImpl::setForegroundAlpha(int aAlpha)
{
    GFX_LOG_FUNC_CALL();
    mForeground.setAlpha(aAlpha);
    mFgColorDirty = true;
}

void GraphicsContextImpl::setForegroundColor(int aArgb, bool aUpdateAlpha)
{
    GFX_LOG_FUNC_CALL();
    if (aUpdateAlpha)
    {
        mForeground.setRgba(aArgb);
    }
    else
    {
        mForeground.setRgb(aArgb);
    }
    mFgColorDirty = true;
}

void GraphicsContextImpl::setStrokeWidth(int aWidth)
{
    GFX_LOG_FUNC_CALL();
    mPen->setWidth(aWidth);
    mPainter->setPen(*mPen);
}

void GraphicsContextImpl::setStrokeStyle(TStrokeStyle aStyle)
{
    GFX_LOG_FUNC_CALL();
    switch (aStyle)
    {
        case EStrokeSolid:
            mPen->setStyle(Qt::SolidLine);
            break;
        case EStrokeDot:
            mPen->setStyle(Qt::DotLine);
            break;
        case EStrokeDash:
            mPen->setStyle(Qt::DashLine);
            break;
        case EStrokeDashDot:
            mPen->setStyle(Qt::DashDotLine);
            break;
        case EStrokeDashDotDot:
            mPen->setStyle(Qt::DashDotDotLine);
            break;
        default:
            Q_ASSERT(false && "The stroke style is not recognized");
            return;
    }
    mPainter->setPen(*mPen);
}

void GraphicsContextImpl::translate(int aX, int aY)
{
    mPainter->translate(QPoint(aX, aY));
}

void GraphicsContextImpl::scale(int aX, int aY)
{
    mPainter->scale(aX, aY);
}

void GraphicsContextImpl::resetTransform()
{
    mPainter->resetTransform();
}

void GraphicsContextImpl::saveSettings()
{
    GFX_LOG_FUNC_CALL();
    mPainter->save();
}

void GraphicsContextImpl::restoreSettings()
{
    GFX_LOG_FUNC_CALL();
    mPainter->restore();
}

void GraphicsContextImpl::dispose()
{
    GFX_LOG_FUNC_CALL();
    delete this;
}

// Private helper Methods

void GraphicsContextImpl::doCleanup()
{
    GFX_LOG_FUNC_CALL();
    delete mPainter;
    mPainter = NULL;
    delete mPen;
    mPen = NULL;
}

void GraphicsContextImpl::checkBgColor()
{
    if(mBgColorDirty)
    {
        QBrush brush(mPainter->brush());
        brush.setColor(mBackground);
        mPainter->setBrush(brush);
        mBgColorDirty = false;
    }
}

void GraphicsContextImpl::checkFgColor()
{
    if(mFgColorDirty)
    {
        mPen->setColor(mForeground);
        mPainter->setPen(*mPen);
        mFgColorDirty = false;
    }
}

QColor GraphicsContextImpl::convertArgbToQColor(unsigned int aArgb)
{
    GFX_LOG_FUNC_CALL();
    int r, g, b, a;

    // get components from #AARRGGBB
    r = ((aArgb & 0x00FF0000u) >> 16);
    g = ((aArgb & 0x0000FF00u) >>  8);
    b = ((aArgb & 0x000000FFu)      );
    a = ((aArgb & 0xFF000000u) >> 24);
    return QColor(r, g, b, a);
}

bool GraphicsContextImpl::isPtrWithinRange(const int* aStartPtr, const int* aEndPtr,const int* aPtr)
{
    GFX_LOG_FUNC_CALL();
    if((aPtr < aStartPtr) || (aPtr > aEndPtr))
    {
        return false;
    }
    return true;
}

void GraphicsContextImpl::setBrushForFill()
{
    GFX_LOG_FUNC_CALL();
    QBrush brush(mBackground);
    mPainter->setBrush(brush);
    // set pen also to bg color so that the outline of filled primitive
    // is also with same color, as by default Qt uses pen color
    //for outline and brush color for fill
    QPen pen(mPainter->pen());
    pen.setColor(mBackground);
    mPainter->setPen(pen);
}

QImage GraphicsContextImpl::doTransform(QImage image, int aManipulation)
{
    QMatrix matrix;
    QImage transformed;

    switch (aManipulation) {
        case ETransRot90:
            matrix.rotate(90);
            break;
        case ETransRot180:
            matrix.rotate(180);
            break;
        case ETransRot270:
            matrix.rotate(270);
            break;
        default:
            break;
    }
    if(!aManipulation==0) {
        if(!(aManipulation == ETransFlipHorizontal) && !(aManipulation == ETransFlipVertical)) {
            transformed = image.transformed(matrix, Qt::FastTransformation);
        }
        else {
            transformed = image.mirrored(aManipulation == ETransFlipHorizontal, aManipulation == ETransFlipVertical);
        }
    }
    else {
        transformed = image;
    }
    return transformed;
}

void GraphicsContextImpl::flushBufferToSwtWidget(QWidget* aWidget, const QRect& aPaintArea)
{
    QCoreApplication::sendEvent(aWidget, new SwtBufferFlushEvent());
    aWidget->repaint(aPaintArea);
}

QPoint GraphicsContextImpl::widgetOffsetInWindow(QWidget* aWidget)
{
    QWidget* window = aWidget;
    QPoint offset;
    while(!window->isWindow())
        {
        offset = window->mapToParent(offset);
        window = window->parentWidget();
        }
    return offset;
}

void GraphicsContextImpl::drawImageCheckOverlap(QPainter& aPainter,
        const QPaintDevice& aTargetDevice, const QRect& aTargetRect,
        const QImage& aSourceImage, const QRect& aSourceRect)
{
    // If areas overlap then make a copy, Qt doesn't seem to handle this
    if((&aTargetDevice == &static_cast<const QPaintDevice&>(aSourceImage))
            && aSourceRect.intersects(aTargetRect))
    {
        // Make a copy of the intersecting area
        QRect intersecting = aSourceRect.intersected(aTargetRect);
        QImage copy = aSourceImage.copy(intersecting);

        // Write areas that don't intersect directly to the target
        QRegion notIntersecting(aSourceRect);
        notIntersecting.subtracted(intersecting);
        QVector<QRect> notIntersectingAreas = notIntersecting.rects();
        for(int i = 0; i < notIntersectingAreas.size(); ++i)
        {
            aPainter.drawImage(aTargetRect, aSourceImage, notIntersectingAreas.at(i));
        }

        // Write the copy of the intersecting area to the target
        aPainter.drawImage(intersecting.topLeft()
                + aTargetRect.topLeft() - aSourceRect.topLeft(), copy);
    }
    else
    {
        aPainter.drawImage(aTargetRect, aSourceImage, aSourceRect);
    }
}

void GraphicsContextImpl::drawPixmapCheckOverlap(QPainter& aPainter,
        const QPaintDevice& aTargetDevice, const QRect& aTargetRect,
        const QPixmap& aSourcePixmap, const QRect& aSourceRect)
{
    // If areas overlap then make a copy, Qt doesn't seem to handle this
    if((&aTargetDevice == &static_cast<const QPaintDevice&>(aSourcePixmap))
            && aSourceRect.intersects(aTargetRect))
    {
        // Make a copy of the intersecting area
        QRect intersecting = aSourceRect.intersected(aTargetRect);
        QPixmap copy = aSourcePixmap.copy(intersecting);

        // Write areas that don't intersect directly to the target
        QRegion notIntersecting(aSourceRect);
        notIntersecting.subtracted(intersecting);
        QVector<QRect> notIntersectingAreas = notIntersecting.rects();
        for(int i = 0; i < notIntersectingAreas.size(); ++i)
        {
            aPainter.drawPixmap(aTargetRect, aSourcePixmap, notIntersectingAreas.at(i));
        }

        // Write the copy of the intersecting area to the target
        aPainter.drawPixmap(intersecting.topLeft()
                + aTargetRect.topLeft() - aSourceRect.topLeft(), copy);
    }
    else
    {
        aPainter.drawPixmap(aTargetRect, aSourcePixmap, aSourceRect);
    }
}

} // namespace GFX
} // namespace Java
