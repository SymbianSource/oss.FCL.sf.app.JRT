/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#include "javax_microedition_m3g_Image2D.h"

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Image2D__1set
(JNIEnv* aEnv, jclass, jint aHImage2D, jint aX, jint aY, jint aWidth, jint aHeight, jbyteArray aImageArray)
{
    jbyte* imageArray = NULL;
    if (aImageArray)
    {
        imageArray = aEnv->GetByteArrayElements(aImageArray, NULL);
        if (imageArray == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
            return;
        }
    }
    M3G_DO_LOCK
    m3gSetSubImage((M3GImage)aHImage2D, aX, aY, aWidth, aHeight, aImageArray ? aEnv->GetArrayLength(aImageArray) : NULL, imageArray);
    M3G_DO_UNLOCK(aEnv)

    if (imageArray)
    {
        aEnv->ReleaseByteArrayElements(aImageArray, imageArray, JNI_ABORT);
    }
}

static void getImageScanline(const ImageStruct *pImg,
                             M3Gint line,
                             M3Guint *pixels,
                             M3Gbool *trueAlpha)
{
    const CFbsBitmap *bitmap = pImg->color;
    const int width = bitmap->SizeInPixels().iWidth;
    int count = width;
    M3G_ASSERT(count >= 0);

    M3Guint *dst = pixels;

    TDisplayMode mode = bitmap->DisplayMode();
    int stride = bitmap->ScanLineLength(count, mode);
    bitmap->LockHeap();
    const void *srcAddr = ((const char *) bitmap->DataAddress()) + line * stride;

    const CFbsBitmap *mask = pImg->mask;

    // Alpha data is stored in a separate bitmap if present, so first
    // just copy the RGB image data with a default full alpha

    if (mode == EColor16MU || mode == EColor16MA)
    {
        const unsigned char *src = (const unsigned char *) srcAddr;
        while (count--)
        {
            M3Guint argb;
            argb  = (M3Guint)(*src++);  // red -- "src" is a _byte_ pointer to the FbsBitmap's DataAddress
            argb |= ((M3Guint)(*src++)) <<  8;  // green
            argb |= ((M3Guint)(*src++)) << 16;      // blue
            if (mask)
            {
                // if separate alpha bitmap is present, set full alpha and alpha informations
                // will be copied in second pass
                argb |= 0xFF000000L;    // alpha
            }
            else
            {
                // if separate alpha is NULL alpha information must be copied from alpha channel
                // of source bitmap to alpha channel of target bitmap
                argb |= ((M3Guint)(*src)) << 24;    // alpha
            }
            src++;
            *dst++ = argb;
        }
    }
    else if (mode == EColor64K)
    {
        const unsigned short *src = (const unsigned short *) srcAddr;
        while (count--)
        {
            unsigned short pixel = *src++;
            *dst++ = 0xFF000000L
                     | (((pixel & 0x001F) >> 2) | ((pixel & 0x001F) << 3))
                     | (((pixel & 0x0600) >> 1) | ((pixel & 0x07E0) << 5))
                     | (((pixel & 0xE000) << 3) | ((pixel & 0xF800) << 8));
        }
    }
    else if (mode == EColor4K)
    {
        const unsigned short *src = (const unsigned short *) srcAddr;
        while (count--)
        {
            unsigned short pixel = *src++;
            *dst++ = 0xFF000000L
                     | ((pixel & 0x000F) | ((pixel & 0x000F) << 4))
                     | (((pixel & 0x00F0) | ((pixel & 0x00F0) << 4)) << 4)
                     | (((pixel & 0x0F00) | ((pixel & 0x0F00) << 4)) << 8);
        }
    }
    else
    {
        M3G_ASSERT(M3G_FALSE);
    }
    bitmap->UnlockHeap();

    // Check for the presence of an alpha mask, and if present, do a
    // second pass to mix that into the RGB values already copied

    *trueAlpha = (mask != NULL);

    if (mask)
    {

        mask->LockHeap();

        // NOTE: we assume that if the alpha mask is using an RGBA
        // display mode as the base bitmap, the alpha value is stored
        // in the red color channel of the mask only.
        //

        dst = pixels;
        count = width;

        mode = mask->DisplayMode();
        stride = mask->ScanLineLength(mask->SizeInPixels().iWidth, mode);
        srcAddr = ((const char *) mask->DataAddress()) + line * stride;

        if (mode == EColor16MU || mode == EColor16MA)
        {
            const unsigned long *src = (const unsigned long *) srcAddr;
            while (count--)
            {
                unsigned short mask = *src++;
                *dst++ = (*dst & 0x00FFFFFF) | ((mask & 0x00FF) << 24);
            }
        }
        else if (mode == EColor64K)
        {
            const unsigned short *src = (const unsigned short *) srcAddr;
            while (count--)
            {
                unsigned short mask = *src++;
                *dst++ = (*dst & 0x00FFFFFF) |
                         ((((mask & 0x001F) >> 2) | ((mask & 0x001F) << 3)) << 24);
            }
        }
        else if (mode == EColor4K)
        {
            const unsigned short *src = (const unsigned short *) srcAddr;
            while (count--)
            {
                unsigned short mask = *src++;
                *dst++ = (*dst & 0x00FFFFFF) | (((mask & 0x000F) | ((mask & 0x000F) << 4)) << 24);
            }
        }
        else if (mode == EGray256)
        {
            const unsigned char *src = (const unsigned char *) srcAddr;
            while (count--)
            {
                *dst++ = (*dst & 0x00FFFFFF) | ((*src++) << 24);
            }
        }
        else if (mode == EGray2)
        {

            // Initialize src as byte type
            const unsigned char *src = (const unsigned char *) srcAddr;

            /* Go through each pixel in the stride one by one.
            /  Each src (EGray2 type bitmap) byte holds 8 pixels aplha mask, i.e 1 Bpp,
            /  so check each bit in the byte and set aplha for the dst bitmap either to 0xFF
            /  or to 0x00 depending source bit values 1/0.
            */

            /* Counter for the bits in one byte, i.e. 0-7 */
            int bit = 0;

            /* count = number of pixels in one stride */
            while (count)
            {
                while (bit < 8)
                {
                    *dst++ = (*dst & 0x00FFFFFF) | (((*src) & 0x01 << bit) ? 0xFF << 24 : 0x0);
                    bit++;
                    count--;

                    /* If we have processed all pixels in the image scanline,
                    /  exit and discard rest of the bits in the byte.
                    */

                    if (count <= 0) break;
                }
                /* reset bit counter as we move on to the next byte */
                bit = 0;
                *src++;
            }
        }
        else
        {
            M3G_ASSERT(M3G_FALSE);
        }
        mask->UnlockHeap();
    }
}

