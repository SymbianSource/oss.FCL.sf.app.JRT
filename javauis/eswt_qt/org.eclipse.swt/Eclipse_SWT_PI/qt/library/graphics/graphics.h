/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial API and implementation
 *******************************************************************************/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QImage>
#include <QPixmap>
#ifdef __SYMBIAN32__
#include <EGL/egl.h>
#endif

class QFont;
class QByteArray;

namespace Java { namespace GFX {

class Buffer;
class GraphicsContext;
class GraphicsFactory;
class Image;
class ImageLoader;
class ImageDataWrapper;
class PaletteDataWrapper;
class WindowSurface;

/**
 * gfx errors
 */
enum TGfxError
{
    EGfxErrorNone                   = 0,
    EGfxErrorNoMemory               = 1,
    EGfxErrorIO                     = 2,
    EGfxErrorIllegalArgument        = 4,
    EGfxErrorIllegalState           = 5,
    EGfxErrorArrayIndexOutOfBounds  = 6
};

/**
 * Rendering target types
 */
enum TTargetType
{
    ETypeNone           =  0,
    ETypeWidget         =  1,
    ETypeImage          =  2,
    ETypeBuffer         =  3,
    ETypeWindowSurface  =  4
};

/**
 * Image pixel formats
 */
enum TImageFormat
{
    EFormatNone                  = 0,
    EFormatRGB32                 = 1,
    EFormatARGB32                = 2,
    EFormatARGB32Premultiplied   = 3,
    EFormatRGB16                 = 4,
    EFormatRGB555                = 5,
    EFormatRGB444                = 6,
    EFormatARGB4444Premultiplied = 7,
    EFormatMONO                  = 8
};

/**
 * Native image types
 */
enum TImageType
{
    ENone    = -1,
    EImage    = 0,
    EPixmap    = 1
};

/**
 * Image transforms
 */
enum TTransform
{
    ETransNone              = 100,
    ETransRot90             = 101,
    ETransRot180            = 102,
    ETransRot270            = 103,
    ETransMirror            = 104,
    ETransMirrorRot90       = 105,
    ETransMirrorRot180      = 106,
    ETransMirrorRot270      = 107,
    ETransFlipHorizontal    = 108,
    ETransFlipVertical      = 109
};

/**
 * Blending modes
 */
enum TBlendingMode
{
    ESrc        = 10,
    ESrcOver    = 11,
    EXor        = 12
};

/**
 * Stroke styles
 */
enum TStrokeStyle
{
    EStrokeNo           = 0,
    EStrokeSolid        = 1,
    EStrokeDot          = 2,
    EStrokeDash         = 3,
    EStrokeDashDot      = 4,
    EStrokeDashDotDot   = 5
};

/**
 * Indices for font metrics data
 */
enum TFontMetricsDataIndex
{
    EFmAscent           = 0,
    EFmAverageCharWidth = 1,
    EFmDescent          = 2,
    EFmHeight           = 3,
    EFmLeading          = 4
};

/**
 * Common indexes for rectangle data arrays
 */
enum TRectIndex
{
    ERectX      = 0,
    ERectY      = 1,
    ERectWidth  = 2,
    ERectHeight = 3
};

/**
 * Window surface types
 *
 */
enum WindowSurfaceType
{
    WsTypeUnknown       = 0x0000,  // unknonwn or uninitialized type
    WsTypeQtImage       = 0x0001,  // QImage
    WsTypeQtPixmap      = 0x0002,  // QPixmap
    WsTypeSymbianBitmap = 0x0004,  // CFbsBitmap
    WsTypeEglSurface    = 0x0016   // EGL surface
};


/**
 * Interface to Buffer.
 */
class Buffer
{
public:
    /**
     * Destructor
     * Use dispose instead of delete
     */
    virtual ~Buffer() {}

    /**
     * Disposes this instance of buffer
     */
    virtual void dispose() = 0;

    /**
     * Retrieves bindable target of Buffer, i.e. QPaintDevice.
     * @return bindable
     */
    virtual QPaintDevice* getBindable() = 0;

    /**
     * Gets the invalid rect, i.e. rectangle containing area where
     * draw operations have affected.
     *
     * Values are stored in aRect array as follows:
     * <ul>
     * <li>aRect[ERectX] - the x coordinate of rect</li>
     * <li>aRect[ERectY] - the y coordinate of rect</li>
     * <li>aRect[ERectWidth] - the width of rect</li>
     * <li>aRect[ERectHeight] - the height of rect</li>
     * </ul>
     *
     * @param aRect Int array where rect data is populated
     */
    virtual void getInvalidRect(int aRect[]) = 0;

    /**
     * Retrieves wrapped buffer object
     * @return QPicture
     */
    virtual QPicture* getPicture() = 0;

};

/**
 * WindowSurface class wraps actual rendering surface for sharing it between java grapchics
 * external renderers like m3g and m2g. It also perfroms needed blitting activations in case
 * external renderer does not directly support current surface type.
 *
 * Example of using WindowSurface
 * <code>
 * // Set supported surface types of external renderer
 * int caps = Java::GXF:SwImage | Java::GFX::PBuffer;
 * // get surface
 * QPaintDevice* surface = WindowSurface.bind(caps);
 * // cast device to actual type
 * switch (WindowSurface.getType)
 * {
 *      case SwImage:
 *           surface = static_cast<QImage*>(surface);
 *           break;
 *       case PBuffer:
 *           surface = static_cast<QGLPixelBuffer*>(surface);
 *
 * </code>
 *
 * After using surface WindowSurface.release() must be called
 */
class WindowSurface
{
public:

    /**
     * Destructor
     * @note use dispose method instead of delete
     */
    virtual ~WindowSurface() {}

    /**
     * Binds this surface for rendering outside GraphicsContext, e.g. rendering by m3g.
     * When rendering has been finished by external renderer release() must be called on this instance.
     *
     * Surface type given to caller depends on the graphics hardware configuration, the
     * graphics system used by QPainter and on the capabilites given on the aCapability parameter.
     * Note that the minimun requirement is that SwImage (QImage) is supported.
     *
     * If caller is for instance capable for rendering on VgImage and QImage it should set value for
     * aCapabilites as shown below. If in this case QPainter is operating in OpenVG (Hw) the returned surface is VgImage,
     * if it is on turn operating on software based raster paintengine returned surface is SwImage (QImage).
     *
     * <code>
     * int caps = Java::GFX::SwImage|Java::GFX::VgImage;
     * </code>
     *
     * IMPORTANT: The caller must not delete QPaintDevice pointer returned from bind -method
     *
     * @param aCapabilies The hints of supported surface types, one or combination of surfacetypes
     *                     defined in WindowSurfaceTypes, Default SwImage
     * @return QPaintDevice to be used as rendering target
     */
    virtual void bind(int aCapabilies = WsTypeQtImage) = 0;

