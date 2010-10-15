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

#include "imagebase.h"
#include "gfxlog.h"

namespace Java { namespace GFX {

ImageBase::ImageBase() : mAlpha(-1), mHasMask(false)
    {
    }

ImageBase::~ImageBase()
    {
    }

void ImageBase::doGetRgb(QImage& image, int* aRgbdata, int aOffset, int aScanlength, 
                         int aX, int aY, int aWidth, int aHeight)
    {
    GFX_LOG_FUNC_CALL();

    if(aX != 0 || aY != 0 || aWidth < image.width() || aHeight < image.height())
        {
        image = image.copy(aX, aY, aWidth, aHeight);
        if(image.isNull())
            {
            throw GfxException(EGfxErrorNoMemory, "copying from original image failed");
            }
        }

    // If image is not 32bpp we need to convert it
    if(image.format() != QImage::Format_RGB32 &&
       image.format() != QImage::Format_ARGB32)
        {
        image = image.convertToFormat(QImage::Format_ARGB32);
        if(image.isNull())
            {
            throw GfxException(EGfxErrorNoMemory, "format convertion to 32bpp failed");
            }
        }

    // Temporary storage for pixels
    QRgb* pixel = NULL;

    // dataArray index, start from offset
    int targetIndex = aOffset;
    // Iterate through lines
    for(int b = 0; b < aHeight; ++b)
        {
        // Obtain pointer to start of current line (y)
        const unsigned char* lineStart = image.scanLine(b);
        // Iterate through pixels on each line
        for (int a = 0; a < aWidth; ++a)
            {
            // Set the current pixel, relative to line start
            pixel = ((QRgb*)lineStart) + a;
            // Move target pointer to the next slot
            targetIndex = aOffset + a + (b * aScanlength);
            // Shift pixels to correct places, needed for 32-bit format
            // as the bits order in memory may vary between systems
            aRgbdata[targetIndex] =  ((qAlpha(*pixel) & 0xff) << 24) |
                                     ((qRed(*pixel)   & 0xff) << 16) |
                                     ((qGreen(*pixel) & 0xff) << 8 ) |
                                     ((qBlue(*pixel)  & 0xff));
            }
        }
    }

void ImageBase::doGetRgb(QImage& image, char* aRgbdata, char* aTransparencyMask,int aOffset, 
                         int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat)
    {
    GFX_LOG_FUNC_CALL();

    if(aX != 0 || aY != 0 || aWidth < image.width() || aHeight < image.height())
        {
        image = image.copy(aX, aY, aWidth, aHeight);
        if(image.isNull())
            {
            throw GfxException(EGfxErrorNoMemory, "copying from original image failed");
            }
        }

    if(aScanlength < 0)
        {
         image = image.mirrored(false, true);
         if(image.isNull())
             {
             throw GfxException(EGfxErrorNoMemory, "Mirroring failed");
             }
        }

    // If image is not monochrome we need to convert it
    if(image.format() != QImage::Format_Mono)
    {
        image = image.convertToFormat(QImage::Format_Mono);
        if(image.isNull())
        {
            throw GfxException(EGfxErrorNoMemory, "Format conversion to 8bpp failed");
        }
    }

    // dataArray index, start from offset
    int targetIndex = aOffset;

    const unsigned char* imageStart = image.bits();
    QImage mask = image.alphaChannel();
    const unsigned char* maskStart = mask.bits();

    // Find the number of full bytes
    int fullBytes = aWidth/8;
    int bpl = image.bytesPerLine();

    if(bpl == fullBytes)
        {
        memcpy(aRgbdata+targetIndex, imageStart, bpl*aHeight);
        memcpy(aTransparencyMask+targetIndex, maskStart, bpl*aHeight);
        }
    else
        {
        memcpy(aRgbdata+targetIndex, imageStart, fullBytes*aHeight + aHeight);
        memcpy(aTransparencyMask+targetIndex, maskStart, fullBytes*aHeight + aHeight);
        }
    }

void ImageBase::doGetRgb(QImage& image, short* aRgbdata, int aOffset, int aScanlength, 
                         int aX, int aY, int aWidth, int aHeight, int aFormat)
    {
    GFX_LOG_FUNC_CALL();

    // Match format to QT
    int format;
    switch (aFormat) {
        case EFormatRGB555:
            format = QImage::Format_RGB555;
            break;
        case EFormatRGB16:
            format = QImage::Format_RGB16;
            break;
         case EFormatRGB444:
            format = QImage::Format_RGB444;
            break;
         case EFormatARGB4444Premultiplied:
            format = QImage::Format_ARGB4444_Premultiplied;
            break;
         default:
            format = QImage::Format_RGB16;
    }

    if(aX != 0 || aY != 0 || aWidth < image.width() || aHeight < image.height())
        {
        image = image.copy(aX, aY, aWidth, aHeight);
        if(image.isNull())
            {
            throw GfxException(EGfxErrorNoMemory, "copying from original image failed");
            }
        }

    // If image is not format we need, convert it
    if(image.format() != format)
        {
        image = image.convertToFormat((QImage::Format)format);
        if(image.isNull())
            {
            throw GfxException(EGfxErrorNoMemory, "format convertion to 16bpp failed");
            }
        }

    // Temporary storage for pixels
    short* pixel = NULL;
    // dataArray index, start from offset
    int targetIndex = aOffset;

    // Iterate through lines
    for(int b=0; b < aHeight; b++)
        {
        // Obtain pointer to start of current line (y)
        const unsigned char* lineStart = image.scanLine(b);
        // Iterate through pixels on each line
        for(int a=0; a < aWidth; a++)
            {
            // Set the current pixel, relative to line start
            pixel = ((short*)lineStart) + a;
            // Move target pointer to the next slot
            targetIndex = aOffset + a + (b * aScanlength);
            aRgbdata[targetIndex] =  *pixel;
            }
        }
    }

QTransform ImageBase::generateTransformMatrix(TTransform aTransform)
    {
    GFX_LOG_FUNC_CALL();

    // In case there's both mirror and rotate is requested, the angle of rotation
    // must be negative since when the transform is applied the z-axis is processed
    // before y-axis, while the spec mandates reverse order. So by having negative  
    // degrees for rotation along z-axis here, gives correct output.
    QTransform transform;
    switch(aTransform) 
        {
        case ETransNone:
            // No transform or mirror
            break;
        case ETransRot90:
            transform.rotate(90, Qt::ZAxis);
            break;
        case ETransRot180:
            transform.rotate(180, Qt::ZAxis);
            break;
        case ETransRot270:
            transform.rotate(270, Qt::ZAxis);
            break;
        case ETransMirror:
            transform.rotate(180,Qt::YAxis);
            break;
        case ETransMirrorRot90:
            transform.rotate(180, Qt::YAxis);
            transform.rotate(-90, Qt::ZAxis);
            break;
        case ETransMirrorRot180:
            transform.rotate(180, Qt::YAxis);
            transform.rotate(-180, Qt::ZAxis);
            break;
        case ETransMirrorRot270:
            transform.rotate(180, Qt::YAxis);
            transform.rotate(-270, Qt::ZAxis);
            break;
        default:
            Q_ASSERT_X(false, "Graphics", "Transform type not recognized");
        }
    return transform;
    }

int ImageBase::getAlpha() 
    {
    return mAlpha;
    }

bool ImageBase::hasMask()
    {
    return mHasMask;
    }

QImage ImageBase::imageDataToQImage(ImageDataWrapper* aData)
    {
    if(aData->getDepth() != 32)
        {
        // Java side always converts the ImageData to 32 bit before passing it
        // to native side, bail out if we get something else
        throw GfxException(EGfxErrorNoMemory, "Only 32 bit ImageData is supported in Image creation");
        }

    int bpp = 4; // bit depth = 32, 4 bytes per pixel
    int size = aData->getWidth()*aData->getHeight()*bpp;
    char* pixelData = aData->getData(ImageDataWrapper::EPixelData);

    for(int index = 0; index < size; index += bpp)
        {
        int pixel = *(reinterpret_cast<int*>(pixelData+index));

        pixelData[index]   = (uchar)(pixel >> 24) & 0xFF;
        pixelData[index+1] = (uchar)(pixel >> 16) & 0xFF;
        pixelData[index+2] = (uchar)(pixel >> 8) & 0xFF;
        pixelData[index+3] = 0xFF; // Set alpha to opaque here, alpha channel data will be handled later
        }

    QImage image(reinterpret_cast<uchar*>(aData->getData(ImageDataWrapper::EPixelData)),
                                              aData->getWidth(),
                                              aData->getHeight(),
                                              aData->getBytesPerLine(),
                                              QImage::Format_ARGB32);
    if(image.isNull())
        {
        throw GfxException(EGfxErrorNoMemory, "Image creation failed");
        }

    // Set indexed palette (if one is set)
    if(!aData->isDirect())
        {
        image.setColorTable(*aData->getPaletteData()->getIndexedPalette());
        }

    mHasMask = false;
    mAlpha = aData->getAlpha();

    if(aData->getData(ImageDataWrapper::EMaskData))
        {
        QImage mask(reinterpret_cast<uchar*>(aData->getData(ImageDataWrapper::EMaskData)),
                                             aData->getWidth(),
                                             aData->getHeight(),
                                             QImage::Format_Mono);
        if(mask.isNull())
            {
            throw GfxException(EGfxErrorNoMemory, "Image alpha channel creation failed");
            }
        image.setAlphaChannel(mask);
        mHasMask = true;
        }
    else if(mAlpha != -1)
        {
        // Global alpha is set, overrides alpha channel data
        QImage alpha(aData->getWidth(), aData->getHeight(), QImage::Format_Indexed8);
        if(alpha.isNull())
            {
            throw GfxException(EGfxErrorNoMemory, "Image alpha channel creation failed");
            }

        alpha.fill(aData->getAlpha());
        image.setAlphaChannel(alpha);
        }
    else if(aData->getData(ImageDataWrapper::EAlphaData))
        {
        // Alpha channel data is set
        const int w = aData->getWidth();
        QImage alpha(reinterpret_cast<uchar*>(aData->getData(ImageDataWrapper::EAlphaData)),
                                              w,
                                              aData->getHeight(),
                                              w,
                                              QImage::Format_Indexed8);
        QVector<QRgb> colors(255);
        for(int i=0; i<255; i++)
            {
            colors[i] = qRgb(i, i, i);
            }
        alpha.setColorTable(colors);
        colors.clear();

        if(alpha.isNull())
            {
            throw GfxException(EGfxErrorNoMemory, "Image alpha channel creation failed");
            }
        image.setAlphaChannel(alpha);
        }
    return image;
    }


QImage::Format ImageBase::mapInternalFormatToQt(TImageFormat internalImageFormat)
    {
    switch (internalImageFormat) 
        {
        case EFormatARGB32:
            return QImage::Format_ARGB32;
        case EFormatRGB32:
            return QImage::Format_RGB32;
        case EFormatARGB32Premultiplied:
            return QImage::Format_ARGB32_Premultiplied;
         case EFormatRGB16:
            return QImage::Format_RGB16;
         case EFormatRGB555:
            return QImage::Format_RGB555;
         case EFormatRGB444:
            return QImage::Format_RGB444;
         case EFormatARGB4444Premultiplied:
            return QImage::Format_ARGB4444_Premultiplied;
         case EFormatMONO:
            return QImage::Format_Mono;
         default:
            return QImage::Format_Invalid;
        }
    }

TImageFormat ImageBase::mapQtFormatToInternal(QImage::Format qtImageFormat)
    {
    switch (qtImageFormat) 
        {
        case QImage::Format_ARGB32:
            return EFormatARGB32;
        case QImage::Format_RGB32:
            return EFormatRGB32;
        case QImage::Format_ARGB32_Premultiplied:
            return EFormatARGB32Premultiplied;
         case QImage::Format_RGB16:
            return EFormatRGB16;
         case QImage::Format_RGB555:
            return EFormatRGB555;
         case QImage::Format_RGB444:
            return EFormatRGB444;
         case QImage::Format_ARGB4444_Premultiplied:
            return EFormatARGB4444Premultiplied;
         case QImage::Format_Mono:
            return EFormatMONO;
         default:
            return EFormatNone;
        }
    }

} // namespace GFX
} // namespace Java
