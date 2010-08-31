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
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QSharedDataPointer>
#include <QRect>
#include <QImage>
#include "gfxutils.h"

namespace Java { namespace GFX {

//
// CollisionDetection
//

#define HANDLE_TO_POINTER(type, variable, handle) type variable = reinterpret_cast<type>( handle )

/*static*/ bool gfxUtils::detectCollision(int aImage1PixmapHandle, int aTransform1, int aP1x, int aP1y, int aR1x1, int aR1y1, int aR1x2, int aR1y2,
                                          int aImage2PixmapHandle, int aTransform2, int aP2x, int aP2y, int aR2x1, int aR2y1, int aR2x2, int aR2y2)
{
    // Calculate the intersection of collisionRectangles:
    QRect rect1(aP1x, aP1y, aR1x2 - aR1x1, aR1y2 - aR1y1);
    QRect rect2(aP2x, aP2y, aR2x2 - aR2x1, aR2y2 - aR2y1);
    QRect rect3 = rect1.intersected(rect2);

    // Save intersection to collisionRectangles:
    aR1x1 += rect3.x() - aP1x;
    aR1y1 += rect3.y() - aP1y;
    aR1x2 = aR1x1 + rect3.width();
    aR1y2 = aR1y1 + rect3.height();
    aR2x1 += rect3.x() - aP2x;
    aR2y1 += rect3.y() - aP2y;
    aR2x2 = aR2x1 + rect3.width();
    aR2y2 = aR2y1 + rect3.height();

    // Get image's pixel data:
    HANDLE_TO_POINTER(QPixmap*, image1, aImage1PixmapHandle);
    QImage qimage1 = image1->toImage();
    const unsigned char* data1 = qimage1.bits();
    int totalWidth1 = qimage1.width();

    HANDLE_TO_POINTER(QPixmap*, image2, aImage2PixmapHandle);
    QImage qimage2 = image2->toImage();
    const unsigned char* data2 = qimage2.bits();
    int totalWidth2 = qimage2.width();

    // These rectangles defines areas to be checked.
    QRect image1Rect(aR1x1, aR1y1, aR1x2 - aR1x1, aR1y2 - aR1y1);
    QRect image2Rect(aR2x1, aR2y1, aR2x2 - aR2x1, aR2y2 - aR2y1);

    // Following variables define how much we need to increase or
    // decrease x and y in every loop round:
    int rect1XInc = 0;
    int rect1YInc = 0;
    int rect2XInc = 0;
    int rect2YInc = 0;

    // These defines how much we need to increment/decrement x and y
    // when moving to next line:
    int rect1XLineInc = 0;
    int rect1YLineInc = 0;
    int rect2XLineInc = 0;
    int rect2YLineInc = 0;

    // These variables contains the coordinates of the pixel we are
    // checking:
    int rect1X = 0;
    int rect1Y = 0;
    int rect2X = 0;
    int rect2Y = 0;

    // Tells where to start:
    int rect1XStart = 0;
    int rect1YStart = 0;
    int rect2XStart = 0;
    int rect2YStart = 0;

    // Position of last pixel.
    int rect1XEnd = 0;
    int rect1YEnd = 0;
    int rect2XEnd = 0;
    int rect2YEnd = 0;

    switch(aTransform1) {
        case 0: // TRANS_NONE:
            rect1XInc = 1;
            rect1YInc = 0;
            rect1XLineInc = 0;
            rect1YLineInc = 1;
            rect1XStart = image1Rect.x();
            rect1YStart = image1Rect.y();
            rect1XEnd = image1Rect.x() + image1Rect.width() - 1;
            rect1YEnd = image1Rect.y() + image1Rect.height() - 1;
            break;
        case 5: // TRANS_ROT90:
            rect1XInc = 0;
            rect1YInc = 1;
            rect1XLineInc = -1;
            rect1YLineInc = 0;
            rect1XStart = image1Rect.x() + image1Rect.width() -1;
            rect1YStart = image1Rect.y();
            rect1XEnd = image1Rect.x();
            rect1YEnd = image1Rect.y() + image1Rect.width() - 1;
            break;
        case 3: // TRANS_ROT180:
            rect1XInc = -1;
            rect1YInc = 0;
            rect1XLineInc = 0;
            rect1YLineInc = -1;
            rect1XStart = image1Rect.x() + image1Rect.width() -1;
            rect1YStart = image1Rect.y() + image1Rect.height() - 1;
            rect1XEnd = image1Rect.x();
            rect1YEnd = image1Rect.y();
            break;
        case 6: // TRANS_ROT270:
            rect1XInc = 0;
            rect1YInc = -1;
            rect1XLineInc = 1;
            rect1YLineInc = 0;
            rect1XStart = image1Rect.x();
            rect1YStart = image1Rect.y() + image1Rect.height() - 1;
            rect1XEnd = image1Rect.x() + image1Rect.height() - 1;
            rect1YEnd = image1Rect.y();
            break;
        case 2: // TRANS_MIRROR:
            rect1XInc = -1;
            rect1YInc = 0;
            rect1XLineInc = 0;
            rect1YLineInc = 1;
            rect1XStart = image1Rect.x() + image1Rect.width() - 1;
            rect1YStart = image1Rect.y();
            rect1XEnd = image1Rect.x();
            rect1YEnd = image1Rect.y() + image1Rect.height() - 1;
            break;
        case 7: // TRANS_MIRROR_ROT90:
            rect1XInc = 0;
            rect1YInc = -1;
            rect1XLineInc = -1;
            rect1YLineInc = 0;
            rect1XStart = image1Rect.x() + image1Rect.width() -1;
            rect1YStart = image1Rect.y() + image1Rect.height() - 1;
            rect1XEnd = image1Rect.x();
            rect1YEnd = image1Rect.y();
            break;
        case 1: // TRANS_MIRROR_ROT180:
            rect1XInc = 1;
            rect1YInc = 0;
            rect1XLineInc = 0;
            rect1YLineInc = -1;
            rect1XStart = image1Rect.x();
            rect1YStart = image1Rect.y() + image1Rect.height() - 1;
            rect1XEnd = image1Rect.x() + image1Rect.width() - 1;
            rect1YEnd = image1Rect.y();
            break;
        case 4: // TRANS_MIRROR_ROT270:
            rect1XInc = 0;
            rect1YInc = 1;
            rect1XLineInc = 1;
            rect1YLineInc = 0;
            rect1XStart = image1Rect.x();
            rect1YStart = image1Rect.y();
            rect1XEnd = image1Rect.x() + image1Rect.height() - 1;
            rect1YEnd = image1Rect.y() + image1Rect.width() - 1;
            break;
        default:
            break;
    }

    switch(aTransform2) {
        case 0: // TRANS_NONE:
            rect2XInc = 1;
            rect2YInc = 0;
            rect2XLineInc = 0;
            rect2YLineInc = 1;
            rect2XStart = image2Rect.x();
            rect2YStart = image2Rect.y();
            rect2XEnd = image2Rect.x() + image2Rect.width() - 1;
            rect2YEnd = image2Rect.y() + image2Rect.height() - 1;
            break;
        case 5: // TRANS_ROT90:
            rect2XInc = 0;
            rect2YInc = 1;
            rect2XLineInc = -1;
            rect2YLineInc = 0;
            rect2XStart = image2Rect.x() + image2Rect.width() -1;
            rect2YStart = image2Rect.y();
            rect2XEnd = image2Rect.x();
            rect2YEnd = image2Rect.y() + image2Rect.width() - 1;
            break;
        case 3: // TRANS_ROT180:
            rect2XInc = -1;
            rect2YInc = 0;
            rect2XLineInc = 0;
            rect2YLineInc = -1;
            rect2XStart = image2Rect.x() + image2Rect.width() -1;
            rect2YStart = image2Rect.y() + image2Rect.height() - 1;
            rect2XEnd = image2Rect.x();
            rect2YEnd = image2Rect.y();
            break;
        case 6: // TRANS_ROT270:
            rect2XInc = 0;
            rect2YInc = -1;
            rect2XLineInc = 1;
            rect2YLineInc = 0;
            rect2XStart = image2Rect.x();
            rect2YStart = image2Rect.y() + image2Rect.height() - 1;
            rect2XEnd = image2Rect.x() + image2Rect.height() - 1;
            rect2YEnd = image2Rect.y();
            break;
        case 2: // TRANS_MIRROR:
            rect2XInc = -1;
            rect2YInc = 0;
            rect2XLineInc = 0;
            rect2YLineInc = 1;
            rect2XStart = image2Rect.x() + image2Rect.width() - 1;
            rect2YStart = image2Rect.y();
            rect2XEnd = image2Rect.x();
            rect2YEnd = image2Rect.y() + image2Rect.height() - 1;
            break;
        case 7: // TRANS_MIRROR_ROT90:
            rect2XInc = 0;
            rect2YInc = -1;
            rect2XLineInc = -1;
            rect2YLineInc = 0;
            rect2XStart = image2Rect.x() + image2Rect.width() -1;
            rect2YStart = image2Rect.y() + image2Rect.height() - 1;
            rect2XEnd = image2Rect.x();
            rect2YEnd = image2Rect.y();
            break;
        case 1: // TRANS_MIRROR_ROT180:
            rect2XInc = 1;
            rect2YInc = 0;
            rect2XLineInc = 0;
            rect2YLineInc = -1;
            rect2XStart = image2Rect.x();
            rect2YStart = image2Rect.y() + image2Rect.height() - 1;
            rect2XEnd = image2Rect.x() + image2Rect.width() - 1;
            rect2YEnd = image2Rect.y();
            break;
        case 4: // TRANS_MIRROR_ROT270:
            rect2XInc = 0;
            rect2YInc = 1;
            rect2XLineInc = 1;
            rect2YLineInc = 0;
            rect2XStart = image2Rect.x();
            rect2YStart = image2Rect.y();
            rect2XEnd = image2Rect.x() + image2Rect.height() - 1;
            rect2YEnd = image2Rect.y() + image2Rect.width() - 1;
            break;
        default:
            break;
    }


    rect1X = rect1XStart;
    rect1Y = rect1YStart;
    rect2X = rect2XStart;
    rect2Y = rect2YStart;

    // Go through the intersection area pixel by pixel.
    // Following code assumes that format is 32-bit RGBA or 32-bit RGB.
    while(true) {
        // Check is there hit:
        if(data1[rect1Y * totalWidth1 * 4 + rect1X * 4 + 3]) {
            if(data2[rect2Y * totalWidth2 * 4 + rect2X * 4 + 3]) {
                return true;
            }
        }

        if((rect1X == rect1XEnd) && (rect1Y == rect1YEnd)) {
            // Done, no hit:
            return false;
        }

        // Move to next line if in the end:
        if(rect1XLineInc == 0) {
            if(rect1X == rect1XEnd) {
                rect1X = rect1XStart - rect1XInc;
                rect1Y += rect1YLineInc;
            }
        }
        else {
            if(rect1Y == rect1YEnd) {
                rect1Y = rect1YStart - rect1YInc;
                rect1X += rect1XLineInc;
            }
        }

        if(rect2XLineInc == 0) {
            if(rect2X == rect2XEnd) {
                rect2X = rect2XStart - rect2XInc;
                rect2Y += rect2YLineInc;
            }
        }
        else {
            if(rect2Y == rect2YEnd) {
                rect2Y = rect2YStart - rect2YInc;
                rect2X += rect2XLineInc;
            }
        }

        // Move to next pixel:
        rect1X += rect1XInc;
        rect1Y += rect1YInc;
        rect2X += rect2XInc;
        rect2Y += rect2YInc;
    }
}

} // namespace GFX
} // namespace Java