    /**
     * Prepares this window surface for painting. Must be called before painting starts.
     * Should be called by UI toolkits, not by external renderers, like m3g.
     * @param aX The x-coordinate of the paint area
     * @param aY The y-coordinate of the paint area
     * @param aWidth The width of the paint area
     * @param aHeight The height of the paint area
     */
    virtual void beginPaint(int aX, int aY, int aWidth, int aHeight) = 0;
    
    /**
     * Ends painting intiated by call to beginPaint. Must be when painting has ended.
     * Should be called by UI toolkits, not by external renderers, like m3g.
     */
    virtual void endPaint() = 0;
    
    /**
     * Flushes the contents of this window surface to the display. Should be called by 
     * UI toolkits.
     */
    virtual void flush() = 0;
    
    /**
     * Gets the type of the windowSurface
     * @return One of types defined in WindowSurfaceType
     */
    virtual int getType() = 0;

    /**
     * Getter for the QPaintDevice of contained target. Primarily to be used 
     * when drawing to window surface target with QPainter
     * @return The paintDevice of the target
     */
    virtual QPaintDevice* getDevice() = 0;
    
#ifdef __SYMBIAN32__
    /**
     * Getter for egl surface.
     * @return The egl draw surface id, or EGL_NO_SURFACE if the target is not egl type
     */
    virtual EGLSurface getEglSurface() = 0;
    
    /**
     * Getter for current the API that is currently bound to EGL. 
     * In order to get valid return value the type of this window surface 
     * must be WsTypeEglSurface.
     * @return The API that's bound to EGL
     */
    virtual EGLenum getEglApi() = 0;
#endif
    
    /**
     * Getter for QImage
     * @return The target QImage, or NULL it is not the true type of target
     */
    virtual QImage* getQtImage() = 0;
    
    /**
     * Refreshes the window surface data. Refresh needs to be called
     * e.g. when Qt has changed its window surface when switching from sw to hw
     * rendering.
     */
    virtual void refresh() = 0;
    /**
     * Releases external renderer from this instance.
     * Calling this method automatically blits all pixels rendered on the surface to
     * actual target, i.e. to widget or offscreen image.
     */
    virtual void release() = 0;

    /**
     * Disposes this instance of windowsurface
     */
    virtual void dispose() = 0;
    
    // Symbian specific methods
    
#ifdef __SYMBIAN32__    
    /**
     * Getter for symbian bitmap.
     * @return The symbian bitmap if it is available, otherwise NULL
     */
    virtual CFbsBitmap* getSymbianBitmap() = 0;
#endif    
    
    /**
     * Switches to software rendering if the window goes invisible
     * and back to hardware rendering when becoming visible.
     */
    virtual void handleSymbianWindowVisibilityChange(bool aVisible) = 0;
};



/**
 * Interface to graphics context
 */
class GraphicsContext
{
public:
    /**
     * Destructor
     * @note use dispose method instead of delete
     */
    virtual ~GraphicsContext() {}

    /**
     * Gets window surface for currrently bound target
     */
    virtual WindowSurface* getWindowSurface() = 0;

    /**
     * Disposes graphics context, i.e. frees all resources.
     */
    virtual void dispose() = 0;

    /**
     * Binds graphics context to given rendering target.
     * @param aTarget The rendering target handle.
     * @param aType The type of given rendering target.
     * @param aBufferFlushTargetHandle The actual target where the buffer will be flushed to
     * @exception GfxException
     */
    virtual void bindTarget(int aTarget, TTargetType aType, const int& aBufferFlushTargetHandle) = 0;

    /**
     * Renders draw commands stored in given buffer to currently bound target.
     * @param aBuffer The rendering target handle.
     * @exception GfxException
     */
    virtual void render(Buffer* aBuffer) = 0;

    /**
     * Releases current rendering target and frees resources.
     * After calling release target no graphics operations are allowed
     * as there's no target to render to. Note, there is no checking for
     * existing target, so it fails on platform operations.
     */
    virtual void releaseTarget() = 0;

    /**
     * Copies rectangular area from currently bound target to given image target.
     * The copy rectangle top left corned is defined by x and y and bottom right corner
     * by image width and height.
     *
     * @param aImage The image to copy into
     * @param aX The x coordinate of top left corner copy area
     * @param aY The y coordinate of top left corner copy area
     */
    virtual void copyArea(Image* aImage, int aX, int aY) = 0;

    /**
     * Copies rectangular area within currently bound target from source
     * location to destination location.
     *
     * @param aSrcX The top left x coordinate of source rectangle
     * @param aSrcY The top left y coordinate of source rectangle
     * @param aWidth The width of source rectangle
     * @param aHeight The height of source rectangle
     * @param aDestX The top left x coordinate for the copy
     * @param aDestY The top left y coordinate for the copy
     * @param aPaint If to generate paint events for the new and obscured areas
     */
    virtual void copyArea(int aSrcX, int aSrcY, int aWidth, int aHeight, int aDestX, int aDestY, bool aPaint) = 0;

    /**
     * Draws outline of elliptical arc within specified rectangle.
     * Arc is drawn counterclockwise.
     *
     * @param aX The x coordinate of top left corner of arc rectangle
     * @param aY The y coordinate of top left corner of arc rectangle
     * @param aWidth The width of arc rectangle
     * @param aHeight The height of arc rectangle
     * @param aStartAngle The starting angle for arc
     * @param aArcAngle The span angle for arc
     */
    virtual void drawArc(int aX, int aY, int aWidth, int aHeight, int aStartAngle, int aArcAngle) = 0;

    /**
     * Draws an ellipse specified by rectangle.
     *
     * @param aX The x coordinate of top left corner of ellipse rectangle
     * @param aY The y coordinate of top left corner of ellipse rectangle
     * @param aWidth The width of ellipse rectangle
     * @param aHeight The height of ellipse rectangle
     */
    virtual void drawEllipse(int aX, int aY, int aWidth, int aHeight) = 0;

    /**
     * Draws a focus rectangle specified by the parameters.
     *
     * @param aX The x coordinate of top left corner of the rectangle
     * @param aY The y coordinate of top left corner of the rectangle
     * @param aWidth The width of the rectangle
     * @param aHeight The height of the rectangle
     */
    virtual void drawFocus(int aX, int aY, int aWidth, int aHeight) = 0;

