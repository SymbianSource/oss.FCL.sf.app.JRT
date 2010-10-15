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

#ifndef IMAGELOADER_H_
#define IMAGELOADER_H_

#include "graphics.h"

class QImageReader;

namespace Java { namespace GFX {

class ImageloaderImpl : public ImageLoader
{
public:
    /**
     * Ctor
     */
    ImageloaderImpl(TImageType aType);

    /**
     * Dtor
     */
    virtual ~ImageloaderImpl();

    /**
     * from ImageLoader
     * @see ImageLoader
     */
    virtual void append(const char* aData, int aLength, int aOffset);
    virtual void beginStream(int aBufferSize);
    virtual Image* endStream();
    virtual void dispose();
    virtual Image* load(const QString& aFileName);
    virtual void setLoadSize(int aWidth, int aHeight);
    virtual void setResultImageType(TImageType aType);

private:
    /**
     * Helper method for increasing internal buffer size
     * @param newSize The total buffer size to which the buffer sould be grown to
     * @throws GfxException if memory allocation fails
     */
    void growBuffer(int aNewSize);

    /**
     *  Resets bufferData, i.e. frees resources
     */
    void resetData();

    /**
     * Checks the error codes from the reader and throws the appropriate 
     * exception. 
     */
    void throwError(const QImageReader& aReader);
    
    /**
     * If "scale-on-load" size has been set then applies that to the reader. 
     */
    void applyLoadSize(QImageReader& aReader);
    
    /**
     * Struct for buffer related data
     */
    struct bufferData
    {
        /**
         * Pointer to data buffer
         */
        char* buffer;

        /**
         * The total capacity of data buffer
         */
        int capacity;

        /**
         * The total allocation of the data buffer
         */
        int allocation;
    };
    bufferData* mBufferData;
    int loadSizeWidth;
    int loadSizeHeight;
    TImageType mResultImageType;
};

} // namespace GFX
} // namespace Java

#endif /*IMAGELOADER_H_*/
