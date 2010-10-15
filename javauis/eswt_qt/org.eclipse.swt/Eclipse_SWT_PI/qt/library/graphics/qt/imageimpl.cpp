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

#include <QPixmap>
#include "imageimpl.h"
#include "gfxlog.h"

namespace Java { namespace GFX {

ImageImpl::ImageImpl() : ImageBase()
    {
    GFX_LOG_FUNC_CALL();
    }

ImageImpl::~ImageImpl()
    {
    GFX_LOG_FUNC_CALL();
    }

void ImageImpl::createBySize(int aWidth, int aHeight, int aFillColor, TImageFormat aFormat)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mImage.isNull());

    mImage = QImage(aWidth, aHeight, mapInternalFormatToQt(aFormat));
    // Check if creation was successful
    if(mImage.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (QImage) creation failed");
        }
    // Finally fill with given fillColor, also clears the image mem area
    // otherwise there might be some random coloured pixels in image
    mImage.fill(aFillColor);
    }


void ImageImpl::createFromQImage(const QImage& aImage)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mImage.isNull());

    mImage = QImage(aImage);
    // Validate allocation
    if(mImage.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (QImage) creation failed");
        }
    }
    
void ImageImpl::createFromQPixmap(const QPixmap& aPixmap)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mImage.isNull());

    mImage = aPixmap.toImage();
    // Validate allocation
    if(mImage.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (QImage) creation failed");
        }    
    }

void ImageImpl::createFromImage(Image* aImage, int aX, int aY, int aWidth, int aHeight)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mImage.isNull());

    // Check if the copy should be exactly same as the original
    if((aImage->getWidth() == aWidth) && (aImage->getHeight() == aHeight) || 
        QRect(aX, aY, aWidth, aHeight).isEmpty())
        {
        mImage = aImage->toImage();
        }
    // it's needed to make partial copy of the original
    else
        {
        switch(aImage->type())
            {
            case EImage:
                {
                mImage = aImage->getConstImage()->copy(aX, aY, aWidth, aHeight);
                break;
                }
            case EPixmap:
                {
                mImage = (aImage->getConstPixmap()->copy(aX, aY, aWidth, aHeight)).toImage();
                break;
                }
            default:
                throw GfxException(EGfxErrorIllegalArgument, "Unsupported source image type");
            }
        }
    if(mImage.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (pixmap) creation failed");
        }
    }

void ImageImpl::createFromImageData(ImageDataWrapper* aData)
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mImage.isNull());

    mImage = imageDataToQImage(aData);

    if(mImage.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (QImage) creation failed");
        }
    }

void ImageImpl::createFromRGB(int* aRgbdata, int aWidth, int aHeight, bool aHasAlpha) 
    {
    GFX_LOG_FUNC_CALL();
    Q_ASSERT(mImage.isNull());

    // Create QImage from rgbdata 
    mImage = QImage(reinterpret_cast<unsigned char*>(aRgbdata), aWidth, aHeight,
        (aHasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32));

    // Validate creation
    if(mImage.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (QImage) creation failed");
        }
    }
    
void ImageImpl::dispose()
    {
    GFX_LOG_FUNC_CALL();
    delete this;
    }

QPaintDevice* ImageImpl::getBindable()
    {
    GFX_LOG_FUNC_CALL();
    return static_cast<QPaintDevice*>(&mImage);
    }

TImageFormat ImageImpl::getFormat()
    {
    return mapQtFormatToInternal(mImage.format());
    }

int ImageImpl::getHeight()
    {
    GFX_LOG_FUNC_CALL();
    return mImage.height();
    }

const QImage* ImageImpl::getConstImage()
    {
    GFX_LOG_FUNC_CALL();
    return &mImage;
    }
        
        
QImage* ImageImpl::getImage()
    {
    GFX_LOG_FUNC_CALL();
    return &mImage;
    }

const QPixmap* ImageImpl::getConstPixmap()
    {
    GFX_LOG_FUNC_CALL();
    return NULL;
    }

QPixmap* ImageImpl::getPixmap()
    {
    GFX_LOG_FUNC_CALL();
    return NULL;
    }

void ImageImpl::getRgb(int* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight)
    {
    GFX_LOG_FUNC_CALL();
    doGetRgb(mImage, aRgbdata, aOffset, aScanlength, aX, aY, aWidth, aHeight);
    }

void ImageImpl::getRgb(char* aRgbdata, char* aTransparencyMask,int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat)
    {
    GFX_LOG_FUNC_CALL();
    doGetRgb(mImage, aRgbdata, aTransparencyMask, aOffset, aScanlength, aX, aY, aWidth, aHeight, aFormat);
    }

void ImageImpl::getRgb(short* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat)
    {
    GFX_LOG_FUNC_CALL();
    doGetRgb(mImage, aRgbdata, aOffset, aScanlength, aX, aY, aWidth, aHeight, aFormat);
    }
int ImageImpl::getWidth()
    {
    GFX_LOG_FUNC_CALL();
    return mImage.width();
    }

const QImage ImageImpl::toConstImage()
    {
    GFX_LOG_FUNC_CALL();
    return QImage(mImage);
    }

QImage ImageImpl::toImage()
    {
    GFX_LOG_FUNC_CALL();
    return QImage(mImage);
    }

QPixmap ImageImpl::toPixmap()
    {
    GFX_LOG_FUNC_CALL();
    return QPixmap::fromImage(mImage);
    }

void ImageImpl::transform(TTransform aTransform) 
    {
    GFX_LOG_FUNC_CALL();
    mImage = mImage.transformed(generateTransformMatrix(aTransform), Qt::FastTransformation);
    // Validate creation
    if(mImage.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image (QImage) creation failed");
        }
    }

TImageType ImageImpl::type()
    {
    return EImage;
    }

bool ImageImpl::hasAlphaChannel()
    {
    GFX_LOG_FUNC_CALL();
    return mImage.hasAlphaChannel();
    }

} // namespace GFX
} // namespace Java