    /**
     * Draws an image at (x, y) by copying a part of image into the paint device.
     * The default, (0, 0) (and negative) means all the way to the bottom-right of the image.
     *
     * @param aImage The source from where to draw
     * @param aManipulation The transformation to be applied to the region before it is drawn 
     * @param aTx The top-left x coordinate in the paint device that is to be drawn onto
     * @param aTy The top-left y coordinate in the paint device that is to be drawn onto
     * @param aTw The width of the image that is to be drawn onto.
     * @param aTh The heigth of the image that is to be drawn onto.
     * @param aSx The top-left x coordinate in image that is to be drawn
     * @param aSy The top-left y coordinate in image that is to be drawn
     * @param aSw The width of the image that is to be drawn.
     * @param aSh The heigth of the image that is to be drawn.
     * @see QPainter::drawImage
     */
    virtual void drawImage(Image* aImage, int aManipulation, int aTx, int aTy, int aTw = -1, int aTh = -1,
                           int aSx = 0, int aSy = 0, int aSw = -1, int aSh = -1) = 0;

    /**
     * Draws image to specified cooridnates
     *
     * @param aImage The image that is to be drawn
     * @param x The x-coordinate where to draw
     * @param y The y-coordinate where to draw
     */
    virtual void drawImage(Image* aImage, int x, int y) = 0;

    /**
     * Draws a line between defined points.
     *
     * @param aX1 The x coordinate of staring point
     * @param aY1 The y coordinate of staring point
     * @param aX2 The x coordinate of ending point
     * @param aY2 The y coordinate of ending point
     */
    virtual void drawLine(int aX1, int aY1, int aX2, int aY2) = 0;

    /**
     * Draws a point to specified location.
     *
     * @param aX The x coordinate to draw the point
     * @param aY The y coordinate to draw the point
     */
    virtual void drawPoint(int aX, int aY) = 0;

    /**
     * Draws a closed polygon which is defined integer array of coorinates.
     * Lines are drawn between each consecutive x,y pairs and between first and last point.
     *
     * @param aPointArray An array alternating x and y values which are the verices of the polygon
     * @param aLength The number of point pairs in array
     */
    virtual void drawPolygon(int aPointArray[], int aLength) = 0;

    /**
     * Draws polyline which is defined integer array of coorinates.
     * Lines are drawn between each consecutive x,y pairs, but not between first and last coordinate.
     *
     * @param pointArray An array alternating x and y values which are the corners of the polyline
     * @param aLength The number of point pairs in array
     */
    virtual void drawPolyline(int aPointArray[],int aLength) = 0;

    /**
     * Draws outline rectangle specified by the arguments.
     *
     * @param aX The x coordinate of top left corner of rectangle
     * @param aY The y coordinate of top left corner of rectangle
     * @param aWidth The width of rectangle
     * @param aHeight The height of rectangle
     */
    virtual void drawRect(int aX, int aY, int aWidth, int aHeight) = 0;

    /**
     * Draws a series of RGB+transparency values to specified region
     *
     * Scanlength can be negative which results in reading <code>aRgbData</code> array in reverse direction
     *
     * @param aRgbData an array or ARGB values in format #AARRGGBB
     * @param aRgbDataLength The length of aRgbData array
     * @param aOffset the array index of the first ARGB value
     * @param aScanlenght the relative arra offset between consecutive rows in the <code>aRgbData</code> array
     * @param aX the horizontal location of the region to be rendered
     * @param aY the vertical location of the region to be rendered
     * @param aWidth the width of the region to be rendered
     * @param aHeight the height of the region to be rendered
     * @param aProcessAlpha <code>true</code> if <code>rgbData</code> has an alpha channel, <code>false</code> if all pixels are fully opaque
     * @param aManipulation an transformation made on the region before drawing it
     * @throws GfxException
     */
    virtual void drawRGB(int aRgbData[], int aRgbDataLength, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, bool aProcessAlpha, int aManipulation) = 0;

     /**
     * Draws a series of RGB+transparency values to specified region
     *
     * Scanlength can be negative which results in reading <code>aRgbData</code> array in reverse direction
     *
     * @param aRgbData an array or RGB values in format 1 bit perpixel
     * @param aTransparencyMask an array of mask values
     * @param aRgbDataLength The length of aRgbData array
     * @param aOffset the array index of the first RGB value
     * @param aScanlenght the relative arra offset between consecutive rows in the <code>aRgbData</code> array
     * @param aX the horizontal location of the region to be rendered
     * @param aY the vertical location of the region to be rendered
     * @param aWidth the width of the region to be rendered
     * @param aHeight the height of the region to be rendered
     * @param aManipulation an transformation made on the region before drawing it
     * @param aFormat an image format in which  <code>aRgbData</code> array is stored.
     * @throws GfxException
     */
    virtual void drawRGB(char aRgbData[], char aTransparencyMask[], int aRgbDataLength, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aManipulation, int aFormat) = 0;

      /**
     * Draws a series of RGB+transparency values to specified region
     *
     * Scanlength can be negative which results in reading <code>aRgbData</code> array in reverse direction
     *
     * @param aRgbData an array or aRGB values in 16 bit per pixel
     * @param aTransparencyMask an array of mask values
     * @param aRgbDataLength The length of aRgbData array
     * @param aOffset the array index of the first RGB value
     * @param aScanlenght the relative arra offset between consecutive rows in the <code>aRgbData</code> array
     * @param aX the horizontal location of the region to be rendered
     * @param aY the vertical location of the region to be rendered
     * @param aWidth the width of the region to be rendered
     * @param aHeight the height of the region to be rendered
     * @param aProcessAlpha <code>true</code> if <code>rgbData</code> has an alpha channel, <code>false</code> if all pixels are fully opaque
     * @param aManipulation an transformation made on the region before drawing it
     * @param aFormat an image format in which  <code>aRgbData</code> array is stored.
     * @throws GfxException
     */
    virtual void drawRGB(short aRgbData[], int aRgbDataLength, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, bool aProcessAlpha, int aManipulation, int aFormat) = 0;

    /**
     * Draws outline round-cornered rectangle specified by the arguments.
     *
     * @param aX The x coordinate of top left corner of rectangle
     * @param aY The y coordinate of top left corner of rectangle
     * @param aWidth The width of rectangle
     * @param aHeight The height of rectangle
     * @param aArcWidth The width of the rectangle corner arc
     * @param aArcHeight The heiht of the rectangle corner arc
     */
    virtual void drawRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) = 0;

