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
#include "gfxlog.h"
#include "imagedataimpl.h"
#include "palettedataimpl.h"

namespace Java { namespace GFX {

ImageDataWrapperImpl::ImageDataWrapperImpl()
: mAlpha(0),
  mBytesPerLine(0),
  mDelayTime(0),
  mDepth(0),
  mMaskPad(0),
  mScanlinePad(0),
  mTransparentPixel(0),
  mDisposalMethod(ImageDataWrapper::EDmUnspecified),
  mImageType(ImageDataWrapper::EUndefinedImage),
  mPaletteData(0)
{
    GFX_LOG_FUNC_CALL();
}

ImageDataWrapperImpl::~ImageDataWrapperImpl()
{
    GFX_LOG_FUNC_CALL();
    delete mPaletteData;
}

char* ImageDataWrapperImpl::getData(ImageDataWrapper::TDataType aType)
{
    GFX_LOG_FUNC_CALL();

    if (getDataSize(aType) == 0) return 0;

    switch(aType)
    {
        case EPixelData: return mPixelData.data();
        case EAlphaData: return mAlphaData.data();
        case EMaskData: return mMaskData.data();
        default: return NULL;
    }
}

int ImageDataWrapperImpl::getDataSize(ImageDataWrapper::TDataType aType)
{
    GFX_LOG_FUNC_CALL();
    switch(aType)
    {
        case EPixelData: return mPixelData.size();
        case EAlphaData: return mAlphaData.size();
        case EMaskData: return mMaskData.size();
        default: return 0;
    }
}

void ImageDataWrapperImpl::setData(Image* aSrc)
{
    GFX_LOG_FUNC_CALL();
    QImage image = aSrc->getPixmap()->toImage();
    fillData(&image, aSrc->getAlpha(), aSrc->hasMask());
}

void ImageDataWrapperImpl::setData(ImageDataWrapper::TDataType aType,
        const QByteArray& aData)
{
    GFX_LOG_FUNC_CALL();

    // QByteArray is implicitly shared, just assign
    switch(aType)
    {
        case EPixelData:
            mPixelData = aData;
            return;
        case EAlphaData:
            mAlphaData = aData;
            return;
        case EMaskData:
            mMaskData = aData;
            return;
        default: return;
    }
}

void ImageDataWrapperImpl::fillData(const QImage* aImage, int aAlpha, bool aMask)
{
    GFX_LOG_FUNC_CALL();

    int size = aImage->numBytes();

    // Cleanup old data
    mPixelData.clear();
    mAlphaData.clear();
    mMaskData.clear();
    delete mPaletteData;

    // Get image information
    mDepth = aImage->depth();
    mSize = QSize(aImage->width(), aImage->height());
    mScanlinePad = 4;
    mTransparentPixel = -1;
    mAlpha = aAlpha;
    mBytesPerLine = aImage->bytesPerLine();

    // Create pixel data
    mPixelData.fill(0, size);

    int bpp = mDepth / 8;
    const uchar* data = aImage->scanLine(0);

    char* pixelData = mPixelData.data();

    int pixel = 0;

    // Create
    for (int i = 0; i < size; i += bpp)
    {
        switch (mDepth)
        {
        case 32:
            pixel = (data[i] & 0xFF) << 24 | (data[i+1] & 0xFF) << 16 | (data[i+2] & 0xFF) << 8 | (data[i+3] & 0xFF);
            *(int*)(pixelData + i) = pixel;
            break;
        case 24:
            pixel = (data[i] & 0xFF) << 16 | (data[i+1] & 0xFF) << 8 | (data[i+2] & 0xFF);
            *(int*)(pixelData + i) = pixel;
            break;
        case 16:
            pixel = (data[i] & 0xFF) << 8 | (data[i+1] & 0xFF);
            *(int*)(pixelData + i) = pixel;
            break;
        case 8:
            *(pixelData + i) = (data[i] & 0xFF);
            break;
        default:
            throw GfxException(EGfxErrorIllegalArgument, "Unsupported bit depth");
        }
    }

    if (aMask)
    {
        // Mask data is 1 bit/pixel
        int maskSize = aImage->width() * aImage->height() / 8;
        mAlphaData.fill(0xFF, maskSize);
        mMaskPad = 8;

        if (aImage->hasAlphaChannel())
        {
            QImage alpha = aImage->alphaChannel();
            alpha.convertToFormat(QImage::Format_Mono);

            mAlphaData = QByteArray::fromRawData(reinterpret_cast<const char*>(alpha.scanLine(0)), maskSize);
        }
    }
    else if (mAlpha != -1)
    {
        mAlphaData.fill(mAlpha, aImage->width()*aImage->height());
    }
    else if (aImage->hasAlphaChannel())
    {

    QImage alphaImage = aImage->alphaChannel();
    const int w = alphaImage.width();
    for(int j= 0; j < alphaImage.height(); j++)
        {
        for(int i = 0; i < w; i++)
            {
            mAlphaData[j * w + i] = alphaImage.pixel(i, j) & 0xFF;
            }
        }
    }

    // Create palette
    if (aImage->numColors() > 0)
    {
        // If image has color table set, create indexed palette
        QVector<QRgb> colorTable = aImage->colorTable();
        mPaletteData = new IndexedPaletteDataWrapper(colorTable);
    }
    else
    {
        // Direct palette in use
        PaletteDataWrapper::TDirectData data(0x00FF0000, 0x0000FF00, 0x000000FF);
        mPaletteData = new DirectPaletteDataWrapper(data);
    }
}

void ImageDataWrapperImpl::setPaletteData(PaletteDataWrapper* aPaletteData)
{
    GFX_LOG_FUNC_CALL();
    delete mPaletteData;
    mPaletteData = aPaletteData;
}

bool ImageDataWrapperImpl::isDirect()
{
    GFX_LOG_FUNC_CALL();
    return mPaletteData->isDirect();
}

} // namespace GFX
} // namespace Java
