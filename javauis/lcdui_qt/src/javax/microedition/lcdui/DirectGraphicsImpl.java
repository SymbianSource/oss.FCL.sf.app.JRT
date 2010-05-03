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
package javax.microedition.lcdui;

import javax.microedition.lcdui.game.Sprite;

import com.nokia.mid.ui.DirectGraphics;

/**
 * DirectGraphics interface implementation class.
 *
 * @see DirectGraphics
 */
class DirectGraphicsImpl implements DirectGraphics {

    private static final int MANIPULATION_MASK = 0x0FFF;

    private Graphics graphics;

    private int alpha;

    DirectGraphicsImpl(Graphics g) {
        graphics = g;
    }

    public void drawImage(Image img, int x, int y, int anchor,
            int manipulation) {

        int transform = getTransformation(manipulation);
        if (transform < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_MANIPULATION);
        }
        graphics.drawRegion(img, 0, 0, img.getWidth(), img.getHeight(),
                transform, x, y, anchor);
    }

    public void drawPixels(final int[] pixels, final boolean transparency,
            final int offset, final int scanlength, final int x, final int y,
            final int width, final int height, final int manipulation,
            final int format) {

        int transform = getTransformation(manipulation);
        if (transform < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_MANIPULATION);
        }
        if (format != TYPE_INT_888_RGB && format != TYPE_INT_8888_ARGB) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_FORMAT);
        }

        final boolean processAlpha = (format == TYPE_INT_8888_ARGB);
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                graphics.getGc().drawRGB(pixels, offset, scanlength,
                        x, y, width, height, processAlpha,
                        getNativeTransformValue(manipulation));
            }
        });
    }

    public void drawPixels(final byte[] pixels, final byte[] transparencyMask,
            final int offset, final int scanlength, final int x, final int y,
            final int width, final int height, final int manipulation,
            final int format) {

        int transform = getTransformation(manipulation);
        if (transform < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_MANIPULATION);
        }
        if (format != TYPE_BYTE_1_GRAY && format != TYPE_BYTE_2_GRAY) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_FORMAT);
        }

        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                graphics.getGc().drawRGB(pixels, transparencyMask, offset, scanlength,
                        x, y, width, height, getNativeTransformValue(manipulation),
                        getNativeFormatValue(format));
            }
        });
    }

    public void drawPixels(final short[] pixels, final boolean transparency,
            final int offset, final int scanlength, final int x, final int y,
            final int width, final int height, final int manipulation,
            final int format) {

        int transform = getTransformation(manipulation);
        if (transform < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_MANIPULATION);
        }
        if (format != TYPE_USHORT_4444_ARGB
                && format != TYPE_USHORT_444_RGB
                && format != TYPE_USHORT_565_RGB
                && format != TYPE_USHORT_555_RGB) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_FORMAT);
        }

        final boolean processAlpha = (format == TYPE_USHORT_4444_ARGB);
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                graphics.getGc().drawRGB(pixels, offset, scanlength,
                        x, y, width, height, processAlpha,
                        getNativeTransformValue(manipulation),
                        getNativeFormatValue(format));
            }
        });
    }

    public void drawPolygon(int[] xPoints, int xOffset, int[] yPoints,
            int yOffset, int nPoints, int argbColor) {
        setARGBColor(argbColor);
        final int[] points = new int[nPoints * 2];
        for (int i = 0; i < nPoints; i++) {
            points[i * 2] = xPoints[xOffset + i];
            points[(i * 2) + 1] = yPoints[yOffset + i];
        }
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                graphics.getGc().drawPolygon(points);
            }
        });
    }

    public void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3,
            int argbColor) {
        setARGBColor(argbColor);
        final int[] points = {x1, y1, x2, y2, x3, y3};
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                graphics.getGc().drawPolygon(points);
            }
        });
    }

    public void fillPolygon(int[] xPoints, int xOffset, int[] yPoints,
            int yOffset, int nPoints, int argbColor) {
        setARGBColor(argbColor);
        final int[] points = new int[nPoints * 2];
        for (int i = 0; i < nPoints; i++) {
            points[i * 2] = xPoints[xOffset + i];
            points[(i * 2) + 1] = yPoints[yOffset + i];
        }
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                graphics.getGc().fillPolygon(points);
            }
        });
    }

    public void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3,
            int argbColor) {
        setARGBColor(argbColor);
        final int[] points = {x1, y1, x2, y2, x3, y3};
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                graphics.getGc().fillPolygon(points);
            }
        });
    }


    public void getPixels(final int[] pixels, final int offset,
            final int scanlength, final int x, final int y, final int width,
            final int height, final int format) {
        if (format != TYPE_INT_888_RGB) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_FORMAT);
        }
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException(
                   MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_WIDTH_HEIGHT);
        }
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                org.eclipse.swt.internal.qt.graphics.Image cgImg;
                Object target = graphics.getGc().getBoundTarget();
                if (target != null && target instanceof org.eclipse.swt.internal.qt.graphics.Image) {
                    cgImg = (org.eclipse.swt.internal.qt.graphics.Image) target;
                }
                else {
                    cgImg = new org.eclipse.swt.internal.qt.graphics.Image(width, height);
                    // TODO: in future the copyArea() signature will change
                    graphics.getGc().copyArea(cgImg, x, y);
                }
                cgImg.getRGB(pixels, offset, scanlength, x, y, width, height);
                if (target == null || !(target instanceof org.eclipse.swt.internal.qt.graphics.Image)) {
                    cgImg.dispose();
                }
            }
        });
    }

    public void getPixels(final byte[] pixels, final byte[] transparencyMask,
            final int offset, final int scanlength, final int x, final int y,
            final int width, final int height, final int format) {
        if (format != TYPE_BYTE_1_GRAY) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_FORMAT);
        }
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException(
                   MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_WIDTH_HEIGHT);
        }
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                org.eclipse.swt.internal.qt.graphics.Image cgImg;
                Object target = graphics.getGc().getBoundTarget();
                if (target != null && target instanceof org.eclipse.swt.internal.qt.graphics.Image) {
                    cgImg = (org.eclipse.swt.internal.qt.graphics.Image) target;
                }
                else {
                    cgImg = new org.eclipse.swt.internal.qt.graphics.Image(width, height);
                    graphics.getGc().copyArea(cgImg, x, y);
                }
                cgImg.getRGB(pixels, transparencyMask, offset, scanlength, x, y, width,
                        height, getNativeFormatValue(format));
                if (target == null || !(target instanceof org.eclipse.swt.internal.qt.graphics.Image)) {
                    cgImg.dispose();
                }
            }
        });
    }

    public void getPixels(final short[] pixels, final int offset,
            final int scanlength, final int x, final int y, final int width,
            final int height, final int format) {
        if (format != TYPE_USHORT_4444_ARGB
            && format != TYPE_USHORT_444_RGB
            && format != TYPE_USHORT_565_RGB
            && format != TYPE_USHORT_555_RGB) {
            throw new IllegalArgumentException(
                    MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_FORMAT);
        }
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException(
                   MsgRepository.DIRECTGRAPHICS_EXCEPTION_INVALID_WIDTH_HEIGHT);
        }
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                org.eclipse.swt.internal.qt.graphics.Image cgImg;
                Object target = graphics.getGc().getBoundTarget();
                if (target != null && target instanceof org.eclipse.swt.internal.qt.graphics.Image) {
                    cgImg = (org.eclipse.swt.internal.qt.graphics.Image) target;
                }
                else {
                    cgImg = new org.eclipse.swt.internal.qt.graphics.Image(width, height);
                    graphics.getGc().copyArea(cgImg, x, y);
                }
                cgImg.getRGB(pixels, offset, scanlength, 0, 0, width, height, getNativeFormatValue(format));
                if (target == null || !(target instanceof org.eclipse.swt.internal.qt.graphics.Image)) {
                    cgImg.dispose();
                }
            }
        });
    }

    public void setARGBColor(int argbColor) {
        alpha = (argbColor >> 24) & Graphics.COMPONENT_MASK;
        graphics.setColor(argbColor);
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                graphics.getGc().setForegroundAlpha(alpha);
                graphics.getGc().setBackgroundAlpha(alpha);
            }
        });
    }

    public int getAlphaComponent() {
        return alpha;
    }

    public int getNativePixelFormat() {
        // TODO: is this the native pixel format ?
        return TYPE_INT_8888_ARGB;
    }

    private static int getTransformation(int manipulation) {
        // manipulations are C-CW and sprite rotations are CW
        int ret = -1;
        int rotation = manipulation & MANIPULATION_MASK;
        if ((manipulation & FLIP_HORIZONTAL) != 0) {
            if ((manipulation & FLIP_VERTICAL) != 0) {
                // horiz and vertical flipping
                switch (rotation) {
                    case 0:
                        ret = Sprite.TRANS_ROT180;
                        break;
                    case ROTATE_90:
                        ret = Sprite.TRANS_ROT90;
                        break;
                    case ROTATE_180:
                        ret = Sprite.TRANS_NONE;
                        break;
                    case ROTATE_270:
                        ret = Sprite.TRANS_ROT270;
                        break;
                    default:
                }
            }
            else {
                // horizontal flipping
                switch (rotation) {
                    case 0:
                        ret = Sprite.TRANS_MIRROR_ROT180;
                        break;
                    case ROTATE_90:
                        ret = Sprite.TRANS_MIRROR_ROT90;
                        break;
                    case ROTATE_180:
                        ret = Sprite.TRANS_MIRROR;
                        break;
                    case ROTATE_270:
                        ret = Sprite.TRANS_MIRROR_ROT270;
                        break;
                    default:
                }
            }
        }
        else {
            if ((manipulation & FLIP_VERTICAL) != 0) {
                // vertical flipping
                switch (rotation) {
                    case 0:
                        ret = Sprite.TRANS_MIRROR;
                        break;
                    case ROTATE_90:
                        ret = Sprite.TRANS_MIRROR_ROT270;
                        break;
                    case ROTATE_180:
                        ret = Sprite.TRANS_MIRROR_ROT180;
                        break;
                    case ROTATE_270:
                        ret = Sprite.TRANS_MIRROR_ROT90;
                        break;
                    default:
                }
            }
            else {
                // no flipping
                switch (rotation) {
                    case 0:
                        ret = Sprite.TRANS_NONE;
                        break;
                    case ROTATE_90:
                        ret = Sprite.TRANS_ROT270;
                        break;
                    case ROTATE_180:
                        ret = Sprite.TRANS_ROT180;
                        break;
                    case ROTATE_270:
                        ret = Sprite.TRANS_ROT90;
                        break;
                    default:
                }
            }
        }
        return ret;
    }

    /**
     * Get Native transformation value out of given LCDUI manipulation
     *
     * @param manipulation LCDUI manipulation
     *
     * @return corresponding native transformation value
     */
    private static int getNativeTransformValue(int manipulation) {
        int returnVal = 0;
        switch (manipulation) {
            case 0:
                returnVal =  org.eclipse.swt.internal.qt.graphics.Image.TRANS_NONE;
                break;
            case DirectGraphics.FLIP_HORIZONTAL:
                returnVal =  org.eclipse.swt.internal.qt.graphics.Image.TRANS_FLIP_HORIZONTAL;
                break;
            case DirectGraphics.FLIP_VERTICAL:
                returnVal =  org.eclipse.swt.internal.qt.graphics.Image.TRANS_FLIP_VERTICAL;
                break;
            case DirectGraphics.ROTATE_90:
                returnVal =  org.eclipse.swt.internal.qt.graphics.Image.TRANS_ROT90;
                break;
            case DirectGraphics.ROTATE_180:
                returnVal =  org.eclipse.swt.internal.qt.graphics.Image.TRANS_ROT180;
                break;
            case DirectGraphics.ROTATE_270:
                returnVal =  org.eclipse.swt.internal.qt.graphics.Image.TRANS_ROT270;
                break;
            default:
                returnVal =  org.eclipse.swt.internal.qt.graphics.Image.TRANS_NONE;
        }
        return returnVal;
    }

    /**
     * Get Native format value out of given LCDUI format
     *
     * @param format LCDUI format
     *
     * @return corresponding native format value
     */
    private static int getNativeFormatValue(int format) {
        int returnVal = 0;
        switch (format) {
            case 0:
                returnVal = org.eclipse.swt.internal.qt.graphics.Image.FORMAT_IMG_NONE;
                break;
            case DirectGraphics.TYPE_BYTE_1_GRAY:
                returnVal = org.eclipse.swt.internal.qt.graphics.Image.FORMAT_IMG_MONO;
                break;
            case DirectGraphics.TYPE_INT_8888_ARGB:
                returnVal = org.eclipse.swt.internal.qt.graphics.Image.FORMAT_IMG_ARGB32;
                break;
            case DirectGraphics.TYPE_INT_888_RGB:
                returnVal = org.eclipse.swt.internal.qt.graphics.Image.FORMAT_IMG_RGB32;
                break;
            case DirectGraphics.TYPE_USHORT_4444_ARGB:
                returnVal = org.eclipse.swt.internal.qt.graphics.Image.FORMAT_IMG_RGB4444PREMULTIPLIED;
                break;
            case DirectGraphics.TYPE_USHORT_444_RGB:
                returnVal = org.eclipse.swt.internal.qt.graphics.Image.FORMAT_IMG_RGB444;
                break;
            case DirectGraphics.TYPE_USHORT_555_RGB:
                returnVal = org.eclipse.swt.internal.qt.graphics.Image.FORMAT_IMG_RGB555;
                break;
            case DirectGraphics.TYPE_USHORT_565_RGB:
                returnVal = org.eclipse.swt.internal.qt.graphics.Image.FORMAT_IMG_RGB16;
                break;
            default:
                break;
        }
        return returnVal;
    }

}