    /**
     * Draws the given string, using the receiver's current font and foreground color.
     * Test is drawn within the rectangle with origin (x, y), width and height.
     * No tab expansion or carriage return processing will be performed.
     * If isTransparent is true, then the background of the rectangular
     * area where the string is being drawn will not be modified,
     * otherwise it will be filled with the receiver's background color.
     * Alignments and flags can be combined by using the bit-wise or operation.
     *
     * @param aText The string to be drawn
     * @param aX The x coordinate of the top left corner of the rectangular area where the string is to be drawn
     * @param aY The y coordinate of the top left corner of the rectangular area where the string is to be drawn
     * @param aWidth The rectangular area width where the string is to be drawn.
     * If -1 then the width is calculated based on the given <code>aText</code>, <code>aAlignments</code> and <code>aFlags</code>.
     * @param aHeight The rectangular area height where the string is to be drawn.
     * If -1 then the height is calculated based on the given <code>aText</code>, <code>aAlignments</code> and <code>aFlags</code>.
     * @param aLength Length of the given text
     * @param aAlingments Alignments @see Qt::AlignmentFlag
     * @param aFlags Flags @see Qt::TextFlag
     * @param aIsTransparent If true the background will be transparent,
     * otherwise it will be opaque
     */
    virtual void drawString(
        const unsigned short* aText, int aX, int aY, int aWidth, int aHeight,
        int aLength, int aAlignments, int aFlags, bool aIsTransparent) = 0;

    /**
     * Draws given window surface to current target.
     * 
     * @param aSurface The WindowSurface to draw
     * @param aX The x-coordinate of the area to draw
     * @param aY The y-coordinate of the area to draw
     * @param aWidth The width of the area to draw
     * @param aHeigth The height of the area to draw
     */
    virtual void drawWindowSurface(WindowSurface* aSurface, int aX, int aY, int aWidth, int aHeight) = 0;
	
    /**
     * Fill interior of circular or elliptical arc within specified rectangular area.
     *
     * @param aX The x coordinate of top left corner of arc rectangle
     * @param aY The y coordinate of top left corner of arc rectangle
     * @param aWidth The width of arc rectangle
     * @param aHeight The height of arc rectangle
     * @param aStartAngle The starting angle for arc
     * @param aArcAngle The span angle for arc
     */
    virtual void fillArc(int aX, int aY, int aWidth, int aHeight, int aStartAngle, int aArcAngle) = 0;

    /**
     * Fills interior of an ellipse/oval within specified rectangular area, using current color.
     *
     * @param aX The x coordinate of top left corner of ellipse rectangle
     * @param aY The y coordinate of top left corner of ellipse rectangle
     * @param aWidth The width of ellipse rectangle
     * @param aHeight The height of ellipse rectangle
     */
    virtual void fillEllipse(int aX, int aY, int aWidth, int aHeight) = 0;

    /**
     * Fills interior of specified rectangle with a gradient sweeping from left to right or top to bottom
     * from foregroundcolor to backgroundcolor or from backgroundcolor to foregroundcolor depending on swapColors.
     *
     * @param aX The x coordinate of top left corner of rectangle
     * @param aY The y coordinate of top left corner of rectangle
     * @param aWidth The width of rectangle
     * @param aHeight The height of rectangle
     * @param aVertical if true sweeps from top to bottom, else sweeps from left to right
     * @param aSwapColors if true sweep is done from backgroundcolor to foregroundcolor, else from foregroundcolor to backgroundcolor
     */
    virtual void fillGradientRect(int aX, int aY, int aWidth, int aHeight, bool aVertical, bool aSwapColors) = 0;

    /**
     * Fills interior of closed polygon which is specified by array of integer coordinates,
     * using current color. Lines are drawn between each consecutive x,y pairs and between first and last point.
     *
     * @param aPointArray An array alternating x and y values which are the verices of the polygon
     * @param aLength The number of points in <code>aPointarray</code>
     */
    virtual void fillPolygon(int aPointArray[], int aLength) = 0;

    /**
     * Fills interior of the specified rectangle, using current color.
     *
     * @param aX The x coordinate of top left corner of rectangle
     * @param aY The y coordinate of top left corner of rectangle
     * @param aWidth The width of rectangle
     * @param aHeight The height of rectangle
     */
    virtual void fillRect(int aX, int aY, int aWidth, int aHeight) = 0;

    /**
     * Fills interior of round-cornered rectangle, using current color.
     *
     * @param aX The x coordinate of top left corner of rectangle
     * @param aY The y coordinate of top left corner of rectangle
     * @param aWidth The width of rectangle
     * @param aHeight The height of rectangle
     * @param aArcWidth The width of the rectangle corner arc
     * @param aArcHeight The heiht of the rectangle corner arc
     */
    virtual void fillRoundRect(int aX, int aY, int aWidth, int aHeight, int aArcWidth, int aArcHeight) = 0;

    /**
     * Get the width of the specified character in the font which is
     * currently selected into the receiver.
     * @param aCh The character to be measured
     * @param aIsAdvanced If true advanced width of the given character is returned.
     * @return If <code>aIsAdvanced</code> is false return width of the actual character,
     * not including the leading and tailing whitespace or overhang.
     */
    virtual int getCharacterWidth(char aCh, bool aIsAdvanced) = 0;

    /**
     * Gets the <code>FontMetrics</code> data.
     *
     * Values are stored in the data array according to the
     * <code>TFontMetricsDataIndex</code> enum.
     *
     * @param aData The integer array where to copy <code>FontMetrics</code> data.
     * @param aFont If not null then Fontmetrics data will be based on it
     * otherwise data is based on the font which is currently selected.
     */

    virtual void getFontMetricsData(int aData[], const QFont& aFont) = 0;
    /**
     * Gets the <code>FontMetrics</code> data that is
     * on the font which is currently selected.
     *
     * Values are stored in the data array according to the
     * <code>TFontMetricsDataIndex</code> enum.
     *
     * @param aData The integer array where to copy <code>FontMetrics</code> data.
     */
    virtual void getFontMetricsData(int aData[]) = 0;

    /**
     * Gets the alpha component of the background color.
     *
     * @return The alpha value (0-255)
     */
    virtual int getBackgroundAlpha() = 0;

    /**
     * Get the current background color
     * The return value also holds a value for the alpha-channel.
     *
     * @return An ARGB quadruplet on the format #AARRGGBB, equivalent to an unsigned int.
     */
    virtual int  getBackgroundColor() = 0;

    /**
     * Gets current blending mode.
     * Default blending mode is SRC_OVER.
     *
     * @return one of TBlendingMode
     */
    virtual int getBlendingMode() = 0;