static void createImage(M3GInterface m3g,
                        M3GImage *img,
                        M3GImageFormat format,
                        ImageStruct *pImage)
{
    M3GImage image;

    TSize sz = pImage->color->SizeInPixels();
    M3Gint width = sz.iWidth, height = sz.iHeight;

    image = m3gCreateImage(m3g, format, width, height, 0);
    if (image == NULL)
    {
        img = NULL;
        return;    // exception automatically raised
    }

    {
        M3Gint y;

        //M3Guint *tempPixels = (M3Guint *) User::Alloc(width * 4);
        M3Guint *tempPixels = (M3Guint *) malloc(width * 4);
        if (tempPixels == NULL)
        {
            m3gDeleteObject((M3GObject) image);
            *img = NULL;
            return;
        }

        for (y = 0; y < height; ++y)
        {
            M3Gbool trueAlpha;
            getImageScanline(pImage, y, tempPixels, &trueAlpha);
            m3gSetImageScanline(image, y, trueAlpha, tempPixels);
        }
        m3gCommitImage(image);

        //User::Free(tempPixels);
        free(tempPixels);
        *img = image;
    }
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Image2D__1ctorImage
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aHM3g, jint aFormat, jint aImageHandle)
{
    if (aImageHandle != NULL)
    {
        ImageStruct imageStruct;
        MMIDImage* image = JavaUnhand<MMIDImage>(aImageHandle);
        MMIDBitmapImage* bitmapImage = image->BitmapImage();
        if (!bitmapImage)
        {
            return NULL;
        }
        imageStruct.color = bitmapImage->ColorBitmap();
        imageStruct.mask = bitmapImage->AlphaBitmap();
        bitmapImage->RemoveRef();

        CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
        M3GInterface m3g = (M3GInterface)aHM3g;

        M3GImage img;

        M3G_DO_LOCK
        eventSource->ExecuteV(&createImage,
                              m3g, &img, (M3GImageFormat) aFormat, &imageStruct);
        M3G_DO_UNLOCK(aEnv)

        return (jint)img;
    }

    return 0;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Image2D__1getFormat
(JNIEnv* aEnv, jclass, jint aHImage2D)
{
    M3G_DO_LOCK
    jint format = (jint)m3gGetFormat((M3GImage)aHImage2D);
    M3G_DO_UNLOCK(aEnv)
    return format;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Image2D__1ctorSizePixelsPalette
(JNIEnv* aEnv, jclass, jint aM3g, jint aFormat, jint aWidth, jint aHeight, jbyteArray aImage, jbyteArray aPalette)
{
    M3GImageFormat format = (M3GImageFormat)aFormat;

    int bpp = jsr184BytesPerPixel(format);

    if (validateArray(aEnv, aImage, aWidth * aHeight))
    {
        if (aPalette == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/NullPointerException");
            return 0;
        }
        int paletteLen = aEnv->GetArrayLength(aPalette);
        if ((paletteLen < 256 *(unsigned)bpp) &&
                (paletteLen % (unsigned)bpp != 0))
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/IllegalArgumentException");
            return 0;
        }
        else
        {
            M3G_DO_LOCK

            M3GImage hImg = m3gCreateImage((M3GInterface)aM3g,
                                           format,
                                           aWidth, aHeight,
                                           M3G_PALETTED);
            if (hImg != NULL)
            {
                jbyte* palette = NULL;

                int numEntries = paletteLen / bpp;
                if (numEntries > 256)
                {
                    numEntries = 256;
                }

                jbyte* image = aEnv->GetByteArrayElements(aImage, NULL);
                if (image == NULL)
                {
                    M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
                    return 0;
                }

                m3gSetImage(hImg, image);

                palette = aEnv->GetByteArrayElements(aPalette, NULL);
                if (palette == NULL)
                {
                    if (image)
                    {
                        aEnv->ReleaseByteArrayElements(aImage, image, JNI_ABORT);
                    }
                    M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
                    return 0;
                }

                m3gSetImagePalette(hImg, numEntries, palette);
                m3gCommitImage(hImg);

                if (image)
                {
                    aEnv->ReleaseByteArrayElements(aImage, image, JNI_ABORT);
                }
                if (palette)
                {
                    aEnv->ReleaseByteArrayElements(aPalette, palette, JNI_ABORT);
                }
            }
            M3G_DO_UNLOCK(aEnv)
            return ((unsigned) hImg);
        }
    }
    return 0;
}

JNIEXPORT jboolean JNICALL Java_javax_microedition_m3g_Image2D__1isMutable
(JNIEnv* aEnv, jclass, jint aHImage2D)
{
    M3G_DO_LOCK
    jboolean isMutable = (jboolean)m3gIsMutable((M3GImage)aHImage2D);
    M3G_DO_UNLOCK(aEnv)
    return isMutable;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Image2D__1getHeight
(JNIEnv* aEnv, jclass, jint aHImage2D)
{
    M3G_DO_LOCK
    jint height = (jint)m3gGetHeight((M3GImage)aHImage2D);
    M3G_DO_UNLOCK(aEnv)
    return height;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Image2D__1ctorSize
(JNIEnv* aEnv, jclass, jint aM3g, jint aFormat, jint aWidth, jint aHeight)
{
    M3G_DO_LOCK
    jint handle = (M3Guint) m3gCreateImage((M3GInterface)aM3g,
                                           (M3GImageFormat)aFormat,
                                           aWidth, aHeight,
                                           M3G_DYNAMIC|M3G_RENDERING_TARGET);
    M3G_DO_UNLOCK(aEnv)
    return handle;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Image2D__1getWidth
(JNIEnv* aEnv, jclass, jint aHImage2D)
{
    M3G_DO_LOCK
    jint width = (jint)m3gGetWidth((M3GImage)aHImage2D);
    M3G_DO_UNLOCK(aEnv)
    return width;
}


JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Image2D__1ctorSizePixels
(JNIEnv* aEnv, jclass, jint aM3g, jint aFormat, jint aWidth, jint aHeight, jbyteArray aImage)
{
    M3GImageFormat format = (M3GImageFormat)aFormat;

    if (validateArray(aEnv, aImage, jsr184BytesPerPixel(format) * aWidth * aHeight))
    {
        M3G_DO_LOCK

        M3GImage hImg = m3gCreateImage((M3GInterface)aM3g, format, aWidth, aHeight, 0);
        if (hImg != NULL)
        {
            M3GImageFormat format = (M3GImageFormat)aFormat;

            int bpp = jsr184BytesPerPixel(format);
            jbyte* imageScanline = (jbyte*)malloc(aWidth * bpp);
            for (int i=0; i < aHeight; i++)
            {
                aEnv->GetByteArrayRegion(aImage, aWidth * i * bpp, aWidth * bpp, imageScanline);
                m3gSetSubImage(hImg, 0, i, aWidth, 1, aWidth * bpp, imageScanline);
            }
            m3gCommitImage(hImg);

            free(imageScanline);
        }
        M3G_DO_UNLOCK(aEnv)
        return (unsigned) hImg;
    }
    return 0;
}
