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
* Defines an interface for an LCDUI custom component container.
*
*/


#ifndef MMIDCUSTOMCOMPONENTCONTAINER_H
#define MMIDCUSTOMCOMPONENTCONTAINER_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <w32std.h>

// FORWARD DECLARATIONS
class MMIDCustomComponent;
class CCoeControl;
#ifdef RD_JAVA_S60_RELEASE_9_2
class CMIDDisplayable;
#endif // RD_JAVA_S60_RELEASE_9_2


/**
 * Interface for LCDUI custom component container.
 *
 * This interface can be used to provide services for storing and handling
 * LCDUI custom UI components.
 *
 * @lib lcdui
 * @since S60 5.0
 */
class MMIDCustomComponentContainer
{
public: // New methods

    /**
     * Registers a new custom component to this container.
     *
     * The ownership of the component is NOT transfferred. Nothing happens
     * if the component has already been registered. The component is
     * added on top of the stack in this container. Index can be changed
     * later.
     *
     * @param aComponent The component to be registered.
     * @since S60 5.0
     */
    virtual void RegisterComponentL(
        MMIDCustomComponent* aComponent) = 0;

    /**
     * Unregisters an existing custom component from this container.
     *
     * Nothing happens if the component is not found from this container.
     *
     * @param aComponent The component to be unregistered.
     * @since S60 5.0
     */
    virtual void UnregisterComponent(
        MMIDCustomComponent* aComponent) = 0;

    /**
     * Changes the index of the specified custom component.
     *
     * @param aComponent The component which index will be changed
     * @param aNewIndex The new index of the component.
     */
    virtual void SetComponentIndexL(
        MMIDCustomComponent* aComponent,
        TInt aNewIndex) = 0;

    /**
     * Stores the index of focused component in parent.
     *
     * @param aComponent The component which gained focus
     * @since S60 5.0
     */
    virtual void SetFocusedComponent(
        MMIDCustomComponent* aComponent) = 0;

    /**
     * Returns pointer to focused component.
     *
     * @return Pointer to focused component.
     */
    virtual MMIDCustomComponent* GetFocusedComponent() const = 0;

    /**
     * Returns the index of the given component in this container.
     *
     * @return The index of the given component in this container.
     *         KErrNotFound is returned if this container does not have
     *         the specified component.
     * @since S60 5.0
     */
    virtual TInt ComponentIndex(
        MMIDCustomComponent* aComponent) const = 0;

    /**
     * Returns the control of this component container or NULL of one does
     * not exist.
     *
     * @return The control of this container.
     * @since S60 5.0
     */
    virtual CCoeControl& Control() = 0;

    /**
     * Gets the full screen status of this component container.
     *
     * Typically the container is the same object which is returned when
     * calling <code>Control()</code>.
     *
     * @return <code>ETrue</code> if this component container is set to
     *         full screen and <code>EFalse</code> if not.
     * @since S60 5.0
     */
    virtual TBool IsFullScreen() const = 0;

    /**
     * Changes focused component to argument (now only for TextEditor).
     *
     * @param Pointer to new focused TextEditor.
     */
    virtual void ChangeFocusStateOfTextEditorsL(TBool aFocusState, MMIDCustomComponent* aTextEditor) = 0;

#ifdef RD_JAVA_S60_RELEASE_9_2
    /**
     * Returns flag information, if partial keyboard is open.
     *
     * @return True if Partial VKB is open. False otherwise.
     */
    virtual TBool IsPartialVKBOpen() const = 0;

    /**
     * Gets the Displayable from container.
     *
     * @return The Displayable from container.
     * @since S60 5.0
     */
    virtual CMIDDisplayable* GetDisplayable() const = 0;
#endif // RD_JAVA_S60_RELEASE_9_2

protected: // Destructor

    /**
     * Destructor. Disallows destruction through this interface
     */
    virtual ~MMIDCustomComponentContainer() {}
};

#endif // MMIDCUSTOMCOMPONENTCONTAINER_H

// End of file
