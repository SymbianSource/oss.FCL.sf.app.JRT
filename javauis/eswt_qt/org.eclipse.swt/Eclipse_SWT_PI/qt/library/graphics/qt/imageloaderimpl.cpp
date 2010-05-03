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
#include <QImageReader>
#include <QBuffer>
#include "imageloaderimpl.h"
#include "graphics.h"
#include "gfxlog.h"

namespace Java { namespace GFX {

ImageloaderImpl::ImageloaderImpl()
: mBufferData(new bufferData), loadSizeWidth(-1), loadSizeHeight(-1)
{
    GFX_LOG_FUNC_CALL();
    mBufferData->buffer = NULL;
    mBufferData->allocation = 0;
    mBufferData->capacity = 0;
}

ImageloaderImpl::~ImageloaderImpl()
{
    GFX_LOG_FUNC_CALL();
    resetData();
    delete mBufferData;
}

void ImageloaderImpl::append(const char* aData, int aLength, int /*aOffset*/)
{
    GFX_LOG_FUNC_CALL();

    // validate input data
    if (!aData)
    {
        // there's nothing to append so return
        return;
    }

    // validate buffer
    Q_ASSERT((mBufferData->buffer != NULL) && "Internal buffer is null when appending");

    // check buffer capacity and increase if needed
    if (mBufferData->capacity < (mBufferData->allocation + aLength) )
    {
        growBuffer(mBufferData->allocation + aLength ); // exception automatically thrown if fails
    }

    // Append input data to internal buffer
    memcpy((char*)(mBufferData->buffer+mBufferData->allocation), aData, aLength);
    // update allocation
    mBufferData->allocation += aLength;
}

void ImageloaderImpl::beginStream(int aBufferSize)
{
    GFX_LOG_FUNC_CALL();

    // Alloc buffer with given size
    mBufferData->buffer = new(std::nothrow) char[aBufferSize];

    // validate allocation
    if(!mBufferData->buffer)
    {
        resetData();
        throw GfxException(EGfxErrorNoMemory, "Buffer creation failed");
    }

    // Set buffer info
    mBufferData->capacity = aBufferSize;
    mBufferData->allocation = 0;
}

Image* ImageloaderImpl::endStream()
{
    GFX_LOG_FUNC_CALL();
    
    // Prepare data & open IOdevice
    QByteArray array = QByteArray::fromRawData(mBufferData->buffer, mBufferData->allocation);
    QBuffer buffer(&array);
    buffer.open(QIODevice::ReadOnly);

    // Initialize imageReader
    QImageReader imgReader(&buffer);
    applyLoadSize(imgReader);

    // Read image and close buffer
    QImage image = imgReader.read();
    buffer.close();

    // Check success of read
    if(image.isNull())
    {
        throwError(imgReader);
    }
    
    Image* img = GraphicsFactory::createImage(image);

    // cleanup, release buffer
    resetData();
    
    return img;
}

void ImageloaderImpl::dispose()
{
    GFX_LOG_FUNC_CALL();
    delete this;
}

Image* ImageloaderImpl::load(const QString& aFileName)
{
    GFX_LOG_FUNC_CALL();

    // Initialize imageReader
    QImageReader imgReader(aFileName);
    applyLoadSize(imgReader);
    
    // Read image and close buffer
    QImage image = imgReader.read();

    // Check success of read
    if(image.isNull())
    {
        throwError(imgReader);
    }

    return GraphicsFactory::createImage(image);
}

void ImageloaderImpl::setLoadSize(int aWidth, int aHeight)
{
    loadSizeWidth = aWidth;
    loadSizeHeight = aHeight;
}

// Private
void ImageloaderImpl::growBuffer(const int aNewSize)
{
    GFX_LOG_FUNC_CALL();

    // create and alloc new bigger buffer
    char* newBuffer = NULL;
    newBuffer = new(std::nothrow) char[aNewSize];

    // validate allocation
    if(!newBuffer)
    {
        resetData();
        throw GfxException(EGfxErrorNoMemory, "Increasing buffer size failed");
    }

    // Copy from old buffer to new one
    memcpy(newBuffer, mBufferData->buffer, strlen(mBufferData->buffer));

    // cleanup and update info
    delete mBufferData->buffer;
    mBufferData->buffer = newBuffer;
    mBufferData->capacity = aNewSize;
}

void ImageloaderImpl::resetData()
{
    if (mBufferData->buffer)
    {
        delete mBufferData->buffer;
    }
    mBufferData->buffer = NULL;
    mBufferData->allocation = 0;
    mBufferData->capacity = 0;
}

void ImageloaderImpl::throwError(const QImageReader& aReader)
{
    switch(aReader.error())
    {
    case QImageReader::UnsupportedFormatError:
        throw GfxException(EGfxErrorIllegalArgument, aReader.errorString());
    case QImageReader::InvalidDataError:
        throw GfxException(EGfxErrorIllegalState, aReader.errorString());
    default:
        throw GfxException(EGfxErrorIO, aReader.errorString());
    }
}

void ImageloaderImpl::applyLoadSize(QImageReader& aReader)
{
    if(loadSizeWidth > 0 && loadSizeHeight > 0)
    {
        aReader.setScaledSize(QSize(loadSizeWidth, loadSizeHeight));
    }
}

} // namespace GFX
} // namespace Java
