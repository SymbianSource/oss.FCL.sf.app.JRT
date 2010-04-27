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
* Description: Defines an interface for LCDUI Canvas Graphics Item component.
*
*/


#ifndef MMIDCANVASGRAPHICSITEMPAINTER_H
#define MMIDCANVASGRAPHICSITEMPAINTER_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <lcdui.h>


// CLASS DESCRIPTION
/**
 * Interface for the Text editor component.
 *
 * This interface can be used to interact as a custom UI component
 * in LCDUI components that implement a container for custom
 * components.
 *
 * The client can use this interface to access information about
 * UI controls that the implementing class provides.
 *
 * @since S60 5.0
 */
class MMIDCanvasGraphicsItemPainter : public MMIDComponent
{
public: // New methods
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    virtual TType Type() const
    {
        return ECanvasGraphicsItemPainter;
    }

    /**
     * Sets the position of this canvas graphics item component.
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    virtual void SetPosition(const TInt aX, const TInt aY) = 0;

    /**
     * Sets the size of this graphics item component.
     *
     * @param aWidth The width value of the anchor point.
     * @param aHeight The height value of the anchor point.
     * @since S60 5.0
     */
    virtual void SetItemSizeL(const TInt aWidth, const TInt aHeight) = 0;

    /**
     * Returns the backing bitmap onto which Graphics may draw.
     * Does not transfer ownership.
     * @returns A pointer to the bitmap.
     */
    virtual CFbsBitmap* FrameBuffer() const = 0;

    /**
     * Resets frame buffer bitmap before paint method is called.
     */
    virtual void ResetFrameBuffer(const TPoint& aPosition, const TSize& aSize) = 0;

protected: // Destructor

    /**
     * Destructor. Disallows destruction through this interface
     */
    virtual ~MMIDCanvasGraphicsItemPainter() {}
};

#endif // MMIDCANVASGRAPHICSITEMPAINTER_H

// End of file
