/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface between LCDUI and MMAPI
*
*/


#ifndef REFLCDUI_H
#define REFLCDUI_H

#include <w32std.h>

class MDirectContent;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;


/**
 * A receiver of asynchronous callbacks which are invoked
 * by MDirectContainer::MdcAsyncGetDSAResources()
 * and MDirectContainer:: MdcAsyncGetUICallback.
 * All the methods runs in UI thread.
 *
 * @since  S60 v5.0
 */
class MUiEventConsumer
{
public:
    /**
     * Callback to MDirectContainer::MdcAsyncGetDSAResources().
     * Provides resources to enable starting DirectScreenAccess.
     * All these resources have to be used from the thread where it was
     * created in!
     *
     * @since  S60 v5.0
     * @param "aWs" UI Window server session
     * @param "aScreenDevice" Screen device to draw DSA to
     * @param "aWindow" The window to draw directly
     */
    virtual void MdcDSAResourcesCallback(
        RWsSession &aWs,
        CWsScreenDevice &aScreenDevice,
        RWindowBase &aWindow) = 0;

    /**
     * Callback to MDirectContainer:: MdcAsyncGetUICallback.
     *
     * @since  S60 v5.0
     * @param "aCallbackId" Contains the value as specified
     * in callback invoker
     */
    virtual void MdcUICallback(const TInt aCallbackId) = 0;
};

/**
 * A container, such as a Canvas or Item, on which content can be displayed.
 */
class MDirectContainer
{
public:

    /**
     * Set the content.
     *
     * @param "aContent" Content which is displayed on this container.
     */
    virtual void MdcAddContent(MDirectContent* aContent) = 0;

    /**
     * Remove the content. This will called when the Player is closed.
     *
     * @param "aContent" Content which is displayed on this container.
     */
    virtual void MdcRemoveContent(MDirectContent* aContent) = 0;

    /**
     * Adds content bound to this container. Container will not allow painting in this region
     *
     * @param aRect Content bounds
     */
    virtual void MdcAddContentBounds(const TRect& aRect) = 0;

    /**
     * Removes one content bound from this container.
     * Container will now allow painting to this region
     *
     * @param aRect Content bound to be removed.
     */
    virtual void MdcRemoveContentBounds(const TRect& aRect) = 0;

    /**
     * Get a rect of a window (with respect to screen) on which content can be displayed.
     * If this changes, MdcContainerWindowRectChanged() should be called.
     *
     * @returns Rect of a window for content display. Returns empty rect if item
     * is not yet added to a form.
     */
    virtual TRect MdcContainerWindowRect() const = 0;

    /**
     * Get the visiblity of the container. The content must not be displayed
     * when it's container is invisible.
     * If this changes MdcContainerVisibilityChanged should be called.
     *
     * @returns "aVisible" ETrue if the container is visible, EFalse if it is invisible.
     */
    virtual TBool MdcContainerVisibility() const = 0;

    /**
     * Get the bounds of the area that the content can occupy.
     * The content should be clipped to this area.
     * If this changes MdcContentBoundsChanged should be called.
     *
     * @returns Maximum area that the content can occupy in screen co-ordinates.
     */
    virtual TRect MdcContentBounds() const = 0;

    /**
     * Get the area on the Item on which content can be displayed. The content
     * should draw to all of this area.
     * This should only be called when the content is displayed on an Item.
     * If this changes MdcItemContentRectChanged should be called.
     *
     * @param "aContentRect" Return parameter for content area in Item co-ordinates.
     * @param "aScreenRect" Return parameter for content area in screen co-ordinates.
     */
    virtual void MdcItemContentRect(TRect& aContentRect,TRect& aScreenRect) const = 0;

    /**
     * Allows a control created for content display to pass on pointer events.
     *
     * @param "aPointerEvent" The pointer event. iPosition will be reset to the
     * parent window position. iParentPosition will be invalid for the container.
     */
    virtual void MdcHandlePointerEventL(TPointerEvent& aPointerEvent) = 0;

    /**
     * Flush the container content
     *
     * @param "aRect" The content area to be proceeded.
     */
    virtual void MdcFlushContainer(const TRect& aRect) = 0;

    /**
     * Invokes callback aConsumer->MdcDSAResourcesCallback in UI thread.
     * Used in case when MMAPI needs to start DSA within UI thread.
     *
     * @since  S60 v5.0
     * @param "aConsumer" A consumer of callback
     */
    virtual void MdcGetDSAResources(MUiEventConsumer& aConsumer) = 0;

    /**
     * Invokes callback aConsumer->MdcUICallback in UI thread.
     * Used in case when MMAPI needs to run some operation within UI thread.
     *
     * @since  S60 v5.0
     * @param "aConsumer" A consumer of callback
     * @param "aCallbackId" A number identifying the callback
     */
    virtual void MdcGetUICallback(
        MUiEventConsumer& aConsumer,
        const TInt aCallbackId) = 0;

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Notifies container about added content in UI thread.
     *
     * @since  S60 v9.2
     */
    virtual void MdcNotifyContentAdded() {}
#endif
};

/**
 * Content, such as a video clip or camera view finder, which is displayed on a container.
 */
class MDirectContent
{
public:
    /**
     * Notify content that container window rect (Symbian RWindow)
     * is available.
     *
     * @param "aRect" New window Rect on which content can be displayed
     */
    virtual void MdcContainerWindowRectChanged(const TRect& aRect) = 0;

    /**
     * Notify content that container visiblity has changed. The content must not
     * be displayed when it's container is invisible.
     *
     * @param "aVisible" ETrue if the container is visible, EFalse if it is invisible.
     */
    virtual void MdcContainerVisibilityChanged(TBool aVisible) = 0;

    /**
     * Notify content that the container bounds have changed.
     *
     * @param "aRect" Maximum area the content can occupy in screen co-ordinates.
     * The content should be clipped to this area.
     */
    virtual void MdcContentBoundsChanged(const TRect& aRect) = 0;

    /**
     * Notify content that its display area has changed.
     * This should only be called when the content is displayed on an Item.
     *
     * @param "aDisplayRect" Display area in Item co-ordinates.
     * @param "aScreenRect" Display area in screen co-ordinates.
     */
    virtual void MdcItemContentRectChanged(const TRect& aContentRect,const TRect& aScreenRect) = 0;

    /**
     * Notify content that container is about to be destoryed.
     */
    virtual void MdcContainerDestroyed() = 0;

    /**
     * Notify content that any drawing
     * via direct screen access must be aborted
     */
    virtual void MdcAbortDSA() = 0;

    /**
     * Allow the content to draw
     * via direct screen access after MdcAbortDSA
     */
    virtual void MdcResumeDSA() = 0;
};

#endif // REFLCDUI_H