    /**
     * Gets the current clip rectangle. Retrieved clip is copied in given integer array which
     * must have at least size of 4, in order to populate all clip values.
     *
     * Values are stored in clip array as follows:
     * <ul>
     * <li>aClipArray[ERectX] - the x coordinate of clip</li>
     * <li>aClipArray[ERectY] - the y coordinate of clip
     * <li>aClipArray[ERectWidth] - the width of clip
     * <li>aClipArray[ERectHeight] - the height of clip
     * </ul>
     * @param clipArray The int array where to copy clip data
     */
    virtual void getClip(int aClipArray[]) = 0;

    /**
     * Gets the alpha component of the foreground color.
     *
     * @return The alpha value (0-255)
     */
    virtual int getForegroundAlpha() = 0;

    /**
     * Get the current foreground color
     * The return value also holds a value for the alpha-channel.
     *
     * @return An ARGB quadruplet on the format #AARRGGBB, equivalent to an unsigned int.
     */
    virtual int  getForegroundColor() = 0;

    /**
     * Returns a bounding box required by the given <code>string</code> when it is
     * drawn inside the given rectangle (<code>rectX</code>, <code>rectY</code>,
     * <code>rectWidth</code>, <code>rectHeight</code>) with the given <code>flags</code>
     * and <code>alignments</code> using the currently set font.
     * If the text does not fit within the given rectangle then the required bounding box
     * is returned.
     *
     * @param aBoundingBox The integer array where to copy bounding box data.
     * Result is stored in the array as follows:
     * <ul>
     * <li>aBoundingBox[ERectX] - top-left x of the text bounding box</li>
     * <li>aBoundingBox[ERectY] - top-left y of the text bounding box</li>
     * <li>aBoundingBox[ERectWidth] - the width of the text bounding box</li>
     * <li>aBoundingBox[ERectHeight] - the height of the text bounding box</li>
     * </ul>
     * @param aText The string
     * @param aTextLength Text length.
     * @param aAlingments Alignments @see Qt::AlignmentFlag
     * @param aFlags Flags @see Qt::TextFlag
     * @param aRectX Drawing rectangle's top-left x coordinate.
     * @param aRectY Drawing rectangle's top-left y coordinate.
     * @param aRectWidth Drawing rectangle width.
     * @param aRectHeight Drawing rectangle height.
     */
    virtual void getTextBoundingBox(
        int aBoundingBox[], const unsigned short* aText, int aTextLength, int aAlignments,
        int aFlags, int aRectX, int aRectY, int aRectWidth, int aRectHeight) = 0;

    /**
     * Gets the stroke width in pixels.
     *
     * @return stroke width in pixels
     */
    virtual int  getStrokeWidth() = 0;

    /**
     * Gets the current pen/line style.
     * @see Qt::PenStyle
     * @return Style
     */
    virtual int  getStrokeStyle() = 0;

    /**
     * Gets the x coorinate of translated origin
     * @return the x coorinate of translated origin
     */
    virtual int getTranslateX() = 0;

    /**
     * Gets the y coorinate of translated origin
     * @return the y coorinate of translated origin
     */
    virtual int getTranslateY() = 0;

    /**
     * Sets alpha component of background color.
     *
     * @param aAlpha The alpha value to be set in range 0-255
     */
    virtual void setBackgroundAlpha(int aAlpha) = 0;

    /**
     * Set the current background color
     *
     * @param aArgb An ARGB quadruplet on the format #AARRGGBB, equivalent to an unsigned int.
     * @param aUpdateAlpha if true alpha values is also changed otherwise not
     */
    virtual void setBackgroundColor(int aArgb, bool aUpdateAlpha) = 0;

    /**
     * Sets blending mode. The blending mode dictates how rendered pixels are combined with the destination pixels.
     * The default blending mode is ESrcOver, i.e. if not other mode is set using this method.
     *
     * If the bound target does not have and alpha channel the set mode has no effect.
     *
     * @param aMode The blending mode to be set
     */
    virtual void setBlendingMode(TBlendingMode aMode) = 0;

    /**
     * Returns if clipped or not.
     * @return True if clipping region is set, and false otherwise
     */
    virtual bool hasClipping() = 0;

    /**
     * Reset clipping region
     */
    virtual void cancelClipping() = 0;

    /**
     * Sets the clip rectangle where draw operations will take affect.
     * Rendering operations have no effect outside of the clipping area.
     *
     * If intersects is set as true then the clip area is set to area where the current and given
     * clip rectangle intersect, i.e. where they overlap.
     *
     * @param aX The x coordinate for the clip area
     * @param aY The y coordinate for the clip area
     * @param aWidth The width of the clip area
     * @param aHeight The height of the clip area
     * @param aIntersects if true clip is set as interscetion of current and given clip rectangle,
     * otherwise the clip is set as specified
     */
    virtual void setClip(int aX, int aY, int aWidth, int aHeight, bool aIntersects) = 0;

    /**
     * Sets font which is used in drawString.
     *
     * @param aFontHandle The handle of native QFont
     */
    virtual void setFont(int aFontHandle) = 0;

    /**
     * Sets alpha component of foreground color.
     *
     * @param aAlpha The alpha value to be set in range 0-255
     */
    virtual void setForegroundAlpha(int aAlpha) = 0;

    /**
     * Set the current foreground color
     *
     * @param aArgb An ARGB quadruplet on the format #AARRGGBB, equivalent to an unsigned int.
     * @param aUpdateAlpha if true alpha values is also changed otherwise not
     */
    virtual void setForegroundColor(int aArgb, bool aUpdateAlpha) = 0;

    /**
     * Sets the stroke width in pixels.
     *
     * @param aWidth The stroke width in pixels
     */
    virtual void setStrokeWidth(int aWidth) = 0;

    /**
     * Sets the stroke style.
     *
     * @param aStyle The style to be set
     */
    virtual void setStrokeStyle(TStrokeStyle aStyle) = 0;

    /**
     * Translates the origin of this graphics context to point (x,y). All coordinates used in subsequet
     * rendering operations are relative to this new origin.
     *
     * @param aX the x coordinate of new origin
     * @param aY the y coordinate of new origin
     */
    virtual void translate(int aX, int aY) = 0;

    /**
     * Scales the coordinate system by (x, y). All coordinates used in subsequent
     * rendering operations are scaled with these values.
     *
     * @param x the x scaling factor.
     * @param y the y scaling factor
     */
    virtual void scale(int aX, int aY) = 0;

    /**
     * Resets any transformations that were made using translate(), scale().
     */
    virtual void resetTransform() = 0;

    /**
     * Saves all the painter settings.
     */
    virtual void saveSettings() = 0;

    /**
     * Restores all the painter settings.
     */
    virtual void restoreSettings() = 0;

};

class Image
{
public:
    /**
     * Destructor
     * @note use dispose method instead of delete
     */
    virtual ~Image() {}

