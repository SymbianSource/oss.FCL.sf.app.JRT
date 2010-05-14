/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MMMACONTAINER_H
#define MMMACONTAINER_H

class MMMADirectContent;
class MUiEventConsumer;

/**
A container on which content can be displayed.
Content can be drawn to container using direct screen access or frame buffer
mode.
*/
class MMMAContainer
{
public:
    /**
    Remove the content. This will called when the Player is closed.
    */
    virtual void MdcRemoveContent() = 0;

    /**
    Get the visiblity of the container. The content must not be displayed
    when it's container is invisible.
    If this changes MdcContainerVisibilityChanged should be called.
    @returns "aVisible" ETrue if the container is visible, EFalse if it is invisible.
    */
    virtual TBool MdcContainerVisibility() const = 0;

    /**
    Repaint frame buffer to screen.
    */
    virtual void MdcRepaint() const = 0;

    /**
    Get the bounds of the area that the content can occupy.
    The content should be clipped to this area.
    @param aContentRect Maximum area the content can occupy in screen co-ordinates.
    @param aParentRect Visible part of the parent in screen co-ordinates.
    */
    virtual void MdcGetContentRect(TRect& aControlRect,
                                   TRect& aParentRect) const = 0;

    /**
    This method is called when content knows its preferred size. This method
    will be called for example when video content obtains its size. Container
    must inform new content rect using content's MdcContentBoundsChanged method.
    @param aPreferredSize preferred size for the content.
    */
    virtual void MdcInvalidate(const TSize& aPreferredSize) = 0;

    /**
    Set the size of the render region for the video clip to be fullscreen.
    After this call MESWTDirectContent must be informed about new draw area.
    @param aFullScreen Indicates whether or not to render in full screen mode
    */
    virtual void MdcSetDisplayFullScreen(TBool aFullScreen) = 0;

    /**
     * Invokes callback aConsumer->MdcDSAResourcesCallback in UI thread.
     * Used in case when MMAPI needs to start DSA within UI thread.
     *
     * @since  S60 v5.0
     * @param "aConsumer" A consumer of callback
     * @param "aIsInUiThread" Notifies that the method runs in UI thread
     */
    virtual void MdcGetDSAResources(
        MUiEventConsumer& aConsumer,
        TBool aIsInUiThread) = 0;

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

    /**
    * Get the bounds of the content window.
    *
    * @returns The rect of the content window in screen co-ordinates.
    */
    virtual TRect MdcContainerWindowRect() const = 0;
};


#endif // MMMACONTAINER_H
