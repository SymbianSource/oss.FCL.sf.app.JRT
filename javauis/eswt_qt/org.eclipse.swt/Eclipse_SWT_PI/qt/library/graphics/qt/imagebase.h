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

#ifndef IMAGEBASE_H_
#define IMAGEBASE_H_

#include "graphics.h"
#include <QImage>
#include <QTransform>

namespace Java { namespace GFX {

class ImageBase: public Image
    {
public:
    ImageBase(); 
    virtual ~ImageBase();
    
    /**
     * from Image
     * @see Image
     */
    virtual void createBySize(int aWidth, int aHeight, int aFillColor, TImageFormat aFormat) = 0;
    virtual void createFromQImage(const QImage& aImage) = 0;
    virtual void createFromQPixmap(const QPixmap& aPixmap) = 0;
    virtual void createFromImage(Image* aImage, int aX, int aY, int aWidth, int aHeight) = 0;
    virtual void createFromImageData(ImageDataWrapper* aData) = 0;
    virtual void createFromRGB(int* aRgbdata, int aWidth, int aHeight, bool aHasAlpha) = 0;
    virtual void dispose() = 0;
    virtual QPaintDevice* getBindable() = 0;
    virtual TImageFormat getFormat() = 0;
    virtual int getHeight() = 0;
    virtual const QImage* getConstImage() = 0;
    virtual QImage* getImage() = 0;
    virtual const QPixmap* getConstPixmap() = 0;
    virtual QPixmap* getPixmap() = 0;
    virtual void getRgb(int* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight) = 0;
    virtual void getRgb(char* aRgbdata, char* aTransparencyMask,int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat) = 0;
    virtual void getRgb(short* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat) = 0;
    virtual int getWidth() = 0;
    virtual const QImage toConstImage() = 0;
    virtual QImage toImage() = 0;
    virtual QPixmap toPixmap() = 0;
    virtual void transform(TTransform aTransform) = 0;
    virtual TImageType type() = 0;
    virtual int getAlpha();
    virtual bool hasMask();
    virtual bool hasAlphaChannel() = 0;
    
protected:
    /**
     * Writes RGB to given int type data array.
     */
    void doGetRgb(QImage& image, int* aRgbdata, int aOffset, int aScanlength, int aX, int aY, 
                  int aWidth, int aHeight);
    
    /**
     * Writes RGB to given char type data array.
     */
    void doGetRgb(QImage& image, char* aRgbdata, char* aTransparencyMask,int aOffset, int aScanlength, 
                   int aX, int aY, int aWidth, int aHeight, int aFormat);
    
    /**
     * Writes RGB to given short type data array.
     */ 
    void doGetRgb(QImage& image, short* aRgbdata, int aOffset, int aScanlength, int aX, int aY, 
                  int aWidth, int aHeight, int aFormat);
    
    /**
     * Creates transform matrix for given transformation.
     * @param aTransform the transform type to be created
     */
    QTransform generateTransformMatrix(TTransform aTransform);
    
    /**
     * Processes image data and creates QImage with properties
     * defined in the image data.
     * 
     * @param aData Image data to be processed
     */
    QImage imageDataToQImage(ImageDataWrapper* aData);
    
    /**
     * Maps given QImage based image format to those defined in TImageFormat.
     * @param qtImageFormat the image format in Qt defitions
     * @return format in internal type
     */
    TImageFormat mapQtFormatToInternal(QImage::Format qtImageFormat);
    
    /**
     * Maps given internal (TImageFormat) image format to Qt format.
     * @param internalImageFormat the image format in internal type
     * @return format in Qt type
     */
    QImage::Format mapInternalFormatToQt(TImageFormat internalImageFormat);
    
    /**
     * Level of alpha
     */
    int      mAlpha;
    
    /**
     * Mask flag
     */
    bool     mHasMask;
    
    
    };

} // namespace GFX
} // namespace Java

#endif /* IMAGEBASE_H_ */