     /**
     * Creates new image with given size.
     * @param aWidth The width of the image to be created.
     * @param aHeight The height of the image to be created.
     * @param aFillColor The initial color for the image in format #00RRGGBB
     * where hight order byte, i.e. alpha is ingored
     * @param aFormat The format for new image, this does not apply to all images.
     * @exception GfxException
     */
    virtual void createBySize(int aWidth, int aHeight, int aFillColor, TImageFormat aFormat) = 0;

    /**
     * Creates new image from given existing image.
     * If the given copy region is empty, the whole image is copied.
     *
     * @param aImage The image to create the copy from.
     * @param aX The top-left x coordinate of the copy region.
     * @param aY The top-left y coordinate of the copy region.
     * @param aWidth The width of the copy region.
     * @param aHeight The height of the copy region.
     * @exception GfxException
     */
    virtual void createFromImage(
        Image* aImage, int aX = 0, int aY = 0, int aWidth = 0, int aHeight = 0) = 0;

    /**
     * Creates new image from given image data.
     *
     * @param aData The image data to create from.
     * @exception GfxException
     */
    virtual void createFromImageData(ImageDataWrapper* aData) = 0;

    /**
     * Creates new image from given <code>rgbData</code> array containing argb values.
     *
     * @param aRgbData a RGB data array where one pixel is specified as 0xAARRGGBB
     * @param aWidth The width of the image
     * @param aHeight The height of the image
     * @param aHasAlpha If true the rgb data has also an alpha channel,
     * otherwise all pixels are fully opaque, e.g. 0xFFRRGGBB.
     */
    virtual void createFromRGB(int* aRgbdata, int aWidth, int aHeight, bool aHasAlpha) = 0;

    /**
     * Disposes image, i.e. frees all related resources.
     */
    virtual void dispose() = 0;

    /**
     * Retrieves bindable target of image, i.e. QPaintDevice.
     * @return bindable
     */
    virtual QPaintDevice* getBindable() = 0;

    /**
     * Gets the format of the image.
     *
     * @return a format in which image is represented.
     */
    virtual int getFormat() = 0;

    /**
     * Gets the height of the image.
     * @return height
     */
    virtual int getHeight() = 0;

    /**
     * Returnes native QPixmap.
     * If the image in guestion is not QPixmap, null is returned.
     * Note that the caller of this method must not delete returned pixmap.
     * @return native QPixmap or null if type is not QPixmap
     */
    virtual QPixmap* getPixmap() = 0;

    /**
     * Copies image rgb (32-bit) data of given region to given data array.
     * @param aRgbdata The array to copy the data into.
     * @param aOffset The offset of image data start in array to copy into.
     * @param aScanlength The relative offset between corresponding pixels in consecutive rows of the region.
     * @param aX The x-coordinate of top-left corner of area to be copied.
     * @param aY The y-coordinate of top-left corner of area to be copied.
     * @param aWidth The width of the area to be copied.
     * @param aHeight The height of the area to be copied.
     * @exception GfxException
     */
    virtual void getRgb(int* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight) = 0;


    /**
     * Copies image rgb (1-bit) data of given region to given data array.
     * @param aRgbdata The array to copy the data into.
     * @param aTransparencyMask The array to copy transparency values into
     * @param aOffset The offset of image data start in array to copy into.
     * @param aScanlength The relative offset between corresponding pixels in consecutive rows of the region.
     * @param aX The x-coordinate of top-left corner of area to be copied.
     * @param aY The y-coordinate of top-left corner of area to be copied.
     * @param aWidth The width of the area to be copied.
     * @param aHeight The height of the area to be copied.
     * @param aFormat The format of an image data.
     * @exception GfxException
     */
    virtual void getRgb(char* aRgbdata, char* aTransparencyMask ,int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat) = 0;

    /**
     * Copies image rgb (16-bit) data of given region to given data array.
     * @param aRgbdata The array to copy the data into.
     * @param aOffset The offset of image data start in array to copy into.
     * @param aScanlength The relative offset between corresponding pixels in consecutive rows of the region.
     * @param aX The x-coordinate of top-left corner of area to be copied.
     * @param aY The y-coordinate of top-left corner of area to be copied.
     * @param aWidth The width of the area to be copied.
     * @param aHeight The height of the area to be copied.
     * @param aFormat The format of an image data.
     * @exception GfxException
     */
    virtual void getRgb(short* aRgbdata, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aFormat) = 0;

    /**
     * Gets the width of the image.
     * @return width
     */
    virtual int getWidth() = 0;

    /**
     * Converts this image to QImage for pixel access.
     * If conversion fails a null image is returned.
     * @return QImage instance created from wrapped Qt's image type.
     */
      virtual QImage toImage() = 0;

    /**
     * Transforms image with given transformation.
     * @param aTransform The transform type
     */
    virtual void transform(TTransform aTransform) = 0;

    /**
     * Retrieves image's native type
     * @return Image native type
     */
    virtual TImageType type() = 0;

    /**
     * Returns true if the image object has the alpha channel, otherwise returns false.
     */
    virtual bool hasAlphaChannel() = 0;

    /**
     * Returns the global alpha value for the image.
     */
    virtual int getAlpha() = 0;

    /**
     * Returns true if image has mask data set (instead of alpha channel).
     */
    virtual bool hasMask() = 0;
};

class ImageLoader
{

public:

    /**
     * Destructor
     * @note use dispose method instead of delete
     */
    virtual ~ImageLoader() {}

    /**
     * Appends data to internal buffer.
     * @param aData Data array containg data to be appended
     * @param aLength Length of bytes to be appended
     * @param aOffset Offset from beginning of data array from where to start append
     * @exception GfxException
     */
    virtual void append(const char* aData, int aLength, int aOffset) = 0;

    /**
     * Creates internal buffer with given initial buffer size.
     * @param aBufferSize Initial size for buffer to be created
     * @exception GfxException
     */
    virtual void beginStream(int aBufferSize) = 0;

    /**
     * Ends data stream and creates image based on configuration.
     * @return Interface for created image.
     * @exception GfxException
     */
    virtual Image* endStream() = 0;

    /**
     * Disposes loader and releases all related resources.
     */
    virtual void dispose() = 0;
    
    /**
     * Loads image from file directly using the native APIs. Java security 
     * checks are bypassed. 
     * @param aFilename The filename to pass to the native APIs. 
     * @return Image The loaded image. 
     */
    virtual Image* load(const QString& aFileName) = 0;
    
