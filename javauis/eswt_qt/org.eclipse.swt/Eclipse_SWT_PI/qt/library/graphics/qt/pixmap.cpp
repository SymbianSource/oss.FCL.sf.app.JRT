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
#include "pixmap.h"
#include "gfxlog.h"
#include <memory>

namespace Java { namespace GFX {

Pixmap::Pixmap() : ImageBase()
    {
    GFX_LOG_FUNC_CALL();
    }

Pixmap::~Pixmap()
    {
    GFX_LOG_FUNC_CALL();
    }

void Pixmap::createBySize(int aWidth, int aHeight, int aFillColor, TImageFormat /*aFormat*/)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mPixmap.isNull());

    // Pixmap's format is selected by the framework, thus it cannot be used here
    mPixmap = QPixmap(aWidth, aHeight);

    // Check if creation was successful
    if(mPixmap.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) creation failed");
        }

    // Finally fill with given fillColor, also clears the image mem area
    // otherwise there might be some random coloured pixels in image
    QColor color;
    color.setRgb(aFillColor);
    mPixmap.fill(color);
    }

void Pixmap::createFromQImage(const QImage& aImage)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mPixmap.isNull());

    // Convert to Pixmap
    mPixmap = QPixmap::fromImage(aImage);

    // Validate allocation
    if(mPixmap.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) creation failed");
        }
    }

void Pixmap::createFromQPixmap(const QPixmap& aPixmap)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mPixmap.isNull());

    mPixmap = QPixmap(aPixmap);

    // Validate allocation
    if(mPixmap.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) creation failed");
        }
    }

void Pixmap::createFromImage(Image* aImage, int aX, int aY, int aWidth, int aHeight)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mPixmap.isNull());

    // Check if the copy should be exactly same as the original
    if((aImage->getWidth() == aWidth) && (aImage->getHeight() == aHeight) || 
        QRect(aX, aY, aWidth, aHeight).isEmpty())
        {
        mPixmap = aImage->toPixmap();
        }
    // it's needed to make partial copy of the original
    else
        {
        switch(aImage->type())
            {
            case EImage:
                {
                mPixmap = QPixmap::fromImage(aImage->getConstImage()->copy(aX, aY, aWidth, aHeight));
                break;
                }
            case EPixmap:
                {
                mPixmap = aImage->getConstPixmap()->copy(aX, aY, aWidth, aHeight);
                break;
                }
            default:
                throw GfxException(EGfxErrorIllegalArgument, "Unsupported source image type");
            }
        
        }
    if(mPixmap.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (pixmap) creation failed");
        }
    }

void Pixmap::createFromImageData(ImageDataWrapper* aData)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mPixmap.isNull());

    mPixmap = QPixmap::fromImage(imageDataToQImage(aData));

    if(mPixmap.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) creation failed");
        }
    }

void Pixmap::createFromRGB(int* aRgbdata, int aWidth, int aHeight, bool aHasAlpha)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mPixmap.isNull());

    // Create QImage from rgbdata as QPixmap does not directly support that
    QImage tmpImage(reinterpret_cast<unsigned char*>(aRgbdata), aWidth, aHeight,
        (aHasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32));

    // Validate creation
    if(tmpImage.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) creation failed");
        }

    // Store as QPixmap
    mPixmap = QPixmap::fromImage(tmpImage);

    // Validate conversion
    if(mPixmap.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) creation failed");
        }
    }

void Pixmap::dispose()
    {
    GFX_LOG_FUNC_CALL();
    delete this;
    }

QPaintDevice* Pixmap::getBindable()
    {
    GFX_LOG_FUNC_CALL();
    return static_cast<QPaintDevice*>(&mPixmap);
    }

TImageFormat Pixmap::getFormat()
    {
    GFX_LOG_FUNC_CALL();
    QImage img = mPixmap.toImage();
    // Validate conversion
    if(img.isNull()) 
        {
        throw GfxException(EGfxErrorNoMemory, "Convertion from QPixmap to QImage failed");
        }
    return mapQtFormatToInternal(img.format());
    }

int Pixmap::getHeight()
    {
    GFX_LOG_FUNC_CALL();
    return mPixmap.height();
    }

const QImage* Pixmap::getConstImage()
    {
    GFX_LOG_FUNC_CALL();
    return NULL;
    }

QImage* Pixmap::getImage()
    {
    GFX_LOG_FUNC_CALL();
    return NULL;
    }

const QPixmap* Pixmap::getConstPixmap()
    {
    GFX_LOG_FUNC_CALL();
    return &mPixmap;
    }

QPixmap* Pixmap::getPixmap()
    {
    GFX_LOG_FUNC_CALL();
    return &mPixmap;
    }

int Pixmap::getWidth()
    {
    GFX_LOG_FUNC_CALL();
    return mPixmap.width();
    }

void Pixmap::getRgb(int* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight)
    {
    GFX_LOG_FUNC_CALL();

    // Convert to QImage in order to access pixels
    QImage image = mPixmap.toImage();

    // Validate conversion
    if(image.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Convertion from QPixmap to QImage failed");
        }
    doGetRgb(image, aRgbdata, aOffset, aScanlength, aX, aY, aWidth, aHeight);
    }


void Pixmap::getRgb(char* aRgbdata, char* aTransparencyMask,int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat) {

    GFX_LOG_FUNC_CALL();

    // Convert to QImage in order to access pixels
    QImage image = mPixmap.toImage();

    // Validate conversion
    if(image.isNull()) 
        {
        throw GfxException(EGfxErrorNoMemory, "Conversion from QPixmap to QImage failed");
        }
    doGetRgb(image, aRgbdata, aTransparencyMask, aOffset, aScanlength, aX, aY, aWidth, aHeight, aFormat);
    }

void Pixmap::getRgb(short* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat) 
    {
    GFX_LOG_FUNC_CALL();

    // Convert to QImage in order to access pixels
    QImage image = mPixmap.toImage();

    // Validate conversion
    if(image.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Conversion from QPixmap to QImage failed");
        }
    doGetRgb(image, aRgbdata, aOffset, aScanlength, aX, aY, aWidth, aHeight, aFormat);
    }

const QImage Pixmap::toConstImage()
    {
    GFX_LOG_FUNC_CALL();
    return mPixmap.toImage();
    }

QImage Pixmap::toImage()
    {
    GFX_LOG_FUNC_CALL();
    return mPixmap.toImage();
    }

QPixmap Pixmap::toPixmap()
    {
    GFX_LOG_FUNC_CALL();
    return QPixmap(mPixmap);
    }

void Pixmap::transform(TTransform aTransform)
    {
    GFX_LOG_FUNC_CALL();
    mPixmap = mPixmap.transformed(generateTransformMatrix(aTransform), Qt::FastTransformation);
    if(mPixmap.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) creation failed");
        }
    }

TImageType Pixmap::type()
    {
    GFX_LOG_FUNC_CALL();
    return EPixmap;
    }

bool Pixmap::hasAlphaChannel()
    {
    GFX_LOG_FUNC_CALL();
    return mPixmap.hasAlphaChannel();
    }

} // namespace GFX
} // namespace Java
