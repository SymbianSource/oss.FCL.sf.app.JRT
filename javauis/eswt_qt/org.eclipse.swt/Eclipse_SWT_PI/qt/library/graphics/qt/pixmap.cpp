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

Pixmap::Pixmap() : mAlpha(-1), mHasMask(false)
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

    // Get QPixmap from original
    QPixmap* src = aImage->getPixmap();
    if(!src)
    {
        throw GfxException(EGfxErrorNoMemory, "Image (pixmap) creation failed");
    }

    // If specific values were given and they represent the whole pixmap exactly
    bool isWholePixmap = (src->size() == QSize(aWidth, aHeight)) && (aX == aY == 0);

    // Also an empty area means that the whole pixmap needs to be copied
    bool isEmptyArea = QRect(aX, aY, aWidth, aHeight).isEmpty();

    if( isWholePixmap || isEmptyArea )
    {
        // Assign to our copy using implicit sharing
        mPixmap = *src;
    }
    else
    {
        // Convert to QImage, copy the pixel data area, convert back to QPixmap
        mPixmap = src->copy(aX, aY, aWidth, aHeight);
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
    throw GfxException(EGfxErrorNoMemory, "QImage (Pixmap) creation failed");
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
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) alpha channel creation failed");
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
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) alpha channel creation failed");
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
        throw GfxException(EGfxErrorNoMemory, "Image (Pixmap) alpha channel creation failed");
        }
        image.setAlphaChannel(alpha);
    }

    mPixmap = QPixmap::fromImage(image);

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

int Pixmap::getFormat()
{
    GFX_LOG_FUNC_CALL();
    QImage img = mPixmap.toImage();
    // Validate conversion
    if(img.isNull()) {
        throw GfxException(EGfxErrorNoMemory, "Convertion from QPixmap to QImage failed");
    }
    int imgFormat = img.format();

    switch (imgFormat) {
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

int Pixmap::getHeight()
{
    GFX_LOG_FUNC_CALL();
    return mPixmap.height();
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

int Pixmap::getAlpha()
{
    GFX_LOG_FUNC_CALL();
    return mAlpha;
}

bool Pixmap::hasMask()
{
    GFX_LOG_FUNC_CALL();
    return mHasMask;
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


void Pixmap::getRgb(char* aRgbdata, char* aTransparencyMask,int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int /*aFormat*/) {

    GFX_LOG_FUNC_CALL();

    // Convert to QImage in order to access pixels
    QImage image = mPixmap.toImage();

    // Validate conversion
    if(image.isNull()) {
        throw GfxException(EGfxErrorNoMemory, "Conversion from QPixmap to QImage failed");
    }

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

void Pixmap::getRgb(short* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat) {
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
    // Convert to QImage in order to access pixels
    QImage image = mPixmap.toImage();

    // Validate conversion
    if(image.isNull())
    {
        throw GfxException(EGfxErrorNoMemory, "Conversion from QPixmap to QImage failed");
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

QImage Pixmap::toImage()
{
    return mPixmap.toImage();
}

void Pixmap::transform(TTransform aTransform)
{
    GFX_LOG_FUNC_CALL();

    // For rotation along z-axis
    QTransform imageTransform;
    bool flip = false;
    bool rotate = true;

    switch(aTransform) {
        case ETransNone:
            // No transform or mirror
            return;
        case ETransRot90:
            imageTransform.rotate(90, Qt::ZAxis);
            break;
        case ETransRot180:
            imageTransform.rotate(180, Qt::ZAxis);
            break;
        case ETransRot270:
            imageTransform.rotate(270, Qt::ZAxis);
            break;
        case ETransMirror:
            flip = true;
            rotate = false;
            break;
        case ETransMirrorRot90:
            imageTransform.rotate(90, Qt::ZAxis);
            flip = true;
            break;
        case ETransMirrorRot180:
            imageTransform.rotate(180, Qt::ZAxis);
            flip = true;
            break;
        case ETransMirrorRot270:
            imageTransform.rotate(270, Qt::ZAxis);
            flip = true;
            break;
        default:
            Q_ASSERT_X(false, "Graphics", "Transform type not recognized");
            return;
    }

    // Mirror image first if requested
    if(flip)
    {
        QTransform flipTransform;
        flipTransform.rotate(180,Qt::YAxis);

        // Create temp pixmap for flipped
        QPixmap temp = mPixmap.transformed(flipTransform, Qt::FastTransformation);

        // Validate transformation
        if(temp.isNull())
        {
            throw GfxException(EGfxErrorNoMemory, "temp buffer alloc failed");
        }

        // Free original pixmap and store new
        mPixmap = temp;
    }

    // Then rotate
    if(rotate)
    {
        // Create temp pixmap for rotate
        QPixmap temp = mPixmap.transformed(imageTransform, Qt::FastTransformation);

        // Validate alloc
        if(temp.isNull())
        {
            throw GfxException(EGfxErrorNoMemory, "temp buffer alloc failed");
        }

        // Free original image and store new
        mPixmap = temp;
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