    /**
     * The image will be scaled to this size on load. Useful for SVG images. 
     * @param aWidth The width to scale to when the Image is loaded. 
     * @param aHeight The height to scale to when the Image is loaded. 
     */
    virtual void setLoadSize(int aWidth, int aHeight) = 0;
};

/**
 * Interface for wrapping SWT image data structure
 */
class ImageDataWrapper
{
// Definitios
public:

    /**
     * SWT image types
     */
    typedef enum {
        EUndefinedImage = -1, // unknown format image
        EBmpImage = 0, // a Windows BMP format image
        EBmpRleImage = 1, // a run-length encoded Windows BMP format image
        EGifImage = 2, // a GIF format image
        EIcoImage = 3, // a ICO format image
        EJpegImage = 4, // a JPEG format image
        EPngImage = 5 // a PNG format image
    } TImageType;

    /**
     * Choices how to dispose the current GIF image
     * before displaying the next one in SWT
     */
    typedef enum {
        EDmUnspecified = 0, // disposal method is unspecified
        EDmFillNone, // leave the previous image in place
        EDmFillBackground,
        EDmFillPrevious
    } TDisposalMethod;

    /**
     * Data types
     */
    typedef enum {
        EPixelData = 0,
        EAlphaData,
        EMaskData
    } TDataType;

// Methods
public:
    /**
     * Dtor
     */
    virtual ~ImageDataWrapper() {}

    /**
     * Get image width , in pixels
     * @return Width, in pixels
     */
    virtual int getWidth() = 0;

    /**
     * Get image height , in pixels
     * @return Height, in pixels
     */
    virtual int getHeight() = 0;

    /**
     * Set image size, in pixels
     * @param aWidth Width, in pixels
     * @param aHeight, in pixels
     */
    virtual void setSize(int aWidth, int aHeight) = 0;

    /**
     * Get global alpha value to be used for every pixel.
     * @return Alpha value
     */
    virtual int getAlpha() = 0;

    /**
     * Set global alpha value to be used for every pixel.
     * @param aAlpha Global alpha
     */
    virtual void setAlpha(int aAlpha) = 0;

    /**
     * Get number of bytes per scanline.
     * @return Number of bytes per scanline.
     */
    virtual int getBytesPerLine() = 0;

    /**
     * Set number of bytes per scanline.
     * @param aBytesPerLine Number of bytes per scanline.
     */
    virtual void setBytesPerLine(int aBytesPerLine) = 0;

    /**
     * Get delay time before displaying the next image in an animation
     * @return Delay time
     */
    virtual int getDelayTime() = 0;

    /**
     * Set time to delay before displaying the next image in an animation
     * @param aDelayTime Delay time
     */
    virtual void setDelayTime(int aDelayTime) = 0;

    /**
     * Get color depth of the image, in bits per pixel.
     * @return Color depth
     */
    virtual int getDepth() = 0;

    /**
     * Set color depth of the image, in bits per pixel.
     * @param aDepth Color depth
     */
    virtual void setDepth(int aDepth) = 0;

    /**
     * Get number of the scanline pad.
     * @return Scanline pad
     */
    virtual int getScanlinePad() = 0;

    /**
     * Set number of the scanline pad.
     * @param aScanlinePad Scanline pad
     */
    virtual void setScanlinePad(int aScanlinePad) = 0;

    /**
     * Get number of the scanline pad of the mask.
     * @return Mask's scanline pad
     */
    virtual int getMaskPad() = 0;

    /**
     * Set number of the scanline pad of the mask.
     * @param aMaskPad Mask's scanline pad
     */
    virtual void setMaskPad(int aMaskPad) = 0;

    /**
     * Get transparent pixel.
     * @return The transparent pixel
     */
    virtual int getTransparentPixel() = 0;

    /**
     * Set transparent pixel.
     * @param aTransparentPixel Defines the transparent pixel
     */
    virtual void setTransparentPixel(int aTransparentPixel) = 0;

    /**
     * Get top-left x coordinate of the image within the logical screen.
     * @return Top-left corner point.
     */
    virtual int getTopLeftX() = 0;

    /**
     * Get top-left y coordinate of the image within the logical screen.
     * @return Top-left corner point.
     */
    virtual int getTopLeftY() = 0;

    /**
     * Set top-left corner of the image within the logical screen.
     * @param aX Top-left x coordinate
     * @param aY Top-left y coordinate
     */
    virtual void setTopLeftCorner(int aX, int aY) = 0;


    /**
     * Get how to dispose of image before displaying the next.
     * @return Dispose method
     */
    virtual TDisposalMethod getDisposalMethod() = 0;

    /**
     * Set how to dispose of image before displaying the next.
     * @param aDisposalMethod Disposal method
     */
    virtual void setDisposalMethod(int aDisposalMethod) = 0;

    /**
     * Get image type.
     * @return Type
     */
    virtual TImageType getImageType() = 0;

    /**
     * Set image type
     * @param aImageType Image type
     */
    virtual void setImageType(int aImageType) = 0;

    /**
     * Get data
     * @param aType Data type.
     * @return Data
     */
    virtual char* getData(TDataType aType) = 0;

    /**
     * Get data size
     * @param aType Data type.
     * @return Data size
     */
    virtual int getDataSize(TDataType aType) = 0;

    /**
     * Set data
     * @param aSrc Source image
     */
    virtual void setData(Image* aSrc) = 0;

    /**
     * Set data size
     * @param aType Data type.
     * @param aData The QByteArray encapsulating the data
     */
    virtual void setData(ImageDataWrapper::TDataType aType, const QByteArray& aData) = 0;

    /**
     * Get palette data
     * @return Palette data
     */
    virtual PaletteDataWrapper* getPaletteData() = 0;

    /**
     * Set palette data
     * @param aPaletteData Palette data
     */
    virtual void setPaletteData(PaletteDataWrapper* aPaletteData) = 0;

    /**
     * For checking if image color mapping is indexed or direct.
     * @return <code>true</code> if mapping is direct
     */
    virtual bool isDirect() = 0;

};

/**
 * Interface for wrapping SWT palette structure
 */
class PaletteDataWrapper
{
// Data types
public:
    /**
     * Data structure for direct palettes
     *
     * To extract a component from a colour: first apply the mask and then
     * shift the result by the specified value.
     *
     * To combine components in order to build a colour, for each component:
     * first shift by the opposite of the specified value and then apply the
     * mask. Finally, combine all three components with a OR operator.
     *
     * In both cases, a positive value indicates a left shift whereas a
     * negative one indicates a right shift.
     */
    struct TDirectData
    {
        /**
         * Ctor
         */
        TDirectData(int aRMask = 0, int aGMask = 0, int aBMask = 0,
                    int aRShift = 0, int aGShift = 0, int aBShift = 0)
        : mRedMask(aRMask), mGreenMask(aGMask), mBlueMask(aBMask),
          mRedShift(aRShift), mGreenShift(aGShift), mBlueShift(aBShift) {}

