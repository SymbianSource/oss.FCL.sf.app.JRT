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
#ifndef TARGETQT_H_
#define TARGETQT_H_

#include <QPaintDevice>
#include <QMainWindow>
#include "graphics.h"

namespace Java { namespace GFX {

class SurfaceImpl: public Surface
{
public:
   /**
    * constructor
    */
    SurfaceImpl();

   /**
    * Destructor
    */
    virtual ~SurfaceImpl();

   /**
    * Clears current target
    */
    void clearTarget();

   /**
    * Gets target in a type that can be bound with QPainter
    * @return QPaintDevice of the current target
    */
    QPaintDevice* getBindable();

    /**
     * If the target is a buffer then this returns the actual target where
     * the buffer will be flushed to.
     */
    QPaintDevice* getBufferFlushTarget();

   /**
    * Gets target height
    * @return height of the target
    */
    int getHeight();

   /**
    * Gets native image QImage
    * @return wrapped QImage or if type is not QImage returns null
    */
    QImage* getImage();

   /**
    * Gets the type of image target, either QPixmap or QImage
    * @return EImage or EPixmap if target is image type otherwise ENone
    */
    TImageType getImageType();

   /**
    * Gets the type of the currently set target.
    * @return TTargetType
    */
    virtual TTargetType getType();

   /**
    * Gets native image QPixmap
    * @return wrapped QPixmap or if type is not QPixmap returns null
    */
    QPixmap* getPixmap();

   /**
    * Gets native QWidget
    * @return wrapped QWidget or if type is not QWidget returns null
    */
    QWidget* getWidget();

   /**
    * Gets target width
    * @return width of the target
    */
    int getWidth();

   /**
    * Sets current target for the surface
    */
    virtual void setTarget(int aTarget, TTargetType aType);

    /**
     * If the target is a buffer then this is used to set the actual target
     * where buffer will be flushed to.
     */
    void setBufferFlushTarget(QPaintDevice* aBufferFlushTarget);

private:
    TTargetType mTargetType;
    TImageType mImageType;
    QPaintDevice* mTarget;
    QPaintDevice* mBufferFlushTarget;
};

} // namespace GFX
} // namespace Java

#endif /*TARGETQT_H_*/
