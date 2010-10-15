/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
#ifndef GRAPHICSCONTEXTQT_H_
#define GRAPHICSCONTEXTQT_H_

#include <QPaintDevice>
#include <QMainWindow>
#include <QPainter>
#include <QPen>
#include <QFont>
#include "graphics.h"
#include "surfaceimpl.h"
#include "targetobserver.h"

namespace Java { namespace GFX {

class GraphicsContextImpl: public GraphicsContext
{
public:
    /**
     * Constructor
     */
    GraphicsContextImpl();

    /**
     * Destructor
     */
    ~GraphicsContextImpl();

    /**
     * From GraphicsContext
     * @see GraphicsContext
     */
     virtual WindowSurface* getWindowSurface();

    virtual void dispose();
    virtual void bindTarget(int aTarget, TTargetType aType, const int& aBufferFlushTargetHandle);
    virtual void render(Buffer* aBuffer);
    virtual void releaseTarget();
    virtual void cancelClipping();
    virtual void copyArea(Image* aImage, int aX, int aY);
    virtual void copyArea(int aSrcX, int aSrcY, int aWidth, int aHeight, int aDestX, int aDestY, bool aPaint);
    virtual void drawArc(int aX, int aY, int aWidth, int aHeight, int aStartAngle, int aArcAngle);
    virtual void drawEllipse(int aX, int aY, int aWidth, int aHeight);
    virtual void drawFocus(int aX, int aY, int aWidth, int aHeight);
    virtual void drawImage(Image* aImage, int x, int y);
    virtual void drawImage(Image* aImage, int aManipulation, int aTx, int aTy, int aTw, int aTh, int aSx, int aSy, int aSw, int aSh);
    virtual void drawLine(int aX1, int aY1, int aX2, int aY2);
    virtual void drawPoint(int aX, int aY);
    virtual void drawPolygon(int aPointArray[], int aLength);
    virtual void drawPolyline(int aPointArray[], int aLength);
    virtual void drawRect(int aX, int aY, int aWidth, int aHeight);
    virtual void drawRGB(int aRgbData[], int aRgbDataLength, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, bool aProcessAlpha, int aManipulation);
    virtual void drawRGB(char aRgbData[], char aTransparencyMask[], int aRgbDataLength, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, int aManipulation, int aFormat);
    virtual void drawRGB(short aRgbData[], int aRgbDataLength, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight, bool aProcessAlpha, int aManipulation, int aFormat);
    virtual void drawRoundRect(int aX, int aY, int aWidth, int aHeight, int aArcWidth, int aArcHeight);
    virtual void drawString(
        const unsigned short* aText, int aX, int aY, int aWidth, int aHeight,
        int aLength, int aAlignments, int aFlags, bool aIsTransparent);
    virtual void drawWindowSurface(WindowSurface* aSurface, int aX, int aY, int aWidth, int aHeight);
    virtual void fillArc(int aX, int aY, int aWidth, int aHeight, int aStartAngle, int aArcAngle);
    virtual void fillEllipse(int aX, int aY, int aWidth, int aHeight);
    virtual void fillGradientRect(int aX, int aY, int aWidth, int aHeight, bool aVertical, bool aSwapColors);
    virtual void fillPolygon(int aPointArray[], int aLength);
    virtual void fillRect(int aX, int aY, int aWidth, int aHeight);
    virtual void fillRoundRect(int aX, int aY, int aWidth, int aHeight, int aArcWidth, int aArcHeight);
    virtual int  getBackgroundAlpha();
    virtual int  getBackgroundColor();
    virtual int  getBlendingMode();
    virtual int  getCharacterWidth(char aCh, bool aIsAdvanced);
    virtual void getClip(int aClipArray[]);
    virtual void getFontMetricsData(int aData[], const QFont& aFont);
    virtual void getFontMetricsData(int aData[]);
    virtual int  getForegroundAlpha();
    virtual int  getForegroundColor();
    virtual int  getStrokeWidth();
    virtual int  getStrokeStyle();
    virtual int  getTranslateX();
    virtual int  getTranslateY();
    virtual void getTextBoundingBox(
        int aBoundingBox[], const unsigned short* aText, int aTextLength, int aAlignments,
        int aFlags, int aRectX, int aRectY, int aRectWidth, int aRectHeight);
    virtual bool hasClipping();
    virtual void setBackgroundAlpha(int aAlpha);
    virtual void setBackgroundColor(int aArgb, bool aUpdateAlpha);
    virtual void setBlendingMode(TBlendingMode aMode);
    virtual void setClip(int aX, int aY, int aWidth, int aHeight, bool aIntersects);
    virtual void setFont(int aFontHandle);
    virtual void setForegroundAlpha(int aAlpha);
    virtual void setForegroundColor(int aArgb, bool aUpdateAlpha);
    virtual void setStrokeWidth(int aWidth);
    virtual void setStrokeStyle(TStrokeStyle aStyle);
    virtual void translate(int aX, int aY);
    virtual void scale(int aX, int aY);
    virtual void resetTransform();
    virtual void saveSettings();
    virtual void restoreSettings();