        /**
         * Copy ctor
         */
        TDirectData(const TDirectData& aRhs)
        {
            *this = aRhs;
        }

        /**
         * Asignment operator
         */
        TDirectData& operator=(const TDirectData& aRhs)
        {
            if(this != &aRhs)
            {
                mRedMask = aRhs.mRedMask;
                mGreenMask = aRhs.mGreenMask;
                mBlueMask = aRhs.mBlueMask;
                mRedShift = aRhs.mRedShift;
                mGreenShift = aRhs.mGreenShift;
                mBlueShift = aRhs.mBlueShift;
            }
            return *this;
        }

        int mRedMask;
        int mGreenMask;
        int mBlueMask;
        int mRedShift;
        int mGreenShift;
        int mBlueShift;

    };

    /**
     * Indexed rgb data
     */
    struct TIndexedRgb
    {
        /**
         * Ctor
         */
        TIndexedRgb(int aRed = -1, int aGreen = -1, int aBlue = -1)
        : mRed(aRed), mGreen(aGreen), mBlue(aBlue) {}

        /**
         * Copy ctor
         */
        TIndexedRgb(const TIndexedRgb& aRhs)
        {
            *this = aRhs;
        }

        /**
         * Asignment operator
         */
        TIndexedRgb& operator=(const TIndexedRgb& aRhs)
        {
            if(this != &aRhs)
            {
                mRed = aRhs.mRed;
                mGreen = aRhs.mGreen;
                mBlue = aRhs.mBlue;
            }
            return *this;
        }

        int mRed;
        int mGreen;
        int mBlue;
    };

// Methods
public:
    /**
     * Dtor
     */
    virtual ~PaletteDataWrapper() {}

    /**
     * For checking if image color mapping is indexed or direct.
     * @return <code>true</code> if mapping is direct
     */
    virtual bool isDirect() = 0;

    /**
     * Returns direct data.
     * @return Direct data. Null if the palette data is indexed.
     */
    virtual TDirectData* getDirectData() = 0;

    /**
     * Returns number of the indexed data.
     * @return Indexed data count. Zero if the palette data is direct.
     */
    virtual int getIndexedRgbCount() = 0;

    /**
     * Returns indexed data.
     * @param aIndex Defains location of the wanted rgb data.
     * @return Indexed rgb data. The returned structure contains invalid values if the
     * palette is direct.
     */
    virtual TIndexedRgb getIndexedRgb(int aIndex) = 0;


    virtual QVector<QRgb>* getIndexedPalette() = 0;
};

class Surface
{
public:
    //virtual void* getTarget() = 0;
    //virtual void setTarget(int aTarget, TTargetType aType) = 0;
};

/*
 * Exception class for graphics.
 *
 * @param errorCode The graphics errorcode which maps to java exception
 * @param msg The detailed description of exception occured
 *
 * @see jniutils resolveException for java exception mapping
 */
class GfxException {
public:
    GfxException(const int& aErrorCode, const QString aMsg) : mErrorCode(aErrorCode)
    {
        mMsg = new char[aMsg.size()+1];
        strcpy(mMsg, aMsg.toAscii().data());
    };
    //~GfxException() { delete mMsg; };
    int getErrorCode() { return mErrorCode; };
    const char* getMsg() { return (const char*)mMsg; };
private:
    int mErrorCode;
    char* mMsg;
};

/**
 * Interface to graphics factory
 */

class GraphicsFactory
{
public:

    /**
     * Creates a WindowSurface instance for paint device.
     * @param aSurface Target paint device for the window surface
     * @param aType The type of paint device, one of defined types in WindowSurfaceType enum
     */
    static WindowSurface* createWindowSurface(QPaintDevice* aSurface, WindowSurfaceType aType);
    
    /**
     * Creates a WindowSurface instance for window owning widget.
     * @param aWidget Window owning widget
     * @param aAutoRefresh If true the Qt window surface pointer is updated each time
     *                     the bind is called, otherwise not 
     */
    static WindowSurface* createWindowSurface(QWidget* aWidget, bool aAutoRefresh);

    /**
     * Creates a Buffer instance.
     */
    static Buffer* createBuffer();

    /**
     * Creates a graphics context instance.
     */
    static GraphicsContext* createGraphicsContext();

    /**
     * Creates an image with given width, height and initial fillColor.
     *
     * @param aWidth Image width
     * @param aHeight Image height
     * @param aFillColor The initial color for the image in format #00RRGGBB
     * where hight order byte, i.e. alpha is ingored
     */
    static Image* createImage(int aWidth, int aHeight, int aFillColor);

    /**
     * Create a copy based on the given image.
     * If the given copy region is empty, the whole image is copied.
     *
     * @param aImage The source image
     * @param aX The top-left x coordinate of the copy region.
     * @param aY The top-left y coordinate of the copy region.
     * @param aWidth The width of the copy region.
     * @param aHeight The height of the copy region.
     * @see Image::createImage
     */
    static Image* createImage(
        Image* aImage, int aX = 0, int aY = 0, int aWidth = 0, int aHeight = 0);
    
    /**
     * Create image from a QImage
     *
     * @param aImage The source QImage
     */
    static Image* createImage(const QImage& aImage);

    /**
     * Create image from a QPixmap.
     * 
     * @param aImage The source QPixmap
     */
    static Image* createImage(const QPixmap& aPixmap);

    /**
     * Creates an image based on the given ARGB data array.
     * @param aRgbData ARGB data array. One pixel is specified as 0xAARRGGBB.
     * @param aWidth Image width
     * @param aHeight Image height
     * @param aHasAlpha If true the rgb data has also an alpha channel,
     * otherwise all pixels are fully opaque, e.g. 0xFFRRGGBB.
     */
    static Image* createImage(int* aRgbData, int aWidth, int aHeight, bool aHasAlpha);

    /**
     * Creates new imageloader instance
     */
    static ImageLoader* createImageLoader();

    /**
     * TODO
     */
    static Image* createImage(ImageDataWrapper* aImageDataPtr);

    /**
     * TODO
     */
    static ImageDataWrapper* createImageData(Image* aImage = NULL);

    /**
     * TODO
     */
    static PaletteDataWrapper* createPaletteData(PaletteDataWrapper::TDirectData& aDirectData);

    /**
     * TODO
     */
    static PaletteDataWrapper* createPaletteData(QVector<QRgb>& aIndexedData);
};

}}
#endif //GRAPHICS_H
