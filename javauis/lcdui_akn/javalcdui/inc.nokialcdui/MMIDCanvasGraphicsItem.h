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


#ifndef MMIDCANVASGRAPHICSITEM_H
#define MMIDCANVASGRAPHICSITEM_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <lcdui.h>

// FORWARD DECLARATIONS
class MMIDCustomComponentContainer;

// CLASS DESCRIPTION
/**
 * Interface for the Graphics item component.
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
class MMIDCanvasGraphicsItem : public MMIDComponent
{
public: // Type definitions

public: // New methods
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    virtual TType Type() const
    {
        return ECanvasGraphicsItem;
    }

    /**
     * Sets the size of this canvas graphics item.
     *
     * @param aWidth
     *          The width of the item in pixels.
     * @param aHeight
     *          The height of the item in pixels.
     * @since S60 5.0
     */
    virtual void SetSizeL(const TInt aWidth, const TInt aHeight) = 0;

    /**
     * Sets the parent of this graphics item component. The parent must
     * implement the custom component container interface in order to
     * provide necessary services for registering custom UI components.
     *
     * The ownership of the parent is not transffered to this object.
     *
     * @param aComponentContainer The parent MIDP custom component
     *        container.
     * @since S60 5.0
     */
    virtual void SetParentL(
        MMIDCustomComponentContainer* aComponentContainer) = 0;

    /**
     * Sets the direct container of this graphics item component.
     *
     * Note that the graphics item does not necessary need to have direct
     * container. If the parent component is responsible for somekind of
     * custom drawing using direct screena access, this method can be used
     * to register the direct container.
     *
     * The graphics item adds itself to the direct container so that it
     * is not able to draw on top of the graphics item when direct content
     * is added to the screen.
     *
     * @param aDirectContainer The direct container of this compoennt if
     *        any. <code>NULL</code> removes the current container.
     * @since S60 5.0
     */
    virtual void SetDirectContainerL(
        MDirectContainer* aDirectContainer) = 0;

    /**
     * Sets the elevation of this graphics item component.
     *
     * If the specified elevation exeeds to amount of items in the
     * custom component container, the item becomes the topmost item
     * in the custom component stack.
     *
     * Note that the elevation requsted may not be the actual elevation
     * of the item (if, for example, the elevation is higher than the
     * amount of all items). The actual elevation can be fetched using
     * <code>Elevation()</code>.
     *
     * @param aElevation The new elevation (Z-position) of this item.
     * @since S60 5.0
     */
    virtual void SetElevationL(TInt aElevation) = 0;

    /**
     * Returns the elevation of this graphics item component.
     *
     * @return The elevation of this graphics item component.
     * @since S60 5.0
     */
    virtual TInt Elevation() = 0;

    /**
     * Sets this graphics item component visible if it is hidden.
     *
     * Depending on the current status of the graphics item, this operation
     * is no-op if there is nothing to be done. (i.e. the graphics item is set
     * hidden when it is already hidden).
     *
     * @param aVisible Indicates the visibility status of the graphics item.
     * @since S60 5.0
     */
    virtual void SetVisibleL(TBool aVisible) = 0;

    /**
     * Sets the position of this graphics item component.
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    virtual void SetPosition(const TInt aX, const TInt aY) = 0;

protected: // Destructor

    /**
     * Destructor. Disallows destruction through this interface
     */
    virtual ~MMIDCanvasGraphicsItem() {}
};

#endif // MMIDCANVASGRAPHICSITEM_H

// End of file
