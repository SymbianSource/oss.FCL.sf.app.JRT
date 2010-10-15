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
#ifndef PIXMAP_H_
#define PIXMAP_H_

#include <QPixmap>
#include "imagebase.h"

namespace Java { namespace GFX {

class Pixmap: public ImageBase
{
public:
    /**
     * Ctor
     */
    Pixmap();

    /**
     * Dtor
     */
    virtual ~Pixmap();

    /**
     * from Image
     * @see Image
     */
    virtual void createBySize(int aWidth, int aHeight, int aFillColor, TImageFormat aFormat);
    virtual void createFromQImage(const QImage& aImage);
    virtual void createFromQPixmap(const QPixmap& aPixmap);
    virtual void createFromImage(Image* aImage, int aX, int aY, int aWidth, int aHeight);
    virtual void createFromImageData(ImageDataWrapper* aData);
    virtual void createFromRGB(int* aRgbdata, int aWidth, int aHeight, bool aHasAlpha);
    virtual void dispose();
    virtual QPaintDevice* getBindable();
    virtual TImageFormat getFormat();
    virtual int getHeight();
    virtual const QImage* getConstImage();
    virtual QImage* getImage();
    virtual const QPixmap* getConstPixmap();
    virtual QPixmap* getPixmap();
    virtual void getRgb(int* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight);
    virtual void getRgb(char* aRgbdata, char* aTransparencyMask,int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat);
    virtual void getRgb(short* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat);
    virtual int getWidth();
    virtual const QImage toConstImage();
    virtual QImage toImage();
    virtual QPixmap toPixmap();
    virtual void transform(TTransform aTransform);
    virtual TImageType type();

    virtual bool hasAlphaChannel();

private:
    QPixmap  mPixmap;
};

} // namespace GFX
} // namespace Java

#endif /*PIXMAP_H_*/
