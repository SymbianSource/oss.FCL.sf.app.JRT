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
#include "graphicscontextimpl.h"

namespace Java { namespace GFX {

//
// CollisionDetection
//

#define HANDLE_TO_POINTER(type, variable, handle) type variable = reinterpret_cast<type>( handle )

bool gfxUtils::detectCollision(Image* aImage1, int aTransform1, int aP1x, int aP1y, int aR1x1, int aR1y1, int aR1x2, int aR1y2,
                               Image* aImage2, int aTransform2, int aP2x, int aP2y, int aR2x1, int aR2y1, int aR2x2, int aR2y2)
{
    int transform1 = getCgTransformValue(aTransform1);
    int transform2 = getCgTransformValue(aTransform2);

    // Calculate the intersection of collisionRectangles:
    QRect rect1(aP1x, aP1y, aR1x2 - aR1x1, aR1y2 - aR1y1);
    QRect rect2(aP2x, aP2y, aR2x2 - aR2x1, aR2y2 - aR2y1);

    if (aTransform1 & EReflectDiag)
    {
        int tmp = rect1.width();
        rect1.setWidth(rect1.height());
        rect1.setHeight(tmp);
    }
    if (aTransform2 & EReflectDiag)
    {
        int tmp = rect2.width();
        rect2.setWidth(rect2.height());
        rect2.setHeight(tmp);
    }

    QRect rect3 = rect1.intersected(rect2);

    QImage qimage1;

    if(transform1 == ETransRot90)
    {
        GraphicsContextImpl gc;
        // create a copy of the image and transform it, i.e. don't modify the original
        qimage1 = gc.doTransform(aImage1->toImage(), transform1);
    }
    else
    {
        qimage1 = aImage1->toImage();
    }
    int totalWidth1 = qimage1.width();

    QImage qimage2;

    if(transform2 == ETransRot90)
    {
        GraphicsContextImpl gc;
        // create a copy of the image and transform it, i.e. don't modify the original
        qimage2 = gc.doTransform(aImage2->toImage(), transform2);
    }
    else
    {
        qimage2 = aImage2->toImage();
    }
    int totalWidth2 = qimage2.width();	
        
    const unsigned char* data1 = qimage1.bits();
    const unsigned char* data2 = qimage2.bits();

    // Save intersection to collisionRectangles:
    aR1x1 += rect3.x() - aP1x;
    aR1y1 += rect3.y() - aP1y;
    aR1x2 = aR1x1 + rect3.width();
    aR1y2 = aR1y1 + rect3.height();
    aR2x1 += rect3.x() - aP2x;
    aR2y1 += rect3.y() - aP2y;
    aR2x2 = aR2x1 + rect3.width();
    aR2y2 = aR2y1 + rect3.height();

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

    switch(transform1) 
    {
        case ETransNone:
            rect1XInc = 1;
            rect1YInc = 0;
            rect1XLineInc = 0;
            rect1YLineInc = 1;
            rect1XStart = image1Rect.x();
            rect1YStart = image1Rect.y();
            rect1XEnd = image1Rect.x() + image1Rect.width() - 1;
            rect1YEnd = image1Rect.y() + image1Rect.height() - 1;
            break;
        case ETransRot90:
            rect1XInc = 0;
            rect1YInc = 1;
            rect1XLineInc = -1;
            rect1YLineInc = 0;
            rect1XStart = image1Rect.x() + image1Rect.width() -1;
            rect1YStart = image1Rect.y();
            rect1XEnd = image1Rect.x();
            rect1YEnd = image1Rect.y() + image1Rect.height() - 1;
            break;
        case ETransRot180:
            rect1XInc = -1;
            rect1YInc = 0;
            rect1XLineInc = 0;
            rect1YLineInc = -1;
            rect1XStart = image1Rect.x() + image1Rect.width() -1;
            rect1YStart = image1Rect.y() + image1Rect.height() - 1;
            rect1XEnd = image1Rect.x();
            rect1YEnd = image1Rect.y();
            break;
        case ETransRot270:
            rect1XInc = 0;
            rect1YInc = -1;
            rect1XLineInc = 1;
            rect1YLineInc = 0;
            rect1XStart = image1Rect.x();
            rect1YStart = image1Rect.y() + image1Rect.height() - 1;
            rect1XEnd = image1Rect.x() + image1Rect.height() - 1;
            rect1YEnd = image1Rect.y();
            break;
        case ETransMirror:
            rect1XInc = -1;
            rect1YInc = 0;
            rect1XLineInc = 0;
            rect1YLineInc = 1;
            rect1XStart = image1Rect.x() + image1Rect.width() - 1;
            rect1YStart = image1Rect.y();
            rect1XEnd = image1Rect.x();
            rect1YEnd = image1Rect.y() + image1Rect.height() - 1;
            break;
        case ETransMirrorRot90:
            rect1XInc = 0;
            rect1YInc = -1;
            rect1XLineInc = -1;
            rect1YLineInc = 0;
            rect1XStart = image1Rect.x() + image1Rect.width() -1;
            rect1YStart = image1Rect.y() + image1Rect.height() - 1;
            rect1XEnd = image1Rect.x();
            rect1YEnd = image1Rect.y();
            break;
        case ETransMirrorRot180:
            rect1XInc = 1;
            rect1YInc = 0;
            rect1XLineInc = 0;
            rect1YLineInc = -1;
            rect1XStart = image1Rect.x();
            rect1YStart = image1Rect.y() + image1Rect.height() - 1;
            rect1XEnd = image1Rect.x() + image1Rect.width() - 1;
            rect1YEnd = image1Rect.y();
            break;
        case ETransMirrorRot270:
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

    switch(transform2) 
    {
        case ETransNone:
            rect2XInc = 1;
            rect2YInc = 0;
            rect2XLineInc = 0;
            rect2YLineInc = 1;
            rect2XStart = image2Rect.x();
            rect2YStart = image2Rect.y();
            rect2XEnd = image2Rect.x() + image2Rect.width() - 1;
            rect2YEnd = image2Rect.y() + image2Rect.height() - 1;
            break;
        case ETransRot90:
            rect2XInc = 0;
            rect2YInc = 1;
            rect2XLineInc = -1;
            rect2YLineInc = 0;
            rect2XStart = image2Rect.x() + image2Rect.width() -1;
            rect2YStart = image2Rect.y();
            rect2XEnd = image2Rect.x();
            rect2YEnd = image2Rect.y() + image2Rect.height() - 1;
            break;
        case ETransRot180:
            rect2XInc = -1;
            rect2YInc = 0;
            rect2XLineInc = 0;
            rect2YLineInc = -1;
            rect2XStart = image2Rect.x() + image2Rect.width() -1;
            rect2YStart = image2Rect.y() + image2Rect.height() - 1;
            rect2XEnd = image2Rect.x();
            rect2YEnd = image2Rect.y();
            break;
        case ETransRot270:
            rect2XInc = 0;
            rect2YInc = -1;
            rect2XLineInc = 1;
            rect2YLineInc = 0;
            rect2XStart = image2Rect.x();
            rect2YStart = image2Rect.y() + image2Rect.height() - 1;
            rect2XEnd = image2Rect.x() + image2Rect.height() - 1;
            rect2YEnd = image2Rect.y();
            break;
        case ETransMirror:
            rect2XInc = -1;
            rect2YInc = 0;
            rect2XLineInc = 0;
            rect2YLineInc = 1;
            rect2XStart = image2Rect.x() + image2Rect.width() - 1;
            rect2YStart = image2Rect.y();
            rect2XEnd = image2Rect.x();
            rect2YEnd = image2Rect.y() + image2Rect.height() - 1;
            break;
        case ETransMirrorRot90:
            rect2XInc = 0;
            rect2YInc = -1;
            rect2XLineInc = -1;
            rect2YLineInc = 0;
            rect2XStart = image2Rect.x() + image2Rect.width() -1;
            rect2YStart = image2Rect.y() + image2Rect.height() - 1;
            rect2XEnd = image2Rect.x();
            rect2YEnd = image2Rect.y();
            break;
        case ETransMirrorRot180:
            rect2XInc = 1;
            rect2YInc = 0;
            rect2XLineInc = 0;
            rect2YLineInc = -1;
            rect2XStart = image2Rect.x();
            rect2YStart = image2Rect.y() + image2Rect.height() - 1;
            rect2XEnd = image2Rect.x() + image2Rect.width() - 1;
            rect2YEnd = image2Rect.y();
            break;
        case ETransMirrorRot270:
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
    while(true) 
    {
        // Check is there hit:
        if(data1[rect1Y * totalWidth1 * 4 + rect1X * 4 + 3])
        {
            if(data2[rect2Y * totalWidth2 * 4 + rect2X * 4 + 3]) 
            {
                return true;
            }
        }

        if((rect1X == rect1XEnd) && (rect1Y == rect1YEnd))
        {
            // Done, no hit:
            return false;
        }

        // Move to next line if in the end:
        if(rect1XLineInc == 0)
        {
            if(rect1X == rect1XEnd)
            {
                rect1X = rect1XStart - rect1XInc;
                rect1Y += rect1YLineInc;
            }
        }
        else
        {
            if(rect1Y == rect1YEnd)
            {
                rect1Y = rect1YStart - rect1YInc;
                rect1X += rect1XLineInc;
            }
        }

        if(rect2XLineInc == 0)
        {
            if(rect2X == rect2XEnd)
            {
                rect2X = rect2XStart - rect2XInc;
                rect2Y += rect2YLineInc;
            }
        }
        else
        {
            if(rect2Y == rect2YEnd)
            {
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

    /**
     *  Maps transform constants to Common Graphics.
     */
	int gfxUtils::getCgTransformValue(int aTransform)
    {
        int retVal = ETransNone;
        switch(aTransform)
        {
            case ETransNoneType:
                retVal = ETransNone;
                break;
            case ETransRot90Type:
                retVal = ETransRot90;
                break;
            case ETransRot180Type:
                retVal = ETransRot180;
                break;
            case ETransRot270Type:
                retVal = ETransRot270;
                break;
            case ETransMirrorType:
                retVal = ETransMirror;
                break;
            case ETransMirrorRot90Type:
                retVal = ETransMirrorRot90;
                break;
            case ETransMirrorRot180Type:
                retVal = ETransMirrorRot180;
                break;
            case ETransMirrorRot270Type:
                retVal = ETransMirrorRot270;
                break;
            default:
                break;
        }
        return retVal;
    }
} // namespace GFX
} // namespace Java

