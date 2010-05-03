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

#ifndef IMAGEDATAQT_H_
#define IMAGEDATAQT_H_

#include <jni.h>
#include <QVector>
#include <QColor>
#include "graphics.h"

namespace Java { namespace GFX {

/**
 * Helper class for parsing <code>org.eclipse.swt.graphics.ImageData</code>
 */
class ImageDataWrapperImpl : public ImageDataWrapper
{
public:
    /**
     * Ctor
     */
    ImageDataWrapperImpl();

    /**
     * Dtor
     */
    virtual ~ImageDataWrapperImpl();

    /**
     * @see ImageDataWrapper::getWidth
     */
    virtual inline int getWidth()
    {
        return mSize.width();
    }

    /**
     * @see ImageDataWrapper::getHeight
     */
    virtual inline int getHeight()
    {
        return mSize.height();
    }

    /**
     * @see ImageDataWrapper::setSize
     */
    virtual inline void setSize(int aWidth, int aHeight)
    {
        mSize.setWidth(aWidth);
        mSize.setHeight(aHeight);
    }

    /**
     * @see ImageDataWrapper::getAlpha
     */
    virtual inline int getAlpha()
    {
        return mAlpha;
    }

    /**
     * @see ImageDataWrapper::setAlpha
     */
    virtual inline void setAlpha(int aAlpha)
    {
        mAlpha = aAlpha;
    }

    /**
     * @see ImageDataWrapper::getBytesPerLine
     */
    virtual inline int getBytesPerLine()
    {
        return mBytesPerLine;
    }

    /**
     * @see ImageDataWrapper::setBytesPerLine
     */
    virtual inline void setBytesPerLine(int aBytesPerLine)
    {
        mBytesPerLine = aBytesPerLine;
    }

    /**
     * @see ImageDataWrapper::getDelayTime
     */
    virtual inline int getDelayTime()
    {
        return mDelayTime;
    }

    /**
     * @see ImageDataWrapper::setDelayTime
     */
    virtual inline void setDelayTime(int aDelayTime)
    {
        mDelayTime = aDelayTime;
    }

    /**
     * @see ImageDataWrapper::getDepth
     */
    virtual inline int getDepth()
    {
        return mDepth;
    }

    /**
     * @see ImageDataWrapper::setDepth
     */
    virtual inline void setDepth(int aDepth)
    {
        mDepth = aDepth;
    }

    /**
     * @see ImageDataWrapper::getScanlinePad
     */
    virtual inline int getScanlinePad()
    {
        return mScanlinePad;
    }

    /**
     * @see ImageDataWrapper::setScanlinePad
     */
    virtual inline void setScanlinePad(int aScanlinePad)
    {
        mScanlinePad = aScanlinePad;
    }

    /**
     * @see ImageDataWrapper::getMaskPad
     */
    virtual inline int getMaskPad()
    {
        return mMaskPad;
    }

    /**
     * @see ImageDataWrapper::setMaskPad
     */
    virtual inline void setMaskPad(int aMaskPad)
    {
        mMaskPad = aMaskPad;
    }

    /**
     * @see ImageDataWrapper::getTransparentPixel
     */
    virtual inline int getTransparentPixel()
    {
        return mTransparentPixel;
    }

    /**
     * @see ImageDataWrapper::setTransparentPixel
     */
    virtual inline void setTransparentPixel(int aTransparentPixel)
    {
        mTransparentPixel = aTransparentPixel;
    }

    /**
     * @see ImageDataWrapper::getTopLeftX
     */
    virtual inline int getTopLeftX()
    {
        return mTopLeftCorner.x();
    }

    /**
     * @see ImageDataWrapper::getTopLeftY
     */
    virtual inline int getTopLeftY()
    {
        return mTopLeftCorner.y();
    }

    /**
     * @see ImageDataWrapper::setTopLeftCorner
     */
    virtual inline void setTopLeftCorner(int aX, int aY)
    {
        mTopLeftCorner.setX(aX);
        mTopLeftCorner.setY(aY);
    }

    /**
     * @see ImageDataWrapper::getDisposalMethod
     */
    virtual inline TDisposalMethod getDisposalMethod()
    {
        return mDisposalMethod;
    }

    /**
     * @see ImageDataWrapper::setDisposalMethod
     */
    virtual inline void setDisposalMethod(int aDisposalMethod)
    {
        mDisposalMethod = static_cast<TDisposalMethod>(aDisposalMethod);
    }

    /**
     * @see ImageDataWrapper::getImageType
     */
    virtual inline TImageType getImageType()
    {
        return mImageType;
    }

    /**
     * @see ImageDataWrapper::setImageType
     */
    virtual void setImageType(int aImageType)
    {
        mImageType = static_cast<TImageType>(aImageType);
    }

    /**
     * @see ImageDataWrapper::getData
     */
    virtual char* getData(ImageDataWrapper::TDataType aType);

    /**
     * @see ImageDataWrapper::getData
     */
    virtual int getDataSize(ImageDataWrapper::TDataType aType);

    /**
     * @see ImageDataWrapper::setData
     */
    virtual void setData(Image* aSrc);

    /**
     * @see ImageDataWrapper::setData
     */
    void setData(ImageDataWrapper::TDataType aType, const QByteArray& aData);

    /**
     * @see ImageDataWrapper::getPaletteData
     */
    virtual inline PaletteDataWrapper* getPaletteData()
    {
        return mPaletteData;
    }

    /**
     * @see ImageDataWrapper::setPaletteData
     */
    virtual void setPaletteData(PaletteDataWrapper* aPaletteData);

    /**
     * @see ImageDataWrapper::isDirect
     */
    virtual bool isDirect();

protected:

    /**
     * Copy ctor. not implemented.
     */
    ImageDataWrapperImpl(const ImageDataWrapperImpl&);

    /**
     * Assignment operator, Not implemented.
     */
    ImageDataWrapperImpl& operator=(const ImageDataWrapperImpl&);

    /**
     * Fills internal data structures based on a QImage
     */
    void fillData(const QImage* aImage, int aAlpha, bool aMask);

protected:
    QSize mSize; // image size, in pixels
    int mAlpha; // global alpha value to be used for every pixel.
    int mBytesPerLine; // number of bytes per scanline.
    int mDelayTime; // time to delay before displaying the next image in an animation
    int mDepth; // The color depth of the image, in bits per pixel.
    int mMaskPad; // containing the scanline pad of the mask.
    int mScanlinePad; // The scanline padding.
    int mTransparentPixel; // The transparent pixel.
    TDisposalMethod mDisposalMethod; // how to dispose of image before displaying the next.
    TImageType mImageType; // type of file
    QPoint mTopLeftCorner; // top-left corner of the image within the logical screen.
    QByteArray mPixelData; // pixel data of the image
    QByteArray mAlphaData; // alpha data of the image
    QByteArray mMaskData; // mask data of the image
    PaletteDataWrapper* mPaletteData;
};

} // namespace GFX
} // namespace Java

#endif /*IMAGEDATAQT_H_*/
