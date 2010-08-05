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
#include "graphics.h"

namespace Java { namespace GFX {

class Pixmap: public Image
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
    virtual int getFormat();
    virtual int getHeight();
    virtual QPixmap* getPixmap();
    virtual void getRgb(int* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight);
    virtual void getRgb(char* aRgbdata, char* aTransparencyMask,int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat);
    virtual void getRgb(short* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat);
    virtual int getWidth();
    virtual QImage toImage();
    virtual void transform(TTransform aTransform);
    virtual TImageType type();
    virtual int getAlpha();
    virtual bool hasMask();
    virtual bool hasAlphaChannel();

private:
    QPixmap  mPixmap;
    int      mAlpha;
    bool     mHasMask;
};

} // namespace GFX
} // namespace Java

#endif /*PIXMAP_H_*/