    /**
    * Returns trasformed image
    *
    * @param image image to perform transformation on
    * @param manuplation indicates what kind of transformation is needed
    */
    QImage doTransform(QImage image, int manipulation);

private:

    /**
     * Checks if background color needs to be updated and updates it if necessary
     */
    void checkBgColor();

    /**
     * Checks if foreground color needs to be updated and updates it if necessary
     */
    void checkFgColor();

    /**
     * Converts given color to QColor
     *
     * @param aColor The color to be converted
     * @return QColor instance of given color
     */
    QColor convertArgbToQColor(unsigned int aColor);

    /**
     * Validates that given <code>aPrt</code> is with range defined by aStartPtr and aEndPtr
     *
     * @param aStartPrt pointer to start of memory area
     * @param aEndPrt pointer to end of memory area
     * @param aPrt the pointer to check
     * @return true if given ptr is with in bounds othewise false
     */
    bool isPtrWithinRange(const int* aStartPtr, const int* aEndPtr, const int* aPrt);

    /**
     * Performs clean for graphicsContext, i.e. deletes all allocations
     * and resets state.
     */
    void doCleanup();

    /**
     * Applies settings for painter needed when filling primitives
     * Settings:
     * Brush style -> Qt::SolidPattern
     * Brush color -> mBackground
     * Pen color -> mBackground
     */
    void setBrushForFill();

    /**
     * If SWT Control has a buffer then this synchronously flushes it.
     * @param aWidget The Widget which is the Control.handle
     * @param The area of the widget to draw. Reported to Qt as the invalid area when flushing.
     */
    void flushBufferToSwtWidget(QWidget* aWidget, const QRect& aPaintArea);

    /**
     * Gets the location of the widget in its window.
     */
    QPoint widgetOffsetInWindow(QWidget* aWidget);

    /**
     * Draws QImage to another (or the same) QImage handling also the case
     * where the source and target areas overlap. QPainter::drawImage doesn't
     * handle this. If called with different source and target images or
     * non-overlapping areas then simply calls QPainter::drawImage.
     */
    static void drawImageCheckOverlap(QPainter& aPainter,
            const QPaintDevice& aTargetDevice, const QRect& aTargetRect,
            const QImage& aSourceImage, const QRect& aSourceRect);

    /**
     * Draws QPixmap to another (or the same) QPixmap handling also the case
     * where the source and target areas overlap. QPainter::drawPixmap doesn't
     * handle this. If called with different source and target pixmaps or
     * non-overlapping areas then simply calls QPainter::drawPixmap.
     */
    static void drawPixmapCheckOverlap(QPainter& aPainter,
            const QPaintDevice& aTargetDevice, const QRect& aTargetRect,
            const QPixmap& aSourceImage, const QRect& aSourceRect);

private:
    SurfaceImpl mSurface;
    QPainter* mPainter;
    QPen* mPen;
    QColor mBackground;
    QColor mForeground;
    bool mBgColorDirty;
    bool mFgColorDirty;
    TargetObserver mObserver;
};

} // namespace GFX
} // namespace Java

#endif /*GRAPHICSCONTEXTQT_H_*/
