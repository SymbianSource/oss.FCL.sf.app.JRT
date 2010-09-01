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
* Description:  CMIDCanvasGraphicsItem - Implements the native part Canvas Graphics Item class.
*
*/


#ifndef CMIDCANVASGRAPHICSITEM_H
#define CMIDCANVASGRAPHICSITEM_H

// EXTERNAL INCLUDES
#include <e32base.h>

// INTERNAL INCLUDES
#include <MMIDCanvasGraphicsItem.h>
#include <MMIDCustomComponent.h>
#include <MMIDScalable.h>

// FORWARD DECLARATIONS
class CMIDCanvasGraphicsItemPainter;
class CCoeControl;
class MMIDCustomComponentContainer;
class CMIDUtils;

// CLASS DESCRIPTION
/**
 * Defines the Canvas Graphics Item native side component.
 */
NONSHARABLE_CLASS(CMIDCanvasGraphicsItem) :
        public CBase,
        public MMIDCanvasGraphicsItem,
        public MMIDCustomComponent,
        public MMIDScalable
{
public: // Type definitions

    class TCtorParams
    {
    public:
        // The CanvasGraphicsItemPainter native side peer handle. Not owned.
        CMIDCanvasGraphicsItemPainter* iPainterHandle;
        // The shared CMIDUtils instance. Not owned.
        CMIDUtils* iUtils;

        TCtorParams::TCtorParams() :
                iPainterHandle(NULL),
                iUtils(NULL) {}
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @param aParams The construction parameters of this class.
     * @return New instance from this class
     */
    static CMIDCanvasGraphicsItem* NewL(const TCtorParams& aParams);

    /**
     * Destructor.
     */
    virtual ~CMIDCanvasGraphicsItem();

public: // From MMIDCustomComponent

    /**
     * Returns the count of controls in this custom component.
     *
     * Note that the count must not change during the lifetime
     * of the object. The count of all components is updated
     * when it is added to the custom component container.
     *
     * This is done in order to improve the performance when counting
     * countrols.
     *
     * @return The count of controls in this custom component.
     * @since S60 5.0
     */
    TInt CustomComponentControlCount() const;

    /**
     * Returns an UI control of this custom component.
     *
     * The index specifies which component is requested. The method
     * works similar to CCoeControl::ComponentControl() method. The
     * ownership of the control is NOT transferred to the caller.
     *
     * @param aIndex The index of the control to be retrieved.
     * @return The UI control of this custom component.
     * @since S60 5.0
     */
    CCoeControl* CustomComponentControl(TInt aIndex);

    /**
     * Indicates that the container is about to be disposed.
     *
     * The custom component does not have to remove itself
     * from the container since the container removes all components
     * when disposing.
     *
     * @since S60 5.0
     */
    void CustomComponentContainerDisposing();

    /**
     * Indicates if traversal is needed and does the traverse.
     *
     * Not used on CanvasGraphicsItem.
     *
     * @param aEvent Key Event.
     * @since S60 5.0
     */
    void TraverseL(const TKeyEvent& /* aEvent */) {}

    /**
     * Processes the received pointer event.
     *
     * @param  aPointerEvent The pointer event. iPosition will be reset to the
     *         parent window position. iParentPosition will be invalid for the
     *         container.
     * @since S60 5.0
     */
    void ProcessPointerEventL(const TPointerEvent& /*aPointerEvent*/) {}

    /**
     * Gets the current touch-enabled state
     *
     * @return false, this component is not touch-enabled
     * @since S60 5.0
     */
    TBool IsTouchEnabled()
    {
        return EFalse;
    }

    /**
     * Handles fullscreen state of parent Canvas.
     *
     * @since S60 5.0
     */
    void HandleFullscreenModeChange();

    /**
     * Handles system resource change messages.
     *
     * @param aType A message UID value.
     *
     * @since S60 5.0
     */
    void HandleResourceChange(TInt aType);

    /**
     * Handles switching from foreground to background and vice versa.
     *
     * @param aForeground Flag if it switches to foreground or to backgound.
     *
     * @since S60 5.0
     */
    void HandleForeground(TBool aForeground);

public: // From MMIDScalable

    TBool IsScalingOn() const;

public: // From MMIDCanvasGraphicsItem

    /**
     * Sets the parent of this canvas graphics item component. The parent must
     * implement the custom component container interface in order to
     * provide necessary services for registering custom UI components.
     *
     * The ownership of the parent is not transffered to this object.
     *
     * @param aComponentContainer The parent MIDP custom component
     *        container.
     * @since S60 5.0
     */
    void SetParentL(MMIDCustomComponentContainer* aComponentContainer);

    /**
     * Sets the direct container of this canvas graphics item component.
     *
     * Note that the canvas graphics item does not necessary need to have direct
     * container. If the parent component is responsible for somekind of
     * custom drawing using direct screena access, this method can be used
     * to register the direct container.
     *
     * The canvas graphics item adds itself to the direct container so that it
     * is not able to draw on top of the canvas graphics item when direct content
     * is added to the screen.
     *
     * @param aDirectContainer The direct container of this compoennt if
     *        any. <code>NULL</code> removes the current container.
     * @since S60 5.0
     */
    void SetDirectContainerL(MDirectContainer* aDirectContainer);

    /**
     * Sets the elevation of this canvas graphics item component.
     *
     * If the specified elevation exeeds to amount of items in the
     * custom component container, the item becomes the topmost item
     * in the custom component stack.
     *
     * Note that the elevation requsted may not be the actual elevation
     * of the item (if, for example, the elevation is higher than the
     * amount of all items, the elevation is set to the top).
     *
     * @param aElevation The new elevation (Z-position) of this item.
     * @since S60 5.0
     */
    void SetElevationL(TInt aElevation);

    /**
     * Returns the elevation of this canvas graphics item component.
     *
     * @return The elevation of this canvas graphics item component.
     * @since S60 5.0
     */
    TInt Elevation();

    /**
     * Sets this canvas graphics item component visible if it is hidden.
     *
     * Depending on the current status of the canvas graphics item, this operation
     * is no-op if there is nothing to be done. (i.e. the canvas graphics item is set
     * hidden when it is already hidden).
     *
     * @param aVisible Indicates the visibility status of the canvas graphics item.
     * @since S60 5.0
     */
    void SetVisibleL(TBool aVisible);

    /**
     * Sets the size of this canvas graphics item component.
     *
     * @param aWidth The width of the item.
     * @param aHeight The height of the item.
     * @since S60 5.0
     */
    void SetSizeL(const TInt aWidth, const TInt aHeight);

    /**
     * Sets the position of this canvas graphics item component.
     *
     * Note: Calling this function results in a call to
     *          CMIDCanvasGraphicsItemEdwin::PositionChanged().
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    void SetPosition(const TInt aX, const TInt aY);

public: // From MMIDComponent

    /**
     * Invoked by the framework to clean up any  resources used by the
     * implementation.
     *
     * Typically, the implementation will do "delete this" but is clearly
     * not restricted to
     *
     * @since S60 5.0
     */
    void Dispose();
public: // Own methods

    /**
     * Removes this component from component container 
     * when its painter being disposed first (during finalization)
     */
    void CMIDCanvasGraphicsItem::DeregisterCanvasGraphicsItem();


private: // own methods
    /**
     * This method is called when some change which can affect scaling
     * of CanvasGraphicsItem.
     *
     * @param aChange Indicator what change was occurred.
     * @since S60 5.0
     */
    void HandleChangeForScaling(TChange aChange);

private: // Construtors

    /**
     * C++ constructor.
     */
    CMIDCanvasGraphicsItem();

    /**
     * Second phase constructor
     *
     * @param aParams The construction parameters of this class.
     */
    void ConstructL(const TCtorParams& aParams);

private: // Data

    // canvas graphics item control. Not owned.
    CMIDCanvasGraphicsItemPainter* iItemPainter;

    // Container for this object. Not owned.
    MMIDCustomComponentContainer* iComponentContainer;

    // The shared CMIDUtils instance. Not owned.
    CMIDUtils* iUtils;

    // A non-scaled size of item. Needed by scaling.
    TSize iNonScaledSize;

    // A non-scaled position of item. Needed by scaling.
    TPoint iNonScaledPosition;

#ifdef RD_JAVA_S60_RELEASE_9_2
    // Indicates state of partial VKB
    TBool iPartialVKBOpen;
#endif
};

#endif // CMIDCANVASGRAPHICSITEM_H

// End of file
