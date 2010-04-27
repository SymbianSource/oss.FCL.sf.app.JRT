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
* Defines an interface for LCDUI custom components
*
*/


#ifndef MMIDCUSTOMCOMPONENT_H
#define MMIDCUSTOMCOMPONENT_H

// EXTERNAL INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CCoeControl;

/**
 * Interface for LCDUI custom components.
 *
 * This interface can be used to interact as a custom UI component
 * in LCDUI components that implement a container for custom
 * components.
 *
 * The client can use this interface to access information about
 * UI controls that the implementing class provides.
 *
 * @lib lcdui
 * @since S60 5.0
 */
class MMIDCustomComponent
{
public: // New methods

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
    virtual TInt CustomComponentControlCount() const = 0;

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
    virtual CCoeControl* CustomComponentControl(TInt aIndex) = 0;

    /**
     * Indicates that the container is about to be disposed.
     *
     * The custom component does not have to remove itself
     * from the container since the container removes all components
     * when disposing.
     *
     * @since S60 5.0
     */
    virtual void CustomComponentContainerDisposing() = 0;

    /**
     * Indicates if traversal can be done.
     *
     * Used when changing focus by key down and up.
     *
     * @param aEvent Key Event.
     * @since S60 5.0
     */
    virtual void TraverseL(const TKeyEvent& aEvent) = 0;

    /**
     * Processes the received pointer event.
     *
     * @param  aPointerEvent The pointer event. iPosition will be reset to the
     *         parent window position. iParentPosition will be invalid for the
     *         container.
     * @since S60 5.0
     */
    virtual void ProcessPointerEventL(const TPointerEvent& aPointerEvent) = 0;

    /**
     * Gets the current touch-enabled state
     *
     * @return true if the component is touch-enabled, false otherwise
     * @since S60 5.0
     */
    virtual TBool IsTouchEnabled() = 0;

    /**
     * Handles fullscreen state of parent Canvas.
     *
     * @since S60 5.0
     */
    virtual void HandleFullscreenModeChange() = 0;

    /**
     * Handles changes of screen orientation.
     *
     * @since S60 5.0
     */
    virtual void HandleResolutionChange() = 0;

protected: // Destructor

    /**
     * Destructor. Disallows destruction through this interface
     */
    virtual ~MMIDCustomComponent() {}
};

#endif // MMIDCUSTOMCOMPONENT_H

// End of file
